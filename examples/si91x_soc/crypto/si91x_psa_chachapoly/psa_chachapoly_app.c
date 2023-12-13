/*******************************************************************************
 * @file  psa_chachapoly_app.c
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "psa_chachapoly_app.h"
#include "psa/crypto.h"
#include "sl_si91x_psa_wrap.h"
#include <stdio.h>

#define USE_WRAPPED_KEYS 0

//!Test vectors

static const unsigned char key_test_data[] = { 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a,
                                               0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95,
                                               0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f };

static const unsigned char iv_test_data[] = { 0x01, 0x07, 0x00, 0x00, 0x00, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46 };

static const unsigned char ad_test_data[] = { 0x50, 0x51, 0x52, 0x53, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7 };

static const unsigned char msg_test_data[CHACHAPOLY_TEST_PT_MAX_LEN] = {
  0x4c, 0x61, 0x64, 0x69, 0x65, 0x73, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x47, 0x65, 0x6e, 0x74, 0x6c, 0x65, 0x6d, 0x65,
  0x6e, 0x20, 0x6f, 0x66, 0x20, 0x74, 0x68, 0x65, 0x20, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x20, 0x6f, 0x66, 0x20, 0x27,
  0x39, 0x39, 0x3a, 0x20, 0x49, 0x66, 0x20, 0x49, 0x20, 0x63, 0x6f, 0x75, 0x6c, 0x64, 0x20, 0x6f, 0x66, 0x66, 0x65,
  0x72, 0x20, 0x79, 0x6f, 0x75, 0x20, 0x6f, 0x6e, 0x6c, 0x79, 0x20, 0x6f, 0x6e, 0x65, 0x20, 0x74, 0x69, 0x70, 0x20,
  0x66, 0x6f, 0x72, 0x20, 0x74, 0x68, 0x65, 0x20, 0x66, 0x75, 0x74, 0x75, 0x72, 0x65, 0x2c, 0x20, 0x73, 0x75, 0x6e,
  0x73, 0x63, 0x72, 0x65, 0x65, 0x6e, 0x20, 0x77, 0x6f, 0x75, 0x6c, 0x64, 0x20, 0x62, 0x65, 0x20, 0x69, 0x74, 0x2e
};

static const unsigned char res_test_data[NB_TESTS][CHACHAPOLY_TEST_CT_MAX_LEN] = {
  { 0xbb, 0x13, 0xef, 0xb7, 0x31, 0x3e, 0xa5, 0xae, 0x98, 0x8d, 0xdf, 0x6f, 0xd6, 0x0d, 0x73, 0x0d, 0xa7, 0x75, 0x2a,
    0xc7, 0x8b, 0x53, 0x85, 0x4b, 0x1a, 0x75, 0xd6, 0x7b, 0x29, 0xbe, 0x5c, 0x77, 0xe9, 0x24, 0xeb, 0x62, 0x75, 0xd1,
    0x67, 0x9d, 0x02, 0xea, 0xaf, 0x7d, 0x4c, 0x5b, 0x30, 0x87, 0xd3, 0x78, 0x62, 0x6d, 0x50, 0x68, 0xfd, 0x3f, 0x5d,
    0xcc, 0x6a, 0x64, 0x3e, 0x70, 0x0e, 0x4f, 0x4a, 0x12, 0xaa, 0x8c, 0x01, 0x05, 0xb4, 0x24, 0xd9, 0xa5, 0x0e, 0xba,
    0xb0, 0x87, 0xf4, 0xfa, 0x0e, 0x0e, 0x66, 0x97, 0xd3, 0xd4, 0xf7, 0x65, 0xf1, 0xea, 0xd1, 0x72, 0x69, 0x4f, 0x38,
    0xa0, 0x14, 0x1e, 0xfa, 0xb9, 0xb5, 0x5a, 0xf9, 0x53, 0x85, 0x96, 0x18, 0xf5, 0x10, 0xd6, 0xad, 0xd7, 0x27, 0xe5,
    0xd5, 0x6e, 0x05, 0xf0, 0xaf, 0xee, 0x96, 0x40, 0x4b, 0xaf, 0xec, 0xf4, 0x62, 0xb5, 0xa5, 0x2b }
};

static const size_t key_len_test_data[NB_TESTS]   = { 32 };
static const size_t iv_len_test_data[NB_TESTS]    = { 12 };
static const size_t msg_len_test_data[NB_TESTS]   = { 114 };
static const size_t add_len_test_data[NB_TESTS]   = { 12 };
static const size_t tag_len_test_data[NB_TESTS]   = { 16 };
static const size_t crypt_len_test_data[NB_TESTS] = { 130 };

//Buffers to store responses
unsigned char cipher_tag_buffer[NB_TESTS][CHACHAPOLY_TEST_CT_MAX_LEN];
unsigned char decryption_output[NB_TESTS][CHACHAPOLY_TEST_CT_MAX_LEN];

void test_psa_chachapoly()
{
  struct test_vector vector[NB_TESTS];
  psa_status_t ret;

  /* psa crypto library initialization */
  ret = psa_crypto_init();
  if (ret == PSA_SUCCESS) {
    printf("\n PSA crypto library initialization Success \n");
  } else {
    printf("\n PSA crypto library initialization Failed with error: %ld\n", ret);
  }

  for (int i = 0; i < NB_TESTS; i++) {
    memcpy(vector[i].key_test_data, key_test_data, key_len_test_data[i]);
    memcpy(vector[i].iv_test_data, iv_test_data, iv_len_test_data[i]);
    memcpy(vector[i].ad_test_data, ad_test_data, add_len_test_data[i]);
    memcpy(vector[i].msg_test_data, msg_test_data, msg_len_test_data[i]);
    memcpy(vector[i].res_test_data, res_test_data[i], crypt_len_test_data[i]);
    vector[i].tag_len = tag_len_test_data[i];
    size_t out_len;

    psa_key_id_t key_id;
    psa_key_attributes_t key_attr = psa_key_attributes_init();

    //Set up attributes for a ChachaPoly key
    psa_set_key_type(&key_attr, PSA_KEY_TYPE_CHACHA20);
    psa_set_key_usage_flags(&key_attr, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&key_attr, PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CHACHA20_POLY1305, vector[i].tag_len));
