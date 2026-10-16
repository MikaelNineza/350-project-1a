#ifndef PLAYER_H
#define PLAYER_H

#include "CollisionObject.h"
#include "DrawContext.h"
#include <vector>
#include <memory>
#include <string>

class Player : public CMPUT350::CollisionObject
{
public:
    Player(CMPUT350::Point2D loc);

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

    void ReceiveNotification(const std::string& key) override;
    const std::vector<CMPUT350::Shape>& GetShapes() override;
    const CMPUT350::Rect& GetBounds() override;
    CMPUT350::Point2D GetLocation() const override;
    float GetRotation() const override;
    int GetLives() const;
    void TakeDamage();
    bool IsImmune();

private: 
    CMPUT350::Point2D loc;
    bool isAlive;
    std::vector<std::weak_ptr<CMPUT350::CollisionObject>> bullets;
    std::vector<CMPUT350::Shape> shapes;
    bool requestFire;
    bool requestLeft;
    bool requestRight;
    int lives;
    bool died;
    std::vector<CMPUT350::RGBColor> colors;
    bool immune;
};

#endif
