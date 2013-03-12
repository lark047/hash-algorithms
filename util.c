#include "util.h"

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t append_padding(uint8_t **digest_ref, const char *msg, uint32_t *length, struct hash_info *info)
{
    /* TODO what if the length in bits isn't a multiple of CHAR_BIT? */

    uint32_t nils = 0;

    /* +1 for 0x80 */
    while (((*length + 1) * CHAR_BIT + nils) % info->digest_length != info->padded_length)
    {
        ++nils;
    }

    PRINT("Adding %u 0x0 bits\n", nils);
    nils /= CHAR_BIT;
    PRINT("Adding %u 0x0 bytes\n", nils);

    const uint32_t total_length = *length + 1 + nils + info->block_size / CHAR_BIT;
    const uint32_t block_count = total_length / info->block_size;
    PRINT("total_length / 64 = %u (%u) <== should be 0\n", block_count, total_length % info->block_size);

    PRINT("Allocating %d block%s of 512 bits...\n", block_count, (block_count == 1 ? "" : "s"));

    (*digest_ref) = malloc(total_length * sizeof *(*digest_ref));
    PRINT("Allocated %u bytes...\n", total_length * sizeof *(*digest_ref));

    memcpy(*digest_ref, msg, total_length);

    /* a single "1" bit is appended to the message... */
    (*digest_ref)[*length] = 0x80;

    /*
     * ... and then "0" bits are appended so that the length in bits of
     * the padded message becomes congruent to 448, modulo 512. In all, at
     * least one bit and at most 512 bits are appended.
     */
    memset(*digest_ref + *length + 1, 0x0, nils);
#ifdef DEBUG
    print_d(*digest_ref, 56);
#endif
    PRINT("%s\n", "*******");

    *length = total_length;

    return block_count;
}

#if 0
uint32_t rotl(const uint32_t value, const uint8_t shift)
{
#if 0 /* don't need optimization */
    if ((shift &= sizeof(value) * CHAR_BIT - 1) == 0)
    {
        return value;
    }
#endif

    return (value << shift) | (value >> (sizeof(value) * CHAR_BIT - shift));
}

uint32_t rotr(const uint32_t value, const uint8_t shift)
{
    return (value >> shift) | (value << (sizeof(value) * CHAR_BIT - shift));
}

uint32_t shr(const uint32_t value, const uint8_t shift)
{
    return value >> shift;
}
#endif

void print_d(uint8_t *data, uint8_t length)
{
    for (int i = 0; i < length; ++i)
    {
        printf("%02x ", (char) data[i] & 0xff);
        fflush(stdout);

        if ((i + 1) % 8 == 0) printf("\n");
    }
}
