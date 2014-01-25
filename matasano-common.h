#ifndef MATASANO_COMMON_H_
#define MATASANO_COMMON_H_

#include <stdint.h>
#include <stdbool.h>

#define _STR(x)    #x
#define STR(x) _STR(x)

#ifdef DEBUG
#  define fprint_d(fp, f, ...) fprintf(fp, "[debug] " __FILE__ ":" STR(__LINE__) " " f, __VA_ARGS__)
#  define print_d(f, ...)      fprint_d(stdout, f, __VA_ARGS__)
#else
#  define fprint_d(fp, f, ...) /* NO-OP */
#  define print_d(f, ...)      /* NO-OP */
#endif

#define STR_EQ(s1,s2) (strcmp(s1, s2) == 0)

void StringToHex(const char * const, uint8_t * const);
void HexToCleanString(const uint8_t * const, const uint64_t, unsigned char * const);

void PrintHex(const uint8_t * const, const uint64_t, const bool);
void PrintHexWithSpace(const uint8_t * const, const uint64_t);
void PrintAsString(const uint8_t * const, const uint64_t);

/* result of XOR decode */
struct result
{
    uint8_t key;
    double score;

    /* ciphertext */
    uint8_t *hex;

    /* plaintext */
    unsigned char *text;
};

#endif /* MATASANO_COMMON_H_ */
