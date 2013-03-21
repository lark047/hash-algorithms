/**
 * This document describes the MD5 message-digest algorithm. The
 * algorithm takes as input a message of arbitrary length and produces
 * as output a 128-bit "fingerprint" or "message digest" of the input.
 * It is conjectured that it is computationally infeasible to produce
 * two messages having the same message digest, or to produce any
 * message having a given prespecified target message digest. The MD5
 * algorithm is intended for digital signature applications, where a
 * large file must be "compressed" in a secure manner before being
 * encrypted with a private (secret) key under a public-key cryptosystem
 * such as RSA.
 *
 * The MD5 algorithm is designed to be quite fast on 32-bit machines. In
 * addition, the MD5 algorithm does not require any large substitution
 * tables; the algorithm can be coded quite compactly.
 *
 * The MD5 algorithm is an extension of the MD4 message-digest algorithm
 * 1,2]. MD5 is slightly slower than MD4, but is more "conservative" in
 * design. MD5 was designed because it was felt that MD4 was perhaps
 * being adopted for use more quickly than justified by the existing
 * critical review; because MD4 was designed to be exceptionally fast,
 * it is "at the edge" in terms of risking successful cryptanalytic
 * attack. MD5 backs off a bit, giving up a little in speed for a much
 * greater likelihood of ultimate security. It incorporates some
 * suggestions made by various reviewers, and contains additional
 * optimizations. The MD5 algorithm is being placed in the public domain
 * for review and possible adoption as a standard.
 *
 * For OSI-based applications, MD5's object identifier is
 *
 * md5 OBJECT IDENTIFIER ::=
 *   iso(1) member-body(2) US(840) rsadsi(113549) digestAlgorithm(2) 5}
 *
 * In the X.509 type AlgorithmIdentifier [3], the parameters for MD5
 * should have type NULL.
 */

#include "md5.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#define BITS_PER_WORD          32
#define WORDS_PER_BLOCK        16
#define TSIZE                  64
#define MD5_LENGTH       (32 + 1)

/* MD5 general hash function */
uint8_t *MD5(uint8_t *, const uint64_t);

/* functions called by MD5 */
extern uint64_t append_padding(uint8_t **, uint8_t *, uint64_t *, struct hash_info *);
static void append_length(uint8_t *, const uint64_t, const uint32_t, const uint16_t);
static void process(uint8_t **, const uint32_t, const uint16_t);

/* hash functions */
static uint32_t h(uint8_t, uint32_t, uint32_t, uint32_t);
static void r(uint32_t *, uint32_t, uint32_t, uint32_t, uint8_t, uint8_t, uint8_t);

/* hash registers */
static uint32_t H[4];

/* values table */
static uint32_t T[TSIZE] = {0};

/* pointer to 32-bit word blocks */
static const uint8_t *X;

/* helper functions */
static void init_table(void);
static void flip(uint32_t *);
extern uint32_t rotl(const uint32_t, const uint8_t);

#define K(x,m,a) (((x) * (m) + (a)) % WORDS_PER_BLOCK)
#define K1(x)                                      (x)
#define K2(x)                               K((x),5,1)
#define K3(x)                               K((x),3,5)
#define K4(x)                               K((x),7,0)

#define S(x,i)                       (s[(x)][(i) % 4])
#define S1(x)                      (((x) % 4) * 5 + 7)
#define S2(x)                                 S(0,(x))
#define S3(x)                                 S(1,(x))
#define S4(x)                                 S(2,(x))

extern uint8_t *hash_file(FILE *, uint8_t *(*hash)(uint8_t *, const uint64_t));

uint8_t *MD5file(FILE *fp)
{
    return hash_file(fp, MD5);
}

uint8_t *MD5string(const char *msg)
{
    PRINT("found newline in msg: %s\n", (strchr(msg, '\n') ? "true" : "false"));
    return MD5((uint8_t *) msg, strlen(msg));
}

