#ifndef LIFEHASH_FORMAT_UTILS_HPP
#define LIFEHASH_FORMAT_UTILS_HPP

#include <string>

#include "data.hpp"

namespace LifeHash
{

// Convert the given data to a hex string.
const std::string to_hex(const Data& data);

// Convert the given UTF-8 string to a block of data.
const Data to_data(const std::string& utf8);

// Covert the given block of data to a string of 1s and 0s.
const std::string to_binary(const Data& data);

} // namespace LifeHash

#endif
