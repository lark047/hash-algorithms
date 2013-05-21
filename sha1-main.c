#include "sha.h"
#include "util.h"
#include "hmac.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

extern void testSHA1(void);

int main(int argc, char **argv)
{
    int rc = EXIT_FAILURE;

    if (argc == 2 && (STR_EQ(argv[1], "-t") || STR_EQ(argv[1], "--test")))
    {
        rc = do_test("SHA1 Test Suite", testSHA1);
    }
    else if (argc == 2)
    {
        rc = do_hash_string(argv[1], SHA1string, DIGEST_LENGTH);
    }
    else if (argc == 3 && (STR_EQ(argv[1], "-f") || STR_EQ(argv[1], "--file")))
    {
        char * const filename = argv[2];

        PRINT("Calculating SHA1 for \"%s\"...\n", filename);
        PRINT("Using byte size of %u\n", (unsigned) CHAR_BIT);

        FILE *fp = fopen(filename, "rb");

        if (fp)
        {
            uint8_t *digest = SHA1file(fp);
            char *buf = to_string(digest, DIGEST_LENGTH);

            printf("%s\n", buf);

            /* clean up */
            PRINT("%s\n", "Cleaning up...");
            free(digest);
            free(buf);
            fclose(fp);

            digest = NULL;
            buf = NULL;
            fp = NULL;

            rc = EXIT_SUCCESS;
        }
        else
        {
            fprintf(stderr, "[ERROR] Could not open %s for reading.", filename);
        }
    }
    else if (argc == 4 && (STR_EQ(argv[1], "-h") || STR_EQ(argv[1], "--hmac")))
    {
        uint8_t *digest = SHA1hmac(argv[2], argv[3]);
        char *buf = to_string(digest, DIGEST_LENGTH);

        printf("%s\n", buf);

        /* clean up */
        PRINT("%s\n", "Cleaning up...");
        free(digest);
        free(buf);
        digest = NULL;
        buf = NULL;

        rc = EXIT_SUCCESS;
    }
    else
    {
        printf("Usage: %s \"<message>\"\n", argv[0]);
        puts("  prints the SHA1 hash of <message>\n");
        printf("Usage: %s -f <filename>\n", argv[0]);
        puts("  prints the SHA1 hash of the file named <filename>\n");
        printf("Usage: %s -h \"<key>\" \"<message>\"\n", argv[0]);
        puts("  prints the SHA1 HMAC hash of <message> using <key>\n");
    }

    PRINT("Exiting with status %d\n", rc);
    return rc;
}
