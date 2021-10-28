#include <string>
#include "hex.hpp"

using namespace std;

namespace LifeHash {

string byte_to_hex(uint8_t byte) {
    auto hex = "0123456789abcdef";
    string result;
    result.append(1, hex[(byte >> 4) & 0xF]);
    result.append(1, hex[byte & 0xF]);
    return result;
}

string data_to_hex(const Data& in) {
    string result;
    for(auto c: in) {
        result.append(byte_to_hex(c));
    }
    return result;
}

uint8_t hex_digit_to_bin(char hex) {
    if (hex >= '0' && hex <= '9') {
        return hex - '0';
    } else if (hex >= 'A' && hex <= 'F') {
        return hex - 'A' + 10;
    } else if (hex >= 'a' && hex <= 'f') {
        return hex - 'a' + 10;
    } else {
        throw domain_error("Invalid hex digit");
    }
}

Data hex_to_data(const string& hex) {
    Data result;

    auto len = hex.length();
    if(len % 2 != 0) {
        throw domain_error("Hex string must have even number of characters.");
    }
    auto count = len / 2;
    result.reserve(count);
    for(auto i = 0; i < count; i++) {
        auto b1 = hex_digit_to_bin(hex[i * 2]);
        auto b2 = hex_digit_to_bin(hex[i * 2 + 1]);
        result.push_back((b1 << 4) | b2);
    }

    return result;
}

}
