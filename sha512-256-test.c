#include "sha.h"
#include "util.h"

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CUnit/Basic.h>

#if 0
static void testSHA512256file(void);
#endif

static void testSHA512256string(void);

/**
 * From: http://csrc.nist.gov/groups/ST/toolkit/documents/Examples/SHA512_256.pdf
 */
static char *test_msgs512256[] = {
    "abc",
    "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
    0
};

void testSHA512256(void)
{
#if 0
    testSHA512256file();
#endif
    testSHA512256string();
}

#if 0
void testSHA512256file(void)
{
    /* TODO i can't find a generator for SHA-512/256 */
    char *sha512256s[] = {
        "",
        ""
    };

    for (uint8_t i = 0; test_files[i]; ++i)
    {
        FILE *fp = fopen(test_files[i], "rb");

        CU_ASSERT_PTR_NOT_NULL_FATAL(fp)

        char *expected = sha512256s[i];
        char *actual = (char *) SHA512256file(fp);

        CU_ASSERT_PTR_NOT_NULL_FATAL(actual);
        CU_ASSERT_STRING_EQUAL(actual, expected);

        if (STR_EQ(expected, actual))
        {
            PRINT("%s\n", actual);
        }
        else
        {
            fprintf(stderr, "\n");
            fprintf(stderr, "file    : %s\n", test_files[i]);
            fprintf(stderr, "expected: %s\n", expected);
            fprintf(stderr, "actual  : %s\n", actual);
        }

        free(actual);
        fclose(fp);
    }
}
#endif

static void testSHA512256string(void)
{
    /* TODO The examples are from the official test vectors. */
    char *sha512256s[] = {
        "53048e2681941ef99b2e29b76b4c7dabe4c2d0c634fc6d46e0e2f13107e7af23",
        "3928e184fb8690f840da3988121d31be65cb9d3ef83ee6146feac861e19b563a",
        "",
        "",
        "",
        "",
        "",
        "",
        ""
    };

    for (uint8_t i = 0; test_msgs512256[i]; ++i)
    {
        uint8_t *digest = SHA512256string(test_msgs512256[i]);

        CU_ASSERT_PTR_NOT_NULL_FATAL(digest);

        char *expected = sha512256s[i];
        char *actual = to_string(digest, DIGEST_LENGTH);

        CU_ASSERT_PTR_NOT_NULL_FATAL(actual);
        CU_ASSERT_STRING_EQUAL(actual, expected);

        if (STR_EQ(expected, actual))
        {
            PRINT("%s\n", actual);
        }
        else
        {
            fprintf(stderr, "\n");
            fprintf(stderr, "string  : -->%s<--\n", test_msgs512256[i]);
            fprintf(stderr, "expected: %s\n", expected);
            fprintf(stderr, "actual  : %s\n", actual);
        }

        free(digest);
        free(actual);
    }
}
