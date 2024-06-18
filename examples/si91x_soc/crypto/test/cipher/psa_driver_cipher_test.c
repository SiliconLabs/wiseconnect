/***************************************************************************/ /**
 * @brief Test program for the cipher entry points of our PSA drivers
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

#include "psa_driver_cipher_test.h"
#include "psa_driver_cipher_test_data.h"

/*******************************************************************************
 ******************************   AES TEST   ***********************************
 ******************************************************************************/

// Needed in order to force IV generation to expected output value.
static char *expected_iv;

/*
 * @brief
 *   Convert ascii test vectors as defined above into binary format.
 *
 */
static int cipher_read_bytes(uint8_t *buf, unsigned int buflen, const char *str)
{
  int ret = 0;
  int i;
  uint8_t tmp;
  uint8_t val;

  while (ret < (int)buflen) {
    val = 0;
    for (i = 1; i >= 0; i--) {
      tmp = *(str++);

      /* Skip whitespace */
      while (tmp == ' ' || tmp == '\r' || tmp == '\n') {
        tmp = *(str++);
      }

      /* Reached end of string? */
      if (tmp == 0) {
        goto done;
      }

      if (tmp > '9') {
        /* Ensure uppercase hex */
        tmp &= ~0x20;

        val |= ((tmp - 'A') + 10) << (4 * i);
      } else {
        val |= (tmp - '0') << (4 * i);
      }
    }

    *(buf++) = val;
    ret++;
  }

done:
  // Check for buffer overflow: either the string terminated before filling
  // the local buffer, or the string terminates exactly when the buffer has
  // been filled.
  TEST_ASSERT(i == 1 || i == -1 || buflen == 0);
  TEST_ASSERT((ret < (int)buflen) || (*str == 0) || buflen == 0);

  goto exit;
exit:
  return ret;
}

static inline void sl_psa_set_key_type(psa_key_attributes_t *attributes, psa_key_type_t type)
{
  /* Common case: quick path */
  attributes->MBEDTLS_PRIVATE(core).MBEDTLS_PRIVATE(type) = type;
}

/*
 * @brief
 *   Test AES with the mode and test vector supplied.
 *
 * @param[in] output_offset
 *   The number of bytes to offset the output buffer relative to the the input
 *   buffer. This is used to test if partial overlap between input/output
 *   buffers work. Limited to 128 bytes for test space usage reasons.
 */
static void test_sl_psa_driver_aes(uint32_t psa_alg_mode,
                                   char *mode_string,
                                   const sl_aes_test_vector_t *test_vector,
                                   size_t test_vector_size,
                                   uint32_t output_offset)
{
  uint8_t text[16 + AES_TEXT_MAX_LEN + AES_TEXT_MAX_OFFSET];
  uint8_t expected[16 + AES_TEXT_MAX_LEN];
  uint8_t key[32 + 1];
  uint8_t read_iv[16];
  uint8_t *key_ptr        = key;
  size_t unaligned_offset = 0;
  psa_status_t status;

  if (output_offset > AES_TEXT_MAX_OFFSET) {
    TEST_FAIL_MESSAGE("Invalid output_offset value");
  }

  for (uint32_t i = 0; i < test_vector_size * 2; i++) {
    psa_key_attributes_t attributes = psa_key_attributes_init();
    size_t key_len                  = test_vector[i / 2].key_len;

    if (i & 0x1) {
      unaligned_offset = 1;
      UnityPrintf("Testing AES-%d %s (unaligned, offset=%d)\n",
                  test_vector[i / 2].key_len * 8,
                  mode_string,
                  output_offset);
    } else {
      unaligned_offset = 0;
      UnityPrintf("Testing AES-%d %s (offset=%d)\n", test_vector[i / 2].key_len * 8, mode_string, output_offset);
    }

    for (test_key_type_t test_key_type = plaintext; test_key_type < invalid_key_type; test_key_type++) {

      key_ptr = &key[unaligned_offset];

      // No IV for ECB-mode.
      if (psa_alg_mode != PSA_ALG_ECB_NO_PADDING) {
        expected_iv = test_vector[i / 2].iv;
        cipher_read_bytes(read_iv, test_vector[i / 2].iv_len, expected_iv);
      }

      //////////////////
      // Encrypt Test //
      //////////////////
      cipher_read_bytes(&key[unaligned_offset], test_vector[i / 2].key_len, test_vector[i / 2].key);
      cipher_read_bytes(&text[unaligned_offset], test_vector[i / 2].text_len, test_vector[i / 2].plain_text);
      size_t output_bytes;

      psa_set_key_algorithm(&attributes, psa_alg_mode);
      psa_set_key_bits(&attributes, test_vector[i / 2].key_len * 8);
      sl_psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);

      status = TRANSPARENT_CIPHER_ENCRYPT_FCT(&attributes,
                                              key_ptr,
                                              key_len,
                                              psa_alg_mode,
                                              (psa_alg_mode != PSA_ALG_ECB_NO_PADDING ? read_iv : NULL),
                                              (psa_alg_mode != PSA_ALG_ECB_NO_PADDING ? test_vector[i / 2].iv_len : 0),
                                              &text[unaligned_offset],
                                              test_vector[i / 2].text_len,
                                              &text[unaligned_offset + output_offset],
                                              sizeof(text) - unaligned_offset - output_offset,
                                              &output_bytes);

      TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

      uint32_t iv_output_len = (psa_alg_mode == PSA_ALG_ECB_NO_PADDING) ? 0 : test_vector[i / 2].iv_len;
      TEST_ASSERT_EQUAL(output_bytes, test_vector[i / 2].cipher_len);

      UnityPrintf(" - AES-%d %s encrypt bytes:%d\n",
                  test_vector[i / 2].key_len * 8,
                  mode_string,
                  test_vector[i / 2].text_len);

      cipher_read_bytes(expected, test_vector[i / 2].cipher_len, test_vector[i / 2].cipher_text);
      TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, &text[unaligned_offset + output_offset], test_vector[i / 2].cipher_len);

      //////////////////
      // Decrypt Test //
      //////////////////

      cipher_read_bytes(&text[unaligned_offset], iv_output_len, test_vector[i / 2].iv);
      cipher_read_bytes(&text[unaligned_offset + iv_output_len],
                        test_vector[i / 2].cipher_len,
                        test_vector[i / 2].cipher_text);

      status = TRANSPARENT_CIPHER_DECRYPT_FCT(&attributes,
                                              key_ptr,
                                              key_len,
                                              psa_alg_mode,
                                              &text[unaligned_offset],
                                              iv_output_len + test_vector[i / 2].cipher_len,
                                              &text[unaligned_offset + output_offset],
                                              sizeof(text) - unaligned_offset - output_offset,
                                              &output_bytes);
      TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
      TEST_ASSERT_EQUAL(output_bytes, test_vector[i / 2].text_len);

      UnityPrintf(" - AES-%d %s decrypt bytes:%d\n",
                  test_vector[i / 2].key_len * 8,
                  mode_string,
                  test_vector[i / 2].text_len);

      cipher_read_bytes(expected, test_vector[i / 2].text_len, test_vector[i / 2].plain_text);
      TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, &text[unaligned_offset + output_offset], test_vector[i / 2].text_len);
    }
  }
  goto exit;
