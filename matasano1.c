#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG
#  define print_d(f, ...)  printf("[debug] " f, __VA_ARGS__)
#else
#  define print_d(f, ...)  /* NO-OP */
#endif

#define SWAP(T,a,b)  do { T tmp = a; a = b; b = tmp; } while (0)

#define PADDING0  ""
#define PADDING1  "="
#define PADDING2  (PADDING1 PADDING1)
#define CHARS     "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

/**
 * Input: (0x)49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d
 * Expected output: SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t
 */

static void to_base64(const uint8_t *, const char *, char *);

/* TODO param1 should be uint8_t * */
void EncodeBase64(const char *, char * const);
void DecodeBase64(const char * /* TODO , const uint8_t * */);

int main(int argc, char **argv)
{
    int rc = EXIT_FAILURE;

    if (argc == 2)
    {
        char *base64 = malloc(2 * ceil(strlen(argv[1]) / 3) + 1);

        if (base64)
        {
            EncodeBase64(argv[1], base64);

            printf("%s\n", base64);

            free(base64);
            rc = EXIT_SUCCESS;
        }
    }

    return rc;
}

void to_base64(const uint8_t *bytes, const char *padding, char *base64)
{
    char *p = base64;
    const size_t padding_length = strlen(padding); /* 0, 1, or 2 */

    /* 0x49276d: */

    /* 01001001 00100111 01101101 */
    /* ^^^^^^                     */
    uint8_t index = bytes[0] >> 2;
    print_d("output 0: 0x%02x\n", index);
    *p++ = CHARS[index];

    /* 01001001 00100111 01101101 */
    /*       ^^ ^^^^              */
    index  = (bytes[0] & 0x03) << 4;
    index |= (bytes[1] & 0xf0) >> 4;
    print_d("output 1: 0x%02x\n", index);
    *p++ = CHARS[index];

    /* 01001001 00100111 01101101 */
    /*              ^^^^ ^^       */
    index  = (bytes[1] & 0x0f) << 2;
    index |= (bytes[2] & 0xc0) >> 6;
    print_d("output 2: 0x%02x\n", index);
    *p++ = CHARS[index];

    /* 01001001 00100111 01101101 */
    /*                     ^^^^^^ */
    index  = bytes[2] & 0x3f;
    print_d("output 3: 0x%02x\n", index);
    *p++ = CHARS[index];

    snprintf(base64 + 4 - padding_length, padding_length + 1, "%s", padding);
}

void EncodeBase64(const char *msg, char * const base64)
{
    /* operate on 3 bytes at a time */
    uint8_t bytes[3];

    for (uint8_t i = 0; i < strlen(msg); i += 6)
    {
        print_d("strlen(msg) - %u = %u\n", i, strlen(msg) - i);
        const char *padding;

        sscanf(msg + i, "%6x", (unsigned *) bytes);

        print_d("before byte 1 = 0x%02x\n", bytes[0]);
        print_d("before byte 2 = 0x%02x\n", bytes[1]);
        print_d("before byte 3 = 0x%02x\n", bytes[2]);

        switch (strlen(msg) - i)
        {
            case 2:
                padding = PADDING2;
                break;
            case 4:
                SWAP(uint8_t, bytes[0], bytes[1]);
                padding = PADDING1;
                break;
            default:
                SWAP(uint8_t, bytes[0], bytes[2]);
                padding = PADDING0;
                break;
        }

        print_d("after byte 1 = 0x%02x\n", bytes[0]);
        print_d("after byte 2 = 0x%02x\n", bytes[1]);
        print_d("after byte 3 = 0x%02x\n", bytes[2]);

        char encoded[4 + 1];
        to_base64(bytes, padding, encoded);

        snprintf(base64 + 4 * i, 5, "%4s", encoded);
    }
}

void DecodeBase64(const char *base64 /* TODO , const uint8_t *msg */)
{
}

#if 0

a n y   c a r n a l   p l e a s u r e .
616e79206361726e616c20706c6561737572652e

p l e a s u r e .
706c6561737572652e

#endif
