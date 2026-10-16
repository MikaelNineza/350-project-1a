
#ifndef GAMEENGINE_H
#define GAMEENGINE_H

namespace CMPUT350 {
class GameEngine;
}

#include <memory>
#include <unordered_map>
#include "EngineView.h"
#include "GameObject.h"
#include "MathUtil.h"
#include "NotificationManager.h"
#include <SFML/Graphics.hpp>

namespace CMPUT350 {

class DrawContext;

/**
 * @brief Owns the window and game objects and runs the main game loop.
 *
 * Create one, add game objects with AddGameObject(), then call Run().
 */
class GameEngine : public EngineView {
public:
    GameEngine(unsigned int width, unsigned int height, const std::string& name);
    ~GameEngine();

    GameEngine(const GameEngine&) = delete;             // Prevent copy-construction
    GameEngine(GameEngine&&) = delete;                  // Prevent move-construction
    GameEngine& operator=(const GameEngine&) = delete;  // Prevent assignment
    GameEngine& operator=(GameEngine&&) = delete;       // Prevent move-assignment

    void AddGameObject(std::shared_ptr<GameObject> gameObject) override;
    void InstallKeyDownNotification(int, const std::string &notification) override;
	void InstallKeyUpNotification(int, const std::string &notification) override;
	const_iterator cbegin() const override;
	const_iterator cend() const override;
	const_iterator begin() const override;
	const_iterator end() const override;

    void HandleCollisions(DrawContext &drawContext);
    void Run();

    bool ProcessEvents(GameContext *context);

private:
    NotificationManager mNotificationManager;
    int mCurrFrame = 0;
    std::shared_ptr<sf::RenderWindow> mWindow;
    std::shared_ptr<sf::Font> mFont;
    std::vector<std::shared_ptr<GameObject>> mGameObjects;
    std::vector<std::shared_ptr<GameObject>> mNewObjects;
    std::unordered_map<int, std::vector<std::string>> mNotificationKeyDown;
    std::unordered_map<int, std::vector<std::string>> mNotificationKeyUp;
};

}  // namespace CMPUT350

#endif  // GAMEENGINE_H
