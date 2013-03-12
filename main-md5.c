#include "md5.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <CUnit/Basic.h>

static void testMD5(void)
{
    const char *msgs[] = {
        "",
        "a",
        "abc",
        "message digest",
        "abcdefghijklmnopqrstuvwxyz",
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
        "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
        "The quick brown fox jumps over the lazy dog",
        "The quick brown fox jumps over the lazy dog.",
        0
    };

    const char *md5s[] = {
        "d41d8cd98f00b204e9800998ecf8427e",
        "0cc175b9c0f1b6a831c399e269772661",
        "900150983cd24fb0d6963f7d28e17f72",
        "f96b697d7cb7938d525a2f31aaf161d0",
        "c3fcd3d76192e4007dfb496cca67e13b",
        "d174ab98d277d9f5a5611c2c9f419d9f",
        "57edf4a22be3c955ac49da2e2107b67a",
        "9e107d9d372bb6826bd81d3542a419d6",
        "e4d909c290d0fb1ca068ffaddf22cbd0"
    };
    const char *actual, *expected;

    for (uint8_t i = 0; msgs[i]; ++i)
    {
        expected = md5s[i];
        actual = (const char *) MD5string(msgs[i]);

        CU_ASSERT_STRING_EQUAL(actual, expected);

        if (strcmp(expected, actual))
        {
            fprintf(stderr, "\n");
            fprintf(stderr, "%s\n", msgs[i]);
            fprintf(stderr, "expected: %s\n", expected);
            fprintf(stderr, "actual  : %s\n", actual);
        }
    }
}

int main(int argc, char **argv)
{
    int rc = EXIT_FAILURE;

    if (argc == 2)
    {
        if (strcmp(argv[1], "-t") == 0)
        {
            CU_pSuite suite;

            if (CU_initialize_registry() != CUE_SUCCESS)
            {
                rc = CU_get_error();
            }
            else
            {
                if ((suite = CU_add_suite("SHA56 Test Suite", NULL, NULL)) == NULL)
                {
                    CU_cleanup_registry();
                    rc = CU_get_error();
                }
                else
                {
                    if (CU_ADD_TEST(suite, testMD5) == NULL)
                    {
                        CU_cleanup_registry();
                        rc = CU_get_error();
                    }
                    else
                    {
                        CU_basic_set_mode(CU_BRM_VERBOSE);
                        CU_basic_run_tests();
                        CU_cleanup_registry();

                        rc = CU_get_error();
                    }
                }
            }
        }
        else
        {
            char * const string = argv[1];

            PRINT("Calculating MD5 for \"%s\"...\n", string);
            PRINT("Using byte size of %u\n", (unsigned) CHAR_BIT);

            uint8_t *digest = MD5string(string);

    /* 0: 11b3d4b6bbf79f541eb76a7f55f2fd3f */
    /* 1: 3cf198ac802190f62e0036ecf545ee79 */
    /* 2: 661cef4763c313be6cc59c6ca1d7b0f4 */
    /* 3: 2d72b44b8b48a01ce13ec54837e9227d */
    /* 4: 004bb4008b48a01ce13ec54837e9227d */
    /* 5: b44b2d72a01c8b480048c500007d2200 */
    /* 6: b44b2d72a01c8b48c548e13e227d37e9 */
    /* 7: 4bb42b721ca08a4848c5ec3e7d2292e9 */
    /* 8: 4bb4722d1ca0488b48c53ee17d22e937 */
    /* 9: 077589fe7aca3c60ee10c19689066637 */
    /* a: 3b9660e801c5380a0a4a6a7a7f2bc1b6 */
    /* b: 43161baecdeead902ebcad6889ba88f2 */
    /* c: 9619e65a1a58f039eef883449af57b34 */
    /* d: 624dbb202abda69f8cbceac0ee363419 */
    /* e: 9e107d9d372bb6826bd81d3542a419d6 */
    /*    9e107d9d372bb6826bd81d3542a419d6 */


            printf("%s\n", digest);

            /* clean up */
            PRINT("%s\n", "Cleaning up...");
            PRINT("0x%p\n", (void *) digest);
            free(digest);
            digest = NULL;

            rc = EXIT_SUCCESS;
        }
    }
    else
    {
        printf("Usage: %s \"<string>\"\n", argv[0]);
        puts("  prints the MD5 hash of <string>\n");
    }

    PRINT("Exiting with status %d\n", rc);
    return rc;
}
