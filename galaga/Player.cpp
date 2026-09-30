#include <cassert>
#include "Player.h"
#include "Bullet.h"

#define MOVEMENT_SPEED 15.f
#define PLAYER_WIDTH 40.f
#define PLAYER_HEIGHT 40.f

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
}

void Player::LateUpdate(CMPUT350::GameContext* context)
{
}

bool Player::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    // player moves left
    if (key == 'a') {
        loc.x -= std::min(MOVEMENT_SPEED, loc.x);
        return true;
    }
    // player moves right
    if (key == 'd') {
        auto width = context->ScreenContext->GetWindowWidth();
        loc.x += std::min(MOVEMENT_SPEED, width - loc.x);
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

void Player::RenderBackground(CMPUT350::GameContext* context)
{
    return;
}

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
    auto topLeft = loc - CMPUT350::Point2D(PLAYER_WIDTH / 2.f, PLAYER_HEIGHT / 2.f);
    sBounds = CMPUT350::Rect(topLeft, PLAYER_WIDTH, PLAYER_HEIGHT); // player is 40x40 pixels
    return sBounds;
}
