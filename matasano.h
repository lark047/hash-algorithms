#ifndef MATASANO_H_
#define MATASANO_H_

#include <stdio.h>
#include <stdint.h>

#include "matasano-common.h"

/* 1. Convert hex to base64 and back. */

const char *EncodeBase64(const char * const);
const uint8_t *DecodeBase64(const char * const);

/* 2. Fixed XOR */

const uint8_t *FixedXOR(const uint8_t * const, const uint8_t * const, const uint64_t);

/* 3. Single-character XOR Cipher */
const struct result *DecodeXOR(const uint8_t * const, const uint64_t);

/* 4. Detect single-character XOR */
const struct result *DecodeXORFromFile(FILE *);

/* 5. Repeating-key XOR Cipher */
const uint8_t *RepeatingKeyXOR(const uint8_t * const, const uint64_t, const uint8_t * const, const uint64_t);

/* 6. Break repeating-key XOR */
const struct result *BreakRepeatingKeyXOR(FILE *);

#endif /* MATASANO_H_ */
