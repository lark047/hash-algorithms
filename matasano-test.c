#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>

#include <CUnit/Basic.h>
#include <CUnit/CUError.h>

#include "matasano.h"
#include "matasano-test.h"

#define FREE(ptr) do { free((void *) ptr); ptr = NULL; } while (0)

static void testEncodeDecodeBase64(void);
static void testFixedXOR(void);
static void testDecodeXOR(void);
static void testDecodeXORFromFile(void);
static void testRepeatingKeyXOR(void);
static void testBreakRepeatingKeyXOR(void);

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
            if ( CU_ADD_TEST(suite, testEncodeDecodeBase64) != NULL &&
                CU_ADD_TEST(suite, testFixedXOR) != NULL  &&
                CU_ADD_TEST(suite, testDecodeXOR) != NULL &&
                CU_ADD_TEST(suite, testDecodeXORFromFile) != NULL &&
                CU_ADD_TEST(suite, testRepeatingKeyXOR) != NULL /* &&
                CU_ADD_TEST(suite, testBreakRepeatingKeyXOR) != NULL*/)
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
    const char *base64;

    errno = 0;
    base64 = EncodeBase64(NULL);
    CU_ASSERT_PTR_NULL(base64);
    CU_ASSERT_EQUAL(EINVAL, errno);

    errno = 0;
    base64 = EncodeBase64("");
    CU_ASSERT_PTR_NOT_NULL(base64);
    CU_ASSERT_EQUAL(0, errno);
    CU_ASSERT_STRING_EQUAL(base64, "");

    errno = 0;
    base64 = EncodeBase64("e");
    CU_ASSERT_PTR_NULL(base64);
    CU_ASSERT_EQUAL(EINVAL, errno);

    char *input[] = {
        "2e",                                /* "."     */
        "6f23",                              /* "o#"    */
        "355a3f",                            /* "5Z?"   */
        "79475e22",                          /* "yG^\"" */
        "49276d206b696c6c696e6720796f7572"
        "20627261696e206c696b65206120706f"
        "69736f6e6f7573206d757368726f6f6d",  /* "I'm killing your brain like a poisonous mushroom" */
        "4d616e2069732064697374696e677569"
        "736865642c206e6f74206f6e6c792062"
        "792068697320726561736f6e2c206275"
        "7420627920746869732073696e67756c"
        "61722070617373696f6e2066726f6d20"
        "6f7468657220616e696d616c732c2077"
        "686963682069732061206c757374206f"
        "6620746865206d696e642c2074686174"
        "2062792061207065727365766572616e"
        "6365206f662064656c6967687420696e"
        "2074686520636f6e74696e7565642061"
        "6e6420696e6465666174696761626c65"
        "2067656e65726174696f6e206f66206b"
        "6e6f776c656467652c20657863656564"
        "73207468652073686f72742076656865"
        "6d656e6365206f6620616e7920636172"
        "6e616c20706c6561737572652e"         /* see https://en.wikipedia.org/wiki/Base64#Examples */
    };

    char *expected[] = {
        "Lg==",
        "byM=",
        "NVo/",
        "eUdeIg==",
        "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t",
        "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1"
        "dCBieSB0aGlzIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3"
        "aGljaCBpcyBhIGx1c3Qgb2YgdGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFu"
        "Y2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGludWVkIGFuZCBpbmRlZmF0aWdhYmxl"
        "IGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRoZSBzaG9ydCB2ZWhl"
        "bWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4="
    };

    for (uint8_t i = 0; i < LENGTH(input); ++i)
    {
        errno = 0;

        base64 = EncodeBase64(input[i]);
        // print_d("%s\n", base64);

        CU_ASSERT_PTR_NOT_NULL_FATAL(base64);
        CU_ASSERT_EQUAL(0, errno);

        if (!STR_EQ(expected[i], base64))
        {
            fprintf(stderr, "Expected: %s Actual: %s\n", expected[i], base64);
        }

        CU_ASSERT_STRING_EQUAL(base64, expected[i]);
        FREE(base64);
    }
}

