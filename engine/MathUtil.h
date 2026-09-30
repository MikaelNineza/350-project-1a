#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <cmath>
#include <iostream>

namespace CMPUT350 {

using namespace std;

struct Point2D {
    float x, y;
    Point2D(float x = 0, float y = 0) : x(x), y(y) {}
    double Distance(const Point2D &other) const {
        return sqrt(pow(x - other.x, 2) + pow(y - other.y, 2));
    }
    Point2D operator+(const Point2D &other) const {
        return Point2D(x + other.x, y + other.y);
    }
    Point2D operator+(const float &other) const {
        return Point2D(x + other, y + other);
    }
    Point2D operator-(const Point2D &other) const {
        return Point2D(x - other.x, y - other.y);
    }
    Point2D operator-(const float &other) const {
        return Point2D(x - other, y - other);
    }
    Point2D operator*(const float &scalar) const {
        return Point2D(x * scalar, y * scalar);
    }
    Point2D &operator+=(const float &scalar) {
        x += scalar;
        y += scalar;
        return *this;
    }
    Point2D &operator+=(const Point2D &other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    Point2D &operator-=(const Point2D &other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    bool operator==(const Point2D &other) const {
        return x == other.x && y == other.y;
    }
    Point2D &operator*=(const int &scalar) {
        float f = (float)scalar;
        x *= f;
        y *= f;
        return *this;
    }
    Point2D &operator/=(const int &scalar) {
        float f = (float)scalar;
        x /= f;
        y /= f;
        return *this;
    }
    float operator*(const Point2D &other) const {
        return (x * other.x) + (y * other.y);
    }
    float Dot(Point2D b) const {
        return (x * b.x) + (y * b.y);
    }
    static float Dot(Point2D a, Point2D b) {
        return (a.x * b.x) + (a.y * b.y);
    }
    static float Cross(Point2D a, Point2D b) {
        return (a.x * b.y) - (a.y * b.x);
    }
    void Normalize() {
        auto length = Distance(Point2D(0, 0));
        x /= length;
        y /= length;
    }
};

static std::ostream &operator<<(std::ostream &os, const Point2D &p) {
    os << "(" << p.x << ", " << p.y << ")";
    return os;
}

static Point2D operator*(float number, const Point2D &rhs) {
    return Point2D(number * rhs.x, number * rhs.y);
}

struct Line {
    Point2D p1, p2;

    Line(Point2D p1 = {0, 0}, Point2D p2 = {0, 0}) : p1(p1), p2(p2) {}
    Line(float x1, float y1, float x2, float y2) : p1(x1, y1), p2(x2, y2) {}
    float Length() const {
        return p1.Distance(p2);
    }
    Point2D ClosestPoint(const Point2D &p) const {
        return p1.Distance(p) < p2.Distance(p) ? p1 : p2;
    }
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

static std::ostream &operator<<(std::ostream &os, const Line &l) {
    os << "Start: " << l.p1 << "End: " << l.p2;
    return os;
}

struct Circle {
    Point2D center;
    float radius;

    Circle(Point2D c = {0, 0}, float r = 0) : center(c), radius(r) {}

    Circle(float x, float y, float r) : center(x, y), radius(r) {}
};

struct Rect {
    Point2D topLeft;
    float width, height;

    Rect(float left, float top, float width, float height)
        : topLeft(Point2D(left, top)), width(width), height(height) {}

    Rect(Point2D tl = {0, 0}, int w = 0, int h = 0) : topLeft(tl), width(w), height(h) {}

    // Creates bounding box around p1 and p2 with positive width/height
    Rect(Point2D p1, Point2D p2)
        : topLeft(std::min(p1.x, p2.x), std::min(p1.y, p2.y)),
          width(fabs(p1.x - p2.x)),
          height(fabs(p1.y - p2.y)) {}

    Rect(Point2D center, float radius)
        : topLeft(center.x - radius, center.y - radius), width(2 * radius), height(2 * radius) {}

    Rect &operator|=(const Rect &other) {
        // minimally include "other" rectangle within this one
        // calculate and set new fields
        float left = min(topLeft.x, other.topLeft.x);
        float right = max(topLeft.x, other.topLeft.x);
        float top = min(topLeft.y, other.topLeft.y);
        float bottom = max(topLeft.y, other.topLeft.y);

        topLeft = Point2D(left, top);
        width = right - left;
        height = bottom - top;
        return *this;
    }
    Rect &operator|=(const Point2D &other) {
        // minimally include "other" point within this rectangle
        float left = min(topLeft.x, other.x);
        float right = max(topLeft.x, other.x);
        float top = min(topLeft.y, other.y);
        float bottom = max(topLeft.y, other.y);

        topLeft = Point2D(left, top);
        width = right - left;
        height = bottom - top;
        return *this;
    }
    Rect &operator|=(const Line &other) {
        // a line can be included using endpoints
        *this |= other.p1;
        *this |= other.p2;
        return *this;
    }
    Rect &operator&=(const Rect &other) {
        // intersection
        float left = max(topLeft.x, other.topLeft.x);
        float right = min(topLeft.x + width, other.topLeft.x + other.width);
        float top = max(topLeft.y, other.topLeft.y);
        float bottom = min(topLeft.y + height, other.topLeft.y + other.height);

        topLeft = Point2D(left, top);
        width = max(0.0f, right - left);
        height = max(0.0f, bottom - top);
        if (width == 0.0f && height == 0.0f) { 
            topLeft = Point2D(0.0f, 0.0f);
        }
        return *this;
    }
    Rect &operator+=(const Point2D &other) {
        topLeft += other;
        return *this;
    }
    Rect operator+(const Point2D &other) const {
        Rect rect = *this;
        rect += other;
        return rect;
    }
    void Inset(int inset) {
        // shrink each size by "inset" units
        topLeft.x += inset;
        topLeft.y += inset;
        width -= 2 * inset;
        height -= 2 * inset;
    }
    bool IsInside(const Point2D &p) const {
        float x1 = topLeft.x;
        float x2 = x1 + width;

        float y1 = topLeft.y;
        float y2 = y1 + height;

        return x1 <= p.x && p.x <= x2 && y1 <= p.y && p.y <= y2;
    }
};

static std::ostream &operator<<(std::ostream &os, const Rect &l) {
    os << "Top left: " << l.topLeft << ", Width: " << l.width << ", Height: " << l.height;
    return os;
}

}  // namespace CMPUT350

#endif  // MATHUTIL_H
