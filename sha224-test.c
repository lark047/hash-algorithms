#include "sha.h"
#include "util.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <CUnit/Basic.h>

#if 0
extern uint8_t *SHA224file(FILE *);
#endif
extern uint8_t *SHA224string(const char *);

extern const char *test_files[];
extern const char *test_msgs[];

#if 0
static void testSHA224file(void);
#endif
static void testSHA224string(void);

void testSHA224(void)
{
    /* TODO can't find an online SHA224 generator */
#if 0
    testSHA224file();
#endif
    testSHA224string();
}

#if 0
void testSHA224file(void)
{
    const char sha224s[] = {
        "",
        ""
    };

    for (uint8_t i = 0; test_files[i]; ++i)
    {
        FILE *fp = fopen(test_files[i], "rb");

        CU_ASSERT_PTR_NOT_NULL_FATAL(fp)

        const char *expected = sha224s[i];
        const char *actual = (const char *) SHA224file(fp);

        CU_ASSERT_STRING_EQUAL(actual, expected);

        if (strcmp(expected, actual))
        {
            fprintf(stderr, "\n");
            fprintf(stderr, "file    : %s\n", test_files[i]);
            fprintf(stderr, "expected: %s\n", expected);
            fprintf(stderr, "actual  : %s\n", actual);
        }
        else
        {
            PRINT("%s\n", actual);
        }

        fclose(fp);
    }
}
#endif

void testSHA224string(void)
{
    const char *sha224s[] = {
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
        const char *expected = sha224s[i];
        const char *actual = (const char *) SHA224string(test_msgs[i]);

        CU_ASSERT_STRING_EQUAL(actual, expected);

        if (strcmp(expected, actual))
        {
            fprintf(stderr, "\n");
            fprintf(stderr, "string  : -->%s<--\n", test_msgs[i]);
            fprintf(stderr, "expected: %s\n", expected);
            fprintf(stderr, "actual  : %s\n", actual);
        }
        else
        {
            PRINT("%s\n", actual);
        }
    }
}
