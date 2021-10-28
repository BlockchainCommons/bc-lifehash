#include <algorithm>
#include <set>
#include <stdexcept>
#include <vector>

#include "cell-grid.hpp"
#include "change-grid.hpp"
#include "color-grid.hpp"
#include "color.hpp"
#include "format-utils.hpp"
#include "frac-grid.hpp"
#include "gradients.hpp"
#include "numeric.hpp"
#include "patterns.hpp"
#include "sha256.hpp"
#include "size.hpp"
#include "hex.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

using namespace std;

namespace LifeHash {

struct Image {
    size_t width;
    size_t height;
    std::vector<uint8_t> colors;
};

Image make_from_utf8(const std::string& s, Version version, size_t module_size, bool has_alpha);
Image make_from_data(const std::vector<uint8_t>& data, Version version, size_t module_size, bool has_alpha);
Image make_from_digest(const std::vector<uint8_t>& digest, Version version, size_t module_size, bool has_alpha);

static Image make_image(size_t width, size_t height, const std::vector<double>& floatColors, size_t module_size, bool has_alpha) {
    if (module_size == 0) {
        throw domain_error("Invalid module size.");
    }

    auto scaled_width = width * module_size;
    auto scaled_height = height * module_size;
    auto result_components = has_alpha ? 4 : 3;
    auto scaled_capacity = scaled_width * scaled_height * result_components;

    std::vector<uint8_t> result_colors(scaled_capacity);
    for (size_t target_y = 0; target_y < scaled_width; target_y++) {
        for (size_t target_x = 0; target_x < scaled_height; target_x++) {
            auto source_x = target_x / module_size;
            auto source_y = target_y / module_size;
            auto source_offset = (source_y * width + source_x) * 3;

            auto target_offset = (target_y * scaled_width + target_x) * result_components;

            result_colors[target_offset] = clamped(floatColors[source_offset]) * 255;
            result_colors[target_offset + 1] = clamped(floatColors[source_offset + 1]) * 255;
            result_colors[target_offset + 2] = clamped(floatColors[source_offset + 2]) * 255;
            if(has_alpha) {
                result_colors[target_offset + 3] = 255;
            }
        }
    }

    return {scaled_width, scaled_height, result_colors};
}

Image make_from_utf8(const std::string& s, Version version, size_t module_size, bool has_alpha) {
    return make_from_data(to_data(s), version, module_size, has_alpha);
}

Image make_from_data(const std::vector<uint8_t>& data, Version version, size_t module_size, bool has_alpha) {
    auto digest = sha256(data);
    return make_from_digest(digest, version, module_size, has_alpha);
}

Image make_from_digest(const std::vector<uint8_t>& digest, Version version, size_t module_size, bool has_alpha) {
    if (digest.size() != 32) {
        throw domain_error("Digest must be 32 bytes.");
    }

    int length;
    size_t max_generations;

    switch (version) {
        case Version::version1:
        case Version::version2:
            length = 16;
            max_generations = 150;
            break;
        case Version::detailed:
        case Version::fiducial:
        case Version::grayscale_fiducial:
            length = 32;
            max_generations = 300;
            break;
        default:
            throw domain_error("Invalid version.");
    }

    const Size size(length, length);

    // These get reused from generation to generation by swapping them.
    auto current_cell_grid = new CellGrid(size);
    auto next_cell_grid = new CellGrid(size);
    auto current_change_grid = new ChangeGrid(size);
    auto next_change_grid = new ChangeGrid(size);

    set<Data> history_set;
    vector<Data> history;

    switch (version) {
        case Version::version1:
            next_cell_grid->set_data(digest);
            break;
        case Version::version2:
            // Ensure that .version2 in no way resembles .version1
            next_cell_grid->set_data(sha256(digest));
            break;
        case Version::detailed:
        case Version::fiducial:
        case Version::grayscale_fiducial:
            auto digest1 = digest;
            // Ensure that grayscale fiducials in no way resemble the regular color fiducials
            if (version == Version::grayscale_fiducial) {
                digest1 = sha256(digest1);
            }
            auto digest2 = sha256(digest1);
            auto digest3 = sha256(digest2);
            auto digest4 = sha256(digest3);
            auto digest_final = digest1;
            digest_final.insert(digest_final.end(), digest2.begin(), digest2.end());
            digest_final.insert(digest_final.end(), digest3.begin(), digest3.end());
            digest_final.insert(digest_final.end(), digest4.begin(), digest4.end());
            next_cell_grid->set_data(digest_final);
            break;
    }

    next_change_grid->set_all(true);

    while (history.size() < max_generations) {
        swap(current_cell_grid, next_cell_grid);
        swap(current_change_grid, next_change_grid);

        auto data = current_cell_grid->data();
        auto hash = sha256(data);
        if (history_set.find(hash) != history_set.end()) {
            break;
        }
        history_set.insert(hash);
        history.push_back(data);

        current_cell_grid->next_generation(*current_change_grid, *next_cell_grid, *next_change_grid);
    }

    FracGrid frac_grid(size);
    for (auto i = 0; i != history.size(); i++) {
        current_cell_grid->set_data(history[i]);
        auto frac = clamped(lerp_from(0, history.size(), i + 1));
        frac_grid.overlay(*current_cell_grid, frac);
    }

    // Normalizing the frac_grid to the range 0..1 was a step left out of .version1
    // In some cases it can cause the full range of the gradient to go unused.
    // This fixes the problem for the other versions, while remaining compatible
    // with .version1.
    if (version != Version::version1) {
        double min_value = INFINITY;
        double max_value = -INFINITY;
        frac_grid.for_all([&](const Point& p) {
            auto value = frac_grid.get_value(p);
            min_value = min(min_value, value);
            max_value = max(max_value, value);
        });

        frac_grid.for_all([&](const Point& p) {
            auto value = lerp_from(min_value, max_value, frac_grid.get_value(p));
            frac_grid.set_value(value, p);
        });
    }

    auto entropy = BitEnumerator(digest);

    switch (version) {
        case Version::detailed:
            // Throw away a bit of entropy to ensure we generate different colors and patterns from .version1
            entropy.next();
            break;
        case Version::version2:
            // Throw away two bits of entropy to ensure we generate different colors and patterns from .version1 or .detailed.
            entropy.next_uint2();
            break;
        default:
            break;
    }

    auto gradient = select_gradient(entropy, version);
    auto pattern = select_pattern(entropy, version);
    auto color_grid = ColorGrid(frac_grid, gradient, pattern);

    auto image = make_image(color_grid.size.width, color_grid.size.height, color_grid.colors(), module_size, has_alpha);

    delete current_cell_grid;
    delete next_cell_grid;
    delete current_change_grid;
    delete next_change_grid;

    return image;
}


}  // namespace LifeHash

