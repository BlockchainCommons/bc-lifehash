#ifndef LIFEHASH_POINT_HPP
#define LIFEHASH_POINT_HPP

namespace LifeHash
{

// A struct representing an integer cartesian point.
struct Point {
    int x;
    int y;

    Point(int x, int y) : x(x), y(y) {}

    static const Point zero;
};

inline bool operator==(const Point& lhs, const Point& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }

} // namespace LifeHash

#endif
