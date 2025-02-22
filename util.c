#include "util.h"

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CUnit/Basic.h>

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

void flip64(uint64_t *value)
{
    *value = ((*value >> 56) & 0x00000000000000ff)  /* move byte 7 to byte 0 */
           | ((*value >> 40) & 0x000000000000ff00)  /* move byte 6 to byte 1 */
           | ((*value >> 24) & 0x0000000000ff0000)  /* move byte 5 to byte 2 */
           | ((*value >>  8) & 0x00000000ff000000)  /* move byte 4 to byte 3 */
           | ((*value <<  8) & 0x000000ff00000000)  /* move byte 3 to byte 4 */
           | ((*value << 24) & 0x0000ff0000000000)  /* move byte 2 to byte 5 */
           | ((*value << 40) & 0x00ff000000000000)  /* move byte 1 to byte 6 */
           | ((*value << 56) & 0xff00000000000000); /* move byte 0 to byte 7 */
}

FILE *open(const char *filename, const char *ext)
{
    /* TODO need bigger int? */
    const uint8_t sizes[] = {
        strlen(filename),
        strlen(ext)
    };
    char *hfile = malloc(sizes[0] + 1 + sizes[1] + 1);
    PRINT("allocated %u bytes\n", sizes[0] + 1 + sizes[1] + 1);

    FILE *fp = NULL;

    if (hfile)
    {
        PRINT("copying \"%s\"...\n", filename);
        memcpy(hfile, filename, sizes[0]);
        PRINT("copying '%c'...\n", '.');
        hfile[sizes[0]] = '.';

        PRINT("copying \"%s\"...\n", ext);
        memcpy(hfile + sizes[0] + 1, ext, sizes[1]);
        PRINT("copying \\%d byte...\n", 0);
        hfile[sizes[0] + 1 + sizes[1]] = 0;

        /* remove '-', '/' */
        PRINT("removing '%c', '%c'...\n", '-', '/');
        char *p, *q;
        p = q = hfile + sizes[0] + 1;

        while (*p)
        {
            PRINT("*p = '%c' (0x%02x)\n", *p, *p);
            PRINT("*q = '%c' (0x%02x)\n", *q, *q);

            while (*p && (*p == '-' || *p == '/'))
            {
                ++p;
            }

            *q++ = *p++;
        }

        *q = 0;

        PRINT("opening file \"%s\"\n", hfile);
        fp = fopen(hfile, "r");
        PRINT("opened file \"%s\"\n", hfile);
        free(hfile);
    }

    return fp;
}

/* command line options */
int do_test(const char *label, const char *test_name, void (*test_function)(void))
{
    CU_pSuite suite;

    if (CU_initialize_registry() == CUE_SUCCESS)
    {
        if ((suite = CU_add_suite(label, NULL, NULL)))
        {
            if (CU_add_test(suite, test_name, test_function))
            {
                CU_basic_set_mode(CU_BRM_VERBOSE);
                CU_basic_run_tests();
            }
        }
        CU_cleanup_registry();
    }
    return CU_get_error();
}

int do_hash_string(const char *msg, uint8_t *(*hash)(const char *), uint8_t digest_length)
{
    PRINT("Calculating hash for \"%s\"...\n", msg);
    PRINT("Using digest length of %u\n", digest_length);

    /* TODO check returns */
    uint8_t *digest = hash(msg);
    char *buf = to_string(digest, digest_length);

    printf("%s\n", buf);

    /* clean up */
    PRINT("%s\n", "Cleaning up...");
    free(digest);
    free(buf);

    digest = NULL;
    buf = NULL;

    return EXIT_SUCCESS;
}

