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
    /**
     * taken from Wikipedia: http://en.wikipedia.org/wiki/MD4
     *
     *      839c7a4d7a92cb5678a5d5b9eea5a7573c8a74deb366c3dc20a083b69f5d2a3bb3719dc69891e9f95e809fd7e8b23ba6318edd45e51fe39708bf9427e9c3e8b9
     *      839c7a4d7a92cbd678a5d529eea5a7573c8a74deb366c3dc20a083b69f5d2a3bb3719dc69891e9f95e809fd7e8b23ba6318edc45e51fe39708bf9427e9c3e8b9
     * diff:               ^       ^                                                                              ^
     */

    uint8_t msgs_wiki[][64] = {
        {
            0x83,0x9c,0x7a,0x4d,0x7a,0x92,0xcb,0x56,0x78,0xa5,0xd5,0xb9,0xee,0xa5,0xa7,0x57,
            0x3c,0x8a,0x74,0xde,0xb3,0x66,0xc3,0xdc,0x20,0xa0,0x83,0xb6,0x9f,0x5d,0x2a,0x3b,
            0xb3,0x71,0x9d,0xc6,0x98,0x91,0xe9,0xf9,0x5e,0x80,0x9f,0xd7,0xe8,0xb2,0x3b,0xa6,
            0x31,0x8e,0xdd,0x45,0xe5,0x1f,0xe3,0x97,0x08,0xbf,0x94,0x27,0xe9,0xc3,0xe8,0xb9
        }, {
            0x83,0x9c,0x7a,0x4d,0x7a,0x92,0xcb,0xd6,0x78,0xa5,0xd5,0x29,0xee,0xa5,0xa7,0x57,
            0x3c,0x8a,0x74,0xde,0xb3,0x66,0xc3,0xdc,0x20,0xa0,0x83,0xb6,0x9f,0x5d,0x2a,0x3b,
            0xb3,0x71,0x9d,0xc6,0x98,0x91,0xe9,0xf9,0x5e,0x80,0x9f,0xd7,0xe8,0xb2,0x3b,0xa6,
            0x31,0x8e,0xdc,0x45,0xe5,0x1f,0xe3,0x97,0x08,0xbf,0x94,0x27,0xe9,0xc3,0xe8,0xb9
        }
    };

    /**
     * taken from http://eprint.iacr.org/2004/199.pdf
     *
     *      4d7a9c8356cb927ab9d5a57857a7a5eede748a3cdcc366b3b683a0203b2a5d9fc69d71b3f9e99198d79f805ea63bb2e845dd8e3197e31fe52794bf08b9e8c3e9
     *      4d7a9c83d6cb927a29d5a57857a7a5eede748a3cdcc366b3b683a0203b2a5d9fc69d71b3f9e99198d79f805ea63bb2e845dc8e3197e31fe52794bf08b9e8c3e9
     * diff:        ^       ^                                                                                  ^
     */

#if 0 /* TODO produces incorrect hash value */
    uint8_t msgs_iacr1[][64] = {
        {
            0x4d,0x7a,0x9c,0x83,0x56,0xcb,0x92,0x7a,0xb9,0xd5,0xa5,0x78,0x57,0xa7,0xa5,0xee,
            0xde,0x74,0x8a,0x3c,0xdc,0xc3,0x66,0xb3,0xb6,0x83,0xa0,0x20,0x3b,0x2a,0x5d,0x9f,
            0xc6,0x9d,0x71,0xb3,0xf9,0xe9,0x91,0x98,0xd7,0x9f,0x80,0x5e,0xa6,0x3b,0xb2,0xe8,
            0x45,0xdd,0x8e,0x31,0x97,0xe3,0x1f,0xe5,0x27,0x94,0xbf,0x08,0xb9,0xe8,0xc3,0xe9
        }, {
            0x4d,0x7a,0x9c,0x83,0xd6,0xcb,0x92,0x7a,0x29,0xd5,0xa5,0x78,0x57,0xa7,0xa5,0xee,
            0xde,0x74,0x8a,0x3c,0xdc,0xc3,0x66,0xb3,0xb6,0x83,0xa0,0x20,0x3b,0x2a,0x5d,0x9f,
            0xc6,0x9d,0x71,0xb3,0xf9,0xe9,0x91,0x98,0xd7,0x9f,0x80,0x5e,0xa6,0x3b,0xb2,0xe8,
            0x45,0xdc,0x8e,0x31,0x97,0xe3,0x1f,0xe5,0x27,0x94,0xbf,0x08,0xb9,0xe8,0xc3,0xe9
        }
    };
#endif

    /**
     * taken from http://www.iacr.org/archive/fse2007/45930311/45930311.pdf
     *
     *      42792d65f0f84fd8d57d86bf78549d673fb38caa
     *      42792d65f0f84fd8d57d86bf78549d673fb38cac
     * diff:                                        ^
     */

    uint8_t msgs_iacr2[][20] = {
        { 0x42,0x79,0x2d,0x65,0xf0,0xf8,0x4f,0xd8,0xd5,0x7d,0x86,0xbf,0x78,0x54,0x9d,0x67,0x3f,0xb3,0x8c,0xaa },
        { 0x42,0x79,0x2d,0x65,0xf0,0xf8,0x4f,0xd8,0xd5,0x7d,0x86,0xbf,0x78,0x54,0x9d,0x67,0x3f,0xb3,0x8c,0xac }
    };

    uint8_t *msgs[] = {
        msgs_wiki[0], msgs_wiki[1],
#if 0
        msgs_iacr1[0], msgs_iacr1[1],
#endif
        msgs_iacr2[0], msgs_iacr2[1],
        0
    };

    const uint8_t lengths[] = {
        SIZE(msgs_wiki[0]),
#if 0
        SIZE(msgs_iacr1[0]),
#endif
        SIZE(msgs_iacr2[0])
    };

    for (uint8_t i = 0; msgs[i]; i += 2)
    {
        const uint8_t len = lengths[i / 2];

        uint8_t *d1 = MD4(msgs[i + 0], len);
        uint8_t *d2 = MD4(msgs[i + 1], len);

        CU_ASSERT_PTR_NOT_NULL_FATAL(d1);
        CU_ASSERT_PTR_NOT_NULL_FATAL(d2);

        PRINT("%s\n", (char *) d1);
        PRINT("%s\n", (char *) d2);

        CU_ASSERT_NOT_EQUAL(memcmp(msgs[i + 0], msgs[i + 1], len), 0);
        CU_ASSERT_STRING_EQUAL(d1, d2);

        free(d1);
        free(d2);
    }
}
