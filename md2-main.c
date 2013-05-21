#include "md.h"
#include "util.h"
#include "hmac.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <CUnit/Basic.h>

extern void testMD2(void);

int main(int argc, char **argv)
{
    int rc = EXIT_FAILURE;

    if (argc == 2 && (STR_EQ(argv[1], "-t") || STR_EQ(argv[1], "--test")))
    {
        CU_pSuite suite;

        if (CU_initialize_registry() == CUE_SUCCESS)
        {
            if ((suite = CU_add_suite("MD2 Test Suite", NULL, NULL)))
            {
                if (CU_ADD_TEST(suite, testMD2))
                {
                    CU_basic_set_mode(CU_BRM_VERBOSE);
                    CU_basic_run_tests();
                }
            }
            CU_cleanup_registry();
        }
        rc = CU_get_error();
    }
    else if (argc == 2)
    {
        char * const msg = argv[1];

        PRINT("Calculating MD2 for \"%s\"...\n", msg);
        PRINT("Using digest length of %u\n", DIGEST_LENGTH);

        uint8_t *digest = MD2string(msg);
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
    else if (argc == 3 && (STR_EQ(argv[1], "-f") || STR_EQ(argv[1], "--file")))
    {
        char * const filename = argv[2];

        PRINT("Calculating MD2 for \"%s\"...\n", filename);
        PRINT("Using byte size of %u\n", (unsigned) CHAR_BIT);

        FILE *fp = fopen(filename, "rb");

        if (fp)
        {
            uint8_t *digest = MD2file(fp);
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
    else if (argc == 4 && (STR_EQ(argv[1], "-h") || STR_EQ(argv[1], "--hmac")))
    {
        uint8_t *digest = MD2hmac(argv[2], argv[3]);
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
