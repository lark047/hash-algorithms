#include "md.h"
#include "sha.h"
#include "util.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <CUnit/Basic.h>

extern void testMD2(void);
extern void testMD4(void);
extern void testMD5(void);
extern void testSHA1(void);
extern void testSHA224(void);
extern void testSHA256(void);
extern void testSHA384(void);
extern void testSHA512(void);
extern void testSHA512224(void);
extern void testSHA512256(void);

static short index_of(const char *, const char *[]);

static int do_tests(const char *, const char **, ...);
static int do_hash_strings(const char *, const char **, uint8_t *(**)(const char *), const uint8_t *);
static int do_hash_files(const char *, const char **, uint8_t *(**)(FILE *), const uint8_t *);
static int do_verifies(const char *, const char **, uint8_t *(**)(FILE *), const uint8_t *);
static int do_hmacs(const char *, const char *, const char **, uint8_t *(**)(const char *, const char *), const uint8_t *);

static void usage_main(const char *);

int main(int argc, char **argv)
{
    int rc = EXIT_FAILURE;

    const char *labels[] = {
        "MD2",
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

    const uint8_t digest_lengths[] = {
        16, 16, 16, 20, 28, 32, 48, 64, 28, 32
    };

    uint8_t *(*hash_strings[])(const char *) = {
        MD2string,
        MD4string,
        MD5string,
        SHA1string,
        SHA224string,
        SHA256string,
        SHA384string,
        SHA512string,
        SHA512224string,
        SHA512256string
    };

    uint8_t *(*hash_files[])(FILE *) = {
        /* MD2file */ NULL,
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

    uint8_t *(*hash_hmacs[])(const char *, const char *) = {
        MD2hmac,
        MD4hmac,
        MD5hmac,
        SHA1hmac,
        SHA224hmac,
        SHA256hmac,
        SHA384hmac,
        SHA512hmac,
        /* SHA512224hmac */ NULL,
        /* SHA512256hmac */ NULL
    };

    switch (get_option(argc, argv))
    {
        case Test:
        {
            const char *labels[] = {
                STRINGIFY(testMD2),
                STRINGIFY(testMD4),
                STRINGIFY(testMD5),
                STRINGIFY(testSHA1),
                STRINGIFY(testSHA224),
                STRINGIFY(testSHA256),
                STRINGIFY(testSHA384),
                STRINGIFY(testSHA512),
                STRINGIFY(testSHA512224),
                STRINGIFY(testSHA512256)
            };

            rc = do_tests("Cryptography Test Suite", labels, testMD2,    testMD4,    testMD5,    testSHA1,      testSHA224,
                                                             testSHA256, testSHA384, testSHA512, testSHA512224, testSHA512256,
                                                             NULL);
            break;
        }

        case String:
            rc = do_hash_strings(argv[1], labels, hash_strings, digest_lengths);
            break;

        case File:
            rc = do_hash_files(argv[2], labels, hash_files, digest_lengths);
            break;

        case Verify:
            rc = do_verifies(argv[2], labels, hash_files, digest_lengths);
            break;

        case MAC:
            rc = do_hmacs(argv[2], argv[3], labels, hash_hmacs, digest_lengths);
            break;

        default:
            usage_main(*argv);
            break;
    }

    if (argc == 4 && STR_EQ(argv[1], "--function"))
    {
        const short index = index_of(argv[2], labels);

        if (index == -1)
        {
            fprintf(stderr, "Hash function \"%s\" not supported\n", argv[2]);
        }
        else
        {
            uint8_t *digest = hash_strings[index](argv[3]);
            char *string = to_string(digest, digest_lengths[index]);

            /* TODO error handling */
            printf("%s\n", string);

            free(digest);
            free(string);

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

    PRINT("Exiting with status %d\n", rc);
    return rc;
}

static short index_of(const char *needle, const char *haystack[])
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

static int do_tests(const char *suite_name, const char **labels, ...)
{
    bool run = true;
    CU_pSuite suite;
    va_list args;

    if (CU_initialize_registry() == CUE_SUCCESS)
    {
        if ((suite = CU_add_suite(suite_name, NULL, NULL)))
        {
            va_start(args, labels);

            void (*test_function)(void);

            while (true)
            {
                test_function = va_arg(args, void (*)(void));

                if (test_function == NULL)
                {
                    break;
                }

                if (!CU_add_test(suite, *labels++, test_function))
                {
                    printf("[warn] could not add test \"%s\"\n", *labels);
                    break;
                }
            }

            va_end(args);

            if (run)
            {
                CU_basic_set_mode(CU_BRM_VERBOSE);
                CU_basic_run_tests();
            }
        }
        CU_cleanup_registry();
    }
    return run && CU_get_error();
}

static int do_hash_strings(const char *msg, const char **labels, uint8_t *(**hash)(const char *), const uint8_t *lengths)
{
    for (uint8_t i = 0; i < 10; ++i)
    {
        uint8_t *digest = hash[i](msg);
        char *string = to_string(digest, lengths[i]);

        printf("%11s: %s\n", labels[i], string);

        /* TODO error handling */
        free(digest);
        free(string);
    }

    return EXIT_SUCCESS;
}

static int do_hash_files(const char *filename, const char **labels, uint8_t *(**hash)(FILE *), const uint8_t *lengths)
{
    for (uint8_t i = 0; i < 10; ++i)
    {
        FILE *fp =        /* TODO */ hash[i] ? fopen(filename, "rb") : NULL;
        uint8_t *digest = /* TODO */ hash[i] ? hash[i](fp) : NULL;
        char *string =    /* TODO */ hash[i] ? to_string(digest, lengths[i]) : "(not yet implemented)";

        /* TODO error handling */
        printf("%11s: %s\n", labels[i], string);

        if (hash[i])
        {
            fclose(fp);
            free(digest);
            free(string);
        }
    }

    return EXIT_SUCCESS;
}

static int do_verifies(const char *filename, const char **labels, uint8_t *(**hash)(FILE *), const uint8_t *lengths)
{
    FILE *fp, *fhash;

    /* TODO check return */
    fp = fopen(filename, "rb");

    for (uint8_t i = 0; i < 10; ++i)
    {
        fhash = open(filename, labels[i]);

        if (fhash)
        {
            char verify[256];
            PRINT("hashing \"%s\" with %s...\n", filename, labels[i]);

            uint8_t *digest = /* TODO */ hash[i] ? hash[i](fp) : NULL;
            char *string =    /* TODO */ hash[i] ? to_string(digest, lengths[i]) : "(not yet implemented)";
            PRINT("%s(%s) = %s\n", labels[i], filename, string);

            fscanf(fhash, "%s", verify);
            PRINT("found verified hash: %s\n", verify);

            printf("%11s: verifying \"%s\"... ", labels[i], filename);
            fflush(stdout);

            if (STR_EQ(string, verify))
            {
                printf("OK\n");
            }
            else
            {
                printf("FAILED\n\n");
                printf("expected: %s\n", verify);
                printf("actual  : %s\n", string);
            }

            fseek(fp, 0, SEEK_SET);

            fclose(fhash);
            free(digest);
            free(string);

            memset(verify, 0, sizeof verify);
        }
    }

    if (fp)
    {
        fclose(fp);
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}

static int do_hmacs(const char *key, const char *msg, const char **labels, uint8_t *(**hash)(const char *, const char *), const uint8_t *lengths)
{
    for (uint8_t i = 0; i < 10; ++i)
    {
        uint8_t *digest = /* TODO */ hash[i] ? hash[i](key, msg) : NULL;
        char *string =    /* TODO */ hash[i] ? to_string(digest, lengths[i]) : "(not yet implemented)";

        /* TODO error handling */
        printf("%11s: %s\n", labels[i], string);

        if (hash[i])
        {
            free(digest);
            free(string);
        }
    }

    return EXIT_SUCCESS;
}

static void usage_main(const char *program)
{
    printf("Usage: %s -t|--test\n", program);
    printf("Usage: %s --file \"filename\"\n", program); /* TODO include --function */

#if 0
    printf("Usage: %s [--function function] \"message\"\n\n", program);
#endif

    printf("Usage: %s -h|--hmac \"key\" \"message\"\n", program); /* TODO include --function */
    printf("Usage: %s -v|--verify \"filename\"\n\n", program);
    printf("Hashes a given message or file with the specified hash function, if \n");
    printf("one is provided. If no hash function is specified, %s will use all\n", program);
    printf("available functions.\n\n");
    printf("    -t, --test\t\t\tRun the test suite.\n");
    printf("    --file filename\t\tHash the given file with all hash functions, unless\n");
    printf("                   \t\tone is provided with --function.\n");

#if 0
    printf("    --function hash\tHash the file/message with the specified hash\n");
    printf("                   \tfunction. Can be combined with --file.\n");
#endif

    printf("    --verify filename\t\tVerifies a file against the provided signature(s).\n");
    printf("    --h, --hmac\t\t\tCalculate the HMAC hash of <message> using <key>\n");
    printf("                   \t\tfor all hash functions, unless one is provided with\n");
    printf("                   \t\t--function.\n");
}
