/**
 * @file Galaga.cpp
 * @brief Implementation of the Galaga game controller.
 */
#include "Galaga.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Enemy1.h"
#include "Enemy2.h"
#include "Stars.h"

/// Starts on the title screen with 0 points at level 1.
Galaga::Galaga(int width, int height) :
    state(State::InsertCoins),
    lastState(State::InsertCoins),
    width(width),
    height(height),
    points(0),
    level(1) {
}

/**
 * One-time setup:
 *  - Binds A/Left and D/Right (press and release) to movement notifications,
 *    Space to "Fire", 5 to "InsertCoins" and 1 to "StartGame".
 *  - Registers this object for enemy/player death notifications.
 *  - Adds a 250-star background covering the whole window.
 *  - Builds the entrance path templates. Index order in `entrances`:
 *    0 = topSCurveLeft, 1 = topSCurveRight, 2 = leftDoubleLoop,
 *    3 = zigZagDive, 4 = rightDoubleLoop.
 */
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

    // Each template has 9 points; SpawnEnemy() appends the formation slot as the 10th,
    // giving 3 cubic Bezier segments.

    // Enters from the top-left and sweeps through an S-curve.
    std::vector<CMPUT350::Point2D> topSCurveLeft = {
        {200, -60}, {200, 200}, {600, 200}, {600, 400},
        {600, 600}, {150, 600}, {150, 450},
        {150, 300}, {500, 250}
    };

    // Mirror image of topSCurveLeft, entering from the top-right.
    std::vector<CMPUT350::Point2D> topSCurveRight = {
        {568, -60}, {568, 200}, {168, 200}, {168, 400},
        {168, 600}, {618, 600}, {618, 450},
        {618, 300}, {268, 250}
    };

    // Enters from off the right edge and loops twice.
    std::vector<CMPUT350::Point2D> rightDoubleLoop = {
        {828, 200}, {400, 100}, {100, 300}, {300, 450},
        {500, 600}, {650, 350}, {450, 250},
        {250, 200}, {300, 500}
    };

    // Enters from off the left edge and loops twice.
    std::vector<CMPUT350::Point2D> leftDoubleLoop = {
        {-60, 200}, {368, 100}, {668, 300}, {468, 450},
        {268, 600}, {118, 350}, {318, 250},
        {518, 200}, {468, 500}
    };

    // Enters from the top-centre and zig-zags side to side.
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

/**
 * Runs the state machine for one frame. `elapsed` is the number of frames
 * since the current state was entered.
 *
 *  - InsertCoins: wait for the "InsertCoins" key (5).
 *  - StartGame:   wait for the "StartGame" key (1).
 *  - Startup:     create the player if needed, reset wave counters, then after
 *                 STARTUP_WAIT_FRAMES move to Waves.
 *  - Waves:       spawn NUM_ENEMIES enemies per wave, one every
 *                 ENEMY_SPAWN_INTERVAL frames. A new wave starts only once all
 *                 enemies are in formation. When every wave is in formation,
 *                 move to Gameplay.
 *  - Gameplay:    every ATTACK_WAIT_FRAMES frames, send up to 3 enemies
 *                 (round robin) to attack the player, but only if every enemy is
 *                 in formation. While the player is immune, no attacks start;
 *                 immunity is reset once all enemies are back in formation.
 *                 If all enemies are dead, go to the next level (Startup).
 *  - GameOver:    kill every player, enemy and bullet, then after
 *                 GAMEOVER_WAIT_FRAMES reset score/level and return to the title.
 */
void Galaga::Update(CMPUT350::GameContext* context) {
    // record the frame on which a new state was entered
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
            // only create a player for a new game; it carries over between levels
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
                // start the next wave once the previous one has settled (or immediately for the first wave)
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
                // player was hit: wait for every enemy to return to formation, then end immunity
                else if (player->IsImmune()) {
                    bool formation = true;
                    for (const auto& obj : *context->mEngineView) {
                        auto enemy = std::dynamic_pointer_cast<Enemy>(obj);
                        if (enemy != nullptr && !enemy->InFormation()) {
                            formation = false;
                            break;
                        }
                    }
                    if (formation) {
                        context->mNotificationManager->Notify("ResetImmunity");
                    }
                }
                else if (elapsed % ATTACK_WAIT_FRAMES == 0){
                    // call attack on subset of enemies
                    // (skip this round entirely if any living enemy is still moving)
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
                    // pick up to 3 attackers in round-robin order, trying each enemy at most once
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
            // clear everything
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

/**
 * Draws on-screen text for the current state:
 *  - InsertCoins / StartGame: title and the key prompt.
 *  - Startup: "LEVEL n", plus score and lives.
 *  - Waves / Gameplay: score (top-left) and lives (bottom-left).
 *  - GameOver: "GAME OVER", level reached and final score.
 */
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
            context->GUIContext->DrawCenteredText("GAME OVER", 32, CMPUT350::Point2D(width / 2.f, height / 3.f), RED);
            context->GUIContext->DrawCenteredText(std::format("LEVEL REACHED: {}", level), 16, CMPUT350::Point2D(width / 2.f, height / 2.f), WHITE);
            context->GUIContext->DrawCenteredText(std::format("FINAL SCORE: {}", points), 16, CMPUT350::Point2D(width / 2.f,  height * 0.6f), WHITE);
            return;
        }
        context->GUIContext->DrawText(std::format("SCORE: {}", points), 16, CMPUT350::Point2D(0.f, 4.f), WHITE);
        auto player = mPlayer.lock();
        if (player != nullptr) {
            context->GUIContext->DrawText(std::format("LIVES: {}", player->GetLives()), 16, CMPUT350::Point2D(0.f, context->GUIContext->GetWindowHeight() - 20), WHITE);
        }
    }
}

/// Advances the menu states, ends the game, or adds to the score (see Galaga.h).
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

/**
 * Spawns one enemy for the current wave.
 *
 * Formation layout: NUM_ENEMIES columns spread evenly between a 50 px margin
 * on each side; each wave is a row 40 px below the previous, starting at
 * y = 50.
 *
 * Wave types (spawnWaves has already been decremented for the current wave):
 *  - spawnWaves == 0 or 4 (first/last wave when NUM_WAVES is 5):
 *    Enemy2s that alternate between the left and right S-curve entrances,
 *    filling the row from the middle outward.
 *  - Other waves: Enemy1s that all use entrances[spawnWaves + 1] and fill
 *    the row from right to left.
 *
 * The formation slot is appended to the entrance template so the path ends
 * exactly in the enemy's slot.
 */
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
        // map spawn order to columns; spawnEnemy counts down, so with 8 enemies the
        // columns fill as 4, 3, 5, 2, 6, 1, 7, 0 (from the middle outward)
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