//
// C Interface
//

extern "C" {

typedef enum LifeHashVersion {
    lifehash_version1,
    lifehash_version2,
    lifehash_detailed,
    lifehash_fiducial,
    lifehash_grayscale_fiducial
} LifeHashVersion;

typedef struct LifeHashImage {
    size_t width;
    size_t height;
    uint8_t* colors;
} LifeHashImage;

EMSCRIPTEN_KEEPALIVE
void lifehash_image_free(LifeHashImage* image) {
    free(image->colors);
    free(image);
}

EMSCRIPTEN_KEEPALIVE
static LifeHashImage* lifehash_make_image(const LifeHash::Image& image) {
    auto result_image = static_cast<LifeHashImage*>(malloc(sizeof(LifeHashImage)));
    auto result_colors = static_cast<uint8_t*>(malloc(image.colors.size()));
    result_image->width = image.width;
    result_image->height = image.height;
    result_image->colors = result_colors;
    auto p = result_colors;
    for(auto i = image.colors.begin(); i != image.colors.end();) { *p++ = *i++; }
    return result_image;
}

EMSCRIPTEN_KEEPALIVE
LifeHashImage* lifehash_make_from_utf8(const char* s, LifeHashVersion version, size_t module_size, bool has_alpha) {
    return lifehash_make_image(LifeHash::make_from_utf8(string(s), static_cast<LifeHash::Version>(version), module_size, has_alpha));
}

EMSCRIPTEN_KEEPALIVE
LifeHashImage* lifehash_make_from_data(const uint8_t* data, size_t len, LifeHashVersion version, size_t module_size, bool has_alpha) {
    return lifehash_make_image(LifeHash::make_from_data(std::vector<uint8_t>(data, data + len), static_cast<LifeHash::Version>(version), module_size, has_alpha));
}

EMSCRIPTEN_KEEPALIVE
LifeHashImage* lifehash_make_from_digest(const uint8_t* digest, LifeHashVersion version, size_t module_size, bool has_alpha) {
    return lifehash_make_image(LifeHash::make_from_digest(std::vector<uint8_t>(digest, digest + 32), static_cast<LifeHash::Version>(version), module_size, has_alpha));
}

EMSCRIPTEN_KEEPALIVE
char* lifehash_data_to_hex(const uint8_t* data, size_t len) {
    auto d = LifeHash::Data(data, data + len);
    auto hex = LifeHash::data_to_hex(d);
    auto str = (char*)malloc(hex.length() + 1);
    strcpy(str, hex.c_str());
    return str;
}

EMSCRIPTEN_KEEPALIVE
bool lifehash_hex_to_data(const uint8_t* utf8, size_t utf8_len, uint8_t** out, size_t* out_len) {
    try {
        auto hex = std::string(utf8, utf8 + utf8_len);
        auto data = LifeHash::hex_to_data(hex);
        auto buf = (uint8_t*)malloc(data.size());
        memcpy(buf, &data[0], data.size());
        *out = buf;
        *out_len = data.size();
        return true;
    } catch(...) {
        return false;
    }
}

EMSCRIPTEN_KEEPALIVE
void lifehash_sha256(const uint8_t* data, size_t len, uint8_t digest[SHA256_DIGEST_LENGTH]) {
    LifeHash::sha256_Raw(data, len, digest);
}

}
