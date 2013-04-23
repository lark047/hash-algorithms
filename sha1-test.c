#include "sha.h"
#include "util.h"
#include "hmac.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CUnit/Basic.h>

static void testSHA1file(void);
static void testSHA1string(void);
static void testSHA1_HMAC(void);

void testSHA1(void)
{
    testSHA1file();
    testSHA1string();
    testSHA1_HMAC();
}

void testSHA1file(void)
{
    char *sha1s[] = {
        "1f10a1c87261cec87ba9b38e94154dea2a62c4d9",
        "564663cf03a708201283cfe922bc99cc121988ad",
        "13436f8528bf39679646a6d7a77ffeaee25afef1"
    };

    for (uint8_t i = 0; test_files[i]; ++i)
    {
        FILE *fp = fopen(test_files[i], "rb");

        CU_ASSERT_PTR_NOT_NULL_FATAL(fp)

        char *expected = sha1s[i];
        char *actual = (char *) SHA1file(fp);

        CU_ASSERT_PTR_NOT_NULL_FATAL(actual);
        CU_ASSERT_STRING_EQUAL(actual, expected);

        if (STR_EQ(expected, actual))
        {
            PRINT("%s\n", actual);
        }
        else
        {
            fprintf(stderr, "\n");
            fprintf(stderr, "file    : %s\n", test_files[i]);
            fprintf(stderr, "expected: %s\n", expected);
            fprintf(stderr, "actual  : %s\n", actual);
        }

        free(actual);
        fclose(fp);
    }
}

void testSHA1string(void)
{
    char *sha1s[] = {
        "da39a3ee5e6b4b0d3255bfef95601890afd80709",
        "86f7e437faa5a7fce15d1ddcb9eaeaea377667b8",
        "a9993e364706816aba3e25717850c26c9cd0d89d",
        "c12252ceda8be8994d5fa0290a47231c1d16aae3",
        "32d10c7b8cf96570ca04ce37f2a19d84240d3a89",
        "761c457bf73b14d27e9e9265c46f4b4dda11f940",
        "50abf5706a150990a08b2c5ea40fa0e585554732",
        "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12",
        "408d94384216f890ff7a0c3528e8bed1e0b01621"
    };

    for (uint8_t i = 0; test_msgs[i]; ++i)
    {
        char *expected = sha1s[i];
        char *actual = (char *) SHA1string(test_msgs[i]);

        CU_ASSERT_PTR_NOT_NULL_FATAL(actual);
        CU_ASSERT_STRING_EQUAL(actual, expected);

        if (STR_EQ(expected, actual))
        {
            PRINT("%s\n", actual);
        }
        else
        {
            fprintf(stderr, "\n");
            fprintf(stderr, "string  : -->%s<--\n", test_msgs[i]);
            fprintf(stderr, "expected: %s\n", expected);
            fprintf(stderr, "actual  : %s\n", actual);
        }

        free(actual);
    }
}

static void testSHA1_HMAC(void)
{
    char *keys[] = {
        "",
        "key",
        /* TODO 64-byte key */
        "==8utHoe30ASpIEmIe5Roa3#*e1$l7v8a1IeGi!gIu2OUr&$do++hiAjlAH_eTRi_Wi4dOu+l!2hiuThl7frIaclUswiuFI*GluSwiA*?ug@ASi$swOatR8A3Las0ia5",
        0
    };

    char *sha1s[][3] = {
        {
            "fbdb1d1b18aa6c08324b7d64b71fb76370690e1d", "f42bb0eeb018ebbd4597ae7213711ec60760843f", "449b4871ce15452fc74dd1dfc70b8a2641209b2b"
        }, {
            "7f984109f39759f3f41dba04f5183741e36f1445", "0db21f05052f323e714ef9bf1f7b000ffe97e8a0", "9994db0c46b9a898116d1e531632dc8eaf99a792"
        }, {
            "9b4a918f398d74d3e367970aba3cbe54e4d2b5d9", "4fd0b215276ef12f2b3e4c8ecac2811498b656fc", "564d4e82f3d441450ed26ff9b64144541a977ac2"
        }, {
            "68c060a719fd1eaff3f9e124fee10095e14681ed", "81582b5984805bc92ce5b047528345e6e1468538", "82233e9a90a8a8784ac4daccdb59b846df8ff6c4"
        }, {
            "92115000ba8e81227647d78a6167b65aa437186d", "fd6e6a3c6f1a6fb07e0c5a6a679c0a46b9d00347", "151a96c6dc4060b85ba98649dff395cb73addefe"
        }, {
            "08b97c580f373204e582c217cbca76c8020d5991", "49be431f72f1219cf3882c0e71df6fc0e7c985a0", "512329f87b53e46bba592133fa32382e534a7870"
        }, {
            "d62285f0c1273254eb0efc282e75c790f0ab9eac", "6f04a59bf07d170283e856120613ea2a53654f21", "4521dbc67a70b4dc38b5d97d8520fc9246034206"
        }, {
            "2ba7f707ad5f187c412de3106583c3111d668de8", "de7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9", "1c576addeee2638205d4833ef5ca5dc56984eae3"
        }, {
            "384946219bf8b430634809640eb4c7f7e1c83abd", "d35a14d20c3cea675e50ad0a255f75edda73348c", "331e7b837880a6776655da8256c47a9cda2ba972"
        }
    };

    for (uint8_t i = 0; test_msgs[i]; ++i)
    {
        for (uint8_t j = 0; keys[j]; ++j)
        {
            char *expected = sha1s[i][j];
            char *actual = (char *) HMAC_SHA1(keys[j], test_msgs[i]);

            CU_ASSERT_PTR_NOT_NULL_FATAL(actual);
            CU_ASSERT_STRING_EQUAL(actual, expected);

            if (STR_EQ(expected, actual))
            {
                PRINT("%s\n", actual);
            }
            else
            {
                fprintf(stderr, "\n");
                fprintf(stderr, "string  : -->%s<--\n", test_msgs[i]);
                fprintf(stderr, "expected: %s\n", expected);
                fprintf(stderr, "actual  : %s\n", actual);
            }

            free(actual);
        }
    }
}
