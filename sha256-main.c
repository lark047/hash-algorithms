#include "sha.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

extern void testSHA256(void);

int main(int argc, char **argv)
{
    int rc = EXIT_FAILURE;

    switch (get_option(argc, argv))
    {
        case Test:
            rc = do_test("SHA256 Test Suite", testSHA256);
            break;

        case String:
            rc = do_hash_string(argv[1], SHA256string, DIGEST_LENGTH);
            break;

        case File:
            rc = do_hash_file(argv[2], SHA256file, DIGEST_LENGTH);
            break;

        case Verify:
            rc = do_verify(argv[2], "SHA256", SHA256file, DIGEST_LENGTH);
            break;

        case MAC:
            rc = do_hmac(argv[2], argv[3], SHA256hmac, DIGEST_LENGTH);
            break;

        default:
            usage(*argv, "SHA256");
            break;
    }

    PRINT("Exiting with status %d\n", rc);
    return rc;
}
