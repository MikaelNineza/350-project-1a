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
    GameContext context{this, &drawContext};

    while (mWindow->isOpen())  // window is open
    {
        // 0. Remove any objects that are now dead
        mGameObjects.erase(std::remove_if(mGameObjects.begin(), mGameObjects.end(), isObjectDead), mGameObjects.end());

        // 1. Activate and initialize any objects added during the last frame
        std::vector<std::shared_ptr<GameObject>> temp;
        temp.swap(mNewObjects);
        for (auto& obj : temp) {
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
                obj->Update(&context);
        }

        // 4. Process collision events
        int objCount = mGameObjects.size();
        for (int i = 0; i < objCount; ++i) {
            auto firstObj = std::dynamic_pointer_cast<CollisionObject>(mGameObjects[i]);
            if (!firstObj) {
                continue;
            }

            for (int j = i + 1; j < objCount; ++j) {
                auto secondObj = std::dynamic_pointer_cast<CollisionObject>(mGameObjects[j]);
                if (!secondObj) {
                    continue;
                }

                Rect overlap = firstObj->GetBounds();
                overlap &= secondObj->GetBounds();

                if (overlap.width > 0 && overlap.height > 0) {
                    firstObj->CollisionEnter(secondObj);
                    secondObj->CollisionEnter(firstObj);
                }

            }
        }

        // 5. Late updates
        for(auto& obj : mGameObjects) {
                obj->LateUpdate(&context);
        }

        // Clear window
        mWindow->clear(sf::Color::Black);

        // 6. Render background
        for(auto& obj : mGameObjects) {
            if (auto graphic = std::dynamic_pointer_cast<GraphicsObject>(obj)) {
                graphic->RenderBackground(&context);
            }
        }

        // 7. Render foreground
        for(auto& obj : mGameObjects) {
            if (auto graphic = std::dynamic_pointer_cast<GraphicsObject>(obj)) {
                graphic->RenderForeground(&context);
            }
        }

        // Actually render to window
        mWindow->display();
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
		else if (const auto* keyPressed = event->getIf<sf::Event::TextEntered>())
		{
			for(auto& obj : mGameObjects) {
                obj->HandleKeyEvent(context, keyPressed->unicode);
            }
		}       
	}

    return false;
}

}  // namespace CMPUT350
