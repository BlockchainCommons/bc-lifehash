#ifndef LIFEHASH_GRID_HPP
#define LIFEHASH_GRID_HPP

#include <algorithm>
#include <functional>
#include <vector>

#include "color.hpp"
#include "point.hpp"
#include "size.hpp"

namespace LifeHash
{

// A class that holds a 2-dimensional grid of values, 
// and allows the reading, writing, and iteration 
// through those values.
template <typename T>
class Grid {
   public:
    const Size size;

   protected:
    const std::size_t capacity;
    const int maxX;
    const int maxY;

    std::vector<T> storage;

    Grid(const Size& size) : size(size), capacity(size.width * size.height), storage(capacity), maxX(size.width - 1), maxY(size.height - 1) { }

    virtual Color color_for_value(const T& value) const = 0;

   private:
    inline size_t offset(const Point& p) const { return p.y * size.width + p.x; }

    inline static int circularIndex(int index, int modulus) { return (index + modulus) % modulus; }

   public:
    void set_all(const T& value) { std::fill_n(storage.begin(), capacity, value); }

    inline void set_value(const T& value, const Point& p) { storage[offset(p)] = value; }

    inline const T get_value(const Point& p) const { return storage[offset(p)]; }

    void for_all(const std::function<void(const Point&)>& f) {
        for (int y = 0; y <= maxY; y++) {
            for (int x = 0; x <= maxX; x++) {
                f(Point(x, y));
            }
        }
    }

    void for_all(const std::function<void(const Point&)>& f) const {
        const_cast<Grid*>(this)->for_all(f);
    }

    void for_neighborhood(const Point& point, const std::function<void(const Point&, const Point&)>& f) {
        for (int oy = -1; oy <= 1; oy++) {
            for (int ox = -1; ox <= 1; ox++) {
                auto o = Point(ox, oy);
                auto px = circularIndex(ox + point.x, size.width);
                auto py = circularIndex(oy + point.y, size.height);
                auto p = Point(px, py);
                f(o, p);
            }
        }
    }

    std::vector<double> colors() const {
        std::vector<double> result;
        for (auto i = storage.begin(); i != storage.end(); i++) {
            auto c = color_for_value(*i);
            result.push_back(c.r);
            result.push_back(c.g);
            result.push_back(c.b);
        }
        return result;
    }
};

} // namespace LifeHash

#endif
