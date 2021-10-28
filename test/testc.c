#include "../src/lifehash.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

static void test_lifehash() {
    LifeHashImage* image = lifehash_make_from_utf8("Hello", lifehash_version2, 1, false);
    assert(image->width == 32);
    assert(image->height == 32);
    uint8_t expected[] = { 146, 126, 130, 178, 104, 92, 182, 101, 87, 202, 88, 64, 199, 89, 66, 197, 90, 69, 182, 101, 87, 180, 102, 89, 159, 117, 114, 210, 82, 54 };
    for(size_t i = 0; i < 30; i++) {
        assert(image->colors[i] == expected[i]);
    }
    lifehash_image_free(image);
}

static void test_lifehash_with_alpha() {
    LifeHashImage* image = lifehash_make_from_utf8("Hello", lifehash_version2, 1, true);
    assert(image->width == 32);
    assert(image->height == 32);
    uint8_t expected[] = { 146, 126, 130, 255, 178, 104, 92, 255, 182, 101, 87, 255, 202, 88, 64, 255, 199, 89, 66, 255, 197, 90, 69, 255, 182, 101, 87, 255, 180, 102, 89, 255, 159, 117, 114, 255, 210, 82, 54, 255 };
    for(size_t i = 0; i < 40; i++) {
        assert(image->colors[i] == expected[i]);
    }
    lifehash_image_free(image);
}

static void test_hex() {
    uint8_t data[] = {0x00, 0x01, 0x02, 0x03, 0xff};
    char* hex = lifehash_data_to_hex(&data[0], 5);
    assert(strcmp(hex, "00010203ff") == 0);
    uint8_t* data2 = NULL;
    size_t data2_len = 0;
    assert(lifehash_hex_to_data(hex, strlen(hex), &data2, &data2_len));
    assert(data2_len == 5);
    assert(memcmp(&data[0], data2, data2_len) == 0);
    free(hex);
    free(data2);
}

static void test_digest() {
    char* s = "Hello";
    uint8_t digest[32];
    lifehash_sha256(s, strlen(s), digest);
    char* hex = lifehash_data_to_hex(digest, 32);
    assert(strcmp(hex, "185f8db32271fe25f561a6fc938b2e264306ec304eda518007d1764826381969") == 0);
    free(hex);
}

int main() {
    test_lifehash();
    test_lifehash_with_alpha();
    test_hex();
    test_digest();
    return 0;
}
