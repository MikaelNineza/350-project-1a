/**
 * @file Player.cpp
 * @brief Implementation of the Player ship.
 */
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"

/// Horizontal distance the player moves per frame (pixels).
#define MOVEMENT_SPEED 15.f
/// Width of the player's collision box (pixels).
#define PLAYER_WIDTH 40.f
/// Height of the player's collision box (pixels).
#define PLAYER_HEIGHT 40.f

/**
 * Initialises the player with 3 lives at the given location and builds the
 * ship drawing: 14 rectangles in local coordinates (fitting inside the 40x40
 * bounds), each paired with a colour at the same index in `colors`.
 */
Player::Player(CMPUT350::Point2D loc) : loc(loc), isAlive(true), requestFire(false), requestLeft(false), requestRight(false), lives(3), died(false), immune(false)
{
    /*
     * The design for the ship was AI generated using Claude.
     * Prompt: These are the current shapes I have for my galaga ship. Make a comprehensive list of rectangles
     * based on a good looking galaga ship. Rectangles can be drawn directly; the bounds for the player is 40x40.
     *
     * Followed by another prompt to colour each rectangle. This was also done for the enemy classes
     */
    shapes.clear();
    colors.clear();

    // --- fuselage (center column) ---
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(-2.f, -20.f), 4.f, 6.f));    // 0  nose tip
    colors.push_back(CMPUT350::Colors::white);
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(-4.f, -14.f), 8.f, 8.f));    // 1  upper fuselage
    colors.push_back(CMPUT350::Colors::white);
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(-6.f, -6.f), 12.f, 14.f));   // 2  main body
    colors.push_back(CMPUT350::Colors::white);
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(-4.f, 8.f), 8.f, 6.f));      // 3  tail
    colors.push_back(CMPUT350::Colors::red);

    // --- inner wings ---
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(-14.f, -2.f), 8.f, 12.f));   // 4  left inner wing
    colors.push_back(CMPUT350::Colors::gray);
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(6.f, -2.f), 8.f, 12.f));     // 5  right inner wing
    colors.push_back(CMPUT350::Colors::gray);

    // --- outer wing pods ---
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(-20.f, -6.f), 6.f, 18.f));   // 6  left pod
    colors.push_back(CMPUT350::Colors::red);
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(14.f, -6.f), 6.f, 18.f));    // 7  right pod
    colors.push_back(CMPUT350::Colors::red);
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(-20.f, -10.f), 6.f, 4.f));   // 8  left pod tip
    colors.push_back(CMPUT350::Colors::blue);
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(14.f, -10.f), 6.f, 4.f));    // 9  right pod tip
    colors.push_back(CMPUT350::Colors::blue);

    // --- rear thrusters ---
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(-8.f, 10.f), 4.f, 8.f));     // 10 left thruster
    colors.push_back(CMPUT350::Colors::gray);
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(4.f, 10.f), 4.f, 8.f));      // 11 right thruster
    colors.push_back(CMPUT350::Colors::gray);

    // --- cockpit detail ---
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(-2.f, -10.f), 4.f, 8.f));    // 12 cockpit
    colors.push_back(CMPUT350::Colors::blue);
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(-1.f, -4.f), 2.f, 4.f));     // 13 cockpit highlight
    colors.push_back(CMPUT350::Colors::white);
}

/// Subscribes to the input notifications installed by Galaga and to "ResetImmunity".
void Player::Initialize(CMPUT350::GameContext* context)
{
    context->mNotificationManager->Register(context->CurrObject, "MoveLeftHeld");
    context->mNotificationManager->Register(context->CurrObject, "MoveLeftReleased");
    context->mNotificationManager->Register(context->CurrObject, "MoveRightHeld");
    context->mNotificationManager->Register(context->CurrObject, "MoveRightReleased");
    context->mNotificationManager->Register(context->CurrObject, "Fire");
    context->mNotificationManager->Register(context->CurrObject, "ResetImmunity");
}

/**
 * Per-frame update:
 *  1. If killed: mark as not alive, send "PlayerDied" and stop.
 *  2. If immune: send "PlayerImmune". (deprecated, now uses helper IsImmune())
 *  3. Drop references to bullets the engine has already removed.
 *  4. If fire was requested: fire a bullet upward from the ship's nose,
 *     unless immune or 2 bullets are already on screen. Note that when
 *     immune, this returns early and skips movement for the frame.
 *  5. Move left or right (clamped to the screen) if exactly one direction
 *     key is held.
 */
