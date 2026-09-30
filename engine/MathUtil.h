#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <cmath>
#include <algorithm>
#include <iostream>

namespace CMPUT350 {

/**
 * @brief A 2D point / vector with float components.
 *
 * Used both as a position and as a direction or offset. Provides arithmetic
 * operators, dot and cross products, distance, and normalization.
 */
struct Point2D {
    float x, y;

    /**
     * @brief Constructs a point.
     * @param x X coordinate (default 0).
     * @param y Y coordinate (default 0).
     */
    Point2D(float x = 0, float y = 0) : x(x), y(y) {}

    /**
     * @brief Euclidean distance to another point.
     * @param other The point to measure to.
     * @return Distance between this point and @p other.
     */
    double Distance(const Point2D &other) const {
        return sqrt(pow(x - other.x, 2) + pow(y - other.y, 2));
    }

    /**
     * @brief Component-wise addition of two points.
     * @param other The point to add.
     * @return A new point (x + other.x, y + other.y).
     */
    Point2D operator+(const Point2D &other) const {
        return Point2D(x + other.x, y + other.y);
    }

    /**
     * @brief Adds a scalar to both components.
     * @param other The value added to x and y.
     * @return A new point (x + other, y + other).
     */
    Point2D operator+(const float &other) const {
        return Point2D(x + other, y + other);
    }

    /**
     * @brief Component-wise subtraction of two points.
     * @param other The point to subtract.
     * @return A new point (x - other.x, y - other.y).
     */
    Point2D operator-(const Point2D &other) const {
        return Point2D(x - other.x, y - other.y);
    }

    /**
     * @brief Subtracts a scalar from both components.
     * @param other The value subtracted from x and y.
     * @return A new point (x - other, y - other).
     */
    Point2D operator-(const float &other) const {
        return Point2D(x - other, y - other);
    }

    /**
     * @brief Scales the point by a scalar.
     * @param scalar The scale factor.
     * @return A new point (x * scalar, y * scalar).
     * @see operator*(float, const Point2D&) for the scalar-on-the-left form.
     */
    Point2D operator*(const float &scalar) const {
        return Point2D(x * scalar, y * scalar);
    }

    /**
     * @brief Adds a scalar to both components in place.
     * @param scalar The value added to x and y.
     * @return Reference to this point.
     */
    Point2D &operator+=(const float &scalar) {
        x += scalar;
        y += scalar;
        return *this;
    }

