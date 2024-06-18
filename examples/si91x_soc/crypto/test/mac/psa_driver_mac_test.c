/***************************************************************************/ /**
 * @brief Silicon Labs PSA Crypto Secure Element Driver MAC tests
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

#include "psa_driver_mac_test.h"
#include "psa_driver_mac_test_data.h"

static inline void sl_psa_set_key_type(psa_key_attributes_t *attributes, psa_key_type_t type)
{
  /* Common case: quick path */
  attributes->MBEDTLS_PRIVATE(core).MBEDTLS_PRIVATE(type) = type;
}

static int unhexify(unsigned char *obuf, const char *ibuf)
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

void test_psa_driver_hmac(void)
{
  unsigned char key_buffer_real[516];
  unsigned char *key_buffer = key_buffer_real + 1;
  unsigned char message[512];
  unsigned char output[64];
  unsigned char expected_output[64];
  uint32_t key_len, msg_len, mac_len;
  psa_status_t status;
  psa_algorithm_t alg;
  size_t out_len;
  psa_key_attributes_t attributes = psa_key_attributes_init();

  // Loop over transparent and opaque keys
  for (test_key_type_t k = plaintext; k < invalid_key_type; k++) {
    for (uint32_t i = 0; i < sizeof(hmac_messages) / sizeof(sl_se_hmac_test_vector_t); i++) {
      memset(key_buffer_real, 0x00, sizeof(key_buffer_real));
      memset(message, 0x00, sizeof(message));
      memset(output, 0x00, sizeof(output));
      memset(expected_output, 0x00, sizeof(expected_output));

      switch (hmac_messages[i].hash_size) {
        case 20:
          alg = PSA_ALG_HMAC(PSA_ALG_SHA_1);
          break;
        case 28:
          /* Skip test vectors for unsupported hash size. (SHA-224)*/
          continue;
          alg = PSA_ALG_HMAC(PSA_ALG_SHA_224);
          break;
        case 32:
          alg = PSA_ALG_HMAC(PSA_ALG_SHA_256);
          break;
        case 48:
          alg = PSA_ALG_HMAC(PSA_ALG_SHA_384);
          break;
        case 64:
          alg = PSA_ALG_HMAC(PSA_ALG_SHA_512);
          break;
        default:
          /* Bad hash size */
          TEST_ASSERT(false);
          return;
      }

      TEST_ASSERT(sizeof(key_buffer_real) >= hmac_messages[i].key_len);
      key_len = unhexify(key_buffer, hmac_messages[i].key);
      TEST_ASSERT_EQUAL(key_len, hmac_messages[i].key_len);
      msg_len = unhexify(message, hmac_messages[i].msg);

      psa_set_key_algorithm(&attributes, alg);
      psa_set_key_bits(&attributes, key_len * 8);
      sl_psa_set_key_type(&attributes, PSA_KEY_TYPE_HMAC);

#if defined(BE_VERBOSE)
      UnityPrintf("key type=%u  tv=%d  %s  kl=%d  ml=%d  ol=%d\n",
                  k,
                  hmac_messages[i].count,
                  hmac_messages[i].hash_size == 20   ? "SHA1"
                  : hmac_messages[i].hash_size == 28 ? "SHA224"
                  : hmac_messages[i].hash_size == 32 ? "SHA256"
                  : hmac_messages[i].hash_size == 48 ? "SHA384"
                  : hmac_messages[i].hash_size == 64 ? "SHA512"
                  : hmac_messages[i].hash_size == 48 ? "SHA384"
                  : hmac_messages[i].hash_size == 64 ? "SHA512"
                                                     : "Unknown hash type",
                  key_len,
                  msg_len,
                  hmac_messages[i].t_len);
#endif

      status = TRANSPARENT_MAC_COMPUTE(&attributes,
                                       key_buffer,
                                       key_len,
                                       alg,
                                       message,
                                       msg_len,
                                       output,
                                       sizeof(output),
                                       &out_len);

#if defined(BE_VERBOSE)
      UnityPrintf("HMAC message bytes:%d\n", msg_len);
#endif
      TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
      mac_len = unhexify(expected_output, hmac_messages[i].mac);
      TEST_ASSERT(mac_len <= out_len);
      TEST_ASSERT_EQUAL(0, memcmp(output, expected_output, mac_len));
    }
  }
  goto exit;
exit:
  return;
}

