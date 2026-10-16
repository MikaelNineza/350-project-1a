#ifndef GRAPHICS_OBJECT_H
#define GRAPHICS_OBJECT_H

#include "GameObject.h"
#include "MathUtil.h"
namespace CMPUT350 {

class GameContext;

/**
 * @brief A game object that is drawn in the world.
 *
 * Before calling the render methods, the engine sets context->ScreenContext to this object's
 * location and rotation, so drawing at (0, 0) draws at the object's center.
 */
class GraphicsObject : public GameObject {
public:
    virtual void RenderBackground(GameContext *context);
    virtual void RenderForeground(GameContext *context);
	virtual Point2D GetLocation() const = 0;
	virtual float GetRotation() const = 0;

};

}  // namespace CMPUT350

#endif
