#include "sha.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

extern void testSHA224(void);

int main(int argc, char **argv)
{
    int rc = EXIT_FAILURE;

    switch (get_option(argc, argv))
    {
        case Test:
            rc = do_test("SHA224 Test Suite", testSHA224);
            break;

        case String:
            rc = do_hash_string(argv[1], SHA224string, DIGEST_LENGTH);
            break;

        case File:
            rc = do_hash_file(argv[2], SHA224file, DIGEST_LENGTH);
            break;

        default:
            printf("Usage: %s \"<message>\"\n", argv[0]);
            puts("  prints the SHA224 hash of <message>\n");
            printf("Usage: %s -f <filename>\n", argv[0]);
            puts("  prints the SHA224 hash of the file named <filename>\n");
            break;
    }

    PRINT("Exiting with status %d\n", rc);
    return rc;
}
