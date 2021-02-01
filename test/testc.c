#include "../src/lifehash.h"

#include <stdio.h>
#include <assert.h>

int main() {
    LifeHashImage* image = lifehash_make_from_utf8("Hello", lifehash_version2, 1);
    assert(image->width == 32);
    assert(image->height == 32);
    uint8_t expected[] = { 146, 126, 130, 178, 104, 92, 182, 101, 87, 202, 88, 64, 199, 89, 66, 197, 90, 69, 182, 101, 87, 180, 102, 89, 159, 117, 114, 210, 82, 54 };
    for(size_t i = 0; i < 30; i++) {
        assert(image->colors[i] == expected[i]);
    }
    lifehash_image_free(image);
    return 0;
}
