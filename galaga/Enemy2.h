//
// Created by User on 2026-10-10.
//

#ifndef GALAGA_ENEMY2_H
#define GALAGA_ENEMY2_H

#include "Enemy.h"

class Enemy2 : public Enemy {
public:
    Enemy2(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path);
    bool Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) override;
    void RenderForeground(CMPUT350::GameContext *context) override;
};

#endif //GALAGA_ENEMY2_H