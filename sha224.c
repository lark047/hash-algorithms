#include "sha.h"
#include "util.h"

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* SHA224 general hash function */
uint8_t *SHA224(const uint8_t *, uint64_t);

/* functions called by SHA224 */
static void append_length(uint8_t *, uint64_t, uint64_t, uint8_t);
static void process(const uint8_t *, uint64_t, uint8_t);

/* hash functions defined in sha.h */

/* hash registers */
static uint32_t H[8];

/* values table */
static const uint32_t K[] = {
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

uint8_t *SHA224file(FILE *fp)
{
    return hash_file(fp, SHA224);
}

uint8_t *SHA224string(const char *msg)
{
    PRINT("found newline in msg: %s\n", (strchr(msg, '\n') ? "true" : "false"));
    return SHA224((uint8_t *) msg, strlen(msg));
}

uint8_t *SHA224(const uint8_t *msg, uint64_t msg_length)
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

    /**
     * For SHA-224, the initial hash value, H(0), shall consist of the following eight 32-bit words, in
     * hex:
     */

    H[0] = 0xc1059ed8;
    H[1] = 0x367cd507;
    H[2] = 0x3070dd17;
    H[3] = 0xf70e5939;
    H[4] = 0xffc00b31;
    H[5] = 0x68581511;
    H[6] = 0x64f98fa7;
    H[7] = 0xbefa4fa4;

    /**
     * TODO
     * (no accompanying text describing the origin of the above eight words)
     */

    /**
     * SHA-224
     *
     * SHA-224 may be used to hash a message, M, having a length of l bits, where 0 <= l < 2^64. The
     * function is defined in the exact same manner as SHA-256 (see sha256.c), with the following two
     * exceptions:
     *         1. The initial hash value, H(0), shall be set as specified [above]; and
     *         2. The 224-bit message digest is obtained by truncating the final hash value, H(N), to its
     *            left most 224 bits.
     */

    process(buffer, N, info.block_length);
    free(buffer);

    uint8_t *digest = malloc(DIGEST_LENGTH * sizeof *digest);
    PRINT("allocated %u bytes\n", DIGEST_LENGTH);

    flip(&H[0]);
    flip(&H[1]);
    flip(&H[2]);
    flip(&H[3]);
    flip(&H[4]);
    flip(&H[5]);
    flip(&H[6]);

    memcpy(digest, H, DIGEST_LENGTH);

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
    uint32_t $0, $1, $2, $3, $4, $5, $6, $7, T[2];

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
                W[t] = sigma1(W[t - 2]) + W[t - 7] + sigma0(W[t - 15]) + W[t - 16];
            }
        }

        $0 = H[0];
        $1 = H[1];
        $2 = H[2];
        $3 = H[3];
        $4 = H[4];
        $5 = H[5];
        $6 = H[6];
        $7 = H[7];

        for (uint8_t t = 0; t < ROUNDS; ++t)
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

        H[0] += $0;
        H[1] += $1;
        H[2] += $2;
        H[3] += $3;
        H[4] += $4;
        H[5] += $5;
        H[6] += $6;
        H[7] += $7;
    }
}
