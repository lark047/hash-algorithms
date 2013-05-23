#include "sha.h"
#include "util.h"
#include "hmac.h"

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CUnit/Basic.h>

static void testSHA224file(void);
static void testSHA224string(void);
static void testSHA224_HMAC(void);

void testSHA224(void)
{
    testSHA224file();
    testSHA224string();
    testSHA224_HMAC();
}

static void testSHA224file(void)
{
    char *sha224s[] = {
        "15466c1039f59bb6be71a39bd532c58784bf1bb16d75f6b0c8decb8c",
        "e8c15d187727485006a83b38577d4a6dd7d6426461ac74d1bb6f6a76",
        "c84a6a460c52b0c66a0379a05f8b91012ac841d433c0634f02595848"
    };

    for (uint8_t i = 0; test_files[i]; ++i)
    {
        FILE *fp = fopen(test_files[i], "rb");

        CU_ASSERT_PTR_NOT_NULL_FATAL(fp)

        uint8_t *digest = SHA224file(fp);

        CU_ASSERT_PTR_NOT_NULL_FATAL(digest);

        char *expected = sha224s[i];
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

static void testSHA224string(void)
{
    char *sha224s[] = {
        "d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f",
        "abd37534c7d9a2efb9465de931cd7055ffdb8879563ae98078d6d6d5",
        "23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7",
        "2cb21c83ae2f004de7e81c3c7019cbcb65b71ab656b22d6d0c39b8eb",
        "45a5f72c39c5cff2522eb3429799e49e5f44b356ef926bcf390dccc2",
        "bff72b4fcb7d75e5632900ac5f90d219e05e97a7bde72e740db393d9",
        "b50aecbe4e9bb0b57bc5f3ae760a8e01db24f203fb3cdcd13148046e",
        "730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525",
        "619cba8e8e05826e9b8c519c0a5c68f4fb653e8a3d8aa04bb2c8cd4c"
    };

    for (uint8_t i = 0; test_msgs[i]; ++i)
    {
        uint8_t *digest = SHA224string(test_msgs[i]);

        CU_ASSERT_PTR_NOT_NULL_FATAL(digest);

        char *expected = sha224s[i];
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

static void testSHA224_HMAC(void)
{
    char *keys[] = {
        "",
        "key",
        /* TODO 64-byte key */
        "==8utHoe30ASpIEmIe5Roa3#*e1$l7v8a1IeGi!gIu2OUr&$do++hiAjlAH_eTRi_Wi4dOu+l!2hiuThl7frIaclUswiuFI*GluSwiA*?ug@ASi$swOatR8A3Las0ia5",
        0
    };

    char *sha224s[][3] = {
        {
            "5ce14f72894662213e2748d2a6ba234b74263910cedde2f5a9271524",
            "5aa677c13ce1128eeb3a5c01cef7f16557cd0b76d18fd557d6ac3962",
            "0c29c6b995b50454debd20cdc1ec15a7f881ebde986b633482e0d7f6"
        }, {
            "e5663b6b984184f7d8a5483a7e110dd0c0b9faea0d52eebc1cd07b46",
            "da6b634bdc14c43e4feac83189c85ee5595c67cf365098604b1d7293",
            "aaccb82da7d1d9e60137e823bd47f1030a7b99cb80dbfeec35071412"
        }, {
            "d473c456fa6aad72bbec9c6ad63ca92d8675caa0b7f451fa4b692081",
            "f524670b7e34f31467de0aa96593861cf65117d414fb2d86158d760e",
            "ed2376dcde6c51f0aa8406f1f58541e2a7d6569f1288bc6ea0560370"
        }, {
            "e001df0077cb68554889cb5330aaed89a9c83ec681c59ef5a0f9b337",
            "4252d81ae0819c9477966a456b8559f57b35461ff451d282a15005dd",
            "f35af13870ebb886e5b37ec3c6c73308ac63235c47f7d77b1ec5c7e2"
        }, {
            "dcbb7596f9164b0e9b3179c82c9d2708a58db08db4eb933d98fbeb09",
            "084223f6f37bd543e4c6893c21565a2da51337f7329e9700bf7a7baf",
            "d031b9ff513ed4cc3c356b67e86afd5aec0a5aac24b8b5ba5c2aedc6"
        }, {
            "5bce1111e177e4003d78091a53cc8dd9cf33baf8baa25aec93086830",
            "b8be09ea011a7b0950822a4e3cf758846c24d1b30438572202c7e6bd",
            "730d9e8600a4c2ada51ecd7df17eae73a2b7a40158ff1b1e099947a7"
        }, {
            "5e14eae728ecc9c9bcda6903d46b7907a665858aa3f5adbd2e409de9",
            "28793e526aaaa0aadaf4fdd51479ffe296865aeff018e43d309875f8",
            "2c588c09cec3e3a8dc0ea5a34e7220978793ae0a829cfb5b17bacff5"
        }, {
            "7db2472a551b5d5586c194a94863b6277e4bf1266ff6334b8c1d52dd",
            "88ff8b54675d39b8f72322e65ff945c52d96379988ada25639747e69",
            "a8b497cc2afdfcaa7da5856e00d664dacf4d711868031a13bc0834f9"
        }, {
            "7473554bd2d99a5fd77ab3b1fb10487fb9500a4e9ee0aeb61a02b10c",
            "45cf7e77213aff0776786d1850278d0af3e9ebaa0877c32ad26db216",
            "fc05fb73b67fc92bfcb0725d7ea30d38e787097ca320a06a5d84190a"
        }
    };

    for (uint8_t i = 0; test_msgs[i]; ++i)
    {
        for (uint8_t j = 0; keys[j]; ++j)
        {
            uint8_t *digest = SHA224hmac(keys[j], test_msgs[i]);

            CU_ASSERT_PTR_NOT_NULL_FATAL(digest);

            char *expected = sha224s[i][j];
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
