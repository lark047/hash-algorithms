#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CUnit/Basic.h>

extern uint8_t *MD5string(const char *);
extern uint8_t *SHA1string(const char *);
extern uint8_t *SHA224string(const char *);
extern uint8_t *SHA256string(const char *);
extern uint8_t *SHA384string(const char *);
extern uint8_t *SHA512string(const char *);
extern uint8_t *SHA512224string(const char *);
extern uint8_t *SHA512256string(const char *);

extern uint8_t *MD5file(FILE *);
extern uint8_t *SHA1file(FILE *);
/* extern uint8_t *SHA224file(FILE *); */
extern uint8_t *SHA256file(FILE *);
extern uint8_t *SHA384file(FILE *);
extern uint8_t *SHA512file(FILE *);
/* ... */

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

    const char *labels[] = {
        "MD5",
        "SHA1",
        "SHA224",
        "SHA256",
        "SHA384",
        "SHA512",
        "SHA512/224",
        "SHA512/256"
    };

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
        uint8_t *(*hashes[])(const char *) = {
            MD5string,
            SHA1string,
            SHA224string,
            SHA256string,
            SHA384string,
            SHA512string,
            SHA512224string,
            SHA512256string
        };

        /* printf("Hashing \"%s\"\n", msg); */

        for (uint8_t i = 0; i < SIZE(hashes); ++i)
        {
            uint8_t *digest = hashes[i](argv[1]);

            printf("%10s: %s\n", labels[i], (char *) digest);

            free(digest);
        }
    }
    else if (argc == 3 && strcmp(argv[1], "-f") == 0)
    {
        uint8_t *(*hashes[])(FILE *) = {
            MD5file,
            SHA1file,
            /* SHA224file, */ NULL,
            SHA256file,
            SHA384file,
            SHA512file,
            /* SHA512224file */ NULL,
            /* SHA512256file */ NULL,
        };

        for (uint8_t i = 0; i < SIZE(hashes); ++i)
        {
            FILE *fp =        /* TODO */ hashes[i] ? fopen(argv[2], "r") : NULL;
            uint8_t *digest = /* TODO */ hashes[i] ? hashes[i](fp) : (uint8_t *) "(not yet implemented)";

            /* TODO error handling */
            printf("%10s: %s\n", labels[i], (char *) digest);

            if (hashes[i]) {
                fclose(fp);
                free(digest);
            }
        }
    }
    else
    {
        /* TODO usage */
    }

    PRINT("Exiting with status %d\n", rc);
    return rc;
}
