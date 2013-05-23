#include "md.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

extern void testMD2(void);

int main(int argc, char **argv)
{
    int rc = EXIT_FAILURE;

    switch (get_option(argc, argv))
    {
        case Test:
            rc = do_test("MD2 Test Suite", testMD2);
            break;

        case String:
            rc = do_hash_string(argv[1], MD2string, DIGEST_LENGTH);
            break;

        case File:
            rc = do_hash_file(argv[2], MD2file, DIGEST_LENGTH);
            break;

        case Verify:
            rc = do_verify(argv[2], "MD2", MD2file, DIGEST_LENGTH);
            break;

        case MAC:
            rc = do_hmac(argv[2], argv[3], MD2hmac, DIGEST_LENGTH);
            break;

        default:
            usage(*argv, "MD2");
            break;
    }

    PRINT("Exiting with status %d\n", rc);
    return rc;
}
