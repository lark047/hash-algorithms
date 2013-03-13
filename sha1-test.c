#include <stdio.h>
#include <stdint.h>

#include <CUnit/Basic.h>

extern char *SHA1string(const char *);

void testSHA1(void)
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

    const char *sha1s[] = {
        "da39a3ee5e6b4b0d3255bfef95601890afd80709",
        "86f7e437faa5a7fce15d1ddcb9eaeaea377667b8",
        "a9993e364706816aba3e25717850c26c9cd0d89d",
        "c12252ceda8be8994d5fa0290a47231c1d16aae3",
        "32d10c7b8cf96570ca04ce37f2a19d84240d3a89",
        "761c457bf73b14d27e9e9265c46f4b4dda11f940",
        "50abf5706a150990a08b2c5ea40fa0e585554732",
        "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12",
        "408d94384216f890ff7a0c3528e8bed1e0b01621"
    };
    const char *actual, *expected;

    for (uint8_t i = 0; msgs[i]; ++i)
    {
        expected = sha1s[i];
        actual = (const char *) SHA1string(msgs[i]);

        CU_ASSERT_STRING_EQUAL(actual, expected);

        if (strcmp(expected, actual))
        {
            fprintf(stderr, "\n");
            fprintf(stderr, "%s\n", msgs[i]);
            fprintf(stderr, "expected: %s\n", expected);
            fprintf(stderr, "actual  : %s\n", actual);
        }
    }
}
