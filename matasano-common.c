#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>

#include "matasano-common.h"

static struct freq english;

static const uint32_t LETTER_COUNT[] = {
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

static void PrintHelper(const uint8_t * const, const uint64_t, const char *, bool, int (*)(int));
static int alphaplus(int c);

void StringToHex(const char * const msg, uint8_t * const hex)
{
    // TODO assert strlen(msg) % 2 == 0

    for (uint64_t i = 0; i < strlen(msg) / 2; ++i)
    {
        // print_d("scanning \"%c%c\"\n", msg[2 * i], msg[2 * i + 1]);
        sscanf(msg + 2 * i, "%2" SCNx8, hex + i);
        // print_d("found byte 0x%02x (%u)\n", hex[i], hex[i]);
    }
}

void HexToCleanString(const uint8_t * const hex, const uint64_t length, unsigned char * const msg)
{
    for (uint64_t i = 0; i < length; ++i)
    {
        msg[i] = alphaplus(hex[i]) ? hex[i] : '*';
        // if (!(isalpha(hex[i]) || hex[i] == ' ')) { print_d("printing '*' for 0x%02x\n", hex[i]); }
    }
}

static int always(int c)
{
    return c == c;
}

void PrintHex(const uint8_t * const hex, const uint64_t length, bool raw)
{
    print_d("PrintHex(%p, %" PRIu64 ", %d)\n", (void *) hex, length, raw);
    PrintHelper(hex, length, "", raw, always);
}

void PrintHexWithSpace(const uint8_t * const hex, const uint64_t length)
{
    PrintHelper(hex, length, " ", true, always);
}

static int alphaplus(int c)
{
    return isalpha(c) || ispunct(c) || c == ' ' || isdigit(c);
}

void PrintAsString(const uint8_t * const hex, const uint64_t length)
{
    PrintHelper(hex, length, "", false, alphaplus);
}

static void PrintHelper(const uint8_t * const hex, const uint64_t length, const char *spacer, bool raw, int (*printable)(int))
{
    char *fmt = (raw ? "%02x%s" : "%c%s");

    for (uint64_t i = 0; i < length; ++i)
    {
        printf(fmt, ((*printable)(hex[i]) ? hex[i] : '*'), spacer);
        fflush(stdout);
    }

    printf("\n");
}

double CalculateScore(const uint8_t * const xor_result, const uint64_t length)
{
    struct freq text;

    english.freq = (uint32_t *) LETTER_COUNT;
    english.length = SAMPLE_TEXT_LENGTH;

    if ((text.freq = calloc(ASIZE, sizeof *(text.freq))) == NULL)
    {
        /* TODO handle more appropriately */
        print_d("could not allocate %zu bytes!\n", ASIZE * sizeof *(text.freq));
        return 0;
    }

    text.length = text.hits = length;

    for (uint64_t i = 0; i < length; ++i)
    {
        unsigned char c = xor_result[i];

        if (isalpha(c) || c == ' ')
        {
            int index = (isalpha(c) ? tolower(c) - 'a' : 26);
            ++text.freq[index];
        }
        else
        {
            --text.hits;
        }
    }

    if (text.hits == 0)
    {
        free(text.freq);
        return DBL_MAX;
    }

    // const double hit_score = (double) (ASIZE - text.hits + 1) / ASIZE;
    const double hit_score = (double) text.length / text.hits; /* inverse to minimize */
    double freq_score = 0;

    for (uint8_t i = 0; i < ASIZE; ++i)
    {
        const double ef = (double) english.freq[i] / english.length;
        // const double tf = (double) text.freq[i] / text.length;

        double lscore = pow(ef * text.length - text.freq[i], 2);
        print_d("exp('%c') = %.8f\n", ALPHA[i], ef * text.length);
        print_d("act('%c') = %" PRIu32 "\n", ALPHA[i], text.freq[i]);

        freq_score += lscore;
        print_d("adding %.8f for '%c'; total %.8f\n", lscore, ALPHA[i], freq_score);
    }

    // freq_score = 1 / sqrt(freq_score);
    freq_score = sqrt(freq_score);
    free(text.freq);

    print_d("normalizing %.5f by %.5f\n", freq_score, hit_score);
    return freq_score * hit_score;
}
