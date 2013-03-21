#ifndef MD5_H_
#define MD5_H_

#include <stdint.h>
#include <stdio.h>

#if defined MD5
# define BLOCK_SIZE_BITS      64
# define PADDED_LENGTH_BITS  448
# define DIGEST_LENGTH_BITS  512
#endif

/* public interface */

uint8_t *MD5string(const char *);
/* ... */

uint8_t *MD5file(FILE *);
/* ... */

#undef MD5

#endif /* MD5_H_ */
