#include "sha.h"
#include "util.h"
#include "hmac.h"

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CUnit/Basic.h>

static void testSHA384file(void);
static void testSHA384string(void);
static void testSHA384_HMAC(void);

void testSHA384(void)
{
    testSHA384file();
    testSHA384string();
    testSHA384_HMAC();
}

static void testSHA384file(void)
{
    char *sha384s[] = {
        "01d5721fb074b9e9c00da0fba1469b8920bf46261097a63fb87faf4252ad588dfc81e2b7b0907809c7328233231cdde0",
        "85d454a3ef93dadb8af9450e5d4421b9517a9a31582cd4974ab45f8907d2508463189fedfbf0f962ded1015ebf8fe6fc",
        "7ff3700354d8ef45ec18b45bbd27b60cf3587506d433ccc0ea767f1ec83029aea72e97046249bc2a071a023b3ae5e487"
    };

    for (uint8_t i = 0; test_files[i]; ++i)
    {
        FILE *fp = fopen(test_files[i], "rb");

        CU_ASSERT_PTR_NOT_NULL_FATAL(fp)

        uint8_t *digest = SHA384file(fp);

        CU_ASSERT_PTR_NOT_NULL_FATAL(digest);

        char *expected = sha384s[i];
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

static void testSHA384string(void)
{
    char *sha384s[] = {
        "38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b",
        "54a59b9f22b0b80880d8427e548b7c23abd873486e1f035dce9cd697e85175033caa88e6d57bc35efae0b5afd3145f31",
        "cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7",
        "473ed35167ec1f5d8e550368a3db39be54639f828868e9454c239fc8b52e3c61dbd0d8b4de1390c256dcbb5d5fd99cd5",
        "feb67349df3db6f5924815d6c3dc133f091809213731fe5c7b5f4999e463479ff2877f5f2936fa63bb43784b12f3ebb4",
        "1761336e3f7cbfe51deb137f026f89e01a448e3b1fafa64039c1464ee8732f11a5341a6f41e0c202294736ed64db1a84",
        "b12932b0627d1c060942f5447764155655bd4da0c9afa6dd9b9ef53129af1b8fb0195996d2de9ca0df9d821ffee67026",
        "ca737f1014a48f4c0b6dd43cb177b0afd9e5169367544c494011e3317dbf9a509cb1e5dc1e85a941bbee3d7f2afbc9b1",
        "ed892481d8272ca6df370bf706e4d7bc1b5739fa2177aae6c50e946678718fc67a7af2819a021c2fc34e91bdb63409d7"
    };

    for (uint8_t i = 0; test_msgs[i]; ++i)
    {
        uint8_t *digest = SHA384string(test_msgs[i]);

        CU_ASSERT_PTR_NOT_NULL_FATAL(digest);

        char *expected = sha384s[i];
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

static void testSHA384_HMAC(void)
{
    char *keys[] = {
        "",
        "key",
        /* TODO 64-byte key */
        "==8utHoe30ASpIEmIe5Roa3#*e1$l7v8a1IeGi!gIu2OUr&$do++hiAjlAH_eTRi_Wi4dOu+l!2hiuThl7frIaclUswiuFI*GluSwiA*?ug@ASi$swOatR8A3Las0ia5",
        0
    };

    char *sha384s[][3] = {
        {
            "6c1f2ee938fad2e24bd91298474382ca218c75db3d83e114b3d4367776d14d3551289e75e8209cd4b792302840234adc",
            "99f44bb4e73c9d0ef26533596c8d8a32a5f8c10a9b997d30d89a7e35ba1ccf200b985f72431202b891fe350da410e43f",
            "7dd4bd7844633b9e66efb7502e37463b8d78c266b028389438f0960ba3af7ea4da6572f0c9d482925ed178de981b6c65"
        }, {
            "08b49fe6df418fd7e17b2ea9f0f9f18a4c58c983a354e515c37f157b01105954495631b1178a851789cc90d65367c162",
            "5d3b9a2b2b166c987b009e5a9946f7525e05f2f91adf65ba5ad135da5d0630296665ac14ca0f30b17d8a7f6674a13083",
            "293d777212f8003f3bbbd38295daab5027dae074c75bdc0d73bc8c7e79fef1159f27f53def50d6b8302a4336d1194b23"
        }, {
            "948f7c5caa500c31d7d4a0f52f3e3da7e33c8a9fe6ef528b8a9ac3e4adc4e24d908e6f40b737510e82354759dc5e9f06",
            "30ddb9c8f347cffbfb44e519d814f074cf4047a55d6f563324f1c6a33920e5edfb2a34bac60bdc96cd33a95623d7d638",
            "a9c9d0b9e64dec8d71286e95dddb802a8aea840b8be329b0466c0bcbb804df763d0c8d61f506448cc91d230f6c2b94ef"
        }, {
            "ad455918f2c5ceb1feb7e3b414a72e0bea2acccbdc1e318c45fd53d0d66c9d6dd72ab052c502ea553fabd9b28a3ada90",
            "316c0d2cef3eaa48bda32e9ec78fc6f8b018d04ba534c40748d544e29fcab705da9292409ac881bdcdbcd45fab11b55f",
            "7973a025785c338070b29734e5c4520396951c2023e54bc59a60376cced70576031b2f0ce21c151342e3f512226e148d"
        }, {
            "4c51e42fa65d91d1a247e9ff48de70c323763393f08aaa4d8b2aec462be5f31e34d16346d97654f4a330e8cfbb6e57cd",
            "17729173fb785f9397b319ea1998e09401247002185c6e8b9d68f95b9d529e370f4c59278c80d5c33b5ab37ca87cc55d",
            "c325dfbd4424a8db4710e2982da09c1e8e164e7e2bf94613fc951b1393aaed2370886bb86ac53c1de8443a5d653497e9"
        }, {
            "018089ad5e76e818df2e3a87989ab23144a45867fc62584014cb93e7300f1149b6b5b586176ac95703174342d18bdc8a",
            "f239f867531d54cb7d41d9fec38289329070e99480bd52fe36a10d83d7bfe5dcb27ae9566a749de9351c5a36c707b2fd",
            "a1a38a29c502e6866a2fff5b89e2ac20056dd8c5069fc9140b820dfc94116dbc1e595f99b3050a0dcdc6a64f7127aee5"
        }, {
            "dd9b938e80874bf3f49959a743c9bc7440855da303d348d0cc9d4c991962c1db116a0445e70269896702c418c442b1a4",
            "83c79083cdd94ae037d144c7accd4b85c2c5e613e876c9a7890c73d4438781beeacd2f947144f2ec1ffe93efac7cc8bd",
            "1dce6e30786f027e8a2e9b0303e85076dd5dd6f6a70384ad57b39a25e8b5c0c863db12b3c1a41e7f537c4ee0a9f0e708"
        }, {
            "0a3d8f99afb726f97d32cc513f3a5ad51246984fd3e916cefb82fc7967ee42eae547cd88aefd84493d2585e55906e1b0",
            "d7f4727e2c0b39ae0f1e40cc96f60242d5b7801841cea6fc592c5d3e1ae50700582a96cf35e1e554995fe4e03381c237",
            "270c597ba3ddffdf8890ac8c0470f23e796288cd20adad359ef4fee48af78b0a524d6748294d6797c41ad7ef5c41a57c"
        }, {
            "48de162e23c80aa908d93d1f9a2481bed3d1081e8958cd60f8d223692e21d6ed5bdeb523570a7ba58910ce96060925c4",
            "3e7fa092f34fb8d6756fe3aa2a98f514679c8c3fb28cfd2cd2f7d054cb2fd64cf28ca17c067a595dc85dc12c161ddbc8",
            "5a4986a048e88ef0b5037433b68c1460623a0222fa6eb305bcc39894caec2cbe42caadb2b335b65fbe0c2834cc430429"
        }
    };

    for (uint8_t i = 0; test_msgs[i]; ++i)
    {
        for (uint8_t j = 0; keys[j]; ++j)
        {
            uint8_t *digest = HMAC_SHA384(keys[j], test_msgs[i]);

            CU_ASSERT_PTR_NOT_NULL_FATAL(digest);

            char *expected = sha384s[i][j];
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
