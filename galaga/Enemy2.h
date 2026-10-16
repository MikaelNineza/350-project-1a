//
// Created by User on 2026-10-10.
//

/**
 * @file Enemy2.h
 * @brief Red, 2-health enemy with a simpler swooping attack.
 */
#ifndef GALAGA_ENEMY2_H
#define GALAGA_ENEMY2_H

#include "Enemy.h"
#include "DrawContext.h"
#include <vector>



/**
 * @class Enemy2
 * @brief Tougher enemy (2 health) with a smooth swooping attack that fires
 *        continuously while diving.
 *
 * Drawn red at full health and magenta after taking one hit. Used for the
 * first and last waves of each level.
 */
class Enemy2 : public Enemy {
public:
    /**
     * @brief Creates an Enemy2 with 2 health.
     * @param loc  Formation slot.
     * @param path Entrance path control points (last point is the formation slot).
     */
    Enemy2(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path);
    /**
     * @brief Starts a swooping dive toward the target.
     * @return true if the attack started, false if the enemy was not in formation.
     */
    bool Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) override;
    /// Draws the ship red at full health, magenta once damaged.
    void RenderForeground(CMPUT350::GameContext *context) override;
    /**
     * @brief Per-frame logic: handles death, sways in formation, follows the
     *        current path, fires during attacks, and switches to the return
     *        path once the attack finishes.
     */
    void Update(CMPUT350::GameContext* context) override;
private:
    /// While attacking, fire on frames that are a multiple of this value (no shot limit).
    static constexpr int FIRE_NTH_FRAME = 30;
    /// Colour table used at full health (index matches Enemy::shapes).
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
    /// Colour table used after the ship has been hit once (index matches Enemy::shapes).
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