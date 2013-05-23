#include "md.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

extern void testMD5(void);

int main(int argc, char **argv)
{
    int rc = EXIT_FAILURE;

    switch (get_option(argc, argv))
    {
        case Test:
            rc = do_test("MD5 Test Suite", testMD5);
            break;

        case String:
            rc = do_hash_string(argv[1], MD5string, DIGEST_LENGTH);
            break;

        case File:
            rc = do_hash_file(argv[2], MD5file, DIGEST_LENGTH);
            break;

        case Verify:
            rc = do_verify(argv[2], "MD5", MD5file, DIGEST_LENGTH);
            break;

        case MAC:
            rc = do_hmac(argv[2], argv[3], MD5hmac, DIGEST_LENGTH);
            break;

        default:
            usage(*argv, "MD5");
            break;
    }

    PRINT("Exiting with status %d\n", rc);
    return rc;
}
