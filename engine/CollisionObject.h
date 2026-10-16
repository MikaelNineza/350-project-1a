#ifndef COLLISION_OBJECT_H
#define COLLISION_OBJECT_H


#include "GraphicsObject.h"
#include "MathUtil.h"
#include <memory>
#include <vector>

namespace CMPUT350 {

// Which member of a ShapeUnion is in use.
enum class ShapeType {
    kLine, kCircle, kRect
};
// Storage for one collision shape. Shape::t says which member is valid.
union ShapeUnion {
    ShapeUnion(Line l) : line(l) {}
    ShapeUnion(Circle c) : circle(c) {}
    ShapeUnion(Rect r) : rect(r) {}
    Line line;
    Circle circle;
    Rect rect; // Axis-aligned rectangle
};
// A collision shape tagged with its type. Coordinates are local to the owning object.
struct Shape {
    Shape(const Line &l) : shape(l), t(ShapeType::kLine) {}
    Shape(const Circle &c) : shape(c), t(ShapeType::kCircle) {}
    Shape(const Rect &r) : shape(r), t(ShapeType::kRect) {}
    ShapeUnion shape;
    ShapeType t;
};

/**
 * @brief Tests a line against any shape.
 *
 * @param l The line.
 * @param other The shape to test against. Both must be in the same coordinate space.
 * @param collisionPoint Output: the hit point, set only when the function returns true.
 * @return true if they intersect.
 */
inline bool LineCollision(const Line &l, const Shape &other, Point2D &collisionPoint) {
    if (other.t == ShapeType::kCircle) {
        return other.shape.circle.Intersects(l, collisionPoint);
    }
    if (other.t == ShapeType::kLine) {
        return l.Crosses(other.shape.line, collisionPoint);
    }
    if (other.t == ShapeType::kRect) {
        return other.shape.rect.Intersects(l, collisionPoint);
    }

    return false;
}

/**
 * @brief Tests two circle or rect shapes for overlap.
 *
 * @param first The first shape.
 * @param second The second shape. Both must be in the same coordinate space.
 * @return true if the shapes overlap. Always false if either shape is a line;
 *         use LineCollision() for lines.
 */
inline bool CollisionMade(const Shape &first, const Shape &second) {
    if (first.t == ShapeType::kLine || second.t == ShapeType::kLine) {
        return false;
    }

    if (first.t == ShapeType::kCircle) {
        if (second.t == ShapeType::kCircle) {
            return first.shape.circle.Intersects(second.shape.circle);
        }
        return first.shape.circle.Intersects(second.shape.rect);
    }

    if (first.t == ShapeType::kRect) {
        if (second.t == ShapeType::kCircle) {
            return second.shape.circle.Intersects(first.shape.rect);
        }
        Rect overlap = first.shape.rect & second.shape.rect;
        return overlap.width > 0 && overlap.height > 0;
    }

    return false;

}

/**
 * @brief A graphics object that the engine checks for collisions each frame.
 *
 * See GameEngine::HandleCollisions() for how collisions are detected.
 */
class CollisionObject : public GraphicsObject {
public:
    virtual void CollisionEnter(const std::shared_ptr<CollisionObject> &obj) = 0;
    virtual void CollisionEnter(const std::shared_ptr<CollisionObject> &obj, const Point2D &collisionPoint)
	{ CollisionEnter(obj); }
    virtual const Rect &GetBounds() = 0;
    virtual const std::vector<Shape> &GetShapes() = 0;
};

}  // namespace CMPUT350

#endif
