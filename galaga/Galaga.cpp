#include "Galaga.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Enemy1.h"
#include "Enemy2.h"
#include "Stars.h"

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
    context->mNotificationManager->Register(context->CurrObject, "PlayerDied");
    context->mEngineView->AddGameObject(std::make_shared<Stars>(250, CMPUT350::Rect(0, 0, context->ScreenContext->GetWindowWidth(), context->ScreenContext->GetWindowHeight())));

    /*
     * All of these paths were AI generated using Claude.
     * Prompt: Given this class that supports cubic bezier curves from a set of points (the finished Bezier class was pasted for the prompt),
     * and a world size of 768x1024, give many samples of a bezier curve that an incoming Galaga enemy might follow, e.g. starting off
     * screen and the last point being "formation". Assume that "formation" is already provided in code.
     *
     * Many paths were provided, these are the ones that were picked for usage.
     */

    std::vector<CMPUT350::Point2D> topSCurveLeft = {
        {200, -60}, {200, 200}, {600, 200}, {600, 400},
        {600, 600}, {150, 600}, {150, 450},
        {150, 300}, {500, 250}
    };

    std::vector<CMPUT350::Point2D> topSCurveRight = {
        {568, -60}, {568, 200}, {168, 200}, {168, 400},
        {168, 600}, {618, 600}, {618, 450},
        {618, 300}, {268, 250}
    };

    std::vector<CMPUT350::Point2D> rightDoubleLoop = {
        {828, 200}, {400, 100}, {100, 300}, {300, 450},
        {500, 600}, {650, 350}, {450, 250},
        {250, 200}, {300, 500}
    };

    std::vector<CMPUT350::Point2D> leftDoubleLoop = {
        {-60, 200}, {368, 100}, {668, 300}, {468, 450},
        {268, 600}, {118, 350}, {318, 250},
        {518, 200}, {468, 500}
    };

    std::vector<CMPUT350::Point2D> zigZagDive = {
        {384, -60}, {100, 100}, {100, 300}, {384, 350},
        {668, 400}, {668, 600}, {450, 650},
        {300, 700}, {200, 500}
    };

    entrances.push_back(topSCurveLeft);
    entrances.push_back(topSCurveRight);
    entrances.push_back(leftDoubleLoop);
    entrances.push_back(zigZagDive);
    entrances.push_back(rightDoubleLoop);
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
                std::shared_ptr<Player> player = std::make_shared<Player>(CMPUT350::Point2D(context->ScreenContext->GetWindowWidth() / 2, 900));
                context->mEngineView->AddGameObject(player);
                mPlayer = player;
            }
            spawnEnemy = 0;
            enemyAttackCounter = 0;
            spawnWaves = NUM_WAVES;
            if (elapsed >= STARTUP_WAIT_FRAMES) {
                state = State::Waves;
            }
            break;
        case State::Waves:
            // add an enemy object to the screen every k-th frame:
            if (spawnEnemy > 0) {
                if (elapsed % ENEMY_SPAWN_INTERVAL != 0) {
                    return;
                }
                SpawnEnemy(context);
                spawnEnemy--;
            }
            // move to next wave of enemies
            else if (spawnWaves > 0) {
                bool readyNextWave = true;
                bool enemyInit = false;
                for (const auto& obj : *context->mEngineView) {
                    if (auto enemy = std::dynamic_pointer_cast<Enemy>(obj)) {
                        enemyInit = true;
                        if (!enemy->InFormation()) {
                            readyNextWave = false;
                            break;
                        }
                    }
                }
                if (readyNextWave || !enemyInit) {
                    spawnWaves--;
                    spawnEnemy = NUM_ENEMIES;
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
            auto player = mPlayer.lock();
            if (player != nullptr) {
                if (!player->IsAlive()) {
                    state = State::GameOver;
                    return;
                }
                else if (elapsed % ATTACK_WAIT_FRAMES == 0){
                    // call attack on subset of enemies
                    std::vector<Enemy*> enemies;
                    for (const auto& obj : *context->mEngineView) {
                        auto enemy = std::dynamic_pointer_cast<Enemy>(obj);
                        if (enemy == nullptr || !enemy->IsAlive()) {
                            continue;
                        }
                        if (!enemy->InFormation()) {
                            return;
                        }
                        enemies.push_back(enemy.get());
                    }
                    int count = 3;
                    int attempts = 0;
                    int maxAttempts = enemies.size();
                    while (count > 0 && attempts < maxAttempts) {
                        if (enemies[enemyAttackCounter % enemies.size()]->Attack(context, player->GetLocation())) {
                            count--;
                            enemyAttackCounter++;
                        }
                        attempts++;
                    }
                }
            }
            // check to see if there are no enemies remaining
            bool allEnemiesDead = true;
            for (const auto& obj : *context->mEngineView) {
                auto enemy = std::dynamic_pointer_cast<Enemy>(obj);
                if (enemy != nullptr && enemy->IsAlive()) {
                    allEnemiesDead = false;
                    break;
                }
            }
            if (allEnemiesDead) {
                level++;
                state = State::Startup;
            }
            break;
        }
        case State::GameOver:
            for (const auto& obj : *context->mEngineView) {
                auto player = std::dynamic_pointer_cast<Player>(obj);
                auto enemy = std::dynamic_pointer_cast<Enemy>(obj);
                auto bullet = std::dynamic_pointer_cast<Bullet>(obj);
                if (player != nullptr) {
                    player->Kill();
                }
                if (enemy != nullptr) {
                    enemy->Kill();
                }
                if (bullet != nullptr) {
                    bullet->Kill();
                }
            }
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
        context->GUIContext->DrawCenteredText("GALAGA", 32, CMPUT350::Point2D(width / 2.f, height / 2.f), WHITE);
        context->GUIContext->DrawCenteredText("INSERT COINS (PRESS 5)", 24, CMPUT350::Point2D(width / 2.f, height * 0.75f), YELLOW);
    }
    else if (state == State::StartGame) {
        context->GUIContext->DrawCenteredText("GALAGA", 32, CMPUT350::Point2D(width / 2.f, height / 2.f), WHITE);
        context->GUIContext->DrawCenteredText("START GAME (PRESS 1)", 24, CMPUT350::Point2D(width / 2.f, height * 0.75f), GREEN);
    }
    else if (state >= State::Startup) {
        if (state == State::Startup) {
            context->GUIContext->DrawCenteredText(std::format("LEVEL {}", level), 16, CMPUT350::Point2D(width / 2.f, height / 2.f), WHITE);
        }
        else if (state == State::GameOver) {
            context->GUIContext->DrawCenteredText("GAME OVER", 32, CMPUT350::Point2D(width / 2.f, height / 2.f), RED);
            return;
        }
        context->GUIContext->DrawText(std::format("SCORE: {}", points), 16, CMPUT350::Point2D(0.f, 4.f), WHITE);
        auto player = mPlayer.lock();
        if (player != nullptr) {
            context->GUIContext->DrawText(std::format("LIVES: {}", player->GetLives()), 16, CMPUT350::Point2D(0.f, context->GUIContext->GetWindowHeight() - 20), WHITE);
        }
    }
}

