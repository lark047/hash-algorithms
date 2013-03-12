#include "sha.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main(int argc, char **argv)
{
    int rc = EXIT_FAILURE;

    if (argc == 2)
    {
        char * const string = argv[1];

        PRINT("Calculating SHA1 for \"%s\"...\n", string);
        PRINT("Using byte size of %u\n", (unsigned) CHAR_BIT);

        uint8_t *digest = SHAstring(string);

    /* 0: 2fd4e1c67a2d28fced849ee1bb76e7391b93eb12 */
    /*    2fd4e1c67a2d28fced849ee1bb76e7391b93eb12 */

    /**
     * Results
     *
     * ""
     *   expected: da39a3ee5e6b4b0d3255bfef95601890afd80709
     *     actual: da39a3ee5e6b4b0d3255bfef95601890afd80709
     *
     * "a"
     *   expected: 86f7e437faa5a7fce15d1ddcb9eaeaea377667b8
     *     actual: 86f7e437faa5a7fce15d1ddcb9eaeaea377667b8
     *
     * "abc"
     *   expected: a9993e364706816aba3e25717850c26c9cd0d89d
     *     actual: a9993e364706816aba3e25717850c26c9cd0d89d
     *
     * "message digest"
     *   expected: c12252ceda8be8994d5fa0290a47231c1d16aae3
     *     actual: c12252ceda8be8994d5fa0290a47231c1d16aae3
     *
     * "abcdefghijklmnopqrstuvwxyz"
     *   expected: 32d10c7b8cf96570ca04ce37f2a19d84240d3a89
     *     actual: 32d10c7b8cf96570ca04ce37f2a19d84240d3a89
     *
     * "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
     *   expected: 761c457bf73b14d27e9e9265c46f4b4dda11f940
     *     actual: 761c457bf73b14d27e9e9265c46f4b4dda11f940
     *
     * "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
     *   expected: 50abf5706a150990a08b2c5ea40fa0e585554732
     *     actual: 50abf5706a150990a08b2c5ea40fa0e585554732
     *
     * "The quick brown fox jumps over the lazy dog"
     *   expected: 2fd4e1c67a2d28fced849ee1bb76e7391b93eb12
     *     actual: 2fd4e1c67a2d28fced849ee1bb76e7391b93eb12
     *
     * "The quick brown fox jumps over the lazy dog."
     *   expected: 408d94384216f890ff7a0c3528e8bed1e0b01621
     *     actual: 408d94384216f890ff7a0c3528e8bed1e0b01621
     */

        printf("%s\n", digest);

        /* clean up */
        PRINT("%s\n", "Cleaning up...");
        PRINT("0x%p\n", (void *) digest);
        free(digest);
        digest = NULL;

        rc = EXIT_SUCCESS;
    }
    else
    {
        printf("Usage: %s \"<string>\"\n", argv[0]);
        puts("  prints the SHA1 hash of <string>\n");
    }

    PRINT("Exiting with status %d\n", rc);
    return rc;
}
