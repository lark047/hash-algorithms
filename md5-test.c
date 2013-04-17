#include "md.h"
#include "util.h"
#include "hmac.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CUnit/Basic.h>

static void testMD5file(void);
static void testMD5string(void);
static void testMD5_collision(void);
static void testMD5file_collision(void);
static void testMD5_HMAC(void);

void testMD5(void)
{
    testMD5file();
    testMD5string();
    testMD5_collision();
    testMD5file_collision();
    testMD5_HMAC();
}

static void testMD5file(void)
{
    char *md5s[] = {
        "6901b58e9fe9368c6b200d5738d64f21",
        "b777bb64154defc9bb2602753c3ea8a6",
        "3d50f385e48f5ac4c03a8607cf114e6d"
    };

    for (uint8_t i = 0; test_files[i]; ++i)
    {
        FILE *fp = fopen(test_files[i], "rb");

        CU_ASSERT_PTR_NOT_NULL_FATAL(fp)

        char *expected = md5s[i];
        char *actual = (char *) MD5file(fp);

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

static void testMD5string(void)
{
    char *md5s[] = {
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
        char *expected = md5s[i];
        char *actual = (char *) MD5string(test_msgs[i]);

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

static void testMD5_collision(void)
{
    /* from http://www.mscs.dal.ca/~selinger/md5collision/ */

    uint8_t msgs_dal[][128] = {
        {
            0xd1,0x31,0xdd,0x02,0xc5,0xe6,0xee,0xc4,0x69,0x3d,0x9a,0x06,0x98,0xaf,0xf9,0x5c,
            0x2f,0xca,0xb5,0x87,0x12,0x46,0x7e,0xab,0x40,0x04,0x58,0x3e,0xb8,0xfb,0x7f,0x89,
            0x55,0xad,0x34,0x06,0x09,0xf4,0xb3,0x02,0x83,0xe4,0x88,0x83,0x25,0x71,0x41,0x5a,
            0x08,0x51,0x25,0xe8,0xf7,0xcd,0xc9,0x9f,0xd9,0x1d,0xbd,0xf2,0x80,0x37,0x3c,0x5b,
            0xd8,0x82,0x3e,0x31,0x56,0x34,0x8f,0x5b,0xae,0x6d,0xac,0xd4,0x36,0xc9,0x19,0xc6,
            0xdd,0x53,0xe2,0xb4,0x87,0xda,0x03,0xfd,0x02,0x39,0x63,0x06,0xd2,0x48,0xcd,0xa0,
            0xe9,0x9f,0x33,0x42,0x0f,0x57,0x7e,0xe8,0xce,0x54,0xb6,0x70,0x80,0xa8,0x0d,0x1e,
            0xc6,0x98,0x21,0xbc,0xb6,0xa8,0x83,0x93,0x96,0xf9,0x65,0x2b,0x6f,0xf7,0x2a,0x70
        }, {
            0xd1,0x31,0xdd,0x02,0xc5,0xe6,0xee,0xc4,0x69,0x3d,0x9a,0x06,0x98,0xaf,0xf9,0x5c,
            0x2f,0xca,0xb5,0x07,0x12,0x46,0x7e,0xab,0x40,0x04,0x58,0x3e,0xb8,0xfb,0x7f,0x89,
            0x55,0xad,0x34,0x06,0x09,0xf4,0xb3,0x02,0x83,0xe4,0x88,0x83,0x25,0xf1,0x41,0x5a,
            0x08,0x51,0x25,0xe8,0xf7,0xcd,0xc9,0x9f,0xd9,0x1d,0xbd,0x72,0x80,0x37,0x3c,0x5b,
            0xd8,0x82,0x3e,0x31,0x56,0x34,0x8f,0x5b,0xae,0x6d,0xac,0xd4,0x36,0xc9,0x19,0xc6,
            0xdd,0x53,0xe2,0x34,0x87,0xda,0x03,0xfd,0x02,0x39,0x63,0x06,0xd2,0x48,0xcd,0xa0,
            0xe9,0x9f,0x33,0x42,0x0f,0x57,0x7e,0xe8,0xce,0x54,0xb6,0x70,0x80,0x28,0x0d,0x1e,
            0xc6,0x98,0x21,0xbc,0xb6,0xa8,0x83,0x93,0x96,0xf9,0x65,0xab,0x6f,0xf7,0x2a,0x70
        }
    };

    uint8_t *msgs[] = {
        msgs_dal[0], msgs_dal[1],
        0
    };

    const uint8_t lengths[] = {
        SIZE(msgs_dal[0])
    };

    for (uint8_t i = 0; msgs[i]; i += 2)
    {
        const uint8_t len = lengths[i / 2];

        uint8_t *d1 = MD5(msgs[i + 0], len);
        uint8_t *d2 = MD5(msgs[i + 1], len);

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

static void testMD5file_collision(void)
{
    char *collision_files[] = {
        "files/letter_of_rec.ps",
        "files/order.ps",
        0
    };

    for (uint8_t i = 0; collision_files[i]; i += 2)
    {
        FILE *f1 = fopen(collision_files[i + 0], "rb");
        FILE *f2 = fopen(collision_files[i + 1], "rb");

        CU_ASSERT_PTR_NOT_NULL_FATAL(f1);
        CU_ASSERT_PTR_NOT_NULL_FATAL(f2);

        uint8_t *d1 = MD5file(f1);
        uint8_t *d2 = MD5file(f2);

        CU_ASSERT_PTR_NOT_NULL_FATAL(d1);
        CU_ASSERT_PTR_NOT_NULL_FATAL(d2);

        PRINT("%s\n", (char *) d1);
        PRINT("%s\n", (char *) d2);

        CU_ASSERT_STRING_EQUAL(d1, d2);

        free(d1);
        free(d2);

        fclose(f1);
        fclose(f2);
    }
}

static void testMD5_HMAC(void)
{
    char *keys[] = {
        "",
        "key",
        "==8utHoe30ASpIEmIe5Roa3#*e1$l7v8a1IeGi!gIu2OUr&$do++hiAjlAH_eTRi_Wi4dOu+l!2hiuThl7frIaclUswiuFI*GluSwiA*?ug@ASi$swOatR8A3Las0ia5",
        0
    };

    char *md5s[][3] = {
        {
            "74e6f7298a9c2d168935f58c001bad88", "63530468a04e386459855da0063b6596", "c7dd61a377879126bc0d229aff0ff24a"
        }, {
            "3673438f11d71c21a9b8b59232a3dd61", "80ea44ccea14cc2263889567cfacccd7", "db2e0480cd24bd920cb7e5db6bc9da00"
        }, {
            "dd2701993d29fdd0b032c233cec63403", "d2fe98063f876b03193afb49b4979591", "c600cb3b71f529f810260bffc0dffda4"
        }, {
            "3ba8be08caf4c56ce9568a981f366f70", "73aec30615dd8f88e38a672a9d758788", "7616fc2a059fc6ba9a188eec22b7ca17"
        }, {
            "3d7e2530f66f8541361aed88603dd77a", "14fe9f9a654e250f7d7532ca3633c067", "9e2deccbb4557dfc311063ab1ac4a94f"
        }, {
            "d59789e1172d0ac210a5ad40182fa661", "e8ade536d6eb223c43e1b45fb28ad3c8", "5f996e5b68bb57d5a1ca66c2be29d841"
        }, {
            "19ff30ad2150544df4eeb994348c8242", "40cdfa72ad545a6258a13d4ce5e6e2df", "3ae00877a414f0d144d61c82652a1a35"
        }, {
            "ad262969c53bc16032f160081c4a07a0", "80070713463e7749b90c2dc24911e275", "3a1b7a14594a06c8dba868c3c97418f0"
        }, {
            "41ecaa288efe0daa720c7bbcf4e5f3c8", "120a17985a1e97bf8f0e38a52fb9fe79", "c4573d4fb4a694e249eb364dc5e15e43"
        }
    };

    for (uint8_t i = 0; test_msgs[i]; ++i)
    {
        for (uint8_t j = 0; keys[j]; ++j)
        {
            char *expected = md5s[i][j];
            char *actual = (char *) HMAC_MD5(keys[j], test_msgs[i]);

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

}
