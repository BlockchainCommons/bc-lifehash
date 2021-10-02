#include "color.hpp"

#include <math.h>

#include "hsb-color.hpp"

namespace LifeHash
{

Color::Color(const HSBColor &hsbColor) {
    Color c = hsbColor.color();
    this->r = c.r;
    this->g = c.g;
    this->b = c.b;
}

Color Color::white = Color(1, 1, 1);
Color Color::black = Color(0, 0, 0);
Color Color::red = Color(1, 0, 0);
Color Color::green = Color(0, 1, 0);
Color Color::blue = Color(0, 0, 1);
Color Color::cyan = Color(0, 1, 1);
Color Color::magenta = Color(1, 0, 1);
Color Color::yellow = Color(1, 1, 0);

double Color::luminance() const { return sqrtf(powf(0.299 * r, 2) + powf(0.587 * g, 2) + powf(0.114 * b, 2)); }

Color Color::burn(double t) const {
    auto f = max(1.0 - t, 1.0e-7);
    return Color(
        min(1.0 - (1.0 - r) / f, 1.0), 
        min(1.0 - (1.0 - g) / f, 1.0), 
        min(1.0 - (1.0 - b) / f, 1.0)
        );
}

Color Color::lerp_to(const Color &other, double t) const { 
    auto f = clamped(t);
    auto red = clamped(r * (1 - f) + other.r * f);
    auto green = clamped(g * (1 - f) + other.g * f);
    auto blue = clamped(b * (1 - f) + other.b * f);
    return Color(red, green, blue);
}

Color Color::from_uint8_values(uint8_t r, uint8_t g, uint8_t b) {
    return Color(double(r) / 255, double(g) / 255, double(b) / 255);
}

} // namespace LifeHash
