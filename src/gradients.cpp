#include "gradients.hpp"

#include <algorithm>

#include "color.hpp"
#include "hsb-color.hpp"

using namespace std;

namespace LifeHash
{

static ColorFunc grayscale = blend(Color::black, Color::white);

static ColorFunc select_grayscale(BitEnumerator& entropy) { return entropy.next() ? grayscale : reverse(grayscale); }

static Color make_hue(double t) { return HSBColor(t).color(); }

static ColorFunc spectrum = blend({
    Color::from_uint8_values(0, 168, 222), 
    Color::from_uint8_values(51, 51, 145), 
    Color::from_uint8_values(233, 19, 136), 
    Color::from_uint8_values(235, 45, 46),
    Color::from_uint8_values(253, 233, 43), 
    Color::from_uint8_values(0, 158, 84), 
    Color::from_uint8_values(0, 168, 222)
});

static ColorFunc spectrum_cmyk_safe = blend({
    Color::from_uint8_values(0, 168, 222), 
    Color::from_uint8_values(41, 60, 130), 
    Color::from_uint8_values(210, 59, 130), 
    Color::from_uint8_values(217, 63, 53),
    Color::from_uint8_values(244, 228, 81), 
    Color::from_uint8_values(0, 158, 84), 
    Color::from_uint8_values(0, 168, 222)
});

static Color adjust_for_luminance(const Color& color, const Color& contrast_color) {
    auto lum = color.luminance();
    auto contrast_lum = contrast_color.luminance();
    double threshold = 0.6;
    auto offset = abs(lum - contrast_lum);
    if (offset > threshold) {
        return color;
    }
    double boost = 0.7;
    auto t = lerp(0, threshold, boost, 0, offset);
    if (contrast_lum > lum) {
        // darken this color
        return color.darken(t).burn(t * 0.6);
    } else {
        // lighten this color
        return color.lighten(t).burn(t * 0.6);
    }
}

static ColorFunc monochromatic(BitEnumerator& entropy, ColorFunc hue_generator) {
    auto hue = entropy.next_frac();
    auto is_tint = entropy.next();
    auto is_reversed = entropy.next();
    auto key_advance = entropy.next_frac() * 0.3 + 0.05;
    auto neutral_advance = entropy.next_frac() * 0.3 + 0.05;

    auto key_color = hue_generator(hue);

    double contrast_brightness;
    if (is_tint) {
        contrast_brightness = 1;
        key_color = key_color.darken(0.5);
    } else {
        contrast_brightness = 0;
    }
    auto neutral_color = grayscale(contrast_brightness);

    auto key_color_2 = key_color.lerp_to(neutral_color, key_advance);
    auto neutral_color_2 = neutral_color.lerp_to(key_color, neutral_advance);

    auto gradient = blend(key_color_2, neutral_color_2);
    return is_reversed ? reverse(gradient) : gradient;
}

static ColorFunc monochromatic_fiducial(BitEnumerator& entropy) {
    auto hue = entropy.next_frac();
    auto is_reversed = entropy.next();
    auto is_tint = entropy.next();

    auto contrast_color = is_tint ? Color::white : Color::black;
    auto key_color = adjust_for_luminance(spectrum_cmyk_safe(hue), contrast_color);

    auto gradient = blend({key_color, contrast_color, key_color});
    return is_reversed ? reverse(gradient) : gradient;
}

static ColorFunc complementary(BitEnumerator& entropy, ColorFunc hue_generator) {
    auto spectrum1 = entropy.next_frac();
    auto spectrum2 = modulo(spectrum1 + 0.5, 1);
    auto lighter_advance = entropy.next_frac() * 0.3;
    auto darker_advance = entropy.next_frac() * 0.3;
    auto is_reversed = entropy.next();

    auto color1 = hue_generator(spectrum1);
    auto color2 = hue_generator(spectrum2);

    auto luma1 = color1.luminance();
    auto luma2 = color2.luminance();

    Color darker_color;
    Color lighter_color;
    if (luma1 > luma2) {
        darker_color = color2;
        lighter_color = color1;
    } else {
        darker_color = color1;
        lighter_color = color2;
    }

    auto adjustedLighterColor = lighter_color.lighten(lighter_advance);
    auto adjustedDarkerColor = darker_color.darken(darker_advance);

    auto gradient = blend(adjustedDarkerColor, adjustedLighterColor);
    return is_reversed ? reverse(gradient) : gradient;
}

static ColorFunc complementary_fiducial(BitEnumerator& entropy) {
    auto spectrum1 = entropy.next_frac();
    auto spectrum2 = modulo((spectrum1 + 0.5), 1);
    auto is_tint = entropy.next();
    auto is_reversed = entropy.next();
    auto neutral_color_bias = entropy.next();

    auto neutral_color = is_tint ? Color::white : Color::black;
    auto color1 = spectrum_cmyk_safe(spectrum1);
    auto color2 = spectrum_cmyk_safe(spectrum2);

    auto biased_neutral_color = neutral_color.lerp_to(neutral_color_bias ? color1 : color2, 0.2).burn(0.1);
    auto gradient = blend({
        adjust_for_luminance(color1, biased_neutral_color), 
        biased_neutral_color, 
        adjust_for_luminance(color2, biased_neutral_color)
    });
    return is_reversed ? reverse(gradient) : gradient;
}

static ColorFunc triadic(BitEnumerator& entropy, ColorFunc hue_generator) {
    auto spectrum1 = entropy.next_frac();
    auto spectrum2 = modulo(spectrum1 + 1.0 / 3, 1);
    auto spectrum3 = modulo((spectrum1 + 2.0 / 3), 1);
    auto lighter_advance = entropy.next_frac() * 0.3;
    auto darker_advance = entropy.next_frac() * 0.3;
    auto is_reversed = entropy.next();

    auto color1 = hue_generator(spectrum1);
    auto color2 = hue_generator(spectrum2);
    auto color3 = hue_generator(spectrum3);
    vector<Color> colors = {color1, color2, color3};
    sort(colors.begin(), colors.end(), [](const Color& a, const Color& b) -> bool { return a.luminance() < b.luminance(); });

    auto darker_color = colors[0];
    auto middle_color = colors[1];
    auto lighter_color = colors[2];

    auto adjusted_lighter_color = lighter_color.lighten(lighter_advance);
    auto adjusted_darker_color = darker_color.darken(darker_advance);

    auto gradient = blend({adjusted_lighter_color, middle_color, adjusted_darker_color});
    return is_reversed ? reverse(gradient) : gradient;
}

static ColorFunc triadic_fiducial(BitEnumerator& entropy) {
    auto spectrum1 = entropy.next_frac();
    auto spectrum2 = modulo(spectrum1 + 1.0 / 3, 1);
    auto spectrum3 = modulo((spectrum1 + 2.0 / 3), 1);
    auto is_tint = entropy.next();
    auto neutral_insert_index = entropy.next_uint8() % 2 + 1;
    auto is_reversed = entropy.next();

    auto neutralColor = is_tint ? Color::white : Color::black;

    vector<Color> colors = {spectrum_cmyk_safe(spectrum1), spectrum_cmyk_safe(spectrum2), spectrum_cmyk_safe(spectrum3)};
    switch (neutral_insert_index) {
        case 1:
            colors[0] = adjust_for_luminance(colors[0], neutralColor);
            colors[1] = adjust_for_luminance(colors[1], neutralColor);
            colors[2] = adjust_for_luminance(colors[2], colors[1]);
            break;
        case 2:
            colors[1] = adjust_for_luminance(colors[1], neutralColor);
            colors[2] = adjust_for_luminance(colors[2], neutralColor);
            colors[0] = adjust_for_luminance(colors[0], colors[1]);
            break;
        default:
            throw domain_error("Internal error.");
    }

    colors.insert(colors.begin() + neutral_insert_index, neutralColor);

    auto gradient = blend(colors);
    return is_reversed ? reverse(gradient) : gradient;
}

static ColorFunc analogous(BitEnumerator& entropy, ColorFunc hue_generator) {
    auto spectrum1 = entropy.next_frac();
    auto spectrum2 = modulo(spectrum1 + 1.0 / 12, 1);
    auto spectrum3 = modulo(spectrum1 + 2.0 / 12, 1);
    auto spectrum4 = modulo(spectrum1 + 3.0 / 12, 1);
    auto advance = entropy.next_frac() * 0.5 + 0.2;
    auto is_reversed = entropy.next();

    auto color1 = hue_generator(spectrum1);
    auto color2 = hue_generator(spectrum2);
    auto color3 = hue_generator(spectrum3);
    auto color4 = hue_generator(spectrum4);

    Color darkest_color;
    Color dark_color;
    Color light_color;
    Color lightest_color;

    if (color1.luminance() < color4.luminance()) {
        darkest_color = color1;
        dark_color = color2;
        light_color = color3;
        lightest_color = color4;
    } else {
        darkest_color = color4;
        dark_color = color3;
        light_color = color2;
        lightest_color = color1;
    }

    auto adjusted_darkest_color = darkest_color.darken(advance);
    auto adjusted_dark_color = dark_color.darken(advance / 2);
    auto adjusted_light_color = light_color.lighten(advance / 2);
    auto adjusted_lightest_color = lightest_color.lighten(advance);

    auto gradient = blend({adjusted_darkest_color, adjusted_dark_color, adjusted_light_color, adjusted_lightest_color});
    return is_reversed ? reverse(gradient) : gradient;
}

static ColorFunc analogous_fiducial(BitEnumerator& entropy) {
    auto spectrum1 = entropy.next_frac();
    auto spectrum2 = modulo(spectrum1 + 1.0 / 10, 1);
    auto spectrum3 = modulo(spectrum1 + 2.0 / 10, 1);
    auto is_tint = entropy.next();
    auto neutral_insert_index = entropy.next_uint8() % 2 + 1;
    auto is_reversed = entropy.next();

    auto neutral_color = is_tint ? Color::white : Color::black;

    vector<Color> colors = {spectrum_cmyk_safe(spectrum1), spectrum_cmyk_safe(spectrum2), spectrum_cmyk_safe(spectrum3)};
    switch (neutral_insert_index) {
        case 1:
            colors[0] = adjust_for_luminance(colors[0], neutral_color);
            colors[1] = adjust_for_luminance(colors[1], neutral_color);
            colors[2] = adjust_for_luminance(colors[2], colors[1]);
            break;
        case 2:
            colors[1] = adjust_for_luminance(colors[1], neutral_color);
            colors[2] = adjust_for_luminance(colors[2], neutral_color);
            colors[0] = adjust_for_luminance(colors[0], colors[1]);
            break;
        default:
            throw domain_error("Internal error");
    }
    colors.insert(colors.begin() + neutral_insert_index, neutral_color);

    auto gradient = blend(colors);
    return is_reversed ? reverse(gradient) : gradient;
}

ColorFunc select_gradient(BitEnumerator& entropy, Version version) {
    if (version == Version::grayscale_fiducial) {
        return select_grayscale(entropy);
    }

    auto value = entropy.next_uint2();

    switch (value) {
        case 0:
            switch (version) {
                case Version::version1:
                    return monochromatic(entropy, make_hue);
                case Version::version2:
                case Version::detailed:
                    return monochromatic(entropy, spectrum_cmyk_safe);
                case Version::fiducial:
                    return monochromatic_fiducial(entropy);
                case Version::grayscale_fiducial:
                    return grayscale;
            }
            break;
        case 1:
            switch (version) {
                case Version::version1:
                    return complementary(entropy, spectrum);
                case Version::version2:
                case Version::detailed:
                    return complementary(entropy, spectrum_cmyk_safe);
                case Version::fiducial:
                    return complementary_fiducial(entropy);
                case Version::grayscale_fiducial:
                    return grayscale;
            }
            break;
        case 2:
            switch (version) {
                case Version::version1:
                    return triadic(entropy, spectrum);
                case Version::version2:
                case Version::detailed:
                    return triadic(entropy, spectrum_cmyk_safe);
                case Version::fiducial:
                    return triadic_fiducial(entropy);
                case Version::grayscale_fiducial:
                    return grayscale;
            }
            break;
        case 3:
            switch (version) {
                case Version::version1:
                    return analogous(entropy, spectrum);
                case Version::version2:
                case Version::detailed:
                    return analogous(entropy, spectrum_cmyk_safe);
                case Version::fiducial:
                    return analogous_fiducial(entropy);
                case Version::grayscale_fiducial:
                    return grayscale;
            }
            break;
    }
    return grayscale;
}

} // namespace LifeHash
