#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>
namespace CMPUT350 {

class GameContext;

/**
 * @brief Base class for everything the engine updates each frame.
 *
 * Every method has an empty default, so subclasses override only what they need. The engine
 * calls them in this order each frame: Initialize() (once, the frame after the object is
 * added), Update(), LateUpdate(), then RenderUI(). See GameEngine::Run().
 */
class GameObject {
public:
    virtual ~GameObject() = default;
    virtual void Initialize(GameContext *context);
    virtual void Update(GameContext *context);
    virtual void LateUpdate(GameContext *context);
    virtual void RenderUI(GameContext *context);
    virtual bool IsAlive() const;
    virtual void Kill();
    virtual void ReceiveNotification(const std::string& key);
private:
    bool mIsAlive = true;
};

}  // namespace CMPUT350

#endif  // GAMEOBJECT_H
