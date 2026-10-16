/**
 * @file Enemy.cpp
 * @brief Implementation of the shared Enemy base class.
 */
#include "Enemy.h"
#include "Bullet.h"
#include "Player.h"

/**
 * Places the enemy at the start of its entrance path, marks it as moving
 * along that path, and builds the ship's rectangles. The rectangles are in
 * local coordinates centred on the enemy and fit inside the 40x40 bounds.
 * Their order matches the colour tables in Enemy1/Enemy2.
 */
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

/// No setup required.
void Enemy::Initialize(CMPUT350::GameContext* context)
{
}

/**
 * Replaces the current path with the attack path and restarts at t = 0.
 * Refused (returns false) if the enemy is already moving along a path,
 * i.e. still entering, attacking, or returning to formation.
 */
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

/// Base update does nothing; see Enemy1::Update / Enemy2::Update.
void Enemy::Update(CMPUT350::GameContext* context)
{
    return;
}

/// Not used by enemies.
void Enemy::LateUpdate(CMPUT350::GameContext* context)
{
    return;
}

/// Not used by enemies.
void Enemy::RenderBackground(CMPUT350::GameContext* context)
{
    return;
}

/// Base render does nothing; subclasses draw the ship with their colours.
void Enemy::RenderForeground(CMPUT350::GameContext* context)
{
    return;
}

/**
 * Collision handling:
 *  - Player bullet: lose one health; die at zero health.
 *  - Enemy bullet: ignored (no friendly fire).
 *  - Player ship: die, unless the player is currently immune.
 */
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
        if (player->IsImmune()) {
            return;
        }
        Kill();
    }
}

/**
 * Flags the enemy as killed. The subclass Update() sees this on the next
 * frame, sends the score notification and sets isAlive to false.
 */
void Enemy::Kill()
{
    died = true;
}

/// Returns false once the enemy has been removed.
bool Enemy::IsAlive() const
{
    return isAlive;
}

/**
 * Returns a 40x40 collision box centred on the enemy, in local coordinates.
 *
 * Note: uses a function-local static, so the returned reference is shared by
 * all enemies and is only valid until the next call.
 */
const CMPUT350::Rect& Enemy::GetBounds()
{
    static CMPUT350::Rect sBounds(0, 0, 0, 0);
    auto topLeft = CMPUT350::Point2D(0.f, 0.f) - CMPUT350::Point2D(ENEMY_WIDTH / 2.f, ENEMY_HEIGHT / 2.f);
    sBounds = CMPUT350::Rect(topLeft, ENEMY_WIDTH, ENEMY_HEIGHT);
    return sBounds;
}

/// Returns the rectangles making up the ship drawing.
const std::vector<CMPUT350::Shape>& Enemy::GetShapes() {
    return shapes;
}

/// Returns the enemy's current world position.
CMPUT350::Point2D Enemy::GetLocation() const {
    return loc;
}

/**
 * While on a path, rotates the ship to face along the curve's tangent at t.
 * The ship is drawn nose-up, so pi/2 is added to convert from the math angle
 * to the screen's orientation. In formation the ship is not rotated.
 */
float Enemy::GetRotation() const {
    if (!onPath) {
        return 0.f;
    }
    auto heading = curve.GetSlope(t);
    return std::atan2(heading.y, heading.x) + PI / 2.f; // 0 deg w.r.t 2d plane, adjusted to screencontext with pi/2 clockwise rotation
}

/// True when the enemy is resting in its slot rather than following a path.
bool Enemy::InFormation() const {
    return !onPath;
}

/// Sets the enemy's hit points.
void Enemy::SetHealth(int newHealth) {
    health = newHealth;
}

/// Returns the enemy's remaining hit points.
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
/**
 * Builds the 7 control points (two cubic segments) of the return path.
 * The side is chosen from which half of the 768-wide screen the attack ended
 * in: x = 50 for the left edge, x = 718 for the right edge.
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
