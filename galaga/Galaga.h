/**
 * @file Galaga.h
 * @brief Top-level game controller: state machine, waves, attacks, scoring and UI.
 */
#ifndef GALAGA_H
#define GALAGA_H

#include "MathUtil.h"
#include "GameContext.h"
#include "GameObject.h"
#include "Keyboard.h"
#include "DrawContext.h"
#include <vector>
#include <string>

/// Points for killing an enemy in formation (doubled if it was moving).
constexpr int POINTS = 10;
/// Number of enemy waves (formation rows) spawned per level.
constexpr int NUM_WAVES = 5;
/// Number of enemies spawned per wave.
constexpr int NUM_ENEMIES = 8;
/// Frames to show the "LEVEL n" screen before enemies start spawning.
constexpr int STARTUP_WAIT_FRAMES = 120;
/// Frames between spawning consecutive enemies within a wave.
constexpr int ENEMY_SPAWN_INTERVAL = 5;
/// Frames to show the "GAME OVER" screen before returning to the title.
constexpr int GAMEOVER_WAIT_FRAMES = 150;
/// Frames between attack rounds during gameplay.
constexpr int ATTACK_WAIT_FRAMES = 60;

class Player;

/**
 * @class Galaga
 * @brief Owns the overall game flow.
 *
 * State machine:
 * @code
 *   InsertCoins --(5)--> StartGame --(1)--> Startup --(wait)--> Waves
 *        ^                                     ^                  |
 *        |                                     | all enemies      | all enemies
 *        |                                     | dead (level++)   v in formation
 *     GameOver <------- player dead ------- Gameplay <-----------+
 * @endcode
 *
 * Galaga also installs the key bindings, spawns the Player and enemies,
 * picks which enemies attack, tracks score/level and draws the UI text.
 */
class Galaga : public CMPUT350::GameObject {
public:
    /**
     * @brief Creates the controller in the InsertCoins state.
     * @param width  Window width, used to position UI text.
     * @param height Window height, used to position UI text.
     */
    Galaga(int width, int height);
    /**
     * @brief Installs key bindings, registers for game notifications,
     *        adds the starfield background and sets up the entrance paths.
     */
    void Initialize(CMPUT350::GameContext* context) override;
    /// Runs one frame of the current game state (see class description).
    void Update(CMPUT350::GameContext* context) override;
    /// Draws the title, level, score, lives and game-over text for the current state.
    void RenderUI(CMPUT350::GameContext* context) override;
    /**
     * @brief Handles notifications:
     *  - "InsertCoins"          -> StartGame
     *  - "StartGame"            -> Startup
     *  - "PlayerDied"           -> GameOver
     *  - "EnemyDied"            -> +POINTS
     *  - "EnemyDiedWhileMoving" -> +2 * POINTS
     */
    void ReceiveNotification(const std::string& key) override;

private:
    /**
     * @brief Spawns the next enemy of the current wave at the start of its
     *        entrance path, with its formation slot appended to the path.
     */
    void SpawnEnemy(CMPUT350::GameContext* context);
    const CMPUT350::RGBColor& WHITE = CMPUT350::Colors::white;   ///< UI colour.
    const CMPUT350::RGBColor& YELLOW = CMPUT350::Colors::yellow; ///< UI colour.
    const CMPUT350::RGBColor& GREEN = CMPUT350::Colors::green;   ///< UI colour.
    const CMPUT350::RGBColor& RED = CMPUT350::Colors::red;       ///< UI colour.
    /// Game states, in the order they are normally visited.
    enum class State { InsertCoins, StartGame, Startup, Waves, Gameplay, GameOver };
    State state;            ///< Current state.
    State lastState;        ///< State on the previous frame (used to detect transitions).
    int width;              ///< Window width.
    int height;             ///< Window height.
    int points;             ///< Current score.
    int level;              ///< Current level (starts at 1).
    int spawnWaves;         ///< Waves still to start; also selects the row/enemy type of the current wave.
    int spawnEnemy;         ///< Enemies left to spawn in the current wave.
    int stateStartFrame;    ///< Frame number when the current state was entered.
    int enemyAttackCounter; ///< Round-robin index of the next enemy to attack.
    std::weak_ptr<Player> mPlayer; ///< The player ship, if one exists.
    std::vector<std::vector<CMPUT350::Point2D>> entrances; ///< Entrance path templates (formation slot is appended when spawning).
};
#endif