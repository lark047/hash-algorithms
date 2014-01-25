#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <float.h>

#include "matasano.h"

#define LINESIZE (60 + 1 + 1)

enum GuessType
{
    TWO_BLOCKS,
    TOP_THREE,
    FOUR_BLOCKS
};

static enum GuessType guess_type = FOUR_BLOCKS;

static uint8_t guess_keysize(const uint8_t *, const uint64_t, const uint8_t);
static size_t compute_hamming_distance(const uint8_t *, const uint8_t *, const uint8_t);
static void transpose(uint8_t **, uint8_t, size_t, uint8_t **); /* TODO change size_t to uint32_t */

const uint8_t *BreakRepeatingKeyXOR(FILE *fp)
{
    print_d("%s\n", "");
    //size_t dist = ComputeHammingDistance("this is a test", "wokka wokka!!!");
    //print_d("hamming distance is %zu\n", dist);

    size_t bytes_read = 0;
    char line[LINESIZE], *base64 = NULL;

    while (fgets(line, sizeof line, fp) != NULL)
    {
        char *p = strchr(line, '\n');
        if (p)
        {
            *p = '\0';
        }

        print_d("read \"%s\" (%zu)\n", line, strlen(line));

        size_t len = strlen(line);
        base64 = realloc(base64, bytes_read + len + 1); /* TODO check */

        strncpy(base64 + bytes_read, line, len);
        bytes_read += len;
    }

    print_d("base64 = \"%s\"\n", base64);
    print_d("encoded length = %zu\n", strlen(base64));
    print_d("decoded length = %zu\n", 3 * strlen(base64) / 4);

    size_t decoded_length = 3 * strlen(base64) / 4;
    uint8_t *decoded_base64 = malloc(decoded_length * sizeof *decoded_base64); /* TODO check */

    DecodeBase64(base64, decoded_base64);
    PrintHex(decoded_base64, decoded_length, true);
    free(base64);

    /* a. Let KEYSIZE be the guessed length of the key; try values from 2 to (say) 40. */
    uint8_t keysize = guess_keysize(decoded_base64, decoded_length, 200);

    size_t partitions = decoded_length / keysize;
    partitions += (decoded_length % keysize == 0 ? 0 : 1);

    print_d("dividing into %zu partitions\n", partitions);
    print_d("creating %zu x %" PRIu8 " (%zu block) array\n", partitions, keysize, partitions * keysize);

    /* <partitions> blocks of size <keysize>, or <partitions> x <keysize> array */
    uint8_t **blocks = malloc(partitions * sizeof *blocks); /* TODO check */

    /* <keysize> blocks of size <partitions>, or <keysize> x <partitions> array */
    uint8_t **transposed = malloc(keysize * sizeof *transposed); /* TODO check */

    for (uint8_t i = 0; i < keysize; ++i)
    {
        transposed[i] = malloc(partitions * sizeof *transposed); /* TODO check */
    }

    for (size_t i = 0; i < partitions; ++i)
    {
        blocks[i] = malloc(keysize * sizeof *blocks[i]); /* TODO check */
        memset(blocks[i], 0, keysize);

        uint8_t bytes = keysize;

        if ((i + 1) * keysize > decoded_length)
        {
            bytes -= (i + 1) * keysize % decoded_length;
        }

        print_d("copying %" PRIu8 " bytes into block %zu\n", bytes, i);
        memcpy(blocks[i], decoded_base64 + i * keysize, bytes);

        print_d("block %zu:\n", i);
        PrintHex(blocks[i], keysize, true);
    }

    transpose(blocks, keysize, partitions, transposed);

    /* done with blocks */
    for (size_t i = 0; i < partitions; ++i)
    {
        free(blocks[i]);
    }

    free(blocks);

    uint8_t *key = malloc(keysize * sizeof *key); /* TODO check */

    for (uint8_t i = 0; i < keysize; ++i)
    {
        struct result *result = DecodeXOR(transposed[i], partitions);
        key[i] = result->key;
        free(result);
    }

    print_d("%s\n", "key is:");
    PrintHex(key, keysize, true);

    for (uint8_t i = 0; i < keysize; ++i)
    {
        free(transposed[i]);
    }

    const uint8_t *result = RepeatingKeyXOR(decoded_base64, decoded_length, key, keysize);
    PrintHex(result, decoded_length, false);

    free(decoded_base64);
    free(transposed);
    free(key);
    free((void *) result);

    return NULL;
}

