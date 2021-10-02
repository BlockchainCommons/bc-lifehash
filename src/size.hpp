#ifndef LIFEHASH_SIZE_HPP
#define LIFEHASH_SIZE_HPP

#include <cstddef>

namespace LifeHash
{

// A struct representing an integer 2-dimensional size.
struct Size {
    int width;
    int height;

    Size(int width, int height) : width(width), height(height) {}
};

} // namespace LifeHash

#endif
