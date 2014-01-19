#ifndef MATASANO_H_
#define MATASANO_H_

#include <stdio.h>
#include <stdint.h>

#include "matasano-common.h"

/* 1. Convert hex to base64 and back. */

void EncodeBase64(const char * const, char * const);
void DecodeBase64(const char * const, uint8_t * const);

/* 2. Fixed XOR */

const uint8_t *FixedXOR(const uint8_t * const, const uint8_t * const, const uint64_t);

/* 3. Single-character XOR Cipher */
struct result *DecodeXOR(const uint8_t * const, const uint64_t);

/* 4. Detect single-character XOR */
struct result *DecodeXORFromFile(FILE *);

/* 5. Repeating-key XOR Cipher */
const uint8_t *RepeatingKeyXOR(const char * const, const char * const);

#endif /* MATASANO_H_ */