exit:
  return;
}

/*
 * @brief
 *   Test the AES CBC (Cipher-block chaining) cipher mode without padding with
 *   non-zero offsets between the input and output buffers.
 *
 */
static void test_sl_psa_driver_aes_cbc_no_padding_offsets(void)
{
  uint32_t offsets[3] = { 17, 32, 223 };
  for (uint32_t i = 0; i < sizeof(offsets) / sizeof(uint32_t); ++i) {
    test_sl_psa_driver_aes(PSA_ALG_CBC_NO_PADDING,
                           "CBC (NO PADDING)",
                           aes_cbc_no_padding_test_vector,
                           sizeof(aes_cbc_no_padding_test_vector) / sizeof(sl_aes_test_vector_t),
                           offsets[i]);
  }
}

/*
 * @brief
 *   Test the AES CBC (Cipher-block chaining) cipher mode without padding.
 *
 */
static void test_sl_psa_driver_aes_cbc_no_padding(void)
{
  test_sl_psa_driver_aes(PSA_ALG_CBC_NO_PADDING,
                         "CBC (NO PADDING)",
                         aes_cbc_no_padding_test_vector,
                         sizeof(aes_cbc_no_padding_test_vector) / sizeof(sl_aes_test_vector_t),
                         0);
}

/*
 * @brief
 *   Test the AES CTR (Counter) cipher mode.
 *
 */
static void test_sl_psa_driver_aes_ctr(void)
{
  test_sl_psa_driver_aes(PSA_ALG_CTR,
                         "CTR",
                         aes_ctr_test_vector,
                         sizeof(aes_ctr_test_vector) / sizeof(sl_aes_test_vector_t),
                         0);
}

/*
 * @brief
 *   Test the AES ECB (Electronic codebook) cipher mode.
 *
 */
static void test_sl_psa_driver_aes_ecb(void)
{
  test_sl_psa_driver_aes(PSA_ALG_ECB_NO_PADDING,
                         "ECB",
                         aes_ecb_test_vector,
                         sizeof(aes_ecb_test_vector) / sizeof(sl_aes_test_vector_t),
                         0);
}

int test_psa_driver_cipher(void)
{

#if defined(UNITY_OUTPUT_SERIAL)
  UNITY_UsartSetup();
#endif

  UnityBeginGroup("psa-driver-cipher");
  // External Plain Text Keys
  UnityPrintf("\n# PSA Crypto Driver Cipher Tests\n---\n");
  RUN_TEST(test_sl_psa_driver_aes_cbc_no_padding, __LINE__);

  RUN_TEST(test_sl_psa_driver_aes_ctr, __LINE__);

  RUN_TEST(test_sl_psa_driver_aes_ecb, __LINE__);

  RUN_TEST(test_sl_psa_driver_aes_cbc_no_padding_offsets, __LINE__);

  UnityPrintf("\n");

  UnityEnd();

#if defined(ENABLE_TEST_COVERAGE)
  __gcov_flush();
#endif

  UnityPrint("ENDSWO");
  while (1)
    ;
}
