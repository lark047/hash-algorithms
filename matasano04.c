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

        const struct result *tmp = DecodeXOR(hex, length);

        print_d("length %" PRIu8 " and score %f with key 0x%02x\n", length, tmp->score, tmp->key.c);

        // print_d("decoded with '%c' (0x%02x)\n", (isprint(tmp->key) ? tmp->key : '*'), tmp->key);
        // PrintAsString(tmp->text, 30);

        if (result == NULL || result->score < tmp->score)
        {
            result = tmp;
        }
        else
        {
            free((void *) tmp->hex);
            free(tmp->text);
            free((void *) tmp);
        }
    }

    free(hex);

    return result;
}