int do_hash_file(const char *filename, uint8_t *(*hash)(FILE *), uint8_t digest_length)
{
    int rc = EXIT_FAILURE;

    PRINT("Calculating hash for \"%s\"...\n", filename);
    PRINT("Using byte size of %u\n", (unsigned) CHAR_BIT);

    FILE *fp = fopen(filename, "rb");

    if (fp)
    {
        /* TODO check returns */
        uint8_t *digest = hash(fp);
        char *buf = to_string(digest, digest_length);

        printf("%s\n", buf);

        /* clean up */
        PRINT("%s\n", "Cleaning up...");
        free(digest);
        free(buf);
        fclose(fp);

        digest = NULL;
        buf = NULL;
        fp = NULL;

        rc = EXIT_SUCCESS;
    }
    else
    {
        fprintf(stderr, "[ERROR] Could not open %s for reading.", filename);
    }

    return rc;
}

int do_verify(const char *filename, const char *label, uint8_t *(*hash)(FILE *), uint8_t digest_length)
{
    FILE *fp, *fhash;

    if ((fp = fopen(filename, "rb")) == NULL)
    {
        printf("[error] could not open %s for reading.\n", filename);
        return EXIT_FAILURE;
    }

    if ((fhash = open(filename, label)) == NULL)
    {
        printf("[warn] no %s signature found for %s.\n", label, filename);
        return EXIT_FAILURE;
    }

    PRINT("hashing \"%s\" with %s...\n", filename, label);
    uint8_t *digest = hash(fp);
    char *string = to_string(digest, digest_length);
    PRINT("%s(%s) = %s\n", label, filename, string);

    /* TODO check return */
    PRINT("allocating %u bytes...\n", digest_length * CHARS_PER_BYTE + 1);
    char *verify = malloc(digest_length * CHARS_PER_BYTE + 1);
    fscanf(fhash, "%s", verify);
    PRINT("found verified hash: %s\n", verify);

    printf("Verifying \"%s\"... ", filename);
    fflush(stdout);

    if (STR_EQ(string, verify))
    {
        printf("OK.\n");
    }
    else
    {
        printf("FAILED.\n\n");
        printf("expected: %s\n", verify);
        printf("actual  : %s\n", string);
    }

    PRINT("%s...\n", "cleaning up");
    free(verify);
    free(string);
    free(digest);

    fclose(fhash);
    fclose(fp);

    return EXIT_SUCCESS;
}

int do_hmac(const char *key, const char *msg, uint8_t *(*hash)(const char *, const char *), uint8_t digest_length)
{
    /* TODO check returns */
    uint8_t *digest = hash(key, msg);
    char *buf = to_string(digest, digest_length);

    printf("%s\n", buf);

    /* clean up */
    PRINT("%s\n", "Cleaning up...");
    free(digest);
    free(buf);
    digest = NULL;
    buf = NULL;

    return EXIT_SUCCESS;
}

void usage(const char *program, const char *label)
{
    printf("Usage: %s [-t|--test] [-f|--file] [-h|--hmac] \"<message>\"\n", program);
    printf("Hash a given message or file with the %s hash function.\n\n", label);
    printf("-t, --test\t\t\tRun the test suite.\n");
    printf("-f, --file \"<filename>\"\t\tCalculate the %s hash of file named <filename>.\n", label);
    printf("-h, --hmac \"<key>\" \"<message>\"\tCalculate the %s HMAC hash of <message> using <key>.\n", label);
    // TODO printf("-v, --verify\tVerifies a file against the provided signature.\n");
}

enum command_option get_option(int count, char **args)
{
    if (count == 2 && (STR_EQ(args[1], "-t") || STR_EQ(args[1], "--test")))
    {
        return Test;
    }
    else if (count == 2)
    {
        return String;
    }
    else if (count == 3 && (STR_EQ(args[1], "-f") || STR_EQ(args[1], "--file")))
    {
        return File;
    }
    else if (count == 3 && (STR_EQ(args[1], "-v") || STR_EQ(args[1], "--verify")))
    {
        return Verify;
    }
    else if (count == 4 && (STR_EQ(args[1], "-h") || STR_EQ(args[1], "--hmac")))
    {
        return MAC;
    }

    return 0;
}

#undef BUFSIZE
