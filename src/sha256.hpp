/**
 * Copyright (c) 2000-2001 Aaron D. Gifford
 * Copyright (c) 2013-2014 Pavol Rusnak
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTOR(S) ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTOR(S) BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef LIFEHASH_SHA_256_HPP
#define LIFEHASH_SHA_256_HPP

#include <stddef.h>
#include <stdint.h>

#include "data.hpp"

namespace LifeHash
{

#define SHA256_BLOCK_LENGTH 64
#define SHA256_DIGEST_LENGTH 32
#define SHA256_DIGEST_STRING_LENGTH (SHA256_DIGEST_LENGTH * 2 + 1)

typedef struct _SHA256_CTX {
    uint32_t state[8];
    uint64_t bitcount;
    uint32_t buffer[SHA256_BLOCK_LENGTH / sizeof(uint32_t)];
} SHA256_CTX;

/*** ENDIAN REVERSAL MACROS *******************************************/
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1234
#define BIG_ENDIAN 4321
#endif

#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

#if BYTE_ORDER == LITTLE_ENDIAN
#define REVERSE32(w, x)                                                              \
    {                                                                                \
        uint32_t tmp = (w);                                                          \
        tmp = (tmp >> 16) | (tmp << 16);                                             \
        (x) = (uint32_t)(((tmp & 0xff00ff00UL) >> 8) | ((tmp & 0x00ff00ffUL) << 8)); \
    }
#endif /* BYTE_ORDER == LITTLE_ENDIAN */

extern const uint32_t sha256_initial_hash_value[8];

void sha256_Transform(const uint32_t* state_in, const uint32_t* data, uint32_t* state_out);
void sha256_Init(SHA256_CTX*);
void sha256_Update(SHA256_CTX*, const uint8_t*, size_t);
void sha256_Final(SHA256_CTX*, uint8_t[SHA256_DIGEST_LENGTH]);
char* sha256_End(SHA256_CTX*, char[SHA256_DIGEST_STRING_LENGTH]);
void sha256_Raw(const uint8_t*, size_t, uint8_t[SHA256_DIGEST_LENGTH]);
char* sha256_Data(const uint8_t*, size_t, char[SHA256_DIGEST_STRING_LENGTH]);

// Calculates the SHA256 digest of the given data.
const Data sha256(const Data& buf);

} // namespace LifeHash

#endif
