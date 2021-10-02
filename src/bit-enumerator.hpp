#ifndef LIFEHASH_BIT_ENUMERATOR_HPP
#define LIFEHASH_BIT_ENUMERATOR_HPP

#include <stdint.h>

#include <functional>

#include "data.hpp"

namespace LifeHash
{

// A class that takes a block of data and returns its bits singularly or in clusters.
struct BitEnumerator {
   private:
    const Data data;
    size_t index = 0;
    uint8_t mask = 0x80;

   public:
    BitEnumerator(const Data& data);

    bool has_next() const;
    bool next();
    int next_uint2();
    int next_uint8();
    int next_uint16();
    double next_frac();

    inline void for_all(const std::function<void(bool)>& f) {
        while (has_next()) {
            f(next());
        }
    }
};

} //namespace LifeHash
#endif