void Player::Update(CMPUT350::GameContext* context)
{
    if (died) {
        isAlive = false;
        context->mNotificationManager->Notify("PlayerDied");
        return;
    }

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

    if (requestFire) {
        requestFire = false;
        if (immune) {
            return;
        }
        if (bullets.size() < 2) {
            auto dir = CMPUT350::Point2D(0.0f, -1.0f); // bullet shoots straight up
            auto start_pos = loc;
            start_pos.y -= PLAYER_HEIGHT / 2.f; // bullets spawn at the top of the player
            std::shared_ptr<Bullet> bullet = std::make_shared<Bullet>(start_pos, dir, true, context->CurrObject);
            context->mEngineView->AddGameObject(bullet);
            bullets.push_back(bullet);
        }
    }

    // move either left or right not both at the same time
    if (requestLeft ^ requestRight) {
        if (requestLeft) {
            loc.x = std::max(PLAYER_WIDTH / 2.f, loc.x - MOVEMENT_SPEED);
        }

        if (requestRight) {
            auto width = context->ScreenContext->GetWindowWidth();
            loc.x = std::min(width - PLAYER_WIDTH / 2.f, loc.x + MOVEMENT_SPEED);
        }
    }
}

/// Not used by the player.
void Player::LateUpdate(CMPUT350::GameContext* context)
{
}

/**
 * Converts input notifications into requests that Update() acts on:
 * held/released keys set or clear the movement flags, "Fire" queues one shot,
 * and "ResetImmunity" ends the post-hit immunity.
 */
void Player::ReceiveNotification(const std::string& key)
{
    if (key == "MoveLeftHeld") {
        requestLeft = true;
    }
    else if (key == "MoveLeftReleased") {
        requestLeft = false;
    }
    else if (key == "MoveRightHeld") {
        requestRight = true;
    }
    else if (key == "MoveRightReleased") {
        requestRight = false;
    }
    else if (key == "Fire") {
        requestFire = true;
    }
    else if (key == "ResetImmunity") {
        immune = false;
    }
}

/// Not used by the player.
void Player::RenderBackground(CMPUT350::GameContext* context)
{
    return;
}

/**
 * Draws each ship rectangle with its colour. While immune, the ship is only
 * drawn on alternating 5-frame intervals, making it blink.
 */
void Player::RenderForeground(CMPUT350::GameContext* context)
{
    if (immune) {
        if ((context->currentFrame / 5) % 2 == 1) {
            for (int i = 0; i < shapes.size(); i++) {
                context->ScreenContext->DrawRect(shapes[i].shape.rect, colors[i]);
            }
        }
    }
    else {
        for (int i = 0; i < shapes.size(); i++) {
            context->ScreenContext->DrawRect(shapes[i].shape.rect, colors[i]);
        }
    }
}

/**
 * Collision handling (skipped entirely while immune):
 *  - Enemy bullet: lose a life. The player's own bullets are ignored.
 *  - Living enemy ship: lose a life.
 * Afterwards, if no lives remain the player is killed; otherwise it becomes
 * immune until Galaga sends "ResetImmunity".
 *
 * Note: immunity is set after any non-immune collision, including with an
 * object that did not cause damage.
 */
void Player::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    if (immune) {
        return;
    }
    std::shared_ptr<Bullet> bullet = std::dynamic_pointer_cast<Bullet>(obj);
    std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(obj);

    if (bullet != nullptr) {
        if (bullet->IsPlayerBullet()) {
            return;
        }
        TakeDamage();
        immune = true;
    }
    if (enemy != nullptr && enemy->IsAlive()) {
        TakeDamage();
        immune = true;
    }
    if (lives == 0) {
        Kill();
        return;
    }
}

/// Flags the player as killed; Update() finishes the removal on the next frame.
void Player::Kill()
{
    died = true;
}

/// Returns false once the player has been removed.
bool Player::IsAlive() const
{
    return isAlive;
}

/**
 * Returns a 40x40 collision box centred on the ship, in local coordinates.
 *
 * Note: uses a function-local static, so the returned reference is only
 * valid until the next call.
 */
const CMPUT350::Rect& Player::GetBounds()
{
    static CMPUT350::Rect sBounds(0, 0, 0, 0);
    auto topLeft = CMPUT350::Point2D(0.f, 0.f) - CMPUT350::Point2D(PLAYER_WIDTH / 2.f, PLAYER_HEIGHT / 2.f);
    sBounds = CMPUT350::Rect(topLeft, PLAYER_WIDTH, PLAYER_HEIGHT); // player is 40x40 pixels
    return sBounds;
}

/// Returns the rectangles making up the ship drawing.
const std::vector<CMPUT350::Shape>& Player::GetShapes() {
    return shapes;
}

/// Returns the ship's current world position.
CMPUT350::Point2D Player::GetLocation() const {
    return loc;
}

/// The player ship is never rotated.
float Player::GetRotation() const {
    return 0.f;
}

/// Returns the number of remaining lives.
int Player::GetLives() const {
    return lives;
}

/// Removes one life. Death is handled in CollisionEnter().
void Player::TakeDamage() {
    lives--;
}

/// Returns true while the player is immune after being hit.
bool Player::IsImmune() {
    return immune;
}
