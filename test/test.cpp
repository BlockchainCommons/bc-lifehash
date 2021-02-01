#include "../src/lifehash.hpp"

#include <assert.h>
#include <string>
#include <vector>

int main() {
    auto image = LifeHash::make_from_utf8("Hello");
    assert(image.width == 32);
    assert(image.height == 32);
    std::vector<uint8_t> expected = { 146, 126, 130, 178, 104, 92, 182, 101, 87, 202, 88, 64, 199, 89, 66, 197, 90, 69, 182, 101, 87, 180, 102, 89, 159, 117, 114, 210, 82, 54 };
    for(auto i = 0; i < expected.size(); i++) {
        assert(image.colors[i] == expected[i]);
    }
    return 0;
}
