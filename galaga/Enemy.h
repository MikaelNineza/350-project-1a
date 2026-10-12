#ifndef ENEMY_H
#define ENEMY_H

#include "CollisionObject.h"
#include "GameContext.h"

class Enemy : public CMPUT350::CollisionObject
{
public:
    Enemy(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D &path);

    // GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    bool IsAlive() const override;
    void Kill() override;

    // Graphics Object Functions
    void RenderBackground(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;


    // Collision Object Functions
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;
    const CMPUT350::Rect& GetBounds() override;

    virtual bool Attack(CMPUT350::GameContext* context, const CMPUT350::Point2D& target) = 0;
    void ReceiveNotification(const std::string& key) override;
    const std::vector<CMPUT350::Shape>& GetShapes() override;
    CMPUT350::Point2D GetLocation() const override;
    float GetRotation() const override;

private:
    CMPUT350::Point2D loc;
    bool isAlive;
    int health;
    std::vector<CMPUT350::Point2D> path;
    float t;
    GameContext* context;
};


#endif

