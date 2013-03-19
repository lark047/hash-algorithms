#include "sha.h"
#include "util.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <CUnit/Basic.h>

extern uint8_t *SHA1file(FILE *);
extern uint8_t *SHA1string(const char *);

extern const char *test_msgs[];

static void testSHA1file(void);
static void testSHA1string(void);

void testSHA1(void)
{
    testSHA1file();
    testSHA1string();
}

void testSHA1file(void)
{
    /* TODO doesn't work with text/lorem-ipsum-with-newline.txt */

    const char *filename = "text/lorem-ipsum.txt"; /* 11417 bytes */
    FILE *fp = fopen(filename, "r");

    if (fp)
    {
        const char *expected = "1f10a1c87261cec87ba9b38e94154dea2a62c4d9";
        const char *actual = (const char *) SHA1file(fp);

        CU_ASSERT_STRING_EQUAL(actual, expected);

        if (strcmp(expected, actual))
        {
            fprintf(stderr, "\n");
            fprintf(stderr, "%s\n", filename);
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

void testSHA1string(void)
{
    const char *sha1s[] = {
        "da39a3ee5e6b4b0d3255bfef95601890afd80709",
        "86f7e437faa5a7fce15d1ddcb9eaeaea377667b8",
        "a9993e364706816aba3e25717850c26c9cd0d89d",
        "c12252ceda8be8994d5fa0290a47231c1d16aae3",
        "32d10c7b8cf96570ca04ce37f2a19d84240d3a89",
        "761c457bf73b14d27e9e9265c46f4b4dda11f940",
        "50abf5706a150990a08b2c5ea40fa0e585554732",
        "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12",
        "408d94384216f890ff7a0c3528e8bed1e0b01621"
    };

    for (uint8_t i = 0; test_msgs[i]; ++i)
    {
        const char *expected = sha1s[i];
        const char *actual = (const char *) SHA1string(test_msgs[i]);

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
