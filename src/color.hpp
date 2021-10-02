#ifndef LIFEHASH_COLOR_HPP
#define LIFEHASH_COLOR_HPP

#include "numeric.hpp"

namespace LifeHash
{

struct HSBColor;

// A struct representing a color.
struct Color {
    double r;
    double g;
    double b;

    Color() : r(0), g(0), b(0) {}
    Color(double r, double g, double b) : r(r), g(g), b(b) {}
    Color(const HSBColor &hsbColor);

    Color lerp_to(const Color &other, double t) const;
    static Color from_uint8_values(uint8_t r, uint8_t g, uint8_t b);

    static Color white;
    static Color black;

    static Color red;
    static Color green;
    static Color blue;
    static Color cyan;
    static Color magenta;
    static Color yellow;

    inline Color lighten(double t) const { return lerp_to(white, t); }
    inline Color darken(double t) const { return lerp_to(black, t); }

    Color burn(double t) const;

    double luminance() const;
};

} // namespace LifeHash

#endif
