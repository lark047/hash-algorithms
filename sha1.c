#include "sha.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define SHA1_LENGTH  (40 + 1)

/* functions called by SHAstring */
extern uint32_t append_padding(uint8_t **, const char *, uint32_t *, struct hash_info *);
static void append_length(uint8_t *, const uint64_t, const uint32_t, const uint16_t);
static void process(uint8_t **, const uint32_t, const uint16_t);

/* hash function */
static uint32_t h(uint8_t, uint32_t, uint32_t, uint32_t);

/* hash registers */
static uint32_t h0, h1, h2, h3, h4;

/* values table */
static uint32_t K[] = {
    0x5a827999, 0x6ed9eba1, 0x8f1bbcdc, 0xca62c1d6
};

/* pointer to 32-bit word blocks */
static const uint8_t *M;

uint8_t *SHA1string(const char *msg)
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
    print_d(digest, block_count, info);
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

    /* the initial hash value, H(0), shall consist of the following five 32-bit words, in hex: */

    h0 = 0x67452301;
    h1 = 0xefcdab89;
    h2 = 0x98badcfe;
    h3 = 0x10325476;
    h4 = 0xc3d2e1f0;

    /**
     * SHA-1
     *
     * SHA-1 may be used to hash a message, M, having a length of l bits, where 0 <= l < 2^64. The
     * algorithm uses 1) a message schedule of eighty 32-bit words, 2) five working variables of 32 bits
     * each, and 3) a hash value of five 32-bit words. The final result of SHA-1 is a 160-bit message
     * digest.
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
    uint32_t W[ROUNDS];

    /* temporary registers */
    uint32_t $0, $1, $2, $3, $4, T;

    for (uint32_t block = 0; block < block_count; ++block)
    {
        M = *digest + block * block_size;

        for (uint8_t t = 0; t < ROUNDS; ++t)
        {
            if (t < 16)
            {
                W[t] = PREPARE(M, t);
            }
            else
            {
                W[t] = ROTL(W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16], 1);
            }
        }

        $0 = h0;
        $1 = h1;
        $2 = h2;
        $3 = h3;
        $4 = h4;

        for (uint8_t t = 0; t < ROUNDS; ++t)
        {
            T = ROTL($0, 5) + h(t, $1, $2, $3) + $4 + K[(uint8_t) (t / 20)] + W[t];

            $4 = $3;
            $3 = $2;
            $2 = ROTL($1, 30);
            $1 = $0;
            $0 = T;
        }

        h0 += $0;
        h1 += $1;
        h2 += $2;
        h3 += $3;
        h4 += $4;
    }

    free(*digest);
    *digest = malloc(SHA1_LENGTH);

    snprintf((char *) *digest, SHA1_LENGTH, "%08x%08x%08x%08x%08x", h0, h1, h2, h3, h4);
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
