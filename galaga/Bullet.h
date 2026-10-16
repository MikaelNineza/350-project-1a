#ifndef BULLET_H
#define BULLET_H

#include "CollisionObject.h"
#include "GameContext.h"

constexpr float BULLET_SPEED = 20.f;
constexpr float BULLET_SIZE = 5.f;
class Bullet : public CMPUT350::CollisionObject
{
public:
    Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player, std::weak_ptr<CMPUT350::GameObject> parent);
    bool IsPlayerBullet();

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

private:
    CMPUT350::Point2D location;
    CMPUT350::Point2D prev_location;
    CMPUT350::Point2D heading;
    bool player;
    bool isAlive;
    std::weak_ptr<CMPUT350::GameObject> parent;
    std::vector<CMPUT350::Shape> shapes;
};
#endif // BULLET_H
