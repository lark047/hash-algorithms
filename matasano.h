#ifndef MATASANO_H_
#define MATASANO_H_

#include <stdint.h>

#ifdef DEBUG
#  define print_d(f, ...)  printf("[debug] " f, __VA_ARGS__)
#else
#  define print_d(f, ...)  /* NO-OP */
#endif

#define STR_EQ(s1, s2)  (strcmp((s1), (s2)) == 0)

/* util functions */
void PrintHex(const uint8_t *, const uint64_t);

/* exercise functions */
void EncodeBase64(const char *msg, char * const base64);
void DecodeBase64(const char *base64, uint8_t * const msg);

#endif /* MATASANO_H_ */