static void testDecodeBase64(void)
{
    const uint8_t *hex;

    errno = 0;
    hex = DecodeBase64(NULL);
    CU_ASSERT_PTR_NULL(hex);
    CU_ASSERT_EQUAL(EINVAL, errno);

    errno = 0;
    hex = DecodeBase64("");
    CU_ASSERT_PTR_NULL(hex);
    CU_ASSERT_EQUAL(0, errno);

    errno = 0;
    hex = DecodeBase64("d4=");
    CU_ASSERT_PTR_NULL(hex);
    CU_ASSERT_EQUAL(EINVAL, errno);

    char *input[] = {
        "Lg==",                              /* "."     */
        "byM=",                              /* "o#"    */
        "NVo/",                              /* "5Z?"   */
        "eUdeIg==",                          /* "yG^\"" */
        "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBs"
        "aWtlIGEgcG9pc29ub3VzIG11c2hyb29t",  /* I'm killing your brain like a poisonous mushroom */
        "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5v"
        "dCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1"
        "dCBieSB0aGlzIHNpbmd1bGFyIHBhc3Np"
        "b24gZnJvbSBvdGhlciBhbmltYWxzLCB3"
        "aGljaCBpcyBhIGx1c3Qgb2YgdGhlIG1p"
        "bmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFu"
        "Y2Ugb2YgZGVsaWdodCBpbiB0aGUgY29u"
        "dGludWVkIGFuZCBpbmRlZmF0aWdhYmxl"
        "IGdlbmVyYXRpb24gb2Yga25vd2xlZGdl"
        "LCBleGNlZWRzIHRoZSBzaG9ydCB2ZWhl"
        "bWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVh"
        "c3VyZS4="                           /* see https://en.wikipedia.org/wiki/Base64#Examples */
    };

    uint8_t *expected[6];
    uint8_t bytes_len[] = { 3, 3, 3, 6, 48, 240 };

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

    // TODO expected[5] = malloc(bytes_len[5] * sizeof *expected);
    // 4d 61 6e 20 69 73 20 64 69 73 74 69 6e 67 75 69 73 68 65 64 2c 20 6e 6f 74 20 6f 6e 6c 79 20 62 79 20 68 69 73 20 72 65 61 73 6f 6e 2c 20 62 75 74 20 62 79 20 74 68 69 73 20 73 69 6e 67 75 6c 61 72 20 70 61 73 73 69 6f 6e 20 66 72 6f 6d 20 6f 74 68 65 72 20 61 6e 69 6d 61 6c 73 2c 20 77 68 69 63 68 20 69 73 20 61 0d 0a 6c 75 73 74 20 6f 66 20 74 68 65 20 6d 69 6e 64 2c 20 74 68 61 74 20 62 79 20 61 20 70 65 72 73 65 76 65 72 61 6e 63 65 20 6f 66 20 64 65 6c 69 67 68 74 20 69 6e 20 74 68 65 20 63 6f 6e 74 69 6e 75 65 64 20 61 6e 64 20 69 6e 64 65 66 61 74 69 67 61 62 6c 65 20 67 65 6e 65 72 61 74 69 6f 6e 20 6f 66 20 6b 6e 6f 77 6c 65 64 67 65 2c 20 65 78 63 65 65 64 73 20 74 68 65 20 73 68 6f 72 74 20 76 65 68 65 6d 65 6e 63 65 20 6f 66 20 61 6e 79 20 63 61 72 6e 61 6c 20 70 6c 65 61 73 75 72 65 2e

    for (uint8_t i = 0; i < 5; ++i)
    {
        errno = 0;

        hex = DecodeBase64(input[i]);

        CU_ASSERT_PTR_NOT_NULL_FATAL(hex);
        CU_ASSERT_EQUAL(0, errno);

        for (size_t j = 0; j < bytes_len[i]; ++j)
        {
            // print_d("%u: comparing %u (expected) and %u (actual)\n", j, expected[i][j], hex[j]);
            if (expected[i][j] != hex[j])
            {
                fprintf(stderr, "%zu: expected: 0x%0x Actual: 0x%0x\n", j, expected[i][j], hex[j]);
            }

            CU_ASSERT_EQUAL(hex[j], expected[i][j]);
        }

        FREE(expected[i]);
        FREE(hex);
    }
}

