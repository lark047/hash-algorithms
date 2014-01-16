#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "matasano.h"
#include "matasano-test.h"
#include "matasano-common.h"

int main(int argc, char **argv)
{
    int rc = EXIT_FAILURE;

    if (argc == 2 && (STR_EQ(argv[1], "-t") || STR_EQ(argv[1], "--test")))
    {
        const char *msg = RunTests();

        if (!STR_EQ(msg, ""))
        {
            fprintf(stderr, msg);
        }

        rc = (STR_EQ(msg, "") ? EXIT_SUCCESS : EXIT_FAILURE);
    }
    else if (argc == 2)
    {
        char *base64 = malloc(2 * ceil(strlen(argv[1]) / 3) + 1);
        uint8_t *bytes = malloc(strlen(argv[1]) / 2 + 1);

        if (base64 && bytes)
        {
            EncodeBase64(argv[1], base64);
            DecodeBase64((const char *) base64, bytes);

            printf("original: %s\n", argv[1]);
            printf("encoded: %s\n", base64);
            printf("decoded: %s\n", bytes);

            printf("\n\n");

            for (uint8_t i = 0; i < strlen(argv[1]) / 2; ++i)
            {
                printf("0x%02x ", bytes[i]);
                fflush(stdout);
            }

            free(base64);
            free(bytes);

            rc = EXIT_SUCCESS;
        }
    }
    else if (argc == 3 && (STR_EQ(argv[1], "-e") || STR_EQ(argv[1], "--encode")))
    {
        char *base64 = malloc(2 * ceil(strlen(argv[2]) / 3) + 1);
        if (base64)
        {
            EncodeBase64(argv[2], base64);
            printf("%s\n", base64);
            free(base64);
            rc = EXIT_SUCCESS;
        }
    }
    else if (argc == 3 && (STR_EQ(argv[1], "-d") || STR_EQ(argv[1], "--decode")))
    {
        // TODO strlen(argv[1]) % 4 == 0

        uint8_t *bytes = malloc(3 * strlen(argv[2]) / 4 + 1);
        print_d("allocated %zu bytes\n", 3 * strlen(argv[2]) / 4 + 1);
        print_d("%s\n", argv[2]);

        if (bytes)
        {
            DecodeBase64(argv[2], bytes);
            print_d("input length: %zu\n", strlen(argv[2]));
            print_d("decoded byte length: %lu\n", (size_t) (3 * strlen(argv[2]) / 4));

            PrintHexWithSpace(bytes, 3 * strlen(argv[2]) / 4);

            free(bytes);
            rc = EXIT_SUCCESS;
        }
    }

    return rc;
}
