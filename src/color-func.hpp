#ifndef LIFEHASH_COLOR_FUNC_HPP
#define LIFEHASH_COLOR_FUNC_HPP

#include "color.hpp"
#include <functional>
#include <vector>

namespace LifeHash
{

// A function that takes a fraction [0..1] and returns a color along a gradient.
typedef std::function<Color(double)> ColorFunc;

// Returns the reverse the given color function.
ColorFunc reverse(ColorFunc c);

// Returns a color function that blends from one color to another.
ColorFunc blend(const Color& color1, const Color& color2);

// Returns a color function that blends through each of the given colors at equal intervals.
ColorFunc blend(const std::vector<Color>& colors);

} // namespace LifeHash

#endif
