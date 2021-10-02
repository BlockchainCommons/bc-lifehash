#include "bit-aggregator.hpp"

namespace LifeHash
{

void BitAggregator::append(bool bit) {
    if (bitMask == 0) {
        bitMask = 0x80;
        _data.push_back(0);
    }

    if (bit) {
        _data[_data.size() - 1] |= bitMask;
    }

    bitMask >>= 1;
}

Data BitAggregator::data() const {
    return _data;
}

} // namespace LifeHash
