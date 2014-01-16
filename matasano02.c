#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "matasano.h"

const uint8_t *FixedXOR(const uint8_t * const hex1, const uint8_t * const hex2, const uint64_t length)
{
    if (hex1 == NULL || hex2 == NULL)
    {
        // TODO
        return NULL;
    }

    /* caller should free */
    uint8_t * const result = malloc(length * sizeof *result);

    if (result == NULL)
    {
        // TODO
        return NULL;
    }

    for (uint64_t i = 0; i < length; ++i)
    {
        result[i] = hex1[i] ^ hex2[i];
    }

    return result;
}
