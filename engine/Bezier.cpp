#include "Bezier.h"

namespace CMPUT350 {

Bezier::Bezier(const std::vector<Point2D> &pts) : points(pts) {
}

Point2D Bezier::GetPoint(float t) const {
    return GetPoint(points, t);
}

Point2D Bezier::GetSlope(float t) const {
    return GetSlope(points, t);
}

Point2D Bezier::GetPoint(const std::vector<Point2D> &pts, float t) {
    std::vector<Point2D> tmp;
    for (size_t x = 0; x < pts.size()-1; x++) {
        tmp.push_back(pts[x]*(1.0f-t) + pts[x+1]*t);
    }
    
    while (tmp.size() > 1)
    {
        for (size_t x = 0; x < tmp.size()-1; x++) {
            tmp[x] = tmp[x]*(1.0f-t) + tmp[x+1]*t;
        }
        tmp.pop_back();
    }
    return tmp[0];
}

Point2D Bezier::GetSlope(const std::vector<Point2D> &pts, float t) {
    auto p1 = pts[0];
    auto p2 = pts[1];
    auto p3 = pts[2];
    auto p4 = pts[3];

    auto x = 3*std::pow(1 - t, 2) * (p2.x - p1.x) + 6*(1 - t)*t * (p3.x - p2.x) + 3*std::pow(t, 2) * (p4.x - p3.x);
    auto y = 3*std::pow(1 - t, 2) * (p2.y - p1.y) + 6*(1 - t)*t * (p3.y - p2.y) + 3*std::pow(t, 2) * (p4.y - p3.y);

    return Point2D(x, y);
}
    
}