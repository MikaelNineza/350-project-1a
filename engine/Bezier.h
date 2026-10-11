#ifndef BEZIER_H
#define BEZIER_H

#include "MathUtil.h"
#include <memory>
#include <vector>

namespace CMPUT350 {
class Bezier {
public:
    Bezier(const std::vector<Point2D> &pts);
    
    Point2D GetPoint(float t) const;
    Point2D GetSlope(float t) const;
    
    static Point2D GetPoint(const std::vector<Point2D> &pts, float t);
    static Point2D GetSlope(const std::vector<Point2D> &pts, float t);
private:
    std::vector<Point2D> points;
};
}

#endif
