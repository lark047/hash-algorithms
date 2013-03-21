#include "sha.h"
#include "util.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <CUnit/Basic.h>

extern uint8_t *SHA512224file(FILE *);
extern uint8_t *SHA512224string(const char *);

extern const char *test_files[];
extern const char *test_msgs[];

static void testSHA512224file(void);
static void testSHA512224string(void);

void testSHA512224(void)
{
    testSHA512224file();
    testSHA512224string();
}

void testSHA512224file(void)
{
    /* TODO i can't find a generator */
    const char *sha512224s[] = {
        "",
        ""
    };

    for (uint8_t i = 0; test_files[i]; ++i)
    {
        FILE *fp = fopen(test_files[i], "rb");

        CU_ASSERT_PTR_NOT_NULL_FATAL(fp)

        const char *expected = sha512224s[i];
        const char *actual = (const char *) SHA512224file(fp);

        CU_ASSERT_STRING_EQUAL(actual, expected);

        if (strcmp(expected, actual))
        {
            fprintf(stderr, "\n");
            fprintf(stderr, "file    : %s\n", test_files[i]);
            fprintf(stderr, "expected: %s\n", expected);
            fprintf(stderr, "actual  : %s\n", actual);
        }
        else
        {
            PRINT("%s\n", actual);
        }

        fclose(fp);
    }
}

void testSHA512224string(void)
{
    /* TODO i can't find a generator */
    const char *sha512224s[] = {
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
        const char *expected = sha512224s[i];
        const char *actual = (const char *) SHA512224string(test_msgs[i]);

        CU_ASSERT_STRING_EQUAL(actual, expected);

        if (strcmp(expected, actual))
        {
            fprintf(stderr, "\n");
            fprintf(stderr, "string  : -->%s<--\n", test_msgs[i]);
            fprintf(stderr, "expected: %s\n", expected);
            fprintf(stderr, "actual  : %s\n", actual);
        }
        else
        {
            PRINT("%s\n", actual);
        }
    }
}
