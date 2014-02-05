#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
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

static enum GuessType guess_type = TOP_THREE;

static uint8_t *guess_keysize(const uint8_t *, const uint64_t, const uint8_t);
static size_t compute_hamming_distance(const uint8_t *, const uint8_t *, const uint8_t);
static void transpose(uint8_t **, uint8_t, size_t, uint8_t **); /* TODO change size_t to uint32_t */
static void free_array(uint8_t ***, const uint64_t);

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

    const uint8_t *hex = DecodeBase64(base64);
    PrintHex(hex, decoded_length, true);
    free(base64);

    /* a. Let KEYSIZE be the guessed length of the key; try values from 2 to (say) 40. */
    uint8_t *keysize = guess_keysize(hex, decoded_length, 200);

    for (uint8_t i = 0; keysize[i]; ++i)
    {
        size_t partitions = decoded_length / keysize[i];
        partitions += (decoded_length % keysize[i] == 0 ? 0 : 1);

        print_d("dividing into %zu partitions\n", partitions);
        print_d("creating %zu x %" PRIu8 " (%zu block) array\n", partitions, keysize[i], partitions * keysize[i]);

        /* <partitions> blocks of size <keysize>, or <partitions> x <keysize> array */
        uint8_t **blocks = malloc(partitions * sizeof *blocks); /* TODO check */

        /* <keysize> blocks of size <partitions>, or <keysize> x <partitions> array */
        uint8_t **transposed = malloc(keysize[i] * sizeof *transposed); /* TODO check */

        for (uint8_t j = 0; j < keysize[i]; ++j)
        {
            transposed[j] = malloc(partitions * sizeof *transposed); /* TODO check */
        }

        /* Now that you probably know the KEYSIZE: break the ciphertext into blocks of KEYSIZE length. */
        for (size_t j = 0; j < partitions; ++j)
        {
            blocks[j] = malloc(keysize[i] * sizeof *blocks[j]); /* TODO check */
            memset(blocks[j], 0, keysize[i]);

            uint8_t bytes = keysize[i];

            if ((j + 1) * keysize[i] > decoded_length)
            {
                bytes -= (j + 1) * keysize[i] % decoded_length;
            }

            print_d("copying %" PRIu8 " bytes into block %zu\n", bytes, j);
            memcpy(blocks[j], hex + j * keysize[i], bytes);

            print_d("block %zu:\n", j);
            PrintHex(blocks[j], keysize[i], true);
        }

        /* f. Now transpose the blocks: ... */
        transpose(blocks, keysize[i], partitions, transposed);
        print_d("%s\n", "transposed blocks");
        for (uint8_t j = 0; j < keysize[i]; ++j)
        {
            PrintHex(transposed[j], partitions, true);
        }

        /* done with blocks */
        free_array(&blocks, partitions);

        uint8_t *key = malloc(keysize[i] * sizeof *key); /* TODO check */

        /* g. Solve each block as if it was single-character XOR. */
        for (uint8_t j = 0; j < keysize[i]; ++j)
        {
            const struct result *r = DecodeXOR(transposed[j], partitions);
            key[j] = r->key;
            free((void *) r->hex);
            free(r->text);
            free((void *) r);
        }

        print_d("%s\n", "key is:");
        PrintHex(key, keysize[i], true);

        /* done with transposed */
        free_array(&transposed, keysize[i]);

        const uint8_t *result = RepeatingKeyXOR(hex, decoded_length, key, keysize[i]);
        PrintAsString(result, decoded_length);

        free(key);
        free((void *) result);
    }

    free((void *) hex);
    free(keysize);

    return NULL;
}

