#include "GameEngine.h"
#include "DrawContext.h"
#include "GameContext.h"
#include "GameObject.h"
#include "GraphicsObject.h"
#include "CollisionObject.h"

/// @brief
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
 * The window is limited to FPS_LIMIT (30) frames per second. The font is loaded from the
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

                   if (shape1.t == ShapeType::kCircle) {
                        if (shape2.t == ShapeType::kCircle) {
                            collisionHappened = shape1.shape.circle.Intersects(shape2.shape.circle);
                        } else if (shape2.t == ShapeType::kLine) {
                            collisionHappened = shape1.shape.circle.Intersects(shape2.shape.line, hitInWorld);
                            hitLine[1] = true;
                        } else if (shape2.t == ShapeType::kRect) {
                            drawContext.SetContextOffset(objs[1]->GetLocation());
                            drawContext.SetContextRotation(objs[1]->GetRotation());
                            Circle circle(drawContext.ReverseTransform(shape1.shape.circle.center), shape1.shape.circle.radius);
                            collisionHappened = circle.Intersects(shape2.shape.rect);
                        }
                   } else if (shape1.t == ShapeType::kLine) {
                        if (shape2.t == ShapeType::kCircle) {
                            collisionHappened = shape2.shape.circle.Intersects(shape1.shape.line, hitInWorld);
                            hitLine[0] = true;
                        } else if (shape2.t == ShapeType::kLine) {
                            collisionHappened = shape1.shape.line.Crosses(shape2.shape.line, hitInWorld);
                            hitLine[0] = true;
                            hitLine[1] = true;
                        } else if (shape2.t == ShapeType::kRect) {
                            drawContext.SetContextOffset(objs[1]->GetLocation());
                            drawContext.SetContextRotation(objs[1]->GetRotation());
                            Line line(drawContext.ReverseTransform(shape1.shape.line.p1), drawContext.ReverseTransform(shape1.shape.line.p2));
                            collisionHappened = shape2.shape.rect.Intersects(line, hitInWorld);
                            if (collisionHappened) {
                                hitInWorld = drawContext.Transform(hitInWorld);
                                hitLine[0] = true;
                            }
                        }
                    }
                    


                    if (collisionHappened) {
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
 *   4. Checks every pair of collision objects for overlapping bounding boxes and calls
 *      CollisionEnter() on both objects of each overlapping pair.
 *   5. Calls LateUpdate() on every object.
 *   6. Clears the window and calls RenderBackground() on every graphics object.
 *   7. Calls RenderForeground() on every graphics object.
 *   8. Displays the finished frame.
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

        const bool drawBB = true;
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
 * @param context The game context passed on to each object's HandleKeyEvent().
 * @return true if the window was closed and the game should stop, false otherwise.
 *
 * A close event closes the window and returns immediately. Each text-entered event is
 * passed as a char to HandleKeyEvent() on every active game object. Resize events are
 * ignored.
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

void GameEngine::InstallKeyDownNotification(int key, const std::string &notification) {
    if (mNotificationKeyDown.find(key) == mNotificationKeyDown.end()) {
        mNotificationKeyDown[key] = {notification};
    } else if (std::find(mNotificationKeyDown[key].begin(), mNotificationKeyDown[key].end(), notification) == mNotificationKeyDown[key].end()) {
        mNotificationKeyDown[key].push_back(notification);
    }
}

void GameEngine::InstallKeyUpNotification(int key, const std::string &notification) {
    if (mNotificationKeyUp.find(key) == mNotificationKeyUp.end()) {
        mNotificationKeyUp[key] = {notification};
    } else if (std::find(mNotificationKeyUp[key].begin(), mNotificationKeyUp[key].end(), notification) == mNotificationKeyUp[key].end()) {
        mNotificationKeyUp[key].push_back(notification);
    }
}

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
