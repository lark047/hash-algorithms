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
    "files/lorem-ipsum.txt",              /*    11,417 bytes */
    "files/lorem-ipsum-with-newline.txt", /*    11,458 bytes */
    "files/random.bin",                   /* 2,162,688 bytes */
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

uint8_t *hash_file(FILE *fp, uint8_t *(*hash)(const uint8_t *, uint64_t))
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

    PRINT("hashing %llu byte file...\n", length);
    uint8_t *digest = hash(msg, length);

    /* there's gotta be a better way... I just want to return hash(msg); */
    free(msg);

    return digest;
}

uint64_t append_padding(uint8_t **buffer_ref, const uint8_t *msg, uint64_t length, const struct hash_info *info)
{
    /* TODO what if the length in bits isn't a multiple of CHAR_BIT? */

    uint32_t bytes = 0;

    do
    {
        ++bytes;
    }
    while ((length * CHAR_BIT + bytes) % info->digest_length != info->pad_msg_to_length);

    bytes /= CHAR_BIT;

    /* -1 for 0x80 */
    bytes -= 1;

    PRINT("adding %u 0x%02x bytes\n", bytes, 0);

    const uint64_t padded_length = length + 1 + bytes;
    const uint64_t total_length = padded_length + info->block_length / CHAR_BIT;

#ifdef DEBUG
    const uint64_t block_count = total_length / info->block_length;
#endif

    PRINT("total_length = %llu + %d + %u + %u = %llu\n", length, 1, bytes, info->block_length / CHAR_BIT, total_length);
    PRINT("%llu / 64 = %llu (%llu) <== should be 0\n", total_length, block_count, total_length % info->block_length);
    PRINT("allocating %llu block%s of %u bits...\n", block_count, (block_count == 1 ? "" : "s"), info->digest_length);

    (*buffer_ref) = malloc(total_length * sizeof *(*buffer_ref));
    PRINT("allocated %llu bytes\n", total_length * sizeof *(*buffer_ref));

    /* set padded_length bytes to 0x0 */
    memset(*buffer_ref, 0x0, padded_length);
    PRINT("set %llu bytes to 0x%02x\n", padded_length, 0);

    /* copy the message in */
    memcpy(*buffer_ref, msg, length);

    /* a single "1" bit is appended to the message... */
    PRINT("adding 0x%x byte...\n", 0x80);
    (*buffer_ref)[length] = 0x80;

    PRINT("%s\n", "returning from append_length");
    PRINT("block count  : %llu\n", block_count);
    PRINT("padded length: %llu\n", padded_length);

    return padded_length;
}

void print_d(const uint8_t *data, uint64_t block_count, const struct hash_info *info)
{
    PRINT("block count : %llu\n", block_count);
    PRINT("block length: %u bits\n", info->digest_length);
    const uint64_t length = block_count * info->digest_length / CHAR_BIT;
    PRINT("printing %llu bytes...\n", length);
    PRINT("printing %llu bits...\n", length * CHAR_BIT);
    const uint8_t block_size = info->block_length / (info->block_length > 32 ? CHAR_BIT : 1);

    for (uint64_t i = 0; i < length; ++i)
    {
        printf("%02x ", (char) data[i] & 0xff);
        fflush(stdout);

        if ((i + 1) % block_size == 0) printf("\n");
    }
}

char *to_string(uint8_t *digest, uint8_t size)
{
    char *buf = malloc(size * CHARS_PER_BYTE + 1);

    for (uint8_t i = 0; i < size; ++i)
    {
        PRINT("next = 0x%02x\n", digest[i]);
        sprintf(buf + i * CHARS_PER_BYTE, "%02x", digest[i]);
    }

    return buf;
}

void flip(uint32_t *value)
{
    *value = ((*value >> 24) & 0x000000ff)  /* move byte 3 to byte 0 */
           | ((*value >>  8) & 0x0000ff00)  /* move byte 2 to byte 1 */
           | ((*value <<  8) & 0x00ff0000)  /* move byte 1 to byte 2 */
           | ((*value << 24) & 0xff000000); /* move byte 0 to byte 3 */
}

#undef BUFSIZE
