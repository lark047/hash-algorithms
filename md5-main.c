#include "md.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

extern void testMD5(void);

int main(int argc, char **argv)
{
    int rc = EXIT_FAILURE;

    if (argc == 2 && (STR_EQ(argv[1], "-t") || STR_EQ(argv[1], "--test")))
    {
        rc = do_test("MD5 Test Suite", testMD5);
    }
    else if (argc == 2)
    {
        rc = do_hash_string(argv[1], MD5string, DIGEST_LENGTH);
    }
    else if (argc == 3 && (STR_EQ(argv[1], "-f") || STR_EQ(argv[1], "--file")))
    {
        rc = do_hash_file(argv[2], MD5file, DIGEST_LENGTH);
    }
    else if (argc == 4 && (STR_EQ(argv[1], "-h") || STR_EQ(argv[1], "--hmac")))
    {
        rc = do_hmac(argv[2], argv[3], MD5hmac, DIGEST_LENGTH);
    }
    else
    {
        usage(*argv, "MD5");
    }

    PRINT("Exiting with status %d\n", rc);
    return rc;
}
