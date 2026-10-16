/**
 * @file Enemy1.cpp
 * @brief Implementation of Enemy1 (cyan, 1 health, weaving attack).
 */
#include "Enemy1.h"
#include "Bullet.h"

/// Constructs the base enemy and gives it 1 health.
Enemy1::Enemy1(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D>& path) : Enemy(loc, path) {
    SetHealth(1);
}
/**
 * Builds a weaving attack path from the enemy's current position: it loops
 * up and toward the screen centre, then zig-zags down past the target
 * (±220 px either side of target.x), and finally drops straight down off the
 * bottom of the screen (y = 1100). `side` mirrors the path for enemies on the
 * right half of the screen.
 *
 * If the attack starts, the enemy gets 3 shots to fire during the dive.
 */
bool Enemy1::Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) {
    CMPUT350::Point2D start = GetLocation();

    // depending on which half of the screen, determine if enemy will go back to formation on lhs or rhs
    float screenW = context->GUIContext->GetWindowWidth();
    float side = 1.f;
    if (start.x > screenW / 2.f) {
        side = -1.f;
    }

    /*
     * The attack path was AI generated using Claude.
     * Prompt: Here is an example of an attack path (Enemy2.cpp) on a 768x1024 screen. Make one that is harder for the user to attack
     *
     * There is more variation in movement for Enemy1 as it has less health.
     */
    std::vector<CMPUT350::Point2D> attackPath = {
        start,
        CMPUT350::Point2D(start.x - side * 60.f,  start.y - 100.f),
        CMPUT350::Point2D(start.x - side * 150.f, start.y - 20.f),
        CMPUT350::Point2D(start.x - side * 150.f, start.y + 100.f),
        CMPUT350::Point2D(start.x - side * 150.f, start.y + 200.f),
        CMPUT350::Point2D(target.x + side * 220.f, start.y + 250.f),
        CMPUT350::Point2D(target.x + side * 220.f, start.y + 380.f),
        CMPUT350::Point2D(target.x + side * 220.f, start.y + 460.f),
        CMPUT350::Point2D(target.x - side * 220.f, start.y + 480.f),
        CMPUT350::Point2D(target.x - side * 220.f, start.y + 600.f),
        CMPUT350::Point2D(target.x - side * 220.f, start.y + 700.f),
        CMPUT350::Point2D(target.x, 950.f),
        CMPUT350::Point2D(target.x, 1100.f)
    };

    if (BeginAttack(attackPath)) {
        fireCount = 3;
        return true;
    }
    return false;
}

/// Draws each ship rectangle with its matching colour from `colors`.
void Enemy1::RenderForeground(CMPUT350::GameContext *context) {
    for (int i = 0; i < shapes.size(); i++) {
        context->ScreenContext->DrawRect(shapes[i].shape.rect, colors[i]);
    }
}

/**
 * Per-frame update, in priority order:
 *  1. If killed: mark as not alive and notify Galaga for scoring
 *     ("EnemyDiedWhileMoving" = double points if it was on a path,
 *     otherwise "EnemyDied").
 *  2. If in formation: sway horizontally around the formation slot.
 *  3. If on a path: advance t so the enemy moves ~PIXELS_PER_FRAME pixels,
 *     then either
 *       - keep moving (and fire every FIRE_NTH_FRAME frames while attacking,
 *         up to fireCount shots),
 *       - switch from the finished attack to the side return path, or
 *       - snap into the formation slot once the entrance/return path ends.
 *
 * A path with N control points has (N - 1) / 3 cubic segments, which is the
 * maximum value of t.
 */
void Enemy1::Update(CMPUT350::GameContext *context) {
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
        if (attacking && context->currentFrame % FIRE_NTH_FRAME == 0 && fireCount > 0) {
            std::shared_ptr<Bullet> bullet = std::make_shared<Bullet>(loc, CMPUT350::Point2D(0.f, 1.f), false, context->CurrObject);
            context->mEngineView->AddGameObject(bullet);
            fireCount--;
        }
    }
    // done attacking, so move back to formation
    else if (attacking) {
        path = BuildSideReturnPath(loc, formationLoc);
        curve = CMPUT350::Bezier(path);
        t = 0.f;
        attacking = false;
    }
    else {
        onPath = false;
        loc = formationLoc;
    }
}
