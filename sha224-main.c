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
        if (STR_EQ(argv[1], "-t"))
        {
            CU_pSuite suite;

            if (CU_initialize_registry() == CUE_SUCCESS)
            {
                if ((suite = CU_add_suite("SHA224 Test Suite", NULL, NULL)))
                {
                    if (CU_ADD_TEST(suite, testSHA224))
                    {
                        CU_basic_set_mode(CU_BRM_VERBOSE);
                        CU_basic_run_tests();
                    }
                }
                CU_cleanup_registry();
            }
            rc = CU_get_error();
        }
        else
        {
            char * const msg = argv[1];

            PRINT("Calculating SHA224 for \"%s\"...\n", msg);
            PRINT("Using digest length of %u\n", DIGEST_LENGTH);

            uint8_t *digest = SHA224string(msg);
            char *buf = to_string(digest, DIGEST_LENGTH);

            printf("%s\n", buf);

            /* clean up */
            PRINT("%s\n", "Cleaning up...");
            free(digest);
            free(buf);

            digest = NULL;
            buf = NULL;

            rc = EXIT_SUCCESS;
        }
    }
    else if (argc == 3 && STR_EQ(argv[1], "-f"))
    {
        char * const filename = argv[2];

        PRINT("Calculating SHA224 for \"%s\"...\n", filename);
        PRINT("Using byte size of %u\n", (unsigned) CHAR_BIT);

        FILE *fp = fopen(filename, "rb");

        if (fp)
        {
            uint8_t *digest = SHA224file(fp);
            char *buf = to_string(digest, DIGEST_LENGTH);

            printf("%s\n", buf);

            /* clean up */
            PRINT("%s\n", "Cleaning up...");
            free(digest);
            free(buf);
            fclose(fp);

            digest = NULL;
            buf = NULL;
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
        printf("Usage: %s \"<message>\"\n", argv[0]);
        puts("  prints the SHA224 hash of <message>\n");
        printf("Usage: %s -f <filename>\n", argv[0]);
        puts("  prints the SHA224 hash of the file named <filename>\n");
    }

    PRINT("Exiting with status %d\n", rc);
    return rc;
}
