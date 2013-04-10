#include "sha.h"
#include "util.h"

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* SHA1 general hash function */
uint8_t *SHA1(const uint8_t *, uint64_t);

/* functions called by SHA1 */
static void append_length(uint8_t *, uint64_t, uint64_t, uint8_t);
static void process(const uint8_t *, uint64_t, uint8_t);

/* hash function */
static uint32_t h(uint8_t, uint32_t, uint32_t, uint32_t);

/* hash registers */
static uint32_t H[5];

/* values table */
static const uint32_t K[] = {
    0x5a827999,
    0x6ed9eba1,
    0x8f1bbcdc,
    0xca62c1d6
};

/* pointer to 32-bit word blocks */
static const uint8_t *M;

uint8_t *SHA1file(FILE *fp)
{
    return hash_file(fp, SHA1);
}

uint8_t *SHA1string(const char *msg)
{
    PRINT("found newline in msg: %s\n", (strchr(msg, '\n') ? "true" : "false"));
    return SHA1((uint8_t *) msg, strlen(msg));
}

uint8_t *SHA1(const uint8_t *msg, uint64_t msg_length)
{
    struct hash_info info = {
        BLOCK_LENGTH_BITS,
        PAD_MSG_TO_LENGTH_BITS,
        DIGEST_LENGTH_BITS
    };

    uint8_t *buffer;

    /* length in bytes */
    PRINT("Message length: %llu byte%s\n", msg_length, (msg_length == 1 ? "" : "s"));

    /**
     * Padding the Message
     *
     * Suppose that the length of the message, M, is l bits. Append the bit "1" to the end of the
     * message, followed by k zero bits, where k is the smallest, non-negative solution to the equation
     * (l + 1 + k) % 512 = 448. Then append the 64-bit block that is equal to the number
     * l expressed using a binary representation
     */

    const uint64_t l = msg_length * CHAR_BIT;
    PRINT("Message length: %llu bits\n", l);

    const uint64_t padded_length = append_padding(&buffer, msg, msg_length, &info);

    append_length(buffer, l, padded_length, info.block_length / CHAR_BIT);
    const uint64_t N = padded_length + (info.digest_length - info.pad_msg_to_length) / CHAR_BIT;
    PRINT("buffer is %llu bytes long\n", N);

#ifdef DEBUG
    print_d(buffer, N / info.block_length, &info);
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

    /* For SHA-1, the initial hash value, H(0), shall consist of the following five 32-bit words, in hex: */

    H[0] = 0x67452301;
    H[1] = 0xefcdab89;
    H[2] = 0x98badcfe;
    H[3] = 0x10325476;
    H[4] = 0xc3d2e1f0;

    /**
     * SHA-1
     *
     * SHA-1 may be used to hash a message, M, having a length of l bits, where 0 <= l < 2^64. The
     * algorithm uses 1) a message schedule of eighty 32-bit words, 2) five working variables of 32 bits
     * each, and 3) a hash value of five 32-bit words. The final result of SHA-1 is a 160-bit message
     * digest.
     */

    process(buffer, N, info.block_length);
    free(buffer);

    uint8_t *digest = malloc(DIGEST_LENGTH * sizeof *digest);
    PRINT("allocated %u bytes\n", DIGEST_LENGTH);

    for (uint8_t i = 0, bytes = DIGEST_LENGTH / 5; i < 5; ++i)
    {
        snprintf((char *) digest + i * bytes, bytes + 1, "%08x", H[i]);
    }

    return digest;
}

void append_length(uint8_t *buffer, uint64_t length, uint64_t padded_index, uint8_t block_length)
{
    /* assume length < 2^64 */
    for (uint8_t i = 0; i < block_length; ++i)
    {
        buffer[padded_index + i] = (length >> (CHAR_BIT * (7 - i))) & 0xff;
    }
}

void process(const uint8_t *buffer, uint64_t N, uint8_t block_length)
{
    uint32_t W[ROUNDS];

    /* temporary registers */
    uint32_t $0, $1, $2, $3, $4, T;

    for (uint64_t b = 0; b < N / block_length; ++b)
    {
        M = buffer + b * block_length;

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

        $0 = H[0];
        $1 = H[1];
        $2 = H[2];
        $3 = H[3];
        $4 = H[4];

        for (uint8_t t = 0; t < ROUNDS; ++t)
        {
            T = ROTL($0, 5) + h(t, $1, $2, $3) + $4 + K[(uint8_t) (t / 20)] + W[t];

            $4 = $3;
            $3 = $2;
            $2 = ROTL($1, 30);
            $1 = $0;
            $0 = T;
        }

        H[0] += $0;
        H[1] += $1;
        H[2] += $2;
        H[3] += $3;
        H[4] += $4;
    }
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
