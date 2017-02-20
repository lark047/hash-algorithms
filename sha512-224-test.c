#include "sha.h"
#include "util.h"

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CUnit/Basic.h>

#if 0
static void testSHA512224file(void);
#endif

static void testSHA512224string(void);

/**
 * From: http://csrc.nist.gov/groups/ST/toolkit/documents/Examples/SHA512_224.pdf
 */
static char *test_msgs512224[] = {
    "abc",
    "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
    0
};

void testSHA512224(void)
{
#if 0
    testSHA512224file();
#endif
    testSHA512224string();
}

#if 0
void testSHA512224file(void)
{
    /* TODO i can't find a generator for SHA-512/224. */
    char *sha512224s[] = {
        "",
        ""
    };

    for (uint8_t i = 0; test_files[i]; ++i)
    {
        FILE *fp = fopen(test_files[i], "rb");

        CU_ASSERT_PTR_NOT_NULL_FATAL(fp)

        char *expected = sha512224s[i];
        char *actual = (char *) SHA512224file(fp);

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

        free((void *) actual);
        fclose(fp);
    }
}
#endif

static void testSHA512224string(void)
{
    /* TODO The examples are from the official test vectors. */
    char *sha512224s[] = {
        "4634270f707b6a54daae7530460842e20e37ed265ceee9a43e8924aa",
        "23fec5bb94d60b23308192640b0c453335d664734fe40e7268674af9",
        "",
        "",
        "",
        "",
        "",
        "",
        ""
    };

    for (uint8_t i = 0; test_msgs512224[i]; ++i)
    {
        uint8_t *digest = SHA512224string(test_msgs512224[i]);

        CU_ASSERT_PTR_NOT_NULL_FATAL(digest);

        char *expected = sha512224s[i];
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
            fprintf(stderr, "string  : -->%s<--\n", test_msgs512224[i]);
            fprintf(stderr, "expected: %s\n", expected);
            fprintf(stderr, "actual  : %s\n", actual);
        }

        free(digest);
        free(actual);
    }
}
