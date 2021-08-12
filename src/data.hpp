#ifndef LIFEHASH_DATA_HPP
#define LIFEHASH_DATA_HPP

#include <stdint.h>

#include <vector>

namespace LifeHash
{

// An idiom for a block of data used throughout LifeHash.
typedef std::vector<uint8_t> Data;

} // namespace LifeHash

#endif
