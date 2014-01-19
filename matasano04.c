#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <inttypes.h>

#include "matasano.h"
#include "matasano-common.h"

/* string length (60) + newline + nul byte */
#define BUFSIZE (60 + 1 + 1)

struct result *DecodeXORFromFile(FILE *fp)
{
    char line[BUFSIZE], *p; /* TODO remove hard-coded size */
    uint8_t * const hex = malloc(30 * sizeof *hex); /* TODO ^ and check */
    struct result *result = NULL;

    while (fgets(line, BUFSIZE, fp) != NULL)
    {
        int c = fgetc(fp);
        if (!isspace(c))
        {
            print_d("next char is '%c' (0x%02x)\n", c, c);
            fputc(c, fp);
        }

        if ((p = strchr(line, '\n')) != NULL)
        {
            *p = 0;
        }

        if ((p = strchr(line, '\r')) != NULL)
        {
            *p = 0;
        }

        print_d("read \"%s\"\n", line);

        StringToHex(line, hex);
        // PrintHex(hex, 30);

        struct result *tmp = DecodeXOR(hex, 30);

        print_d("decoded with '%c' (0x%02x): %s\n", (isprint(tmp->key) ? tmp->key : ' '), tmp->key, tmp->text);

        if (result == NULL || result->score < tmp->score)
        {
            result = tmp;
        }
        else
        {
            free(tmp);
        }
    }

    free(hex);

    return result;
}
