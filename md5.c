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

#include "md.h"
#include "hmac.h"
#include "util.h"

#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* functions called by MD5 */
static void append_length(uint8_t *, uint64_t, uint64_t, uint8_t);
static void process(const uint8_t *, uint64_t, uint8_t);

/* hash function */
static void r(uint32_t *, uint32_t, uint32_t, uint32_t, uint8_t, uint8_t, uint8_t);

/* hash registers */
static uint32_t H[4];

/* values table */
static const uint32_t T[] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

/* pointer to 32-bit word blocks */
static const uint8_t *X;

#define K(m,a) ((i * (m) + (a)) % 16)
#define K1                        (i)
#define K2                     K(5,1)
#define K3                     K(3,5)
#define K4                     K(7,0)

#define S(x)          (s[(x)][i % 4])
#define S1          ((i % 4) * 5 + 7)
#define S2                       S(0)
#define S3                       S(1)
#define S4                       S(2)

uint8_t *MD5file(FILE *fp)
{
    return hash_file(fp, MD5);
}

uint8_t *MD5string(const char *msg)
{
    PRINT("found newline in msg: %s\n", (strchr(msg, '\n') ? "true" : "false"));
    return MD5((uint8_t *) msg, strlen(msg));
}

uint8_t *MD5hmac(const char *key, const char *msg)
{
    return HMACstring(key, msg, MD5, 64, 16);
}

uint8_t *MD5(const uint8_t *msg, uint64_t msg_length)
{
    const struct hash_info info = {
        BLOCK_LENGTH_BITS,
        PAD_MSG_TO_LENGTH_BITS,
        DIGEST_LENGTH_BITS
    };

    uint8_t *buffer;

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

    const uint64_t padded_length = append_padding(&buffer, msg, msg_length, &info);

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

    append_length(buffer, b, padded_length, info.block_length / CHAR_BIT);
    const uint64_t N = padded_length + (info.digest_length - info.pad_msg_to_length) / CHAR_BIT;
    PRINT("buffer is %llu bytes long\n", N);

#ifdef DEBUG
    print_d(buffer, N / info.block_length, &info);
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

    process(buffer, N, info.block_length);
    free(buffer);

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

    uint8_t *digest = malloc(DIGEST_LENGTH * sizeof *digest);
    PRINT("allocated %u bytes\n", DIGEST_LENGTH);

    memcpy(digest, H, DIGEST_LENGTH);

    return digest;
}

static void append_length(uint8_t *buffer, uint64_t length, uint64_t padded_index, uint8_t block_length)
{
    /* assume length < 2^64 */
    for (uint8_t i = 0; i < block_length; ++i)
    {
        buffer[padded_index + i] = (length >> (CHAR_BIT * i)) & 0xff;
    }
}

