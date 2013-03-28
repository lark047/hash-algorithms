/**
  * This document describes the MD2 message-digest algorithm. The
  * algorithm takes as input a message of arbitrary length and produces
  * as output a 128-bit "fingerprint" or "message digest" of the input.
  * It is conjectured that it is computationally infeasible to produce
  * two messages having the same message digest, or to produce any
  * message having a given prespecified target message digest. The MD2
  * algorithm is intended for digital signature applications, where a
  * large file must be "compressed" in a secure manner before being
  * signed with a private (secret) key under a public-key cryptosystem
  * such as RSA.
  *
  * License to use MD2 is granted for non-commerical Internet Privacy-
  * Enhanced Mail [1-3].
  *
  * This document is an update to the August 1989 RFC 1115 [3], which
  * also gives a reference implementation of MD2. The main differences
  * are that a textual description of MD2 is included, and that the
  * reference implementation of MD2 is more portable.
  *
  * For OSI-based applications, MD2's object identifier is
  *
  * md2 OBJECT IDENTIFIER ::=
  * iso(1) member-body(2) US(840) rsadsi(113549) digestAlgorithm(2) 2}
  *
  * In the X.509 type AlgorithmIdentifier [4], the parameters for MD2
  * should have type NULL.
  */

#include "md.h"
#include "util.h"

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* MD2 general hash function */
uint8_t *MD2(uint8_t *, const uint64_t);

/* functions called by MD2 */
static uint64_t append_padding$(uint8_t **, uint8_t *, uint64_t *, struct hash_info *);
static void calc_checksum(uint8_t *, uint8_t *, uint64_t, uint8_t);
static void append_checksum(uint8_t *, uint8_t *, const uint64_t, const uint16_t);
static void process(uint8_t **, uint8_t *, const uint32_t, const uint16_t);

/* Permutation of 0..255 constructed from the digits of pi. It gives a
   "random" nonlinear byte substitution operation.
 */
static const uint8_t S[] = {
    0x29,0x2e,0x43,0xc9,0xa2,0xd8,0x7c,0x01,0x3d,0x36,0x54,0xa1,0xec,0xf0,0x06,0x13,
    0x62,0xa7,0x05,0xf3,0xc0,0xc7,0x73,0x8c,0x98,0x93,0x2b,0xd9,0xbc,0x4c,0x82,0xca,
    0x1e,0x9b,0x57,0x3c,0xfd,0xd4,0xe0,0x16,0x67,0x42,0x6f,0x18,0x8a,0x17,0xe5,0x12,
    0xbe,0x4e,0xc4,0xd6,0xda,0x9e,0xde,0x49,0xa0,0xfb,0xf5,0x8e,0xbb,0x2f,0xee,0x7a,
    0xa9,0x68,0x79,0x91,0x15,0xb2,0x07,0x3f,0x94,0xc2,0x10,0x89,0x0b,0x22,0x5f,0x21,
    0x80,0x7f,0x5d,0x9a,0x5a,0x90,0x32,0x27,0x35,0x3e,0xcc,0xe7,0xbf,0xf7,0x97,0x03,
    0xff,0x19,0x30,0xb3,0x48,0xa5,0xb5,0xd1,0xd7,0x5e,0x92,0x2a,0xac,0x56,0xaa,0xc6,
    0x4f,0xb8,0x38,0xd2,0x96,0xa4,0x7d,0xb6,0x76,0xfc,0x6b,0xe2,0x9c,0x74,0x04,0xf1,
    0x45,0x9d,0x70,0x59,0x64,0x71,0x87,0x20,0x86,0x5b,0xcf,0x65,0xe6,0x2d,0xa8,0x02,
    0x1b,0x60,0x25,0xad,0xae,0xb0,0xb9,0xf6,0x1c,0x46,0x61,0x69,0x34,0x40,0x7e,0x0f,
    0x55,0x47,0xa3,0x23,0xdd,0x51,0xaf,0x3a,0xc3,0x5c,0xf9,0xce,0xba,0xc5,0xea,0x26,
    0x2c,0x53,0x0d,0x6e,0x85,0x28,0x84,0x09,0xd3,0xdf,0xcd,0xf4,0x41,0x81,0x4d,0x52,
    0x6a,0xdc,0x37,0xc8,0x6c,0xc1,0xab,0xfa,0x24,0xe1,0x7b,0x08,0x0c,0xbd,0xb1,0x4a,
    0x78,0x88,0x95,0x8b,0xe3,0x63,0xe8,0x6d,0xe9,0xcb,0xd5,0xfe,0x3b,0x00,0x1d,0x39,
    0xf2,0xef,0xb7,0x0e,0x66,0x58,0xd0,0xe4,0xa6,0x77,0x72,0xf8,0xeb,0x75,0x4b,0x0a,
    0x31,0x44,0x50,0xb4,0x8f,0xed,0x1f,0x1a,0xdb,0x99,0x8d,0x33,0x9f,0x11,0x83,0x14
};