    /**
     * @brief Adds another point to this one in place.
     * @param other The point to add.
     * @return Reference to this point.
     */
    Point2D &operator+=(const Point2D &other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    /**
     * @brief Subtracts another point from this one in place.
     * @param other The point to subtract.
     * @return Reference to this point.
     */
    Point2D &operator-=(const Point2D &other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    /**
     * @brief Exact equality comparison.
     * @param other The point to compare with.
     * @return true if both x and y are exactly equal (no tolerance for
     *         floating-point error).
     */
    bool operator==(const Point2D &other) const {
        return x == other.x && y == other.y;
    }

    /**
     * @brief Scales this point in place by an integer.
     * @param scalar The scale factor, converted to float.
     * @return Reference to this point.
     */
    Point2D &operator*=(const int &scalar) {
        float f = (float)scalar;
        x *= f;
        y *= f;
        return *this;
    }

    /**
     * @brief Divides this point in place by an integer.
     *
     * @warning No check for zero; dividing by 0 yields inf/NaN components.
     *
     * @param scalar The divisor, converted to float.
     * @return Reference to this point.
     */
    Point2D &operator/=(const int &scalar) {
        float f = (float)scalar;
        x /= f;
        y /= f;
        return *this;
    }

    /**
     * @brief Dot product, written as a * b.
     * @param other The other vector.
     * @return x * other.x + y * other.y.
     */
    float operator*(const Point2D &other) const {
        return (x * other.x) + (y * other.y);
    }

    /**
     * @brief Dot product of this vector with @p b.
     * @param b The other vector.
     * @return x * b.x + y * b.y.
     */
    float Dot(Point2D b) const {
        return (x * b.x) + (y * b.y);
    }

    /**
     * @brief Dot product of two vectors.
     * @param a First vector.
     * @param b Second vector.
     * @return a.x * b.x + a.y * b.y.
     */
    static float Dot(Point2D a, Point2D b) {
        return (a.x * b.x) + (a.y * b.y);
    }

    /**
     * @brief 2D cross product (the z component of the 3D cross product).
     * @param a First vector.
     * @param b Second vector.
     * @return a.x * b.y - a.y * b.x. Zero when the vectors are parallel; the
     *         sign indicates which side of @p a the vector @p b lies on.
     */
    static float Cross(Point2D a, Point2D b) {
        return (a.x * b.y) - (a.y * b.x);
    }

    /**
     * @brief Scales this vector in place to unit length.
     *
     * A zero-length vector is left unchanged.
     */
    void Normalize() {
        auto length = Distance(Point2D(0, 0));
        if (length) {
            x /= length;
            y /= length;
        }
    }
};

/**
 * @brief Writes a point to a stream in the form "(x, y)".
 * @param os Output stream.
 * @param p  Point to print.
 * @return @p os, to allow chaining.
 */
static std::ostream &operator<<(std::ostream &os, const Point2D &p) {
    os << "(" << p.x << ", " << p.y << ")";
    return os;
}

/**
 * @brief Scales a point by a scalar with the scalar on the left (number * point).
 * @param number The scale factor.
 * @param rhs    The point to scale.
 * @return A new point (number * rhs.x, number * rhs.y).
 */
static Point2D operator*(float number, const Point2D &rhs) {
    return Point2D(number * rhs.x, number * rhs.y);
}

/**
 * @brief A line segment defined by two endpoints.
 */
struct Line {
    Point2D p1, p2;

    /**
     * @brief Constructs a segment from two points.
     * @param p1 Start point (default origin).
     * @param p2 End point (default origin).
     */
    Line(Point2D p1 = {0, 0}, Point2D p2 = {0, 0}) : p1(p1), p2(p2) {}

    /**
     * @brief Constructs a segment from raw coordinates.
     * @param x1 Start x.
     * @param y1 Start y.
     * @param x2 End x.
     * @param y2 End y.
     */
    Line(float x1, float y1, float x2, float y2) : p1(x1, y1), p2(x2, y2) {}

    /**
     * @brief Length of the segment.
     * @return Distance from p1 to p2.
     */
    float Length() const {
        return p1.Distance(p2);
    }

    /**
     * @brief Finds the point on the segment that is closest to a given point.
     *
     * Projects @p p onto the infinite line through p1 and p2, expressed as
     * p1 + t * (p2 - p1), then clamps t to [0, 1] so the result never leaves
     * the segment. If the projection falls beyond an end, that endpoint is
     * returned.
     *
     * A zero-length segment (p1 == p2) is handled separately: the function
     * returns p1 immediately, avoiding a division by zero.
     *
     * @param p The reference point.
     * @return The closest point on the segment to @p p.
     */
    Point2D ClosestPoint(const Point2D &p) const {
        auto a = p1;
        auto b = p2;
        auto ab = b - a;

        if (ab.x == 0.0f && ab.y == 0.0f){
            return a;
        }

        auto av = p - a;

        float t = Point2D::Dot(av, ab) / Point2D::Dot(ab, ab);
        t = std::max(0.f, std::min(1.f, t));

        return a + t * ab;
    }

    /**
     * @brief Tests whether this segment crosses another and finds the crossing.
     *
     * Solves Q + t * Q_vec = P + u * P_vec for the two segments using 2D cross
     * products. The segments cross if both t and u lie in [0, 1] (endpoints
     * touching counts as crossing).
     *
     * @param other         The other segment.
     * @param crossingPoint Output: set to the intersection point only when the
     *                      function returns true; otherwise left unmodified.
     * @return true if the segments intersect. Returns false if they do not
     *         intersect, or if they are parallel (including collinear and
     *         overlapping segments).
     */
    bool Crosses(Line other, Point2D &crossingPoint) const {
        Point2D Q = p1;
        Point2D Q_vec = p2 - p1;

        Point2D P = other.p1;
        Point2D P_vec = other.p2 - other.p1;
        
        float dem = Point2D::Cross(Q_vec, P_vec);
        if (dem == 0.0f) {
            return false;
        }

        float t = Point2D::Cross(P - Q, P_vec) / dem;
        float u = Point2D::Cross(P - Q, Q_vec) / dem;
        
        if (!(0 <= t && t <= 1 && 0 <= u && u <= 1)) { 
            return false;
        }

        crossingPoint.x = Q.x + t * Q_vec.x;
        crossingPoint.y = Q.y + t * Q_vec.y;

        return true;
    }
};

/**
 * @brief Writes a line to a stream in the form "Start: (x, y) End: (x, y)".
 * @param os Output stream.
 * @param l  Line to print.
 * @return @p os, to allow chaining.
 */
static std::ostream &operator<<(std::ostream &os, const Line &l) {
    os << "Start: " << l.p1 << " End: " << l.p2;
    return os;
}

/**
 * @brief A circle defined by a center and radius.
 */
struct Circle {
    Point2D center;
    float radius;

    /**
     * @brief Constructs a circle from a center point and radius.
     * @param c Center (default origin).
     * @param r Radius (default 0).
     */
    Circle(Point2D c = {0, 0}, float r = 0) : center(c), radius(r) {}

    /**
     * @brief Constructs a circle from raw coordinates and radius.
     * @param x Center x.
     * @param y Center y.
     * @param r Radius.
     */
    Circle(float x, float y, float r) : center(x, y), radius(r) {}
};

/**
 * @brief An axis-aligned rectangle stored as a top-left corner plus size.
 *
 * Uses screen-style coordinates: y increases downward, so "top" is the
 * smallest y value.
 */
struct Rect {
    Point2D topLeft;
    float width, height;

    /**
     * @brief Constructs a rectangle from edge coordinates and size.
     * @param left   X of the left edge.
     * @param top    Y of the top edge.
     * @param width  Width of the rectangle.
     * @param height Height of the rectangle.
     */
    Rect(float left, float top, float width, float height)
        : topLeft(Point2D(left, top)), width(width), height(height) {}

    /**
     * @brief Constructs a rectangle from a top-left corner and integer size.
     *
     * @note Width and height are taken as int, so float arguments are truncated
     *       toward zero (e.g. 0.75f becomes 0).
     *
     * @param tl Top-left corner (default origin).
     * @param w  Width (default 0).
     * @param h  Height (default 0).
     */
    Rect(Point2D tl = {0, 0}, int w = 0, int h = 0) : topLeft(tl), width(w), height(h) {}

    /**
     * @brief Constructs the bounding box of two points.
     *
     * The points may be given in any order; width and height are always
     * non-negative.
     *
     * @param p1 First corner.
     * @param p2 Opposite corner.
     */
    Rect(Point2D p1, Point2D p2)
        : topLeft(std::min(p1.x, p2.x), std::min(p1.y, p2.y)),
          width(fabs(p1.x - p2.x)),
          height(fabs(p1.y - p2.y)) {}

    /**
     * @brief Constructs the square that bounds a circle.
     * @param center Center of the circle.
     * @param radius Radius of the circle; the square's side is 2 * radius.
     */
    Rect(Point2D center, float radius)
        : topLeft(center.x - radius, center.y - radius), width(2 * radius), height(2 * radius) {}

    /**
     * @brief Grows this rectangle to the union bounding box with another rectangle.
     *
     * The result is the smallest rectangle containing both.
     *
     * @param other The rectangle to include.
     * @return Reference to this rectangle.
     */
    Rect &operator|=(const Rect &other) {
        // minimally include "other" rectangle within this one
        // calculate and set new fields
        float left = std::min(topLeft.x, other.topLeft.x);
        float top = std::min(topLeft.y, other.topLeft.y);

        float right = std::max(topLeft.x + width, other.topLeft.x + other.width);
        float bottom = std::max(topLeft.y + height, other.topLeft.y + other.height);

        topLeft = Point2D(left, top);
        width = right - left;
        height = bottom - top;
        return *this;
    }

    /**
     * @brief Grows this rectangle just enough to contain a point.
     * @param other The point to include.
     * @return Reference to this rectangle.
     */
    Rect &operator|=(const Point2D &other) {
        // minimally include "other" point within this rectangle
        Rect pt(other, 0.f, 0.f);
        *this |= pt;
        return *this;
    }

    /**
     * @brief Grows this rectangle just enough to contain a line segment.
     *
     * Includes both endpoints, which is sufficient because a rectangle that
     * contains both endpoints contains the whole segment.
     *
     * @param other The line segment to include.
     * @return Reference to this rectangle.
     */
    Rect &operator|=(const Line &other) {
        // a line can be included using endpoints
        *this |= other.p1;
        *this |= other.p2;
        return *this;
    }

    /**
     * @brief Shrinks this rectangle to its intersection with another.
     *
     * If the rectangles do not overlap, width and height are clamped to 0. If
     * both end up 0, the top-left corner is reset to the origin.
     *
     * @param other The rectangle to intersect with.
     * @return Reference to this rectangle.
     */
    Rect &operator&=(const Rect &other) {
        // intersection
        float left = std::max(topLeft.x, other.topLeft.x);
        float right = std::min(topLeft.x + width, other.topLeft.x + other.width);
        float top = std::max(topLeft.y, other.topLeft.y);
        float bottom = std::min(topLeft.y + height, other.topLeft.y + other.height);

        topLeft = Point2D(left, top);
        width = std::max(0.0f, right - left);
        height = std::max(0.0f, bottom - top);
        if (width == 0.0f && height == 0.0f) { 
            topLeft = Point2D(0.0f, 0.0f);
        }
        return *this;
    }

    /**
     * @brief Translates this rectangle in place.
     * @param other Offset added to the top-left corner; size is unchanged.
     * @return Reference to this rectangle.
     */
    Rect &operator+=(const Point2D &other) {
        topLeft += other;
        return *this;
    }

    /**
     * @brief Returns a translated copy of this rectangle.
     * @param other Offset added to the top-left corner of the copy.
     * @return The translated rectangle; this rectangle is unchanged.
     */
    Rect operator+(const Point2D &other) const {
        Rect rect = *this;
        rect += other;
        return rect;
    }

    /**
     * @brief Shrinks the rectangle equally on every side.
     *
     * Moves the top-left corner in by @p inset and reduces width and height by
     * 2 * @p inset. A negative value grows the rectangle.
     *
     * @note @p inset is an int, so float arguments are truncated toward zero.
     *
     * @param inset Number of units to remove from each side.
     */
    void Inset(int inset) {
        // shrink each side by "inset" units
        topLeft.x += inset;
        topLeft.y += inset;
        width -= 2 * inset;
        height -= 2 * inset;
    }

    /**
     * @brief Tests whether a point lies inside the rectangle.
     *
     * Edges are inclusive, so a point exactly on the boundary counts as inside.
     *
     * @param p The point to test.
     * @return true if @p p is inside or on the edge of the rectangle.
     */
    bool IsInside(const Point2D &p) const {
        float x1 = topLeft.x;
        float x2 = x1 + width;

        float y1 = topLeft.y;
        float y2 = y1 + height;

        return x1 <= p.x && p.x <= x2 && y1 <= p.y && p.y <= y2;
    }
};

/**
 * @brief Writes a rectangle to a stream in the form
 *        "Top left: (x, y), Width: w, Height: h".
 * @param os Output stream.
 * @param l  Rectangle to print.
 * @return @p os, to allow chaining.
 */
static std::ostream &operator<<(std::ostream &os, const Rect &l) {
    os << "Top left: " << l.topLeft << ", Width: " << l.width << ", Height: " << l.height;
    return os;
}

}  // namespace CMPUT350

#endif  // MATHUTIL_H
