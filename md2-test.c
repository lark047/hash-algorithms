#include "md.h"
#include "util.h"
#include "hmac.h"

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CUnit/Basic.h>

static void testMD2string(void);
static void testMD2_HMAC(void);

#if 0
static void testMD2file(void);
static void testMD2_collision(void);
#endif

void testMD2(void)
{
    /* testMD2file(); */
    testMD2string();
    /* testMD2_collision(); */
    testMD2_HMAC();
}

#if 0
static void testMD2file(void)
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

        uint8_t *digest = MD2file(fp);

        CU_ASSERT_PTR_NOT_NULL_FATAL(digest);

        char *expected = md2s[i];
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
            fprintf(stderr, "file    : %s\n", test_files[i]);
            fprintf(stderr, "expected: %s\n", expected);
            fprintf(stderr, "actual  : %s\n", actual);
        }

        fclose(fp);
        free(digest);
        free(actual);
    }
}
#endif

static void testMD2string(void)
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
        uint8_t *digest = MD2string(test_msgs[i]);

        CU_ASSERT_PTR_NOT_NULL_FATAL(digest);

        char *expected = md2s[i];
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
            fprintf(stderr, "string  : -->%s<--\n", test_msgs[i]);
            fprintf(stderr, "expected: %s\n", expected);
            fprintf(stderr, "actual  : %s\n", actual);
        }

        free(digest);
        free(actual);
    }
}

#if 0
static void testMD2_collision(void)
{
    /* TODO find some MD2 collision examples */

    /**
     * taken from
     *
     *      839c7a4d7a92cb5678a5d5b9eea5a7573c8a74deb366c3dc20a083b69f5d2a3bb3719dc69891e9f95e809fd7e8b23ba6318edd45e51fe39708bf9427e9c3e8b9
     *      839c7a4d7a92cbd678a5d529eea5a7573c8a74deb366c3dc20a083b69f5d2a3bb3719dc69891e9f95e809fd7e8b23ba6318edc45e51fe39708bf9427e9c3e8b9
     * diff:               ^       ^                                                                              ^
     */

    uint8_t msgs_wiki[][] = {
        {
        }, {
        }
    };

    /**
     * taken from
     *
     *      4d7a9c8356cb927ab9d5a57857a7a5eede748a3cdcc366b3b683a0203b2a5d9fc69d71b3f9e99198d79f805ea63bb2e845dd8e3197e31fe52794bf08b9e8c3e9
     *      4d7a9c83d6cb927a29d5a57857a7a5eede748a3cdcc366b3b683a0203b2a5d9fc69d71b3f9e99198d79f805ea63bb2e845dc8e3197e31fe52794bf08b9e8c3e9
     * diff:        ^       ^                                                                                  ^
     */

    /**
     * taken from
     *
     *      42792d65f0f84fd8d57d86bf78549d673fb38caa
     *      42792d65f0f84fd8d57d86bf78549d673fb38cac
     * diff:                                       ^
     */

    uint8_t msgs_iacr2[][] = {
    };

    uint8_t *msgs[] = {
        msgs_wiki[0], msgs_wiki[1],
        msgs_iacr2[0], msgs_iacr2[1],
        0
    };

    uint8_t lengths[] = {
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

        char *bufs[] = {
            to_string(d1, DIGEST_LENGTH),
            to_string(d2, DIGEST_LENGTH)
        };

        CU_ASSERT_PTR_NOT_NULL_FATAL(bufs[0]);
        CU_ASSERT_PTR_NOT_NULL_FATAL(bufs[1]);

        PRINT("%s\n", bufs[0]);
        PRINT("%s\n", bufs[1]);

        CU_ASSERT_NOT_EQUAL(memcmp(msgs[i + 0], msgs[i + 1], len), 0);
        CU_ASSERT_STRING_EQUAL(bufs[0], bufs[1]);

        free(d1);
        free(d2);
        free(bufs[0]);
        free(bufs[1]);
    }
}
#endif

static void testMD2_HMAC(void)
{
    char *keys[] = {
        "",
        "key",
        /* TODO 64-byte key */
        "==8utHoe30ASpIEmIe5Roa3#*e1$l7v8a1IeGi!gIu2OUr&$do++hiAjlAH_eTRi_Wi4dOu+l!2hiuThl7frIaclUswiuFI*GluSwiA*?ug@ASi$swOatR8A3Las0ia5",
        0,
    };

    char *md2s[][3] = {
        {
            "6f6e031223b36cd2a997787a03d16bf5", "7bf1ff992bec5708c70f0db5d5054486", "dd91b79e9947cef2d2a015b83c344c57"
        }, {
            "c732ea04ad1a795c6f9d60db270f1425", "3602aada95845349eae2474f7944f3c6", "d300a02026e817a09581eb46f8a9f7b3"
        }, {
            "17b0f48a8ee89e4c7748890ea9d31b90", "d3f42db81f8a7e379cc87e187fba861b", "fefe253fd12626ad7cc15c41ba137824"
        }, {
            "458a33dc7169121c602ac71a6f44e95c", "e596a93a225fdcfc2c8af61ff1d1d119", "41d5f3aa3f9ca86c0a7b6170a777c1e5"
        }, {
            "dd13f3165c4d29c3b75f789523b1ef8f", "d0df8523efa5b6a9fe099c10209e8d84", "160858784cf404f28665161b8c9811ae"
        }, {
            "7416b7925d545e7849b982e6c0323e72", "25fd227415bf7436b4f1673ed7790d07", "f115d2e131e0d6f43acdbaa490cacd24"
        }, {
            "daa525e74f2019d91bf6933d13fa9850", "654b38351a6af480c4ef9eb8fb637f67", "ae011a519550aff337110e484eef2a9a"
        }, {
            "6e0e1a43d01c170ab9da7794c0130934", "13758b9534bfb38d850457814613b0c1", "d91be2a92401be5e909659b6a14aeba3"
        }, {
            "58117bd5463a565a22f7f85dddeb2089", "a162a4059188270346f1addb5259f9e5", "dd9db032cd815ad58055c12574251bb8"
        }
    };

    for (uint8_t i = 0; test_msgs[i]; ++i)
    {
        for (uint8_t j = 0; keys[j]; ++j)
        {
            uint8_t *digest = MD2hmac(keys[j], test_msgs[i]);

            CU_ASSERT_PTR_NOT_NULL_FATAL(digest);

            char *expected = md2s[i][j];
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
                fprintf(stderr, "string  : -->%s<--\n", test_msgs[i]);
                fprintf(stderr, "key     : -->%s<--\n", keys[j]);
                fprintf(stderr, "expected: %s\n", expected);
                fprintf(stderr, "actual  : %s\n", actual);
            }

            free(digest);
            free(actual);
        }
    }
}
