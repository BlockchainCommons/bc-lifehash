#include "../src/lifehash.hpp"

#include <assert.h>
#include <string>
#include <vector>

static void test_lifehash() {
    auto image = LifeHash::make_from_utf8("Hello");
    assert(image.width == 32);
    assert(image.height == 32);
    std::vector<uint8_t> expected = { 146, 126, 130, 178, 104, 92, 182, 101, 87, 202, 88, 64, 199, 89, 66, 197, 90, 69, 182, 101, 87, 180, 102, 89, 159, 117, 114, 210, 82, 54 };
    for(auto i = 0; i < expected.size(); i++) {
        assert(image.colors[i] == expected[i]);
    }
}

static void test_lifehash_with_alpha() {
    auto image = LifeHash::make_from_utf8("Hello", LifeHash::Version::version2, 1, true);
    assert(image.width == 32);
    assert(image.height == 32);
    std::vector<uint8_t> expected = { 146, 126, 130, 255, 178, 104, 92, 255, 182, 101, 87, 255, 202, 88, 64, 255, 199, 89, 66, 255, 197, 90, 69, 255, 182, 101, 87, 255, 180, 102, 89, 255, 159, 117, 114, 255, 210, 82, 54, 255 };
    for(auto i = 0; i < expected.size(); i++) {
        assert(image.colors[i] == expected[i]);
    }
}

static void test_hex() {
    assert(LifeHash::data_to_hex({0x00, 0x01, 0x02, 0x03, 0xff}) == "00010203ff");
    assert(LifeHash::hex_to_data("00010203ff") == std::vector<uint8_t>({0x00, 0x01, 0x02, 0x03, 0xff}));
}

static void test_digest() {
    std::string s = "Hello";
    auto data = std::vector<uint8_t>(s.begin(), s.end());
    auto digest = LifeHash::sha256(data);
    auto hex = LifeHash::data_to_hex(digest);
    assert(hex == "185f8db32271fe25f561a6fc938b2e264306ec304eda518007d1764826381969");
}

int main() {
    test_lifehash();
    test_lifehash_with_alpha();
    test_hex();
    test_digest();
    return 0;
}
