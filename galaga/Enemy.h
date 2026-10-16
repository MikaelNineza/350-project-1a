/**
 * @file Enemy.h
 * @brief Abstract base class for all Galaga enemies.
 *
 * An enemy is always in one of three movement modes:
 *  - Entering:  following its entrance Bezier path into its formation slot
 *               (onPath == true, attacking == false).
 *  - Formation: sitting in its slot and swaying left/right (onPath == false).
 *  - Attacking: diving along an attack path toward the player
 *               (onPath == true, attacking == true), then following a return
 *               path built by BuildSideReturnPath() back to formation.
 *
 * Subclasses (Enemy1, Enemy2) supply their own attack path, colours, firing
 * behaviour and per-frame Update().
 */
#ifndef ENEMY_H
#define ENEMY_H

#include "CollisionObject.h"
#include "GameContext.h"
#include "Bezier.h"
#include <vector>
#include <string>

/// Width of an enemy's collision box (pixels).
constexpr float ENEMY_WIDTH = 40.f;
/// Height of an enemy's collision box (pixels).
constexpr float ENEMY_HEIGHT = 40.f;
/// Approximate distance an enemy travels along a path each frame (pixels).
constexpr float PIXELS_PER_FRAME = 15.f;
/// Maximum horizontal sway distance from the formation slot (pixels).
constexpr float SWAY_DIST = 10.f;
/// Sway frequency, in radians per frame.
constexpr float SWAY_SPEED = 0.15f;
/// Approximation of pi used for rotation calculations.
constexpr float PI = 3.141592f;

/**
 * @class Enemy
 * @brief Shared state and behaviour for enemies: path following, formation,
 *        health, collisions and rendering geometry.
 */
class Enemy : public CMPUT350::CollisionObject
{
public:
    /**
     * @brief Creates an enemy that starts at the first point of its entrance path.
     * @param loc  The enemy's formation slot (where it rests once it arrives).
     * @param path Control points of the entrance Bezier curve. The first point
     *             is the spawn position; the last should be the formation slot.
     */
    Enemy(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path);

    // GameObject Functions

    /// Called once when added to the engine. No setup is needed.
    void Initialize(CMPUT350::GameContext* context) override;
    /// Base implementation does nothing; subclasses provide movement logic.
    void Update(CMPUT350::GameContext* context) override;
    /// Does nothing.
    void LateUpdate(CMPUT350::GameContext* context) override;
    /// @return false once the enemy has been removed (set in the subclass Update()).
    bool IsAlive() const override;
    /// Flags the enemy as killed. It is removed on its next Update().
    void Kill() override;

    // Graphics Object Functions

    /// Does nothing.
    void RenderBackground(CMPUT350::GameContext* context) override;
    /// Base implementation draws nothing; subclasses draw the ship in their colours.
    void RenderForeground(CMPUT350::GameContext* context) override;

    // Collision Object Functions

    /**
     * @brief Handles hits from player bullets and contact with the player.
     *
     * A player bullet removes one point of health and kills the enemy when health
     * reaches zero. Enemy bullets are ignored. Touching a non-immune player
     * kills the enemy.
     */
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;
    /// @return A 40x40 collision box centred on the enemy (local coordinates).
    const CMPUT350::Rect& GetBounds() override;

    /**
     * @brief Starts an attack dive toward a target.
     * @param context The game context.
     * @param target  The point to dive toward (usually the player's location).
     * @return true if the attack started, false if the enemy is not in formation.
     */
    virtual bool Attack(CMPUT350::GameContext* context, const CMPUT350::Point2D& target) = 0;
    /// @return The rectangles that make up the ship's drawing.
    const std::vector<CMPUT350::Shape>& GetShapes() override;
    /// @return The enemy's current world position.
    CMPUT350::Point2D GetLocation() const override;
    /// @return Rotation (radians) so the ship faces its direction of travel; 0 in formation.
    float GetRotation() const override;
    /**
     * @brief Switches the enemy onto an attack path.
     * @param attackPath Control points of the attack Bezier curve.
     * @return true if the attack started, false if the enemy was already moving.
     */
    bool BeginAttack(const std::vector<CMPUT350::Point2D>& attackPath);

    /// @return true if the enemy is resting in its formation slot (not on any path).
    bool InFormation() const;
    /// Sets the enemy's remaining hit points.
    void SetHealth(int newHealth);
    /// @return The enemy's remaining hit points.
    int GetHealth() const;
    /**
     * @brief Builds a path back to formation after an attack.
     *
     * The path swings down and out to the nearest side of the screen, travels
     * up that edge past the formation row, then curves over and drops into
     * the formation slot.
     *
     * @param loc          Where the attack dive ended.
     * @param formationLoc The enemy's formation slot.
     * @return Control points for the return Bezier curve.
     */
    std::vector<CMPUT350::Point2D> BuildSideReturnPath(const CMPUT350::Point2D &loc, const CMPUT350::Point2D& formationLoc);

protected:
    CMPUT350::Point2D loc;            ///< Current world position.
    CMPUT350::Point2D formationLoc;   ///< Formation slot the enemy returns to.
    bool isAlive;                     ///< false once the enemy has been removed.
    int health;                       ///< Remaining hit points (set by subclasses).
    std::vector<CMPUT350::Point2D> path; ///< Control points of the current path.
    float t;                          ///< Current parameter along `curve`.
    CMPUT350::Bezier curve;           ///< Curve built from `path`.
    bool onPath; // true if its moving along a bezier curve, false if not (in formation)
    bool attacking;                   ///< true while on an attack dive (not the return/entrance path).
    std::vector<CMPUT350::Shape> shapes; ///< Rectangles making up the ship drawing.
    bool died;                        ///< Set by Kill(); processed on the next Update().
    bool killedPlayer;                ///< If true, no score notification is sent on death.
};


#endif