uint8_t *MD2file(FILE *fp)
{
    return hash_file(fp, MD2);
}

uint8_t *MD2string(const char *msg)
{
    PRINT("found newline in msg: %s\n", (strchr(msg, '\n') ? "true" : "false"));
    return MD2((uint8_t *) msg, strlen(msg));
}

uint8_t *MD2(uint8_t *msg, const uint64_t msg_length)
{
    struct hash_info info = {
        BLOCK_SIZE_BITS,
        PADDED_LENGTH_BITS,
        DIGEST_LENGTH_BITS
    };

    uint8_t *digest;

    /* length in bytes */
    PRINT("Message length: %llu byte%s\n", msg_length, (msg_length == 1 ? "" : "s"));

    /**
      * We begin by supposing that we have a b-byte message as input, and
      * that we wish to find its message digest. Here b is an arbitrary
      * nonnegative integer; b may be zero, and it may be arbitrarily large.
      * We imagine the bytes of the message written down as follows:
      *
      *                 m_0 m_1 ... m_{b-1}
      *
      * The following five steps are performed to compute the message digest
      * of the message.
      */

    const uint64_t b = msg_length * CHAR_BIT;
    PRINT("Message length: %llu bits\n", b);

    /**
     * Step 1. Append Padding Bits
     *
     * The message is "padded" (extended) so that its length (in bytes) is
     * congruent to 0, modulo 16. That is, the message is extended so that
     * it is a multiple of 16 bytes long. Padding is always performed, even
     * if the length of the message is already congruent to 0, modulo 16.
     *
     * Padding is performed as follows: "i" bytes of value "i" are appended
     * to the message so that the length in bytes of the padded message
     * becomes congruent to 0, modulo 16. At least one byte and at most 16
     * 16 bytes are appended.
     *
     * At this point the resulting message (after padding with bytes) has a
     * length that is an exact multiple of 16 bytes. Let M[0 ... N-1] denote
     * the bytes of the resulting message, where N is a multiple of 16.
     */

    uint64_t padded_length = msg_length;
    const uint64_t block_count = append_padding$(&digest, msg, &padded_length, &info);

    /**
     * Step 2. Append Checksum
     *
     * A 16-byte checksum of the message is appended to the result of the
     * previous step.
     *
     * This step uses a 256-byte "random" permutation constructed from the
     * digits of pi. Let S[i] denote the i-th element of this table. The
     * table is given in the appendix.
     *
     * The 16-byte checksum C[0 ... 15] is appended to the message. Let M[0
     * with checksum), where N' = N + 16.
     */

    uint8_t C[16] = {0};

    /* -1 for checksum block */
    calc_checksum(msg, C, block_count - 1, info.block_size / CHAR_BIT);

    PRINT("padded length = %llu\n", padded_length);
    append_checksum(digest, C, padded_length, info.block_size);

#ifdef DEBUG
    print_d(digest, block_count, &info);
#endif

    /**
     * Step 3. Initialize MD Buffer
     *
     * A 48-byte buffer X is used to compute the message digest. The buffer
     * is initialized to zero.
     */

    uint8_t X[48] = {0};

    /**
     * Step 4. Process Message in 16-Word Blocks
     *
     * We first define four auxiliary functions that each take as input
     * three 32-bit words and produce as output one 32-bit word.
     *
     *        F(X,Y,Z) = XY v not(X) Z
     *        G(X,Y,Z) = XY v XZ v YZ
     *        H(X,Y,Z) = X xor Y xor Z
     *
     * In each bit position F acts as a conditional: if X then Y else Z.
     * The function F could have been defined using + instead of v since XY
     * and not(X)Z will never have "1" bits in the same bit position.)  In
     * each bit position G acts as a majority function: if at least two of
     * X, Y, Z are on, then G has a "1" bit in that bit position, else G has
     * a "0" bit. It is interesting to note that if the bits of X, Y, and Z
     * are independent and unbiased, the each bit of f(X,Y,Z) will be
     * independent and unbiased, and similarly each bit of g(X,Y,Z) will be
     * independent and unbiased. The function H is the bit-wise XOR or
     * parity" function; it has properties similar to those of F and G.
     */

    process(&digest, X, block_count, info.block_size / CHAR_BIT);

    return digest;
}

