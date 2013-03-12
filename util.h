#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>

uint32_t append_padding(uint8_t **, const char *, uint32_t *);

#if 0
uint32_t rotl(const uint32_t, const uint8_t);
uint32_t rotr(const uint32_t, const uint8_t);
uint32_t shr(const uint32_t, const uint8_t);
#else
# define ROTL(v,s) (((v) << (s)) | ((v) >> (sizeof(v) * CHAR_BIT - (s))))
# define ROTR(v,s) (((v) >> (s)) | ((v) << (sizeof(v) * CHAR_BIT - (s))))
# define SHR(v,s)  ((v) >> (s))
#endif

#ifdef DEBUG
# define PRINT(f, ...)  printf("[debug] " f, __VA_ARGS__)
#else
# define PRINT(f, ...)  /* NO-OP */
#endif

#if defined MD5 || defined SHA1 || defined SHA224 || defined SHA256
# define BLOCK_SIZE_BITS      64
# define PADDED_LENGTH_BITS  448
# define DIGEST_LENGTH_BITS  512
#endif

void print_d(uint8_t *, uint8_t);

#endif /* UTIL_H_ */
