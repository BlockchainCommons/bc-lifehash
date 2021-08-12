#ifndef LIFEHASH_COLOR_GRID_HPP
#define LIFEHASH_COLOR_GRID_HPP

#include <vector>

#include "color-func.hpp"
#include "frac-grid.hpp"
#include "patterns.hpp"

namespace LifeHash
{

// A class that takes a grayscale grid and applies color and
// symmetery to it to yield the finished LifeHash.
class ColorGrid : public Grid<Color> {
   public:
    ColorGrid(const FracGrid& frac_grid, ColorFunc gradient, Pattern pattern);

    virtual Color color_for_value(const Color& color) const { return color; }

   private:
    struct Transform {
        bool transpose;
        bool reflect_x;
        bool reflect_y;
    };

    static Size target_size(const Size& in_size, Pattern pattern);

    Point transform_point(const Point& point, const Transform& transform);
    void draw(const Point& p, const Color& color, const std::vector<Transform>& transforms);
    
    static std::vector<Transform> snowflake_transforms;
    static std::vector<Transform> pinwheel_transforms;
    static std::vector<Transform> fiducial_transforms;
};

} // namespace LifeHash

#endif