uint64_t append_padding$(uint8_t **digest_ref, uint8_t *msg, uint64_t *length, struct hash_info *info)
{
    /* TODO what if the length in bits isn't a multiple of CHAR_BIT? */
    /* TODO use general one in util.c */

    uint32_t nils = 0;

    do
    {
        ++nils;
    }
    while ((*length * CHAR_BIT + nils) % info->digest_length != info->padded_length);

    nils /= CHAR_BIT;
    PRINT("adding %u 0x%02x bytes\n", nils, nils);

    const uint64_t total_length = *length + nils + info->block_size / CHAR_BIT;
    const uint64_t block_count = total_length * CHAR_BIT / info->block_size;

    PRINT("total_length = %llu + %u + %u = %llu\n", *length, nils, info->block_size / CHAR_BIT, total_length);
    PRINT("%llu / 64 = %llu (%llu) <== should be 0\n", total_length * CHAR_BIT, block_count, (total_length * CHAR_BIT) % info->block_size);
    PRINT("allocating %llu block%s of %u bits...\n", block_count, (block_count == 1 ? "" : "s"), info->digest_length);

    (*digest_ref) = malloc(total_length * sizeof **digest_ref);
    PRINT("allocated %llu bytes\n", total_length * sizeof **digest_ref);

    /* set non-checksum bytes to nils */
    memset(*digest_ref, nils, total_length - 16);
    PRINT("set %llu bytes to 0x%02x\n", total_length - 16, nils);

    /* copy the message in */
    memcpy(*digest_ref, msg, *length);

    *length += nils;

    PRINT("block count: %llu\n", block_count);
    PRINT("length     : %llu\n", *length);

    return block_count;
}

void calc_checksum(uint8_t *msg, uint8_t *C, uint64_t block_count, uint8_t block_size)
{
    /* done at declaration */
    /* memset(C, 0x0, 16); */

    uint8_t L, c;

    for (uint8_t i = 0; i < block_count; ++i)
    {
        for (uint8_t j = 0; j < 16; ++j)
        {
            c = msg[i * block_size + j];
            C[j] = S[c ^ L];
            L = C[j];
        }
    }

#ifdef DEBUG
    printf("checksum: ");
    fflush(stdout);
    for (uint8_t i = 0; i < 16; ++i)
    {
        printf("%02x ", C[i]);
        fflush(stdout);
    }
    printf("\n");
#endif
}

void append_checksum(uint8_t *digest, uint8_t *C, const uint64_t index, const uint16_t block_size)
{
    const uint8_t len_bytes = block_size / CHAR_BIT;

    /* assume length < 2^64 */
    memcpy(digest + index, C, len_bytes);
}

void process(uint8_t **digest, uint8_t *X, const uint32_t block_count, const uint16_t block_size)
{
    /* Process each 16-word block. */
    for (uint32_t i = 0; i < block_count; ++i)
    {
        /* can replace first loop assignment */
        /* memcpy(X + 16, *digest + i * block_size, 16); */

        for (uint8_t j = 0; j < 16; ++j)
        {
            X[j + 16] = (*digest)[j + i * block_size];
            X[j + 32] = X[j + 16] ^ X[j];
        }

        uint16_t t = 0;

        for (uint8_t j = 0; j < 18; ++j)
        {
            for (uint8_t k = 0; k < 48; ++k)
            {
                t = X[k] ^= S[t];
            }

            t = (t + j) % 256;
        }
    }

    /**
     * Step 5. Output
     *
     * The message digest produced as output is X[0 ... 15]. That is, we
     * begin with X[0], and end with X[15].
     *
     * This completes the description of MD2. A reference implementation in
     * C is given in the appendix.
     */

    free(*digest);
    *digest = malloc(DIGEST_LENGTH); PRINT("allocated %u bytes\n", DIGEST_LENGTH);

    for (uint8_t i = 0, bytes = DIGEST_LENGTH / 16; i < 16; ++i)
    {
        snprintf((char *) *digest + i * bytes, bytes + 1, "%02x", X[i * bytes]);
    }
}
