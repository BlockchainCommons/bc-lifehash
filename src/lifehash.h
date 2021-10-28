#ifndef LIFEHASH_H
#define LIFEHASH_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

//
// This is the C interface to liblifehash. The C++ interface is in "lifehash.hpp".
//

// The available versions of LifeHash.
typedef enum LifeHashVersion {
    lifehash_version1, // DEPRECATED. Uses HSB gamut. Not CMYK-friendly. Has some minor gradient bugs.
    lifehash_version2, // CMYK-friendly gamut. Recommended for most purposes.
    lifehash_detailed, // Double resolution. CMYK-friendly gamut.
    lifehash_fiducial, // Optimized for generating machine-vision fiducials. High-contrast. CMYK-friendly gamut.
    lifehash_grayscale_fiducial // Optimized for generating machine-vision fiducials. High-contrast.
} LifeHashVersion;

// An RGB image returned from the functions that make LifeHashes.
typedef struct LifeHashImage {
    size_t width;
    size_t height;
    uint8_t* colors;
} LifeHashImage;

// Release an image returned by the LifeHash creation functions.
void lifehash_image_free(LifeHashImage* image);

// Make a LifeHash from a UTF-8 string, which may be of any length.
// The caller is responsible to ensure that the string has undergone any
// necessary Unicode normalization in order to produce consistent results.
//
// The caller is responsible to release the returned image by calling
// `lifehash_image_free()`.
LifeHashImage* lifehash_make_from_utf8(const char* s, LifeHashVersion version, size_t module_size);

// Make a LifeHash from given data, which may be of any size.
//
// The caller is responsible to release the returned image by calling
// `lifehash_image_free()`.
LifeHashImage* lifehash_make_from_data(const uint8_t* data, size_t len, LifeHashVersion version, size_t module_size);

// Make a LifeHash from the SHA256 digest of some other data.
// The digest must be exactly 32 pseudorandom bytes. This is the base
// LifeHash creation algorithm, but if you don't already have a SHA256 hash of
// some data, then you should access it by calling `lifehash_make_from_data()`. If you
// are starting with a UTF-8 string, call `lifehash_make_from_utf8()`.
//
// The caller is responsible to release the returned image by calling
// `lifehash_image_free()`.
LifeHashImage* lifehash_make_from_digest(const uint8_t* digest, LifeHashVersion version, size_t module_size);

// Convert the given data to hexadecimal.
// The caller is responsible to release the returned string by calling `free()`.
char* lifehash_data_to_hex(const uint8_t* data, size_t len);

// Convert the given hexadecimal string to binary data.
// Returns `true` if successful, and `false` if the string is invalid hexadecimal.
// When successful, the caller is responsible to release the returned data by calling `free()`.
bool lifehash_hex_to_data(const uint8_t* utf8, size_t utf8_len, uint8_t** out, size_t* out_len);

#ifdef __cplusplus
}
#endif

#endif
