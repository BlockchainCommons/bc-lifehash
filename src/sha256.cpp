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

#include "sha256.hpp"

#include <stdint.h>
#include <string.h>

#include "memzero.hpp"

namespace LifeHash
{

/*** SHA-256/384/512 Machine Architecture Definitions *****************/
/*
 * BYTE_ORDER NOTE:
 *
 * Please make sure that your system defines BYTE_ORDER.  If your
 * architecture is little-endian, make sure it also defines
 * LITTLE_ENDIAN and that the two (BYTE_ORDER and LITTLE_ENDIAN) are
 * equivilent.
 *
 * If your system does not define the above, then you can do so by
 * hand like this:
 *
 *   #define LITTLE_ENDIAN 1234
 *   #define BIG_ENDIAN    4321
 *
 * And for little-endian machines, add:
 *
 *   #define BYTE_ORDER LITTLE_ENDIAN
 *
 * Or for big-endian machines:
 *
 *   #define BYTE_ORDER BIG_ENDIAN
 *
 * The FreeBSD machine this was written on defines BYTE_ORDER
 * appropriately by including <sys/types.h> (which in turn includes
 * <machine/endian.h> where the appropriate definitions are actually
 * made).
 */

#if !defined(BYTE_ORDER) || (BYTE_ORDER != LITTLE_ENDIAN && BYTE_ORDER != BIG_ENDIAN)
#error Define BYTE_ORDER to be equal to either LITTLE_ENDIAN or BIG_ENDIAN
#endif

typedef uint8_t sha2_byte;    /* Exactly 1 byte */
typedef uint32_t sha2_word32; /* Exactly 4 bytes */

/*** SHA-256/384/512 Various Length Definitions ***********************/
/* NOTE: Most of these are in sha2.h */
#define SHA256_SHORT_BLOCK_LENGTH (SHA256_BLOCK_LENGTH - 8)

#define MEMCPY_BCOPY(d, s, l) memcpy((d), (s), (l))

/*** THE SIX LOGICAL FUNCTIONS ****************************************/
/*
 * Bit shifting and rotation (used by the six SHA-XYZ logical functions:
 *
 *   NOTE:  In the original SHA-256/384/512 document, the shift-right
 *   function was named R and the rotate-right function was called S.
 *   (See: http://csrc.nist.gov/cryptval/shs/sha256-384-512.pdf on the
 *   web.)
 *
 *   The newer NIST FIPS 180-2 document uses a much clearer naming
 *   scheme, SHR for shift-right, ROTR for rotate-right, and ROTL for
 *   rotate-left.  (See:
 *   http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 *   on the web.)
 *
 *   WARNING: These macros must be used cautiously, since they reference
 *   supplied parameters sometimes more than once, and thus could have
 *   unexpected side-effects if used without taking this into account.
 */

/* Shift-right (used in SHA-256, SHA-384, and SHA-512): */
#define SHR(b, x) ((x) >> (b))
/* 32-bit Rotate-right (used in SHA-256): */
#define ROTR32(b, x) (((x) >> (b)) | ((x) << (32 - (b))))
/* 64-bit Rotate-right (used in SHA-384 and SHA-512): */
#define ROTR64(b, x) (((x) >> (b)) | ((x) << (64 - (b))))
/* 32-bit Rotate-left (used in SHA-1): */
#define ROTL32(b, x) (((x) << (b)) | ((x) >> (32 - (b))))

/* Two of six logical functions used in SHA-1, SHA-256, SHA-384, and SHA-512: */
#define Ch(x, y, z) (((x) & (y)) ^ ((~(x)) & (z)))
#define Maj(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

/* Four of six logical functions used in SHA-256: */
#define Sigma0_256(x) (ROTR32(2, (x)) ^ ROTR32(13, (x)) ^ ROTR32(22, (x)))
#define Sigma1_256(x) (ROTR32(6, (x)) ^ ROTR32(11, (x)) ^ ROTR32(25, (x)))
#define sigma0_256(x) (ROTR32(7, (x)) ^ ROTR32(18, (x)) ^ SHR(3, (x)))
#define sigma1_256(x) (ROTR32(17, (x)) ^ ROTR32(19, (x)) ^ SHR(10, (x)))

/*** SHA-XYZ INITIAL HASH VALUES AND CONSTANTS ************************/

/* Hash constant words K for SHA-256: */
static const sha2_word32 K256[64] = {0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL, 0xd807aa98UL, 0x12835b01UL, 0x243185beUL,
                                     0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL, 0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL, 0x2de92c6fUL, 0x4a7484aaUL,
                                     0x5cb0a9dcUL, 0x76f988daUL, 0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL, 0x06ca6351UL, 0x14292967UL, 0x27b70a85UL,
                                     0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL, 0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL,
                                     0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL, 0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL,
                                     0x682e6ff3UL, 0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL, 0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL};

/* Initial hash value H for SHA-256: */
const sha2_word32 sha256_initial_hash_value[8] = {0x6a09e667UL, 0xbb67ae85UL, 0x3c6ef372UL, 0xa54ff53aUL, 0x510e527fUL, 0x9b05688cUL, 0x1f83d9abUL, 0x5be0cd19UL};

/*
 * Constant used by SHA256/384/512_End() functions for converting the
 * digest to a readable hexadecimal character string:
 */
static const char* sha2_hex_digits = "0123456789abcdef";

/*** SHA-256: *********************************************************/
void sha256_Init(SHA256_CTX* context) {
    if (context == (SHA256_CTX*)0) {
        return;
    }
    MEMCPY_BCOPY(context->state, sha256_initial_hash_value, SHA256_DIGEST_LENGTH);
    memzero(context->buffer, SHA256_BLOCK_LENGTH);
    context->bitcount = 0;
}

void sha256_Transform(const sha2_word32* state_in, const sha2_word32* data, sha2_word32* state_out) {
    sha2_word32 a = 0, b = 0, c = 0, d = 0, e = 0, f = 0, g = 0, h = 0;
    sha2_word32 T1 = 0, T2 = 0, W256[16] = {0};
    int j = 0;

    /* Initialize registers with the prev. intermediate value */
    a = state_in[0];
    b = state_in[1];
    c = state_in[2];
    d = state_in[3];
    e = state_in[4];
    f = state_in[5];
    g = state_in[6];
    h = state_in[7];

    j = 0;
    do {
        /* Apply the SHA-256 compression function to update a..h with copy */
        T1 = h + Sigma1_256(e) + Ch(e, f, g) + K256[j] + (W256[j] = *data++);
        T2 = Sigma0_256(a) + Maj(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + T1;
        d = c;
        c = b;
        b = a;
        a = T1 + T2;

        j++;
    } while (j < 16);

    do {
        /* Part of the message block expansion: */
        sha2_word32 s0 = 0, s1 = 0;
        s0 = W256[(j + 1) & 0x0f];
        s0 = sigma0_256(s0);
        s1 = W256[(j + 14) & 0x0f];
        s1 = sigma1_256(s1);

        /* Apply the SHA-256 compression function to update a..h */
        T1 = h + Sigma1_256(e) + Ch(e, f, g) + K256[j] + (W256[j & 0x0f] += s1 + W256[(j + 9) & 0x0f] + s0);
        T2 = Sigma0_256(a) + Maj(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + T1;
        d = c;
        c = b;
        b = a;
        a = T1 + T2;

        j++;
    } while (j < 64);

    /* Compute the current intermediate hash value */
    state_out[0] = state_in[0] + a;
    state_out[1] = state_in[1] + b;
    state_out[2] = state_in[2] + c;
    state_out[3] = state_in[3] + d;
    state_out[4] = state_in[4] + e;
    state_out[5] = state_in[5] + f;
    state_out[6] = state_in[6] + g;
    state_out[7] = state_in[7] + h;

    /* Clean up */
    a = b = c = d = e = f = g = h = T1 = T2 = 0;
}

void sha256_Update(SHA256_CTX* context, const sha2_byte* data, size_t len) {
    unsigned int freespace = 0, usedspace = 0;

    if (len == 0) {
        /* Calling with no data is valid - we do nothing */
        return;
    }

    usedspace = (context->bitcount >> 3) % SHA256_BLOCK_LENGTH;
    if (usedspace > 0) {
        /* Calculate how much free space is available in the buffer */
        freespace = SHA256_BLOCK_LENGTH - usedspace;

        if (len >= freespace) {
            /* Fill the buffer completely and process it */
            MEMCPY_BCOPY(((uint8_t*)context->buffer) + usedspace, data, freespace);
            context->bitcount += freespace << 3;
            len -= freespace;
            data += freespace;
#if BYTE_ORDER == LITTLE_ENDIAN
            /* Convert TO host byte order */
            for (int j = 0; j < 16; j++) {
                REVERSE32(context->buffer[j], context->buffer[j]);
            }
#endif
            sha256_Transform(context->state, context->buffer, context->state);
        } else {
            /* The buffer is not yet full */
            MEMCPY_BCOPY(((uint8_t*)context->buffer) + usedspace, data, len);
            context->bitcount += len << 3;
            /* Clean up: */
            usedspace = freespace = 0;
            return;
        }
    }
    while (len >= SHA256_BLOCK_LENGTH) {
        /* Process as many complete blocks as we can */
        MEMCPY_BCOPY(context->buffer, data, SHA256_BLOCK_LENGTH);
#if BYTE_ORDER == LITTLE_ENDIAN
        /* Convert TO host byte order */
        for (int j = 0; j < 16; j++) {
            REVERSE32(context->buffer[j], context->buffer[j]);
        }
#endif
        sha256_Transform(context->state, context->buffer, context->state);
        context->bitcount += SHA256_BLOCK_LENGTH << 3;
        len -= SHA256_BLOCK_LENGTH;
        data += SHA256_BLOCK_LENGTH;
    }
    if (len > 0) {
        /* There's left-overs, so save 'em */
        MEMCPY_BCOPY(context->buffer, data, len);
        context->bitcount += len << 3;
    }
    /* Clean up: */
    usedspace = freespace = 0;
}

void sha256_Final(SHA256_CTX* context, sha2_byte digest[]) {
    unsigned int usedspace = 0;

    /* If no digest buffer is passed, we don't bother doing this: */
    if (digest != (sha2_byte*)0) {
        usedspace = (context->bitcount >> 3) % SHA256_BLOCK_LENGTH;
        /* Begin padding with a 1 bit: */
        ((uint8_t*)context->buffer)[usedspace++] = 0x80;

        if (usedspace > SHA256_SHORT_BLOCK_LENGTH) {
            memzero(((uint8_t*)context->buffer) + usedspace, SHA256_BLOCK_LENGTH - usedspace);

#if BYTE_ORDER == LITTLE_ENDIAN
            /* Convert TO host byte order */
            for (int j = 0; j < 16; j++) {
                REVERSE32(context->buffer[j], context->buffer[j]);
            }
#endif
            /* Do second-to-last transform: */
            sha256_Transform(context->state, context->buffer, context->state);

            /* And prepare the last transform: */
            usedspace = 0;
        }
        /* Set-up for the last transform: */
        memzero(((uint8_t*)context->buffer) + usedspace, SHA256_SHORT_BLOCK_LENGTH - usedspace);

#if BYTE_ORDER == LITTLE_ENDIAN
        /* Convert TO host byte order */
        for (int j = 0; j < 14; j++) {
            REVERSE32(context->buffer[j], context->buffer[j]);
        }
#endif
        /* Set the bit count: */
        context->buffer[14] = context->bitcount >> 32;
        context->buffer[15] = context->bitcount & 0xffffffff;

        /* Final transform: */
        sha256_Transform(context->state, context->buffer, context->state);

#if BYTE_ORDER == LITTLE_ENDIAN
        /* Convert FROM host byte order */
        for (int j = 0; j < 8; j++) {
            REVERSE32(context->state[j], context->state[j]);
        }
#endif
        MEMCPY_BCOPY(digest, context->state, SHA256_DIGEST_LENGTH);
    }

    /* Clean up state data: */
    memzero(context, sizeof(SHA256_CTX));
    usedspace = 0;
}

char* sha256_End(SHA256_CTX* context, char buffer[]) {
    sha2_byte digest[SHA256_DIGEST_LENGTH] = {0}, *d = digest;

    if (buffer != (char*)0) {
        sha256_Final(context, digest);

        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            *buffer++ = sha2_hex_digits[(*d & 0xf0) >> 4];
            *buffer++ = sha2_hex_digits[*d & 0x0f];
            d++;
        }
        *buffer = (char)0;
    } else {
        memzero(context, sizeof(SHA256_CTX));
    }
    memzero(digest, SHA256_DIGEST_LENGTH);
    return buffer;
}

void sha256_Raw(const sha2_byte* data, size_t len, uint8_t digest[SHA256_DIGEST_LENGTH]) {
    SHA256_CTX context = {0};
    sha256_Init(&context);
    sha256_Update(&context, data, len);
    sha256_Final(&context, digest);
}

char* sha256_Data(const sha2_byte* data, size_t len, char digest[SHA256_DIGEST_STRING_LENGTH]) {
    SHA256_CTX context = {0};

    sha256_Init(&context);
    sha256_Update(&context, data, len);
    return sha256_End(&context, digest);
}

const Data sha256(const Data& buf) {
    uint8_t digest[SHA256_DIGEST_LENGTH];
    sha256_Raw(buf.data(), buf.size(), digest);
    return Data(digest, digest + SHA256_DIGEST_LENGTH);
}

} // namespace LifeHash
