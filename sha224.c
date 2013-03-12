#include "sha.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define SHA224_LENGTH  (56 + 1)

/* functions called by SHAstring */
extern uint32_t append_padding(uint8_t **, const char *, uint32_t *, struct hash_info *);
static void append_length(uint8_t *, const uint64_t, const uint32_t, const uint16_t);
static void process(uint8_t **, const uint32_t, const uint16_t);

/* hash functions */
#define CH(x,y,z)                        (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z)          (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define SIGMA0(x)     (ROTR((x),  2) ^ ROTR((x), 13) ^ ROTR((x), 22))
#define SIGMA1(x)     (ROTR((x),  6) ^ ROTR((x), 11) ^ ROTR((x), 25))
#define sigma0(x)     (ROTR((x),  7) ^ ROTR((x), 18) ^  SHR((x),  3))
#define sigma1(x)     (ROTR((x), 17) ^ ROTR((x), 19) ^  SHR((x), 10))

/* hash registers */
static uint32_t h0, h1, h2, h3, h4, h5, h6, h7;

/* values table */
static uint32_t K[] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

/* TODO
 * These words represent the first thirty-two bits of the fractional parts of
 * the cube roots of the first sixty-four prime numbers
 */

/* pointer to 32-bit word blocks */
static const uint8_t *M;

uint8_t *SHAstring(const char *msg)
{
    struct hash_info *info = malloc(sizeof *info);

    info->block_size = BLOCK_SIZE_BITS;
    info->padded_length = PADDED_LENGTH_BITS;
    info->digest_length = DIGEST_LENGTH_BITS;

    uint8_t *digest;

    /* length in bytes */
    const uint32_t message_length = strlen(msg);
    PRINT("Message length: %u bytes\n", message_length);

    /**
     * Padding the Message
     *
     * Suppose that the length of the message, M, is l bits. Append the bit "1" to the end of the
     * message, followed by k zero bits, where k is the smallest, non-negative solution to the equation
     * (l + 1 + k) % 512 = 448. Then append the 64-bit block that is equal to the number
     * l expressed using a binary representation
     */

    const uint64_t l = message_length * CHAR_BIT;
    PRINT("Message length: %llu bits\n", l);

    uint32_t padded_length = message_length;
    const uint32_t block_count = append_padding(&digest, msg, &padded_length, info);

    PRINT("padded length = %u\n", padded_length);
    append_length(digest, l, padded_length, info->block_size);

#ifdef DEBUG
    print_d(digest, padded_length);
#endif

    /**
     * Parsing the Message
     *
     * The message and its padding must be parsed into N m-bit blocks.
     *
     * [T]he message and its padding are parsed into N 512-bit blocks,
     * M(1), M(2),...,M(N). Since the 512 bits of the input block may be expressed as sixteen 32-
     * bit words, the first 32 bits of message block i are denoted M(i,0), the next 32 bits are
     * M(i,1), and so on up to M(i,15).
     */

#if 0
    uint8_t *M;
#endif

    /**
     * Setting the Initial Hash Value (H(0))
     *
     * Before hash computation begins for each of the secure hash algorithms, the initial hash value,
     * H(0), must be set. The size and number of words in H(0) depends on the message digest size.
     */

    /* the initial hash value, H(0), shall consist of the following eight 32-bit words, in hex: */

    h0 = 0xc1059ed8;
    h1 = 0x367cd507;
    h2 = 0x3070dd17;
    h3 = 0xf70e5939;
    h4 = 0xffc00b31;
    h5 = 0x68581511;
    h6 = 0x64f98fa7;
    h7 = 0xbefa4fa4;

    /**
     * TODO
     * These words were obtained by taking the first thirty-two bits of the fractional parts of the square
     * roots of the first eight prime numbers.
     */

    /**
     * SHA-256
     *
     * SHA-256 may be used to hash a message, M, having a length of l bits, where 0 <= l < 2^64. The
     * algorithm uses 1) a message schedule of sixty-four 32-bit words, 2) eight working variables of 32
     * bits each, and 3) a hash value of eight 32-bit words. The final result of SHA-256 is a 256-bit
     * message digest.
     */

    process(&digest, block_count, info->block_size);

    /* clean up */
    free(info);
    info = NULL;

    return digest;
}

void append_length(uint8_t *digest, const uint64_t length, const uint32_t index, const uint16_t block_size)
{
    const uint8_t len_bytes = block_size / CHAR_BIT;

    /* assume length < 2^64 */
    for (uint8_t i = len_bytes; i > 0; --i)
    {
        digest[index - i] = (length >> (CHAR_BIT * (i - 1))) & 0xff;
    }
}

void process(uint8_t **digest, const uint32_t block_count, const uint16_t block_size)
{
    uint32_t W[64];

    /* temporary registers */
    uint32_t $0, $1, $2, $3, $4, $5, $6, $7, T[2];

    for (uint32_t block = 0; block < block_count; ++block)
    {
        M = *digest + block * block_size;

        for (uint8_t t = 0; t < 64; ++t)
        {
            if (t < 16)
            {
                W[t] = (M[4 * t + 0] << 24)
                     + (M[4 * t + 1] << 16)
                     + (M[4 * t + 2] <<  8)
                     + (M[4 * t + 3] <<  0);
            }
            else
            {
                W[t] = sigma1(W[t - 2]) + W[t - 7] + sigma0(W[t - 15]) + W[t - 16];
            }
        }

        $0 = h0;
        $1 = h1;
        $2 = h2;
        $3 = h3;
        $4 = h4;
        $5 = h5;
        $6 = h6;
        $7 = h7;

        for (uint8_t t = 0; t < 64; ++t)
        {
            T[0] = $7 + SIGMA1($4) + CH($4, $5, $6) + K[t] + W[t];
            T[1] = SIGMA0($0) + MAJ($0, $1, $2);

            $7 = $6;
            $6 = $5;
            $5 = $4;
            $4 = $3 + T[0];
            $3 = $2;
            $2 = $1;
            $1 = $0;
            $0 = T[0] + T[1];
        }

        h0 += $0;
        h1 += $1;
        h2 += $2;
        h3 += $3;
        h4 += $4;
        h5 += $5;
        h6 += $6;
        h7 += $7;
    }

    free(*digest);
    *digest = malloc(SHA224_LENGTH);

    snprintf((char *) *digest, SHA224_LENGTH, "%08x%08x%08x%08x%08x%08x%08x", h0, h1, h2, h3, h4, h5, h6);
}

uint32_t h(uint8_t i, uint32_t x, uint32_t y, uint32_t z)
{
    switch ((uint8_t) (i / 20))
    {
        case 0: /*  0 <= i < 20 */ return (x & y) ^ (~x & z);
        case 1: /* 20 <= i < 40 */ return x ^ y ^ z;
        case 2: /* 40 <= i < 60 */ return (x & y) ^ (x & z) ^ (y & z);
        case 3: /* 60 <= i < 80 */ return x ^ y ^ z;
    }

    return 0;
}
