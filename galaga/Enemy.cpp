#include "Enemy.h"
#include "Bullet.h"
#include "Player.h"

#define ENEMY_WIDTH 40.f
#define ENEMY_HEIGHT 40.f
#define PIXELS_PER_FRAME 6.f
#define SWAY_DIST 10.f
#define SWAY_SPEED 0.05f
#define FIRE_NTH_FRAME 60

Enemy::Enemy(CMPUT350::Point2D formationLoc, const std::vector<CMPUT350::Point2D> &path) : formationLoc(formationLoc), path(path), t(0.f), isAlive(true), curve(path), onPath(true), attacking(false)
{
    loc = path[0];
    CMPUT350::Rect local(CMPUT350::Point2D(-ENEMY_WIDTH / 2.f, -ENEMY_HEIGHT / 2.f), ENEMY_WIDTH, ENEMY_HEIGHT);
    shapes.emplace_back(local);
}

void Enemy::Initialize(CMPUT350::GameContext* context)
{
}

bool Enemy::BeginAttack(const std::vector<CMPUT350::Point2D>& attackPath) {
    if (onPath) {
        return false;
    }
    path = attackPath;
    curve = CMPUT350::Bezier(path);
    t = 0.f;
    onPath = true;
    attacking = true;
    return true;
}

void Enemy::Update(CMPUT350::GameContext* context)
{
    if (!isAlive) {
        if (onPath) {
            context->mNotificationManger->Notify("EnemyDiedWhileMoving");
        }
        else {
            context->mNotificationManager->Notify("EnemyDied");
        }
        return;
    }
    // enemy is swaying while in formation
    if (!onPath) {
        loc = CMPUT350::Point2D(formationLoc.x + SWAY_DIST * std::sin(context->currentFrame * SWAY_SPEED), formationLoc.y);
        return;
    }

    auto s = curve.GetSlope(t);
    t += PIXELS_PER_FRAME / s.Distance(CMPUT350::Point2D(0.f, 0.f)); // increment t

    // moving along a path (either going into formation or attacking)
    if (t < (path.size() - 1) / 3.f) {
        loc = curve.GetPoint(t);
        if (attacking && context->currentFrame % FIRE_NTH_FRAME == 0) {
            std::shared_ptr<Bullet> bullet = std::make_shared<Bullet>(loc, CMPUT350::Point2D(0.f, 1.f), false, context->CurrObject);
            context->mEngineView->AddGameObject(bullet);
        }
    }
    // done attacking, so move back to formation
    else if (attacking) {
        // curve = (a new path that goes back to formation)
        t = 0.f;
        attacking = false;
    }
    else {
        onPath = false;
        loc = formationLoc;
    }
}

void Enemy::LateUpdate(CMPUT350::GameContext* context)
{
    return;
}

void Enemy::RenderBackground(CMPUT350::GameContext* context)
{
    return;
}

void Enemy::RenderForeground(CMPUT350::GameContext* context)
{
    context->ScreenContext->DrawRect(shapes[0].shape.rect, CMPUT350::Colors::red);
}

void Enemy::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    std::shared_ptr<Bullet> bullet = std::dynamic_pointer_cast<Bullet>(obj);
    std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(obj);

    if (bullet != nullptr) {
        if (!bullet->IsPlayerBullet()) {
            return;
        }
        if (--health == 0) {
            Kill();
        }
    }
    if (player != nullptr) {
        Kill();
    }
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
    auto topLeft = loc - CMPUT350::Point2D(ENEMY_WIDTH / 2.f, ENEMY_HEIGHT / 2.f);
    sBounds = CMPUT350::Rect(topLeft, ENEMY_WIDTH, ENEMY_HEIGHT);
    return sBounds;
}

const std::vector<CMPUT350::Shape>& Enemy::GetShapes() {
    return shapes;
}

CMPUT350::Point2D Enemy::GetLocation() const {
    return loc;
}

float Enemy::GetRotation() const {
    if (!onPath) {
        return 0.f;
    }
    auto heading = curve.GetSlope(t);
    return std::atan2f(heading.y, heading.x);
}

bool Enemy::InFormation() const {
    return !onPath;
}
