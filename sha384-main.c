#include "sha.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

extern void testSHA384(void);

int main(int argc, char **argv)
{
    int rc = EXIT_FAILURE;

    switch (get_option(argc, argv))
    {
        case Test:
            rc = do_test("SHA384 Test Suite", testSHA384);
            break;

        case String:
            rc = do_hash_string(argv[1], SHA384string, DIGEST_LENGTH);
            break;

        case File:
            rc = do_hash_file(argv[2], SHA384file, DIGEST_LENGTH);
            break;

        case Verify:
            rc = do_verify(argv[2], "SHA384", SHA384file, DIGEST_LENGTH);
            break;

        case MAC:
            rc = do_hmac(argv[2], argv[3], SHA384hmac, DIGEST_LENGTH);
            break;

        default:
            usage(*argv, "SHA384");
            break;
    }

    PRINT("Exiting with status %d\n", rc);
    return rc;
}
