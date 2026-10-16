#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H

#include "DrawContext.h"
#include "EngineView.h"
#include "GameObject.h"
#include "NotificationManager.h"

namespace CMPUT350 {

/**
 * @brief Everything the engine passes to a game object's methods each frame.
 */
class GameContext {
public:
    EngineView *mEngineView;
	DrawContext *ScreenContext;
	DrawContext *GUIContext;
	std::weak_ptr<GameObject> CurrObject;
	NotificationManager *mNotificationManager;
	int currentFrame;
};

}  // namespace CMPUT350

#endif  // GAMECONTEXT_H
