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

    /* 0:  */
    /*     */

    /**
     * Results
     *
     * ""
     *   expected: e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
     *     actual: e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
     *
     * "a"
     *   expected: ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb
     *     actual: ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb
     *
     * "abc"
     *   expected: ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
     *     actual: ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
     *
     * "message digest"
     *   expected: f7846f55cf23e14eebeab5b4e1550cad5b509e3348fbc4efa3a1413d393cb650
     *     actual: f7846f55cf23e14eebeab5b4e1550cad5b509e3348fbc4efa3a1413d393cb650
     *
     * "abcdefghijklmnopqrstuvwxyz"
     *   expected: 71c480df93d6ae2f1efad1447c66c9525e316218cf51fc8d9ed832f2daf18b73
     *     actual: 71c480df93d6ae2f1efad1447c66c9525e316218cf51fc8d9ed832f2daf18b73
     *
     * "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
     *   expected: db4bfcbd4da0cd85a60c3c37d3fbd8805c77f15fc6b1fdfe614ee0a7c8fdb4c0
     *     actual: db4bfcbd4da0cd85a60c3c37d3fbd8805c77f15fc6b1fdfe614ee0a7c8fdb4c0
     *
     * "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
     *   expected: f371bc4a311f2b009eef952dd83ca80e2b60026c8e935592d0f9c308453c813e
     *     actual: f371bc4a311f2b009eef952dd83ca80e2b60026c8e935592d0f9c308453c813e
     *
     * "The quick brown fox jumps over the lazy dog"
     *   expected: d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592
     *     actual: d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592
     *
     * "The quick brown fox jumps over the lazy dog."
     *   expected: ef537f25c895bfa782526529a9b63d97aa631564d5d789c2b765448c8635fb6c
     *     actual: ef537f25c895bfa782526529a9b63d97aa631564d5d789c2b765448c8635fb6c
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
