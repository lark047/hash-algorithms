#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>

#include "matasano.h"

#define CHARS  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

#define INDEX1(bytes) (bytes[0] >> 2)
#define INDEX2(bytes) (((bytes[0] & 0x03) << 4) | ((bytes[1] & 0xf0) >> 4))
#define INDEX3(bytes) (((bytes[1] & 0x0f) << 2) | ((bytes[2] & 0xc0) >> 6))
#define INDEX4(bytes) (bytes[2] & 0x3f)

#define INDEXOF(c) (strchr(CHARS, c) - CHARS)

/**
 * Input: (0x)49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d
 * Expected output: SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t
 */

const char *EncodeBase64(const char * const msg)
{
    size_t msg_length;

    if (msg == NULL || (msg_length = strlen(msg)) % 2 != 0)
    {
        errno = EINVAL;
        return NULL;
    }
    else if (msg_length == 0)
    {
        return "";
    }

    const size_t base64_len = 4 * ceil((double) msg_length / 2 / 3);
    char *base64 = malloc(base64_len + 1);

    if (base64 == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }

    for (size_t i = 0; i < msg_length; i += 6)
    {
        /* operate on 3 bytes at a time */
        uint8_t bytes[3] = {0};
        uint8_t n = 0;

        for (uint8_t j = 0; j < LENGTH(bytes); ++j)
        {
            if (i + 2 * j >= msg_length)
            {
                break;
            }

            n += sscanf(msg + i + 2 * j, "%2" SCNx8, bytes + j * sizeof *bytes);
        }

        char encoded[] = {
            CHARS[INDEX1(bytes)],
            CHARS[INDEX2(bytes)],
            n < 2 ? '=' : CHARS[INDEX3(bytes)],
            n < 3 ? '=' : CHARS[INDEX4(bytes)],
            0
        };

        /* print_d("encoded: \"%s\" (%02x%02x%02x%02x%02x)\n", encoded, encoded[0], encoded[1], encoded[2], encoded[3], encoded[4]); */
        snprintf(base64 + i * 2/3, sizeof encoded, "%4s", encoded);
    }

    return base64;
}

const uint8_t *DecodeBase64(const char * const msg)
{
    size_t msg_length;

    if (msg == NULL || (msg_length = strlen(msg)) % 4 != 0)
    {
        errno = EINVAL;
        return NULL;
    }
    else if (msg_length == 0)
    {
        return NULL;
    }

    uint8_t *hex = malloc(3 * msg_length / 4 + 1);

    if (hex == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }

    /* operate on 4 encoded chars at a time */
    char encoded[4];

    for (size_t i = 0; i < msg_length; i += sizeof encoded)
    {
        /* SSdt... */
        memcpy(encoded, msg + i, sizeof encoded);

        /* 010010 010010 011101 101101 */
        /* ^^^^^^ ^^                   */
        hex[3 * i / 4 + 0] = (INDEXOF(encoded[0]) << 2) | (INDEXOF(encoded[1]) >> 4);

        /* 010010 010010 011101 101101 */
        /*          ^^^^ ^^^^          */
        hex[3 * i / 4 + 1] = (INDEXOF(encoded[1]) << 4) | (encoded[2] == '=' ? 0x0 : (INDEXOF(encoded[2]) >> 2));

        /* 010010 010010 011101 101101 */
        /*                   ^^ ^^^^^^ */
        hex[3 * i / 4 + 2] = (encoded[2] == '=' ? 0x0 : (INDEXOF(encoded[2]) << 6)) | (encoded[3] == '=' ? 0x0 : (INDEXOF(encoded[3]) & 0x3f));
    }

    return hex;
}
