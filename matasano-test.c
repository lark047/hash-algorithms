#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#include <CUnit/Basic.h>
#include <CUnit/CUError.h>

#include "matasano.h"
#include "matasano-test.h"

#define FREE(ptr) do { free((void *) ptr); ptr = NULL; } while (0)
#define SIZE(a)                         (sizeof (a) / sizeof *(a))

static void testEncodeDecodeBase64(void);
static void testFixedXOR(void);
static void testDecodeXOR(void);
static void testDecodeXORFromFile(void);
static void testRepeatingKeyXOR(void);

const char *RunTests(void)
{
    /* initialize the CUnit test registry */
    if (CUE_SUCCESS == CU_initialize_registry())
    {
        CU_pSuite suite = NULL;

        /* add a suite to the registry */
        if ((suite = CU_add_suite("Matasano Crypto Challenge #1", NULL, NULL)) != NULL)
        {
            /* add the tests to the suite */
            if (/* CU_ADD_TEST(suite, testEncodeDecodeBase64) != NULL &&
                CU_ADD_TEST(suite, testFixedXOR) != NULL &&
                CU_ADD_TEST(suite, testDecodeXOR) != NULL &&
                CU_ADD_TEST(suite, testDecodeXORFromFile) != NULL && */
                CU_ADD_TEST(suite, testRepeatingKeyXOR))
            {
                /* Run all tests using the CUnit Basic interface */
                CU_basic_set_mode(CU_BRM_VERBOSE);
                CU_basic_run_tests();
            }
        }
    }

    CU_cleanup_registry();

    return CU_get_error() == CUE_SUCCESS ? "" : CU_get_error_msg();
}

static void testEncodeBase64(void);
static void testDecodeBase64(void);

static void testEncodeDecodeBase64(void)
{
    testEncodeBase64();
    testDecodeBase64();
}

static void testEncodeBase64(void)
{
    char *base64 = NULL;

    EncodeBase64(NULL, base64);
    CU_ASSERT_PTR_NULL(base64);

    EncodeBase64("", base64);
    CU_ASSERT_PTR_NULL(base64);

    /* TODO garbage input */
    // EncodeBase64("", base64);

    char *input[] = {
        "2e",               /* "."     */
        "6f23",             /* "o#"    */
        "355a3f",           /* "5Z?"   */
        "79475e22",         /* "yG^\"" */
        "49276d206b696c6c"
        "696e6720796f7572"
        "20627261696e206c"
        "696b65206120706f"
        "69736f6e6f757320"
        "6d757368726f6f6d"  /* I'm killing your brain like a poisonous mushroom */
    };

    char *expected[] = {
        "Lg==",
        "byM=",
        "NVo/",
        "eUdeIg==",
        "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t"
    };

    for (uint8_t i = 0; i < SIZE(input); ++i)
    {
        base64 = malloc(2 * ceil(strlen(input[i]) / 3) + 1);
        EncodeBase64(input[i], base64);
        CU_ASSERT_PTR_NOT_NULL_FATAL(base64);
        CU_ASSERT_STRING_EQUAL(base64, expected[i]);

        if (!STR_EQ(expected[i], base64))
        {
            fprintf(stderr, "Expected: %s Actual: %s\n", expected[i], base64);
        }

        FREE(base64);
    }
}

