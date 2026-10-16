/**
 * @file Player.h
 * @brief The player's ship.
 */
#ifndef PLAYER_H
#define PLAYER_H

#include "CollisionObject.h"
#include "DrawContext.h"
#include <vector>
#include <memory>
#include <string>

/**
 * @class Player
 * @brief The ship controlled by the user.
 *
 * Moves horizontally along the bottom of the screen and fires bullets
 * straight up, with at most 2 of its bullets on screen at a time. Starts
 * with 3 lives. After being hit the ship becomes immune (and blinks) until
 * Galaga sends "ResetImmunity"; while immune it cannot fire or take damage.
 *
 * Input arrives as notifications installed by Galaga:
 * "MoveLeftHeld", "MoveLeftReleased", "MoveRightHeld", "MoveRightReleased",
 * "Fire" and "ResetImmunity".
 */
class Player : public CMPUT350::CollisionObject
{
public:
    /**
     * @brief Creates the player ship and builds its rectangles and colours.
     * @param loc Starting world position (centre of the ship).
     */
    Player(CMPUT350::Point2D loc);

    // GameObject Functions

    /// Registers for the movement, fire and immunity-reset notifications.
    void Initialize(CMPUT350::GameContext* context) override;
    /**
     * @brief Per-frame logic: handles death, cleans up the list of active
     *        bullets, fires if requested, and moves left or right.
     */
    void Update(CMPUT350::GameContext* context) override;
    /// Does nothing.
    void LateUpdate(CMPUT350::GameContext* context) override;
    /// @return false once the player has been removed (set in Update()).
    bool IsAlive() const override;
    /// Flags the player as killed. It is removed on its next Update().
    void Kill() override;

    // Graphics Object Functions

    /// Does nothing.
    void RenderBackground(CMPUT350::GameContext* context) override;
    /// Draws the ship; while immune it blinks (visible every other 5 frames).
    void RenderForeground(CMPUT350::GameContext* context) override;


    // Collision Object Functions

    /**
     * @brief Handles hits from enemy bullets and enemy ships.
     *
     * Ignored while immune. Otherwise loses a life; if no lives remain the
     * player is killed, else the player becomes immune.
     */
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;

    /// Updates movement/fire requests or clears immunity in response to notifications.
    void ReceiveNotification(const std::string& key) override;
    /// @return The rectangles that make up the ship drawing.
    const std::vector<CMPUT350::Shape>& GetShapes() override;
    /// @return A 40x40 collision box centred on the ship (local coordinates).
    const CMPUT350::Rect& GetBounds() override;
    /// @return The ship's current world position.
    CMPUT350::Point2D GetLocation() const override;
    /// @return Always 0; the ship always points up.
    float GetRotation() const override;
    /// @return Remaining lives.
    int GetLives() const;
    /// Removes one life.
    void TakeDamage();
    /// @return true while the player is immune after being hit.
    bool IsImmune();

private: 
    CMPUT350::Point2D loc;   ///< Current world position.
    bool isAlive;            ///< false once the player has been removed.
    std::vector<std::weak_ptr<CMPUT350::CollisionObject>> bullets; ///< The player's bullets still on screen.
    std::vector<CMPUT350::Shape> shapes; ///< Rectangles making up the ship drawing.
    bool requestFire;        ///< Set by "Fire"; consumed on the next Update().
    bool requestLeft;        ///< true while a move-left key is held.
    bool requestRight;       ///< true while a move-right key is held.
    int lives;               ///< Remaining lives (starts at 3).
    bool died;               ///< Set by Kill(); processed on the next Update().
    std::vector<CMPUT350::RGBColor> colors; ///< Colour for each rectangle in `shapes`.
    bool immune;             ///< true after being hit, until "ResetImmunity".
};

#endif
