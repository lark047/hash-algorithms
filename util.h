#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>
#include <stdio.h>

struct hash_info
{
    /* in bits */
    uint8_t   block_size;
    uint16_t  padded_length;
    uint16_t  digest_length;
};

uint64_t append_padding(uint8_t **, uint8_t *, uint64_t *, const struct hash_info *);
uint8_t *hash_file(FILE *, uint8_t *(*)(uint8_t *, const uint64_t));

# define ROTL(v,s) (((v) << (s)) | ((v) >> (sizeof(v) * CHAR_BIT - (s))))
# define ROTR(v,s) (((v) >> (s)) | ((v) << (sizeof(v) * CHAR_BIT - (s))))
# define SHR(v,s)  ((v) >> (s))

#define STRINGIFY_(x)  #x
#define STRINGIFY(x)   STRINGIFY_(x)

#ifdef DEBUG
# define PRINT(f, ...)  printf("[debug] " __FILE__ ":" STRINGIFY(__LINE__) " " f, __VA_ARGS__)
void print_d(const uint8_t *, uint64_t, const struct hash_info *);
#else
# define PRINT(f, ...)  /* NO-OP */
#endif

#define SIZE(a)     (sizeof (a) / sizeof *(a))
#define STR_EQ(a,b)    (strcmp((a), (b)) == 0)

#ifndef BIT_STRENGTH
# define BIT_STRENGTH    0
#endif
#define CHARS_PER_BYTE   2
#define DIGEST_LENGTH   (CHARS_PER_BYTE * BIT_STRENGTH / CHAR_BIT + 1)

/* for testing */
extern const char *test_files[];
extern const char *test_msgs[];

#endif /* UTIL_H_ */
