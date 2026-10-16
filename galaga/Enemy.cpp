#include "Enemy.h"
#include "Bullet.h"
#include "Player.h"

Enemy::Enemy(CMPUT350::Point2D formationLoc, const std::vector<CMPUT350::Point2D> &path) : formationLoc(formationLoc), path(path), t(0.f), isAlive(true), curve(path), onPath(true), attacking(false), died(false), killedPlayer(false)
{
    loc = path[0];
    shapes.clear();

    // --- fuselage (center column) ---
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(-2.f, -20.f), 4.f, 6.f));    // 0  nose tip
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(-4.f, -14.f), 8.f, 8.f));    // 1  upper fuselage
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(-6.f, -6.f), 12.f, 14.f));   // 2  main body
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(-4.f, 8.f), 8.f, 6.f));      // 3  tail

    // --- inner wings ---
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(-14.f, -2.f), 8.f, 12.f));   // 4  left inner wing
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(6.f, -2.f), 8.f, 12.f));     // 5  right inner wing

    // --- outer wing pods ---
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(-20.f, -6.f), 6.f, 18.f));   // 6  left pod
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(14.f, -6.f), 6.f, 18.f));    // 7  right pod
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(-20.f, -10.f), 6.f, 4.f));   // 8  left pod tip
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(14.f, -10.f), 6.f, 4.f));    // 9  right pod tip

    // --- rear thrusters ---
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(-8.f, 10.f), 4.f, 8.f));     // 10 left thruster
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(4.f, 10.f), 4.f, 8.f));      // 11 right thruster

    // --- cockpit detail ---
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(-2.f, -10.f), 4.f, 8.f));    // 12 cockpit (draw on top of the fuselage)
    shapes.emplace_back(CMPUT350::Rect(CMPUT350::Point2D(-1.f, -4.f), 2.f, 4.f));     // 13 cockpit highlight
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
    return;
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
    return;
}

void Enemy::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    std::shared_ptr<Bullet> bullet = std::dynamic_pointer_cast<Bullet>(obj);
    std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(obj);

    if (bullet != nullptr) {
        if (!bullet->IsPlayerBullet()) {
            return;
        }
        if (--health <= 0) {
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

/*
 * This helper method was AI generated using Gemini.
 * Prompt: The screen is 768x1024, the current return path is done poorly. Make a curve that a galaga enemy would travel along after completing an attack and going back to formation.
 * Make it so the curve just travels along the edge of the screen, either left or right, and then moderately go back to formation.
 * The end of the attack is at "loc" and the formation location is "formationLoc", assume that these are given.
 *
 * A sample path that didn't work as well was previously provided with the prompt as a reference. Gemini gave a helper instead of a fixed set of points.
 */
std::vector<CMPUT350::Point2D> Enemy::BuildSideReturnPath(
    const CMPUT350::Point2D& loc,
    const CMPUT350::Point2D& formationLoc)
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
