#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <stdbool.h>

#include "matasano.h"
#include "matasano-common.h"

#define ASIZE                           26
#define ALPHA "abcdefghijklmnopqrstuvwxyz"

static const uint32_t LETTER_COUNT[ASIZE] = {
#if 0
    // from https://en.wikipedia.org/wiki/Letter_frequency#Relative_frequencies_of_letters_in_the_English_language
     8167, 1492, 2782, 4253, 12702,
     2228, 2015, 6094, 6966,   153,
      772, 4025, 2406, 6749,  7507,
     1929,   95, 5987, 6327,  9056,
     2758,  978, 2360,  150,  1974,
       74
};
#elif 0
#elif 1
    // from http://www.cryptograms.org/letter-frequencies.php#Results_from_Project_Gutenberg
    248362256,  47673928,  79962026, 134044565, 390395169,
     72967175,  61549736, 193607737, 214822972,   4507165,
     22969448, 125951672,  79502870, 214319386, 235661502,
     55746578,   3649838, 184990759, 196844692, 282039486,
     88219598,  30476191,  69069021,   5574077,  59010696,
      2456495
#endif
};

static const uint32_t SAMPLE_SIZE =
#if 0
                 100000
#elif 0
#elif 1
             3104375038
#endif
;

struct freq
{
    /* letter count (per occurrence) */
    uint32_t *freq;

    /* sample size */
    uint32_t size;

    /* distinct letter count */
    uint32_t count;

    /* spaces */
    uint32_t spaces;
};

static double calculate_score(struct freq *, struct freq *);

uint8_t DecodeXOR(const uint8_t * const hex, const uint64_t length, const char *msg)
{
    print_d("%s\n", "**********************************");
    const uint8_t *xor_result;
    uint8_t *buffer = malloc(length * sizeof *buffer); /* TODO check */

    struct freq ef, tf;

    ef.freq = (uint32_t *) LETTER_COUNT;
    ef.size = SAMPLE_SIZE;

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
        memset(tf.freq, 0, ASIZE * sizeof *(tf.freq));
        tf.size = tf.count = tf.spaces = 0;

        print_d("XORing with 0x%02x\n", (int) c);

        memset(buffer, c, length);
        xor_result = FixedXOR(hex, buffer, length);

        /* go through the decoded string */
        for (uint64_t i = 0; i < length; ++i)
        {
            if (isalpha(xor_result[i]))
            {
                // print_d("result[%2" PRIu64 "] = 0x%02x (%c)\n", i, xor_result[i], xor_result[i]);

                int index = tolower(xor_result[i]) - 'a';

                /* count letters */
                uint32_t mask = 1 << index;
                tf.count |= ((tf.count & mask) >> mask ? 0 : mask);
                print_d("(1) letter count = %07lx (2^%d) after '%c'\n", (long unsigned) tf.count, index, xor_result[i]);

                ++tf.freq[index];
            }
            else if (xor_result[i] == ' ')
            {
                ++tf.spaces;
            }

            ++tf.size;
        }

        free((void *) xor_result);

        print_d("calculating score for '%c' (0x%02x)\n", (int) c, (int) c);
        score = calculate_score(&tf, &ef);
        score *= (double) tf.size / length;
        print_d("score with key '%c' (0x%02x) is %.3f\n", isprint((int) c) ? (int) c : ' ', (int) c, score);

        if (score > max_score)
        {
            max_score = score;
            key = c;
        }
    }

    print_d("key '%c' (0x%02x) gives a maximum score of %.3f\n", (int) key, (int) key, max_score);

    memset(buffer, key, length);
    xor_result = FixedXOR(hex, buffer, length);

    memcpy((char *) msg, xor_result, length);

    free(tf.freq);
    free(buffer);
    free((void *) xor_result);

    return key;
}

static double calculate_score(struct freq *text, struct freq *english)
{
    double freq_score = 0, ef, tf;
    uint8_t letter_count = 0;

    for (uint8_t i = 0; i < ASIZE; ++i)
    {
        bool present = (text->count & (1 << i)) >> i;
        letter_count += present;

        print_d("text->count = %" PRIu32 "\n", text->count);
        print_d("text->count & mask = %" PRIu32 "\n", text->count & (1 << i));
        print_d("(2) letter count = %" PRIu8 "\n", letter_count);

        if (present)
        {
            ef = (double) english->freq[i] / english->size;
            tf = (double) text->freq[i] / text->size;

            print_d("ef[%2" PRIu8 "] = %.5f\n", i, ef);
            print_d("tf[%2" PRIu8 "] = %.5f\n", i, tf);
            print_d("adding %.3f for '%c'\n", 1 / fabs(ef - tf), ALPHA[i]);

            freq_score += 1 / fabs(ef - tf);
        }
    }

    /* spaces */
    if (text->spaces > 0)
    {
        ++letter_count;
        ef = 19181.82 / english->size; // according to wiki TODO get other freqs
        tf = (double) text->spaces / text->size;
        print_d("ef(space) = %.5f\n", ef);
        print_d("tf(space) = %.5f (%" PRIu32 " / %" PRIu32 ")\n", tf, text->spaces, text->size);
        print_d("adding %.3f for '%c'\n", 1 / fabs(ef - tf), ' ');
        freq_score += 1 / fabs(ef - tf);
    }

#if 0
    /* other characters: frequency between 't' and 'a' (ignore for now) */
    ef = (double) (english->freq[19] + english->freq[0]) / (2 * english->size);
    tf = text->misc / (text->size + text->spaces + text->misc);
    print_d("warning: dividing by %.8f\n", fabs(ef - tf));
    freq_score += 1 / fabs(ef - tf);
#endif

    print_d("found %" PRIu8 " distinct letter(s)\n", letter_count);

    return freq_score * letter_count;
}
