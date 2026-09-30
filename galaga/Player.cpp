#include <cassert>
#include "Player.h"
#include "Bullet.h"

#define MOVEMENT_SPEED 15.f
#define PLAYER_WIDTH 40.f
#define PLAYER_HEIGHT 40.f

/**
 * @brief Constructs a Player at the given position.
 *
 * The player starts out alive with no active bullets.
 *
 * @param loc Initial center position of the player, in screen coordinates.
 */
Player::Player(CMPUT350::Point2D loc) : loc(loc), isAlive(true)
{
}

/**
 * @brief Engine hook called once when the player is added to the game.
 *
 * Currently does nothing; the player needs no setup beyond its constructor.
 *
 * @param context Shared game context (unused).
 */
void Player::Initialize(CMPUT350::GameContext* context)
{
    return;
}

/**
 * @brief Per-frame update. Prunes bullets that no longer exist in the engine.
 *
 * The player keeps weak references to the bullets it has fired so it can
 * enforce the two-bullet limit. The engine owns the bullets and removes them
 * once IsAlive() returns false, so any weak_ptr that has expired is dropped
 * from the list here. The list is rebuilt into a temporary vector and then
 * swapped in.
 *
 * @param context Shared game context (unused).
 */
void Player::Update(CMPUT350::GameContext* context)
{
    // remove bullets that are no longer in the engine
    // engine responsible for dumping it after checking isAlive
    std::vector<std::weak_ptr<CMPUT350::CollisionObject>> aux;
    aux.reserve(bullets.size());
    for (const auto& bullet : bullets) {
        if (!bullet.expired()) {
            aux.push_back(bullet);
        }
    }
    aux.swap(bullets);
}

/**
 * @brief Engine hook called after all objects have run Update().
 *
 * Currently does nothing.
 *
 * @param context Shared game context (unused).
 */
void Player::LateUpdate(CMPUT350::GameContext* context)
{
}

/**
 * @brief Handles a keyboard event for the player.
 *
 * Supported keys:
 *  - 'a': move left by MOVEMENT_SPEED pixels, clamped so x never goes below 0.
 *  - 'd': move right by MOVEMENT_SPEED pixels, clamped to the window width.
 *  - ' ' (space): fire a bullet straight up from the top of the ship. At most
 *         two player bullets may be active at once; if the limit is reached
 *         no bullet is fired and the event is reported as not handled.
 *
 * A fired bullet is registered with the engine (which takes shared ownership)
 * and a weak reference is stored in the player's bullet list.
 *
 * @param context Shared game context; provides the screen size and engine view.
 * @param key     The character of the key that was pressed.
 * @return true if the key was consumed (moved or fired), false otherwise
 *         (unrecognized key, or fire requested while already at the bullet limit).
 */
bool Player::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    // player moves left
    if (key == 'a') {
        loc.x = std::max(PLAYER_WIDTH / 2.f, loc.x - MOVEMENT_SPEED); // player bounds within window
        return true;
    }
    // player moves right
    if (key == 'd') {
        auto width = context->ScreenContext->GetWindowWidth();
        loc.x = std::min(width - PLAYER_WIDTH / 2.f, loc.x + MOVEMENT_SPEED); // player bounds within window
        return true;
    }
    // player shoots
    if (key == ' ') {
        // shoot bullet
        if (bullets.size() == 2) { // player can fire at most 2 bullets
            return false;
        }
        auto dir = CMPUT350::Point2D(0.0f, -1.0f); // bullet shoots straight up
        auto start_pos = loc;
        start_pos.y -= PLAYER_HEIGHT / 2.f; // bullets spawn at the top of the player
        std::shared_ptr<Bullet> bullet = std::make_shared<Bullet>(start_pos, dir, true);
        context->mEngineView->AddGameObject(bullet);
        bullets.push_back(bullet);
        return true;
    }
    return false;
}

/**
 * @brief Draws background-layer content for the player.
 *
 * The player has nothing to draw in the background layer.
 *
 * @param context Shared game context (unused).
 */
void Player::RenderBackground(CMPUT350::GameContext* context)
{
    return;
}

/**
 * @brief Draws the player's ship in the foreground layer.
 *
 * The ship is composed of four gray rectangles derived from the 40x40 bounds:
 *  - left wing: left quarter of the width, lower half of the height
 *  - main body: the bounds inset by a quarter of the width on every side
 *  - right wing: right quarter of the width, lower half of the height
 *  - tip: a small rectangle above the body, forming the nose of the ship
 *
 * @param context Shared game context; provides the drawing surface.
 */
void Player::RenderForeground(CMPUT350::GameContext* context)
{
    auto left_wing = GetBounds();
    left_wing.topLeft.y += left_wing.height / 2.f;
    left_wing.width /= 4.f;
    left_wing.height /= 2.f;

    auto main_body = GetBounds();
    main_body.Inset(main_body.width / 4.f);

    auto right_wing = GetBounds();
    right_wing.topLeft.y += right_wing.height / 2.f;
    right_wing.topLeft.x += right_wing.width * 0.75f;
    right_wing.width /= 4.f;
    right_wing.height /= 2.f;

    auto tip = GetBounds();
    tip.topLeft.y -= tip.height * 0.375f;
    tip.Inset(tip.height * 0.375f);

    context->ScreenContext->DrawRect(left_wing, CMPUT350::Colors::gray);
    context->ScreenContext->DrawRect(main_body, CMPUT350::Colors::gray); 
    context->ScreenContext->DrawRect(right_wing, CMPUT350::Colors::gray);
    context->ScreenContext->DrawRect(tip, CMPUT350::Colors::gray);
}

/**
 * @brief Engine callback invoked when the player starts colliding with another object.
 *
 * Intentionally empty: collision consequences are handled by Bullet::CollisionEnter,
 * which kills the player when hit by an enemy bullet.
 *
 * @param obj The object the player collided with (unused).
 */
void Player::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    return; // Bullet handles collision logic already
}

/**
 * @brief Marks the player as dead.
 *
 * The engine checks IsAlive() and removes the player on its next cleanup pass.
 */
void Player::Kill()
{
    isAlive = false;
}

/**
 * @brief Reports whether the player is still alive.
 *
 * @return true if the player has not been killed, false otherwise.
 */
bool Player::IsAlive() const
{
    return isAlive;
}

/**
 * @brief Computes the player's axis-aligned bounding box.
 *
 * The box is 40x40 pixels and centered on the player's location. It is used
 * for both collision detection and rendering.
 *
 * @note The returned reference points to a function-local static Rect that is
 *       shared by all Player instances and overwritten on every call. Copy the
 *       value if you need it to survive another call to GetBounds().
 *
 * @return Reference to the player's current bounds.
 */
const CMPUT350::Rect& Player::GetBounds()
{
    static CMPUT350::Rect sBounds(0, 0, 0, 0);
    auto topLeft = loc - CMPUT350::Point2D(PLAYER_WIDTH / 2.f, PLAYER_HEIGHT / 2.f);
    sBounds = CMPUT350::Rect(topLeft, PLAYER_WIDTH, PLAYER_HEIGHT); // player is 40x40 pixels
    return sBounds;
}
