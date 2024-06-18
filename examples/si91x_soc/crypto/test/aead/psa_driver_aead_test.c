/***************************************************************************/ /**
 * @file psa_driver_aead_test.c
 * @brief AEAD function test
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

#include "psa_driver_aead_test.h"
#include "psa_driver_aead_test_data.h"

// -----------------------------------------------------------------------------
// Helpers

/*
 * @brief
 *   Convert ascii test vectors as defined above into binary format.
 *
 */
static int aes_read_bytes(uint8_t *buf, unsigned int buflen, const char *str)
{
  int ret = 0;
  int i;
  uint8_t tmp;
  uint8_t val;

  while (ret < (int)buflen) {
    val = 0;
    for (i = 1; i >= 0; i--) {
      tmp = *(str++);

      /* Skip spaces */
      while (tmp == ' ') {
        tmp = *(str++);
      }

      /* Reached end of string? */
      if (!tmp) {
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

  return ret;
}

/*
 * @brief
 *   Convert ascii test vectors as defined above into binary format.
 *
 */
static int ascii_read_bytes(uint8_t *buf, unsigned int buflen, const char *str)
{
  int ret = 0;
  int i;
  uint8_t tmp;
  uint8_t val;

  while (ret < (int)buflen) {
    val = 0;
    for (i = 1; i >= 0; i--) {
      tmp = *(str++);

      /* Skip spaces */
      while (tmp == ' ') {
        tmp = *(str++);
      }

      /* Reached end of string? */
      if (!tmp) {
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

  return ret;
}

static inline void sl_psa_set_key_type(psa_key_attributes_t *attributes, psa_key_type_t type)
{
  /* Common case: quick path */
  attributes->MBEDTLS_PRIVATE(core).MBEDTLS_PRIVATE(type) = type;
}

// -----------------------------------------------------------------------------
// Tests

static void test_psa_driver_ccm(void)
{
  uint8_t text[AES_TEXT_MAX_LEN + 16 + AES_TEXT_MAX_OFFSET];
  uint8_t expected[AES_TEXT_MAX_LEN];
  uint8_t key_buffer[36];
  uint8_t *key = key_buffer + 1;
  uint8_t nonce[13];
  psa_status_t status;
  size_t out_len;
  psa_key_attributes_t attributes = psa_key_attributes_init();

  uint32_t output_offset[] = { 0, 17, 223 };

  for (uint32_t j = 0; j < sizeof(output_offset) / sizeof(uint32_t); j++) {
    for (test_key_type_t k = plaintext; k < invalid_key_type; k++) {
      for (uint32_t i = 0; i < sizeof(aes_ccm_test_vector) / sizeof(sli_aes_test_vector_t); i++) {
        //////////////////
        // Encrypt Test //
        //////////////////
        aes_read_bytes(key, aes_ccm_test_vector[i].key_len, aes_ccm_test_vector[i].key);
        aes_read_bytes(nonce, aes_ccm_test_vector[i].nonce_len, aes_ccm_test_vector[i].nonce);
        aes_read_bytes(text, aes_ccm_test_vector[i].text_len, aes_ccm_test_vector[i].plain_text);

        psa_set_key_bits(&attributes, aes_ccm_test_vector[i].key_len * 8);
        sl_psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
        psa_set_key_algorithm(&attributes, PSA_ALG_CCM);
        psa_set_key_lifetime(&attributes, 0);

        status = TRANSPARENT_AEAD_ENCRYPT(
          &attributes,
          key,
          aes_ccm_test_vector[i].key_len,
          PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
          nonce,
          aes_ccm_test_vector[i].nonce_len,
          (const unsigned char *)aes_ccm_test_vector[i].add_data,
          aes_ccm_test_vector[i].add_data_len,
          text,
          aes_ccm_test_vector[i].text_len,
          text + output_offset[j],
          sizeof(text),
          &out_len);

        UnityPrintf("AES-%d CCM encrypt bytes:%2d, (offset=%d)\n",
                    aes_ccm_test_vector[i].key_len * 8,
                    aes_ccm_test_vector[i].text_len,
                    output_offset[j]);
        aes_read_bytes(expected, aes_ccm_test_vector[i].cipher_len, aes_ccm_test_vector[i].cipher_text);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, text + output_offset[j], aes_ccm_test_vector[i].cipher_len);
        aes_read_bytes(expected, aes_ccm_test_vector[i].mac_data_len, aes_ccm_test_vector[i].mac_data);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected,
                                     &((text + output_offset[j])[aes_ccm_test_vector[i].cipher_len]),
                                     aes_ccm_test_vector[i].mac_data_len);

        //////////////////
        // Decrypt Test //
        //////////////////
        memmove(text, text + output_offset[j], out_len);

        status = TRANSPARENT_AEAD_DECRYPT(
          &attributes,
          key,
          aes_ccm_test_vector[i].key_len,
          PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
          nonce,
          aes_ccm_test_vector[i].nonce_len,
          (const unsigned char *)aes_ccm_test_vector[i].add_data,
          aes_ccm_test_vector[i].add_data_len,
          text,
          out_len,
          text + output_offset[j],
          sizeof(text),
          &out_len);

        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        TEST_ASSERT_EQUAL(aes_ccm_test_vector[i].text_len, out_len);

        UnityPrintf("AES-%d CCM decrypt bytes:%2d, (offset=%d)\n",
                    aes_ccm_test_vector[i].key_len * 8,
                    aes_ccm_test_vector[i].text_len,
                    output_offset[j]);
        aes_read_bytes(expected, aes_ccm_test_vector[i].text_len, aes_ccm_test_vector[i].plain_text);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, text + output_offset[j], aes_ccm_test_vector[i].text_len);
      }
    }
  }
  goto exit;
exit:
  return;
}

static void test_psa_driver_ccm_invalid_param(void)
{
  uint8_t text[AES_TEXT_MAX_LEN + 16 + AES_TEXT_MAX_OFFSET];
  uint8_t expected[AES_TEXT_MAX_LEN];
  uint8_t key[32];
  uint8_t nonce[13];
  psa_status_t status;
  size_t out_len;
  psa_key_attributes_t attributes = psa_key_attributes_init();
  psa_key_type_t key_type;
  size_t key_len;
  uint32_t output_offset[] = { 0, 17, 223 };
  size_t orig_out_len;

  uint32_t j = 0;
  uint32_t i = 0;

  //////////////////
  // Encrypt Test //
  //////////////////
  aes_read_bytes(key, aes_ccm_test_vector[i].key_len, aes_ccm_test_vector[i].key);
  aes_read_bytes(nonce, aes_ccm_test_vector[i].nonce_len, aes_ccm_test_vector[i].nonce);
  aes_read_bytes(text, aes_ccm_test_vector[i].text_len, aes_ccm_test_vector[i].plain_text);

  psa_set_key_bits(&attributes, aes_ccm_test_vector[i].key_len * 8);
  sl_psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
  psa_set_key_algorithm(&attributes, PSA_ALG_CCM);
  psa_set_key_lifetime(&attributes, 0);
  status = TRANSPARENT_AEAD_ENCRYPT(
    &attributes,
    key,
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    aes_ccm_test_vector[i].nonce_len,
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    text,
    aes_ccm_test_vector[i].text_len,
    text + output_offset[j],
    sizeof(text),
    &out_len);

  TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
  TEST_ASSERT_EQUAL(aes_ccm_test_vector[i].cipher_len + aes_ccm_test_vector[i].mac_data_len, out_len);
  UnityPrintf("AES-%d CCM encrypt bytes:%2d, (offset=%d)\n",
              aes_ccm_test_vector[i].key_len * 8,
              aes_ccm_test_vector[i].text_len,
              output_offset[j]);
  aes_read_bytes(expected, aes_ccm_test_vector[i].cipher_len, aes_ccm_test_vector[i].cipher_text);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, text + output_offset[j], aes_ccm_test_vector[i].cipher_len);
  aes_read_bytes(expected, aes_ccm_test_vector[i].mac_data_len, aes_ccm_test_vector[i].mac_data);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected,
                               &((text + output_offset[j])[aes_ccm_test_vector[i].cipher_len]),
                               aes_ccm_test_vector[i].mac_data_len);

  status = TRANSPARENT_AEAD_ENCRYPT(
    NULL, // attributes
    key,
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    aes_ccm_test_vector[i].nonce_len,
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    text,
    aes_ccm_test_vector[i].text_len,
    text + output_offset[j],
    sizeof(text),
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);

  status = TRANSPARENT_AEAD_ENCRYPT(
    &attributes,
    NULL, // key
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    aes_ccm_test_vector[i].nonce_len,
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    text,
    aes_ccm_test_vector[i].text_len,
    text + output_offset[j],
    sizeof(text),
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);

  status = TRANSPARENT_AEAD_ENCRYPT(
    &attributes,
    key,
    0, // key_buffer_size
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    aes_ccm_test_vector[i].nonce_len,
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    text,
    aes_ccm_test_vector[i].text_len,
    text + output_offset[j],
    sizeof(text),
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);

  status = TRANSPARENT_AEAD_ENCRYPT(
    &attributes,
    key,
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    NULL, // nonce
    aes_ccm_test_vector[i].nonce_len,
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    text,
    aes_ccm_test_vector[i].text_len,
    text + output_offset[j],
    sizeof(text),
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);

  status = TRANSPARENT_AEAD_ENCRYPT(
    &attributes,
    key,
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    0, // nonce_len
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    text,
    aes_ccm_test_vector[i].text_len,
    text + output_offset[j],
    sizeof(text),
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);

  status = TRANSPARENT_AEAD_ENCRYPT(
    &attributes,
    key,
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    aes_ccm_test_vector[i].nonce_len,
    NULL, // additional_data
    4,    // additional_data_len
    text,
    aes_ccm_test_vector[i].text_len,
    text + output_offset[j],
    sizeof(text),
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);

  status = TRANSPARENT_AEAD_ENCRYPT(
    &attributes,
    key,
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    aes_ccm_test_vector[i].nonce_len,
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    text,
    16,   // plaintext_length
    NULL, // ciphertext
    17,   // ciphertext_size
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);

  status = TRANSPARENT_AEAD_ENCRYPT(
    &attributes,
    key,
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    aes_ccm_test_vector[i].nonce_len,
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    text,
    aes_ccm_test_vector[i].text_len,
    text + output_offset[j],
    sizeof(text),
    NULL); // ciphertext_length
  TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);

  status = TRANSPARENT_AEAD_ENCRYPT(
    &attributes,
    key,
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    aes_ccm_test_vector[i].nonce_len,
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    text,
    32, // set plaintext_length bigger than ciphertext buffer
    text + output_offset[j],
    16, // set ciphertext_size less than plaintext_length
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_BUFFER_TOO_SMALL, status);

  key_len = psa_get_key_bits(&attributes);
  psa_set_key_bits(&attributes, 64);
  status = TRANSPARENT_AEAD_ENCRYPT(
    &attributes,
    key,
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    aes_ccm_test_vector[i].nonce_len,
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    text,
    aes_ccm_test_vector[i].text_len,
    text + output_offset[j],
    sizeof(text),
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);
  psa_set_key_bits(&attributes, key_len);

  key_type = psa_get_key_type(&attributes);
  psa_set_key_type(&attributes, PSA_KEY_TYPE_CHACHA20);
  status = TRANSPARENT_AEAD_ENCRYPT(
    &attributes,
    key,
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    aes_ccm_test_vector[i].nonce_len,
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    text,
    aes_ccm_test_vector[i].text_len,
    text + output_offset[j],
    sizeof(text),
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_NOT_SUPPORTED, status);
  psa_set_key_type(&attributes, key_type);

  status = TRANSPARENT_AEAD_ENCRYPT(&attributes,
                                    key,
                                    aes_ccm_test_vector[i].key_len,
                                    PSA_ALG_VENDOR_FLAG,
                                    nonce,
                                    aes_ccm_test_vector[i].nonce_len,
                                    (const unsigned char *)aes_ccm_test_vector[i].add_data,
                                    aes_ccm_test_vector[i].add_data_len,
                                    text,
                                    aes_ccm_test_vector[i].text_len,
                                    text + output_offset[j],
                                    sizeof(text),
                                    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_NOT_SUPPORTED, status);

  psa_set_key_algorithm(&attributes, PSA_ALG_VENDOR_FLAG);
  status = TRANSPARENT_AEAD_ENCRYPT(
    &attributes,
    key,
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    aes_ccm_test_vector[i].nonce_len,
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    text,
    aes_ccm_test_vector[i].text_len,
    text + output_offset[j],
    sizeof(text),
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_NOT_SUPPORTED, status);
  psa_set_key_algorithm(&attributes, PSA_ALG_CCM);

  //////////////////
  // Decrypt Test //
  //////////////////
  memmove(text, text + output_offset[j], out_len);
  status = TRANSPARENT_AEAD_DECRYPT(
    &attributes,
    key,
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    aes_ccm_test_vector[i].nonce_len,
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    text,
    aes_ccm_test_vector[i].cipher_len + aes_ccm_test_vector[i].mac_data_len,
    text + output_offset[j],
    sizeof(text),
    &out_len);
  TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
  TEST_ASSERT_EQUAL(aes_ccm_test_vector[i].text_len, out_len);

  UnityPrintf("AES-%d CCM decrypt bytes:%2d, (offset=%d)\n",
              aes_ccm_test_vector[i].key_len * 8,
              aes_ccm_test_vector[i].text_len,
              output_offset[j]);
  aes_read_bytes(expected, aes_ccm_test_vector[i].text_len, aes_ccm_test_vector[i].plain_text);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, text + output_offset[j], aes_ccm_test_vector[i].text_len);

  // bad tag
  orig_out_len = out_len;
  text[aes_ccm_test_vector[i].text_len] ^= 0xff;
  status = TRANSPARENT_AEAD_DECRYPT(
    &attributes,
    key,
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    aes_ccm_test_vector[i].nonce_len,
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    text,
    aes_ccm_test_vector[i].cipher_len + aes_ccm_test_vector[i].mac_data_len,
    text + output_offset[j],
    sizeof(text),
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_SIGNATURE, status);
  out_len = orig_out_len;

  status = TRANSPARENT_AEAD_DECRYPT(
    NULL, // attributes
    key,
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    aes_ccm_test_vector[i].nonce_len,
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    text,
    aes_ccm_test_vector[i].cipher_len + aes_ccm_test_vector[i].mac_data_len,
    text + output_offset[j],
    sizeof(text),
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);

  status = TRANSPARENT_AEAD_DECRYPT(
    &attributes,
    NULL, // key
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    aes_ccm_test_vector[i].nonce_len,
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    text,
    aes_ccm_test_vector[i].cipher_len + aes_ccm_test_vector[i].mac_data_len,
    text + output_offset[j],
    sizeof(text),
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);

  status = TRANSPARENT_AEAD_DECRYPT(
    &attributes,
    key,
    0, // key_buffer_size
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    aes_ccm_test_vector[i].nonce_len,
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    text,
    aes_ccm_test_vector[i].cipher_len + aes_ccm_test_vector[i].mac_data_len,
    text + output_offset[j],
    sizeof(text),
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);

  status = TRANSPARENT_AEAD_DECRYPT(
    &attributes,
    key,
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    NULL, // nonce
    aes_ccm_test_vector[i].nonce_len,
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    text,
    aes_ccm_test_vector[i].cipher_len + aes_ccm_test_vector[i].mac_data_len,
    text + output_offset[j],
    sizeof(text),
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);

  status = TRANSPARENT_AEAD_DECRYPT(
    &attributes,
    key,
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    aes_ccm_test_vector[i].nonce_len,
    NULL, // additional_data
    4,    // additional_data_len
    text,
    aes_ccm_test_vector[i].cipher_len + aes_ccm_test_vector[i].mac_data_len,
    text + output_offset[j],
    sizeof(text),
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);

  status = TRANSPARENT_AEAD_DECRYPT(
    &attributes,
    key,
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    aes_ccm_test_vector[i].nonce_len,
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    NULL, // ciphertext
    16,   // ciphertext_length
    text + output_offset[j],
    sizeof(text),
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);

  status = TRANSPARENT_AEAD_DECRYPT(
    &attributes,
    key,
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    aes_ccm_test_vector[i].nonce_len,
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    text,
    aes_ccm_test_vector[i].cipher_len + aes_ccm_test_vector[i].mac_data_len + 4, // ciphertext_length
    NULL,                                                                        // plaintext
    sizeof(text),                                                                // plaintext_size
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);

  status = TRANSPARENT_AEAD_DECRYPT(
    &attributes,
    key,
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    aes_ccm_test_vector[i].nonce_len,
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    text,
    aes_ccm_test_vector[i].cipher_len + aes_ccm_test_vector[i].mac_data_len,
    text + output_offset[j],
    sizeof(text),
    NULL); // plaintext_length
  TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);

  status = TRANSPARENT_AEAD_DECRYPT(
    &attributes,
    key,
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    aes_ccm_test_vector[i].nonce_len,
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    text,
    64, // set ciphertext_length bigger than plaintext buffer + tag sizes
    text + output_offset[j],
    16, // set plaintext buffer size less than ciphertext minus tag size
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_BUFFER_TOO_SMALL, status);

  key_len = psa_get_key_bits(&attributes);
  psa_set_key_bits(&attributes, 64);
  status = TRANSPARENT_AEAD_DECRYPT(
    &attributes,
    key,
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    aes_ccm_test_vector[i].nonce_len,
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    text,
    aes_ccm_test_vector[i].cipher_len + aes_ccm_test_vector[i].mac_data_len,
    text + output_offset[j],
    sizeof(text),
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);
  psa_set_key_bits(&attributes, key_len);

  key_type = psa_get_key_type(&attributes);
  psa_set_key_type(&attributes, PSA_KEY_TYPE_CHACHA20);
  status = TRANSPARENT_AEAD_DECRYPT(
    &attributes,
    key,
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    aes_ccm_test_vector[i].nonce_len,
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    text,
    aes_ccm_test_vector[i].cipher_len + aes_ccm_test_vector[i].mac_data_len,
    text + output_offset[j],
    sizeof(text),
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_NOT_SUPPORTED, status);
  psa_set_key_type(&attributes, key_type);

  status = TRANSPARENT_AEAD_DECRYPT(&attributes,
                                    key,
                                    aes_ccm_test_vector[i].key_len,
                                    PSA_ALG_VENDOR_FLAG,
                                    nonce,
                                    aes_ccm_test_vector[i].nonce_len,
                                    (const unsigned char *)aes_ccm_test_vector[i].add_data,
                                    aes_ccm_test_vector[i].add_data_len,
                                    text,
                                    aes_ccm_test_vector[i].cipher_len + aes_ccm_test_vector[i].mac_data_len,
                                    text + output_offset[j],
                                    sizeof(text),
                                    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_NOT_SUPPORTED, status);

  psa_set_key_algorithm(&attributes, PSA_ALG_VENDOR_FLAG);
  status = TRANSPARENT_AEAD_DECRYPT(
    &attributes,
    key,
    aes_ccm_test_vector[i].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), aes_ccm_test_vector[i].mac_data_len),
    nonce,
    aes_ccm_test_vector[i].nonce_len,
    (const unsigned char *)aes_ccm_test_vector[i].add_data,
    aes_ccm_test_vector[i].add_data_len,
    text,
    aes_ccm_test_vector[i].cipher_len + aes_ccm_test_vector[i].mac_data_len,
    text + output_offset[j],
    sizeof(text),
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_NOT_SUPPORTED, status);
  psa_set_key_algorithm(&attributes, PSA_ALG_CCM);

  goto exit;
