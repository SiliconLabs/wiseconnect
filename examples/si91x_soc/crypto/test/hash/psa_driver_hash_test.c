/***************************************************************************/ /**
 * @brief Hash function tests
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "psa_driver_hash_test.h"
#include "psa_driver_hash_test_data.h"

#define PARTIAL_LEN_TBL_SIZE (6)
const unsigned int partial_len_tbl[PARTIAL_LEN_TBL_SIZE] = { 1, 113, 64, 21, 9, 79 };

int unhexify(unsigned char *obuf, const char *ibuf)
{
  unsigned char c, c2;
  int len = strlen(ibuf) / 2;
  TEST_ASSERT(strlen(ibuf) % 2 == 0); // must be even number of bytes

  while (*ibuf != 0) {
    c = *ibuf++;
    if (c >= '0' && c <= '9') {
      c -= '0';
    } else if (c >= 'a' && c <= 'f') {
      c -= 'a' - 10;
    } else if (c >= 'A' && c <= 'F') {
      c -= 'A' - 10;
    } else {
      TEST_ASSERT(0);
    }

    c2 = *ibuf++;
    if (c2 >= '0' && c2 <= '9') {
      c2 -= '0';
    } else if (c2 >= 'a' && c2 <= 'f') {
      c2 -= 'a' - 10;
    } else if (c2 >= 'A' && c2 <= 'F') {
      c2 -= 'A' - 10;
    } else {
      TEST_ASSERT(0);
    }

    *obuf++ = (c << 4) | c2;
  }

  goto exit;
exit:
  return len;
}

#if defined(TEST_SL_SHA1_SHORT)
void test_sl_sha1_short(void)
{
  unsigned char input[2048 / 8];
  unsigned char output[20];
  unsigned char expected_output[20];
  uint32_t len;
  size_t out_len;
  psa_status_t status;

  for (uint32_t i = 0; i < (sizeof(sha1_short_messages) / sizeof(sl_sha_test_vector_t)) * 2; i++) {
    memset(input, 0x00, sizeof(input));
    memset(output, 0x00, sizeof(output));
    memset(expected_output, 0x00, sizeof(expected_output));
    out_len = 0;

    uint8_t *in_buf = (uint8_t *)input;
    in_buf += i & 0x1;

    len = unhexify(in_buf, sha1_short_messages[i / 2].msg);
    status =
      TRANSPARENT_HASH_COMPUTE_FCT(PSA_ALG_SHA_1, (const uint8_t *)in_buf, len, output, sizeof(output), &out_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    if ((i & 0x1) == 0) {
      UnityPrintf("SHA-1 HASH bytes:%d\n", len);
    } else {
      UnityPrintf("SHA-1 HASH (unaligned) bytes:%d\n", len);
    }
    len = unhexify(expected_output, sha1_short_messages[i / 2].expectedDigest);
    TEST_ASSERT_EQUAL(out_len, len);
    TEST_ASSERT_EQUAL(0, memcmp(output, expected_output, len));
  }

  goto exit;
exit:
  return;
}
#endif // TEST_SL_SHA1_SHORT

#if defined(TEST_SL_SHA1_LONG)
void test_sl_sha1_long(void)
{
  unsigned char input[52000 / 8];
  unsigned char output[20];
  unsigned char expected_output[20];
  uint32_t len;
  psa_status_t status;
  size_t out_len;

  for (uint32_t i = 0; i < (sizeof(sha1_long_messages) / sizeof(sl_sha_test_vector_t)) * 2; i++) {
    memset(input, 0x00, sizeof(input));
    memset(output, 0x00, sizeof(output));
    memset(expected_output, 0x00, sizeof(expected_output));
    out_len = 0;

    uint8_t *in_buf = (uint8_t *)input;
    in_buf += i & 0x1;

    len = unhexify(in_buf, sha1_long_messages[i / 2].msg);

    status =
      TRANSPARENT_HASH_COMPUTE_FCT(PSA_ALG_SHA_1, (const uint8_t *)in_buf, len, output, sizeof(output), &out_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    if ((i & 0x1) == 0) {
      UnityPrintf("SHA-1 HASH bytes:%d\n", len);
    } else {
      UnityPrintf("SHA-1 HASH (unaligned) bytes:%d\n", len);
    }
    len = unhexify(expected_output, sha1_long_messages[i / 2].expectedDigest);
    TEST_ASSERT_EQUAL(out_len, len);
    TEST_ASSERT_EQUAL(0, memcmp(output, expected_output, len));
  }

  goto exit;
exit:
  return;
}

#endif // TEST_SL_SHA1_LONG

#if defined(TEST_SL_SHA256_SHORT)
void test_sl_sha256_short(void)
{
  unsigned char input[2048 / 8];
  unsigned char output[32];
  unsigned char expected_output[32];
  uint32_t len;
  psa_status_t status;
  size_t out_len;

  for (uint32_t i = 0; i < (sizeof(sha256_short_messages) / sizeof(sl_sha_test_vector_t)) * 2; i++) {
    memset(input, 0x00, sizeof(input));
    memset(output, 0x00, sizeof(output));
    memset(expected_output, 0x00, sizeof(expected_output));
    out_len = 0;

    uint8_t *in_buf = (uint8_t *)input;
    in_buf += i & 0x1;

    len = unhexify(in_buf, sha256_short_messages[i / 2].msg);

    status = TRANSPARENT_HASH_COMPUTE_FCT(PSA_ALG_SHA_256, in_buf, len, output, sizeof(output), &out_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    if ((i & 0x1) == 0) {
      UnityPrintf("SHA-256 HASH bytes:%d\n", len);
    } else {
      UnityPrintf("SHA-256 HASH (unaligned) bytes:%d\n", len);
    }
    len = unhexify(expected_output, sha256_short_messages[i / 2].expectedDigest);
    TEST_ASSERT_EQUAL(out_len, len);
    TEST_ASSERT_EQUAL(0, memcmp(output, expected_output, len));
  }

  goto exit;
exit:
  return;
}
#endif // TEST_SL_SHA256_SHORT

#if defined(TEST_SL_SHA256_LONG)
void test_sl_sha256_long(void)
{
  unsigned char input[52000 / 8];
  unsigned char output[32];
  unsigned char expected_output[32];
  uint32_t len;
  psa_status_t status;
  size_t out_len;

  for (uint32_t i = 0; i < (sizeof(sha256_long_messages) / sizeof(sl_sha_test_vector_t)) * 2; i++) {
    memset(input, 0x00, sizeof(input));
    memset(output, 0x00, sizeof(output));
    memset(expected_output, 0x00, sizeof(expected_output));
    out_len = 0;

    uint8_t *in_buf = (uint8_t *)input;
    in_buf += i & 0x1;

    len = unhexify(in_buf, sha256_long_messages[i / 2].msg);

    status = TRANSPARENT_HASH_COMPUTE_FCT(PSA_ALG_SHA_256, in_buf, len, output, sizeof(output), &out_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    if ((i & 0x1) == 0) {
      UnityPrintf("SHA-256 HASH bytes:%d\n", len);
    } else {
      UnityPrintf("SHA-256 HASH (unaligned) bytes::%d\n", len);
    }
    len = unhexify(expected_output, sha256_long_messages[i / 2].expectedDigest);
    TEST_ASSERT_EQUAL(out_len, len);
    TEST_ASSERT_EQUAL(0, memcmp(output, expected_output, len));
  }

  goto exit;
exit:
  return;
}

#endif // TEST_SL_SHA256_LONG

#if defined(TEST_SL_SHA224_SHORT)
void test_sl_sha224_short(void)
{
  unsigned char input[2048 / 8];
  unsigned char output[28];
  unsigned char expected_output[28];
  uint32_t len;
  psa_status_t status;
  size_t out_len;

  for (uint32_t i = 0; i < (sizeof(sha224_short_messages) / sizeof(sl_sha_test_vector_t)) * 2; i++) {
    memset(input, 0x00, sizeof(input));
    memset(output, 0x00, sizeof(output));
    memset(expected_output, 0x00, sizeof(expected_output));
    out_len = 0;

    uint8_t *in_buf = (uint8_t *)input;
    in_buf += i & 0x1;

    len = unhexify(in_buf, sha224_short_messages[i / 2].msg);

    status =
      TRANSPARENT_HASH_COMPUTE_FCT(PSA_ALG_SHA_224, (const uint8_t *)in_buf, len, output, sizeof(output), &out_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    if ((i & 0x1) == 0) {
      UnityPrintf("SHA-224 HASH bytes:%d\n", len);
    } else {
      UnityPrintf("SHA-224 HASH (unaligned) bytes:%d\n", len);
    }
    len = unhexify(expected_output, sha224_short_messages[i / 2].expectedDigest);
    TEST_ASSERT_EQUAL(out_len, len);
    TEST_ASSERT_EQUAL(0, memcmp(output, expected_output, len));
  }

  goto exit;
exit:
  return;
}
#endif // TEST_SL_SHA224_SHORT

#if defined(TEST_SL_SHA224_LONG)
void test_sl_sha224_long(void)
{
  unsigned char input[52000 / 8];
  unsigned char output[28];
  unsigned char expected_output[28];
  uint32_t len;
  psa_status_t status;
  size_t out_len;

  for (uint32_t i = 0; i < (sizeof(sha224_long_messages) / sizeof(sl_sha_test_vector_t)) * 2; i++) {
    memset(input, 0x00, sizeof(input));
    memset(output, 0x00, sizeof(output));
    memset(expected_output, 0x00, sizeof(expected_output));
    out_len = 0;

    uint8_t *in_buf = (uint8_t *)input;
    in_buf += i & 0x1;

    len = unhexify(in_buf, sha224_long_messages[i / 2].msg);

    status =
      TRANSPARENT_HASH_COMPUTE_FCT(PSA_ALG_SHA_224, (const uint8_t *)in_buf, len, output, sizeof(output), &out_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    if ((i & 0x1) == 0) {
      UnityPrintf("SHA-224 HASH bytes:%d\n", len);
    } else {
      UnityPrintf("SHA-224 HASH (unaligned) bytes:%d\n", len);
    }

    len = unhexify(expected_output, sha224_long_messages[i / 2].expectedDigest);
    TEST_ASSERT_EQUAL(out_len, len);
    TEST_ASSERT_EQUAL(0, memcmp(output, expected_output, len));
  }

  goto exit;
exit:
  return;
}

#endif // TEST_SL_SHA224_LONG

#if defined(TEST_SL_SI91X_SHA384_SHORT)
void test_sl_sha384_short(void)
{
  unsigned char input[2048 / 8];
  unsigned char output[48];
  unsigned char expected_output[48];
  uint32_t len;
  psa_status_t status;
  size_t out_len;

  for (uint32_t i = 0; i < (sizeof(sha384_short_messages) / sizeof(sl_sha_test_vector_t)) * 2; i++) {
    memset(input, 0x00, sizeof(input));
    memset(output, 0x00, sizeof(output));
    memset(expected_output, 0x00, sizeof(expected_output));
    out_len = 0;

    uint8_t *in_buf = (uint8_t *)input;
    in_buf += i & 0x1;

    len = unhexify(in_buf, sha384_short_messages[i / 2].msg);

    status =
      TRANSPARENT_HASH_COMPUTE_FCT(PSA_ALG_SHA_384, (const uint8_t *)in_buf, len, output, sizeof(output), &out_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    if ((i & 0x1) == 0) {
      UnityPrintf("SHA-384 HASH bytes:%d\n", len);
    } else {
      UnityPrintf("SHA-384 HASH (unaligned) bytes:%d\n", len);
    }
    len = unhexify(expected_output, sha384_short_messages[i / 2].expectedDigest);
    TEST_ASSERT_EQUAL(out_len, len);
    TEST_ASSERT_EQUAL(0, memcmp(output, expected_output, len));
  }

  goto exit;
exit:
  return;
}
#endif // #if defined(TEST_SL_SI91X_SHA384_SHORT)
#if defined(TEST_SL_SI91X_SHA384_LONG)
void test_sl_sha384_long(void)
{
  unsigned char input[52000 / 8];
  unsigned char output[48];
  unsigned char expected_output[48];
  uint32_t len;
  psa_status_t status;
  size_t out_len;

  for (uint32_t i = 0; i < (sizeof(sha384_long_messages) / sizeof(sl_sha_test_vector_t)) * 2; i++) {
    memset(input, 0x00, sizeof(input));
    memset(output, 0x00, sizeof(output));
    memset(expected_output, 0x00, sizeof(expected_output));
    out_len = 0;

    uint8_t *in_buf = (uint8_t *)input;
    in_buf += i & 0x1;

    len = unhexify(in_buf, sha384_long_messages[i / 2].msg);

    status =
      TRANSPARENT_HASH_COMPUTE_FCT(PSA_ALG_SHA_384, (const uint8_t *)in_buf, len, output, sizeof(output), &out_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    if ((i & 0x1) == 0) {
      UnityPrintf("SHA-384 HASH bytes:%d\n", len);
    } else {
      UnityPrintf("SHA-384 HASH (unaligned) bytes:%d\n", len);
    }
    len = unhexify(expected_output, sha384_long_messages[i / 2].expectedDigest);
    TEST_ASSERT_EQUAL(out_len, len);
    TEST_ASSERT_EQUAL(0, memcmp(output, expected_output, len));
  }

  goto exit;
exit:
  return;
}

#endif // (TEST_SL_SI91X_SHA384_LONG)

#if defined(TEST_SL_SI91X_SHA512_SHORT)
void test_sl_sha512_short(void)
{
  unsigned char input[2048 / 8];
  unsigned char output[64];
  unsigned char expected_output[64];
  uint32_t len;
  psa_status_t status;
  size_t out_len;

  for (uint32_t i = 0; i < (sizeof(sha512_short_messages) / sizeof(sl_sha_test_vector_t)) * 2; i++) {
    memset(input, 0x00, sizeof(input));
    memset(output, 0x00, sizeof(output));
    memset(expected_output, 0x00, sizeof(expected_output));
    out_len = 0;

    uint8_t *in_buf = (uint8_t *)input;
    in_buf += i & 0x1;

    len = unhexify(in_buf, sha512_short_messages[i / 2].msg);

    status =
      TRANSPARENT_HASH_COMPUTE_FCT(PSA_ALG_SHA_512, (const uint8_t *)in_buf, len, output, sizeof(output), &out_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    if ((i & 0x1) == 0) {
      UnityPrintf("SHA-512 HASH bytes:%d\n", len);
    } else {
      UnityPrintf("SHA-512 HASH (unaligned) bytes:%d\n", len);
    }
    len = unhexify(expected_output, sha512_short_messages[i / 2].expectedDigest);
    TEST_ASSERT_EQUAL(out_len, len);
    TEST_ASSERT_EQUAL(0, memcmp(output, expected_output, len));
  }

  goto exit;
exit:
  return;
}
#endif // TEST_SL_SI91X_SHA512_SHORT
#if defined(TEST_SL_SI91X_SHA512_LONG)
void test_sl_sha512_long(void)
{
  unsigned char input[52000 / 8];
  unsigned char output[64];
  unsigned char expected_output[64];
  uint32_t len;
  psa_status_t status;
  size_t out_len;

  for (uint32_t i = 0; i < (sizeof(sha512_long_messages) / sizeof(sl_sha_test_vector_t)) * 2; i++) {
    memset(input, 0x00, sizeof(input));
    memset(output, 0x00, sizeof(output));
    memset(expected_output, 0x00, sizeof(expected_output));
    out_len = 0;

    uint8_t *in_buf = (uint8_t *)input;
    in_buf += i & 0x1;

    len = unhexify(in_buf, sha512_long_messages[i / 2].msg);

    status =
      TRANSPARENT_HASH_COMPUTE_FCT(PSA_ALG_SHA_512, (const uint8_t *)in_buf, len, output, sizeof(output), &out_len);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    if ((i & 0x1) == 0) {
      UnityPrintf("SHA-512 HASH bytes:%d\n", len);
    } else {
      UnityPrintf("SHA-512 HASH (unaligned) bytes:%d\n", len);
    }
    len = unhexify(expected_output, sha512_long_messages[i / 2].expectedDigest);
    TEST_ASSERT_EQUAL(out_len, len);
    TEST_ASSERT_EQUAL(0, memcmp(output, expected_output, len));
  }

  goto exit;
exit:
  return;
}

#endif // (TEST_SL_SI91X_SHA512_LONG))

/*
 * @brief
 *   main function
 */
