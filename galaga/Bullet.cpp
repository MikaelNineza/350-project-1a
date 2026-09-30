#include "Bullet.h"
#include "Enemy.h"
#include "Player.h"
#include <vector>

#define BULLET_SPEED 20.0f
#define BULLET_SIZE 5.f

/**
 * @brief Constructs a bullet.
 *
 * The previous location is initialized to the starting location so the bullet
 * initially has zero length.
 *
 * @param location Starting position of the bullet.
 * @param heading  Direction of travel (expected to be a unit vector; it is
 *                 multiplied by BULLET_SPEED each frame).
 * @param player   true if the bullet was fired by the player, false if fired
 *                 by an enemy. Determines which type of object it can hit.
 */
Bullet::Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player) : location(location), prev_location(location), heading(heading), player(player), isAlive(true)
{
}

/**
 * @brief Tells whether this bullet was fired by the player.
 *
 * @return true for a player bullet, false for an enemy bullet.
 */
bool Bullet::IsPlayerBullet()
{
    return player;
}

/**
 * @brief Engine hook called once when the bullet is added to the game.
 *
 * Currently does nothing.
 *
 * @param context Shared game context (unused).
 */
void Bullet::Initialize(CMPUT350::GameContext* context)
{
    return;
}

/**
 * @brief Per-frame update. Advances the bullet along its heading.
 *
 * Saves the current location as prev_location, then moves the bullet by
 * heading * BULLET_SPEED. The previous location is used to draw the bullet as
 * a streak and to build a bounding box that covers the whole distance travelled
 * this frame, so fast bullets cannot tunnel through targets.
 *
 * @param context Shared game context (unused).
 */
void Bullet::Update(CMPUT350::GameContext* context)
{
    // track prev location for bullet bound calc
    prev_location = location;
    location += heading * BULLET_SPEED;
}

/**
 * @brief Post-update step. Kills the bullet once it leaves the screen.
 *
 * A bullet is considered out of bounds when its position reaches or passes any
 * edge of the window. Killed bullets are removed by the engine.
 *
 * @param context Shared game context; provides the window dimensions.
 */
void Bullet::LateUpdate(CMPUT350::GameContext* context)
{
    // kill bullet if it's out of bounds
    int screen_width = context->ScreenContext->GetWindowWidth();
    int screen_height = context->ScreenContext->GetWindowHeight();
    if (location.x <= 0 || location.x >= screen_width || location.y <= 0 || location.y >= screen_height) {
        Kill();
    }
}

/**
 * @brief Handles a keyboard event. Bullets do not respond to input.
 *
 * @param context Shared game context (unused).
 * @param key     The key that was pressed (unused).
 * @return Always false, since the event is never consumed.
 */
bool Bullet::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    return false;
}

/**
 * @brief Draws background-layer content. Bullets have none.
 *
 * @param context Shared game context (unused).
 */
void Bullet::RenderBackground(CMPUT350::GameContext* context)
{
    return;
}

/**
 * @brief Draws the bullet in the foreground layer.
 *
 * The bullet is rendered as a white line, BULLET_SIZE pixels thick, from its
 * previous location to its current location.
 *
 * @param context Shared game context; provides the drawing surface.
 */
void Bullet::RenderForeground(CMPUT350::GameContext* context)
{
    // bullet rendered as a line
    context->ScreenContext->DrawLine(prev_location, location, BULLET_SIZE, CMPUT350::Colors::white);
}

/**
 * @brief Engine callback invoked when the bullet starts colliding with an object.
 *
 * A player bullet only affects Enemy objects and an enemy bullet only affects
 * the Player; collisions with any other type are ignored (dynamic_pointer_cast
 * returns null). On a valid hit, both the bullet and the target are killed.
 *
 * @note In the enemy-bullet branch, the local variable named `player` shadows
 *       the `player` member flag. This is harmless here, but renaming it would
 *       avoid confusion.
 *
 * @param obj The object this bullet collided with.
 */
void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    // if target is hit, kill the bullet and the target
    if (player) {
        std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(obj);
        if (enemy == nullptr) {
            return;
        }
        Kill();
        obj->Kill();
    }
    else {
        std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(obj);
        if (player == nullptr) {
            return;
        }
        Kill();
        obj->Kill();
    }
}

/**
 * @brief Marks the bullet as dead.
 *
 * The engine checks IsAlive() and removes the bullet on its next cleanup pass.
 */
void Bullet::Kill()
{
    isAlive = false;
}

/**
 * @brief Reports whether the bullet is still active.
 *
 * @return true if the bullet has not been killed, false otherwise.
 */
bool Bullet::IsAlive() const
{
    return isAlive;
}

/**
 * @brief Computes the bullet's bounding box for collision detection.
 *
 * Builds a BULLET_SIZE x BULLET_SIZE box centered on the current location and
 * another centered on the previous location, then returns their union. This
 * covers the path swept during the last frame.
 *
 * @note The returned reference points to a function-local static Rect that is
 *       shared by all Bullet instances and overwritten on every call. Copy the
 *       value if you need it to survive another call to GetBounds().
 *
 * @return Reference to the bullet's swept bounds.
 */
const CMPUT350::Rect& Bullet::GetBounds()
{
    static CMPUT350::Rect sBounds(0, 0, 0, 0);

    auto topLeft = location - BULLET_SIZE / 2.f;
    auto topLeftPrev = prev_location - BULLET_SIZE / 2.f;
    sBounds = CMPUT350::Rect(topLeft, BULLET_SIZE, BULLET_SIZE);
    sBounds |= CMPUT350::Rect(topLeftPrev, BULLET_SIZE, BULLET_SIZE);
    
    return sBounds;
}
