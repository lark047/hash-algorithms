#include "md.h"
#include "sha.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <CUnit/Basic.h>

extern void testMD4(void);
extern void testMD5(void);
extern void testSHA1(void);
extern void testSHA224(void);
extern void testSHA256(void);
extern void testSHA384(void);
extern void testSHA512(void);

#if 0
extern void testSHA512224(void);
extern void testSHA512256(void);
#endif

static short index_of(const char *, const char *[]);

int main(int argc, char **argv)
{
    int rc = EXIT_FAILURE;

    const char *labels[] = {
        "MD4",
        "MD5",
        "SHA-1",
        "SHA-224",
        "SHA-256",
        "SHA-384",
        "SHA-512",
        "SHA-512/224",
        "SHA-512/256",
        0
    };

    uint8_t *(*hash_strings[])(const char *) = {
        MD4string,
        MD5string,
        SHA1string,
        SHA224string,
        SHA256string,
        SHA384string,
        SHA512string,
        /* SHA512224string */ NULL,
        /* SHA512256string */ NULL
    };

    uint8_t *(*hash_files[])(FILE *) = {
        MD4file,
        MD5file,
        SHA1file,
        SHA224file,
        SHA256file,
        SHA384file,
        SHA512file,
        /* SHA512224file */ NULL,
        /* SHA512256file */ NULL
    };

    if (argc == 2 && STR_EQ(argv[1], "-t"))
    {
        CU_pSuite suite;

        if (CU_initialize_registry() == CUE_SUCCESS)
        {
            if ((suite = CU_add_suite("Cryptography Test Suite", NULL, NULL)))
            {
                if (CU_ADD_TEST(suite, testMD4)
                 && CU_ADD_TEST(suite, testMD5)
                 && CU_ADD_TEST(suite, testSHA1)
                 && CU_ADD_TEST(suite, testSHA224)
                 && CU_ADD_TEST(suite, testSHA256)
                 && CU_ADD_TEST(suite, testSHA384)
                 && CU_ADD_TEST(suite, testSHA512)
#if 0
                 && CU_ADD_TEST(suite, testSHA512224)
                 && CU_ADD_TEST(suite, testSHA512256)
#endif
                 )
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
        for (uint8_t i = 0; i < SIZE(hash_strings); ++i)
        {
            uint8_t *digest = /* TODO */ hash_strings[i] ? hash_strings[i](argv[1]) : (uint8_t *) "(not yet implemented)";

            printf("%11s: %s\n", labels[i], (char *) digest);

            /* TODO error handling */
            if (hash_strings[i])
            {
                free(digest);
            }
        }

        rc = EXIT_SUCCESS;
    }
    else if (argc == 3 && STR_EQ(argv[1], "--file"))
    {
        for (uint8_t i = 0; i < SIZE(hash_files); ++i)
        {
            FILE *fp =        /* TODO */ hash_files[i] ? fopen(argv[2], "rb") : NULL;
            uint8_t *digest = /* TODO */ hash_files[i] ? hash_files[i](fp) : (uint8_t *) "(not yet implemented)";

            /* TODO error handling */
            printf("%11s: %s\n", labels[i], (char *) digest);

            if (hash_files[i]) {
                fclose(fp);
                free(digest);
            }
        }

        rc = EXIT_SUCCESS;
    }
    else if (argc == 4 && STR_EQ(argv[1], "--function"))
    {
        const short index = index_of(argv[2], labels);

        if (index == -1)
        {
            fprintf(stderr, "Hash function \"%s\" not supported\n", argv[2]);
        }
        else
        {
            uint8_t *digest = hash_strings[index](argv[3]);
            /* TODO error handling */
            printf("%s\n", (char *) digest);
            free(digest);
            rc = EXIT_SUCCESS;
        }
    }
    else if (argc == 5 &&
                ((STR_EQ(argv[1], "--function") && STR_EQ(argv[3], "--file")) ||
                 (STR_EQ(argv[3], "--function") && STR_EQ(argv[1], "--file"))))
    {
        const uint8_t findex = (STR_EQ(argv[1], "--file") ? 2 : 4);
        const uint8_t hindex = (STR_EQ(argv[1], "--file") ? 4 : 2);

#define HASHNAME argv[hindex]
#define FILENAME argv[findex]

        const short index = index_of(HASHNAME, labels);

        if (index == -1)
        {
            fprintf(stderr, "Hash function \"%s\" not supported\n", HASHNAME);
        }
        else
        {
            FILE *fp = fopen(FILENAME, "rb");

            if (fp)
            {
                uint8_t *digest = hash_files[index](fp);
                /* TODO error handling */
                printf("%s\n", (char *) digest);
                free(digest);
                rc = EXIT_SUCCESS;
            }
            else
            {
                fprintf(stderr, "Could not open \"%s\"\n", FILENAME);
            }
        }
#undef HASHNAME
#undef FILENAME
    }
    else
    {
        printf("Usage: %s [-t] [message] [--file filename] [--function hash-function]\n\n", *argv);
        printf("Hashes a given message or file with the specified hash function, if \n");
        printf("one is provided. If no hash function is specified, %s will use all\n", *argv);
        printf("available functions.\n\n");
        printf("    -t\t\t\tRun the test suite.\n");
        printf("    --file filename\tHash the given file with all hash functions,\n");
        printf("                   \tunless one is provided with --function.\n");
        printf("    --function hash\tHash the file/message with the specified hash\n");
        printf("                   \tfunction. Can be combined with --file.\n");
    }

    PRINT("Exiting with status %d\n", rc);
    return rc;
}

short index_of(const char *needle, const char *haystack[])
{
    for (size_t i = 0; haystack[i]; ++i)
    {
        if (strcasecmp(needle, haystack[i]) == 0)
        {
            return i;
        }
    }

    return -1;
}
