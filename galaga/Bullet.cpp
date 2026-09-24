#include "Bullet.h"
#define BULLET_SPEED 4.0f // testing

Bullet::Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player) : location(location), prev_location(location), heading(heading), player(player), isAlive(true)
{
}

bool Bullet::IsPlayerBullet()
{
    return player;
}

void Bullet::Initialize(CMPUT350::GameContext* context)
{
    return;
}

void Bullet::Update(CMPUT350::GameContext* context)
{
    // track prev location for bullet bound calc
    prev_location = location;
    location += heading * BULLET_SPEED;
}

void Bullet::LateUpdate(CMPUT350::GameContext* context)
{
    int screen_width = context->ScreenContext->GetWindowWidth();
    int screen_height = context->ScreenContext->GetWindowHeight();
    if (location.x <= 0 || location.x >= screen_width || location.y <= 0 || location.y >= screen_height) {
        Kill();
    }
}

bool Bullet::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    return false;
}

void Bullet::RenderBackground(CMPUT350::GameContext* context)
{
    return;
}

void Bullet::RenderForeground(CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect(GetBounds(), CMPUT350::Colors::yellow);
}

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
    CMPUT350::Rect empty(0, 0, 0, 0);
    sBounds = empty;
    sBounds |= prev_location;
    sBounds |= location;
    return sBounds;
}