void Galaga::ReceiveNotification(const std::string &key) {
    if (key == "InsertCoins") {
        state = State::StartGame;
    }
    else if (key == "StartGame") {
        state = State::Startup;
    }
    else if (key == "PlayerDied") {
        state = State::GameOver;
    }
    else if (key == "EnemyDied") {
        points += POINTS;
    }
    else if (key == "EnemyDiedWhileMoving") {
        points += 2 * POINTS;
    }
}

void Galaga::SpawnEnemy(CMPUT350::GameContext* context) {
    auto width = context->GUIContext->GetWindowWidth();
    auto height = context->GUIContext->GetWindowHeight();
    float margin = 50.f;
    float rowGap = 40.f;
    float topY = 50.f;

    float x;
    float y = topY + (NUM_WAVES - spawnWaves - 1) * rowGap;
    float spacing = (width - 2 * margin) / (NUM_ENEMIES - 1);
    std::vector<CMPUT350::Point2D> entrance;
    std::shared_ptr<Enemy> enemy;

    // first and last waves have a symmetric opening
    if (spawnWaves == 0 || spawnWaves == 4) {
        int i = spawnEnemy - 1;
        int col = (i % 2 == 0) ? i / 2 : (NUM_ENEMIES - 1) - i / 2;
        x = margin + col * spacing;
        entrance = entrances[spawnEnemy % 2];
        CMPUT350::Point2D formation(x, y);
        entrance.push_back(formation);
        enemy = std::make_shared<Enemy2>(formation, entrance);
    }
    // follows a left opening, a zig zag opening, and a right opening for the middle waves
    else {
        x = margin + (spawnEnemy - 1) * spacing;
        entrance = entrances[spawnWaves + 1];
        CMPUT350::Point2D formation(x, y);
        entrance.push_back(formation);
        enemy = std::make_shared<Enemy1>(formation, entrance);
    }

    context->mEngineView->AddGameObject(enemy);
}