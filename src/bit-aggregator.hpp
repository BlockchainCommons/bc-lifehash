#ifndef LIFEHASH_BIT_AGGREGATOR
#define LIFEHASH_BIT_AGGREGATOR

#include <stdint.h>

#include "data.hpp"

namespace LifeHash
{

// A class that accumulates bits fed into it and returns a block of data containing those bits.
class BitAggregator {
   private:
    Data _data;
    uint8_t bitMask = 0;

   public:
    void append(bool bit);
    Data data() const;
};

} // namespace LifeHash

#endif
