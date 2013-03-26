#include "md.h"
#include "util.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CUnit/Basic.h>

static void testMD4file(void);
static void testMD4string(void);
static void testMD4string_collision(void);

void testMD4(void)
{
    testMD4file();
    testMD4string();
    testMD4string_collision();
}

void testMD4file(void)
{
    char *md4s[] = {
        "cd8da0e8bcacf00b60c020b464457b97",
        "5a75565375c0d0a824fdfcab492fa780"
    };

    for (uint8_t i = 0; test_files[i]; ++i)
    {
        FILE *fp = fopen(test_files[i], "rb");

        CU_ASSERT_PTR_NOT_NULL_FATAL(fp)

        char *expected = md4s[i];
        char *actual = (char *) MD4file(fp);

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

void testMD4string(void)
{
    char *md4s[] = {
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
        char *expected = md4s[i];
        char *actual = (char *) MD4string(test_msgs[i]);

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

        free(actual);
    }
}

void testMD4string_collision(void)
{
#if 0
    /**
     * taken from Wikipedia: http://en.wikipedia.org/wiki/MD4
     */

    const char *msgs[] = {
        "839c7a4d7a92cb5678a5d5b9eea5a7573c8a74deb366c3dc20a083b69f5d2a3bb3719dc69891e9f95e809fd7e8b23ba6318edd45e51fe39708bf9427e9c3e8b9",
        "839c7a4d7a92cbd678a5d529eea5a7573c8a74deb366c3dc20a083b69f5d2a3bb3719dc69891e9f95e809fd7e8b23ba6318edc45e51fe39708bf9427e9c3e8b9",
      /*               ^       ^                                                                              ^                           */
    };
#elif 0

#define LEN 128

    /**
     * taken from http://eprint.iacr.org/2004/199.pdf
     */

    uint8_t *msg = malloc(LEN * sizeof *msg);
    uint8_t *p = msg;

    *p++ = /* 0x4d */ 'M';
    *p++ = /* 0x7a */ 'z';
    *p++ = 0x9c;
    *p++ = 0x83;
    *p++ = /* 0x56 */ 'V';
    *p++ = 0xcb;
    *p++ = 0x92;
    *p++ = /* 0x7a */ 'z';
    *p++ = 0xb9;
    *p++ = 0xd5;
    *p++ = 0xa5;
    *p++ = /* 0x78 */ 'x';
    *p++ = /* 0x57 */ 'W';
    *p++ = 0xa7;
    *p++ = 0xa5;
    *p++ = 0xee;
    *p++ = 0xde;
    *p++ = /* 0x74 */ 't';
    *p++ = 0x8a;
    *p++ = /* 0x3c */ '<';
    *p++ = 0xdc;
    *p++ = 0xc3;
    *p++ = /* 0x66 */ 'f';
    *p++ = 0xb3;
    *p++ = 0xb6;
    *p++ = 0x83;
    *p++ = 0xa0;
    *p++ = /* 0x20 */ ' ';
    *p++ = /* 0x3b */ ';';
    *p++ = /* 0x2a */ '*';
    *p++ = /* 0x5d */ ']';
    *p++ = 0x9f;
    *p++ = 0xc6;
    *p++ = 0x9d;
    *p++ = /* 0x71 */ 'q';
    *p++ = 0xb3;
    *p++ = 0xf9;
    *p++ = 0xe9;
    *p++ = 0x91;
    *p++ = 0x98;
    *p++ = 0xd7;
    *p++ = 0x9f;
    *p++ = 0x80;
    *p++ = /* 0x5e */ '^';
    *p++ = 0xa6;
    *p++ = /* 0x3b */ ';';
    *p++ = 0xb2;
    *p++ = 0xe8;
    *p++ = /* 0x45 */ 'E';
    *p++ = 0xdd;
    *p++ = 0x8e;
    *p++ = /* 0x31 */ '1';
    *p++ = 0x97;
    *p++ = 0xe3;
    *p++ = 0x1f;
    *p++ = 0xe5;
    *p++ = /* 0x27 */ '\'';
    *p++ = 0x94;
    *p++ = 0xbf;
    *p++ = 0x08;
    *p++ = 0xb9;
    *p++ = 0xe8;
    *p++ = 0xc3;
    *p++ = 0xe9;

    uint8_t *d1 = MD4(msg, LEN);
#endif

    /**
     * taken from http://www.iacr.org/archive/fse2007/45930311/45930311.pdf
     */

#define LEN 20

    uint8_t *msg1 = malloc(LEN * sizeof *msg1);
    uint8_t *msg2 = malloc(LEN * sizeof *msg2);

    CU_ASSERT_PTR_NOT_NULL_FATAL(msg1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(msg2);

    uint8_t *p = msg1, *q = msg2;

    *p++ = 0x42; *q++ = 0x42;
    *p++ = 0x79; *q++ = 0x79;
    *p++ = 0x2d; *q++ = 0x2d;
    *p++ = 0x65; *q++ = 0x65;
    *p++ = 0xf0; *q++ = 0xf0;
    *p++ = 0xf8; *q++ = 0xf8;
    *p++ = 0x4f; *q++ = 0x4f;
    *p++ = 0xd8; *q++ = 0xd8;
    *p++ = 0xd5; *q++ = 0xd5;
    *p++ = 0x7d; *q++ = 0x7d;
    *p++ = 0x86; *q++ = 0x86;
    *p++ = 0xbf; *q++ = 0xbf;
    *p++ = 0x78; *q++ = 0x78;
    *p++ = 0x54; *q++ = 0x54;
    *p++ = 0x9d; *q++ = 0x9d;
    *p++ = 0x67; *q++ = 0x67;
    *p++ = 0x3f; *q++ = 0x3f;
    *p++ = 0xb3; *q++ = 0xb3;
    *p++ = 0x8c; *q++ = 0x8c;
    *p++ = 0xaa; *q++ = 0xac; /* aa <-> ac */

    uint8_t *d1 = MD4(msg1, LEN);
    uint8_t *d2 = MD4(msg2, LEN);

    CU_ASSERT_PTR_NOT_NULL_FATAL(d1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(d2);

    PRINT("%s\n", (char *) d1);
    PRINT("%s\n", (char *) d2);

    CU_ASSERT_STRING_NOT_EQUAL(msg1, msg2);
    CU_ASSERT_STRING_EQUAL(d1, d2);

    free(msg1);
    free(msg2);

    free(d1);
    free(d2);
}
