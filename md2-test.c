#include "md.h"
#include "util.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CUnit/Basic.h>

static void testMD2string(void);

#if 0
static void testMD2file(void);
static void testMD2string_collision(void);
#endif

void testMD2(void)
{
    /* testMD2file(); */
    testMD2string();
    /* testMD2string_collision(); */
}

#if 0
void testMD2file(void)
{
    /* TODO find an MD2 file generator */
    char *md2s[] = {
        "",
        ""
    };

    for (uint8_t i = 0; test_files[i]; ++i)
    {
        FILE *fp = fopen(test_files[i], "rb");

        CU_ASSERT_PTR_NOT_NULL_FATAL(fp)

        char *expected = md2s[i];
        char *actual = (char *) MD2file(fp);

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

void testMD2string(void)
{
    char *md2s[] = {
        "8350e5a3e24c153df2275c9f80692773",
        "32ec01ec4a6dac72c0ab96fb34c0b5d1",
        "da853b0d3f88d99b30283a69e6ded6bb",
        "ab4f496bfb2a530b219ff33031fe06b0",
        "4e8ddff3650292ab5a4108c3aa47940b",
        "da33def2a42df13975352846c30338cd",
        "d5976f79d83d3a0dc9806c3c66f3efd8",
        "03d85a0d629d2c442e987525319fc471",
        "71eaa7e440b611e41a6f0d97384b342a"
    };

    for (uint8_t i = 0; test_msgs[i]; ++i)
    {
        char *expected = md2s[i];
        char *actual = (char *) MD2string(test_msgs[i]);

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

#if 0
void testMD2string_collision(void)
{
    /* TODO find some MD2 collision examples */

    /**
     * taken from Wikipedia: http://en.wikipedia.org/wiki/MD2
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

    /**
     * taken from http://www.iacr.org/archive/fse2007/45930311/45930311.pdf
     *
     *      42792d65f0f84fd8d57d86bf78549d673fb38caa
     *      42792d65f0f84fd8d57d86bf78549d673fb38cac
     * diff:                                       ^
     */

    uint8_t msgs_iacr2[][20] = {
        { 0x42,0x79,0x2d,0x65,0xf0,0xf8,0x4f,0xd8,0xd5,0x7d,0x86,0xbf,0x78,0x54,0x9d,0x67,0x3f,0xb3,0x8c,0xaa },
        { 0x42,0x79,0x2d,0x65,0xf0,0xf8,0x4f,0xd8,0xd5,0x7d,0x86,0xbf,0x78,0x54,0x9d,0x67,0x3f,0xb3,0x8c,0xac }
    };

    uint8_t *msgs[] = {
        msgs_wiki[0], msgs_wiki[1],
        msgs_iacr2[0], msgs_iacr2[1],
        0
    };

    const uint8_t lengths[] = {
        SIZE(msgs_wiki[0]),
        SIZE(msgs_iacr2[0])
    };

    for (uint8_t i = 0; msgs[i]; i += 2)
    {
        const uint8_t len = lengths[i / 2];

        uint8_t *d1 = MD2(msgs[i + 0], len);
        uint8_t *d2 = MD2(msgs[i + 1], len);

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
#endif
