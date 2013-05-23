#ifndef SHA_H_
#define SHA_H_

/**
 * This Standard specifies secure hash algorithms, SHA-1, SHA-224, SHA-256, SHA-384,
 * SHA-512, SHA-512/224 and SHA-512/256. All of the algorithms are iterative, one-way hash
 * functions that can process a message to produce a condensed representation called a
 * message digest. These algorithms enable the determination of a message�s integrity: any change to the
 * message will, with a very high probability, result in a different message digest. This property is
 * useful in the generation and verification of digital signatures and message authentication codes,
 * and in the generation of random numbers or bits.
 *
 * Each algorithm can be described in two stages: preprocessing and hash computation.
 * Preprocessing involves padding a message, parsing the padded message into m-bit blocks, and
 * setting initialization values to be used in the hash computation. The hash computation generates
 * a message schedule from the padded message and uses that schedule, along with functions,
 * constants, and word operations to iteratively generate a series of hash values. The final hash
 * value generated by the hash computation is used to determine the message digest.
 *
 * The algorithms differ most significantly in the security strengths that are provided for the data
 * being hashed. The security strengths of these hash functions and the system as a whole when
 * each of them is used with other cryptographic algorithms, such as digital signature algorithms
 * and keyed-hash message authentication codes, can be found in [SP 800-57] and [SP 800-107].
 *
 * Additionally, the algorithms differ in terms of the size of the blocks and words of data that are
 * used during hashing or message digest sizes. Figure 1 presents the basic properties of these hash
 * algorithms.
 *
 * +------------+--------------+------------+-----------+---------------------+
 * | Algorithm  | Message Size | Block Size | Word Size | Message Digest Size |
 * |            |    (bits)    |   (bits)   |   (bits)  |       (bits)        |
 * +------------+--------------+------------+-----------+---------------------+
 * | SHA-1      |    < 2^64    |     512    |     32    |        160          |
 * +------------+--------------+------------+-----------+---------------------+
 * | SHA-224    |    < 2^64    |     512    |     32    |        224          |
 * +------------+--------------+------------+-----------+---------------------+
 * | SHA-256    |    < 2^64    |     512    |     32    |        256          |
 * +------------+--------------+------------+-----------+---------------------+
 * | SHA-384    |    < 2^128   |    1024    |     64    |        384          |
 * +------------+--------------+------------+-----------+---------------------+
 * | SHA-512    |    < 2^128   |    1024    |     64    |        512          |
 * +------------+--------------+------------+-----------+---------------------+
 * |SHA-512/224 |    < 2^128   |    1024    |     64    |        224          |
 * +------------+--------------+------------+-----------+---------------------+
 * |SHA-512/256 |    < 2^128   |    1024    |     64    |        256          |
 * +------------+--------------+------------+-----------+---------------------+
 *                  Figure 1: Secure Hash Algorithm Properties
 */

#include <stdint.h>
#include <stdio.h>

#if defined SHA1
#  define BIT_STRENGTH 160
#elif defined SHA224 || defined SHA512224
#  define BIT_STRENGTH 224
#elif defined SHA256 || defined SHA512256
#  define BIT_STRENGTH 256
#elif defined SHA384
#  define BIT_STRENGTH 384
#elif defined SHA512
#  define BIT_STRENGTH 512
#endif

#if defined SHA224 || defined SHA256 || defined SHA384 || defined SHA512 || defined SHA512224 || defined SHA512256

#  define CH(x,y,z)                (((x) & (y)) ^ (~(x) & (z)))
#  define MAJ(x,y,z)  (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

#endif

/* constants */
#if defined SHA1 || defined SHA224 || defined SHA256

#  define BLOCK_LENGTH_BITS         64
#  define PAD_MSG_TO_LENGTH_BITS   448
#  define DIGEST_LENGTH_BITS       512

#  if defined SHA1

#    define ROUNDS                  80

#  elif defined SHA224 || defined SHA256

#    define ROUNDS                  64

#  endif

