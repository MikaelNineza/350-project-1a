#include "Enemy.h"
#include "Bullet.h"
#include "Player.h"

Enemy::Enemy(CMPUT350::Point2D formationLoc, const std::vector<CMPUT350::Point2D> &path) : formationLoc(formationLoc), path(path), t(0.f), isAlive(true), curve(path), onPath(true), attacking(false), died(false), killedPlayer(false)
{
    loc = path[0];
    CMPUT350::Rect local(CMPUT350::Point2D(-ENEMY_WIDTH / 2.f, -ENEMY_HEIGHT / 2.f), ENEMY_WIDTH, ENEMY_HEIGHT);
    shapes.emplace_back(local);
}

void Enemy::Initialize(CMPUT350::GameContext* context)
{
    for (const auto obj : *context->mEngineView) {
        if (auto player = std::dynamic_pointer_cast<Player>(obj)) {
            playerLoc = player->GetLocation();
        }
    }
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
    if (died) {
        isAlive = false;
        if (killedPlayer) {
            return;
        }
        if (onPath) {
            context->mNotificationManager->Notify("EnemyDiedWhileMoving");
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
        path = BuildSideReturnPath(loc, formationLoc, playerLoc.y);
        curve = CMPUT350::Bezier(path);
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
        if (!bullet->IsPlayerBullet() || !bullet->IsAlive()) {
            return;
        }
        if (--health == 0) {
            Kill();
        }
    }
    if (player != nullptr) {
        killedPlayer = true;
        Kill();
    }
}

void Enemy::Kill()
{
    died = true;
}

bool Enemy::IsAlive() const
{
    return isAlive;
}

const CMPUT350::Rect& Enemy::GetBounds()
{
    static CMPUT350::Rect sBounds(0, 0, 0, 0);
    auto topLeft = CMPUT350::Point2D(0.f, 0.f) - CMPUT350::Point2D(ENEMY_WIDTH / 2.f, ENEMY_HEIGHT / 2.f);
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
    return std::atan2(heading.y, heading.x) + PI / 2.f; // 0 deg w.r.t 2d plane, adjusted to screencontext with pi/2 clockwise rotation
}

bool Enemy::InFormation() const {
    return !onPath;
}

void Enemy::SetHealth(int newHealth) {
    health = newHealth;
}

int Enemy::GetHealth() const {
    return health;
}

// this was ai generated
std::vector<CMPUT350::Point2D> Enemy::BuildSideReturnPath(
    const CMPUT350::Point2D& loc,
    const CMPUT350::Point2D& formationLoc,
    float playerY)
{
    using CMPUT350::Point2D;

    float sideX = (loc.x < 384.f) ? 50.f : 718.f;

    float topOffset = 80.f;

    return {
        loc,                                                    // P0: Current dive exit position
        Point2D((loc.x + sideX) * 0.5f, 1020.f),               // P1: Pull down/out toward bottom screen corner
        Point2D(sideX, (loc.y + formationLoc.y) * 0.5f),       // P2: Straighten path along vertical edge
        Point2D(sideX, formationLoc.y + topOffset),             // P3: Margin waypoint near formation height

        // --- Segment 2: Arc over the formation line and drop into slot ---
        Point2D(sideX, formationLoc.y - topOffset),             // P4: Continue upward past formation Y
        Point2D(formationLoc.x, formationLoc.y - topOffset),    // P5: Curve horizontally over formation column
        formationLoc                                            // P6: Final formation target position
    };
}
