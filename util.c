#include "util.h"

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* for use with hash_file */
#define BUFSIZE 1024

/* test files */
const char *test_files[] = {
    "text/lorem-ipsum.txt",              /* 11417 bytes */
    "text/lorem-ipsum-with-newline.txt", /* 11458 bytes */
    0
};

/* test messages */
const char *test_msgs[] = {
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

uint8_t *hash_file(FILE *fp, uint8_t *(*hash)(uint8_t *, uint64_t))
{
    size_t n;

    uint8_t buf[BUFSIZE], *msg = NULL;
    uint64_t length = 0;

    PRINT("SIZE(buf) = %u / %u = %u\n", sizeof buf, sizeof *buf, SIZE(buf));
    while ((n = fread(buf, sizeof *buf, SIZE(buf), fp)))
    {
        if ((msg = realloc(msg, length + n)) != NULL)
        {
            memcpy(msg + length, buf, n);
            length += n;
        }

        memset(buf, 0, n);
    }

    uint8_t *digest = hash(msg, length);

    /* there's gotta be a better way... I want to just return hash(msg); */
    free(msg);

    return digest;
}

uint64_t append_padding(uint8_t **digest_ref, uint8_t *msg, uint64_t *length, struct hash_info *info)
{
    /* TODO what if the length in bits isn't a multiple of CHAR_BIT? */

    uint32_t nils = 0;

    /* +1 for 0x80 */
    while (((*length + 1) * CHAR_BIT + nils) % info->digest_length != info->padded_length)
    {
        ++nils;
    }

    PRINT("adding %u 0x0 bits\n", nils);
    nils /= CHAR_BIT;
    PRINT("adding %u 0x0 bytes\n", nils);

    const uint64_t total_length = *length + 1 + nils + info->block_size / CHAR_BIT;
    const uint64_t block_count = total_length / info->block_size;

    PRINT("total_length / 64 = %llu (%llu) <== should be 0\n", block_count, total_length % info->block_size);
    PRINT("allocating %llu block%s of %u bits...\n", block_count, (block_count == 1 ? "" : "s"), info->digest_length);

    (*digest_ref) = malloc(total_length * sizeof *(*digest_ref));
    PRINT("allocated %llu bytes\n", total_length * sizeof *(*digest_ref));

    /* set all bytes to 0x0 */
    memset(*digest_ref, 0x0, total_length);
    PRINT("set %llu bytes to 0x0\n", total_length);

    /* copy the message in */
    memcpy(*digest_ref, msg, *length);

    /* a single "1" bit is appended to the message... */
    PRINT("adding 0x%x byte...\n", 0x80);
    (*digest_ref)[*length] = 0x80;

    *length = total_length;

    PRINT("%s\n", "returning from append_length");
    PRINT("block count: %llu\n", block_count);
    PRINT("length     : %llu\n", *length);

    return block_count;
}

void print_d(const uint8_t *data, uint64_t block_count, const struct hash_info *info)
{
    PRINT("block count : %llu\n", block_count);
    PRINT("block length: %u bits\n", info->digest_length);
    const uint64_t length = block_count * info->digest_length / CHAR_BIT;
    PRINT("printing %llu bytes...\n", length);
    PRINT("printing %llu bits...\n", length * CHAR_BIT);
    const uint8_t block_size = info->block_size / CHAR_BIT;

    for (uint64_t i = 0; i < length; ++i)
    {
        printf("%02x ", (char) data[i] & 0xff);
        fflush(stdout);

        if ((i + 1) % block_size == 0) printf("\n");
    }
}

#undef BUFSIZE
