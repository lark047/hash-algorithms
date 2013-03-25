#include "md5.h"
#include "sha.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <CUnit/Basic.h>

extern void testMD5(void);
extern void testSHA1(void);
extern void testSHA224(void);
extern void testSHA256(void);
extern void testSHA384(void);
extern void testSHA512(void);
/* extern void testSHA512224(void); */
/* extern void testSHA512256(void); */

static size_t index_of(const char *, const char *[]);

int main(int argc, char **argv)
{
    int rc = EXIT_FAILURE;

    const char *labels[] = {
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

    if (argc == 2 && STR_EQ(argv[1], "-t"))
    {
        CU_pSuite suite;

        if (CU_initialize_registry() == CUE_SUCCESS)
        {
            if ((suite = CU_add_suite("Cryptography Test Suite", NULL, NULL)))
            {
                if (CU_ADD_TEST(suite, testMD5)
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
        uint8_t *(*hashes[])(const char *) = {
            MD5string,
            SHA1string,
            SHA224string,
            SHA256string,
            SHA384string,
            SHA512string,
#if 0
            SHA512224string,
            SHA512256string
#endif
        };

        for (uint8_t i = 0; i < SIZE(hashes); ++i)
        {
            uint8_t *digest = hashes[i](argv[1]);
            printf("%7s: %s\n", labels[i], (char *) digest);
            free(digest);
        }

        rc = EXIT_SUCCESS;
    }
    else if (argc == 3 && STR_EQ(argv[1], "--file"))
    {
        uint8_t *(*hashes[])(FILE *) = {
            MD5file,
            SHA1file,
            SHA224file,
            SHA256file,
            SHA384file,
            SHA512file,
            /* SHA512224file */ NULL,
            /* SHA512256file */ NULL
        };

        for (uint8_t i = 0; i < SIZE(hashes); ++i)
        {
            FILE *fp =        /* TODO */ hashes[i] ? fopen(argv[2], "rb") : NULL;
            uint8_t *digest = /* TODO */ hashes[i] ? hashes[i](fp) : (uint8_t *) "(not yet implemented)";

            /* TODO error handling */
            printf("%10s: %s\n", labels[i], (char *) digest);

            if (hashes[i]) {
                fclose(fp);
                free(digest);
            }
        }

        rc = EXIT_SUCCESS;
    }
    else if (argc == 4 && STR_EQ(argv[1], "--function"))
    {
        uint8_t *(*hash)(const char *);

        switch (index_of(argv[2], labels))
        {
            case 0:
                hash = MD5string;
                break;
            case 1:
                hash = SHA1string;
                break;
            case 2:
                hash = SHA224string;
                break;
            case 3:
                hash = SHA256string;
                break;
            case 4:
                hash = SHA384string;
                break;
            case 5:
                hash = SHA512string;
                break;
#if 0
            case 6:
                hash = SHA512224string;
                break;
            case 7:
                hash = SHA512256string;
                break;
#endif
            default:
                hash = NULL;
                break;
        }

        if (hash)
        {
            uint8_t *digest = hash(argv[3]);
            printf("%s\n", (char *) digest);
            free(digest);
            rc = EXIT_SUCCESS;
        }
        else
        {
            fprintf(stderr, "Hash function \"%s\" not supported\n", argv[2]);
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

        uint8_t *(*hash)(FILE *);

        switch (index_of(HASHNAME, labels))
        {
            case 0:
                hash = MD5file;
                break;
            case 1:
                hash = SHA1file;
                break;
            case 2:
                hash = SHA224file;
                break;
            case 3:
                hash = SHA256file;
                break;
            case 4:
                hash = SHA384file;
                break;
            case 5:
                hash = SHA512file;
                break;
#if 0
            case 6:
                hash = SHA512224file;
                break;
            case 7:
                hash = SHA512256file;
                break;
#endif
            default:
                hash = NULL;
                break;
        }

        if (hash)
        {
            FILE *fp = fopen(FILENAME, "rb");

            if (fp)
            {
                uint8_t *digest = hash(fp);
                printf("%s\n", (char *) digest);
                free(digest);
                rc = EXIT_SUCCESS;
            }
            else
            {
                fprintf(stderr, "Could not open \"%s\"\n", FILENAME);
            }
        }
        else
        {
            fprintf(stderr, "Hash function \"%s\" not supported\n", HASHNAME);
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

size_t index_of(const char *needle, const char *haystack[])
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
