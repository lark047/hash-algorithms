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

#if 0
#  define BGSIZE 20
#endif

//static const char *BG[BGSIZE] = {
#if 1
//    "th", "he", "in", "er", "an", "re", "nd", "on", "en", "at",
//    "ou", "ed", "ha", "to", "or", "it", "is", "hi", "es", "ng"
#endif
//};

//static const uint32_t BGCOUNT[BGSIZE] = {
#if 1
//   92535489, 87741289, 54433847, 51910883, 51015163, 41694599, 37466077, 33802063, 32967758, 31830493,
//   30637892, 30406590, 30381856, 27877259, 27434858, 27048699, 26452510, 26033632, 26033602, 25106109
#endif
//};

//static const uint32_t BG_SAMPLE_SIZE =
#if 1
//    2383373483
#endif
//;

#if 0
Cooking MC*s like a pound of bacon
    ^^^              ^^^^       ^^

in      match
ng      match
ou      match
nd      match
on      match
#endif

#if 0
struct bg_freq
{
    /* bigrams */
    char **bigrams;

    /* bigram count */
    uint32_t *count;

    /* sample text length */
    uint32_t length;

    /* bigrams hits */
    uint32_t hits;
};
#endif

static double calculate_score(const struct freq * const, const struct freq * const);
// static double calc_bg_score(const struct bg_freq * const, const struct bg_freq * const);

static FILE *fout;

const struct result *DecodeXOR(const uint8_t * const hex, const uint64_t length)
{
    // char *fn = tempnam("C:\\tmp", NULL);
    // print_d("printing to file \"%s\"\n", fn);
    // fout = fopen(fn, "w");
    fout = stdout;

    const uint8_t *xor_result;
    uint8_t *buffer = malloc(length * sizeof *buffer); /* TODO check */

    struct freq ef, tf;
    // struct bg_freq ebg, tbg;

    ef.freq = (uint32_t *) LETTER_COUNT;
    ef.length = SAMPLE_TEXT_LENGTH;

    // ebg.bigrams = (char **) BG;
    // ebg.count = (uint32_t *) BGCOUNT;
    // ebg.length = BG_SAMPLE_SIZE;

    if ((tf.freq = malloc(ASIZE * sizeof *(tf.freq))) == NULL)
    {
        /* TODO handle more appropriately */
        return 0x0;
    }

#if 0
    /* max (length - 1) bigrams */
    if ((tbg.bigrams = malloc((length - 1) * sizeof *(tbg.bigrams))) == NULL) /* TODO free */
    {
        /* TODO handle more appropriately */
        return 0;
    }

    for (uint64_t i = 0; i < length; ++i)
    {
        tbg.bigrams[i] = malloc(3); /* TODO check & free */
        memset(tbg.bigrams[i], 0, 3);
    }

    if ((tbg.count = malloc((length - 1) * sizeof *(tbg.count))) == NULL) /* TODO free */
    {
        /* TODO handle more appropriately */
        return 0;
    }

    memset(tbg.count, 0, (length - 1) * sizeof *(tbg.count));
#endif

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
                // fprint_d(fout, "result[%2" PRIu64 "] = 0x%02x (%c)\n", i, xor_result[i], xor_result[i]);

                int index = (isalpha(xor_result[i]) ? tolower(xor_result[i]) - 'a' : 26);

                /* count letters */
                uint32_t mask = 1 << index;
                tf.count |= ((tf.count & mask) >> mask ? 0 : mask);
                // fprint_d(fout, "(1) letter count = %07lx (2^%d) after '%c'\n", (long unsigned) tf.count, index, xor_result[i]);

                ++tf.freq[index];
            }
            else
            {
                --tf.hits;
            }
        }

#if 0
        memset(tbg.count, 0, BGSIZE * sizeof *(tbg.count));
        tbg.hits = 0;
        tbg.length = length - 1;

        /* record bigrams */
        for (uint64_t i = 0; i < tf.length; ++i)
        {
            if (isalpha(xor_result[i]))
            {
                if (i + 1 >= tf.length || !isalpha(xor_result[i + 1]))
                {
                    /* not a bigram */
                    print_d("not a bigram: \"%c%c\"\n", xor_result[i], '*');
                    continue;
                }

                const char tmpbg[] = {
                    xor_result[i], xor_result[i + 1], '\0'
                };
                bool addbg = true;

                for (uint8_t j = 0; j < BGSIZE; ++j)
                {
                    if (strcmp(tbg.bigrams[j], tmpbg) == 0)
                    {
                        ++tbg.count[j];
                        addbg = false;
                    }
                }

                if (addbg)
                {
                    strcpy(tbg.bigrams[tbg.hits], tmpbg);
                    ++tbg.count[tbg.hits];
                }

                print_d("found bigram \"%s\" %" PRIu32 " time%s\n", tbg.bigrams[tbg.hits], tbg.count[tbg.hits], PLURAL(tbg.count[tbg.hits]));

                ++tbg.hits;
            }
        }
