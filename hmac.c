#include "util.h"
#include "hmac.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint8_t *HMACstring(const char *K, const char *msg, uint8_t *(*H)(const uint8_t *, uint64_t), uint8_t B, uint8_t digest_length)
{
    PRINT("key (length): \"%s\" (%u)\n", K, strlen(K));
    PRINT("msg (length): \"%s\" (%u)\n", msg, strlen(msg));
    return HMAC((uint8_t *) K, strlen(K), (uint8_t *) msg, strlen(msg), H, B, digest_length);
}

uint8_t *HMAC(const uint8_t *K, uint64_t key_length, const uint8_t *msg, uint64_t msg_length, uint8_t *(*H)(const uint8_t *, uint64_t), uint8_t B, uint8_t digest_length)
{
    uint8_t *key = (uint8_t *) K;
    PRINT("key length = %llu\n", key_length);

#ifdef DEBUG
        char *buf = NULL;
#endif

    if (key_length > B)
    {
        PRINT("hashing \"%s\"...\n", (char *) K);
        uint8_t *digest = H(K, key_length);
        key_length = digest_length;

        key = malloc(digest_length);

        PRINT("key digest: %s\n", (buf = to_string(digest, digest_length)));
        PRINT("key length: %u\n", digest_length);

#ifdef DEBUG
        free(buf);
#endif

        memcpy(key, digest, digest_length);

        free(digest);
    }

    if (key_length < B)
    {
        /* append zeroes to the end of K to create a B byte string */
        PRINT("padding key with %llu 0x0 bytes...\n", B - key_length);
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
    PRINT("digest (post-inner): %s\n", (buf = to_string(digest, digest_length)));
    PRINT("digest length = %u\n", digest_length);

#ifdef DEBUG
    free(buf);
#endif

    free(buffer);
    buffer = malloc(B + digest_length);

    /* copy key + digest into buffer */
    memset(buffer + 0, 0x0, B);
    memcpy(buffer + 0, key, key_length);
    memcpy(buffer + B, digest, digest_length);

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
    PRINT("digest (post-outer): %s\n", (buf = to_string(digest, digest_length)));

#ifdef DEBUG
    free(buf);
#endif

    if (key != K)
    {
        free(key);
    }

    return digest;
}
