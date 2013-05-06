#ifndef MD_H_
#define MD_H_

#include <stdint.h>
#include <stdio.h>

#if defined MD2

# define BIT_STRENGTH             128
# define BLOCK_LENGTH_BYTES        16
# define PAD_MSG_TO_LENGTH_BYTES    0
# define DIGEST_LENGTH_BITS       128

#elif defined MD4 || defined MD5

# define BIT_STRENGTH             128
# define BLOCK_LENGTH_BITS         64
# define PAD_MSG_TO_LENGTH_BITS   448
# define DIGEST_LENGTH_BITS       512

#endif

#undef MD2
#undef MD4
#undef MD5

/* public interface */

uint8_t *MD2(const uint8_t *, uint64_t);
uint8_t *MD4(const uint8_t *, uint64_t);
uint8_t *MD5(const uint8_t *, uint64_t);
/* ... */

uint8_t *MD2file(FILE *);
uint8_t *MD4file(FILE *);
uint8_t *MD5file(FILE *);
/* ... */

uint8_t *MD2string(const char *);
uint8_t *MD4string(const char *);
uint8_t *MD5string(const char *);
/* ... */

#endif /* MD_H_ */
