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
        "d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f",
        "abd37534c7d9a2efb9465de931cd7055ffdb8879563ae98078d6d6d5",
        "23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7",
        "2cb21c83ae2f004de7e81c3c7019cbcb65b71ab656b22d6d0c39b8eb",
        "45a5f72c39c5cff2522eb3429799e49e5f44b356ef926bcf390dccc2",
        "bff72b4fcb7d75e5632900ac5f90d219e05e97a7bde72e740db393d9",
        "b50aecbe4e9bb0b57bc5f3ae760a8e01db24f203fb3cdcd13148046e",
        "730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525",
        "619cba8e8e05826e9b8c519c0a5c68f4fb653e8a3d8aa04bb2c8cd4c"
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

            PRINT("Calculating SHA224 for \"%s\"...\n", string);
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
