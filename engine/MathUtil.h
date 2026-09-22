#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <cmath>
#include <iostream>

namespace CMPUT350 {

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
        // if x and y ranges don't overlap then they won't intersect
        if (!Line::OverlappingRanges(*this, other)) {
            return false;
        }

        bool this_vertical = p1.x == p2.x;
        bool other_vertical = other.p1.x == other.p2.x;

        // case 1: both lines are vertical
        if (this_vertical && other_vertical) {
            // if they don't have the same x, they never intersect
            if (p1.x != other.p1.x) {
                return false;
            }
            
            // otherwise they always intersect
            Line::FindIntersect(*this, other, crossingPoint);
            return true;
        }

        // case 2: this is vertical, other isn't
        else if (this_vertical) {
            return Line::CheckVertical(*this, other, crossingPoint);
        }

        else if (other_vertical) {
            return Line::CheckVertical(other, *this, crossingPoint);
        }

        // get slope equation of both line segments y = mx+b
        auto m = (p2.y - p1.y) / (p2.x - p1.x);
        auto m_other = (other.p2.y - other.p1.y) / (other.p2.x - other.p1.x);

        auto b = p1.y - (m * p1.x);
        auto b_other = other.p1.y - (m_other * other.p1.x);

        // check if lines are parallel
        if (m == m_other){
            // if b != b_other they never intersect
            if (b != b_other){
                return false;
            }

            // otherwise they always intersect; pick the first appropriate endpoint as the "crossingpoint"
            Line::FindIntersect(*this, other, crossingPoint);
            return true;
        }
        
        // intersection only happens once at (x, y)
        float x = (b_other - b) / (m - m_other);
        float y = m*x + b;

        // check that both line segments contain the crossingpoint x
        if (!(XWithinRange(*this, x) && XWithinRange(other, x))){
            return false;
        }

        crossingPoint.x = x;
        crossingPoint.y = y;

        return true;
    }

    private static bool CheckVertical(const Line vertical, const Line other, Point2D &crossingPoint) {
        // get the slope equation of the non-vertical line
        auto m = (other.p2.y - other.p1.y) / (other.p2.x - other.p1.x);
        auto b = other.p1.y - (m * other.p1.x);

        // an intersection is guarateed to occur at the fixed x for vertical
        auto x = vertical.p1.x;
        auto y = m * x + b;

        // check to see if the intersection occurs within the range of each line
        if (!(XWithinRange(vertical, x) && XWithinRange(other, x) &&
            YWithinRange(vertical, y) && YWithinRange(other, y))){
            return false;    
        }

        crossingPoint.x = x;
        crossingPoint.y = y;

        return true;
    }

    private static bool XWithinRange(const Line line, float x) {
        float x1 = min(line.p1.x, line.p2.x);
        float x2 = max(line.p1.x, line.p2.x);
        return x1 <= x && x <= x2;
    }

    private static bool YWithinRange(const Line line, float y) {
        float y1 = min(line.p1.y, line.p2.y);
        float y2 = max(line.p1.y, line.p2.y);
        return y1 <= y && y <= y2;
    }

    private static void FindIntersect(const Line first, const Line second, Point2D &crossingPoint) {
        // method called if guaranteed same slope equation and overlapping 
        // check each endpoint and return the first one that indicates overlap
        if (first.p1.x >= min(second.p1.x, second.p2.x) && first.p1.x <= max(second.p1.x, second.p2.x) &&
            first.p1.y >= min(second.p1.y, second.p2.y) && first.p1.y <= max(second.p1.y, second.p2.y)) {
            crossingPoint = first.p1;
        } 
        else if (first.p2.x >= min(second.p1.x, second.p2.x) && first.p2.x <= max(second.p1.x, second.p2.x) &&
                first.p2.y >= min(second.p1.y, second.p2.y) && first.p2.y <= max(second.p1.y, second.p2.y)) {
            crossingPoint = first.p2;
        } 
        else if (second.p1.x >= min(first.p1.x, first.p2.x) && second.p1.x <= max(first.p1.x, first.p2.x) &&
                second.p1.y >= min(first.p1.y, first.p2.y) && second.p1.y <= max(first.p1.y, first.p2.y)) {
            crossingPoint = second.p1;
        } 
        else {
            crossingPoint = second.p2;
        }
    }
    
    // returns true if both lines have overlapping x and y values
    private static bool OverlappingRanges(Line first, Line second) {
        float first_x1 = min(first.p1.x, first.p2.x);
        float first_x2 = max(first.p1.x, first.p2.x);
        float second_x1 = min(second.p1.x, second.p2.x);
        float second_x2 = max(second.p1.x, second.p2.x);

        float first_y1 = min(first.p1.y, first.p2.y);
        float first_y2 = max(first.p1.y, first.p2.y);
        float second_y1 = min(second.p1.y, second.p2.y);
        float second_y2 = max(second.p1.y, second.p2.y);
        
        bool x_overlaps = first_x1 <= second_x2 && second_x1 <= first_x2;
        bool y_overlaps = first_y1 <= second_y2 && second_y1 <= first_y2;
        return x_overlaps && y_overlaps;
    }
};

static std::ostream &operator<<(std::ostream &os, const Line &l) {
    // TODO: write this code
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
        : topLeft(Point2D(top, left)), width(width), height(height) {}

    Rect(Point2D tl = {0, 0}, int w = 0, int h = 0) : topLeft(tl), width(w), height(h) {}

    // Creates bounding box around p1 and p2 with positive width/height
    Rect(Point2D p1, Point2D p2)
        : topLeft(std::min(p1.x, p2.x), std::min(p1.y, p2.y)),
          width(fabs(p1.x - p2.x)),
          height(fabs(p1.y - p2.y)) {}

    Rect(Point2D center, float radius)
        : topLeft(center.x - radius, center.y - radius), width(2 * radius), height(2 * radius) {}

    Rect &operator|=(const Rect &other) {
        // TODO: write this code
        return *this;
    }
    Rect &operator|=(const Point2D &other) {
        // TODO: write this code
        return *this;
    }
    Rect &operator|=(const Line &other) {
        // TODO: write this code
        return *this;
    }
    Rect &operator&=(const Rect &other) {
        // TODO: write this code
        return *this;
    }
    Rect &operator+=(const Point2D &other) {
        // TODO: write this code
        return *this;
    }
    Rect operator+(const Point2D &other) const {
        // TODO: write this code
        return *this;
    }
    void Inset(int inset) {
        // TODO: write this code
    }
    bool IsInside(const Point2D &p) const {
        // TODO: write this code
        return false;
    }
};

static std::ostream &operator<<(std::ostream &os, const Rect &l) {
    // TODO: write this code
    return os;
}

}  // namespace CMPUT350

#endif  // MATHUTIL_H
