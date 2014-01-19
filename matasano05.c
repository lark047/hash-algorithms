#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matasano.h"
// #include "matasano-common.h"

const uint8_t *RepeatingKeyXOR(const char * const msg, const char * const key)
{
    if (msg == NULL || key == NULL)
    {
        /* TODO maybe a better way? */
        return NULL;
    }

    const size_t msg_length = strlen(msg);
    const size_t key_length = strlen(key);

    print_d("%s\n", "");
    print_d("length of \"%s\" is %zu\n", msg, msg_length);
    print_d("length of \"%s\" is %zu\n", key, key_length);

    char *buffer = malloc(msg_length + 1); /* TODO check */
    uint8_t *hex1 = malloc(msg_length * sizeof *hex1); /* TODO check */
    uint8_t *hex2 = malloc(msg_length * sizeof *hex2); /* TODO check */

    if (msg_length == key_length)
    {
        strcpy(buffer, key);
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

            strcpy(buffer + i, key);
        }

        print_d("i = %zu\n", i);
        print_d("buffer is \"%s\" (%zu)\n", buffer, strlen(buffer));

        if (i < msg_length)
        {
            print_d("%zu > %zu\n", i, msg_length);
            strncpy(buffer + i, key, msg_length - i);
        }

        print_d("i = %zu\n", i);
        print_d("buffer is \"%s\" (%zu)\n", buffer, strlen(buffer));
    }

    memcpy(hex1, msg, msg_length);
    memcpy(hex2, buffer, msg_length);

    const uint8_t *result = FixedXOR(hex1, hex2, msg_length);

    free(buffer);
    free(hex1);
    free(hex2);

    return result;
}
