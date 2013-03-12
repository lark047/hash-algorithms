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

        PRINT("Calculating SHA224 for \"%s\"...\n", string);
        PRINT("Using byte size of %u\n", (unsigned) CHAR_BIT);

        uint8_t *digest = SHAstring(string);

    /**
     * Results
     *
     * ""
     *   expected: d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f
     *     actual: d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f
     *
     * "a"
     *   expected: abd37534c7d9a2efb9465de931cd7055ffdb8879563ae98078d6d6d5
     *     actual: abd37534c7d9a2efb9465de931cd7055ffdb8879563ae98078d6d6d5
     *
     * "abc"
     *   expected: 23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7
     *     actual: 23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7
     *
     * "message digest"
     *   expected: 2cb21c83ae2f004de7e81c3c7019cbcb65b71ab656b22d6d0c39b8eb
     *     actual: 2cb21c83ae2f004de7e81c3c7019cbcb65b71ab656b22d6d0c39b8eb
     *
     * "abcdefghijklmnopqrstuvwxyz"
     *   expected: 45a5f72c39c5cff2522eb3429799e49e5f44b356ef926bcf390dccc2
     *     actual: 45a5f72c39c5cff2522eb3429799e49e5f44b356ef926bcf390dccc2
     *
     * "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
     *   expected: bff72b4fcb7d75e5632900ac5f90d219e05e97a7bde72e740db393d9
     *     actual: bff72b4fcb7d75e5632900ac5f90d219e05e97a7bde72e740db393d9
     *
     * "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
     *   expected: b50aecbe4e9bb0b57bc5f3ae760a8e01db24f203fb3cdcd13148046e
     *     actual: b50aecbe4e9bb0b57bc5f3ae760a8e01db24f203fb3cdcd13148046e
     *
     * "The quick brown fox jumps over the lazy dog"
     *   expected: 730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525
     *     actual: 730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525
     *
     * "The quick brown fox jumps over the lazy dog."
     *   expected: 619cba8e8e05826e9b8c519c0a5c68f4fb653e8a3d8aa04bb2c8cd4c
     *     actual: 619cba8e8e05826e9b8c519c0a5c68f4fb653e8a3d8aa04bb2c8cd4c
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
