//
// Created by User on 2026-10-10.
//

#ifndef GALAGA_ENEMY2_H
#define GALAGA_ENEMY2_H

#include "Enemy.h"
#include "DrawContext.h"
#include <vector>



class Enemy2 : public Enemy {
public:
    Enemy2(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path);
    bool Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) override;
    void RenderForeground(CMPUT350::GameContext *context) override;
    void Update(CMPUT350::GameContext* context) override;
private:
    static constexpr int FIRE_NTH_FRAME = 30;
    inline static const std::vector<CMPUT350::RGBColor> redShip = {
        CMPUT350::Colors::red,     // 0  nose tip
        CMPUT350::Colors::red,     // 1  upper fuselage
        CMPUT350::Colors::red,     // 2  main body
        CMPUT350::Colors::gray,    // 3  tail
        CMPUT350::Colors::red,     // 4  left inner wing
        CMPUT350::Colors::red,     // 5  right inner wing
        CMPUT350::Colors::gray,    // 6  left pod
        CMPUT350::Colors::gray,    // 7  right pod
        CMPUT350::Colors::white,   // 8  left pod tip
        CMPUT350::Colors::white,   // 9  right pod tip
        CMPUT350::Colors::gray,    // 10 left thruster
        CMPUT350::Colors::gray,    // 11 right thruster
        CMPUT350::Colors::blue,    // 12 cockpit
        CMPUT350::Colors::white    // 13 cockpit highlight
    };
    inline static const std::vector<CMPUT350::RGBColor> magentaShip = {
        CMPUT350::RGBColor(255, 0, 255),   // 0  nose tip
        CMPUT350::RGBColor(255, 0, 255),   // 1  upper fuselage
        CMPUT350::RGBColor(255, 0, 255),   // 2  main body
        CMPUT350::Colors::white,           // 3  tail
        CMPUT350::RGBColor(255, 0, 255),   // 4  left inner wing
        CMPUT350::RGBColor(255, 0, 255),   // 5  right inner wing
        CMPUT350::Colors::gray,            // 6  left pod
        CMPUT350::Colors::gray,            // 7  right pod
        CMPUT350::Colors::white,           // 8  left pod tip
        CMPUT350::Colors::white,           // 9  right pod tip
        CMPUT350::Colors::gray,            // 10 left thruster
        CMPUT350::Colors::gray,            // 11 right thruster
        CMPUT350::Colors::blue,            // 12 cockpit
        CMPUT350::Colors::white            // 13 cockpit highlight
    };
};

#endif //GALAGA_ENEMY2_H