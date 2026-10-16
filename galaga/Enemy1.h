//
// Created by User on 2026-10-10.
//

#ifndef GALAGA_ENEMY1_H
#define GALAGA_ENEMY1_H

#include "Enemy.h"
#include "DrawContext.h"
#include <vector>

class Enemy1 : public Enemy {
public:
    Enemy1(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path);
    bool Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) override;
    void RenderForeground(CMPUT350::GameContext *context) override;
    void Update(CMPUT350::GameContext* context) override;
private:
    static constexpr int FIRE_NTH_FRAME = 12;
    int fireCount;
    inline static const std::vector<CMPUT350::RGBColor> colors = {
        CMPUT350::RGBColor(0, 255, 255),   // 0  nose tip
        CMPUT350::RGBColor(0, 255, 255),   // 1  upper fuselage
        CMPUT350::RGBColor(0, 255, 255),   // 2  main body
        CMPUT350::Colors::gray,            // 3  tail
        CMPUT350::RGBColor(0, 255, 255),   // 4  left inner wing
        CMPUT350::RGBColor(0, 255, 255),   // 5  right inner wing
        CMPUT350::Colors::blue,            // 6  left pod
        CMPUT350::Colors::blue,            // 7  right pod
        CMPUT350::Colors::white,           // 8  left pod tip
        CMPUT350::Colors::white,           // 9  right pod tip
        CMPUT350::Colors::gray,            // 10 left thruster
        CMPUT350::Colors::gray,            // 11 right thruster
        CMPUT350::Colors::blue,            // 12 cockpit
        CMPUT350::Colors::white            // 13 cockpit highlight
    };
};


#endif //GALAGA_ENEMY1_H
