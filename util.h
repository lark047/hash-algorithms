#ifndef UTIL_H_
#define UTIL_H_

#  include <stdint.h>
#  include <stdio.h>

struct hash_info
{
    uint8_t   block_length;
    uint16_t  pad_msg_to_length;
    uint16_t  digest_length;
};

uint64_t append_padding(uint8_t **, const uint8_t *, uint64_t, const struct hash_info *);
uint8_t *hash_file(FILE *, uint8_t *(*)(const uint8_t *, uint64_t));

char    *to_string(uint8_t *, uint8_t);
void     flip(uint32_t *);
void     flip64(uint64_t *);

#  define ROTL(v,s) (((v) << (s)) | ((v) >> (sizeof(v) * CHAR_BIT - (s))))
#  define ROTR(v,s) (((v) >> (s)) | ((v) << (sizeof(v) * CHAR_BIT - (s))))
#  define SHR(v,s)  ((v) >> (s))

#  define STRINGIFY_(x)  #x
#  define STRINGIFY(x)   STRINGIFY_(x)

#  ifdef DEBUG
#    define PRINT(f, ...)  printf("[debug] " __FILE__ ":" STRINGIFY(__LINE__) " " f, __VA_ARGS__)
void print_d(const uint8_t *, uint64_t, const struct hash_info *);
#  else
#    define PRINT(f, ...)  /* NO-OP */
#  endif

#  define SIZE(a)     (sizeof (a) / sizeof *(a))
#  define STR_EQ(a,b)    (strcmp((a), (b)) == 0)

#  define CHARS_PER_BYTE   2
#  define DIGEST_LENGTH   (BIT_STRENGTH / CHAR_BIT)

/* for testing */
extern const char *test_files[];
extern const char *test_msgs[];

/* command line options */
int do_test(const char *, void (*)(void));
int do_hash_string(const char *, uint8_t *(*)(const char *), uint8_t);

#endif /* UTIL_H_ */