exit:
  return;
}

void test_psa_driver_gcm(void)
{
  uint8_t key_buffer_real[36];
  uint8_t *key_buffer = key_buffer_real + 1;
  uint8_t iv_buffer[12];
  uint8_t in_buffer[AES_TEXT_MAX_LEN + 16];
  uint8_t aad_buffer[AES_TEXT_MAX_LEN];
  uint8_t out_buffer[AES_TEXT_MAX_LEN];
  uint8_t tag_buffer[16];
  uint8_t text[AES_TEXT_MAX_LEN + 16 + AES_TEXT_MAX_OFFSET];
  uint32_t text_offset = 0;
  psa_status_t status;
  size_t out_len;
  psa_key_attributes_t attributes = psa_key_attributes_init();

  sl_psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
  psa_set_key_algorithm(&attributes, PSA_ALG_GCM);

  for (test_key_type_t k = plaintext; k < invalid_key_type; k++) {
    for (size_t i = 0; i < 5; i++) {
      const gcm_block_vector_t *enc_vectors;
      const gcm_block_vector_t *dec_vectors;

      switch (i) {
        case 0:
          enc_vectors = gcm_enc_128_vec;
          dec_vectors = gcm_dec_128_vec;
          text_offset = 0;
          psa_set_key_bits(&attributes, 128);
          break;
        case 1:
          enc_vectors = gcm_enc_192_vec;
          dec_vectors = gcm_dec_192_vec;
          text_offset = 0;
          psa_set_key_bits(&attributes, 192);
          break;
        case 2:
          enc_vectors = gcm_enc_256_vec;
          dec_vectors = gcm_dec_256_vec;
          text_offset = 0;
          psa_set_key_bits(&attributes, 256);
          break;
        case 3:
          enc_vectors = gcm_enc_256_vec_long;
          dec_vectors = gcm_dec_256_vec_long;
          text_offset = 17;
          psa_set_key_bits(&attributes, 256);
          break;
        case 4:
          enc_vectors = gcm_enc_256_vec_long;
          dec_vectors = gcm_dec_256_vec_long;
          text_offset = 128;
          psa_set_key_bits(&attributes, 256);
          break;
        default:
          return;
      }

      UnityPrintf("Testing AES-GCM-%d encryption\n", psa_get_key_bits(&attributes));
      size_t j = 0;
      while (enc_vectors[j].key) {
        memset(text, 0, sizeof(text));

        // Read test vector data.
        ascii_read_bytes(key_buffer, enc_vectors[j].key_len, enc_vectors[j].key);
        ascii_read_bytes(iv_buffer, enc_vectors[j].iv_len, enc_vectors[j].iv);
        ascii_read_bytes(in_buffer, enc_vectors[j].in_len, enc_vectors[j].in);
        ascii_read_bytes(aad_buffer, enc_vectors[j].aad_len, enc_vectors[j].aad);
        ascii_read_bytes(out_buffer, enc_vectors[j].in_len, enc_vectors[j].out);
        ascii_read_bytes(tag_buffer, enc_vectors[j].tag_len, enc_vectors[j].tag);

        memcpy(text, in_buffer, enc_vectors[j].in_len);
        status = TRANSPARENT_AEAD_ENCRYPT(
          &attributes,
          key_buffer,
          enc_vectors[j].key_len,
          PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), enc_vectors[j].tag_len),
          iv_buffer,
          enc_vectors[j].iv_len,
          aad_buffer,
          enc_vectors[j].aad_len,
          text,
          enc_vectors[j].in_len,
          text + text_offset,
          sizeof(text),
          &out_len);

        UnityPrintf("AES-%d GCM encrypt bytes:%2d, (offset=%d)\n",
                    enc_vectors[j].key_len * 8,
                    enc_vectors[j].in_len,
                    text_offset);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        TEST_ASSERT(enc_vectors[j].in_len + enc_vectors[j].tag_len <= out_len);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(out_buffer, text + text_offset, enc_vectors[j].in_len);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(&((text + text_offset)[enc_vectors[j].in_len]),
                                     tag_buffer,
                                     enc_vectors[j].tag_len);

        // Check for inverse operation
        memmove(text, text + text_offset, out_len);

        status = TRANSPARENT_AEAD_DECRYPT(
          &attributes,
          key_buffer,
          enc_vectors[j].key_len,
          PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), enc_vectors[j].tag_len),
          iv_buffer,
          enc_vectors[j].iv_len,
          aad_buffer,
          enc_vectors[j].aad_len,
          text,
          out_len,
          text + text_offset,
          sizeof(text),
          &out_len);

        UnityPrintf("AES-%d GCM decrypt bytes:%2d, (offset=%d)\n",
                    enc_vectors[j].key_len * 8,
                    enc_vectors[j].in_len,
                    text_offset);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        TEST_ASSERT_EQUAL(enc_vectors[j].in_len, out_len);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(in_buffer, text + text_offset, enc_vectors[j].in_len);

        j++;
      }

      j = 0;
      UnityPrintf("Testing AES-GCM-%d decryption\n", psa_get_key_bits(&attributes));
      while (dec_vectors[j].key) {
        // PSA crypto one-shot API does not support arbitrary tag length
        if (dec_vectors[j].tag_len < 4 || dec_vectors[j].tag_len > 16) {
          j++;
          continue;
        }

        memset(text, 0, sizeof(text));

        // Read test vector data.
        ascii_read_bytes(key_buffer, dec_vectors[j].key_len, dec_vectors[j].key);
        ascii_read_bytes(iv_buffer, dec_vectors[j].iv_len, dec_vectors[j].iv);
        ascii_read_bytes(in_buffer, dec_vectors[j].in_len, dec_vectors[j].in);
        ascii_read_bytes(aad_buffer, dec_vectors[j].aad_len, dec_vectors[j].aad);
        ascii_read_bytes(out_buffer, dec_vectors[j].in_len, dec_vectors[j].out);
        ascii_read_bytes(tag_buffer, dec_vectors[j].tag_len, dec_vectors[j].tag);

        memcpy(&in_buffer[dec_vectors[j].in_len], tag_buffer, dec_vectors[j].tag_len);
        memcpy(text, in_buffer, dec_vectors[j].in_len + dec_vectors[j].tag_len);

        status = TRANSPARENT_AEAD_DECRYPT(
          &attributes,
          key_buffer,
          dec_vectors[j].key_len,
          PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), dec_vectors[j].tag_len),
          iv_buffer,
          dec_vectors[j].iv_len,
          aad_buffer,
          dec_vectors[j].aad_len,
          text,
          dec_vectors[j].in_len + dec_vectors[j].tag_len,
          text + text_offset,
          sizeof(text),
          &out_len);

        UnityPrintf("AES-%d GCM decrypt bytes:%2d, (offset=%d)\n",
                    dec_vectors[j].key_len * 8,
                    dec_vectors[j].in_len,
                    text_offset);
        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        TEST_ASSERT_EQUAL(dec_vectors[j].in_len, out_len);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(out_buffer, text + text_offset, dec_vectors[j].in_len);
        j++;
      }
    }
  }

  goto exit;
