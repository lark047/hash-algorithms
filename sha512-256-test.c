#include "sha.h"
#include "util.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CUnit/Basic.h>

#if 0
static void testSHA512256file(void);
static void testSHA512256string(void);
#endif

void testSHA512256(void)
{
#if 0
    testSHA512256file();
    testSHA512256string();
#endif
}

#if 0
void testSHA512256file(void)
{
    /* TODO i can't find a generator */
    const char *sha512256s[] = {
        "",
        ""
    };

    for (uint8_t i = 0; test_files[i]; ++i)
    {
        FILE *fp = fopen(test_files[i], "rb");

        CU_ASSERT_PTR_NOT_NULL_FATAL(fp)

        const char *expected = sha512256s[i];
        const char *actual = (const char *) SHA512256file(fp);

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

void testSHA512256string(void)
{
    /* TODO i can't find a generator */
    const char *sha512256s[] = {
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        ""
    };

    for (uint8_t i = 0; test_msgs[i]; ++i)
    {
        const char *expected = sha512256s[i];
        const char *actual = (const char *) SHA512256string(test_msgs[i]);

        CU_ASSERT_PTR_NOT_NULL_FATAL(actual);
        CU_ASSERT_STRING_EQUAL(actual, expected);

        if (STR_EQ(expected, actual))
        {
            PRINT("%s\n", actual);
        }
        else
        {
            fprintf(stderr, "\n");
            fprintf(stderr, "string  : -->%s<--\n", test_msgs[i]);
            fprintf(stderr, "expected: %s\n", expected);
            fprintf(stderr, "actual  : %s\n", actual);
        }

        free((void *) actual);
    }
}

#endif
