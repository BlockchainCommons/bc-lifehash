#ifndef LIFEHASH_FRAC_GRID_HPP
#define LIFEHASH_FRAC_GRID_HPP

#include "cell-grid.hpp"

namespace LifeHash
{

// A grid of floating point values in [0..1], used for onion-skinning
// the generations of the Game of Life into a single grayscale image.
class FracGrid : public Grid<double> {
   public:
    FracGrid(const Size& size) : Grid(size) {}

    void overlay(const CellGrid& cellGrid, double frac) {
        for_all([&](const Point& p) {
            if (cellGrid.get_value(p)) {
                this->set_value(frac, p);
            }
        });
    }

    virtual Color color_for_value(const double& value) const { return Color::black.lerp_to(Color::white, value); }
};

} // namespace LifeHash

#endif
