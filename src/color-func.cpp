#include "color-func.hpp"
#include "numeric.hpp"

namespace LifeHash
{

ColorFunc reverse(ColorFunc c) {
    return [=](double t) { return c(1 - t); };
}

ColorFunc blend(const Color& color1, const Color& color2) {
    return [=](double t) { return color1.lerp_to(color2, t); };
}

ColorFunc blend(const std::vector<Color>& colors) {
    auto count = colors.size();
    switch (count) {
        case 0:
            return blend(Color::black, Color::black);
            break;
        case 1:
            return blend(colors[0], colors[0]);
            break;
        case 2:
            return blend(colors[0], colors[1]);
            break;
        default:
            return [=](double t) {
                if(t >= 1) {
                    return colors[count - 1];
                } else if(t <= 0) {
                    return colors[0];
                } else {
                    auto segments = count - 1;
                    auto s = t * segments;
                    auto segment = size_t(s);
                    auto segment_frac = modulo(s, 1);
                    auto c1 = colors[segment];
                    auto c2 = colors[segment + 1];
                    return c1.lerp_to(c2, segment_frac);
                }
            };
            break;
    }
}

} // namespace LifeHash
