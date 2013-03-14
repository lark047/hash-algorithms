#include "sha.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <CUnit/Basic.h>

extern void testSHA512256(void);

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
                if ((suite = CU_add_suite("SHA512/256 Test Suite", NULL, NULL)) == NULL)
                {
                    CU_cleanup_registry();
                    rc = CU_get_error();
                }
                else
                {
                    if (CU_ADD_TEST(suite, testSHA512256) == NULL)
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

            PRINT("Calculating SHA512/256 for \"%s\"...\n", string);
            PRINT("Using byte size of %u\n", (unsigned) CHAR_BIT);

            uint8_t *digest = SHA512256string(string);

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
        puts("  prints the SHA512/256 hash of <string>\n");
    }

    PRINT("Exiting with status %d\n", rc);
    return rc;
}
