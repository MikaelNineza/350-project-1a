#include "GameEngine.h"
#include "DrawContext.h"
#include "GameContext.h"
#include "GameObject.h"
#include "GraphicsObject.h"
#include "CollisionObject.h"

namespace CMPUT350 {
    #include "FontData.h"

const uint32_t FPS_LIMIT = 30;
bool isObjectDead(const std::shared_ptr<GameObject> obj);

/**
 * @brief Creates the game window and loads the engine's font.
 *
 * @param width The width of the window, in pixels.
 * @param height The height of the window, in pixels.
 * @param name The title shown in the window's title bar.
 *
 * The window is limited to FPS_LIMIT (30) frames per second and key repeat is enabled, so
 * holding a key down produces repeated key-down events. The font is loaded from the
 * data in FontData.h; if loading fails, a warning is printed to stderr and the engine
 * continues without a usable font.
 */
GameEngine::GameEngine(unsigned int width, unsigned int height, const std::string& name) : mWindow(std::make_shared<sf::RenderWindow>()) {
    mWindow->create(sf::VideoMode({width, height}), name);
    mWindow->setFramerateLimit(FPS_LIMIT);
    mWindow->setKeyRepeatEnabled(true);
    mFont = std::make_shared<sf::Font>();

    if (!mFont->openFromMemory(&_font, _font_len))
    {
        fprintf(stderr, "WARNING: Font did not load.\n");
    }
}

/**
 * @brief Closes the game window.
 *
 * The window is normally already closed by a close event in ProcessEvents; this is a
 * failsafe in case Run() exits some other way.
 */
GameEngine::~GameEngine() {
    mWindow->close();
}

/**
 * @brief Queues a game object to be added to the engine at the start of the next frame.
 *
 * @param gameObject The object to add. The engine keeps a shared pointer to it until the
 *                   object reports that it is no longer alive.
 *
 * The object is not active until Run() moves it into the main object list and calls its
 * Initialize() method, so it receives no updates, events, collisions or render calls in
 * the frame it was added.
 */
void GameEngine::AddGameObject(std::shared_ptr<GameObject> gameObject) {
    mNewObjects.push_back(std::move(gameObject));
}

/**
 * @brief Detects collisions between every pair of collision objects and notifies both objects.
 *
 * @param drawContext A drawing context used only for its coordinate transforms. Its offset
 *                    and rotation are overwritten, so callers must set them again before drawing.
 *
 * Works in two phases:
 *   1. Broad phase: each object's local bounds from GetBounds() are transformed to world
 *      space (as an axis-aligned box around the rotated bounds). Pairs whose world boxes
 *      do not overlap are skipped.
 *   2. Narrow phase: every shape of one object is tested against every shape of the other.
 *      Circles and lines are transformed to world space and tested there. Rects are left in
 *      their owner's local space so that a rotated rect stays a rect; the other shape is
 *      transformed into that local space instead.
 *
 * If any pair of shapes collides, CollisionEnter() is called on both objects. An object
 * whose line shape was involved receives the overload that also takes the collision point,
 * given in that object's local coordinates; otherwise the overload without a point is used.
 * Each pair of objects is reported at most once per call.
 */
void GameEngine::HandleCollisions(DrawContext &drawContext) {
    // For each collision object, save their world bounds for later.
    std::vector<std::shared_ptr<CollisionObject>> colObjects;
    std::vector<Rect> worldBounds;
    for (auto& obj : mGameObjects) {
        auto colObj = std::dynamic_pointer_cast<CollisionObject>(obj);
        if (!colObj) {
            continue;
        }

        drawContext.SetContextOffset(colObj->GetLocation());
        drawContext.SetContextRotation(colObj->GetRotation());
        colObjects.push_back(colObj);
        worldBounds.push_back(drawContext.Transform(colObj->GetBounds()));
    }

    int objCount = colObjects.size();
    for (int i = 0; i < objCount; ++i) {
        for (int j = i + 1; j < objCount; ++j) {
            
            Rect bb1 = worldBounds[i];
            Rect bb2 = worldBounds[j];
            
            if (bb1.topLeft.x > bb2.topLeft.x + bb2.width ||
                bb2.topLeft.x > bb1.topLeft.x + bb1.width ||
                bb1.topLeft.y > bb2.topLeft.y + bb2.height ||
                bb2.topLeft.y > bb1.topLeft.y + bb1.height) {
                continue;
            }
            
            std::shared_ptr<CollisionObject> objs[2] = {colObjects[i], colObjects[j]};
            std::vector<Shape> wordlShapes[2];
            bool collisionHappened = false;
            bool hitLine[2] = {false, false};
            Point2D collisionPoint[2];

            for (int k = 0; k < 2; ++k) {
                drawContext.SetContextOffset(objs[k]->GetLocation());
                drawContext.SetContextRotation(objs[k]->GetRotation());

                for (const Shape &s : objs[k]->GetShapes()) {
                    if (s.t == ShapeType::kCircle) {
                        Circle circle(drawContext.Transform(s.shape.circle.center), s.shape.circle.radius);
                        wordlShapes[k].push_back(Shape(circle));
                    } else if (s.t == ShapeType::kLine) {
                        Line line(drawContext.Transform(s.shape.line.p1), drawContext.Transform(s.shape.line.p2));
                        wordlShapes[k].push_back(Shape(line));
                    } else {
                        // Rect isn't transformed so that if it's rotated, it doesn't turn into its bounding box
                        wordlShapes[k].push_back(s);
                    }
                }
            }

            for (int x = 0; x < wordlShapes[0].size(); ++x) {
                for (int y = 0; y < wordlShapes[1].size(); ++y) {
                    Shape &shape1 = wordlShapes[0][x];
                    Shape &shape2 = wordlShapes[1][y];
                    Point2D hitInWorld;
                    bool perLoopCollided = false;

                   if (shape1.t == ShapeType::kCircle) {
                        if (shape2.t == ShapeType::kCircle) {
                            perLoopCollided = shape1.shape.circle.Intersects(shape2.shape.circle);
                        } else if (shape2.t == ShapeType::kLine) {
                            perLoopCollided = shape1.shape.circle.Intersects(shape2.shape.line, hitInWorld);
                            hitLine[1] = true;
                        } else if (shape2.t == ShapeType::kRect) {
                            drawContext.SetContextOffset(objs[1]->GetLocation());
                            drawContext.SetContextRotation(objs[1]->GetRotation());
                            Circle circle(drawContext.ReverseTransform(shape1.shape.circle.center), shape1.shape.circle.radius);
                            perLoopCollided = circle.Intersects(shape2.shape.rect);
                        }
                   } else if (shape1.t == ShapeType::kLine) {
                        if (shape2.t == ShapeType::kCircle) {
                            perLoopCollided = shape2.shape.circle.Intersects(shape1.shape.line, hitInWorld);
                            hitLine[0] = true;
                        } else if (shape2.t == ShapeType::kLine) {
                            perLoopCollided = shape1.shape.line.Crosses(shape2.shape.line, hitInWorld);
                            hitLine[0] = true;
                            hitLine[1] = true;
                        } else if (shape2.t == ShapeType::kRect) {
                            drawContext.SetContextOffset(objs[1]->GetLocation());
                            drawContext.SetContextRotation(objs[1]->GetRotation());
                            Line line(drawContext.ReverseTransform(shape1.shape.line.p1), drawContext.ReverseTransform(shape1.shape.line.p2));
                            perLoopCollided = shape2.shape.rect.Intersects(line, hitInWorld);
                            if (perLoopCollided) {
                                hitInWorld = drawContext.Transform(hitInWorld);
                                hitLine[0] = true;
                            }
                        }
                    } else if (shape1.t == ShapeType::kRect) {
                        if (shape2.t == ShapeType::kCircle) {
                            drawContext.SetContextOffset(objs[0]->GetLocation());
                            drawContext.SetContextRotation(objs[0]->GetRotation());
                            Circle circle(drawContext.ReverseTransform(shape2.shape.circle.center), shape2.shape.circle.radius);
                            perLoopCollided = circle.Intersects(shape1.shape.rect);
                        } else if (shape2.t == ShapeType::kLine) {
                            drawContext.SetContextOffset(objs[0]->GetLocation());
                            drawContext.SetContextRotation(objs[0]->GetRotation());
                            Line line(drawContext.ReverseTransform(shape2.shape.line.p1), drawContext.ReverseTransform(shape2.shape.line.p2));
                            perLoopCollided = shape1.shape.rect.Intersects(line, hitInWorld);
                            if (perLoopCollided) {
                                hitInWorld = drawContext.Transform(hitInWorld);
                                hitLine[1] = true;
                            }
                        } else if (shape2.t == ShapeType::kRect) {
                            /*
                                THIS IS AI GENERATED CODE.
                                Prompt: How do I complete the else if (shape2.t == ShapeType::kRect) where
                                I compare to see if two rectangle collide the same I have done when a rectangle
                                collides with another shape type.
                                Reflection: The code succesfully impleted the Rect vs Rect collisions.
                            */
                            

                            // Rect vs rect. Work in object 1's local space, where its rect isn't rotated.
                            Rect rect1 = shape1.shape.rect;
                            Rect rect2 = shape2.shape.rect;

                            Point2D corners2[4] = {
                                rect2.topLeft,
                                Point2D(rect2.topLeft.x + rect2.width, rect2.topLeft.y),
                                Point2D(rect2.topLeft.x + rect2.width, rect2.topLeft.y + rect2.height),
                                Point2D(rect2.topLeft.x, rect2.topLeft.y + rect2.height)
                            };

                            // Object 2's corners: its local space -> world -> object 1's local space
                            drawContext.SetContextOffset(objs[1]->GetLocation());
                            drawContext.SetContextRotation(objs[1]->GetRotation());
                            for (int m = 0; m < 4; ++m) {
                                corners2[m] = drawContext.Transform(corners2[m]);
                            }
                            drawContext.SetContextOffset(objs[0]->GetLocation());
                            drawContext.SetContextRotation(objs[0]->GetRotation());
                            for (int m = 0; m < 4; ++m) {
                                corners2[m] = drawContext.ReverseTransform(corners2[m]);
                            }

                            // Does any edge of object 2's rect touch object 1's rect?
                            for (int m = 0; m < 4 && !perLoopCollided; ++m) {
                                Point2D unused;
                                perLoopCollided = rect1.Intersects(Line(corners2[m], corners2[(m + 1) % 4]), unused);
                            }

                            /*END OF AI GENERATED CODE*/
                        }
                    }
                    


                    if (perLoopCollided) {
                        collisionHappened = true;
                        if (hitLine[0]) {
                            drawContext.SetContextOffset(objs[0]->GetLocation());
                            drawContext.SetContextRotation(objs[0]->GetRotation());
                            collisionPoint[0] = drawContext.ReverseTransform(hitInWorld);
                        }

                        if (hitLine[1]) {
                            drawContext.SetContextOffset(objs[1]->GetLocation());
                            drawContext.SetContextRotation(objs[1]->GetRotation());
                            collisionPoint[1] = drawContext.ReverseTransform(hitInWorld);
                        }
                    }
                }
            }

            if (collisionHappened) {
                for (int k = 0; k < 2; ++k) {
                    int otherIdx = k ? 0 : 1;
                    if (hitLine[k]) {
                        objs[k]->CollisionEnter(objs[otherIdx], collisionPoint[k]);
                    } else {
                        objs[k]->CollisionEnter(objs[otherIdx]);
                    }
                }
            }

        }
    }

}

/**
 * @brief Runs the main game loop until the window is closed.
 *
 * Each frame performs these steps in order:
 *   0. Removes objects whose IsAlive() returns false.
 *   1. Moves objects queued by AddGameObject() into the main list and initializes them.
 *   2. Processes window and keyboard events (see ProcessEvents).
 *   3. Calls Update() on every object.
 *   4. Detects collisions between collision objects (see HandleCollisions).
 *   5. Calls LateUpdate() on every object.
 *   6. Clears the window and calls RenderBackground() on every graphics object.
 *   7. Calls RenderForeground() on every graphics object.
 *   8. Calls RenderUI() on every object.
 *   9. If drawBB is true, outlines each collision object's world bounding box in yellow.
 *  10. Displays the finished frame and advances the frame counter.
 *
 * Before an object's RenderBackground() and RenderForeground() calls, the screen context's
 * offset and rotation are set to the object's location and rotation, so the object draws in
 * its own local coordinates. The GUI context is never transformed and draws in window pixels.
 * GameContext::CurrObject is set to the object being called before every call.
 *
 * Does not return until the player closes the window.
 */
void GameEngine::Run() {
    DrawContext drawContext(mWindow, mFont);
    DrawContext guiContext(mWindow, mFont);
    GameContext context{this, &drawContext, &guiContext};
    context.mNotificationManager = &mNotificationManager;
    context.currentFrame = mCurrFrame;

    while (mWindow->isOpen())  // window is open
    {
        context.currentFrame = mCurrFrame;
        // 0. Remove any objects that are now dead
        mGameObjects.erase(std::remove_if(mGameObjects.begin(), mGameObjects.end(), isObjectDead), mGameObjects.end());

        // 1. Activate and initialize any objects added during the last frame
        std::vector<std::shared_ptr<GameObject>> temp;
        temp.swap(mNewObjects);
        for (auto& obj : temp) {
            context.CurrObject = obj;
            obj->Initialize(&context);
            mGameObjects.push_back(std::move(obj));
        }

        // 2. Process events
        bool shouldQuit = ProcessEvents(&context);
        if (shouldQuit) {
            break;
        }

        // 3. Update game objects
        for(auto& obj : mGameObjects) {
            context.CurrObject = obj;
            obj->Update(&context);
        }

        // 4. Process collision events
        HandleCollisions(drawContext);

        // 5. Late updates
        for(auto& obj : mGameObjects) {
            context.CurrObject = obj;
            obj->LateUpdate(&context);
        }

        // Clear window
        mWindow->clear(sf::Color::Black);

        // 6. Render background
        for(auto& obj : mGameObjects) {
            if (auto graphic = std::dynamic_pointer_cast<GraphicsObject>(obj)) {
                drawContext.SetContextOffset(graphic->GetLocation());
                drawContext.SetContextRotation(graphic->GetRotation());
                context.CurrObject = graphic;
                graphic->RenderBackground(&context);
            }
        }

        // 7. Render foreground
        for(auto& obj : mGameObjects) {
            if (auto graphic = std::dynamic_pointer_cast<GraphicsObject>(obj)) {
                drawContext.SetContextOffset(graphic->GetLocation());
                drawContext.SetContextRotation(graphic->GetRotation());
                context.CurrObject = graphic;
                graphic->RenderForeground(&context);
            }
        }

        for(auto& obj : mGameObjects) {
            context.CurrObject = obj;
            obj->RenderUI(&context);
        }

        const bool drawBB = false;
        if (drawBB) {
            for (auto& obj : mGameObjects) {
                if (auto graphic = std::dynamic_pointer_cast<CollisionObject>(obj)) {
                    drawContext.SetContextOffset(graphic->GetLocation());
                    drawContext.SetContextRotation(graphic->GetRotation());
                    Rect box = drawContext.Transform(graphic->GetBounds());
                    guiContext.FrameRect(box, 1.0f, Colors::yellow);
                }
            }
        }

        // Actually render to window
        mWindow->display();
        ++mCurrFrame;
    }
}

/**
 * @brief Predicate used to remove dead objects from the engine.
 *
 * @param obj The object to test.
 * @return true if the object is no longer alive and should be removed, false otherwise.
 */
bool isObjectDead(const std::shared_ptr<GameObject> obj) { return !obj->IsAlive(); }

/**
 * @brief Handles all SFML window events that arrived since the last frame.
 *
 * @param context The current game context. Currently unused.
 * @return true if the window was closed and the game should stop, false otherwise.
 *
 * A close event closes the window and returns immediately. For each key-pressed event, every
 * notification installed for that key's scancode with InstallKeyDownNotification() is sent
 * through the notification manager; key-released events do the same with the notifications
 * from InstallKeyUpNotification(). Resize events and all other events are ignored.
 */
bool GameEngine::ProcessEvents(GameContext *context)
{
	while (const std::optional event = mWindow->pollEvent())
	{
		if (event->is<sf::Event::Closed>())
		{
            mWindow->close();
            return true;
		}
		else if (event->is<sf::Event::Resized>())
		{
		}
        else if (const auto* keyDown = event ->getIf<sf::Event::KeyPressed>()) {
            int keyCode = static_cast<int>(keyDown->scancode);
            if (mNotificationKeyDown.find(keyCode) != mNotificationKeyDown.end()) {
                for (auto& message : mNotificationKeyDown[keyCode]) {
                    mNotificationManager.Notify(message);
                }
            }
        }
        else if (const auto* keyUp = event ->getIf<sf::Event::KeyReleased>()) {
            int keyCode = static_cast<int>(keyUp->scancode);
            if (mNotificationKeyUp.find(keyCode) != mNotificationKeyUp.end()) {
                for (auto& message : mNotificationKeyUp[keyCode]) {
                    mNotificationManager.Notify(message);
                }
            }
        }      
	}

    return false;
}

/**
 * @brief Sends a notification whenever a key is pressed.
 *
 * @param key The key's scancode, as an int (see the constants in Keyboard.h).
 * @param notification The notification key sent to the NotificationManager when the key is
 *                     pressed. Objects receive it if they registered for this key.
 *
 * A key can have several notifications. Installing the same notification for the same key
 * more than once has no extra effect.
 */
void GameEngine::InstallKeyDownNotification(int key, const std::string &notification) {
    if (mNotificationKeyDown.find(key) == mNotificationKeyDown.end()) {
        mNotificationKeyDown[key] = {notification};
    } else if (std::find(mNotificationKeyDown[key].begin(), mNotificationKeyDown[key].end(), notification) == mNotificationKeyDown[key].end()) {
        mNotificationKeyDown[key].push_back(notification);
    }
}

/**
 * @brief Sends a notification whenever a key is released.
 *
 * @param key The key's scancode, as an int (see the constants in Keyboard.h).
 * @param notification The notification key sent to the NotificationManager when the key is
 *                     released. Objects receive it if they registered for this key.
 *
 * A key can have several notifications. Installing the same notification for the same key
 * more than once has no extra effect.
 */
void GameEngine::InstallKeyUpNotification(int key, const std::string &notification) {
    if (mNotificationKeyUp.find(key) == mNotificationKeyUp.end()) {
        mNotificationKeyUp[key] = {notification};
    } else if (std::find(mNotificationKeyUp[key].begin(), mNotificationKeyUp[key].end(), notification) == mNotificationKeyUp[key].end()) {
        mNotificationKeyUp[key].push_back(notification);
    }
}

/**
 * @brief Iterators over the active game objects.
 *
 * Objects queued by AddGameObject() are not included until the next frame starts. The
 * iterators are invalidated when objects are added to or removed from the active list,
 * which happens at the start of each frame.
 */
GameEngine::const_iterator GameEngine::cbegin() const {
    return mGameObjects.cbegin();
}

GameEngine::const_iterator GameEngine::cend() const {
    return mGameObjects.cend();
}

GameEngine::const_iterator GameEngine::begin() const {
    return mGameObjects.begin();
}

GameEngine::const_iterator GameEngine::end() const {
    return mGameObjects.end();
}

}  // namespace CMPUT350
