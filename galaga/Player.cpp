#include <cassert>
#include "Player.h"
#include "Bullet.h"

#define MOVEMENT_SPEED 3.0f // sample value

Player::Player(CMPUT350::Point2D loc) : loc(loc), isAlive(true)
{
}

void Player::Initialize(CMPUT350::GameContext* context)
{
    return;
}

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

    // add shooting cooldown (?)
}

void Player::LateUpdate(CMPUT350::GameContext* context)
{
}

bool Player::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    if (key == 'a') {
        loc.x -= std::min(MOVEMENT_SPEED, loc.x);
        return true;
    }
    if (key == 'd') {
        auto width = context->ScreenContext->GetWindowWidth();
        loc.x += std::min(MOVEMENT_SPEED, width - loc.x);
        return true;
    }
    if (key == ' ') {
        // shoot bullet
        if (bullets.size() == 2) {
            return false;
        }
        auto dir = CMPUT350::Point2D(0.0f, -1.0f); // bullet shoots straight up
        std::shared_ptr<Bullet> bullet = std::make_shared<Bullet>(loc, dir, true);
        context->mEngineView->AddGameObject(bullet);
        bullets.push_back(bullet);
        return true;
    }
    return false;
}

void Player::RenderBackground(CMPUT350::GameContext* context)
{
    return;
}

void Player::RenderForeground(CMPUT350::GameContext* context)
{
    // TODO: make it look better than just a rectangle
    context->ScreenContext->DrawRect(GetBounds(), CMPUT350::Colors::yellow);
}

void Player::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    return; // Bullet handles collision logic already
}

void Player::Kill()
{
    isAlive = false;
}

bool Player::IsAlive() const
{
    return isAlive;
}

const CMPUT350::Rect& Player::GetBounds()
{
    static CMPUT350::Rect sBounds(0, 0, 0, 0);
    auto topLeft = loc - 20;
    sBounds = CMPUT350::Rect(topLeft, 40, 40); // player is 40x40 pixels
    return sBounds;
}
