#include "Bullet.h"
#include "Enemy.h"
#include "Player.h"
#include <vector>
#include <cmath>

#define BULLET_SPEED 20.0f
#define BULLET_SIZE 5.f

Bullet::Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player, std::weak_ptr<CMPUT350::GameObject> parent)
                : location(location), prev_location(location), heading(heading), player(player), isAlive(true), parent(parent)
{
    shapes.emplace_back(CMPUT350::Line(-heading.x * BULLET_SPEED, -heading.y * BULLET_SPEED, 0.f, 0.f));
}

bool Bullet::IsPlayerBullet()
{
    return player;
}

void Bullet::Initialize(CMPUT350::GameContext* context)
{

}

void Bullet::Update(CMPUT350::GameContext* context)
{
    // track prev location for bullet bound calc
    prev_location = location;
    location += heading * BULLET_SPEED;
}

void Bullet::LateUpdate(CMPUT350::GameContext* context)
{
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

void Bullet::RenderBackground(CMPUT350::GameContext* context)
{
    return;
}

void Bullet::RenderForeground(CMPUT350::GameContext* context)
{
    // bullet rendered as a line
    context->ScreenContext->DrawLine(prev_location, location, BULLET_SIZE, CMPUT350::Colors::white);
}

void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    // if target is hit, kill the bullet
    if (player) {
        std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(obj);
        if (enemy == nullptr) {
            return;
        }
        Kill();
    }
    else {
        std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(obj);
        if (player == nullptr) {
            return;
        }
        Kill();
    }
}

void Bullet::Kill()
{
    isAlive = false;
}

bool Bullet::IsAlive() const
{
    return isAlive;
}

const CMPUT350::Rect& Bullet::GetBounds()
{
    static CMPUT350::Rect sBounds(0, 0, 0, 0);

    auto topLeft = location - BULLET_SIZE / 2.f;
    auto topLeftPrev = prev_location - BULLET_SIZE / 2.f;
    sBounds = CMPUT350::Rect(topLeft, BULLET_SIZE, BULLET_SIZE);
    sBounds |= CMPUT350::Rect(topLeftPrev, BULLET_SIZE, BULLET_SIZE);
    
    return sBounds;
}

const std::vector<CMPUT350::Shape>& Bullet::GetShapes() {
    return shapes;
}

CMPUT350::Point2D Bullet::GetLocation() const {
    return location;
}

float Bullet::GetRotation() const {
    return std::atan2(heading.y, heading.x);
}

void Bullet::ReceiveNotification(const std::string& key) {

}
