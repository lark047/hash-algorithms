#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matasano.h"

#define SWAP(T,a,b)  do { T tmp = a; a = b; b = tmp; } while (0)

#define PADDING0  ""
#define PADDING1  "="
#define PADDING2  (PADDING1 PADDING1)
#define CHARS     "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

#ifdef DEBUG
# define CHECK(b) (b ? "true" : "false")
#endif

/**
 * Input: (0x)49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d
 * Expected output: SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t
 */

static void to_base64(const uint8_t *bytes, const char *padding, char *base64)
{
    char *p = base64;
    const size_t padding_length = strlen(padding); /* 0, 1, or 2 */
    print_d("padding length: %zu\n", padding_length);

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

void EncodeBase64(const char * const msg, char * const base64)
{
    if (msg == NULL)
    {
        return;
    }

    /* operate on 3 bytes at a time */
    uint8_t bytes[3];

    // TODO if strlen(msg) % 2 != 0 error

    for (uint8_t i = 0; i < strlen(msg); i += 6)
    {
        print_d("strlen(msg) - %u = %zu\n", i, strlen(msg) - i);
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
        print_d("encoded: %s\n", encoded);

        snprintf(base64 + i * 2/3, 5, "%4s", encoded);
    }
}

void DecodeBase64(const char * const msg, uint8_t * const hex)
{
    if (msg == NULL)
    {
        return;
    }

    /* operate on 4 encoded chars at a time */
    char encoded[4];

    // TODO if strlen(msg) % 4 != 0 error
    print_d("strlen(msg) = %zu, %s\n", strlen(msg), (strlen(msg) % 4 == 0 ? "LENGTH OK" : "INVALID LENGTH"));

#ifdef DEBUG
    const uint32_t decode_length = 3 * strlen(msg) / 4;
#endif

    for (uint32_t i = 0; i < strlen(msg); i += 4)
    {
        /* SSdt... */
        // memset(encoded, 0, sizeof encoded);
        memcpy(encoded, msg + i, sizeof encoded);
        print_d("next 4 encoded bytes: %c%c%c%c\n", encoded[0], encoded[1], encoded[2], encoded[3]);

        /* 010010 010010 011101 101101 */
        /* ^^^^^^ ^^                   */
        char *p = strchr(CHARS, encoded[0]);
        print_d("check %2u < %u: %s\n", 3 * i / 4, decode_length, CHECK(3 * i / 4 < decode_length));
        hex[3 * i / 4 + 0] = (p - CHARS) << 2;

        p = strchr(CHARS, encoded[1]);
        hex[3 * i / 4 + 0] |= (p - CHARS) >> 4;
        print_d("byte 0 = 0x%02x\n", hex[3 * i / 4 + 0]);

        /* 010010 010010 011101 101101 */
        /*          ^^^^ ^^^^          */
        print_d("check %2u < %u: %s\n", 3 * i / 4 + 1, decode_length, CHECK(3 * i / 4 + 1 < decode_length));
        hex[3 * i / 4 + 1] = (p - CHARS) << 4;

        if (encoded[2] != '=')
        {
            p = strchr(CHARS, encoded[2]);
            print_d("check %2u < %u: %s\n", 3 * i / 4 + 1, decode_length, CHECK(3 * i / 4 + 1 < decode_length));
            hex[3 * i / 4 + 1] |= (p - CHARS) >> 2;
        }

        print_d("byte 1 = 0x%02x\n", hex[3 * i / 4 + 1]);

        /* 010010 010010 011101 101101 */
        /*                   ^^ ^^^^^^ */
        print_d("check %2u < %u: %s\n", 3 * i / 4 + 2, decode_length, CHECK(3 * i / 4 + 2 < decode_length));
        if (encoded[2] == '=')
        {
            hex[3 * i / 4 + 2] = 0x0;
        }
        else
        {
            p = strchr(CHARS, encoded[2]);
            hex[3 * i / 4 + 2] = (p - CHARS) << 6;
        }


        if (encoded[3] != '=')
        {
            p = strchr(CHARS, encoded[3]);
            hex[3 * i / 4 + 2] |= (p - CHARS) & 0x3f;
        }

        print_d("byte 2 = 0x%02x\n", hex[3 * i / 4 + 2]);
    }
}
