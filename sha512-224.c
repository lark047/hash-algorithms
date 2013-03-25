#include "sha.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* SHA512/224 requires SHA512 */
extern uint8_t *SHA512_with_initial_values(uint8_t *, const uint64_t, const uint64_t *);

/* SHA512/224 general hash function */
uint8_t *SHA512224(uint8_t *, const uint64_t);

/* functions called by SHAstring */
extern uint64_t append_padding(uint8_t **, uint8_t *, uint64_t *, struct hash_info *);
static void append_length(uint8_t *, const uint64_t, const uint32_t, const uint16_t);
static void generate_initial_hash_values(uint16_t);
static void process(uint8_t **, const uint32_t, const uint16_t);

/* hash functions defined in sha.h */

/* hash registers */
static uint64_t H[8];

/* values table */
static const uint64_t K[] = {
    0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
    0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
    0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
    0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694,
    0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
    0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
    0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4,
    0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70,
    0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
    0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
    0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30,
    0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
    0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
    0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
    0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
    0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b,
    0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
    0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
    0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
    0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817
};

/* TODO
 * These words represent the first sixty-four bits of the fractional parts of
 * the cube roots of the first eighty prime numbers
 */

/* pointer to 32-bit word blocks */
static const uint8_t *M;

extern uint8_t *hash_file(FILE *, uint8_t *(*hash)(uint8_t *, const uint64_t));

uint8_t *SHA512224file(FILE *fp)
{
    return hash_file(fp, SHA512224);
}

uint8_t *SHA512224string(const char *msg)
{
    PRINT("found newline in msg: %s\n", (strchr(msg, '\n') ? "true" : "false"));
    return SHA512224((uint8_t *) msg, strlen(msg));
}

uint8_t *SHA512224(uint8_t *msg, const uint64_t msg_length)
{
    struct hash_info *info = malloc(sizeof *info);

    info->block_size = BLOCK_SIZE_BITS;
    info->padded_length = PADDED_LENGTH_BITS;
    info->digest_length = DIGEST_LENGTH_BITS;

    uint8_t *digest;

    /* length in bytes */
    PRINT("Message length: %llu byte%s\n", msg_length, (msg_length == 1 ? "" : "s"));

    /**
     * Padding the Message
     *
     * Suppose that the length of the message, M, is l bits. Append the bit "1" to the end of the
     * message, followed by k zero bits, where k is the smallest, non-negative solution to the equation
     * (l + 1 + k) % 1024 = 896. Then append the 128-bit block that is equal to the number
     * l expressed using a binary representation
     */

    const uint64_t l = msg_length * CHAR_BIT;
    PRINT("Message length: %llu bits\n", l);

    uint64_t padded_length = msg_length;
    const uint64_t block_count = append_padding(&digest, msg, &padded_length, info);

    PRINT("padded length = %llu\n", padded_length);
    append_length(digest, l, padded_length, info->block_size);

#ifdef DEBUG
    print_d(digest, block_count, info);
#endif

    /**
     * Parsing the Message
     *
     * The message and its padding must be parsed into N m-bit blocks.
     *
     * [T]he message and its padding are parsed into N 1024-bit blocks,
     * M(1), M(2),...,M(N). Since the 1024 bits of the input block may be expressed as sixteen 64-
     * bit words, the first 64 bits of message block i are denoted M(i,0), the next 64 bits are
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

    /* the initial hash value, H(0), shall consist of the following eight 64-bit words, in hex: */

    H[0] = 0x6a09e667f3bcc908;
    H[1] = 0xbb67ae8584caa73b;
    H[2] = 0x3c6ef372fe94f82b;
    H[3] = 0xa54ff53a5f1d36f1;
    H[4] = 0x510e527fade682d1;
    H[5] = 0x9b05688c2b3e6c1f;
    H[6] = 0x1f83d9abfb41bd6b;
    H[7] = 0x5be0cd19137e2179;

    /**
     * TODO
     * These words were obtained by taking the first sixty-four bits of the fractional parts of the square
     * roots of the first eight prime numbers.
     */

    generate_initial_hash_values(224);

    /**
     * SHA-512/224
     *
     * SHA-512/224 may be used to hash a message, M, having a length of l bits, where 0 <= l < 2^128. The
     * algorithm uses 1) a message schedule of eighty 64-bit words, 2) eight working variables of 64
     * bits each, and 3) a hash value of eight 64-bit words. The final result of SHA-512/224 is a 224-bit
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
    const uint64_t lengths[] = {
        (length >>  0), /* upper 32 bits */
        (length >> 32)  /* lower 32 bits */
    };

    /* assume length < 2^128 */
    for (uint8_t i = len_bytes; i > 8; --i)
    {
        digest[index - i + 8] = (lengths[0] >> (CHAR_BIT * (i - 1))) & 0xff;
        digest[index - i + 0] = (lengths[1] >> (CHAR_BIT * (i - 1))) & 0xff;
        PRINT("digest[%u] = 0x%02x\n", index - i + 0, digest[index - i + 0]);
        PRINT("digest[%u] = 0x%02x\n", index - i + 8, digest[index - i + 8]);
    }
}

void generate_initial_hash_values(uint16_t t)
{
    const uint64_t salt = 0xa5a5a5a5a5a5a5a5;
    const uint64_t H0[] = {
        H[0] ^ salt,
        H[1] ^ salt,
        H[2] ^ salt,
        H[3] ^ salt,
        H[4] ^ salt,
        H[5] ^ salt,
        H[6] ^ salt,
        H[7] ^ salt
    };

    const char *template = "SHA-512/%3u";
    char *msg = malloc(strlen(template));

    sprintf(msg, template, t);
    PRINT("Hashing \"%s\"...\n", msg);

    /* TODO need general SHA512/224 function? */
    uint8_t *tmp_digest = SHA512_with_initial_values((uint8_t *) msg, strlen(msg), H0);
    PRINT("Created hash value: %s\n", (char *) tmp_digest);

    free(msg);

#ifdef DEBUG
    uint8_t n = 0;
#endif

    for (uint8_t i = 0; i < SIZE(H); ++i)
    {
#ifdef DEBUG
        n +=
#endif
        sscanf((char *) tmp_digest + 16 * i, "%016llx", H + i);
    }

    PRINT("converted %u strings to int\n", n);
    PRINT("h0: 0x%016llx\n", H[0]);
    PRINT("h1: 0x%016llx\n", H[1]);
    PRINT("h2: 0x%016llx\n", H[2]);
    PRINT("h3: 0x%016llx\n", H[3]);
    PRINT("h4: 0x%016llx\n", H[4]);
    PRINT("h5: 0x%016llx\n", H[5]);
    PRINT("h6: 0x%016llx\n", H[6]);
    PRINT("h7: 0x%016llx\n", H[7]);

    free(tmp_digest);
}

void process(uint8_t **digest, const uint32_t block_count, const uint16_t block_size)
{
    uint64_t W[ROUNDS];

    /* temporary registers */
    uint64_t $0, $1, $2, $3, $4, $5, $6, $7, T[2];

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

    free(*digest);
    *digest = malloc(DIGEST_LENGTH);

    snprintf((char *) *digest, DIGEST_LENGTH, "%016llx%016llx%016llx%08x", H[0], H[1], H[2], (uint32_t) (H[3] >> 32));
}