static void testDecodeBase64(void)
{
    print_d("%s\n", "*****************************************************");
    uint8_t *hex = NULL;

    DecodeBase64(NULL, hex);
    CU_ASSERT_PTR_NULL(hex);

    DecodeBase64("", hex);
    CU_ASSERT_PTR_NULL(hex);

    char *input[] = {
        "Lg==",                                                             /* "."     */
        "byM=",                                                             /* "o#"    */
        "NVo/",                                                             /* "5Z?"   */
        "eUdeIg==",                                                         /* "yG^\"" */
        "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t"  /* I'm killing your brain like a poisonous mushroom */
    };

    uint8_t *expected[5];
    uint8_t bytes_len[] = { 3, 3, 3, 6, 48 };

    expected[0] = malloc(bytes_len[0] * sizeof *expected);
    expected[0][0] = 0x2e;
    expected[0][1] = 0x00;
    expected[0][2] = 0x00;

    expected[1] = malloc(bytes_len[1] * sizeof *expected);
    expected[1][0] = 0x6f;
    expected[1][1] = 0x23;
    expected[1][2] = 0x00;

    expected[2] = malloc(bytes_len[2] * sizeof *expected);
    expected[2][0] = 0x35;
    expected[2][1] = 0x5a;
    expected[2][2] = 0x3f;

    expected[3] = malloc(bytes_len[3] * sizeof *expected);
    expected[3][0] = 0x79;
    expected[3][1] = 0x47;
    expected[3][2] = 0x5e;
    expected[3][3] = 0x22;
    expected[3][4] = 0x00;
    expected[3][5] = 0x00;

    expected[4] = malloc(bytes_len[4] * sizeof *expected);
    expected[4][ 0] = 0x49; expected[4][ 1] = 0x27; expected[4][ 2] = 0x6d; expected[4][ 3] = 0x20;
    expected[4][ 4] = 0x6b; expected[4][ 5] = 0x69; expected[4][ 6] = 0x6c; expected[4][ 7] = 0x6c;
    expected[4][ 8] = 0x69; expected[4][ 9] = 0x6e; expected[4][10] = 0x67; expected[4][11] = 0x20;
    expected[4][12] = 0x79; expected[4][13] = 0x6f; expected[4][14] = 0x75; expected[4][15] = 0x72;
    expected[4][16] = 0x20; expected[4][17] = 0x62; expected[4][18] = 0x72; expected[4][19] = 0x61;
    expected[4][20] = 0x69; expected[4][21] = 0x6e; expected[4][22] = 0x20; expected[4][23] = 0x6c;
    expected[4][24] = 0x69; expected[4][25] = 0x6b; expected[4][26] = 0x65; expected[4][27] = 0x20;
    expected[4][28] = 0x61; expected[4][29] = 0x20; expected[4][30] = 0x70; expected[4][31] = 0x6f;
    expected[4][32] = 0x69; expected[4][33] = 0x73; expected[4][34] = 0x6f; expected[4][35] = 0x6e;
    expected[4][36] = 0x6f; expected[4][37] = 0x75; expected[4][38] = 0x73; expected[4][39] = 0x20;
    expected[4][40] = 0x6d; expected[4][41] = 0x75; expected[4][42] = 0x73; expected[4][43] = 0x68;
    expected[4][44] = 0x72; expected[4][45] = 0x6f; expected[4][46] = 0x6f; expected[4][47] = 0x6d;

    for (uint8_t i = 0; i < SIZE(input); ++i)
    {
        hex = malloc((3 * strlen(input[i]) / 4 + 1) * sizeof *hex);
        print_d("3 * %zu / 4) + 1 = %d\n", strlen(input[i]), (int) (3 * ceil(bytes_len[i] / 4) + 1));
        print_d("allocated %zu bytes\n", 3 * strlen(input[i]) / 4 + 1);
        DecodeBase64(input[i], hex);
        CU_ASSERT_PTR_NOT_NULL_FATAL(hex);

        for (uint8_t j = 0; j < bytes_len[i]; ++j)
        {
            // print_d("%u: comparing %u (expected) and %u (actual)\n", j, expected[i][j], hex[j]);
            CU_ASSERT_EQUAL(hex[j], expected[i][j]);

            if (expected[i][j] != hex[j])
            {
                fprintf(stderr, "%u: expected: 0x%0x Actual: 0x%0x\n", j, expected[i][j], hex[j]);
            }
        }

        FREE(expected[i]);
        FREE(hex);
    }
}

