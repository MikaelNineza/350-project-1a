//
// Created by User on 2026-10-10.
//

/**
 * @file Enemy1.h
 * @brief Cyan, 1-health enemy with an erratic weaving attack.
 */
#ifndef GALAGA_ENEMY1_H
#define GALAGA_ENEMY1_H

#include "Enemy.h"
#include "DrawContext.h"
#include <vector>

/**
 * @class Enemy1
 * @brief Weaker enemy (1 health) that compensates with a harder-to-hit,
 *        side-to-side attack path and a short burst of 3 shots per attack.
 *
 * Used for the middle waves of each level.
 */
class Enemy1 : public Enemy {
public:
    /**
     * @brief Creates an Enemy1 with 1 health.
     * @param loc  Formation slot.
     * @param path Entrance path control points (last point is the formation slot).
     */
    Enemy1(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path);
    /**
     * @brief Starts a weaving dive toward the target and loads 3 shots.
     *
     * The weave direction is mirrored based on which half of the screen the
     * enemy starts in, so it always swings toward the screen centre first.
     *
     * @return true if the attack started, false if the enemy was not in formation.
     */
    bool Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) override;
    /// Draws the ship's rectangles using the cyan/blue colour table.
    void RenderForeground(CMPUT350::GameContext *context) override;
    /**
     * @brief Per-frame logic: handles death, sways in formation, follows the
     *        current path, fires during attacks, and switches to the return
     *        path once the attack finishes.
     */
    void Update(CMPUT350::GameContext* context) override;
private:
    /// While attacking, fire on frames that are a multiple of this value.
    static constexpr int FIRE_NTH_FRAME = 12;
    /// Shots left in the current attack (reset to 3 when an attack starts).
    int fireCount;
    /// Colour for each rectangle in Enemy::shapes (same order/index).
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
