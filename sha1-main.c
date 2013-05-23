#include "sha.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

extern void testSHA1(void);

int main(int argc, char **argv)
{
    int rc = EXIT_FAILURE;

    switch (get_option(argc, argv))
    {
        case Test:
            rc = do_test("SHA1 Test Suite", testSHA1);
            break;

        case String:
            rc = do_hash_string(argv[1], SHA1string, DIGEST_LENGTH);
            break;

        case File:
            rc = do_hash_file(argv[2], SHA1file, DIGEST_LENGTH);
            break;

        case Verify:
            rc = do_verify(argv[2], "SHA1", SHA1file, DIGEST_LENGTH);
            break;

        case MAC:
            rc = do_hmac(argv[2], argv[3], SHA1hmac, DIGEST_LENGTH);
            break;

        default:
            usage(*argv, "SHA1");
            break;
    }

    PRINT("Exiting with status %d\n", rc);
    return rc;
}
