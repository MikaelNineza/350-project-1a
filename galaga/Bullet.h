/**
 * @file Bullet.h
 * @brief Projectile fired by either the player or an enemy.
 *
 * A Bullet travels in a straight line at a constant speed along its heading.
 * It removes itself when it leaves the screen, when it hits a valid target,
 * or when the object that fired it (its parent) is destroyed.
 */
#ifndef BULLET_H
#define BULLET_H

#include "CollisionObject.h"
#include "GameContext.h"

/// Distance (in pixels) a bullet travels every frame.
constexpr float BULLET_SPEED = 15.f;
/// Thickness of the bullet line and size of its collision box (in pixels).
constexpr float BULLET_SIZE = 5.f;

/**
 * @class Bullet
 * @brief A straight-line projectile that collides with the opposing side.
 *
 * Player bullets only collide with (and are destroyed by) Enemy objects;
 * enemy bullets only collide with the Player.
 */
class Bullet : public CMPUT350::CollisionObject
{
public:
    /**
     * @brief Creates a bullet.
     * @param location Starting position in world coordinates.
     * @param heading  Unit direction of travel (e.g. (0,-1) for straight up).
     * @param player   true if fired by the player, false if fired by an enemy.
     * @param parent   The object that fired the bullet. If it is destroyed,
     *                 the bullet is destroyed too.
     */
    Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player, std::weak_ptr<CMPUT350::GameObject> parent);

    /// @return true if this bullet was fired by the player, false if by an enemy.
    bool IsPlayerBullet();

    // GameObject Functions

    /// Called once when added to the engine. Bullets need no setup.
    void Initialize(CMPUT350::GameContext* context) override;
    /// Moves the bullet one step along its heading and remembers the previous location.
    void Update(CMPUT350::GameContext* context) override;
    /// Kills the bullet if it has left the screen or its parent no longer exists.
    void LateUpdate(CMPUT350::GameContext* context) override;
    /// @return false once the bullet has been killed, so the engine can remove it.
    bool IsAlive() const override;
    /// Marks the bullet as dead.
    void Kill() override;

    // Graphics Object Functions

    /// Bullets draw nothing in the background layer.
    void RenderBackground(CMPUT350::GameContext* context) override;
    /**
     * @brief Draws the bullet as a short line (white for the player, yellow for enemies).
     *        A dead bullet is drawn as a small yellow circle (impact flash).
     */
    void RenderForeground(CMPUT350::GameContext* context) override;

    // Collision Object Functions

    /**
     * @brief Kills the bullet when it hits a valid target.
     * @param obj The object collided with. Player bullets react only to Enemy
     *            objects; enemy bullets react only to the Player.
     */
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;

    /// Bullets do not listen to any notifications.
    void ReceiveNotification(const std::string& key) override;
    /// @return The shapes describing the bullet (a single line along its heading).
    const std::vector<CMPUT350::Shape>& GetShapes() override;
    /**
     * @brief Returns the collision box in local coordinates.
     *
     * The box covers both ends of the bullet's line so that fast-moving
     * bullets are less likely to pass through a target between frames.
     */
    const CMPUT350::Rect& GetBounds() override;
    /// @return The bullet's current world position.
    CMPUT350::Point2D GetLocation() const override;
    /// @return Always 0; the line is already drawn along the heading.
    float GetRotation() const override;

private:
    CMPUT350::Point2D location;       ///< Current world position.
    CMPUT350::Point2D prev_location;  ///< Position on the previous frame.
    CMPUT350::Point2D heading;        ///< Unit direction of travel.
    bool player;                      ///< true if fired by the player.
    bool isAlive;                     ///< false once the bullet has been killed.
    std::weak_ptr<CMPUT350::GameObject> parent; ///< The object that fired this bullet.
    std::vector<CMPUT350::Shape> shapes;        ///< Shapes used for drawing/collision.
};
#endif // BULLET_H
