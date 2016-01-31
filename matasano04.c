#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <inttypes.h>

#include "matasano.h"

/* string length (60) + newline + nul byte */
#define BUFSIZE (60 + 1 + 1)

const struct result *DecodeXORFromFile(FILE *fp)
{
    char line[BUFSIZE], *p; /* TODO remove hard-coded size */
    uint8_t * const hex = malloc(30 * sizeof *hex); /* TODO ^ and check */
    const struct result *result = NULL;

    while (fgets(line, BUFSIZE, fp) != NULL)
    {
#if 0
        int c = fgetc(fp);
        if (!isspace(c))
        {
            print_d("next char is '%c' (0x%02x)\n", c, c);
            fputc(c, fp);
        }
#endif

        if ((p = strchr(line, '\n')) != NULL)
        {
            *p = 0;
        }

        if ((p = strchr(line, '\r')) != NULL)
        {
            *p = 0;
        }

        const uint8_t length = strlen(line) / 2;

        print_d("read \"%s\" (%" PRIu8 ")\n", line, length);

        StringToHex(line, hex);
        PrintHexWithSpace(hex, length);

        const struct result *tmp_result = DecodeXOR(hex, length);

        print_d("length %" PRIu8 " and score %f with key 0x%02x\n", length, tmp_result->score, tmp_result->key.c);

        if (result == NULL || tmp_result->score < result->score)
        {
            print_d("best score is %.5f\n", tmp_result->score);
            print_d("decoded with '%c' (0x%02x)\n", (isprint(tmp_result->key.c) ? tmp_result->key.c : '*'), tmp_result->key.c);
            PrintAsString(tmp_result->text, 30);
            result = tmp_result;
        }
        else
        {
            free((void *) tmp_result->hex);
            free(tmp_result->text);
            free((void *) tmp_result);
        }
    }

    free(hex);

    return result;
}
