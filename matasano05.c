#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "matasano.h"

const uint8_t *RepeatingKeyXOR(const uint8_t * const msg, uint64_t msg_length, const uint8_t * const key, uint64_t key_length)
{
    if (msg == NULL || key == NULL)
    {
        /* TODO maybe a better way? */
        return NULL;
    }

    print_d("start RepeatingKeyXOR(msg, %" PRIu64 ", key, %" PRIu64 ")\n", msg_length, key_length);
    uint8_t *buffer = malloc(msg_length); /* TODO check */

    if (msg_length == key_length)
    {
        memcpy(buffer, key, key_length);
    }
    else if (msg_length > key_length)
    {
        memset(buffer, 0, msg_length);

        size_t i;

        for (i = 0; i < msg_length; i+= key_length)
        {
            if (i + key_length > msg_length)
            {
                print_d("%zu + %zu > %zu\n", i, key_length, msg_length);
                break;
            }

            memcpy(buffer + i, key, key_length);
        }

        if (i < msg_length)
        {
            print_d("%zu < %zu\n", i, msg_length);
            memcpy(buffer + i, key, msg_length - i);
        }

        print_d("i = %zu, buffer is:\n", i);
        PrintHex(buffer, msg_length, true);
    }

    const uint8_t *result = FixedXOR(msg, buffer, msg_length);
    free(buffer);

    return result;
}
