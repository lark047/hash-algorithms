#include "sha.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <CUnit/Basic.h>

static void testSHA1(void)
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

    const char *sha1s[] = {
        "da39a3ee5e6b4b0d3255bfef95601890afd80709",
        "86f7e437faa5a7fce15d1ddcb9eaeaea377667b8",
        "a9993e364706816aba3e25717850c26c9cd0d89d",
        "c12252ceda8be8994d5fa0290a47231c1d16aae3",
        "32d10c7b8cf96570ca04ce37f2a19d84240d3a89",
        "761c457bf73b14d27e9e9265c46f4b4dda11f940",
        "50abf5706a150990a08b2c5ea40fa0e585554732",
        "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12",
        "408d94384216f890ff7a0c3528e8bed1e0b01621"
    };
    const char *actual, *expected;

    for (uint8_t i = 0; msgs[i]; ++i)
    {
        expected = sha1s[i];
        actual = (const char *) SHAstring(msgs[i]);

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
                if ((suite = CU_add_suite("SHA1 Test Suite", NULL, NULL)) == NULL)
                {
                    CU_cleanup_registry();
                    rc = CU_get_error();
                }
                else
                {
                    if (CU_ADD_TEST(suite, testSHA1) == NULL)
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

            PRINT("Calculating SHA1 for \"%s\"...\n", string);
            PRINT("Using byte size of %u\n", (unsigned) CHAR_BIT);

            uint8_t *digest = SHAstring(string);

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
        puts("  prints the SHA1 hash of <string>\n");
    }

    PRINT("Exiting with status %d\n", rc);
    return rc;
}
