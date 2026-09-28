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
bool isObjectDead(const shared_ptr<GameObject> obj);

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

GameEngine::~GameEngine() {
    mWindow->close();
}

void GameEngine::AddGameObject(std::shared_ptr<GameObject> gameObject) {
    mNewObjects.push_back(std::move(gameObject));
}

/**
 * @method Run
 * @arguments None
 * @description Gives control to the game engine. Will not return until the game window is closed or
 * all objects have been destroyed.
 */
void GameEngine::Run() {
    DrawContext drawContext(mWindow, mFont);
    GameContext context{this, &drawContext};

    while (true)  // window is open
    {
        // 0. Remove any objects that are now dead
        mGameObjects.erase(std::remove_if(mGameObjects.begin(), mGameObjects.end(), isObjectDead), mGameObjects.end());

        // 1. Activate and initialize any objects added during the last frame
        for (auto& obj : mNewObjects) {
            obj->Initialize(&context);
            mGameObjects.push_back(std::move(obj));
        }
        mNewObjects.clear();

        if (mGameObjects.empty()) {
            break;
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
            auto firstObj = dynamic_pointer_cast<CollisionObject>(mGameObjects[i]);
            if (!firstObj || !firstObj->IsAlive()) {
                continue;
            }

            for (int j = 0; j < objCount; ++j) {
                auto secondObj = dynamic_pointer_cast<CollisionObject>(mGameObjects[j]);
                if (!secondObj || !secondObj->IsAlive() || i == j) {
                    continue;
                }

                Rect overlap = firstObj->GetBounds();
                overlap &= secondObj->GetBounds();

                if (overlap.width > 0 && overlap.height > 0) {
                    firstObj->CollisionEnter(secondObj);
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
            if (auto graphic = dynamic_pointer_cast<GraphicsObject>(obj)) {
                graphic->RenderBackground(&context);
            }
        }

        // 7. Render foreground
        for(auto& obj : mGameObjects) {
            if (auto graphic = dynamic_pointer_cast<GraphicsObject>(obj)) {
                graphic->RenderForeground(&context);
            }
        }

        // Actually render to window
        mWindow->display();
    }
}

bool isObjectDead(const shared_ptr<GameObject> obj) { return !obj->IsAlive(); }

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
