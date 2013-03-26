#ifndef MD_H_
#define MD_H_

#include <stdint.h>
#include <stdio.h>

#if defined MD4 || defined MD5
# undef BIT_STRENGTH
# define BIT_STRENGTH        128
# define BLOCK_SIZE_BITS      64
# define PADDED_LENGTH_BITS  448
# define DIGEST_LENGTH_BITS  512
#endif

#undef MD4
#undef MD5

/* public interface */

uint8_t *MD4(uint8_t *, uint64_t);
uint8_t *MD5(uint8_t *, uint64_t);
/* ... */

uint8_t *MD4file(FILE *);
uint8_t *MD5file(FILE *);
/* ... */

uint8_t *MD4string(const char *);
uint8_t *MD5string(const char *);
/* ... */

#endif /* MD_H_ */
