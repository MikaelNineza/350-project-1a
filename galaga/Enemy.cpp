#include "Enemy.h"
#include "Bullet.h"

#define ENEMY_WIDTH 40.f
#define ENEMY_HEIGHT 40.f

/**
 * @brief Constructs an Enemy at the given position.
 *
 * @param loc Center position of the enemy, in screen coordinates.
 */
Enemy::Enemy(CMPUT350::Point2D loc) : loc(loc), isAlive(true)
{
}

/**
 * @brief Engine hook called once when the enemy is added to the game.
 *
 * Currently does nothing.
 *
 * @param context Shared game context (unused).
 */
void Enemy::Initialize(CMPUT350::GameContext* context)
{
    return;
}

/**
 * @brief Per-frame update. Enemies are currently stationary, so this is a no-op.
 *
 * @param context Shared game context (unused).
 */
void Enemy::Update(CMPUT350::GameContext* context)
{
    return;
}

/**
 * @brief Post-update step. Currently does nothing.
 *
 * @param context Shared game context (unused).
 */
void Enemy::LateUpdate(CMPUT350::GameContext* context)
{
    return;
}

/**
 * @brief Handles a keyboard event. Enemies do not respond to input.
 *
 * @param context Shared game context (unused).
 * @param key     The key that was pressed (unused).
 * @return Always false, since the event is never consumed.
 */
bool Enemy::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    return false;
}

/**
 * @brief Draws background-layer content. Enemies have none.
 *
 * @param context Shared game context (unused).
 */
void Enemy::RenderBackground(CMPUT350::GameContext* context)
{
    return;
}

/**
 * @brief Draws the enemy in the foreground layer as a solid red rectangle
 *        covering its bounding box.
 *
 * @param context Shared game context; provides the drawing surface.
 */
void Enemy::RenderForeground(CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect(GetBounds(), CMPUT350::Colors::red);
}

/**
 * @brief Engine callback invoked when the enemy starts colliding with an object.
 *
 * Intentionally empty: Bullet::CollisionEnter handles killing the enemy when
 * it is struck by a player bullet.
 *
 * @param obj The object the enemy collided with (unused).
 */
void Enemy::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    return;
}

/**
 * @brief Marks the enemy as dead.
 *
 * The engine checks IsAlive() and removes the enemy on its next cleanup pass.
 */
void Enemy::Kill()
{
    isAlive = false;
}

/**
 * @brief Reports whether the enemy is still alive.
 *
 * @return true if the enemy has not been killed, false otherwise.
 */
bool Enemy::IsAlive() const
{
    return isAlive;
}

/**
 * @brief Computes the enemy's axis-aligned bounding box.
 *
 * The box is 40x40 pixels and centered on the enemy's location. It is used for
 * both collision detection and rendering.
 *
 * @note The returned reference points to a function-local static Rect that is
 *       shared by all Enemy instances and overwritten on every call. Copy the
 *       value if you need it to survive another call to GetBounds().
 *
 * @return Reference to the enemy's current bounds.
 */
const CMPUT350::Rect& Enemy::GetBounds()
{
    static CMPUT350::Rect sBounds(0, 0, 0, 0);
    auto topLeft = loc - CMPUT350::Point2D(ENEMY_WIDTH / 2.f, ENEMY_HEIGHT / 2.f);
    sBounds = CMPUT350::Rect(topLeft, ENEMY_WIDTH, ENEMY_HEIGHT);
    return sBounds;
}
