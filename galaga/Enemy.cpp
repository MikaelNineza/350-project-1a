#include "Enemy.h"
#include "Bullet.h"

Enemy::Enemy(CMPUT350::Point2D loc) : loc(loc), isAlive(true)
{
}

void Enemy::Initialize(CMPUT350::GameContext* context)
{
    return;
}

void Enemy::Update(CMPUT350::GameContext* context)
{
    return;
}

void Enemy::LateUpdate(CMPUT350::GameContext* context)
{
    return;
}

bool Enemy::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    return false;
}

void Enemy::RenderBackground(CMPUT350::GameContext* context)
{
    return;
}

void Enemy::RenderForeground(CMPUT350::GameContext* context)
{
    // TODO: make it look better than just a rectangle
    context->ScreenContext->DrawRect(GetBounds(), CMPUT350::Colors::red);
}

void Enemy::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    return;
}

void Enemy::Kill()
{
    isAlive = false;
}

bool Enemy::IsAlive() const
{
    return isAlive;
}

const CMPUT350::Rect& Enemy::GetBounds()
{
    static CMPUT350::Rect sBounds(0, 0, 0, 0);
    auto topLeft = loc - 20;
    sBounds = CMPUT350::Rect(topLeft, 40, 40); // enemy is 40x40 pixels (?) sample
    return sBounds;
}
