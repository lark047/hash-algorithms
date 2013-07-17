#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Input: (0x)49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d
 * Expected output: SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t
 */

int main(int argc, char **argv)
{
    int rc = EXIT_FAILURE;

    if (argc == 2)
    {
        const char *chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        const char *msg = argv[1];
        const uint64_t len = 2 * ceil(strlen(msg)) / 3 + 1;
        uint8_t padding_bytes = 3 - (strlen(msg) / 2) % 3;

        char *base64 = malloc(len), *p;

        if (base64 == NULL)
        {
            fprintf(stderr, "[error] could not allocate %llu bytes\n", len);
        }
        else
        {
            /* operate on 3 bytes at a time */
            uint8_t byte[3], index;

            printf("[debug] allocated %llu bytes\n", len);

            memset(base64, '=', len);
            base64[len] = 0;
            p = base64;

            for (size_t i = 0; i < strlen(msg); i += 6)
            {
                bool last_bytes = (i + 6 > strlen(msg));

                /* 0x49276d */
                memset(byte, 0x00, sizeof byte);
                sscanf(msg + i, "%6x", (unsigned *) byte);

                printf("[debug] before byte 1 = 0x%02x\n", byte[0]);
                printf("[debug] before byte 2 = 0x%02x\n", byte[1]);
                printf("[debug] before byte 3 = 0x%02x\n", byte[2]);

                if (last_bytes)
                {
                    printf("[debug] in last bytes with %u padding byte%s\n", padding_bytes, (padding_bytes == 1 ? "" : "s"));
                    switch (padding_bytes)
                    {
                        case 1:
                            byte[2] = byte[1];
                            byte[1] = byte[0];
                            byte[0] = 0x00;
                            break;
                        case 2:
                            byte[2] = byte[0];
                            byte[1] = 0x00;
                            byte[2] = 0x00;
                            break;
                        default:
                            printf("[error] calculated %u padding bytes\n", padding_bytes);
                            break;
                    }
                }

                printf("[debug] after byte 1 = 0x%02x\n", byte[0]);
                printf("[debug] after byte 2 = 0x%02x\n", byte[1]);
                printf("[debug] after byte 3 = 0x%02x\n", byte[2]);

                // printf("[debug] msg[i] (string): %s\n", byte);
                // printf("[debug] byte = %u, %u >> 2 = %u, char(%u) = %c\n", index, index, index >> 2, index >> 2, chars[index >> 2]);

                /* 01001001 00100111 01101101 */
                /* ^^^^^^                     */
                index = byte[2] >> 2;
                printf("[debug] output %u: 0x%02x\n", i, index);
                *p++ = chars[index];

                /* 01001001 00100111 01101101 */
                /*       ^^ ^^^^              */
                index  = (byte[2] & 0x03) << 4;
                index |= (!last_bytes || padding_bytes < 2 ? (byte[1] & 0xf0) >> 4 : 0);
                printf("[debug] output %u: 0x%02x\n", i + 1, index);
                *p++ = chars[index];

                if (last_bytes && padding_bytes > 1)
                {
                    index = 64;
                }
                else
                {
                    /* 01001001 00100111 01101101 */
                    /*              ^^^^ ^^       */
                    index  = (!last_bytes || padding_bytes < 2 ? (byte[1] & 0x0f) << 2 : 0);
                    index |= (!last_bytes || padding_bytes < 1 ? (byte[0] & 0xc0) >> 6 : 0);
                }

                printf("[debug] output %u: 0x%02x\n", i + 2, index);
                *p++ = chars[index];

                if (last_bytes && padding_bytes > 0)
                {
                    index = 64;
                }
                else
                {
                    /* 01001001 00100111 01101101 */
                    /*                     ^^^^^^ */
                    index  = (!last_bytes || padding_bytes < 1 ? byte[0] & 0x3f : 0);
                }

                printf("[debug] output %u: 0x%02x\n", i + 3, index);
                *p++ = chars[index];
            }

            printf("[info] input : %s\n", msg);
            printf("[info] output: %s\n", base64);

            printf("[info] strings%s match\n", strcmp(base64, "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t") ? " DO NOT" : "");
            if (!strcmp(base64, "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t")) printf("[info] congratulations!\n");

            free(base64);

            rc = EXIT_SUCCESS;
        }
    }

    return rc;
}

#if 0

0x6d
  0110 1101 => 0x6d
& 1100 0000 => 0xc0
  ---------
  0100 0000 => 0x40

  0000 0100 => 0x40 >> 4 => 0x04
| 0001 1100 => 0x1c
  ---------
  0001 0100 => 0x14

49276d 206b69 6c6c69 6e6720 796f75 722062 726169 6e206c 696b65 206120 706f69 736f6e 6f7573 206d75 736872 6f6f6d
SSdt   IGtp   bGxp   bmcg   eW91   ciBi   cmFp   biBs   aWtl   IGEg   cG9p   c29u   b3Vz   IG11   c2hy   b29t

SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t
SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb28=
SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyAA==

0110 1111 0110  1111 0110 1101
011011  110110  111101  101101
0x1b      0x36  0x3d      0x2d
27          54  61          45
b            2  9            t

0110 1111 0110  1111 0000 0000
011011  110110  111100  000000
0x1b      0x36  0x3c      0x00
27          54  60           0
b            2  8            =

0110 1111 0000  0000 0000 0000
011011  110000  000000  000000
0x1b      0x30  0x00      0x00
27          48  0            0
b            w  =            =

#endif
