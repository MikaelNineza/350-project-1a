#ifndef GALAGA_H
#define GALAGA_H

#include "MathUtil.h"
#include <vector>
#include <string>

class Galaga : public CMPUT350::GameObject {
public:
    Galaga(int width, int height);
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void RenderUI(CMPUT350::GameContext* context) override;
    void ReceiveNotification(const std::string& key) override;

private:
    enum class State { InsertCoins, StartGame, Startup, Waves, Gameplay, GameOver };
    State state;
    State lastState;
    int width;
    int height;
    int points;
    int level;
    int spawnEnemies;
    int stateStartFrame;
    std::weak_ptr<Player> mPlayer;
};
#endif