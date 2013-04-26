#include "sha.h"
#include "util.h"
#include "hmac.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CUnit/Basic.h>

static void testSHA512file(void);
static void testSHA512string(void);
static void testSHA512_HMAC(void);

void testSHA512(void)
{
    testSHA512file();
    testSHA512string();
    testSHA512_HMAC();
}

void testSHA512file(void)
{
    char *sha512s[] = {
        "5380d25153631747975a86cb691432f2e0e2bd906f195414e7bc52d06079a1683af0a892262260d5623d820f20d87033e8b685d1455b17d34c7ee0114892ae8a",
        "e42e221feea27d17d92b9c0b7a9ae02fcb84d70ddb8acfbd5ce4c2922f23d9d6e73cbb294a41e72228ceac4aba3514d7b2fcde5cb48e63d3aa02eb0112e73f5f",
        "19c1cbb3e16c3e3f6fe4b5adca71dfc7ad20cc345380a1fe5e173253d087730ffca0cc152235b5d4de7d27982633d5254a2ebe4dbd7a61a71be97aa9fbca4ccb"
    };

    for (uint8_t i = 0; test_files[i]; ++i)
    {
        FILE *fp = fopen(test_files[i], "rb");

        CU_ASSERT_PTR_NOT_NULL_FATAL(fp)

        char *expected = sha512s[i];
        char *actual = (char *) SHA512file(fp);

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

void testSHA512string(void)
{
    char *sha512s[] = {
        "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e",
        "1f40fc92da241694750979ee6cf582f2d5d7d28e18335de05abc54d0560e0f5302860c652bf08d560252aa5e74210546f369fbbbce8c12cfc7957b2652fe9a75",
        "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f",
        "107dbf389d9e9f71a3a95f6c055b9251bc5268c2be16d6c13492ea45b0199f3309e16455ab1e96118e8a905d5597b72038ddb372a89826046de66687bb420e7c",
        "4dbff86cc2ca1bae1e16468a05cb9881c97f1753bce3619034898faa1aabe429955a1bf8ec483d7421fe3c1646613a59ed5441fb0f321389f77f48a879c7b1f1",
        "1e07be23c26a86ea37ea810c8ec7809352515a970e9253c26f536cfc7a9996c45c8370583e0a78fa4a90041d71a4ceab7423f19c71b9d5a3e01249f0bebd5894",
        "72ec1ef1124a45b047e8b7c75a932195135bb61de24ec0d1914042246e0aec3a2354e093d76f3048b456764346900cb130d2a4fd5dd16abb5e30bcb850dee843",
        "07e547d9586f6a73f73fbac0435ed76951218fb7d0c8d788a309d785436bbb642e93a252a954f23912547d1e8a3b5ed6e1bfd7097821233fa0538f3db854fee6",
        "91ea1245f20d46ae9a037a989f54f1f790f0a47607eeb8a14d12890cea77a1bbc6c7ed9cf205e67b7f2b8fd4c7dfd3a7a8617e45f3c463d481c7e586c39ac1ed"
    };

    for (uint8_t i = 0; test_msgs[i]; ++i)
    {
        char *expected = sha512s[i];
        char *actual = (char *) SHA512string(test_msgs[i]);

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

static void testSHA512_HMAC(void)
{
    char *keys[] = {
        "",
        "key",
        /* TODO 64-byte key */
        "==8utHoe30ASpIEmIe5Roa3#*e1$l7v8a1IeGi!gIu2OUr&$do++hiAjlAH_eTRi_Wi4dOu+l!2hiuThl7frIaclUswiuFI*GluSwiA*?ug@ASi$swOatR8A3Las0ia5",
        0
    };

    char *sha512s[][3] = {
        {
            "b936cee86c9f87aa5d3c6f2e84cb5a4239a5fe50480a6ec66b70ab5b1f4ac6730c6c515421b327ec1d69402e53dfb49ad7381eb067b338fd7b0cb22247225d47",
            "84fa5aa0279bbc473267d05a53ea03310a987cecc4c1535ff29b6d76b8f1444a728df3aadb89d4a9a6709e1998f373566e8f824a8ca93b1821f0b69bc2a2f65e",
            "09dd94478f25762dbb10e80a0ad6421b51287c2ffcee0eb497dfc46bc91b54d55c68c179bd1f62e526b6ea081f9f0c2ac52f26164f197ebb2c40eade2fe24b02"
        }, {
            "239254cd9e54392dde39bc8f975f2498cf709caa598ce3762be282d860dbea5d8034eee86f19bbdf3efdab22c6be610a8908faf2800af46243364135e9bb206e",
            "1d51a72107e0cc8efc886683ad30ea10134600bfb147d0d5faf3e95648f3d05459ba8fb541904fd1c5963730e3c2a5cd3a30ee431e225295f345fc5662487a48",
            "f1713255c459cdca4e2ffc4cae38e6d8a85b6b5a38b9d4a2c8118a154bff3bf68398d9a851a69db736714e01fb9ab3524a18e831da7198d19fa9d4aeac09d649"
        }, {
            "29689f6b79a8dd686068c2eeae97fd8769ad3ba65cb5381f838358a8045a358ee3ba1739c689c7805e31734fb6072f87261d1256995370d55725cba00d10bdd0",
            "3926a207c8c42b0c41792cbd3e1a1aaaf5f7a25704f62dfc939c4987dd7ce060009c5bb1c2447355b3216f10b537e9afa7b64a4e5391b0d631172d07939e087a",
            "c8f557f87f3d64ad7ae348657df88de6a1ef5cf7a5eec18df17608a22c294a79c2841b25376445261d869af3dfcfaeb12575c09b24c10f1c20c779bbccf898dd"
        }, {
            "03da324b27142eace203bf11a9b1d6351bd4014b1a5f145fb8201a8154760f456bba4230786ce0cd8f626fe9b547e33f6f09039f0ac7653e006bb66c194c62d4",
            "e9b1c3fb1942554fd1ce2a6f9e393046b8e8ba5bcd84ec918f764516ca7a6ac027ab0074fccefa08361d23a7e0e943e6f27c273fc8d46c446c01a1058475d188",
            "04b240cab9261193ba69344df6572c51edcda983ece6939ba7d1853e8b89d3430dadbb24f741c5af27b34646421207d98e2102e430413faefefa270cadb5bf3d"
        }, {
            "3bb0caeaaf61369f2fec184b392776feb081c348cb2ef1dc3735808d87d78a41eaee844f7998f6d46991a2e48411913bce3a7b06899080fed5abcc0bd225e353",
            "d9ac38365e5ecfbf70cd01649005343341974a2b6c0e0089aa23c9b01b08de1c40fcb9315b5809d7fa1283b25575804495a02ac4fe4b96111021196399b5d721",
            "d055d8839b7ab5d74e887c177375038430f01f247551da923f0c996a17dcf4c0d1a92f9cf5fdc28d3eefb2e8d403fed394af838a37b59b787969a3f424dcc782"
        }, {
            "efef7fbd5bdabf92b5733f34f0d2bcfaadeff1535841b7998230a73cf94d09855fc74ba8647ee7b9f57cb9f7cde6355c734b22e922b8de92551787ebcdcf4ae9",
            "7869d5de820c0bdbfa8450d4b605cf228189089c6db61bbe6973589a43ba2c6ed7b37395da01cd9b5b28421443a30cf2b5118881798d403bc0e75c9296dde1bd",
            "0415f5481ecbe4710becd353044f4afacf127ef2f50ea710ee23c3eecc8e06704a4bf08112b60fa7d54faf90310c37e2716acdc7440a3bccef01410026219435"
        }, {
            "71b506fa10cb70ee67e5c51ebf6672901bca8c1a0df9051e4e436471854c7a4f9926698c4a6ea410d64715fcc47fdf8c188e831cd737f280ea2bcbd3a55661f6",
            "a7d50f8580de6dcd1fa066deb7a64d7deab0522ac34ae59faa2775e08ac3a680518533a9a35089dadc8c8d8122f902a51264f75a68b5b591f0359b7bfcc99e7e",
            "4f84ae8bb12c9ea101cfbe5d7a7264278faea95c40bcbe4269b2b4eeb5f95552be0075125e691d88d4a226d0977612edfa722786727759b84a28b3f06e4cdd58"
        }, {
            "1de78322e11d7f8f1035c12740f2b902353f6f4ac4233ae455baccdf9f37791566e790d5c7682aad5d3ceca2feff4d3f3fdfd9a140c82a66324e9442b8af71b6",
            "b42af09057bac1e2d41708e48a902e09b5ff7f12ab428a4fe86653c73dd248fb82f948a549f7b791a5b41915ee4d1ec3935357e4e2317250d0372afa2ebeeb3a",
            "3c3245db32b1b31e7171675e6ec688252b89f0c67ac497d98bbb018cd2d44315614d6c153bc87a6d86018aed68c878effd8806d8a5c000e6c2f9c9e3f72d27d6"
        }, {
            "3bd63f0acf246ad3cb8490b1a04ac96fcc78965ab6eb818ec776f3bb2d970fdcc953999890329720658b9183d5cc5eb34ed7274be6f12976514cb25bda3fdae9",
            "451b681c334a8a24fe5baf00880443c7898d1c523db1d83bed03b2a46a960aeca5bce23efba258225f5606fdb2c93a4b99d84b4bfd09f119a0971a2dff61db1e",
            "fe25c943e31a32e18cc7c78d176a496d1b68697671edacfa29c580413448fb48fca5cfc14b6579d92daeacf13288d2c57765ff28d69862df3bd404d8865096f8"
        }
    };

    for (uint8_t i = 0; test_msgs[i]; ++i)
    {
        for (uint8_t j = 0; keys[j]; ++j)
        {
            char *expected = sha512s[i][j];
            PRINT("test msg: \"%s\"\n", test_msgs[i]);
            char *actual = (char *) HMAC_SHA512(keys[j], test_msgs[i]);

            CU_ASSERT_PTR_NOT_NULL_FATAL(actual);
            CU_ASSERT_STRING_EQUAL(actual, expected);

            if (STR_EQ(expected, actual))
            {
                PRINT("%s\n", actual);
            }
            else
            {
                fprintf(stderr, "\n");
                fprintf(stderr, "key     : -->%s<--\n", keys[j]);
                fprintf(stderr, "string  : -->%s<--\n", test_msgs[i]);
                fprintf(stderr, "expected: %s\n", expected);
                fprintf(stderr, "actual  : %s\n", actual);
            }

            free(actual);
        }
    }
}
