#ifndef LIFEHASH_CHANGE_GRID_HPP
#define LIFEHASH_CHANGE_GRID_HPP

#include "grid.hpp"

namespace LifeHash
{

// A grid used to optimize the running of Conway's Game of Life by keeping
// track of cells that need consideration in the next generation, which
// allows the pruning of cells that don't need consideration.
class ChangeGrid : public Grid<bool> {
    public:
    ChangeGrid(const Size& size) : Grid(size) { }

    inline void set_changed(const Point& point) {
        for_neighborhood(point, [&](const Point& o, const Point& p) {
            this->set_value(true, p);
        });
    }

    virtual Color color_for_value(const bool& value) const { return value ? Color::red : Color::blue; }
};

} // namespace LifeHash

#endif
