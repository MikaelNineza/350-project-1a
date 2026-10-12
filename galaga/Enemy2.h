//
// Created by User on 2026-10-10.
//

#ifndef GALAGA_ENEMY2_H
#define GALAGA_ENEMY2_H


class Enemy2 : private Enemy {
    Enemy2(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path)
        : Enemy(loc, path), health(2) {};
    void Initialize(CMPUT350::GameContext *context) override;
    void LateUpdate(CMPUT350::GameContext *context) override;
    bool Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) override;
    void RenderForeground(CMPUT350::GameContext *context) override;
};


#endif //GALAGA_ENEMY2_H
