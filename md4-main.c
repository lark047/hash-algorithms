#include "md.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

extern void testMD4(void);

int main(int argc, char **argv)
{
    int rc = EXIT_FAILURE;

    switch (get_option(argc, argv))
    {
        case Test:
            rc = do_test(LABEL " Test Suite", "test" LABEL, testMD4);
            break;

        case String:
            rc = do_hash_string(argv[1], MD4string, DIGEST_LENGTH);
            break;

        case File:
            rc = do_hash_file(argv[2], MD4file, DIGEST_LENGTH);
            break;

        case Verify:
            rc = do_verify(argv[2], LABEL, MD4file, DIGEST_LENGTH);
            break;

        case MAC:
            rc = do_hmac(argv[2], argv[3], MD4hmac, DIGEST_LENGTH);
            break;

        default:
            usage(*argv, LABEL);
            break;
    }

    PRINT("Exiting with status %d\n", rc);
    return rc;
}