uint8_t *MD5(uint8_t *msg, const uint64_t msg_length)
{
    struct hash_info *info = malloc(sizeof *info);

    info->block_size = BLOCK_SIZE_BITS;
    info->padded_length = PADDED_LENGTH_BITS;
    info->digest_length = DIGEST_LENGTH_BITS;

    uint8_t *digest;

    /* length in bytes */
    PRINT("Message length: %llu byte%s\n", msg_length, (msg_length == 1 ? "" : "s"));

    /**
     * We begin by supposing that we have a b-bit message as input, and that
     * we wish to find its message digest. Here b is an arbitrary
     * nonnegative integer; b may be zero, it need not be a multiple of
     * eight, and it may be arbitrarily large. We imagine the bits of the
     * message written down as follows:
     *
     *        m_0 m_1 ... m_{b-1}
     *
     * The following five steps are performed to compute the message digest
     * of the message.
     */

    const uint64_t b = msg_length * CHAR_BIT;
    PRINT("Message length: %llu bits\n", b);

    /**
     * Step 1. Append Padding Bits
     *
     * The message is "padded" (extended) so that its length (in bits) is
     * congruent to 448, modulo 512. That is, the message is extended so
     * that it is just 64 bits shy of being a multiple of 512 bits long.
     * Padding is always performed, even if the length of the message is
     * already congruent to 448, modulo 512.
     *
     * Padding is performed as follows: a single "1" bit is appended to the
     * message, and then "0" bits are appended so that the length in bits of
     * the padded message becomes congruent to 448, modulo 512. In all, at
     * least one bit and at most 512 bits are appended.
     */

    uint64_t padded_length = msg_length;
    const uint64_t block_count = append_padding(&digest, msg, &padded_length, info);

    /**
     * Step 2. Append Length
     *
     * A 64-bit representation of b (the length of the message before the
     * padding bits were added) is appended to the result of the previous
     * step. In the unlikely event that b is greater than 2^64, then only
     * the low-order 64 bits of b are used. (These bits are appended as two
     * 32-bit words and appended low-order word first in accordance with the
     * previous conventions.)
     *
     * At this point the resulting message (after padding with bits and with
     * b) has a length that is an exact multiple of 512 bits. Equivalently,
     * this message has a length that is an exact multiple of 16 (32-bit)
     * words. Let M[0 ... N-1] denote the words of the resulting message,
     * where N is a multiple of 16.
     */

    PRINT("padded length = %llu\n", padded_length);
    append_length(digest, b, padded_length, info->block_size);

#ifdef DEBUG
    print_d(digest, block_count, info);
#endif

    /**
     * Step 3. Initialize MD Buffer
     *
     * A four-word buffer (A,B,C,D) is used to compute the message digest.
     * Here each of A, B, C, D is a 32-bit register. These registers are
     * initialized to the following values in hexadecimal, low-order bytes
     * first):
     *
     *           word A: 01 23 45 67
     *           word B: 89 ab cd ef
     *           word C: fe dc ba 98
     *           word D: 76 54 32 10
     */

    H[0] = 0x67452301;
    H[1] = 0xefcdab89;
    H[2] = 0x98badcfe;
    H[3] = 0x10325476;

    /**
     * Step 4. Process Message in 16-Word Blocks
     *
     * We first define four auxiliary functions that each take as input
     * three 32-bit words and produce as output one 32-bit word.
     *
     *        F(X,Y,Z) = XY v not(X) Z
     *        G(X,Y,Z) = XZ v Y not(Z)
     *        H(X,Y,Z) = X xor Y xor Z
     *        I(X,Y,Z) = Y xor (X v not(Z))
     *
     * In each bit position F acts as a conditional: if X then Y else Z.
     * The function F could have been defined using + instead of v since XY
     * and not(X)Z will never have 1's in the same bit position.) It is
     * interesting to note that if the bits of X, Y, and Z are independent
     * and unbiased, the each bit of F(X,Y,Z) will be independent and
     * unbiased.
     *
     * The functions G, H, and I are similar to the function F, in that they
     * act in "bitwise parallel" to produce their output from the bits of X,
     * Y, and Z, in such a manner that if the corresponding bits of X, Y,
     * and Z are independent and unbiased, then each bit of G(X,Y,Z),
     * H(X,Y,Z), and I(X,Y,Z) will be independent and unbiased. Note that
     * the function H is the bit-wise "xor" or "parity" function of its
     * inputs.
     *
     * This step uses a 64-element table T[1 ... 64] constructed from the
     * sine function. Let T[i] denote the i-th element of the table, which
     * is equal to the integer part of 4294967296 times abs(sin(i)), where i
     * is in radians. The elements of the table are given in the appendix.
     */

    process(&digest, block_count, info->block_size);

    return digest;
}

void init_table()
{
    for (int i = 0; i < TSIZE; ++i)
    {
        T[i] = (1ULL << 32) * fabs(sin(i + 1));
    }
}

void append_length(uint8_t *digest, const uint64_t length, const uint32_t index, const uint16_t block_size)
{
    const uint8_t len_bytes = block_size / CHAR_BIT;

    /* assume length < 2^64 */
    for (uint8_t i = 0; i < len_bytes; ++i)
    {
        digest[index - len_bytes + i] = (length >> (CHAR_BIT * i)) & 0xff;
    }
}