static uint8_t guess_keysize(const uint8_t *decoded_base64, const uint64_t decoded_length, const uint8_t bound)
{
    /* TWO_BLOCKS */
    /* FOUR_BLOCKS */
    uint8_t keysize = 0;
    double min_score = DBL_MAX;

    /* TOP_THREE */
    uint8_t three_keysizes[3] = { 0 };
    double min_three_scores[3] = { DBL_MAX, DBL_MAX, DBL_MAX };

#ifndef DEBUG
    keysize = 58;
    min_score = 1.5;
#else
    for (uint8_t i = 2; i <= bound; ++i)
    {
        print_d("computing Hamming distances for keysize %" PRIu8 "\n", i);

        size_t partitions = decoded_length / i;
        partitions += (decoded_length % i == 0 ? 0 : 1);

        // print_d("dividing into %zu partitions\n", partitions);
        uint8_t *hex1, *hex2;
        const size_t bytes = i * sizeof *hex1;
        double score = 0;

        if (guess_type != FOUR_BLOCKS)
        {
            /* using two keysize-length blocks */
            hex1 = malloc(bytes); /* TODO check */
            hex2 = malloc(bytes); /* TODO check */

            memcpy(hex1, decoded_base64, bytes);
            PrintHex(hex1, i, true);

            memcpy(hex2, decoded_base64 + bytes, bytes);
            PrintHex(hex2, i, true);

            score = compute_hamming_distance(hex1, hex2, i);

            free(hex1);
            free(hex2);
        }
        if (guess_type == FOUR_BLOCKS)
        {
            /* 4 KEYSIZE blocks instead of 2 and average the distances */
            for (uint8_t j = 0; j < 4; ++j)
            {
                switch (j % 2)
                {
                    case 0:
                        hex1 = malloc(bytes); /* TODO check */
                        memcpy(hex1, decoded_base64 + j * bytes, bytes);
                        PrintHex(hex1, i, true);
                        break;

                    case 1:
                        hex2 = malloc(bytes); /* TODO check */
                        memcpy(hex2, decoded_base64 + j * bytes, bytes);
                        PrintHex(hex2, i, true);

                        score += compute_hamming_distance(hex1, hex2, i);

                        break;
                }
            }

            score /= 2;
        }

        score /= i;

        print_d("score for keysize %" PRIu8 " is %.5f\n", i, score);

        if (guess_type != TOP_THREE)
        {
            if (score < min_score)
            {
                min_score = score;
                keysize = i;
            }
        }
        else
        {
            if (score < min_three_scores[0])
            {
                min_three_scores[0] = score;
                three_keysizes[0] = i;
            }
            else if (min_three_scores[0] < score && score < min_three_scores[1])
            {
                min_three_scores[1] = score;
                three_keysizes[1] = i;
            }
            else if (min_three_scores[1] < score && score < min_three_scores[2])
            {
                min_three_scores[2] = score;
                three_keysizes[2] = i;
            }
        }
    }
#endif

    if (guess_type == TWO_BLOCKS)
    {
        print_d("minimum score %.5f with keysize %" PRIu8 "\n", min_score, keysize);
    }
    else if (guess_type == TOP_THREE)
    {
        print_d("lowest score %.5f with keysize %" PRIu8 "\n", min_three_scores[0], three_keysizes[0]);
        print_d("lowest score %.5f with keysize %" PRIu8 "\n", min_three_scores[1], three_keysizes[1]);
        print_d("lowest score %.5f with keysize %" PRIu8 "\n", min_three_scores[2], three_keysizes[2]);
    }

    return keysize;
}

#if 1
/* b. Write a function to compute the edit distance/Hamming distance between two strings. */
static size_t compute_hamming_distance(const uint8_t *msg1, const uint8_t *msg2, uint8_t length)
{
    uint8_t xor;
    size_t diff = 0;

    for (size_t i = 0; i < length; ++i)
    {
        xor = msg1[i] ^ msg2[i];
        while (xor)
        {
            ++diff;
            xor &= xor - 1;
        }
    }

    return diff;
}
#endif

static void transpose(uint8_t **blocks, uint8_t keysize, size_t partitions, uint8_t **result)
{
    for (size_t i = 0; i < partitions; ++i)
    {
        for (uint8_t j = i; j < keysize; ++j)
        {
            if (i != j)
            {
                /* swap blocks[i][j] and blocks[j][i] */
                result[j][i] = blocks[i][j];
            }
        }
    }
}
