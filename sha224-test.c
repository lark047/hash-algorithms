#include "sha.h"
#include "util.h"

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CUnit/Basic.h>

static void testSHA224file(void);
static void testSHA224string(void);

void testSHA224(void)
{
    testSHA224file();
    testSHA224string();
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
