#include "util.h"
#include "hmac.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define B 64

#define HMAC_MD5(k, m) (HMACstring(k, m, MD5))

uint8_t *HMACstring(const char *K, const char *msg, uint8_t *(*H)(const uint8_t *, uint64_t))
{
    return HMAC((uint8_t *) K, strlen(K), (uint8_t *) msg, strlen(msg), H);
}

uint8_t *HMAC(const uint8_t *K, uint64_t key_length, const uint8_t *msg, uint64_t msg_length, uint8_t *(*H)(const uint8_t *, uint64_t))
{
    uint8_t *key = NULL;
    PRINT("key length = %u\n", key_length);

    if (key_length > B)
    {
        PRINT("hashing \"%s\"...\n", (char *) K);
        uint8_t *digest = H(K, key_length);
        key_length = strlen((char *) digest) / 2;

        key = malloc(key_length);
        PRINT("key digest: %s\n", (char *) digest);
        PRINT("key length: %u\n", key_length);
        for (uint8_t i = 0; i < key_length; ++i)
        {
            unsigned x;
            sscanf((char *) digest + i * 2, "%02x", &x);
            key[i] = x;
            PRINT("key[%2u] = 0x%02x\n", i, key[i]);
        }

        free(digest);
    }

    if (key_length < B)
    {
        key = key ? key : (uint8_t *) K;

        /* append zeroes to the end of K to create a B byte string */
        PRINT("padding key with %u 0x0 bytes...\n", B - key_length);
    }

    uint8_t *buffer = malloc(B + msg_length);

    /* copy key + msg into buffer */
    memset(buffer + 0, 0x0, B);
    memcpy(buffer + 0, key, key_length);
    memcpy(buffer + B, msg, msg_length);

    /* inner pad */
    for (uint8_t i = 0; i < B; ++i)
    {
        buffer[i] ^= 0x36;
    }

#ifdef DEBUG
    PRINT("%s:\n", "buffer");
    for (uint8_t i = 0; i < B + msg_length; ++i)
    {
        printf("%02x ", buffer[i]);
        fflush(stdout);
        if ((i + 1) % 8 == 0) printf("\n");
    }
    printf("\n");
#endif

    uint8_t *digest = H(buffer, B + msg_length);
    const uint8_t digest_length = strlen((char *) digest) / 2;
    PRINT("digest (post-inner): %s\n", (char *) digest);
    PRINT("digest length = %u\n", digest_length);

    free(buffer);
    buffer = malloc(B + digest_length);

    /* copy key + digest into buffer */
    memset(buffer + 0, 0x0, B);
    memcpy(buffer + 0, key, key_length);

    /* TODO change the hash functions to return raw bytes, not string */
    for (uint8_t i = 0; i < digest_length; ++i)
    {
        unsigned x;
        sscanf((char *) digest + i * 2, "%02x", &x);
        buffer[B + i] = x;
        /* PRINT("buffer[%2u] = 0x%02x\n", B + i, buffer[B + i]); */
    }

    /* outer pad */
    for (uint8_t i = 0; i < B; ++i)
    {
        buffer[i] ^= 0x5c;
    }

#ifdef DEBUG
    PRINT("%s:\n", "buffer");
    for (uint8_t i = 0; i < B + digest_length; ++i)
    {
        printf("%02x ", buffer[i]);
        fflush(stdout);
        if ((i + 1) % 8 == 0) printf("\n");
    }
    printf("\n");
#endif

    free(digest);
    digest = H(buffer, B + digest_length);
    PRINT("digest (post-outer): %s\n", (char *) digest);

    if (key != K)
    {
        free(key);
    }

    return digest;
}
