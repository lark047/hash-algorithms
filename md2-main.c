#include "md.h"
#include "util.h"
#include "hmac.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

extern void testMD2(void);

int main(int argc, char **argv)
{
    int rc = EXIT_FAILURE;

    if (argc == 2 && (STR_EQ(argv[1], "-t") || STR_EQ(argv[1], "--test")))
    {
        rc = do_test("MD2 Test Suite", testMD2);
    }
    else if (argc == 2)
    {
        rc = do_hash_string(argv[1], MD2string, DIGEST_LENGTH);
    }
    else if (argc == 3 && (STR_EQ(argv[1], "-f") || STR_EQ(argv[1], "--file")))
    {
        rc = do_hash_file(argv[2], MD2file, DIGEST_LENGTH);
    }
    else if (argc == 4 && (STR_EQ(argv[1], "-h") || STR_EQ(argv[1], "--hmac")))
    {
        rc = do_hmac(argv[2], argv[3], MD2hmac, DIGEST_LENGTH);
    }
    else
    {
        printf("Usage: %s \"<message>\"\n", argv[0]);
        puts("  prints the MD2 hash of <message>\n");
        printf("Usage: %s -f <filename>\n", argv[0]);
        puts("  prints the MD2 hash of the file named <filename>\n");
        printf("Usage: %s -h \"<key>\" \"<message>\"\n", argv[0]);
        puts("  prints the MD2 HMAC hash of <message> using <key>\n");
    }

    PRINT("Exiting with status %d\n", rc);
    return rc;
}
