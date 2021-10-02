#include "color-grid.hpp"

#include <algorithm>

namespace LifeHash
{

using namespace std;

Size ColorGrid::target_size(const Size& in_size, Pattern pattern) {
    auto multiplier = pattern == Pattern::fiducial ? 1 : 2;
    return Size(in_size.width * multiplier, in_size.height * multiplier);
}

vector<ColorGrid::Transform> ColorGrid::snowflake_transforms = {
    {false, false, false}, 
    {false, true, false}, 
    {false, false, true}, 
    {false, true, true}
};

vector<ColorGrid::Transform> ColorGrid::pinwheel_transforms = {
    {false, false, false}, 
    {true, true, false}, 
    {true, false, true}, 
    {false, true, true}
};

vector<ColorGrid::Transform> ColorGrid::fiducial_transforms = {
    {false, false, false}
};

inline Point ColorGrid::transform_point(const Point& point, const ColorGrid::Transform& transform) {
    auto result = point;
    if (transform.transpose) {
        swap(result.x, result.y);
    }
    if (transform.reflect_x) {
        result.x = maxX - result.x;
    }
    if (transform.reflect_y) {
        result.y = maxY - result.y;
    }
    return result;
}

ColorGrid::ColorGrid(const FracGrid& frac_grid, ColorFunc gradient, Pattern pattern) 
    : Grid(target_size(frac_grid.size, pattern))
{
    const vector<Transform>& transforms = [&]() {
        switch(pattern) {
            case Pattern::snowflake:
                return snowflake_transforms;
            case Pattern::pinwheel:
                return pinwheel_transforms;
            case Pattern::fiducial:
                return fiducial_transforms;
            default:
                return vector<Transform>();
        }
    }();

    frac_grid.for_all([&](const Point& p) {
        auto color = gradient(frac_grid.get_value(p));
        draw(p, color, transforms);
    });
}

void ColorGrid::draw(const Point& p, const Color& color, const vector<Transform>& transforms) {
    for(auto t: transforms) {
        auto p2 = transform_point(p, t);
        set_value(color, p2);
    }
}

} // namespace LifeHash
