#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>

struct hash_info
{
    /* in bits */
    uint8_t   block_size;
    uint16_t  padded_length;
    uint16_t  digest_length;
};

uint32_t append_padding(uint8_t **, const char *, uint32_t *, struct hash_info *);

# define ROTL(v,s) (((v) << (s)) | ((v) >> (sizeof(v) * CHAR_BIT - (s))))
# define ROTR(v,s) (((v) >> (s)) | ((v) << (sizeof(v) * CHAR_BIT - (s))))
# define SHR(v,s)  ((v) >> (s))

#ifdef DEBUG
# define PRINT(f, ...)  printf("[debug] " f, __VA_ARGS__)
void print_d(const uint8_t *, uint32_t, const struct hash_info *);
#else
# define PRINT(f, ...)  /* NO-OP */
#endif

#define SIZE(a) (sizeof (a) / sizeof *(a))

#endif /* UTIL_H_ */
