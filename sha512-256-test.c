#include <stdio.h>
#include <stdint.h>

#include <CUnit/Basic.h>

extern char *SHA512256string(const char *);

void testSHA512256(void)
{
    const char *msgs[] = {
        "",
        "a",
        "abc",
        "message digest",
        "abcdefghijklmnopqrstuvwxyz",
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
        "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
        "The quick brown fox jumps over the lazy dog",
        "The quick brown fox jumps over the lazy dog.",
        0
    };

    /* TODO i can't find a generator */
    const char *sha512256s[] = {
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        ""
    };
    const char *actual, *expected;

    for (uint8_t i = 0; msgs[i]; ++i)
    {
        expected = sha512256s[i];
        actual = (const char *) SHA512256string(msgs[i]);

        CU_ASSERT_STRING_EQUAL(actual, expected);

        if (strcmp(expected, actual))
        {
            fprintf(stdout, "\n");
            fprintf(stdout, "string  : -->%s<--\n", msgs[i]);
            fprintf(stdout, "expected: %s\n", expected);
            fprintf(stdout, "actual  : %s\n", actual);
        }
    }
}
