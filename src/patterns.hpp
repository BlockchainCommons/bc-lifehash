#ifndef LIFEHASH_PATTERNS_HPP
#define LIFEHASH_PATTERNS_HPP

#include "version.hpp"
#include "bit-enumerator.hpp"

namespace LifeHash
{

// The symmetries used by LifeHash
enum class Pattern {
    snowflake, // Mirror around central axes.
    pinwheel, // Rotate around center.
    fiducial // Identity.
};

// A function that takes a deterministic source of bits and selects a pattern
// used to add symmetry to a a particular LifeHash version.
Pattern select_pattern(BitEnumerator& entropy, LifeHash::Version version);

} // namespace LifeHash

#endif
