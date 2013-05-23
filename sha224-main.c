#include "sha.h"
#include "util.h"

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

        case MAC:
            rc = do_hmac(argv[2], argv[3], SHA224hmac, DIGEST_LENGTH);
            break;

        default:
            usage(*argv, "SHA224");
            break;
    }

    PRINT("Exiting with status %d\n", rc);
    return rc;
}
