#include "Enemy2.h"

Enemy2::Enemy2(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D>& path) : Enemy(loc, path) {
    SetHealth(2);
}
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

void Enemy2::RenderForeground(CMPUT350::GameContext *context) {
    CMPUT350::RGBColor color = GetHealth() >= 2 ? CMPUT350::Colors::yellow : CMPUT350::Colors::red;
    context->ScreenContext->DrawCircle(CMPUT350::Point2D(0.f, 0.f), ENEMY_WIDTH / 2.f, color);
    context->ScreenContext->DrawLine(CMPUT350::Point2D(0.f, 20.f), CMPUT350::Point2D(0.f, 0.f), 3.f, CMPUT350::Colors::blue);
}
