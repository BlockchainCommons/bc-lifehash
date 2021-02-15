#ifndef LIFEHASH_LIFEHASH_HPP
#define LIFEHASH_LIFEHASH_HPP

#include <vector>
#include <stdint.h>
#include <string>

//
// This is the C++ interface to liblifehash. The C interface is in "lifehash.h".
//

namespace LifeHash {

// The available versions of LifeHash.
enum class Version {
    version1, // DEPRECATED. Uses HSB gamut. Not CMYK-friendly. Has some minor gradient bugs.
    version2, // CMYK-friendly gamut. Recommended for most purposes.
    detailed, // Double resolution. CMYK-friendly gamut.
    fiducial, // Optimized for generating machine-vision fiducials. High-contrast. CMYK-friendly gamut.
    grayscale_fiducial // Optimized for generating machine-vision fiducials. High-contrast.
};

// An RGB image returned from the functions that make LifeHashes.
struct Image {
    size_t width;
    size_t height;
    std::vector<uint8_t> colors;
};

// Make a LifeHash from a UTF-8 string, which may be of any length.
// The caller is responsible to ensure that the string has undergone any
// necessary Unicode normalization in order to produce consistent results.
Image make_from_utf8(const std::string& s, Version version = Version::version2, size_t module_size = 1);

// Make a LifeHash from given data, which may be of any size.
Image make_from_data(const std::vector<uint8_t>& data, Version version = Version::version2, size_t module_size = 1);

// Make a LifeHash from the SHA256 fingerprint of some other data.
// The fingerprint must be exactly 32 pseudorandom bytes. This is the base
// LifeHash creation algorithm, but if you don't already have a SHA256 hash of
// some data, then you should access it by calling `make_from_data()`. If you
// are starting with a UTF-8 string, call `make_from_utf8()`.
Image make_from_fingerprint(const std::vector<uint8_t>& fingerprint, Version version = Version::version2, size_t module_size = 1);

}

#endif
