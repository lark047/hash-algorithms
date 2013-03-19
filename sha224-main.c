#include "sha.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <CUnit/Basic.h>

extern void testSHA224(void);

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
                if ((suite = CU_add_suite("SHA224 Test Suite", NULL, NULL)) == NULL)
                {
                    CU_cleanup_registry();
                    rc = CU_get_error();
                }
                else
                {
                    if (CU_ADD_TEST(suite, testSHA224) == NULL)
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
            char * const msg = argv[1];

            PRINT("Calculating SHA224 for \"%s\"...\n", msg);
            PRINT("Using byte size of %u\n", (unsigned) CHAR_BIT);

            uint8_t *digest = SHA224string(msg);

            printf("%s\n", digest);

            /* clean up */
            PRINT("%s\n", "Cleaning up...");
            PRINT("0x%p\n", (void *) digest);
            free(digest);
            digest = NULL;

            rc = EXIT_SUCCESS;
        }
    }
    else if (argc == 3 && strcmp(argv[1], "-f") == 0)
    {
        char * const filename = argv[2];

        PRINT("Calculating SHA224 for \"%s\"...\n", filename);
        PRINT("Using byte size of %u\n", (unsigned) CHAR_BIT);

        FILE *fp = fopen(filename, "r");

        if (fp)
        {
            uint8_t *digest = SHA224file(fp);

            printf("%s\n", (char *) digest);

            /* clean up */
            PRINT("%s\n", "Cleaning up...");
            free(digest);
            fclose(fp);

            digest = NULL;
            fp = NULL;

            rc = EXIT_SUCCESS;
        }
        else
        {
            fprintf(stderr, "[ERROR] Could not open %s for reading.", filename);
        }
    }
    else
    {
        printf("Usage: %s \"<string>\"\n", argv[0]);
        puts("  prints the SHA224 hash of <string>\n");
        printf("Usage: %s -f <filename>\n", argv[0]);
        puts("  prints the SHA224 hash of the file named <filename>\n");
    }

    PRINT("Exiting with status %d\n", rc);
    return rc;
}