static void testFixedXOR(void)
{
    const uint8_t *result;

    result = FixedXOR(NULL, NULL, 0);
    CU_ASSERT_EQUAL(errno, EINVAL);
    CU_ASSERT_PTR_NULL(result);

    uint8_t *hex1 = malloc(1 * sizeof *hex1), *hex2;
    hex1[0] = 0xff;

    result = FixedXOR(hex1, NULL, 1);
    CU_ASSERT_EQUAL(errno, EINVAL);
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

    result = FixedXOR(hex1, hex2, strlen(msg1) / 2);

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
    const uint8_t len = 34;
    const uint8_t hex[] = {
        0x1b, 0x37, 0x37, 0x33, 0x31, 0x36, 0x3f, 0x78,
        0x15, 0x1b, 0x7f, 0x2b, 0x78, 0x34, 0x31, 0x33,
        0x3d, 0x78, 0x39, 0x78, 0x28, 0x37, 0x2d, 0x36,
        0x3c, 0x78, 0x37, 0x3e, 0x78, 0x3a, 0x39, 0x3b,
        0x37, 0x36
    };

    const struct result *r = DecodeXOR(hex, len);
    CU_ASSERT_PTR_NOT_NULL_FATAL(r);
    CU_ASSERT_EQUAL(r->key.c, 'X');
    CU_ASSERT_STRING_EQUAL(r->text, "Cooking MC's like a pound of bacon");

    free((void *) r->hex);
    free(r->text);
    FREE(r);

#if 0
    const uint8_t hex2[] = {
        0x1c, 0x3d, 0xf1, 0x13, 0x53, 0x21, 0xa8, 0xe9,
        0x24, 0x1a, 0x56, 0x07, 0xf8, 0x30, 0x5d, 0x57,
        0x1a, 0xa5, 0x46, 0x00, 0x1e, 0x32, 0x54, 0x55,
        0x5a, 0x11, 0x51, 0x19, 0x24
    };

    const struct result *r2 = DecodeXOR(hex2, 29);
    CU_ASSERT_PTR_NOT_NULL_FATAL(r2);

    print_d("key '%c' (0x%02x) gives score %f and text \"%s\"\n", r2->key, r2->key, r2->score, r2->text);

    free((void *) r2->hex);
    free(r2->text);
    FREE(r2);
#endif
}

static void testDecodeXORFromFile(void)
{
    FILE *fp = fopen("gistfile1.txt", "r");
    CU_ASSERT_PTR_NOT_NULL_FATAL(fp);

    const struct result *r = DecodeXORFromFile(fp);
    CU_ASSERT_PTR_NOT_NULL_FATAL(r);

    char *p = strchr((char *) r->text, '\n');
    if (p) { *p = '\0'; }

    print_d("key = '%c' (0x%02x)\n", r->key.c, r->key.c);
    print_d("score = %.4f\n", r->score);
    print_d("text = \"%s\"\n", r->text);

    CU_ASSERT_EQUAL(r->key.c, 0x35);
    CU_ASSERT_STRING_EQUAL(r->text, "Now that the party is jumping");

    free((void *) r->hex);
    free(r->text);
    FREE(r);

    fclose(fp);
}

static void testRepeatingKeyXOR(void)
{
    const char * const msg = "Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal";
    const char * const key = "ICE";

    const uint8_t *result = RepeatingKeyXOR((uint8_t *) msg, strlen(msg), (uint8_t *) key, strlen(key));
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
        // print_d("result[%2zu] = 0x%02x\n", i, result[i]);
        snprintf((char *) actual + 2 * i, 3, "%02x", result[i]);
        // print_d("actual[%2zu] = %c%c\n", 2 * i, (unsigned) actual[2 * i], (unsigned) actual[2 * i + 1]);
    }

    actual[length] = '\0';
    print_d("actual is \"%s\"\n", actual);

    CU_ASSERT_STRING_EQUAL(actual, expected);

    FREE(result);
    FREE(actual);
}

