#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CUnit/Basic.h>

extern char *MD5string(const char *);
extern char *SHA1String(const char *);
extern char *SHA224string(const char *);
extern char *SHA256string(const char *);
/* ... */

extern void testMD5(void);
extern void testSHA1(void);
extern void testSHA224(void);
extern void testSHA256(void);
/* ... */

int main(int argc, char **argv)
{
    int rc = EXIT_FAILURE;

    if (argc == 2 && strcmp(argv[1], "-t") == 0)
    {
        /* run the test suite */
        CU_pSuite suite;

        if (CU_initialize_registry() != CUE_SUCCESS)
        {
            rc = CU_get_error();
        }
        else
        {
            if ((suite = CU_add_suite("Cryptography Test Suite", NULL, NULL)) == NULL)
            {
                CU_cleanup_registry();
                rc = CU_get_error();
            }
            else
            {
                if (CU_ADD_TEST(suite, testMD5) == NULL ||
                    CU_ADD_TEST(suite, testSHA1) == NULL ||
                    CU_ADD_TEST(suite, testSHA224) == NULL ||
                    CU_ADD_TEST(suite, testSHA256) == NULL)
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
    else if (argc == 2)
    {
        /* TODO */
    }

    PRINT("Exiting with status %d\n", rc);
    return rc;
}
