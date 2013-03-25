#include "md.h"
#include "util.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <CUnit/Basic.h>

extern uint8_t *MD4file(FILE *);
extern uint8_t *MD4string(const char *);

extern const char *test_files[];
extern const char *test_msgs[];

static void testMD4file(void);
static void testMD4string(void);

void testMD4(void)
{
#if 0
    testMD4file();
#endif
    testMD4string();
}

void testMD4file(void)
{
    const char *md4s[] = {
        "",
        ""
    };

    for (uint8_t i = 0; test_files[i]; ++i)
    {
        FILE *fp = fopen(test_files[i], "rb");

        CU_ASSERT_PTR_NOT_NULL_FATAL(fp)

        const char *expected = md4s[i];
        const char *actual = (const char *) MD4file(fp);

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

void testMD4string(void)
{
    const char *md4s[] = {
        "31d6cfe0d16ae931b73c59d7e0c089c0",
        "bde52cb31de33e46245e05fbdbd6fb24",
        "a448017aaf21d8525fc10ae87aa6729d",
        "d9130a8164549fe818874806e1c7014b",
        "d79e1c308aa5bbcdeea8ed63df412da9",
        "043f8582f241db351ce627e153e7f0e4",
        "e33b4ddc9c38f2199c3e7b164fcc0536",
        "1bee69a46ba811185c194762abaeae90",
        "2812c6c7136898c51f6f6739ad08750e"
    };

    for (uint8_t i = 0; test_msgs[i]; ++i)
    {
        const char *expected = md4s[i];
        const char *actual = (const char *) MD4string(test_msgs[i]);

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
