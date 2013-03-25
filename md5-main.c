#include "md.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <CUnit/Basic.h>

extern void testMD5(void);

int main(int argc, char **argv)
{
    int rc = EXIT_FAILURE;

    if (argc == 2)
    {
        if (strcmp(argv[1], "-t") == 0)
        {
            CU_pSuite suite;

            if (CU_initialize_registry() == CUE_SUCCESS)
            {
                if ((suite = CU_add_suite("MD5 Test Suite", NULL, NULL)))
                {
                    if (CU_ADD_TEST(suite, testMD5))
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

            PRINT("Calculating MD5 for \"%s\"...\n", msg);
            PRINT("Using byte size of %u\n", (unsigned) CHAR_BIT);

            uint8_t *digest = MD5string(msg);

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
    else if (argc == 3 && strcmp(argv[1], "-f") == 0)
    {
        char * const filename = argv[2];

        PRINT("Calculating MD5 for \"%s\"...\n", filename);
        PRINT("Using byte size of %u\n", (unsigned) CHAR_BIT);

        FILE *fp = fopen(filename, "rb");

        if (fp)
        {
            uint8_t *digest = MD5file(fp);

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
        puts("  prints the MD5 hash of <string>\n");
        printf("Usage: %s -f <filename>\n", argv[0]);
        puts("  prints the MD5 hash of the file named <filename>\n");
    }

    PRINT("Exiting with status %d\n", rc);
    return rc;
}
