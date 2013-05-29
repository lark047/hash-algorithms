#include "sha.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

extern void testSHA512256(void);

int main(int argc, char **argv)
{
    int rc = EXIT_FAILURE;

    switch (get_option(argc, argv))
    {
        case Test:
            rc = do_test(LABEL " Test Suite", "test" LABEL, testSHA512256);
            break;

        case String:
            rc = do_hash_string(argv[1], SHA512256string, DIGEST_LENGTH);
            break;

#if 0
        case File:
            rc = do_hash_file(argv[2], SHA512256file, DIGEST_LENGTH);
            break;

        case Verify:
            rc = do_verify(argv[2], LABEL, SHA512256file, DIGEST_LENGTH);
            break;

        case MAC:
            rc = do_hmac(argv[2], argv[3], SHA512256hmac, DIGEST_LENGTH);
            break;
#endif

        default:
            usage(*argv, LABEL);
            break;
    }

    // printf("SHA-512/256 is not yet implemented. No suitable generator could be found.\n");

    PRINT("Exiting with status %d\n", rc);
    return rc;
}
