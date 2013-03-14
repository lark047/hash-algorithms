#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CUnit/Basic.h>

extern char *MD5string(const char *);
extern char *SHA1string(const char *);
extern char *SHA224string(const char *);
extern char *SHA256string(const char *);
extern char *SHA384string(const char *);
extern char *SHA512string(const char *);
extern char *SHA512224string(const char *);
/* extern char *SHA512256string(const char *); */

extern void testMD5(void);
extern void testSHA1(void);
extern void testSHA224(void);
extern void testSHA256(void);
extern void testSHA384(void);
extern void testSHA512(void);
/* extern void testSHA512224(void); */
/* extern void testSHA512256(void); */

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
                    CU_ADD_TEST(suite, testSHA256) == NULL ||
                    CU_ADD_TEST(suite, testSHA384) == NULL ||
                    CU_ADD_TEST(suite, testSHA512) == NULL /* ||
                    CU_ADD_TEST(suite, testSHA512224) == NULL /* ||
                    CU_ADD_TEST(suite, testSHA512256) == NULL */)
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
        const char *msg = argv[1];

        /* printf("Hashing \"%s\"\n", msg); */

        printf("       MD5: %s\n", MD5string(msg));
        printf("      SHA1: %s\n", SHA1string(msg));
        printf("    SHA224: %s\n", SHA224string(msg));
        printf("    SHA256: %s\n", SHA256string(msg));
        printf("    SHA384: %s\n", SHA384string(msg));
        printf("    SHA512: %s\n", SHA512string(msg));
        printf("SHA512/224: %s\n", SHA512224string(msg));
        /* printf("SHA512/256: %s\n", SHA512256string(msg)); */
    }

    PRINT("Exiting with status %d\n", rc);
    return rc;
}
