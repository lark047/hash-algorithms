#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>

#include "matasano.h"

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

    /* 0x49276d: */

    /* 01001001 00100111 01101101 */
    /* ^^^^^^                     */
    uint8_t index = bytes[0] >> 2;
    *p++ = CHARS[index];

    /* 01001001 00100111 01101101 */
    /*       ^^ ^^^^              */
    index  = (bytes[0] & 0x03) << 4;
    index |= (bytes[1] & 0xf0) >> 4;
    *p++ = CHARS[index];

    /* 01001001 00100111 01101101 */
    /*              ^^^^ ^^       */
    index  = (bytes[1] & 0x0f) << 2;
    index |= (bytes[2] & 0xc0) >> 6;
    *p++ = CHARS[index];

    /* 01001001 00100111 01101101 */
    /*                     ^^^^^^ */
    index  = bytes[2] & 0x3f;
    *p++ = CHARS[index];

    snprintf(base64 + 4 - padding_length, padding_length + 1, "%s", padding);
}

const char *EncodeBase64(const char * const msg)
{
    if (msg == NULL || strlen(msg) % 2 != 0)
    {
        errno = EINVAL;
        return NULL;
    }
    else if (strlen(msg) == 0)
    {
        return NULL;
    }

    char *base64 = malloc(4 * ceil((double) strlen(msg) / 2 / 3) + 1); /* TODO check */

    for (size_t i = 0; i < strlen(msg); i += 6)
    {
        /* operate on 3 bytes at a time */
        uint8_t bytes[3];

        const char *padding;
        uint8_t n = 0;

        memset(bytes, 0, sizeof bytes);

        for (uint8_t j = 0; j < 3; ++j)
        {
            if (i + 2 * j >= strlen(msg))
            {
                break;
            }

            n += sscanf(msg + i + 2 * j, "%2" SCNx8, bytes + j * sizeof *bytes);
        }

        /* TODO use operations on single uint32_t for decomposing 3 hex digits */

        switch (n)
        {
            case 1:
                padding = PADDING2;
                break;
            case 2:
                padding = PADDING1;
                break;
            default:
                padding = PADDING0;
                break;
        }

        char encoded[4 + 1];

        memset(encoded, 0, sizeof encoded);
        to_base64(bytes, padding, encoded);
        // print_d("encoded: \"%s\" (%02x%02x%02x%02x%02x)\n", encoded, encoded[0], encoded[1], encoded[2], encoded[3], encoded[4]);

        snprintf(base64 + i * 2/3, sizeof encoded, "%4s", encoded);
    }

    return base64;
}

const uint8_t *DecodeBase64(const char * const msg)
{
    if (msg == NULL || strlen(msg) % 4 != 0)
    {
        errno = EINVAL;
        return NULL;
    }
    else if (strlen(msg) == 0)
    {
        return NULL;
    }

    uint8_t *hex = malloc(3 * strlen(msg) / 4 + 1); /* TODO check */

    /* operate on 4 encoded chars at a time */
    char encoded[4];

    for (size_t i = 0; i < strlen(msg); i += 4)
    {
        /* SSdt... */
        memcpy(encoded, msg + i, sizeof encoded);

        /* 010010 010010 011101 101101 */
        /* ^^^^^^ ^^                   */
        char *p = strchr(CHARS, encoded[0]);
        hex[3 * i / 4 + 0] = (p - CHARS) << 2;

        p = strchr(CHARS, encoded[1]);
        hex[3 * i / 4 + 0] |= (p - CHARS) >> 4;

        /* 010010 010010 011101 101101 */
        /*          ^^^^ ^^^^          */
        hex[3 * i / 4 + 1] = (p - CHARS) << 4;

        if (encoded[2] != '=')
        {
            p = strchr(CHARS, encoded[2]);
            hex[3 * i / 4 + 1] |= (p - CHARS) >> 2;
        }

        /* 010010 010010 011101 101101 */
        /*                   ^^ ^^^^^^ */
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
    }

    return hex;
}
