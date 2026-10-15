#ifndef COLLISION_OBJECT_H
#define COLLISION_OBJECT_H


#include "GraphicsObject.h"
#include "MathUtil.h"
#include <memory>
#include <vector>

namespace CMPUT350 {

enum class ShapeType {
    kLine, kCircle, kRect
};
union ShapeUnion {
    ShapeUnion(Line l) : line(l) {}
    ShapeUnion(Circle c) : circle(c) {}
    ShapeUnion(Rect r) : rect(r) {}
    Line line;
    Circle circle;
    Rect rect; // Axis-aligned rectangle
};
struct Shape {
    Shape(const Line &l) : shape(l), t(ShapeType::kLine) {}
    Shape(const Circle &c) : shape(c), t(ShapeType::kCircle) {}
    Shape(const Rect &r) : shape(r), t(ShapeType::kRect) {}
    ShapeUnion shape;
    ShapeType t;
};

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

inline bool CollisionMade(const Shape &first, const Shape &second, Point2D &collisionPoint, int &objectLine) {
    if (first.t == ShapeType::kLine) {
        objectLine = 1;
        return false;
    }

    if (second.t == ShapeType::kLine) {
        objectLine = 2;
        return false;
    }

    objectLine = -1;

    if (first.t == ShapeType::kCircle) {
        if (second.t == ShapeType::kCircle) {
            return first.shape.circle.Intersects(second.shape.circle);
        }
        else if (second.t == ShapeType::kRect) {
            return first.shape.circle.Intersects(second.shape.rect);
        }
    }

    if (first.t == ShapeType::kRect) {
        if (second.t == ShapeType::kCircle) {
            return second.shape.circle.Intersects(first.shape.rect);
        }
        else if (second.t == ShapeType::kRect) {
            Rect overlap = first.shape.rect & second.shape.rect;
            return overlap.width > 0 && overlap.height > 0;
        }
    }

    return false;

}

class CollisionObject : public GraphicsObject {
public:
    virtual void CollisionEnter(const std::shared_ptr<CollisionObject> &obj) = 0;
    virtual void CollisionEnter(const std::shared_ptr<CollisionObject> &obj, const Point2D &collisionPoint)
	{ CollisionEnter(obj); }
    virtual const Rect &GetBounds() = 0;
    // Return the internal shapes in the object. Shapes can be:
    // lines, points/circles, or axis-aligned rectangles
    virtual const std::vector<Shape> &GetShapes() = 0;
};

}  // namespace CMPUT350

#endif
