/**
 * @file Bullet.cpp
 * @brief Implementation of the Bullet projectile.
 */
#include "Bullet.h"
#include "Enemy.h"
#include "Player.h"
#include <vector>
#include <cmath>

/**
 * Stores the starting state and builds the bullet's single line shape,
 * which runs from the origin (the bullet's location) one frame's travel
 * distance along the heading.
 */
Bullet::Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player,
               std::weak_ptr<CMPUT350::GameObject> parent)
    : location(location), prev_location(location), heading(heading), player(player), isAlive(true), parent(parent) {
    CMPUT350::Point2D d = heading * BULLET_SPEED;
    shapes.emplace_back(CMPUT350::Line(d.x, d.y, 0.f, 0.f));
}

/// Returns whether the player (true) or an enemy (false) fired this bullet.
bool Bullet::IsPlayerBullet() {
    return player;
}

/// No setup required for bullets.
void Bullet::Initialize(CMPUT350::GameContext *context) {
}

/// Advances the bullet by BULLET_SPEED pixels along its heading.
void Bullet::Update(CMPUT350::GameContext *context) {
    // track prev location for bullet bound calc
    prev_location = location;
    location += heading * BULLET_SPEED;
}

/**
 * Runs after all objects have updated. Kills the bullet if it is on or past
 * any screen edge, or if the object that fired it has been destroyed.
 */
void Bullet::LateUpdate(CMPUT350::GameContext *context) {
    // kill bullet if it's out of bounds
    int screen_width = context->ScreenContext->GetWindowWidth();
    int screen_height = context->ScreenContext->GetWindowHeight();
    if (location.x <= 0 || location.x >= screen_width || location.y <= 0 || location.y >= screen_height) {
        Kill();
    }

    // kill the bullet if its parent died
    if (parent.expired()) {
        Kill();
    }
}

/// Bullets have nothing to draw in the background layer.
void Bullet::RenderBackground(CMPUT350::GameContext *context) {
    return;
}

/**
 * Draws a live bullet as a line (white = player, yellow = enemy) in local
 * coordinates. A bullet that was killed this frame is drawn as a small yellow
 * circle to show the impact.
 */
void Bullet::RenderForeground(CMPUT350::GameContext* context)
{
    if (isAlive) {
        CMPUT350::Point2D d = heading * BULLET_SPEED;
        auto colour = player ? CMPUT350::Colors::white : CMPUT350::Colors::yellow;
        context->ScreenContext->DrawLine(CMPUT350::Point2D(0.f, 0.f), d, BULLET_SIZE, colour);
    }
    else {
        context->ScreenContext->DrawCircle(CMPUT350::Point2D(0.f, 0.f), BULLET_SIZE, CMPUT350::Colors::yellow);
    }
}

/**
 * Kills the bullet when it touches an opposing object. Player bullets ignore
 * everything except Enemy objects; enemy bullets ignore everything except the
 * Player. Damage is applied by the target in its own CollisionEnter.
 */
void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject> &obj) {
    // if target is hit, kill the bullet
    if (player) {
        std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(obj);
        if (enemy == nullptr) {
            return;
        }
        Kill();
    } else {
        std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(obj);
        if (player == nullptr) {
            return;
        }
        Kill();
    }
}

/// Marks the bullet dead; the engine removes it once IsAlive() returns false.
void Bullet::Kill() {
    isAlive = false;
}

/// Returns false once the bullet has been killed.
bool Bullet::IsAlive() const {
    return isAlive;
}

/**
 * Builds the collision box in local coordinates: a BULLET_SIZE square around
 * the origin, unioned with a square around the other end of the bullet line,
 * so the box covers the whole line.
 *
 * Note: uses a function-local static, so the returned reference is shared by
 * all bullets and is only valid until the next call.
 */
const CMPUT350::Rect &Bullet::GetBounds() {
    static CMPUT350::Rect sBounds(0, 0, 0, 0);

    auto topLeft = CMPUT350::Point2D(0.f, 0.f) - BULLET_SIZE / 2.f;
    auto topLeftPrev = heading * BULLET_SPEED - BULLET_SIZE / 2.f;
    sBounds = CMPUT350::Rect(topLeft, BULLET_SIZE, BULLET_SIZE);
    sBounds |= CMPUT350::Rect(topLeftPrev, BULLET_SIZE, BULLET_SIZE);

    return sBounds;
}

/// Returns the bullet's shapes (one line).
const std::vector<CMPUT350::Shape> &Bullet::GetShapes() {
    return shapes;
}

/// Returns the bullet's current world position.
CMPUT350::Point2D Bullet::GetLocation() const {
    return location;
}

/// Bullets are not rotated; the line already points along the heading.
float Bullet::GetRotation() const {
    //return std::atan2(heading.y, heading.x) + PI / 2.f;
    return 0.f;
}

/// Bullets do not subscribe to notifications.
void Bullet::ReceiveNotification(const std::string &key) {
}
