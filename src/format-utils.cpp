#include "format-utils.hpp"

#include "bit-enumerator.hpp"

using namespace std;

namespace LifeHash
{

const string to_hex(const Data& data) {
    auto hex = "0123456789abcdef";
    string result;
    for (auto c : data) {
        result.append(1, hex[(c >> 4) & 0xF]);
        result.append(1, hex[c & 0xF]);
    }
    return result;
}

const Data to_data(const string& utf8) { return Data(utf8.begin(), utf8.end()); }

const string to_binary(const Data& data) {
    auto e = BitEnumerator(data);
    string result;
    e.for_all([&](bool b) { result.push_back(b ? '1' : '0'); });
    return result;
}

} // namespace LifeHash
