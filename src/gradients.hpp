#ifndef LIFEHASH_GRADIENTS_HPP
#define LIFEHASH_GRADIENTS_HPP

#include "version.hpp"
#include "bit-enumerator.hpp"
#include "color-func.hpp"

namespace LifeHash
{

// A function that takes a deterministic source of bits and selects a gradient
// used to color a particular LifeHash version.
ColorFunc select_gradient(BitEnumerator& entropy, LifeHash::Version version);

} // namespace LifeHash

#endif
