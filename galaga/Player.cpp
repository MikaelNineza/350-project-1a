#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"

#define MOVEMENT_SPEED 15.f
#define PLAYER_WIDTH 40.f
#define PLAYER_HEIGHT 40.f

Player::Player(CMPUT350::Point2D loc) : loc(loc), isAlive(true), requestFire(false), requestLeft(false), requestRight(false), lives(2), died(false)
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
    for (int i = 0; i < shapes.size(); i++) {
        context->ScreenContext->DrawRect(shapes[i].shape.rect, colors[i]);
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
    auto topLeft = CMPUT350::Point2D(0.f, 0.f) - CMPUT350::Point2D(PLAYER_WIDTH / 2.f, PLAYER_HEIGHT / 2.f);
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
