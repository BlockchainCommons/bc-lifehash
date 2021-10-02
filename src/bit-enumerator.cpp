#include "bit-enumerator.hpp"

#include <stdexcept>

namespace LifeHash
{

BitEnumerator::BitEnumerator(const Data& data) : data(data) {}

bool BitEnumerator::has_next() const { return mask != 0 || index != data.size() - 1; }

bool BitEnumerator::next() {
    if (!has_next()) {
        throw std::domain_error("BitEnumerator underflow.");
    }

    if (mask == 0) {
        mask = 0x80;
        index++;
    }

    auto b = (data[index] & mask) != 0;

    mask >>= 1;

    return b;
}

int BitEnumerator::next_uint2() {
    int bitMask = 0x02;
    int value = 0;
    for (int i = 0; i < 2; i++) {
        if (next()) {
            value |= bitMask;
        }
        bitMask >>= 1;
    }
    return value;
}

int BitEnumerator::next_uint8() {
    int bitMask = 0x80;
    int value = 0;
    for (int i = 0; i < 8; i++) {
        if (next()) {
            value |= bitMask;
        }
        bitMask >>= 1;
    }
    return value;
}

int BitEnumerator::next_uint16() {
    int bitMask = 0x8000;
    int value = 0;
    for (int i = 0; i < 16; i++) {
        if (next()) {
            value |= bitMask;
        }
        bitMask >>= 1;
    }
    return value;
}

double BitEnumerator::next_frac() { return next_uint16() / 65535.0; }

} // namespace LifeHash
