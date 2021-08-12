#ifndef LIFEHASH_HSB_COLOR_HPP
#define LIFEHASH_HSB_COLOR_HPP

namespace LifeHash
{

struct Color;

// A struct representing a color in the HSB space.
// Only used by version1 LifeHashes.
struct HSBColor {
    double hue;
    double saturation;
    double brightness;

    HSBColor(double hue, double saturation = 1, double brightness = 1) : hue(hue), saturation(saturation), brightness(brightness) {}

    HSBColor(const Color &color);

    Color color() const;
};

} // namespace LifeHash

#endif
