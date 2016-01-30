#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>

#include "matasano.h"

#define CHARS  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

#if 0

/* using uint32_t */
#  define INDEX1(bytes) ((bytes & 0xfc0000) >> 18)
#  define INDEX2(bytes) ((bytes & 0x03f000) >> 12)
#  define INDEX3(bytes) ((bytes & 0x000fc0) >>  6)
#  define INDEX4(bytes) ((bytes & 0x00003f) >>  0)

#else

/* using uint8_t[3] */
#  define INDEX1(bytes) (bytes[0] >> 2)
#  define INDEX2(bytes) (((bytes[0] & 0x03) << 4) | ((bytes[1] & 0xf0) >> 4))
#  define INDEX3(bytes) (((bytes[1] & 0x0f) << 2) | ((bytes[2] & 0xc0) >> 6))
#  define INDEX4(bytes) (bytes[2] & 0x3f)

#endif

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

    for (size_t i = 0; i < strlen(msg); i += 6)
    {
        /* operate on 3 bytes at a time */
        uint8_t bytes[3] = {0};
        uint8_t n = 0;

        for (uint8_t j = 0; j < LENGTH(bytes); ++j)
        {
            if (i + 2 * j >= strlen(msg))
            {
                break;
            }

            n += sscanf(msg + i + 2 * j, "%2" SCNx8, bytes + j * sizeof *bytes);
        }

#if 0
        /* convert to base 16 string to use strtoul() */
        char base16[9];
        snprintf(base16, sizeof base16, "0x%x%x%x", bytes[0], bytes[1], bytes[2]);

        errno = 0;
        uint32_t b = (uint32_t) strtoul(base16, NULL, 0);

        if (errno != 0)
        {
            fprintf(stderr, "Could not convert %s to uint32_t:", bytes);
            perror(NULL);
            return NULL;
        }
#endif

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
