#include "Galaga.h"
#include "Player.h"
#include "Enemy.h"

#define POINTS 10
#define NUM_ENEMIES 8
#define STARTUP_WAIT_FRAMES 120 // wait 2 seconds until start
#define WAVES_SPAWN_INTERVAL 15 // one enemy every 0.25 seconds
#define GAMEOVER_WAIT_FRAMES 300 // wait 5 seconds until back to menu

Galaga::Galaga(int width, int height) :
    state(State::InsertCoins),
    lastState(State::InsertCoins),
    width(width),
    height(height),
    points(0),
    level(1) {
}

void Galaga::Initialize(CMPUT350::GameContext *context) {
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::A, "MoveLeftHeld");
    context->mEngineView->InstallKeyUpNotification(CMPUT350::Key::A, "MoveLeftReleased");
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Left, "MoveLeftHeld");
    context->mEngineView->InstallKeyUpNotification(CMPUT350::Key::Left, "MoveLeftReleased");

    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::D, "MoveRightHeld");
    context->mEngineView->InstallKeyUpNotification(CMPUT350::Key::D, "MoveRightReleased");
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Right, "MoveRightHeld");
    context->mEngineView->InstallKeyUpNotification(CMPUT350::Key::Right, "MoveRightReleased");

    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Space, "Fire");

    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Num5, "InsertCoins");
    context->mEngineView->InstallKeyDownNotification(CMPUT350::Key::Num1, "StartGame");

    context->mNotificationManager->Register(context->CurrObject, "EnemyDied");
    context->mNotificationManager->Register(context->CurrObject, "EnemyDiedWhileMoving");
}

void Galaga::Update(CMPUT350::GameContext* context) {
    if (state != lastState) {
        lastState = state;
        stateStartFrame = context->currentFrame;
    }

    int elapsed = context->currentFrame - stateStartFrame;

    switch (state) {
        case State::InsertCoins:
            context->mNotificationManager->Register(context->CurrObject, "InsertCoins");
            break;
        case State::StartGame:
            context->mNotificationManager->Unregister(context->CurrObject, "InsertCoins");
            context->mNotificationManager->Register(context->CurrObject, "StartGame");
            break;
        case State::Startup:
            context->mNotificationManager->Unregister(context->CurrObject, "StartGame");
            if (mPlayer.expired()) {
                std::shared_ptr<Player> player = std::make_shared<Player>(CMPUT350::Point2D(768 / 2, 900));
                context->mEngineView->AddGameObject(player);
                mPlayer = player;
            }
            spawnEnemies = 8;
            if (elapsed >= STARTUP_WAIT_FRAMES) {
                state = State::Waves;
            }
            break;
        case State::Waves:
            // add an enemy object to the screen every k-th frame:
            if (spawnEnemies > 0) {
                if (elapsed % WAVES_SPAWN_INTERVAL == 0) {
                    spawnEnemies--;
                    //std::shared_ptr<Enemy> enemy = std::make_shared<Enemy>(CMPUT350::Point2D(0.f, 0.f));
                    //context->mEngineView->AddGameObject(enemy);
                }
            }
            // check to see if all enemies have been added, if so start game
            else {
                bool readyForGameplay = true;
                for (const auto& obj : *context->mEngineView) {
                    if (auto enemy = std::dynamic_pointer_cast<Enemy>(obj)) {
                        if (!enemy->InFormation()) {
                            readyForGameplay = false;
                            break;
                        }
                    }
                }
                if (readyForGameplay) {
                    state = State::Gameplay;
                }
            }
            break;
        case State::Gameplay: {
            // call Attack on subset of enemies deterministically
            //
            // check to see if there are no enemies remaining
            bool allEnemiesDead = true;
            for (const auto& obj : *context->mEngineView) {
                auto enemy = std::dynamic_pointer_cast<Enemy>(obj);
                if (enemy && enemy->IsAlive()) {
                    allEnemiesDead = false;
                    break;
                }
            }
            if (allEnemiesDead) {
                level++;
                spawnEnemies = NUM_ENEMIES;
                state = State::Startup;
            }
            break;
        }
        case State::GameOver:
            if (elapsed >= GAMEOVER_WAIT_FRAMES) {
                points = 0;
                level = 1;
                state = State::InsertCoins;
            }
            break;
    }
}

void Galaga::RenderUI(CMPUT350::GameContext *context) {
    if (state == State::InsertCoins) {
        // draw text to tell user to insert coins (press 5)
    }
    else if (state == State::StartGame) {
        // draw text to tell user to start game (press 1)
    }
    else if (state >= State::Startup) {
        if (state == State::Startup) {
            // display the current level on the screen
        }
        else if (state == State::GameOver) {
            // draw text to tell user that game is over
            return;
        }
        // show the score on the top left
    }
}

void Galaga::ReceiveNotification(const std::string &key) {
    if (key == "InsertCoins") {
        state = State::StartGame;
    }
    else if (key == "StartGame") {
        state = State::Startup;
    }
    else if (key == "EnemyDied") {
        points += POINTS;
    }
    else if (key == "EnemyDiedWhileMoving") {
        points += 2 * POINTS;
    }
}
