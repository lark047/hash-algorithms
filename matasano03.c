#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>
#include <math.h>
#include <float.h>

#include "matasano.h"
#include "matasano-common.h"

#define ASIZE                           26
#define ALPHA "abcdefghijklmnopqrstuvwxyz"

struct entry
{
    /* one-character key used to decode the ciphertext */
    uint8_t key;

    /* letter count (per occurrence) */
    uint64_t freq[ASIZE];

    /* sample size */
    uint32_t factor;

    /* letter count (per letter) */
    uint32_t count;
};

static double calculate_score(struct entry *, struct entry *);

uint8_t DecodeXOR(const uint8_t * const hex, const uint64_t length, const char *msg)
{
    print_d("%s\n", "**********************************");
    const uint8_t *xor_result;
    uint8_t *buffer = malloc(length * sizeof *buffer);

    /*
     * English frequency
     * from https://en.wikipedia.org/wiki/Letter_frequency#Relative_frequencies_of_letters_in_the_English_language
     */
    struct entry ef, tf;

    ef.freq[ 0] =  8167;
    ef.freq[ 1] =  1492;
    ef.freq[ 2] =  2782;
    ef.freq[ 3] =  4253;
    ef.freq[ 4] = 12702;
    ef.freq[ 5] =  2228;
    ef.freq[ 6] =  2015;
    ef.freq[ 7] =  6094;
    ef.freq[ 8] =  6966;
    ef.freq[ 9] =   153;
    ef.freq[10] =   772;
    ef.freq[11] =  4025;
    ef.freq[12] =  2406;
    ef.freq[13] =  6749;
    ef.freq[14] =  7507;
    ef.freq[15] =  1929;
    ef.freq[16] =    95;
    ef.freq[17] =  5987;
    ef.freq[18] =  6327;
    ef.freq[19] =  9056;
    ef.freq[20] =  2758;
    ef.freq[21] =   978;
    ef.freq[22] =  2360;
    ef.freq[23] =   150;
    ef.freq[24] =  1974;
    ef.freq[25] =    74;

    ef.factor = 100000;
    tf.factor = tf.count = 0;

    uint8_t key = 0;
    double score, min_score = DBL_MAX;

    for (uint16_t i = 0; i <= UCHAR_MAX; ++i)
    {
        tf.key = i;
        memset(tf.freq, 0, ASIZE * sizeof *(tf.freq));
        tf.factor = 0;

        print_d("XORing with 0x%02x\n", (int) tf.key);

        memset(buffer, i, length);
        xor_result = FixedXOR(hex, buffer, length);

        for (uint64_t j = 0; j < length; ++j)
        {
            if (64 < xor_result[j] && xor_result[j] < 123)
            {
                // print_d("result[%2" PRIu64 "] = 0x%02x (%c)\n", j, xor_result[j], xor_result[j]);

                int index = tolower(xor_result[j]) - 'a';

                /* count letters */
                tf.count |= (tf.count & (uint32_t) pow(2, index) ? 0 : (uint32_t) pow(2, index));

                ++tf.freq[index];
                ++tf.factor;
            }
        }

        free((void *) xor_result);

        score = calculate_score(&tf, &ef);
        print_d("score with key '%c' (0x%02x) is %.3f\n", isprint((int) i) ? (int) i : ' ', (int) i, score);

        if (score < min_score)
        {
            min_score = score;
            key = i;
        }
    }

    print_d("key '%c' (0x%02x) gives a minimum score of %.3f\n", (int) key, (int) key, min_score);

    memset(buffer, key, length);
    xor_result = FixedXOR(hex, buffer, length);

    HexToString(xor_result, length);

    free(buffer);
    free((void *) xor_result);
}

static double calculate_score(struct entry *text, struct entry *english)
{
    double score = 0;

    /* TODO multiply tf.count * 1 / (ef.freq[i] - tf.freq[i]) */

    print_d("calculating score for '%c' (0x%02x)\n", (int) text->key, (int) text->key);
    for (uint8_t i = 0; i < ASIZE; ++i)
    {
        double ef = (double) english->freq[i] / english->factor;
        double tf = (text->factor == 0 ? 0 : (double) text->freq[i] / text->factor);
        print_d("ef[%2" PRIu8 "] = %.5f\n", i, ef);
        print_d("tf[%2" PRIu8 "] = %.5f\n", i, tf);
        score += fabs(ef - tf);
    }

    return score;
}
