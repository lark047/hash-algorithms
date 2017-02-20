#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

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
        const char *base64 = EncodeBase64(argv[1]);
        const uint8_t *bytes = DecodeBase64(base64);

        printf("original: %s\n", argv[1]);
        printf("encoded: %s\n", base64);
        printf("decoded: %s\n", bytes);

        printf("\n\n");

        for (uint8_t i = 0; i < strlen(argv[1]) / 2; ++i)
        {
            printf("0x%02x ", bytes[i]);
            fflush(stdout);
        }

        free((void *) base64);
        free((void *) bytes);

        rc = EXIT_SUCCESS;
    }
    else if (argc == 3 && (STR_EQ(argv[1], "-e") || STR_EQ(argv[1], "--encode")))
    {
        errno = 0;

        const char *base64 = EncodeBase64(argv[2]);

        if (errno == 0)
        {
            printf("%s\n", base64);
            rc = EXIT_SUCCESS;
        }
        else
        {
            perror("EncodeBase64");
        }

        free((void *) base64);
    }
    else if (argc == 3 && (STR_EQ(argv[1], "-d") || STR_EQ(argv[1], "--decode")))
    {
        errno = 0;

        const uint8_t *bytes = DecodeBase64(argv[2]);
        print_d("input length: %zu\n", strlen(argv[2]));
        print_d("decoded byte length: %lu\n", (size_t) (3 * strlen(argv[2]) / 4));

        PrintHexWithSpace(bytes, 3 * strlen(argv[2]) / 4);

        free((void *) bytes);
        rc = EXIT_SUCCESS;
    }

    return rc;
}
