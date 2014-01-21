#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>

#include "matasano.h"

#define LINESIZE (60 + 1 + 1)

// static size_t compute_hamming_distance(const uint8_t *, const uint8_t *, const uint8_t);
static void transpose(uint8_t **, uint8_t, size_t); /* TODO change size_t to uint32_t */

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

    print_d("base64 = \"%s\" (%zu)\n", base64, strlen(base64));

    size_t decoded_length = 3 * strlen(base64) / 4;
    uint8_t *decoded_base64 = malloc(decoded_length * sizeof *decoded_base64);

    DecodeBase64(base64, decoded_base64);
    PrintHex(decoded_base64, decoded_length);

    uint8_t keysize = 0;

#ifdef DEBUG
    keysize = 29;
#else
    double prev_score = 0, prev_pct = 0, max_ratio = 0;
    for (uint8_t i = 2; i <= 50; ++i)
    {
        print_d("computing Hamming distances for keysize %" PRIu8 "\n", i);

        uint8_t *hex1, *hex2;
        double score = 0, pct = 0;

        size_t partitions = (size_t) floor((double) decoded_length / i);
        partitions += (decoded_length % i == 0 ? 0 : 1);
        print_d("dividing into %zu partitions\n", partitions);

        /* using two keysize-length blocks */
        for (size_t j = 0; j < partitions; j += 2)
        {
            const size_t bytes = i * sizeof *hex1;

            hex1 = malloc(bytes); /* TODO check */
            hex2 = malloc(bytes); /* TODO check */

            memcpy(hex1, decoded_base64 + j * bytes, bytes);
            PrintHex(hex1, i);

            memcpy(hex2, decoded_base64 + (j + 1) * bytes, bytes);
            PrintHex(hex2, i);

            score += (double) ComputeHammingDistance(hex1, hex2, i) / i;

            free(hex1);
            free(hex2);
        }

        print_d("score for keysize %" PRIu8 " is %.5f\n", i, score);

        if (prev_score != 0)
        {
            pct = 100 * (score - prev_score) / score;
            print_d("pct change from previous = %2.5f%%\n", pct);
        }

        if (prev_pct != 0 && max_ratio < pct / prev_pct)
        {
            max_ratio = pct / prev_pct;
            keysize = i;
            print_d("keysize 0x%02x (%" PRIu8 ") has ratio %.5f\n", keysize, keysize, max_ratio);
        }

        prev_score = score;
        prev_pct = pct;
    }
#endif

    size_t partitions = (size_t) floor((double) decoded_length / keysize);
    partitions += (decoded_length % keysize == 0 ? 0 : 1);
    print_d("dividing into %zu partitions\n", partitions);
    print_d("decoded length = %zu\n", decoded_length);
    print_d("creating %zu x %" PRIu8 " (%zu block) array\n", partitions, keysize, partitions * keysize);

    uint8_t **blocks = malloc(partitions * sizeof *blocks); /* TODO check */

    for (size_t i = 0; i < partitions; ++i)
    {
        blocks[i] = malloc(keysize * sizeof *blocks[i]); /* TODO check */
        memset(blocks[i], 0, keysize);

        uint8_t bytes;

        if ((i + 1) * keysize > decoded_length)
        {
            bytes = keysize - ((i + 1) * keysize % decoded_length);
        }
        else
        {
            bytes = keysize;
        }

        print_d("copying %" PRIu8 " bytes into block %zu\n", bytes, i);
        memcpy(blocks[i], decoded_base64 + i * keysize, bytes);

        print_d("block %zu:\n", i);
        PrintHex(blocks[i], keysize);
    }

    transpose(blocks, keysize, partitions);

    uint8_t *key = malloc(partitions * sizeof *key); /* TODO check */

    for (size_t i = 0; i < partitions; ++i)
    {
        struct *result = DecodeXOR(blocks[i], keysize);
        key[i] = result->key;
    }

    for (size_t i = 0; i < partitions; ++i)
    {
        free(blocks[i]);
    }

    free(decoded_base64);
    free(blocks);
    free(key);

    return NULL;
}

#if 0
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

static void transpose(uint8_t **blocks, uint8_t keysize, size_t partitions)
{
    for (size_t i = 0; i < partitions; ++i)
    {
        for (uint8_t j = i; j < keysize; ++j)
        {
            if (i != j)
            {
                /* swap blocks[i][j] and blocks[j][i] */
                uint8_t tmp = blocks[i][j];
                blocks[i][j] = blocks[j][i];
                blocks[j][i] = tmp;
            }
        }
    }
}