void process(uint8_t **digest, const uint32_t block_count, const uint16_t block_size)
{
    init_table();

    const uint8_t s[][4] = {
        { 5,  9, 14, 20 },
        { 4, 11, 16, 23 },
        { 6, 10, 15, 21 }
    };

    /* temporary registers */
    uint32_t $0, $1, $2, $3;

    /* Process each 16-word block. */
    for (uint32_t block = 0, i = 0; block < block_count; ++block, i = 0)
    {
        X = *digest + block * block_size;

        /* Save H[0] as $0, H[1] as $1, H[2] as $2, and H[3] as $3. */
        $0 = H[0];
        $1 = H[1];
        $2 = H[2];
        $3 = H[3];

        /* Round 1. */
        r(&H[0], H[1], H[2], H[3], K1(i), S1(i), i); ++i;
        r(&H[3], H[0], H[1], H[2], K1(i), S1(i), i); ++i;
        r(&H[2], H[3], H[0], H[1], K1(i), S1(i), i); ++i;
        r(&H[1], H[2], H[3], H[0], K1(i), S1(i), i); ++i;
        r(&H[0], H[1], H[2], H[3], K1(i), S1(i), i); ++i;
        r(&H[3], H[0], H[1], H[2], K1(i), S1(i), i); ++i;
        r(&H[2], H[3], H[0], H[1], K1(i), S1(i), i); ++i;
        r(&H[1], H[2], H[3], H[0], K1(i), S1(i), i); ++i;
        r(&H[0], H[1], H[2], H[3], K1(i), S1(i), i); ++i;
        r(&H[3], H[0], H[1], H[2], K1(i), S1(i), i); ++i;
        r(&H[2], H[3], H[0], H[1], K1(i), S1(i), i); ++i;
        r(&H[1], H[2], H[3], H[0], K1(i), S1(i), i); ++i;
        r(&H[0], H[1], H[2], H[3], K1(i), S1(i), i); ++i;
        r(&H[3], H[0], H[1], H[2], K1(i), S1(i), i); ++i;
        r(&H[2], H[3], H[0], H[1], K1(i), S1(i), i); ++i;
        r(&H[1], H[2], H[3], H[0], K1(i), S1(i), i); ++i;

        /* Round 2. */
        r(&H[0], H[1], H[2], H[3], K2(i), S2(i), i); ++i;
        r(&H[3], H[0], H[1], H[2], K2(i), S2(i), i); ++i;
        r(&H[2], H[3], H[0], H[1], K2(i), S2(i), i); ++i;
        r(&H[1], H[2], H[3], H[0], K2(i), S2(i), i); ++i;
        r(&H[0], H[1], H[2], H[3], K2(i), S2(i), i); ++i;
        r(&H[3], H[0], H[1], H[2], K2(i), S2(i), i); ++i;
        r(&H[2], H[3], H[0], H[1], K2(i), S2(i), i); ++i;
        r(&H[1], H[2], H[3], H[0], K2(i), S2(i), i); ++i;
        r(&H[0], H[1], H[2], H[3], K2(i), S2(i), i); ++i;
        r(&H[3], H[0], H[1], H[2], K2(i), S2(i), i); ++i;
        r(&H[2], H[3], H[0], H[1], K2(i), S2(i), i); ++i;
        r(&H[1], H[2], H[3], H[0], K2(i), S2(i), i); ++i;
        r(&H[0], H[1], H[2], H[3], K2(i), S2(i), i); ++i;
        r(&H[3], H[0], H[1], H[2], K2(i), S2(i), i); ++i;
        r(&H[2], H[3], H[0], H[1], K2(i), S2(i), i); ++i;
        r(&H[1], H[2], H[3], H[0], K2(i), S2(i), i); ++i;

        /* Round 3. */
        r(&H[0], H[1], H[2], H[3], K3(i), S3(i), i); ++i;
        r(&H[3], H[0], H[1], H[2], K3(i), S3(i), i); ++i;
        r(&H[2], H[3], H[0], H[1], K3(i), S3(i), i); ++i;
        r(&H[1], H[2], H[3], H[0], K3(i), S3(i), i); ++i;
        r(&H[0], H[1], H[2], H[3], K3(i), S3(i), i); ++i;
        r(&H[3], H[0], H[1], H[2], K3(i), S3(i), i); ++i;
        r(&H[2], H[3], H[0], H[1], K3(i), S3(i), i); ++i;
        r(&H[1], H[2], H[3], H[0], K3(i), S3(i), i); ++i;
        r(&H[0], H[1], H[2], H[3], K3(i), S3(i), i); ++i;
        r(&H[3], H[0], H[1], H[2], K3(i), S3(i), i); ++i;
        r(&H[2], H[3], H[0], H[1], K3(i), S3(i), i); ++i;
        r(&H[1], H[2], H[3], H[0], K3(i), S3(i), i); ++i;
        r(&H[0], H[1], H[2], H[3], K3(i), S3(i), i); ++i;
        r(&H[3], H[0], H[1], H[2], K3(i), S3(i), i); ++i;
        r(&H[2], H[3], H[0], H[1], K3(i), S3(i), i); ++i;
        r(&H[1], H[2], H[3], H[0], K3(i), S3(i), i); ++i;

        /* Round 4. */
        r(&H[0], H[1], H[2], H[3], K4(i), S4(i), i); ++i;
        r(&H[3], H[0], H[1], H[2], K4(i), S4(i), i); ++i;
        r(&H[2], H[3], H[0], H[1], K4(i), S4(i), i); ++i;
        r(&H[1], H[2], H[3], H[0], K4(i), S4(i), i); ++i;
        r(&H[0], H[1], H[2], H[3], K4(i), S4(i), i); ++i;
        r(&H[3], H[0], H[1], H[2], K4(i), S4(i), i); ++i;
        r(&H[2], H[3], H[0], H[1], K4(i), S4(i), i); ++i;
        r(&H[1], H[2], H[3], H[0], K4(i), S4(i), i); ++i;
        r(&H[0], H[1], H[2], H[3], K4(i), S4(i), i); ++i;
        r(&H[3], H[0], H[1], H[2], K4(i), S4(i), i); ++i;
        r(&H[2], H[3], H[0], H[1], K4(i), S4(i), i); ++i;
        r(&H[1], H[2], H[3], H[0], K4(i), S4(i), i); ++i;
        r(&H[0], H[1], H[2], H[3], K4(i), S4(i), i); ++i;
        r(&H[3], H[0], H[1], H[2], K4(i), S4(i), i); ++i;
        r(&H[2], H[3], H[0], H[1], K4(i), S4(i), i); ++i;
        r(&H[1], H[2], H[3], H[0], K4(i), S4(i), i); ++i;

        /*
         * Then perform the following additions. (That is increment each
         * of the four registers by the value it had before this block
         * was started.)
         */
        H[0] += $0;
        H[1] += $1;
        H[2] += $2;
        H[3] += $3;
    }

    /**
     * Addl (from Wikipedia): Notice: the input bytes are considered as bit
     * strings, where the first bit is the most significant bit of the byte.
     * In other words: the order of bits in a byte is BIG ENDIAN, but the
     * order of bytes in a word is LITTLE ENDIAN
     */

    flip(&H[0]);
    flip(&H[1]);
    flip(&H[2]);
    flip(&H[3]);

    /**
     * Step 5. Output
     *
     * The message digest produced as output is A, B, C, D. That is, we
     * begin with the low-order byte of A, and end with the high-order byte
     * of D.
     *
     * This completes the description of MD5. A reference implementation in
     * C is given in the appendix.
     */

    free(*digest);
    *digest = malloc(DIGEST_LENGTH); PRINT("allocated %u bytes\n", DIGEST_LENGTH);

    snprintf((char *) *digest, DIGEST_LENGTH, "%08x%08x%08x%08x", H[0], H[1], H[2], H[3]);
}

