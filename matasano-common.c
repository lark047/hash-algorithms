#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>
#include <stdbool.h>

#include "matasano-common.h"

static void PrintHelper(const uint8_t * const, const uint64_t, const char *, bool, int (*)(int));

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
        msg[i] = (isalpha(hex[i]) || hex[i] == ' ' ? hex[i] : '*');
        // if (!(isalpha(hex[i]) || hex[i] == ' ')) { print_d("printing '*' for 0x%02x\n", hex[i]); }
    }
}

static int always(int c)
{
    return 1;
}

void PrintHex(const uint8_t * const hex, const uint64_t length, bool raw)
{
    PrintHelper(hex, length, "", raw, always);
}

void PrintHexWithSpace(const uint8_t * const hex, const uint64_t length)
{
    PrintHelper(hex, length, " ", true, always);
}

static int alphaplus(int c)
{
    return isalpha(c) || c == ' ';
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
