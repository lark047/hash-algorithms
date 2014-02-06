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

#define ASIZE                                27
#define ALPHA     "abcdefghijklmnopqrstuvwxyz "
#define PLURAL(x)         ((x) == 1 ? "" : "s")

static const uint32_t LETTER_COUNT[ASIZE] = {
#if 0
    // from https://en.wikipedia.org/wiki/Letter_frequency#Relative_frequencies_of_letters_in_the_English_language
     8167,  1492, 2782, 4253, 12702,
     2228,  2015, 6094, 6966,   153,
      772,  4025, 2406, 6749,  7507,
     1929,    95, 5987, 6327,  9056,
     2758,   978, 2360,  150,  1974,
       74, 19181
};
#elif 0
    // from http://www.data-compression.com/english.html
    331535,  63204, 110559, 177959, 529775,
    100661,  80684, 250729, 283899,   4595,
     25704, 168627, 102819, 287164, 303335,
     70019,   4378, 253107, 262364, 371019,
    114524,  42172,  87125,   6965,  74261,
      3986, 975767

#elif 1
    // from http://www.cryptograms.org/letter-frequencies.php#Results_from_Project_Gutenberg
    248362256,  47673928,  79962026, 134044565, 390395169,
     72967175,  61549736, 193607737, 214822972,   4507165,
     22969448, 125951672,  79502870, 214319386, 235661502,
     55746578,   3649838, 184990759, 196844692, 282039486,
     88219598,  30476191,  69069021,   5574077,  59010696,
      2456495, 595475632
#endif
};

static const uint32_t SAMPLE_TEXT_LENGTH =
#if 0
                 100000
#elif 0
                5086936
#elif 1
             3104375038
#endif
;

struct freq
{
    /* letter count (per occurrence) */
    uint32_t *freq;

    /* sample text length */
    uint32_t length;

    /* distinct letter count */
    // @Deprecated
    uint32_t count;

    /* alpha or ' ' hits */
    uint32_t hits;
};

static double calculate_score(const struct freq * const, const struct freq * const);

const struct result *DecodeXOR(const uint8_t * const hex, const uint64_t length)
{
    const uint8_t *xor_result;
    uint8_t *buffer = malloc(length * sizeof *buffer); /* TODO check */

    struct freq ef, tf;

    ef.freq = (uint32_t *) LETTER_COUNT;
    ef.length = SAMPLE_TEXT_LENGTH;

    if ((tf.freq = malloc(ASIZE * sizeof *(tf.freq))) == NULL)
    {
        /* TODO handle more appropriately */
        return 0x0;
    }

    uint8_t key = 0;
    double score, max_score = DBL_MIN;

    /* from 0x00 to 0xff */
    for (uint16_t c = 0; c <= UCHAR_MAX; ++c)
    {
        /* reset */
        memset(tf.freq, 0, ASIZE * sizeof *tf.freq);
        tf.count = 0;
        tf.length = tf.hits = length;

        memset(buffer, c, tf.length);
        xor_result = FixedXOR(hex, buffer, tf.length);

        /* go through the decoded string */
        for (uint64_t i = 0; i < tf.length; ++i)
        {
            if (isalpha(xor_result[i]) || xor_result[i] == ' ')
            {
                // print_d("result[%2" PRIu64 "] = 0x%02x (%c)\n", i, xor_result[i], xor_result[i]);

                int index = (isalpha(xor_result[i]) ? tolower(xor_result[i]) - 'a' : 26);

                /* count letters */
                uint32_t mask = 1 << index;
                tf.count |= ((tf.count & mask) >> mask ? 0 : mask);
                // print_d("(1) letter count = %07lx (2^%d) after '%c'\n", (long unsigned) tf.count, index, xor_result[i]);

                ++tf.freq[index];
            }
            else
            {
                --tf.hits;
            }
        }

#ifdef DEBUG
        unsigned char * const tmpbuf = malloc(length + 1); /* TODO check */
        HexToCleanString(xor_result, tf.length, tmpbuf);
        tmpbuf[length] = '\0';
#endif

        free((void *) xor_result);

        score = calculate_score(&tf, &ef);

        if (score > 0)
        {
            print_d("score with key '%c' (0x%02x) is %.3f \"%s\"\n", isprint((int) c) ? (int) c : ' ', (int) c, score, tmpbuf);
        }

        if (score > max_score)
        {
            max_score = score;
            key = c;
        }
#ifdef DEBUG
        free(tmpbuf);
#endif
    }

    print_d("key '%c' (0x%02x) gives a maximum score of %.3f\n", (int) key, (int) key, max_score);

    memset(buffer, key, tf.length);
    xor_result = FixedXOR(hex, buffer, tf.length); /* TODO check; free()'d by caller */

    struct result *result = malloc(sizeof *result); /* TODO check; free()'d by caller */

    result->key = key;
    result->score = max_score;
    result->hex = xor_result;
    result->text = malloc(tf.length + 1); /* TODO check; free()'d by caller */

    memcpy(result->text, xor_result, tf.length);
    result->text[tf.length] = '\0';

    free(tf.freq);
    free(buffer);

    return result;
}

static double calculate_score(const struct freq *text, const struct freq *english)
{
    const double hit_score = (double) text->hits / text->length;
    double freq_score = 0;

    for (uint8_t i = 0; i < ASIZE; ++i)
    {
        if (text->freq[i] > 0)
        {
            const double ef = (double) english->freq[i] / english->length;
            const uint32_t expected_count = ef * text->length;

            /* letter score */
            double lscore;

            if (expected_count == text->freq[i])
            {
                print_d("match: '%c'\n", ALPHA[i]);
                lscore = ef * text->length;
            }
            else
            {
                const double tf = (double) text->freq[i] / text->length;
                print_d("ef('%c') = %.5f\n", ALPHA[i], ef);
                print_d("tf('%c') = %.5f\n", ALPHA[i], tf);
                lscore = ef / fabs(ef - tf);
            }

            print_d("adding %.5f for '%c'\n", lscore, ALPHA[i]);
            freq_score += lscore;
            print_d("freq score: %f\n", freq_score);
        }
    }

    if (hit_score > 0)
    {
        print_d("normalizing by %.5f (%" PRIu32 " / %" PRIu32 ")\n", hit_score, text->hits, text->length);
    }

    return freq_score * hit_score;
}