static void process(const uint8_t *M, uint64_t N, uint8_t block_length)
{
    const uint8_t s[][4] = {
        { 5,  9, 14, 20 },
        { 4, 11, 16, 23 },
        { 6, 10, 15, 21 }
    };

    /* temporary registers */
    uint32_t $0, $1, $2, $3;

    /* Process each 16-word block. */
    for (uint64_t b = 0, i = 0; b < N / block_length; ++b, i = 0)
    {
        X = M + b * block_length;

        /* Save H[0] as $0, H[1] as $1, H[2] as $2, and H[3] as $3. */
        $0 = H[0];
        $1 = H[1];
        $2 = H[2];
        $3 = H[3];

        /* Round 1. */
        r(&H[0], H[1], H[2], H[3], K1, S1, i); ++i;
        r(&H[3], H[0], H[1], H[2], K1, S1, i); ++i;
        r(&H[2], H[3], H[0], H[1], K1, S1, i); ++i;
        r(&H[1], H[2], H[3], H[0], K1, S1, i); ++i;
        r(&H[0], H[1], H[2], H[3], K1, S1, i); ++i;
        r(&H[3], H[0], H[1], H[2], K1, S1, i); ++i;
        r(&H[2], H[3], H[0], H[1], K1, S1, i); ++i;
        r(&H[1], H[2], H[3], H[0], K1, S1, i); ++i;
        r(&H[0], H[1], H[2], H[3], K1, S1, i); ++i;
        r(&H[3], H[0], H[1], H[2], K1, S1, i); ++i;
        r(&H[2], H[3], H[0], H[1], K1, S1, i); ++i;
        r(&H[1], H[2], H[3], H[0], K1, S1, i); ++i;
        r(&H[0], H[1], H[2], H[3], K1, S1, i); ++i;
        r(&H[3], H[0], H[1], H[2], K1, S1, i); ++i;
        r(&H[2], H[3], H[0], H[1], K1, S1, i); ++i;
        r(&H[1], H[2], H[3], H[0], K1, S1, i); ++i;

        /* Round 2. */
        r(&H[0], H[1], H[2], H[3], K2, S2, i); ++i;
        r(&H[3], H[0], H[1], H[2], K2, S2, i); ++i;
        r(&H[2], H[3], H[0], H[1], K2, S2, i); ++i;
        r(&H[1], H[2], H[3], H[0], K2, S2, i); ++i;
        r(&H[0], H[1], H[2], H[3], K2, S2, i); ++i;
        r(&H[3], H[0], H[1], H[2], K2, S2, i); ++i;
        r(&H[2], H[3], H[0], H[1], K2, S2, i); ++i;
        r(&H[1], H[2], H[3], H[0], K2, S2, i); ++i;
        r(&H[0], H[1], H[2], H[3], K2, S2, i); ++i;
        r(&H[3], H[0], H[1], H[2], K2, S2, i); ++i;
        r(&H[2], H[3], H[0], H[1], K2, S2, i); ++i;
        r(&H[1], H[2], H[3], H[0], K2, S2, i); ++i;
        r(&H[0], H[1], H[2], H[3], K2, S2, i); ++i;
        r(&H[3], H[0], H[1], H[2], K2, S2, i); ++i;
        r(&H[2], H[3], H[0], H[1], K2, S2, i); ++i;
        r(&H[1], H[2], H[3], H[0], K2, S2, i); ++i;

        /* Round 3. */
        r(&H[0], H[1], H[2], H[3], K3, S3, i); ++i;
        r(&H[3], H[0], H[1], H[2], K3, S3, i); ++i;
        r(&H[2], H[3], H[0], H[1], K3, S3, i); ++i;
        r(&H[1], H[2], H[3], H[0], K3, S3, i); ++i;
        r(&H[0], H[1], H[2], H[3], K3, S3, i); ++i;
        r(&H[3], H[0], H[1], H[2], K3, S3, i); ++i;
        r(&H[2], H[3], H[0], H[1], K3, S3, i); ++i;
        r(&H[1], H[2], H[3], H[0], K3, S3, i); ++i;
        r(&H[0], H[1], H[2], H[3], K3, S3, i); ++i;
        r(&H[3], H[0], H[1], H[2], K3, S3, i); ++i;
        r(&H[2], H[3], H[0], H[1], K3, S3, i); ++i;
        r(&H[1], H[2], H[3], H[0], K3, S3, i); ++i;
        r(&H[0], H[1], H[2], H[3], K3, S3, i); ++i;
        r(&H[3], H[0], H[1], H[2], K3, S3, i); ++i;
        r(&H[2], H[3], H[0], H[1], K3, S3, i); ++i;
        r(&H[1], H[2], H[3], H[0], K3, S3, i); ++i;

        /* Round 4. */
        r(&H[0], H[1], H[2], H[3], K4, S4, i); ++i;
        r(&H[3], H[0], H[1], H[2], K4, S4, i); ++i;
        r(&H[2], H[3], H[0], H[1], K4, S4, i); ++i;
        r(&H[1], H[2], H[3], H[0], K4, S4, i); ++i;
        r(&H[0], H[1], H[2], H[3], K4, S4, i); ++i;
        r(&H[3], H[0], H[1], H[2], K4, S4, i); ++i;
        r(&H[2], H[3], H[0], H[1], K4, S4, i); ++i;
        r(&H[1], H[2], H[3], H[0], K4, S4, i); ++i;
        r(&H[0], H[1], H[2], H[3], K4, S4, i); ++i;
        r(&H[3], H[0], H[1], H[2], K4, S4, i); ++i;
        r(&H[2], H[3], H[0], H[1], K4, S4, i); ++i;
        r(&H[1], H[2], H[3], H[0], K4, S4, i); ++i;
        r(&H[0], H[1], H[2], H[3], K4, S4, i); ++i;
        r(&H[3], H[0], H[1], H[2], K4, S4, i); ++i;
        r(&H[2], H[3], H[0], H[1], K4, S4, i); ++i;
        r(&H[1], H[2], H[3], H[0], K4, S4, i); ++i;

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
}

static uint32_t h(uint8_t i, uint32_t x, uint32_t y, uint32_t z)
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

static void r(uint32_t *a, uint32_t b, uint32_t c, uint32_t d, uint8_t k, uint8_t s, uint8_t i)
{
    uint32_t x = (X[4 * k + 0] <<  0)
               + (X[4 * k + 1] <<  8)
               + (X[4 * k + 2] << 16)
               + (X[4 * k + 3] << 24);

    *a = b + ROTL(*a + h(i, b, c, d) + x + T[i], s);
}
