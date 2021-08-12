#include "patterns.hpp"

namespace LifeHash
{

Pattern select_pattern(BitEnumerator& entropy, Version version) {
    switch(version) {
        case Version::fiducial:
        case Version::grayscale_fiducial:
            return Pattern::fiducial;
        default:
            return entropy.next() ? Pattern::snowflake : Pattern::pinwheel;
    }
}

} // namespace LifeHash
