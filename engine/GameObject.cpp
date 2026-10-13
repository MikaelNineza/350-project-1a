#include "GameObject.h"
#include <string>

namespace CMPUT350 {

void GameObject::Initialize(GameContext *context) { return; }
void GameObject::Update(GameContext *context) { return; }
void GameObject::LateUpdate(GameContext *context) { return; }
void GameObject::RenderUI(GameContext *contextrender) { return; }
bool GameObject::IsAlive() const { return mIsAlive; }
void GameObject::Kill() { mIsAlive = false; }
void GameObject::ReceiveNotification(const std::string& key) {}
}  // namespace CMPUT350