void flip(uint32_t *value)
{
    *value = ((*value >> 24) & 0x000000ff)  /* move byte 3 to byte 0 */
           | ((*value >>  8) & 0x0000ff00)  /* move byte 2 to byte 1 */
           | ((*value <<  8) & 0x00ff0000)  /* move byte 1 to byte 2 */
           | ((*value << 24) & 0xff000000); /* move byte 0 to byte 3 */
}

uint32_t h(uint8_t i, uint32_t x, uint32_t y, uint32_t z)
{
    switch ((uint8_t) (i / 16))
    {
        case 0: /*  0 <= i < 16 */ return (x & y) | (~x & z);
        case 1: /* 16 <= i < 32 */ return (x & z) | (y & ~z);
        case 2: /* 32 <= i < 48 */ return x ^ y ^ z;
        case 3: /* 48 <= i < 64 */ return y ^ (x | ~z);
    }

    return 0;
}

void r(uint32_t *a, uint32_t b, uint32_t c, uint32_t d, uint8_t k, uint8_t s, uint8_t i)
{
    uint32_t x = (X[4 * k + 0] <<  0)
               + (X[4 * k + 1] <<  8)
               + (X[4 * k + 2] << 16)
               + (X[4 * k + 3] << 24);

    *a = b + ROTL(*a + h(i, b, c, d) + x + T[i], s);
}