static uint8_t *guess_keysize(const uint8_t *hex, const uint64_t decoded_length, const uint8_t bound)
{
    uint8_t *keysize;
    double *min_score;

    const uint8_t size = (guess_type == TOP_THREE ? 4 : 2);

    keysize = malloc(size * sizeof *keysize); /* TODO check */
    min_score = malloc((size - 1) * sizeof *min_score); /* TODO check */

    memset(keysize, 0, size * sizeof *keysize);
    for (uint8_t i = 0; i < size; ++i)
    {
        min_score[i] = DBL_MAX;
    }

    /* For each KEYSIZE, ... */
    for (uint8_t ks = 2; ks <= bound; ++ks)
    {
        print_d("computing Hamming distances for keysize %" PRIu8 "\n", ks);

        size_t partitions = decoded_length / ks;
        partitions += (decoded_length % ks == 0 ? 0 : 1);

        // print_d("dividing into %zu partitions\n", partitions);
        uint8_t *hex1, *hex2;
        const size_t bytes = ks * sizeof *hex1;
        double score = 0;

        if (guess_type != FOUR_BLOCKS)
        {
            /* ... take the FIRST KEYSIZE worth of bytes, and the SECOND KEYSIZE worth of bytes, ... */
            hex1 = malloc(bytes); /* TODO check */
            hex2 = malloc(bytes); /* TODO check */

            memcpy(hex1, hex, bytes);
            PrintHex(hex1, ks, true);

            memcpy(hex2, hex + bytes, bytes);
            PrintHex(hex2, ks, true);

            /* ... and find the edit distance between them. */
            score = compute_hamming_distance(hex1, hex2, ks);

            free(hex1);
            free(hex2);
        }
        if (guess_type == FOUR_BLOCKS)
        {
            /* 4 KEYSIZE blocks instead of 2 and average the distances */
            for (uint8_t i = 0; i < 4; ++i)
            {
                switch (i % 2)
                {
                    case 0:
                        hex1 = malloc(bytes); /* TODO check */
                        memcpy(hex1, hex + i * bytes, bytes);
                        PrintHex(hex1, ks, true);
                        break;

                    case 1:
                        hex2 = malloc(bytes); /* TODO check */
                        memcpy(hex2, hex + i * bytes, bytes);
                        PrintHex(hex2, ks, true);

                        score += compute_hamming_distance(hex1, hex2, ks);

                        break;
                }
            }

            score /= 2;

            free(hex1);
            free(hex2);
        }

        /* ... Normalize this result by dividing by KEYSIZE. */
        score /= ks;

        print_d("score for keysize %" PRIu8 " is %.5f\n", ks, score);

        if (guess_type == TOP_THREE)
        {
            if (score < min_score[0])
            {
                min_score[0] = score;
                keysize[0] = ks;
            }
            else if (min_score[0] < score && score < min_score[1])
            {
                min_score[1] = score;
                keysize[1] = ks;
            }
            else if (min_score[1] < score && score < min_score[2])
            {
                min_score[2] = score;
                keysize[2] = ks;
            }
        }
        else
        {
            if (score < min_score[0])
            {
                min_score[0] = score;
                keysize[0] = ks;
            }
        }
    }

    if (guess_type == TWO_BLOCKS)
    {
        print_d("minimum score %.5f with keysize %" PRIu8 "\n", min_score[0], keysize[0]);
    }
    else if (guess_type == TOP_THREE)
    {
        print_d("1st lowest score %.5f with keysize %" PRIu8 "\n", min_score[0], keysize[0]);
        print_d("2nd lowest score %.5f with keysize %" PRIu8 "\n", min_score[1], keysize[1]);
        print_d("3rd lowest score %.5f with keysize %" PRIu8 "\n", min_score[2], keysize[2]);
    }

    free(min_score);

    return keysize;
}

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

/* ... make a block that is the first byte of every block, and a block that is the second byte of every block, and so on. */
static void transpose(uint8_t **blocks, uint8_t keysize, size_t partitions, uint8_t **result)
{
    for (size_t i = 0; i < partitions; ++i)
    {
        for (uint8_t j = 0; j < keysize; ++j)
        {
            result[j][i] = blocks[i][j];
        }
    }
}

static void free_array(uint8_t ***array, const uint64_t length)
{
    for (uint64_t i = 0; i < length; ++i)
    {
        free((*array)[i]);
    }
    free(*array);
    *array = NULL;
}