static int asciiReadBytes(uint8_t *buf, unsigned int buflen, const char *str)
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

void test_psa_driver_cmac(void)
{
  uint8_t key_buffer_real[36];
  uint8_t *key_buffer = key_buffer_real + 1;
  uint8_t text[64];
  uint8_t expected[16];
  psa_status_t status;
  size_t out_len;
  psa_key_attributes_t attributes = psa_key_attributes_init();

  // Loop over transparent and opaque keys
  for (test_key_type_t k = plaintext; k < invalid_key_type; k++) {
    // Set key lifetime
    switch (k) {
      case plaintext: // Transparent
        psa_set_key_lifetime(&attributes, 0);
        break;
      default:
        return;
    }

    // Loop over test sets with different key sizes
    for (size_t i = 0; i < 3; i++) {
      const cmac_test_vector_t *vectors;
      switch (i) {
        case 0:
          vectors = cmac_aes128_test_vectors;
          psa_set_key_bits(&attributes, 128);
          sl_psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
          psa_set_key_algorithm(&attributes, PSA_ALG_CMAC);
          break;
        case 1:
          vectors = cmac_aes192_test_vectors;
          psa_set_key_bits(&attributes, 192);
          sl_psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
          psa_set_key_algorithm(&attributes, PSA_ALG_CMAC);
          break;
        case 2:
          vectors = cmac_aes256_test_vectors;
          psa_set_key_bits(&attributes, 256);
          sl_psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
          psa_set_key_algorithm(&attributes, PSA_ALG_CMAC);
          break;
        default:
          return;
      }

      size_t j = 0;
      while (vectors[j].key) {
        psa_algorithm_t exercise_alg = PSA_ALG_CMAC;
        if (vectors[j].tag_len < 16) {
          exercise_alg = PSA_ALG_TRUNCATED_MAC(PSA_ALG_CMAC, vectors[j].tag_len);
        }

        psa_set_key_algorithm(&attributes, exercise_alg);

        // Import and setup key
        asciiReadBytes(key_buffer, vectors[j].key_len, vectors[j].key);
        asciiReadBytes(text, vectors[j].plaintext_len, vectors[j].plaintext);

        TEST_ASSERT_EQUAL(psa_get_key_bits(&attributes) / 8, vectors[j].key_len);

#if defined(BE_VERBOSE)
        UnityPrintf("AES-%d CMAC One-shot  msg_sz %2d   mac_sz %2d   ",
                    psa_get_key_bits(&attributes),
                    vectors[j].plaintext_len,
                    vectors[j].tag_len);
#endif

        switch (k) {
          case plaintext:
            status = TRANSPARENT_MAC_COMPUTE(&attributes,
                                             key_buffer,
                                             vectors[j].key_len,
                                             exercise_alg,
                                             text,
                                             vectors[j].plaintext_len,
                                             text,
                                             sizeof(text),
                                             &out_len);
            break;
          default:
            return;
        }

        TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
        TEST_ASSERT(out_len == (uint32_t)vectors[j].tag_len);

        asciiReadBytes(expected, vectors[j].tag_len, vectors[j].tag);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, text, vectors[j].tag_len);
        j++;
      }
    }
  }
  goto exit;
exit:
  return;
}

/*
 * @brief
 *   main function
 */
int test_psa_driver_mac(void)
{

  UnityBeginGroup("psa-driver-mac");
  RUN_TEST(test_psa_driver_hmac, __LINE__);
  RUN_TEST(test_psa_driver_cmac, __LINE__);

  UnityEnd();
  UnityPrint("ENDSWO");
  while (1)
    ;
}
