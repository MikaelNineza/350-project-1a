#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"

#define MOVEMENT_SPEED 15.f
#define PLAYER_WIDTH 40.f
#define PLAYER_HEIGHT 40.f

Player::Player(CMPUT350::Point2D loc) : loc(loc), isAlive(true), requestFire(false), requestLeft(false), requestRight(false), lives(2), died(false)
{
    CMPUT350::Rect local(CMPUT350::Point2D(-PLAYER_WIDTH / 2.f, -PLAYER_HEIGHT / 2.f), PLAYER_WIDTH, PLAYER_HEIGHT);
    auto left_wing = local;
    left_wing.topLeft.y += left_wing.height / 2.f;
    left_wing.width /= 4.f;
    left_wing.height /= 2.f;

    auto main_body = local;
    main_body.Inset(main_body.width / 4.f);

    auto right_wing = local;
    right_wing.topLeft.y += right_wing.height / 2.f;
    right_wing.topLeft.x += right_wing.width * 0.75f;
    right_wing.width /= 4.f;
    right_wing.height /= 2.f;

    auto tip = local;
    tip.topLeft.y -= tip.height * 0.375f;
    tip.Inset(tip.height * 0.375f);
    shapes.clear();
    shapes.emplace_back(left_wing);
    shapes.emplace_back(main_body);
    shapes.emplace_back(right_wing);
    shapes.emplace_back(tip);
}

void Player::Initialize(CMPUT350::GameContext* context)
{
    context->mNotificationManager->Register(context->CurrObject, "MoveLeftHeld");
    context->mNotificationManager->Register(context->CurrObject, "MoveLeftReleased");
    context->mNotificationManager->Register(context->CurrObject, "MoveRightHeld");
    context->mNotificationManager->Register(context->CurrObject, "MoveRightReleased");
    context->mNotificationManager->Register(context->CurrObject, "Fire");
}

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
        if (bullets.size() < 2) {
            auto dir = CMPUT350::Point2D(0.0f, -1.0f); // bullet shoots straight up
            auto start_pos = loc;
            start_pos.y -= PLAYER_HEIGHT / 2.f; // bullets spawn at the top of the player
            std::shared_ptr<Bullet> bullet = std::make_shared<Bullet>(start_pos, dir, true, context->CurrObject);
            context->mEngineView->AddGameObject(bullet);
            bullets.push_back(bullet);
        }
    }

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

void Player::LateUpdate(CMPUT350::GameContext* context)
{
}

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
}

void Player::RenderBackground(CMPUT350::GameContext* context)
{
    return;
}

void Player::RenderForeground(CMPUT350::GameContext* context)
{
    for (const auto& s : shapes) {
        context->ScreenContext->DrawRect(s.shape.rect, CMPUT350::Colors::gray);
    }
}

void Player::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    std::shared_ptr<Bullet> bullet = std::dynamic_pointer_cast<Bullet>(obj);
    std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(obj);

    if (bullet != nullptr) {
        if (bullet->IsPlayerBullet()) {
            return;
        }
        TakeDamage();
    }
    if (enemy != nullptr) {
        TakeDamage();
    }
    if (lives == 0) {
        Kill();
    }
}

void Player::Kill()
{
    died = true;
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

const std::vector<CMPUT350::Shape>& Player::GetShapes() {
    return shapes;
}

CMPUT350::Point2D Player::GetLocation() const {
    return loc;
}

float Player::GetRotation() const {
    return 0.f;
}

int Player::GetLives() const {
    return lives;
}

void Player::TakeDamage() {
    lives--;
}
