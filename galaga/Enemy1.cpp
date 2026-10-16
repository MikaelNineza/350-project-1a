#include "Enemy1.h"
#include "Bullet.h"

Enemy1::Enemy1(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D>& path) : Enemy(loc, path) {
    SetHealth(1);
}
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

void Enemy1::RenderForeground(CMPUT350::GameContext *context) {
    for (int i = 0; i < shapes.size(); i++) {
        context->ScreenContext->DrawRect(shapes[i].shape.rect, colors[i]);
    }
}

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
