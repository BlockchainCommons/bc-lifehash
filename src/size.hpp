#ifndef LIFEHASH_SIZE_HPP
#define LIFEHASH_SIZE_HPP

#include <cstddef>

// A struct representing an integer 2-dimensional size.
struct Size {
    std::size_t width;
    std::size_t height;

    Size(std::size_t width, std::size_t height) : width(width), height(height) {}
};

#endif
