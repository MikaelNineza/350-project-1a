#ifndef GALAGA_H
#define GALAGA_H

#include "MathUtil.h"
#include "GameContext.h"
#include "GameObject.h"
#include "Keyboard.h"
#include "DrawContext.h"
#include <vector>
#include <string>

constexpr int POINTS = 10;
constexpr int NUM_WAVES = 1;
constexpr int NUM_ENEMIES = 8;
constexpr int STARTUP_WAIT_FRAMES = 120;
constexpr int ENEMY_SPAWN_INTERVAL = 15;
constexpr int GAMEOVER_WAIT_FRAMES = 150;

class Player;

class Galaga : public CMPUT350::GameObject {
public:
    Galaga(int width, int height);
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void RenderUI(CMPUT350::GameContext* context) override;
    void ReceiveNotification(const std::string& key) override;

private:
    void SpawnEnemy(CMPUT350::GameContext* context);
    const CMPUT350::RGBColor& WHITE = CMPUT350::Colors::white;
    const CMPUT350::RGBColor& YELLOW = CMPUT350::Colors::yellow;
    const CMPUT350::RGBColor& GREEN = CMPUT350::Colors::green;
    const CMPUT350::RGBColor& RED = CMPUT350::Colors::red;
    enum class State { InsertCoins, StartGame, Startup, Waves, Gameplay, GameOver };
    State state;
    State lastState;
    int width;
    int height;
    int points;
    int level;
    int spawnWaves;
    int spawnEnemy;
    int stateStartFrame;
    std::weak_ptr<Player> mPlayer;
};
#endif