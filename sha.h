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

#if defined SHA1 || defined SHA224 || defined SHA256
# define BLOCK_SIZE_BITS      64
# define PADDED_LENGTH_BITS  448
# define DIGEST_LENGTH_BITS  512
#endif

uint8_t *SHAstring(const char *);

#endif /* SHA_H_ */