static void testFixedXOR(void)
{
    uint8_t *result;
    print_d("%s\n", "");

    result = (uint8_t *) FixedXOR(NULL, NULL, 0);
    CU_ASSERT_PTR_NULL(result);

    uint8_t *hex1 = malloc(1 * sizeof *hex1), *hex2;
    hex1[0] = 0xff;

    result = (uint8_t *) FixedXOR(hex1, NULL, 1);
    CU_ASSERT_PTR_NULL(result);
    FREE(hex1);

    char *msg1 = "1c0111001f010100061a024b53535009181c";
    char *msg2 = "686974207468652062756c6c277320657965";

    hex1 = malloc(strlen(msg1) * sizeof *hex1 / 2);
    hex2 = malloc(strlen(msg2) * sizeof *hex2 / 2);

    CU_ASSERT_PTR_NOT_NULL_FATAL(hex1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(hex2);

    StringToHex(msg1, hex1);
    StringToHex(msg2, hex2);

    result = (uint8_t *) FixedXOR(hex1, hex2, strlen(msg1) / 2);

    const uint8_t expected[] = {
        0x74, 0x68, 0x65, 0x20, 0x6b, 0x69, 0x64, 0x20, 0x64,
        0x6f, 0x6e, 0x27, 0x74, 0x20, 0x70, 0x6c, 0x61, 0x79
    };

    for (uint8_t i = 0; i < strlen(msg1) / 2; ++i)
    {
        CU_ASSERT_EQUAL(result[i], expected[i]);
    }

    FREE(hex1);
    FREE(hex2);
    FREE(result);

}

static void testDecodeXOR(void)
{
    const char *msg = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";

    uint8_t *hex = malloc(strlen(msg) * sizeof *hex / 2);
    CU_ASSERT_PTR_NOT_NULL_FATAL(hex);

    StringToHex(msg, hex);

    struct result *result = DecodeXOR(hex, strlen(msg) / 2);

    print_d("key = 0x%02x\n", result->key);
    print_d("text = \"%s\"\n", result->text);

    CU_ASSERT_STRING_EQUAL("Cooking MC's like a pound of bacon", result->text);

    FREE(hex);
    FREE(result->hex);
    FREE(result->text);
    FREE(result);
}

static void testDecodeXORFromFile(void)
{
    FILE *fp = fopen("gistfile1.txt", "r");
    CU_ASSERT_PTR_NOT_NULL_FATAL(fp);

    struct result *result = DecodeXORFromFile(fp);
    CU_ASSERT_PTR_NOT_NULL_FATAL(result);

    char *p = strchr((char *) result->text, '\n');
    if (p) { *p = '\0'; }

    print_d("key = '%c' (0x%02x)\n", result->key, result->key);
    print_d("score = %.4f\n", result->score);
    print_d("text = \"%s\"\n", result->text);

    CU_ASSERT_EQUAL(result->key, 0x35);
    CU_ASSERT_STRING_EQUAL(result->text, "Now that the party is jumping");

    FREE(result->hex);
    FREE(result->text);
    FREE(result);

    fclose(fp);
}

static void testRepeatingKeyXOR(void)
{
    const char * const msg = "Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal";
    const char * const key = "ICE";

    uint8_t *result = RepeatingKeyXOR(msg, key);
    CU_ASSERT_PTR_NOT_NULL_FATAL(result);

    const char *expected = "0b3637272a2b2e63622c2e69692a2369"
                           "3a2a3c6324202d623d63343c2a262263"
                           "24272765272a282b2f20430a652e2c65"
                           "2a3124333a653e2b2027630c692b2028"
                           "3165286326302e27282f";

    const size_t length = strlen(expected);
    unsigned char *actual = malloc(length + 1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(actual);

    for (size_t i = 0; i < length / 2; ++i)
    {
        print_d("result[%2zu] = 0x%02x\n", i, result[i]);
        snprintf((char *) actual + 2 * i, 3, "%02x", result[i]);
        print_d("actual[%2zu] = %c%c\n", 2 * i, (unsigned) actual[2 * i], (unsigned) actual[2 * i + 1]);
    }

    actual[length] = '\0';
    print_d("actual is \"%s\"\n", actual);

    CU_ASSERT_STRING_EQUAL(actual, expected);

    FREE(result);
    FREE(actual);
}
