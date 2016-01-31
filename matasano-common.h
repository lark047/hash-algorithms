#ifndef MATASANO_COMMON_H_
#define MATASANO_COMMON_H_

#include <stdint.h>
#include <stdbool.h>

#define _STR(x)    #x
#define STR(x) _STR(x)

#define LENGTH(a) (sizeof (a) / sizeof *(a))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

#ifdef DEBUG
#  define fprint_d(fp, f, ...) fprintf(fp, "[debug] " __FILE__ ":" STR(__LINE__) " " f, __VA_ARGS__)
#  define print_d(f, ...)      fprint_d(stdout, f, __VA_ARGS__)
#else
#  define fprint_d(fp, f, ...) /* NO-OP */
#  define print_d(f, ...)      /* NO-OP */
#endif

#define STR_EQ(s1,s2)     (strcmp(s1, s2) == 0)
#define ASIZE                                27
#define ALPHA     "abcdefghijklmnopqrstuvwxyz "

void StringToHex(const char * const, uint8_t * const);
void HexToCleanString(const uint8_t * const, const uint64_t, unsigned char * const);

void PrintHex(const uint8_t * const, const uint64_t, const bool);
void PrintHexWithSpace(const uint8_t * const, const uint64_t);
void PrintAsString(const uint8_t * const, const uint64_t);

double CalculateScore(const uint8_t * const, const uint64_t);

/* result of XOR decode */
struct result
{
    uint16_t keysize;

    union
    {
        uint8_t   c;
        uint8_t  *ptr;
    } key;

    double score;

    /* ciphertext */
    const uint8_t *hex;

    /* plaintext */
    unsigned char *text;
};

struct freq
{
    /* count by letter */
    uint32_t *freq;

    /* sample text length */
    uint32_t length;

    /* alpha or ' ' hits */
    uint32_t hits;
};

#endif /* MATASANO_COMMON_H_ */