#if USE_WRAPPED_KEYS
    psa_set_key_lifetime(&key_attr,
                         PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_VOLATILE,
                                                                        PSA_KEY_VOLATILE_PERSISTENT_WRAPPED));
#endif
    //Import a volatile key for ChachaPoly Key
    ret = psa_import_key(&key_attr, key_test_data, sizeof(key_test_data), &key_id);

    if (ret == PSA_SUCCESS) {
      printf("\n Key import Success \n");
    } else {
      printf("\n Key import Failed with error: %ld\n", ret);
    }

    ret = psa_aead_encrypt(key_id,
                           PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CHACHA20_POLY1305, vector[i].tag_len),
                           vector[i].iv_test_data,
                           iv_len_test_data[i],
                           vector[i].ad_test_data,
                           add_len_test_data[i],
                           vector[i].msg_test_data,
                           msg_len_test_data[i],
                           cipher_tag_buffer[i],
                           crypt_len_test_data[i],
                           &out_len);

    if (ret == 0 && memcmp(cipher_tag_buffer[i], res_test_data[i], msg_len_test_data[i]) == 0) {
      printf("\n Encryption Success \n");
    } else {
      printf("\n Encryption Failed with error: %ld\n", ret);
    }

    memset(decryption_output[i], 0, CHACHAPOLY_TEST_PT_MAX_LEN);

    ret = psa_aead_decrypt(key_id,
                           PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CHACHA20_POLY1305, vector[i].tag_len),
                           vector[i].iv_test_data,
                           iv_len_test_data[i],
                           vector[i].ad_test_data,
                           add_len_test_data[i],
                           cipher_tag_buffer[i],
                           crypt_len_test_data[i],
                           decryption_output[i],
                           msg_len_test_data[i],
                           &out_len);

    if (ret == 0 && memcmp(decryption_output[i], msg_test_data, msg_len_test_data[i]) == 0) {
      printf("\n Decryption Success \n");
    } else {
      printf("\n Decryption Failed with error: %ld\n", ret);
    }

    // Destroy plain key for ChachaPoly
    ret = psa_destroy_key(key_id);
    if (ret != PSA_SUCCESS) {
      printf("Destroy key failed with error: %ld\n", ret);
    }
  }
}