#  define PREPARE(msg, i) (((uint32_t) msg[4 * (i) + 0] << 24) | \
                           ((uint32_t) msg[4 * (i) + 1] << 16) | \
                           ((uint32_t) msg[4 * (i) + 2] <<  8) | \
                           ((uint32_t) msg[4 * (i) + 3] <<  0))

#elif defined SHA384 || defined SHA512 || defined SHA512224 || defined SHA512256

#  define BLOCK_LENGTH_BITS        128
#  define PAD_MSG_TO_LENGTH_BITS   896
#  define DIGEST_LENGTH_BITS      1024
#  define ROUNDS                    80

#  define PREPARE(msg, i) (((uint64_t) M[8 * (i) + 0] << 56) | \
                           ((uint64_t) M[8 * (i) + 1] << 48) | \
                           ((uint64_t) M[8 * (i) + 2] << 40) | \
                           ((uint64_t) M[8 * (i) + 3] << 32) | \
                           ((uint64_t) M[8 * (i) + 4] << 24) | \
                           ((uint64_t) M[8 * (i) + 5] << 16) | \
                           ((uint64_t) M[8 * (i) + 6] <<  8) | \
                           ((uint64_t) M[8 * (i) + 7] <<  0))

#endif

/* hash functions */
#if defined SHA224 || defined SHA256

#  define SIGMA0(x)  (ROTR((x),  2) ^ ROTR((x), 13) ^ ROTR((x), 22))
#  define SIGMA1(x)  (ROTR((x),  6) ^ ROTR((x), 11) ^ ROTR((x), 25))
#  define sigma0(x)  (ROTR((x),  7) ^ ROTR((x), 18) ^  SHR((x),  3))
#  define sigma1(x)  (ROTR((x), 17) ^ ROTR((x), 19) ^  SHR((x), 10))

#elif defined SHA384 || defined SHA512 || defined SHA512224 || defined SHA512256

#  define SIGMA0(x)  (ROTR((x), 28) ^ ROTR((x), 34) ^ ROTR((x), 39))
#  define SIGMA1(x)  (ROTR((x), 14) ^ ROTR((x), 18) ^ ROTR((x), 41))
#  define sigma0(x)  (ROTR((x),  1) ^ ROTR((x),  8) ^  SHR((x),  7))
#  define sigma1(x)  (ROTR((x), 19) ^ ROTR((x), 61) ^  SHR((x),  6))

#endif

#undef SHA1
#undef SHA224
#undef SHA256
#undef SHA384
#undef SHA512
#undef SHA512224
#undef SHA512256

/* public interface */

uint8_t *SHA1(const uint8_t *, uint64_t);
uint8_t *SHA224(const uint8_t *, uint64_t);
uint8_t *SHA256(const uint8_t *, uint64_t);
uint8_t *SHA384(const uint8_t *, uint64_t);
uint8_t *SHA512(const uint8_t *, uint64_t);

#if 0
uint8_t *SHA512224(const uint8_t *, uint64_t);
uint8_t *SHA512256(const uint8_t *, uint64_t);
#endif

uint8_t *SHA1string(const char *);
uint8_t *SHA224string(const char *);
uint8_t *SHA256string(const char *);
uint8_t *SHA384string(const char *);
uint8_t *SHA512string(const char *);

#if 0
uint8_t *SHA512224string(const char *);
uint8_t *SHA512256string(const char *);
#endif

uint8_t *SHA1file(FILE *);
uint8_t *SHA224file(FILE *);
uint8_t *SHA256file(FILE *);
uint8_t *SHA384file(FILE *);
uint8_t *SHA512file(FILE *);

#if 0
uint8_t *SHA512224file(FILE *);
uint8_t *SHA512256file(FILE *);
#endif

uint8_t *SHA1hmac(const char *, const char *);
uint8_t *SHA224hmac(const char *, const char *);
uint8_t *SHA256hmac(const char *, const char *);
uint8_t *SHA384hmac(const char *, const char *);
uint8_t *SHA512hmac(const char *, const char *);

#if 0
uint8_t *SHA512224hmac(const char *, const char *);
uint8_t *SHA512256hmac(const char *, const char *);
#endif

#endif /* SHA_H_ */
