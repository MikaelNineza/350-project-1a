/**
 * @file Enemy2.cpp
 * @brief Implementation of Enemy2 (red, 2 health, swooping attack).
 */
#include "Enemy2.h"
#include "Bullet.h"

/// Constructs the base enemy and gives it 2 health.
Enemy2::Enemy2(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D>& path) : Enemy(loc, path) {
    SetHealth(2);
}
/**
 * Builds a swooping attack path: a loop out to the right of the start
 * position, a pass through target.x, a swing 200 px to the left of the
 * target, and then a straight drop off the bottom of the screen (y = 1100).
 */
bool Enemy2::Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) {
    CMPUT350::Point2D start = GetLocation();

    std::vector<CMPUT350::Point2D> attackPath = {
        start,
        CMPUT350::Point2D(start.x + 200.f, start.y - 50.f),
        CMPUT350::Point2D(start.x + 250.f, start.y + 250.f),
        CMPUT350::Point2D(target.x, start.y + 400.f),
        CMPUT350::Point2D(target.x - 200.f, start.y + 550.f),
        CMPUT350::Point2D(target.x, 950.f),
        CMPUT350::Point2D(target.x, 1100.f)
    };
    return BeginAttack(attackPath);
}

/// Draws each ship rectangle using the red table at full health, magenta once damaged.
void Enemy2::RenderForeground(CMPUT350::GameContext *context) {
    auto &colors = health > 1 ? redShip : magentaShip;
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
 *       - keep moving (and fire every FIRE_NTH_FRAME frames while attacking),
 *       - switch from the finished attack to the side return path, or
 *       - snap into the formation slot once the entrance/return path ends.
 */
void Enemy2::Update(CMPUT350::GameContext *context) {
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