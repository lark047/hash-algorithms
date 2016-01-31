#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <float.h>

#ifdef DEBUG
#  include <ctype.h>
#endif

#include "matasano.h"

#define PLURAL(x) ((x) == 1 ? "" : "s")

const struct result *DecodeXOR(const uint8_t * const hex, const uint64_t length)
{
    const uint8_t  *xor_result;
    uint8_t        *buffer = malloc(length * sizeof *buffer); /* TODO check */
    uint8_t         key = 0;
    double          min_score = DBL_MAX;

    /* from 0x00 to 0xff */
    for (uint8_t c = 0; c < UCHAR_MAX; ++c)
    {
        memset(buffer, c, length);
        xor_result = FixedXOR(hex, buffer, length);

#ifdef DEBUG
        unsigned char * const tmpbuf = malloc(length + 1); /* TODO check */
        HexToCleanString(xor_result, length, tmpbuf);
        tmpbuf[length] = '\0';
#endif

        double score = CalculateScore(xor_result, length);
        free((void *) xor_result);

        if (score != DBL_MAX)
        {
            print_d("score with key '%c' (0x%02x) is %.5f \"%s\"\n", isprint((int) c) ? (int) c : ' ', (int) c, score, tmpbuf);
        }

#ifdef DEBUG
        free(tmpbuf);
#endif

        if (score < min_score)
        {
            min_score = score;
            key = c;
        }
    }

    print_d("key '%c' (0x%02x) gives a minimum score of %.3f\n", (int) key, (int) key, min_score);

    memset(buffer, key, length);
    xor_result = FixedXOR(hex, buffer, length); /* TODO check; free()'d by caller */
    free(buffer);

    struct result *result = malloc(sizeof *result); /* TODO check; free()'d by caller */

    result->keysize = 1;
    result->key.c = key;
    result->score = min_score;
    result->hex = xor_result;
    result->text = malloc(length + 1); /* TODO check; free()'d by caller */

    memcpy(result->text, xor_result, length);
    result->text[length] = '\0';


    return result;
}