exit:
  return;
}

void test_psa_driver_gcm_invalid_param(void)
{
  uint8_t key_buffer[32];
  uint8_t iv_buffer[12];
  uint8_t in_buffer[AES_TEXT_MAX_LEN + 16];
  uint8_t aad_buffer[AES_TEXT_MAX_LEN];
  uint8_t out_buffer[AES_TEXT_MAX_LEN];
  uint8_t tag_buffer[16];
  uint8_t text[AES_TEXT_MAX_LEN + 16 + AES_TEXT_MAX_OFFSET];
  uint32_t text_offset = 0;
  psa_status_t status;
  size_t out_len;
  psa_key_attributes_t attributes = psa_key_attributes_init();
  psa_key_type_t key_type;

  sl_psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
  psa_set_key_algorithm(&attributes, PSA_ALG_GCM);

  const gcm_block_vector_t *enc_vectors, *dec_vectors;

  enc_vectors = gcm_enc_128_vec;
  dec_vectors = gcm_dec_128_vec;
  text_offset = 0;
  psa_set_key_bits(&attributes, 128);

  UnityPrintf("Testing AES-GCM-%d encryption\n", psa_get_key_bits(&attributes));
  size_t j = 0;
  memset(text, 0, sizeof(text));

  // Read test vector data.
  ascii_read_bytes(key_buffer, enc_vectors[j].key_len, enc_vectors[j].key);
  ascii_read_bytes(iv_buffer, enc_vectors[j].iv_len, enc_vectors[j].iv);
  ascii_read_bytes(in_buffer, enc_vectors[j].in_len, enc_vectors[j].in);
  ascii_read_bytes(aad_buffer, enc_vectors[j].aad_len, enc_vectors[j].aad);
  ascii_read_bytes(out_buffer, enc_vectors[j].in_len, enc_vectors[j].out);
  ascii_read_bytes(tag_buffer, enc_vectors[j].tag_len, enc_vectors[j].tag);

  memcpy(text, in_buffer, enc_vectors[j].in_len);

  status = TRANSPARENT_AEAD_ENCRYPT(
    &attributes,
    key_buffer,
    enc_vectors[j].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), enc_vectors[j].tag_len),
    iv_buffer,
    0, // enc_vectors[j].iv_len,
    aad_buffer,
    enc_vectors[j].aad_len,
    text,
    enc_vectors[j].in_len,
    text + text_offset,
    sizeof(text),
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_ARGUMENT, status);

  key_type = psa_get_key_type(&attributes);
  psa_set_key_type(&attributes, PSA_KEY_TYPE_CHACHA20);
  status = TRANSPARENT_AEAD_ENCRYPT(
    &attributes,
    key_buffer,
    enc_vectors[j].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), enc_vectors[j].tag_len),
    iv_buffer,
    enc_vectors[j].iv_len,
    aad_buffer,
    enc_vectors[j].aad_len,
    text,
    enc_vectors[j].in_len,
    text + text_offset,
    sizeof(text),
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_NOT_SUPPORTED, status);
  psa_set_key_type(&attributes, key_type);

  //////////////////
  // Decrypt Test //
  //////////////////

  j = 0;

  // Read test vector data.
  ascii_read_bytes(key_buffer, dec_vectors[j].key_len, dec_vectors[j].key);
  ascii_read_bytes(iv_buffer, dec_vectors[j].iv_len, dec_vectors[j].iv);
  ascii_read_bytes(in_buffer, dec_vectors[j].in_len, dec_vectors[j].in);
  ascii_read_bytes(aad_buffer, dec_vectors[j].aad_len, dec_vectors[j].aad);
  ascii_read_bytes(out_buffer, dec_vectors[j].in_len, dec_vectors[j].out);
  ascii_read_bytes(tag_buffer, dec_vectors[j].tag_len, dec_vectors[j].tag);

  memcpy(&in_buffer[dec_vectors[j].in_len], tag_buffer, dec_vectors[j].tag_len);
  memcpy(text, in_buffer, dec_vectors[j].in_len + dec_vectors[j].tag_len);

  key_type = psa_get_key_type(&attributes);
  psa_set_key_type(&attributes, PSA_KEY_TYPE_CHACHA20);
  status = TRANSPARENT_AEAD_DECRYPT(
    &attributes,
    key_buffer,
    dec_vectors[j].key_len,
    PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), dec_vectors[j].tag_len),
    iv_buffer,
    dec_vectors[j].iv_len,
    aad_buffer,
    dec_vectors[j].aad_len,
    text,
    dec_vectors[j].in_len + dec_vectors[j].tag_len,
    text + text_offset,
    sizeof(text),
    &out_len);
  TEST_ASSERT_EQUAL(PSA_ERROR_NOT_SUPPORTED, status);
  psa_set_key_type(&attributes, key_type);

  goto exit;
