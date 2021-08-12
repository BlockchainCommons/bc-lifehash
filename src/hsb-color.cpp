#include "hsb-color.hpp"

#include <math.h>

#include <stdexcept>

#include "color.hpp"
#include "numeric.hpp"

namespace LifeHash
{

HSBColor::HSBColor(const Color &color) {
    auto r = color.r;
    auto g = color.g;
    auto b = color.b;

    auto maxValue = max(r, g, b);
    auto minValue = min(r, g, b);

    auto brightness = maxValue;

    auto d = maxValue - minValue;
    auto saturation = maxValue == 0 ? 0 : d / maxValue;

    double hue;
    if (maxValue == minValue) {
        hue = 0;  // achromatic
    } else {
        if (maxValue == r) {
            hue = ((g - b) / d + (g < b ? 6 : 0)) / 6;
        } else if (maxValue == g) {
            hue = ((b - r) / d + 2) / 6;
        } else if (maxValue == b) {
            hue = ((r - g) / d + 4) / 6;
        } else {
            throw std::domain_error("Internal error.");
        }
    }
    this->hue = hue;
    this->saturation = saturation;
    this->brightness = brightness;
}

Color HSBColor::color() const {
    auto v = clamped(brightness);
    auto s = clamped(saturation);
    double red;
    double green;
    double blue;

    if (s <= 0) {
        red = v;
        green = v;
        blue = v;
    } else {
        auto h = modulo(hue, 1);
        if (h < 0) {
            h += 1;
        }
        h *= 6;
        int i = floorf(h);
        auto f = h - i;
        auto p = v * (1 - s);
        auto q = v * (1 - s * f);
        auto t = v * (1 - s * (1 - f));
        switch (i) {
            case 0:
                red = v;
                green = t;
                blue = p;
                break;
            case 1:
                red = q;
                green = v;
                blue = p;
                break;
            case 2:
                red = p;
                green = v;
                blue = t;
                break;
            case 3:
                red = p;
                green = q;
                blue = v;
                break;
            case 4:
                red = t;
                green = p;
                blue = v;
                break;
            case 5:
                red = v;
                green = p;
                blue = q;
                break;
            default:
                throw std::domain_error("Internal error.");
        }
    }

    return Color(red, green, blue);
}

} // namespace LifeHash