int test_psa_driver_hash(void)
{
#if defined(UNITY_OUTPUT_SERIAL)
  UNITY_UsartSetup();
#endif

  UnityBeginGroup("psa-driver-hash");

#if defined(TEST_SL_SHA1_SHORT)
  RUN_TEST(test_sl_sha1_short, __LINE__);
#endif // TEST_SL_SHA1_SHORT

#if defined(TEST_SL_SHA1_LONG)
  RUN_TEST(test_sl_sha1_long, __LINE__);
#endif // TEST_SL_SHA1_LONG

#if defined(TEST_SL_SHA224_SHORT)
  RUN_TEST(test_sl_sha224_short, __LINE__);
#endif // TEST_SL_SHA224_SHORT

#if defined(TEST_SL_SHA224_LONG)
  RUN_TEST(test_sl_sha224_long, __LINE__);
#endif // TEST_SL_SHA224_LONG

#if defined(TEST_SL_SHA256_SHORT)
  RUN_TEST(test_sl_sha256_short, __LINE__);
#endif // TEST_SL_SHA256_SHORT

#if defined(TEST_SL_SHA256_LONG)
  RUN_TEST(test_sl_sha256_long, __LINE__);
#endif // TEST_SL_SHA256_LONG

#if defined(TEST_SL_SI91X_SHA384_SHORT)
  RUN_TEST(test_sl_sha384_short, __LINE__);
#endif
#if defined(TEST_SL_SI91X_SHA384_LONG)
  RUN_TEST(test_sl_sha384_long, __LINE__);
#endif

#if defined(TEST_SL_SI91X_SHA512_SHORT)
  RUN_TEST(test_sl_sha512_short, __LINE__);
#endif // TEST_SL_SI91X_SHA512_SHORT
#if defined(TEST_SL_SI91X_SHA512_LONG)
  RUN_TEST(test_sl_sha512_long, __LINE__);
#endif // TEST_SL_SI91X_SHA512_LONG

  UnityEnd();

#if defined(ENABLE_TEST_COVERAGE)
  __gcov_flush();
#endif

  UnityPrint("ENDSWO");
  while (1)
    ;
}
