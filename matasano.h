#ifndef MATASANO_H_
#define MATASANO_H_

#include <stdint.h>

#include "matasano-common.h"

/* 1. Convert hex to base64 and back. */

void EncodeBase64(const char * const, char * const);
void DecodeBase64(const char * const, uint8_t * const);

/* Fixed XOR */

const uint8_t *FixedXOR(const uint8_t * const, const uint8_t * const, const uint64_t);

/* Single-character XOR Cipher */
uint8_t DecodeXOR(const uint8_t * const, const uint64_t, const char *);

#endif /* MATASANO_H_ */
