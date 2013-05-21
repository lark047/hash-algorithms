#include "sha.h"
#include "util.h"
#include "hmac.h"

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CUnit/Basic.h>

static void testSHA256file(void);
static void testSHA256string(void);
static void testSHA256_HMAC(void);

void testSHA256(void)
{
    testSHA256file();
    testSHA256string();
    testSHA256_HMAC();
}

static void testSHA256file(void)
{
    char *sha256s[] = {
        "3433556b9ae140dd3838bc22d269e5cc32714f800a13000e8ba5718a3e564680",
        "f3c1ff464194ac239bb7bb5e17868be1145a262813bb293aeb211c5aa81ec649",
        "c56c0acdfa4bec2d222527f99d015e26bcc3e897315690ec124d6372a197947d"
    };

    for (uint8_t i = 0; test_files[i]; ++i)
    {
        FILE *fp = fopen(test_files[i], "rb");

        CU_ASSERT_PTR_NOT_NULL_FATAL(fp)

        uint8_t *digest = SHA256file(fp);

        CU_ASSERT_PTR_NOT_NULL_FATAL(digest);

        char *expected = sha256s[i];
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

static void testSHA256string(void)
{
    char *sha256s[] = {
        "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
        "ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb",
        "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad",
        "f7846f55cf23e14eebeab5b4e1550cad5b509e3348fbc4efa3a1413d393cb650",
        "71c480df93d6ae2f1efad1447c66c9525e316218cf51fc8d9ed832f2daf18b73",
        "db4bfcbd4da0cd85a60c3c37d3fbd8805c77f15fc6b1fdfe614ee0a7c8fdb4c0",
        "f371bc4a311f2b009eef952dd83ca80e2b60026c8e935592d0f9c308453c813e",
        "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592",
        "ef537f25c895bfa782526529a9b63d97aa631564d5d789c2b765448c8635fb6c"
    };

    for (uint8_t i = 0; test_msgs[i]; ++i)
    {
        uint8_t *digest = SHA256string(test_msgs[i]);

        CU_ASSERT_PTR_NOT_NULL_FATAL(digest);

        char *expected = sha256s[i];
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

static void testSHA256_HMAC(void)
{
    char *keys[] = {
        "",
        "key",
        /* TODO 64-byte key */
        "==8utHoe30ASpIEmIe5Roa3#*e1$l7v8a1IeGi!gIu2OUr&$do++hiAjlAH_eTRi_Wi4dOu+l!2hiuThl7frIaclUswiuFI*GluSwiA*?ug@ASi$swOatR8A3Las0ia5",
        0
    };

    char *sha256s[][3] = {
        {
            "b613679a0814d9ec772f95d778c35fc5ff1697c493715653c6c712144292c5ad",
            "5d5d139563c95b5967b9bd9a8c9b233a9dedb45072794cd232dc1b74832607d0",
            "482abcd5245bbc0199defb85d213025151ac4e029f7221eedf43426ab709c448"
        }, {
            "9615a95d4a336118c435b9cd54c5e8644ab956b573aa2926274a1280b6674713",
            "780c3db4ce3de5b9e55816fba98f590631d96c075271b26976238d5f4444219b",
            "e136e20d639627902a9724af8f5239ea1d5a7063c053621650499336d8c7a83f"
        }, {
            "fd7adb152c05ef80dccf50a1fa4c05d5a3ec6da95575fc312ae7c5d091836351",
            "9c196e32dc0175f86f4b1cb89289d6619de6bee699e4c378e68309ed97a1a6ab",
            "4fa7f917f96551ecb4ee13ed25dd0495ffdfe995689ba63c5b5e67b9b8715371"
        }, {
            "5da81903793c4123cbd4903be9c224666eeca1ad581914585d13520d260ca0ac",
            "32b31360740cca471a8c528dcc26218e6979e4f79627dfff082da989a6bd3da6",
            "bc56ba807b6c85842dbe22ebd7e94afc187fd16bb2b68c1eaa7b07ef712bfe2a"
        }, {
            "7330f1ff9c3a321d2b2f051ab25a850b6300a1647d87a299f41e05961662896f",
            "d9e4abfac1bcffec48834477f1bcf9040306ec0912cfb7afe28f1ca7ea5c260f",
            "a3f661b6423b623d3c65809f5554966f0ab2b5195865a79ac46cb99790c5ec36"
        }, {
            "d92cfe0f557bf886cd9fca2da4957b413bf9d20838bfec997bb39559425282b5",
            "a670f3d151485f0c486185473c94e202e063dbc43dc83e1f0cbf89fa87dfd9d7",
            "c40d2ceb4469eced83ec08fcdc929e7599b59a1ba8ef5ad58b6d666461872148"
        }, {
            "efba2a164941755edfe627f413d9e053033e660d1b94fb122ea28572769c0370",
            "74fd175cd9448e6a4f0fde8e95c50550aa0a73ebc0589cf3c19b4e3e14d180a8",
            "7bdd31a124eb08add28942c9b43f1f0c8ae9967ca5013faa610a9e230131d34e"
        }, {
            "fb011e6154a19b9a4c767373c305275a5a69e8b68b0b4c9200c383dced19a416",
            "f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8",
            "687ae1de516b200e905409c0605e97c8b7d11930b4ea31e6f2692332f427b276"
        }, {
            "7933a89620ef65f17c1f64daa7473588cd2b7520cbc032251745cfa540efc09a",
            "e98139c39d76eb80d8db982552b44b251b94f312987f91ee72d12ef673caa813",
            "5e6bed85549ee322785eba00f9b280e6fa2e2fc6f3603aae1364e999ebe4c47d"
        }
    };

    for (uint8_t i = 0; test_msgs[i]; ++i)
    {
        for (uint8_t j = 0; keys[j]; ++j)
        {
            uint8_t *digest = SHA256hmac(keys[j], test_msgs[i]);

            CU_ASSERT_PTR_NOT_NULL_FATAL(digest);

            char *expected = sha256s[i][j];
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
