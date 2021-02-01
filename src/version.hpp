#ifndef LIFEHASH_VERSION_HPP
#define LIFEHASH_VERSION_HPP

namespace LifeHash {

// The available versions of LifeHash.
enum class Version {
    version1, // DEPRECATED. Uses HSB gamut. Not CMYK-friendly. Has some minor gradient bugs.
    version2, // CMYK-friendly gamut. Recommended for most purposes.
    detailed, // Double resolution. CMYK-friendly gamut gamut.
    fiducial, // Optimized for generating machine-vision fiducials. High-contrast. CMYK-friendly gamut.
    grayscale_fiducial // Optimized for generating machine-vision fiducials. High-contrast.
};

}
#endif
