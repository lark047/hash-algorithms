#include "sha.h"
#include "util.h"
#include "hmac.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <CUnit/Basic.h>

extern void testSHA512(void);

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
                if ((suite = CU_add_suite("SHA512 Test Suite", NULL, NULL)))
                {
                    if (CU_ADD_TEST(suite, testSHA512))
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

            PRINT("Calculating SHA512 for \"%s\"...\n", msg);
            PRINT("Using byte size of %u\n", (unsigned) CHAR_BIT);

            uint8_t *digest = SHA512string(msg);

            printf("%s\n", (char *) digest);

            /* clean up */
            PRINT("%s\n", "Cleaning up...");
            PRINT("0x%p\n", (void *) digest);
            free(digest);
            digest = NULL;

            rc = EXIT_SUCCESS;
        }
    }
    else if (argc == 3 && STR_EQ(argv[1], "-f"))
    {
        char * const filename = argv[2];

        PRINT("Calculating SHA512 for \"%s\"...\n", filename);
        PRINT("Using byte size of %u\n", (unsigned) CHAR_BIT);

        FILE *fp = fopen(filename, "rb");

        if (fp)
        {
            uint8_t *digest = SHA512file(fp);

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
    else if (argc == 4 && STR_EQ(argv[1], "-h"))
    {
        uint8_t *digest = HMAC_SHA512(argv[2], argv[3]);

        /* TODO return the actual bytes and add function for printing */
        printf("%s\n", (char *) digest);

        /* clean up */
        free(digest);
        digest = NULL;

        rc = EXIT_SUCCESS;
    }
    else
    {
        printf("Usage: %s \"<message>\"\n", argv[0]);
        puts("  prints the SHA512 hash of <message>\n");
        printf("Usage: %s -f <filename>\n", argv[0]);
        puts("  prints the SHA512 hash of the file named <filename>\n");
        printf("Usage: %s -h \"<key>\" \"<message>\"\n", argv[0]);
        puts("  prints the SHA512 HMAC hash of <message> using <key>\n");
    }

    PRINT("Exiting with status %d\n", rc);
    return rc;
}
