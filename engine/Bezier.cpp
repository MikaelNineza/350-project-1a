#include "Bezier.h"
#include <cassert>

namespace CMPUT350 {

/**
 * @brief Constructs a path made of one or more cubic Bezier segments.
 *
 * @param pts The control points. A path with n segments needs 3n + 1 points: each segment
 *            uses four points, and consecutive segments share their end and start point.
 */
Bezier::Bezier(const std::vector<Point2D> &pts) : points(pts) {
}

/**
 * @brief Gets the point on this path at parameter t.
 * @see GetPoint(const std::vector<Point2D>&, float)
 */
Point2D Bezier::GetPoint(float t) const {
    return GetPoint(points, t);
}

/**
 * @brief Gets the derivative of this path at parameter t.
 * @see GetSlope(const std::vector<Point2D>&, float)
 */
Point2D Bezier::GetSlope(float t) const {
    return GetSlope(points, t);
}

/**
 * @brief Gets the point on a multi-segment Bezier path at parameter t.
 *
 * @param pts The control points (3n + 1 points for n segments).
 * @param t The position along the path, from 0 to n. The integer part picks the segment and
 *          the fractional part is the position within it, so t = 1.5 is halfway through the
 *          second segment. Values of n or more use the last segment.
 * @return The point on the curve, found with De Casteljau's algorithm, or (0, 0) if pts has
 *         fewer than 4 points.
 */
Point2D Bezier::GetPoint(const std::vector<Point2D> &pts, float t) {
    if (pts.size() < 4) return Point2D();

    // Use the int part of t to find the ith segment and then
    // set t as its decimal part in order to define the relative position within that segment.
    // segment handles the edge case where t is at the end of the last segment 
    int segmentCount = (pts.size() - 1) / 3;
    int segment = std::min(static_cast<int>(t), segmentCount - 1);
    t -= segment;
    auto segmentAdjustedPts = GetSegment(pts, segment);
    
    
    std::vector<Point2D> tmp;
    for (size_t x = 0; x < segmentAdjustedPts.size()-1; x++) {
        tmp.push_back(segmentAdjustedPts[x]*(1.0f-t) + segmentAdjustedPts[x+1]*t);
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

/**
 * @brief Gets the derivative (direction of travel) of a multi-segment Bezier path.
 *
 * @param pts The control points (3n + 1 points for n segments).
 * @param t The position along the path, chosen the same way as in GetPoint().
 * @return The derivative of the cubic segment at t. It points along the curve and is not
 *         normalized. Returns (0, 0) if pts has fewer than 4 points.
 */
Point2D Bezier::GetSlope(const std::vector<Point2D> &pts, float t) {
    if (pts.size() < 4) return Point2D();

    // Use the int part of t to find the ith segment and then
    // set t as its decimal part in order to define the relative position within that segment.
    // segment handles the edge case where t is at the end of the last segment
    int segmentCount = (pts.size() - 1) / 3;
    int segment = std::min(static_cast<int>(t), segmentCount - 1);
    t -= segment;

    auto segmentAdjustedPts = GetSegment(pts, segment);
    auto p1 = segmentAdjustedPts[0];
    auto p2 = segmentAdjustedPts[1];
    auto p3 = segmentAdjustedPts[2];
    auto p4 = segmentAdjustedPts[3];

    auto x = 3*std::pow(1 - t, 2) * (p2.x - p1.x) + 6*(1 - t)*t * (p3.x - p2.x) + 3*std::pow(t, 2) * (p4.x - p3.x);
    auto y = 3*std::pow(1 - t, 2) * (p2.y - p1.y) + 6*(1 - t)*t * (p3.y - p2.y) + 3*std::pow(t, 2) * (p4.y - p3.y);

    return Point2D(x, y);
}

/**
 * @brief Gets the four control points of one cubic segment.
 *
 * @param pts The control points of the whole path.
 * @param t The index of the segment, starting at 0.
 * @return Points 3t to 3t + 3 of pts. t must be a valid segment index, since it is not checked.
 */
std::vector<Point2D> Bezier::GetSegment(const std::vector<Point2D> &pts, int t) {
    std::vector<Point2D> segment(pts.begin() + (3*t), pts.begin() + (3*t) + 4);
    return segment;
}
    
}