exit:
  return;
}

static void test_psa_driver_chachapoly(void)
{
  uint8_t text[CHAPOL_TEXT_MAX_LEN + 16];
  uint8_t add_data[CHAPOL_TEXT_MAX_LEN];
  uint8_t expected[CHAPOL_TEXT_MAX_LEN];
  uint8_t key[32];
  uint8_t iv[12];
  psa_status_t status;
  size_t out_len;
  psa_key_attributes_t attributes = psa_key_attributes_init();

  for (test_key_type_t k = plaintext; k < invalid_key_type; k++) {
    for (uint32_t i = 0; i < sizeof(chachapoly_test_vector) / sizeof(sli_chachapoly_test_vector_t); i++) {
      //////////////////
      // Encrypt Test //
      //////////////////
      aes_read_bytes(key, chachapoly_test_vector[i].key_len, chachapoly_test_vector[i].key);
      aes_read_bytes(iv, chachapoly_test_vector[i].iv_len, chachapoly_test_vector[i].iv);
      aes_read_bytes(text, chachapoly_test_vector[i].text_len, chachapoly_test_vector[i].plain_text);
      aes_read_bytes(add_data, chachapoly_test_vector[i].add_data_len, chachapoly_test_vector[i].add_data);

      psa_set_key_bits(&attributes, chachapoly_test_vector[i].key_len * 8);
      sl_psa_set_key_type(&attributes, PSA_KEY_TYPE_CHACHA20);
      psa_set_key_algorithm(&attributes, PSA_ALG_CHACHA20_POLY1305);

      status = TRANSPARENT_AEAD_ENCRYPT(
        &attributes,
        key,
        chachapoly_test_vector[i].key_len,
        PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), chachapoly_test_vector[i].tag_len),
        iv,
        chachapoly_test_vector[i].iv_len,
        add_data,
        chachapoly_test_vector[i].add_data_len,
        text,
        chachapoly_test_vector[i].text_len,
        text,
        sizeof(text),
        &out_len);
      TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
      TEST_ASSERT_EQUAL(chachapoly_test_vector[i].cipher_len + 16, out_len);

      UnityPrintf("ChaCha20-Poly1305 encrypt key_len: %d, bytes:%2d,\n",
                  chachapoly_test_vector[i].key_len * 8,
                  chachapoly_test_vector[i].text_len);
      aes_read_bytes(expected, chachapoly_test_vector[i].cipher_len, chachapoly_test_vector[i].cipher_text);
      TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, text, chachapoly_test_vector[i].cipher_len);
      aes_read_bytes(expected, chachapoly_test_vector[i].tag_len, chachapoly_test_vector[i].tag);
      TEST_ASSERT_EQUAL_HEX8_ARRAY(expected,
                                   &text[chachapoly_test_vector[i].cipher_len],
                                   chachapoly_test_vector[i].tag_len);

      //////////////////
      // Decrypt Test //
      //////////////////

      status = TRANSPARENT_AEAD_DECRYPT(
        &attributes,
        key,
        chachapoly_test_vector[i].key_len,
        PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), chachapoly_test_vector[i].tag_len),
        iv,
        chachapoly_test_vector[i].iv_len,
        add_data,
        chachapoly_test_vector[i].add_data_len,
        text,
        out_len,
        text,
        sizeof(text),
        &out_len);
      TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
      TEST_ASSERT_EQUAL(chachapoly_test_vector[i].text_len, out_len);

      UnityPrintf("ChaCha20-Poly1305 decrypt key_len: %d, bytes:%2d,\n",
                  chachapoly_test_vector[i].key_len * 8,
                  chachapoly_test_vector[i].text_len);
      aes_read_bytes(expected, chachapoly_test_vector[i].text_len, chachapoly_test_vector[i].plain_text);
      TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, text, chachapoly_test_vector[i].text_len);
    }
  }
  goto exit;
