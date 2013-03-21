#include "md5.h"
#include "util.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <CUnit/Basic.h>

extern uint8_t *MD5file(FILE *);
extern uint8_t *MD5string(const char *);

extern const char *test_files[];
extern const char *test_msgs[];

static void testMD5file(void);
static void testMD5string(void);

void testMD5(void)
{
    testMD5file();
    testMD5string();
}

void testMD5file(void)
{
    const char *md5s[] = {
        "6901b58e9fe9368c6b200d5738d64f21",
        "b777bb64154defc9bb2602753c3ea8a6"
    };

    for (uint8_t i = 0; test_files[i]; ++i)
    {
        FILE *fp = fopen(test_files[i], "rb");

        CU_ASSERT_PTR_NOT_NULL_FATAL(fp)

        const char *expected = md5s[i];
        const char *actual = (const char *) MD5file(fp);

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

void testMD5string(void)
{
    const char *md5s[] = {
        "d41d8cd98f00b204e9800998ecf8427e",
        "0cc175b9c0f1b6a831c399e269772661",
        "900150983cd24fb0d6963f7d28e17f72",
        "f96b697d7cb7938d525a2f31aaf161d0",
        "c3fcd3d76192e4007dfb496cca67e13b",
        "d174ab98d277d9f5a5611c2c9f419d9f",
        "57edf4a22be3c955ac49da2e2107b67a",
        "9e107d9d372bb6826bd81d3542a419d6",
        "e4d909c290d0fb1ca068ffaddf22cbd0"
    };

    for (uint8_t i = 0; test_msgs[i]; ++i)
    {
        const char *expected = md5s[i];
        const char *actual = (const char *) MD5string(test_msgs[i]);

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