#endif

#ifdef DEBUG
        unsigned char * const tmpbuf = malloc(length + 1); /* TODO check */
        HexToCleanString(xor_result, tf.length, tmpbuf);
        tmpbuf[length] = '\0';
#endif

        score  = calculate_score(&tf, &ef);
        // score += calc_bg_score(&tbg, &ebg);

        free((void *) xor_result);

        if (score > 0)
        {
            fprint_d(fout, "score with key '%c' (0x%02x) is %.3f \"%s\"\n", isprint((int) c) ? (int) c : ' ', (int) c, score, tmpbuf);
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

    fprint_d(fout, "key '%c' (0x%02x) gives a maximum score of %.3f\n", (int) key, (int) key, max_score);

    memset(buffer, key, tf.length);
    xor_result = FixedXOR(hex, buffer, tf.length); /* TODO check; free()'d by caller */

    struct result *result = malloc(sizeof *result); /* TODO check; free()'d by caller */

    result->key = key;
    result->score = max_score;
    result->hex = xor_result;

#if 0
    for (uint8_t i = 0; i < tf.length; ++i)
    {
        if (!(isalpha(xor_result[i]) || xor_result[i] == ' '))
        {
            xor_result[i] = '*';
        }
    }
#endif

    result->text = malloc(tf.length + 1); /* TODO check; free()'d by caller */
    memcpy(result->text, xor_result, tf.length);
    result->text[tf.length] = '\0';

    free(tf.freq);
    free(buffer);

    // fclose(fout);

    return result;
}

static double calculate_score(const struct freq *text, const struct freq *english)
{
    const double hit_score = (double) text->hits / text->length;
    double freq_score = 0;

    // uint8_t letter_count = 0;

    for (uint8_t i = 0; i < ASIZE; ++i)
    {
        if (text->freq[i] > 0)
        {
            // ++letter_count;

            const double ef = (double) english->freq[i] / english->length;

#if 0
            const double tf = (double) text->freq[i] / text->length;

            fprint_d(fout, "ef('%c') = %.5f\n", ALPHA[i], ef);
            fprint_d(fout, "tf('%c') = %.5f\n", ALPHA[i], tf);
            fprint_d(fout, "adding %.3f for '%c'\n", ef / fabs(ef - tf), ALPHA[i]);
#else
            const uint32_t expected_count = text->length * (double) english->freq[i] / english->length;

            /* letter score */
            double lscore;

            if (expected_count == text->freq[i])
            {
                lscore = ef * text->length;
            }
            else
            {
                const double tf = (double) text->freq[i] / text->length;
                lscore = ef / fabs(ef - tf);
            }
#endif

            freq_score += lscore;
            fprint_d(fout, "freq score: %f\n", freq_score);
        }
    }

#if 0
    /* other characters: frequency between 't' and 'a' (ignore for now) */
    ef = (double) (english->freq[19] + english->freq[0]) / (2 * english->length);
    tf = text->misc / (text->length + text->misc);
    //fprint_d(fout, "warning: dividing by %.8f\n", fabs(ef - tf));
    freq_score += 1 / fabs(ef - tf);
#endif

    // fprint_d(fout, "found %" PRIu8 " distinct letter(s)\n", letter_count);
    fprint_d(fout, "normalizing by %.5f (%" PRIu32 " / %" PRIu32 ")\n", hit_score, text->hits, text->length);

    return freq_score * hit_score;
}

#if 0
static double calc_bg_score(const struct bg_freq * const text, const struct bg_freq * const english)
{
    const double hit_score = (double) text->hits / (text->length - 1);
    double freq_score = 0;

    print_d("found %" PRIu32 " bigrams in the text\n", text->hits);

    for (uint8_t i = 0; i < text->hits; ++i)
    {
        for (uint8_t j = 0; j < BGSIZE; ++j)
        {
            // print_d("comparing \"%s\" and \"%s\"\n", text->bigrams[i], english->bigrams[j]);
            if (strcmp(english->bigrams[j], text->bigrams[i]) == 0)
            {
                const double ef = (double) english->count[j] / english->length;
                const double tf = (double) text->count[i] / text->length;

                fprint_d(fout, "ef(\"%s\") = %.5f\n", english->bigrams[j], ef);
                fprint_d(fout, "tf(\"%s\") = %.5f\n", text->bigrams[i], tf);
                fprint_d(fout, "adding %.3f for \"%s\"\n", ef / fabs(ef - tf), text->bigrams[i]);

                freq_score += pow(ef / fabs(ef - tf), 2);
                fprint_d(fout, "freq score: %f\n", freq_score);
            }
        }
    }

    return freq_score * hit_score;
}
#endif