exit:
  return;
}

#if !defined(SLI_CRYPTOACC_PRESENT_SI91X)
// Disabling this test as 91x does not support zero input length.
static void test_psa_driver_chachapoly_zero_input(void)
{
  uint8_t text[CHAPOL_TEXT_MAX_LEN + 16];
  uint8_t add_data[CHAPOL_TEXT_MAX_LEN];
  uint8_t key[32];
  uint8_t iv[12];
  psa_status_t status;
  size_t out_len;
  psa_key_attributes_t attributes = psa_key_attributes_init();

  for (test_key_type_t k = plaintext; k < invalid_key_type; k++) {
    //////////////////
    // Encrypt Test //
    //////////////////
    aes_read_bytes(key, chachapoly_test_null_vector.key_len, chachapoly_test_null_vector.key);
    aes_read_bytes(iv, chachapoly_test_null_vector.iv_len, chachapoly_test_null_vector.iv);
    aes_read_bytes(text, chachapoly_test_null_vector.text_len, chachapoly_test_null_vector.plain_text);
    aes_read_bytes(add_data, chachapoly_test_null_vector.add_data_len, chachapoly_test_null_vector.add_data);

    psa_set_key_bits(&attributes, chachapoly_test_null_vector.key_len * 8);
    sl_psa_set_key_type(&attributes, PSA_KEY_TYPE_CHACHA20);
    psa_set_key_algorithm(&attributes, PSA_ALG_CHACHA20_POLY1305);

    status = TRANSPARENT_AEAD_ENCRYPT(
      &attributes,
      key,
      chachapoly_test_null_vector.key_len,
      PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), chachapoly_test_null_vector.tag_len),
      iv,
      chachapoly_test_null_vector.iv_len,
      add_data,
      chachapoly_test_null_vector.add_data_len,
      text,
      chachapoly_test_null_vector.text_len,
      text,
      sizeof(text),
      &out_len);

    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);

    //////////////////
    // Decrypt Test //
    //////////////////
    aes_read_bytes(text, chachapoly_test_null_vector.tag_len, chachapoly_test_null_vector.tag);
    status = TRANSPARENT_AEAD_DECRYPT(
      &attributes,
      key,
      chachapoly_test_null_vector.key_len,
      PSA_ALG_AEAD_WITH_SHORTENED_TAG(psa_get_key_algorithm(&attributes), chachapoly_test_null_vector.tag_len),
      iv,
      chachapoly_test_null_vector.iv_len,
      add_data,
      chachapoly_test_null_vector.add_data_len,
      text,
      chachapoly_test_null_vector.tag_len,
      text,
      sizeof(text),
      &out_len);

    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
  }
  goto exit;
exit:
  return;
}
#endif // !(SLI_CRYPTOACC_PRESENT_SI91X)

static void test_psa_driver_aead_invalid_param(void)
{
  test_psa_driver_ccm_invalid_param();
  test_psa_driver_gcm_invalid_param();
}

/*
 * @brief
 *   main function
 */

int test_psa_driver_aead(void)
{

  UnityBeginGroup("psa-driver-aead");

  RUN_TEST(test_psa_driver_ccm, __LINE__);
  RUN_TEST(test_psa_driver_gcm, __LINE__);

  RUN_TEST(test_psa_driver_chachapoly, __LINE__);

  /* Removing this because CHACHA20POLY1305 with zero input length is not supported. */
  //RUN_TEST(test_psa_driver_chachapoly_zero_input, __LINE__);

  RUN_TEST(test_psa_driver_aead_invalid_param, __LINE__);

  UnityEnd();

  UnityPrint("ENDSWO");
  while (1)
    ;
}