static void testBreakRepeatingKeyXOR(void)
{
    FILE *fp = fopen("gistfile2.txt", "r");
    CU_ASSERT_PTR_NOT_NULL_FATAL(fp);

    const struct result *result = BreakRepeatingKeyXOR(fp);
    CU_ASSERT_PTR_NOT_NULL_FATAL(result);

    uint8_t expected_key[] = {
        0x54, 0x65, 0x72, 0x6d, 0x69, 0x6e, 0x61, 0x74, 0x6f, 0x72, 0x20, 0x58, 0x3a, 0x20,
        0x42, 0x72, 0x69, 0x6e, 0x67, 0x20, 0x74, 0x68, 0x65, 0x20, 0x6e, 0x6f, 0x69, 0x73,
        0x65
    };

    CU_ASSERT_EQUAL(result->keysize, LENGTH(expected_key));
    CU_ASSERT_PTR_NOT_NULL_FATAL(result->key.ptr);
    CU_ASSERT_PTR_NOT_NULL_FATAL(result->hex);
    CU_ASSERT_PTR_NOT_NULL_FATAL(result->text);

    for (uint8_t i = 0; i < result->keysize; ++i)
    {
        if (expected_key[i] != result->key.ptr[i])
        {
            fprintf(stderr, "Expected: '%c' actual: '%c' at index %" PRIu8 "\n", expected_key[i], result->key.ptr[i], i);
        }

        CU_ASSERT_EQUAL(result->key.ptr[i], expected_key[i]);
    }

    char *expected_text = "I'm back and I'm ringin' the bell \nA rockin' on the mike while the fly girls yell \nIn ecstasy in the"
        " back of me \nWell that's my DJ Deshay cuttin' all them Z's \nHittin' hard and the girlies goin' crazy \nVanilla's on th"
        "e mike, man I'm not lazy. \n\nI'm lettin' my drug kick in \nIt controls my mouth and I begin \nTo just let it flow, let "
        "my concepts go \nMy posse's to the side yellin', Go Vanilla Go! \n\nSmooth 'cause that's the way I will be \nAnd if you "
        "don't give a damn, then \nWhy you starin' at me \nSo get off 'cause I control the stage \nThere's no dissin' allowed \nI"
        "'m in my own phase \nThe girlies sa y they love me and that is ok \nAnd I can dance better than any kid n' play \n\nStag"
        "e 2 -- Yea the one ya' wanna listen to \nIt's off my head so let the beat play through \nSo I can funk it up and make it"
        "sound good \n1-2-3 Yo -- Knock on some wood \nFor good luck, I like my rhymes atrocious \nSupercalafragilisticexpialidoc"
        "ious \nI'm an effect and that you can bet \nI can take a fly girl and make her wet. \n\nI'm like Samson -- Samson to Del"
        "ilah \nThere's no denyin', You can try to hang \nBut you'll keep tryin' to get my style \nOver and over, practice makes "
        "perfect \nBut not if you're a loafer. \n\nYou'll get nowhere, no place, no time, no girls \nSoon -- Oh my God, homebody,"
        " you probably eat \nSpaghetti with a spoon! Come on and say it! \n\nVIP. Vanilla Ice yep, yep, I'm comin' hard like a rh"
        "ino \nIntoxicating so you stagger like a wino \nSo punks stop trying and girl stop cryin' \nVanilla Ice is sellin' and y"
        "ou people are buyin' \n'Cause why the freaks are jockin' like Crazy Glue \nMovin' and groovin' trying to sing along \nAl"
        "l through the ghetto groovin' this here song \nNow you're amazed by the VIP posse. \n\nSteppin' so hard like a German Na"
        "zi \nStartled by the bases hittin' ground \nThere's no trippin' on mine, I'm just gettin' down \nSparkamatic, I'm hangin"
        "' tight like a fanatic \nYou trapped me once and I thought that \nYou might have it \nSo step down and lend me your ear "
        "\n'89 in my time! You, '90 is my year. \n\nYou're weakenin' fast, YO! and I can tell it \nYour body's gettin' hot, so, s"
        "o I can smell it \nSo don't be mad and don't be sad \n'Cause the lyrics belong to ICE, You can call me Dad \nYou're pitc"
        "hin' a fit, so step back and endure \nLet the witch doctor, Ice, do the dance to cure \nSo come up close and don't be sq"
        "uare \nYou wanna battle me -- Anytime, anywhere \n\nYou thought that I was weak, Boy, you're dead wrong \nSo come on, ev"
        "erybody and sing this song \n\nSay -- Play that funky music Say, go white boy, go white boy go \nplay that funky music G"
        "o white boy, go white boy, go \nLay down and boogie and play that funky music till you die. \n\nPlay that funky music Co"
        "me on, Come on, let me hear \nPlay that funky music white boy you say it, say it \nPlay that funky music A little louder"
        " now \nPlay that funky music, white boy Come on, Come on, Come on \nPlay that funky music \nn";

    CU_ASSERT_STRING_EQUAL(result->text, expected_text);

    free(result->key.ptr);
    free((void *) result->hex);
    free((void *) result);

    fclose(fp);
}
