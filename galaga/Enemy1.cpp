#include "Enemy1.h"

Enemy1::Enemy1(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D>& path) : Enemy(loc, path) {
    SetHealth(1);
}
bool Enemy1::Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) {
    CMPUT350::Point2D start = GetLocation();

    float screenW = context->GUIContext->GetWindowWidth();
    float side = 1.f;
    if (start.x > screenW / 2.f) {
        side = -1.f;
    }


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

    return BeginAttack(attackPath);
}

void Enemy1::RenderForeground(CMPUT350::GameContext *context) {
    CMPUT350::RGBColor color = GetHealth() >= 2 ? CMPUT350::Colors::yellow : CMPUT350::Colors::red;
    context->ScreenContext->DrawCircle(CMPUT350::Point2D(0.f, 0.f), ENEMY_WIDTH / 2.f, color);
    context->ScreenContext->DrawLine(CMPUT350::Point2D(0.f, 20.f), CMPUT350::Point2D(0.f, 0.f), 3.f, CMPUT350::Colors::blue);
}
