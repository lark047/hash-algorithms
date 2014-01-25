#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matasano.h"

const uint8_t *RepeatingKeyXOR(const uint8_t * const msg, uint64_t msg_length, const uint8_t * const key, uint64_t key_length)
{
    if (msg == NULL || key == NULL)
    {
        /* TODO maybe a better way? */
        return NULL;
    }

    print_d("%s\n", "");
    print_d("length of \"%s\" is %zu\n", (char *) msg, msg_length);
    print_d("length of \"%s\" is %zu\n", (char *) key, key_length);

    uint8_t *buffer = malloc(msg_length + 1); /* TODO check */
    uint8_t *hex1 = malloc(msg_length * sizeof *hex1); /* TODO check */
    uint8_t *hex2 = malloc(msg_length * sizeof *hex2); /* TODO check */

    if (msg_length == key_length)
    {
        memcpy(buffer, key, key_length);
    }
    else
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

        print_d("i = %zu\n", i);
        // print_d("buffer is \"%s\" (%zu)\n", buffer, strlen(buffer));

        if (i < msg_length)
        {
            print_d("%zu > %zu\n", i, msg_length);
            memcpy(buffer + i, key, msg_length - i);
        }

        print_d("i = %zu\n", i);
        // print_d("buffer is \"%s\" (%zu)\n", buffer, strlen(buffer));
    }

    memcpy(hex1, msg, msg_length);
    memcpy(hex2, buffer, msg_length);

    const uint8_t *result = FixedXOR(hex1, hex2, msg_length);

    free(buffer);
    free(hex1);
    free(hex2);

    return result;
}
