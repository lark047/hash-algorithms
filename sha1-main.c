#include "sha.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

extern void testSHA1(void);

int main(int argc, char **argv)
{
    int rc = EXIT_FAILURE;

    if (argc == 2 && (STR_EQ(argv[1], "-t") || STR_EQ(argv[1], "--test")))
    {
        rc = do_test("SHA1 Test Suite", testSHA1);
    }
    else if (argc == 2)
    {
        rc = do_hash_string(argv[1], SHA1string, DIGEST_LENGTH);
    }
    else if (argc == 3 && (STR_EQ(argv[1], "-f") || STR_EQ(argv[1], "--file")))
    {
        rc = do_hash_file(argv[2], SHA1file, DIGEST_LENGTH);
    }
    else if (argc == 4 && (STR_EQ(argv[1], "-h") || STR_EQ(argv[1], "--hmac")))
    {
        rc = do_hmac(argv[2], argv[3], SHA1hmac, DIGEST_LENGTH);
    }
    else
    {
        usage(*argv, "SHA1");
    }

    PRINT("Exiting with status %d\n", rc);
    return rc;
}
