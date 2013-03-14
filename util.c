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
    PRINT("Allocating %d block%s of %u bits...\n", block_count, (block_count == 1 ? "" : "s"), info->digest_length);

    (*digest_ref) = malloc(total_length * sizeof *(*digest_ref));
    PRINT("Allocated %u bytes\n", total_length * sizeof *(*digest_ref));

    /* set all bytes to 0x0 */
    memset(*digest_ref, 0x0, total_length);
    PRINT("Set %u bytes to 0x0\n", total_length);

    /* copy the message in */
    memcpy(*digest_ref, msg, *length);

    /* a single "1" bit is appended to the message... */
    (*digest_ref)[*length] = 0x80;

    *length = total_length;

    return block_count;
}

void print_d(const uint8_t *data, uint32_t block_count, const struct hash_info *info)
{
    const uint8_t length = block_count * info->digest_length / CHAR_BIT;
    PRINT("Printing %u bits...\n", length * CHAR_BIT);
    const uint8_t block_size = info->block_size / CHAR_BIT;

    for (int i = 0; i < length; ++i)
    {
        printf("%02x ", (char) data[i] & 0xff);
        fflush(stdout);

        if ((i + 1) % block_size == 0) printf("\n");
    }
}
