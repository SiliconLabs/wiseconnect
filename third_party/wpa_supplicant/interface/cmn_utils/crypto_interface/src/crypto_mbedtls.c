/***************************************************************************/ /**
* @file crypto_mbedtls.c
* @brief PSA crypto interface for supplicant
*******************************************************************************
* # License
* <b>Copyright 2024 Silicon Laboratories Inc.
www.silabs.com</b>
*******************************************************************************
*
* SPDX-License-Identifier: Zlib
*
* The licensor of this software is Silicon Laboratories Inc.
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* sl_freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgment in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
******************************************************************************/

#include <psa/crypto.h>
#include <psa/crypto_values.h>
#include "common.h" // TODO: resolve warning for redefintion of #define BIT(x)
#include "mbedtls/bignum.h"
#include "const_time.h"
#include "mbedtls/ecp.h"
#include "sl_wlan_logger.h"

#ifndef AES_BLOCK_SIZE
#define AES_BLOCK_SIZE PSA_BLOCK_CIPHER_BLOCK_LENGTH(PSA_KEY_TYPE_AES)
#endif
#ifndef DES_BLOCK_SIZE
#define DES_BLOCK_SIZE PSA_BLOCK_CIPHER_BLOCK_LENGTH(PSA_KEY_TYPE_DES)
#endif

#define AES_CLEANUP_OP(ctx)                              \
  do {                                                   \
    free(((struct multi_part_aes_ctx *)ctx)->operation); \
    free(ctx);                                           \
  } while (0)

struct multi_part_aes_ctx {
  psa_cipher_operation_t *operation;
  psa_key_id_t key_id;
};

struct crypto_ec {
  mbedtls_mpi *A;
  mbedtls_ecp_group *grp;
};

void crypto_ec_deinit(struct crypto_ec *e);

/*PSA APIs start here*/

/**
 * multi_part_hash - multi-part hash function using PSA APIs
 * @num_elem: Number of elements 
 * @alg: Hash algorithm
 * @input: Array of pointers that reference individual data elements
 * @input_length: Array of length of individual data elements
 * @hash: Buffer to store hash
 * 
 * Returns: 0 on success, -1 on failure.
 *
 * This function calculates a hash value by processing messages in fragments,
 * the hash algorithm is selected based on the provided input parameter.
 */
static int multi_part_hash(size_t num_elem,
                           psa_algorithm_t alg,
                           const uint8_t *input[],
                           const size_t *input_length,
                           uint8_t *hash)
{
  size_t hash_length  = 0;
  size_t hash_size    = PSA_HASH_LENGTH(alg);
  psa_status_t status = psa_crypto_init();
  if (status != PSA_SUCCESS) {
    SL_DEBUG_LOG("Failed to initialize PSA Crypto: status %d\n", status);
    return -1;
  }
  psa_hash_operation_t operation = psa_hash_operation_init();
  status                         = psa_hash_setup(&operation, alg);
  if (status != PSA_SUCCESS) {
    SL_DEBUG_LOG("Failed to setup  hashing: status %d\n", status);
    return -1;
  }
  for (size_t i = 0; i < num_elem; i++) {
    status = psa_hash_update(&operation, input[i], input_length[i]);
    if (status != PSA_SUCCESS) {
      SL_DEBUG_LOG("Failed to update hash with part %zu: status %d\n", i, status);
      return -1;
    }
  }
  status = psa_hash_finish(&operation, hash, hash_size, &hash_length);
  if (status != PSA_SUCCESS) {
    SL_DEBUG_LOG("Failed to finish hashing: status %d\n", status);
    return -1;
  }
  return 0;
}

/**
 * md5_vector - MD5 hash function
 * @num_elem: Number of elements 
 * @addr: Array of pointers that reference individual data elements
 * @len: Array of length of individual data elements
 * @mac: Buffer to store hash
 * 
 * Returns: 0 on success, -1 on failure.
 *
 * This function calculates MD5 hash value by processing messages in fragments.
 */
int md5_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
  return multi_part_hash(num_elem, PSA_ALG_MD5, addr, len, mac);
}

/**
 * sha1_vector - SHA-1 hash function
 * @num_elem: Number of elements 
 * @addr: Array of pointers that reference individual data elements
 * @len: Array of length of individual data elements
 * @mac: Buffer to store hash
 * 
 * Returns: 0 on success, -1 on failure.
 *
 * This function calculates SHA-1 hash value by processing messages in fragments.
 */
int sha1_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
  return multi_part_hash(num_elem, PSA_ALG_SHA_1, addr, len, mac);
}

/**
 * sha256_vector - SHA-256 hash function
 * @num_elem: Number of elements 
 * @addr: Array of pointers that reference individual data elements
 * @len: Array of length of individual data elements
 * @mac: Buffer to store hash
 * 
 * Returns: 0 on success, -1 on failure.
 *
 * This function calculates SHA-256 hash value by processing messages in fragments.
 */
int sha256_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
  return multi_part_hash(num_elem, PSA_ALG_SHA_256, addr, len, mac);
}

/**
 * sha384_vector - SHA-384 hash function
 * @num_elem: Number of elements 
 * @addr: Array of pointers that reference individual data elements
 * @len: Array of length of individual data elements
 * @mac: Buffer to store hash
 * 
 * Returns: 0 on success, -1 on failure.
 *
 * This function calculates SHA-384 hash value by processing messages in fragments.
 */
int sha384_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
  return multi_part_hash(num_elem, PSA_ALG_SHA_384, addr, len, mac);
}

/**
 * sha512_vector - SHA-512 hash function
 * @num_elem: Number of elements 
 * @addr: Array of pointers that reference individual data elements
 * @len: Array of length of individual data elements
 * @mac: Buffer to store hash
 * 
 * Returns: 0 on success, -1 on failure.
 *
 * This function calculates SHA-512 hash value by processing messages in fragments.
 */
int sha512_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
  return multi_part_hash(num_elem, PSA_ALG_SHA_512, addr, len, mac);
}

/**
 * mac_operation_cleanup - freeup the resources used by mac operation
 * @key_id: key identifier to destroy 
 * @operation: state object for multi-part mac operation
 * Does not return any value
 *
 * This function cleanup the resources held by mac operation and destroy key id,
 * this has to be called when operation being aborted or successfully completed
 */
static void mac_operation_cleanup(psa_key_id_t key_id, psa_mac_operation_t *operation)
{
  psa_mac_abort(operation);
  psa_destroy_key(key_id);
}

/**
 * multi_part_hmac - multi-part HMAC calculation PSA APIs
 * @num_elem: Number of elements 
 * @key_data: Key for HMAC operation
 * @key_len: Length of key in bytes
 * @alg: Hash operation used as the foundation for HMAC
 * @input: Array of pointers that reference individual data elements
 * @input_length: Array of length of individual data elements
 * @mac: Buffer to store MAC
 * Returns: 0 on success, -1 on failure.
 *
 * This function computes an HMAC (Hash-based Message Authentication Code) by processing 
 * messages in fragments. The hash algorithm is selected based on the specified input parameter
 */
static int multi_part_hmac(size_t num_elem,
                           const uint8_t *key_data,
                           size_t key_len,
                           psa_algorithm_t alg,
                           const uint8_t *input[],
                           const size_t *input_length,
                           uint8_t *mac)
{
  size_t mac_length = 0;
  size_t mac_size   = PSA_MAC_LENGTH(PSA_KEY_TYPE_HMAC, key_len * 8, PSA_ALG_HMAC(alg));
  if (num_elem > 5) {
    /*
		 * Fixed limit on the number of fragments to avoid having to
		 * allocate memory (which could fail).
		 */
    return -1;
  }
  psa_status_t status = psa_crypto_init();
  if (status != PSA_SUCCESS) {
    SL_DEBUG_LOG("Failed to initialize PSA Crypto: %d\n", status);
    return -1;
  }

  psa_key_attributes_t key_attributes = psa_key_attributes_init();
  psa_set_key_type(&key_attributes, PSA_KEY_TYPE_HMAC);
  psa_set_key_algorithm(&key_attributes, PSA_ALG_HMAC(alg));
  psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_SIGN_MESSAGE);
  psa_key_id_t key_id;
  status = psa_import_key(&key_attributes, key_data, key_len, &key_id);
  if (status != PSA_SUCCESS) {
    SL_DEBUG_LOG("Failed to import key %d\n", status);
    return -1;
  }

  psa_mac_operation_t operation = psa_mac_operation_init();
  status                        = psa_mac_sign_setup(&operation, key_id, PSA_ALG_HMAC(alg));
  if (status != PSA_SUCCESS) {
    mac_operation_cleanup(key_id, &operation);
    SL_DEBUG_LOG("Failed to setup mac sign setup: %d\n", status);
    return -1;
  }

  for (size_t i = 0; i < num_elem; i++) {
    status = psa_mac_update(&operation, input[i], input_length[i]);

    if (status != PSA_SUCCESS) {
      mac_operation_cleanup(key_id, &operation);
      SL_DEBUG_LOG("Failed to update hmac with part %zu: %d\n", i, status);
      return -1;
    }
  }

  status = psa_mac_sign_finish(&operation, mac, mac_size, &mac_length);
  if (status != PSA_SUCCESS) {
    mac_operation_cleanup(key_id, &operation);
    SL_DEBUG_LOG("Failed to finish hashing: %d\n", status);
    return -1;
  }
  mac_operation_cleanup(key_id, &operation);
  return 0;
}

/**
 * hmac_sha1_vector - Computing HMAC using SHA-1
 * @key: Key for HMAC operation
 * @key_len: Length of key in bytes
 * @num_elem: Number of elements 
 * @addr: Array of pointers that reference individual data elements
 * @len: Array of length of individual data elements
 * @mac: Buffer to store MAC
 * 
 * Returns: 0 on success, -1 on failure.
 *
 * This function calculates the HMAC-SHA1 MAC for fragments of messages.
 */
int hmac_sha1_vector(const u8 *key, size_t key_len, size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{

  return multi_part_hmac(num_elem, key, key_len, PSA_ALG_SHA_1, addr, len, mac);
}

/**
 * hmac_md5_vector - Computing HMAC using MD5
 * @key: Key for HMAC operation
 * @key_len: Length of key in bytes
 * @num_elem: Number of elements 
 * @addr: Array of pointers that reference individual data elements
 * @len: Array of length of individual data elements
 * @mac: Buffer to store MAC
 * 
 * Returns: 0 on success, -1 on failure.
 *
 * This function calculates the HMAC-MD5 MAC for fragments of messages.
 */
int hmac_md5_vector(const u8 *key, size_t key_len, size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{

  return multi_part_hmac(num_elem, key, key_len, PSA_ALG_MD5, addr, len, mac);
}

/**
 * hmac_sha256_vector - Computing HMAC using SHA-256
 * @key: Key for HMAC operation
 * @key_len: Length of key in bytes
 * @num_elem: Number of elements 
 * @addr: Array of pointers that reference individual data elements
 * @len: Array of length of individual data elements
 * @mac: Buffer to store MAC
 * 
 * Returns: 0 on success, -1 on failure.
 *
 * This function calculates the HMAC-SHA256 MAC for fragments of messages.
 */
int hmac_sha256_vector(const u8 *key, size_t key_len, size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{

  return multi_part_hmac(num_elem, key, key_len, PSA_ALG_SHA_256, addr, len, mac);
}

/**
 * hmac_sha384_vector - Computing HMAC using SHA-384
 * @key: Key for HMAC operation
 * @key_len: Length of key in bytes
 * @num_elem: Number of elements 
 * @addr: Array of pointers that reference individual data elements
 * @len: Array of length of individual data elements
 * @mac: Buffer to store MAC
 * 
 * Returns: 0 on success, -1 on failure.
 *
 * This function calculates the HMAC-SHA384 MAC for fragments of messages.
 */
int hmac_sha384_vector(const u8 *key, size_t key_len, size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{

  return multi_part_hmac(num_elem, key, key_len, PSA_ALG_SHA_384, addr, len, mac);
}

/**
 * hmac_sha512_vector - Computing HMAC using SHA-512
 * @key: Key for HMAC operation
 * @key_len: Length of key in bytes
 * @num_elem: Number of elements 
 * @addr: Array of pointers that reference individual data elements
 * @len: Array of length of individual data elements
 * @mac: Buffer to store MAC
 * 
 * Returns: 0 on success, -1 on failure.
 *
 * This function calculates the HMAC-SHA512 MAC for fragments of messages.
 */
int hmac_sha512_vector(const u8 *key, size_t key_len, size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{

  return multi_part_hmac(num_elem, key, key_len, PSA_ALG_SHA_512, addr, len, mac);
}

/**
 * omac1_aes_vector - OMAC1 (One-Key Message Authentication Code) using AES
 * @key: Key for AES operation
 * @key_len: Length of key in bytes
 * @num_elem: Number of elements 
 * @addr: Array of pointers that reference individual data elements
 * @len: Array of length of individual data elements
 * @mac: Buffer to store computed OMAC1
 * 
 * Returns: 0 on success, -1 on failure.
 *
 * OMAC1 AES calculates message authentication code that leverages the AES encryption algorithm to 
 * ensure the integrity and authenticity of messages,
 */
int omac1_aes_vector(const u8 *key, size_t key_len, size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
  size_t mac_length   = 0;
  size_t mac_size     = PSA_MAC_LENGTH(PSA_KEY_TYPE_AES, key_len, PSA_ALG_CMAC);
  psa_status_t status = psa_crypto_init();
  if (status != PSA_SUCCESS) {
    SL_DEBUG_LOG("Failed to initialize PSA Crypto: %d\n", status);
    return -1;
  }
  psa_key_attributes_t key_attributes = psa_key_attributes_init();
  psa_set_key_type(&key_attributes, PSA_KEY_TYPE_AES);
  psa_set_key_algorithm(&key_attributes, PSA_ALG_CMAC);
  psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_SIGN_MESSAGE);
  psa_set_key_bits(&key_attributes, key_len * 8);
  psa_key_id_t key_id;
  status = psa_import_key(&key_attributes, key, key_len, &key_id);
  if (status != PSA_SUCCESS) {
    SL_DEBUG_LOG("Failed to import key %d\n", status);
    return -1;
  }
  psa_mac_operation_t operation = psa_mac_operation_init();
  status                        = psa_mac_sign_setup(&operation, key_id, PSA_ALG_CMAC);
  if (status != PSA_SUCCESS) {
    mac_operation_cleanup(key_id, &operation);
    SL_DEBUG_LOG("Failed to setup mac sign setup: %d\n", status);
    return -1;
  }
  for (size_t i = 0; i < num_elem; i++) {
    status = psa_mac_update(&operation, addr[i], len[i]);
    if (status != PSA_SUCCESS) {
      mac_operation_cleanup(key_id, &operation);
      SL_DEBUG_LOG("Invalid input or length at index %zu\n", i);
      return -1;
    }
  }
  status = psa_mac_sign_finish(&operation, mac, mac_size, &mac_length);
  if (status != PSA_SUCCESS) {
    mac_operation_cleanup(key_id, &operation);
    SL_DEBUG_LOG("Failed to finish hashing: %d\n", status);
    return -1;
  }
  mac_operation_cleanup(key_id, &operation);
  return 0;
}

/**
 * crypto_get_random - Random byte generator
 * @buf: Output buffer for the generated data
 * @len: Number of bytes to generate and output.
 * 
 * Returns: 0 on success, -1 on failure.
 *
 * Gererates a random bytes of given length.
 */
int crypto_get_random(void *buf, size_t len)
{
  psa_status_t status = psa_crypto_init();
  if (status != PSA_SUCCESS) {
    SL_DEBUG_LOG("Failed to initialize PSA Crypto: %d\n", status);
    return -1;
  }

  status = psa_generate_random((uint8_t *)buf, len);
  if (status != PSA_SUCCESS) {
    SL_DEBUG_LOG("Failed to generate random number: %d\n", status);
    return -1;
  }
  return 0;
}

/**
 * aes_operation_cleanup - freeup the resources used by multi-part AES operation
 * @key_id: key identifier to destroy 
 * @operation: state object for aes operation
 * Does not return any value
 *
 * This function cleanup the resources held by aes operation and destroy key id,
 * this has to be called when multi-part AES operation being aborted or successfully
 *  completed
 */
static void aes_operation_cleanup(psa_key_id_t key_id, psa_cipher_operation_t *operation)
{
  psa_destroy_key(key_id);
  psa_cipher_abort(operation);
}

/**
 * aes_128_cbc_encrypt - Encrypts data using AES-128 in CBC mode.
 * @key: Key for AES operation
 * @iv: Initialization vector (IV) for the encryption
 * @data: Buffer containing the message to encrypt
 * @data_len: Size of the input buffer in bytes.
 * 
 * Returns: 0 on success, -1 on failure.
 *
 * Encrypts data using the AES  algorithm in CBC mode with a 128-bit key. It requires an 
 * initialization vector (IV) to ensure that identical plaintexts produce different ciphertexts.
 */
int aes_128_cbc_encrypt(const u8 *key, const u8 *iv, u8 *data, size_t data_len)
{
  size_t key_len       = 16;
  size_t output_length = 0;
  psa_status_t status  = psa_crypto_init();
  if (status != PSA_SUCCESS) {
    SL_DEBUG_LOG("Failed to initialize PSA Crypto: %d\n", status);
    return -1;
  }
  psa_key_attributes_t key_attributes = psa_key_attributes_init();
  psa_set_key_type(&key_attributes, PSA_KEY_TYPE_AES);
  psa_set_key_algorithm(&key_attributes, PSA_ALG_CBC_NO_PADDING);
  psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_ENCRYPT);
  psa_set_key_bits(&key_attributes, key_len * 8);
  psa_key_id_t key_id;
  status = psa_import_key(&key_attributes, key, key_len, &key_id);
  if (status != PSA_SUCCESS) {
    SL_DEBUG_LOG("Failed to import key (error %d)\n", (int)status);
    return -1;
  }
  psa_cipher_operation_t operation;
  operation = psa_cipher_operation_init();
  status    = psa_cipher_encrypt_setup(&operation, key_id, PSA_ALG_CBC_NO_PADDING);
  if (status != PSA_SUCCESS) {
    aes_operation_cleanup(key_id, &operation);
    SL_DEBUG_LOG("Failed to setup cipher encryption: %d\n", status);
    return -1;
  }
  status = psa_cipher_set_iv(&operation, iv, AES_BLOCK_SIZE);
  if (status != PSA_SUCCESS) {
    aes_operation_cleanup(key_id, &operation);
    SL_DEBUG_LOG("Failed to setup iv: %d\n", status);
    return -1;
  }
  status = psa_cipher_update(&operation, data, data_len, data, data_len, &output_length);
  if (status != PSA_SUCCESS) {
    aes_operation_cleanup(key_id, &operation);
    SL_DEBUG_LOG("Failed to update cipher encryption: %d\n", status);
    return -1;
  }
  status = psa_cipher_finish(&operation, data, 0, &output_length);
  if (status != PSA_SUCCESS) {
    aes_operation_cleanup(key_id, &operation);
    SL_DEBUG_LOG("Failed to finish cipher encryption: %d\n", status);
    return -1;
  }
  aes_operation_cleanup(key_id, &operation);
  return 0;
}

/**
 * aes_128_cbc_decrypt - Decrypts data using AES-128 in CBC mode.
 * @key: Key for AES operation
 * @iv: Initialization vector (IV) for the decryption
 * @data: Buffer containing the message to decryption
 * @data_len: Size of the input buffer in bytes.
 * 
 * Returns: 0 on success, -1 on failure.
 *
 * Decrypts data that has been encrypted using the AES algorithm in CBC mode 
 * with a 128-bit key. It requires the same initialization vector (IV) that was used 
 * during encryption.
 */
int aes_128_cbc_decrypt(const u8 *key, const u8 *iv, u8 *data, size_t data_len)
{
  size_t key_len       = 16;
  size_t output_length = 0;
  psa_status_t status  = psa_crypto_init();
  if (status != PSA_SUCCESS) {
    SL_DEBUG_LOG("Failed to initialize PSA Crypto: %d\n", status);
    return -1;
  }
  psa_key_attributes_t key_attributes = psa_key_attributes_init();
  psa_set_key_type(&key_attributes, PSA_KEY_TYPE_AES);
  psa_set_key_algorithm(&key_attributes, PSA_ALG_CBC_NO_PADDING);
  psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_DECRYPT);
  psa_set_key_bits(&key_attributes, key_len * 8);
  psa_key_id_t key_id;
  status = psa_import_key(&key_attributes, key, key_len, &key_id);
  if (status != PSA_SUCCESS) {
    SL_DEBUG_LOG("Failed to import key (error %d)\n", (int)status);
    return -1;
  }
  psa_cipher_operation_t operation;
  operation = psa_cipher_operation_init();
  status    = psa_cipher_decrypt_setup(&operation, key_id, PSA_ALG_CBC_NO_PADDING);
  if (status != PSA_SUCCESS) {
    aes_operation_cleanup(key_id, &operation);
    SL_DEBUG_LOG("Failed to setup cipher decryption: %d\n", status);
    return -1;
  }
  status = psa_cipher_set_iv(&operation, iv, AES_BLOCK_SIZE);
  if (status != PSA_SUCCESS) {
    aes_operation_cleanup(key_id, &operation);
    SL_DEBUG_LOG("Failed to setup IV: %d\n", status);
    return -1;
  }
  status = psa_cipher_update(&operation, data, data_len, data, data_len, &output_length);
  if (status != PSA_SUCCESS) {
    aes_operation_cleanup(key_id, &operation);
    SL_DEBUG_LOG("Failed to update cipher decryption: %d\n", status);
    return -1;
  }
  status = psa_cipher_finish(&operation, data, 0, &output_length);
  if (status != PSA_SUCCESS) {
    aes_operation_cleanup(key_id, &operation);
    SL_DEBUG_LOG("Failed to finish cipher decryption: %d\n", status);
    return -1;
  }
  aes_operation_cleanup(key_id, &operation);
  return 0;
}

/**
 * des_encrypt - Encrypts data using Single DES algorithm
 * @clear: Buffer containing the message to encrypt
 * @key: 64 bit Key for DES operation 
 * @cypher: Buffer to store cipher
 * 
 * Returns: 0 on success, -1 on failure.
 *
 * encrypts single block (64 bit) of data using the DES algorithm, a symmetric-key block cipher 
 */
int des_encrypt(const u8 *clear, const u8 *key, u8 *cypher)
{
  size_t output_length = 0;
  size_t input_length  = DES_BLOCK_SIZE;
  size_t key_len       = 8;
  size_t output_size   = PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(PSA_KEY_TYPE_DES, PSA_ALG_ECB_NO_PADDING, input_length);
  uint8_t pkey[8], next, tmp;
  int i;
  /* Add parity bits to the key */
  next = 0;
  for (i = 0; i < 7; i++) {
    tmp     = key[i];
    pkey[i] = (tmp >> i) | next | 1;
    next    = tmp << (7 - i);
  }
  pkey[i]             = next | 1;
  psa_status_t status = psa_crypto_init();
  if (status != PSA_SUCCESS) {
    SL_DEBUG_LOG("Failed to initialize PSA Crypto: %d\n", status);
    return -1;
  }
  psa_key_attributes_t key_attributes = psa_key_attributes_init();
  psa_set_key_type(&key_attributes, PSA_KEY_TYPE_DES);
  psa_set_key_algorithm(&key_attributes, PSA_ALG_ECB_NO_PADDING);
  psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_ENCRYPT);
  psa_set_key_bits(&key_attributes, 64);
  psa_key_id_t key_id;
  status = psa_import_key(&key_attributes, pkey, key_len, &key_id);
  if (status != PSA_SUCCESS) {
    SL_DEBUG_LOG("Failed to import key: %d)\n", status);
    return -1;
  }
  status = psa_cipher_encrypt(key_id, PSA_ALG_ECB_NO_PADDING, clear, input_length, cypher, output_size, &output_length);
  if (status != PSA_SUCCESS) {
    psa_destroy_key(key_id);
    SL_DEBUG_LOG("Failed to do DES ecncryption : %d)\n", status);
    return -1;
  }
  psa_destroy_key(key_id);
  return 0;
}

/**
 * aes_encrypt_init - Init operation for multi-part AES encryption
 * @key: Key for AES operation 
 * @len: Length of key in bytes
 * 
 * Returns: Return a void pointer
 *
 * Function setup the key and AES encryption operation state object for PSA API and returns a void pointer
 * to the AES operation state object 
 */
void *aes_encrypt_init(const u8 *key, size_t len)
{
  psa_status_t status = psa_crypto_init();
  if (status != PSA_SUCCESS) {
    SL_DEBUG_LOG("Failed to initialize PSA Crypto: %d\n", status);
    return NULL;
  }
  psa_key_attributes_t key_attributes = psa_key_attributes_init();
  psa_set_key_type(&key_attributes, PSA_KEY_TYPE_AES);
  psa_set_key_algorithm(&key_attributes, PSA_ALG_ECB_NO_PADDING);
  psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_ENCRYPT);
  psa_set_key_bits(&key_attributes, len * 8);
  struct multi_part_aes_ctx *aes = (struct multi_part_aes_ctx *)os_malloc(sizeof(struct multi_part_aes_ctx));
  if (aes == NULL) {
    SL_DEBUG_LOG("malloc() failed: \n");
    return NULL;
  }
  status = psa_import_key(&key_attributes, key, len, &aes->key_id);
  if (status != PSA_SUCCESS) {
    os_free(aes);
    SL_DEBUG_LOG("Failed to import key: %d\n", status);
    return NULL;
  }
  aes->operation = (psa_cipher_operation_t *)os_malloc(sizeof(psa_cipher_operation_t));
  if (aes->operation == NULL) {
    psa_destroy_key(aes->key_id);
    os_free(aes);
    SL_DEBUG_LOG("malloc() failed: \n");
    return NULL;
  }
  *aes->operation = psa_cipher_operation_init();
  status          = psa_cipher_encrypt_setup(aes->operation, aes->key_id, PSA_ALG_ECB_NO_PADDING);
  if (status != PSA_SUCCESS) {
    aes_operation_cleanup(aes->key_id, aes->operation);
    os_free(aes->operation);
    os_free(aes);
    SL_DEBUG_LOG("Failed to setup cypher encryption: %d\n", status);
    return NULL;
  }
  return (void *)aes;
}

/**
 * aes_encrypt - AES encryption for a single block 
 * @ctx: AES operation context
 * @plain: Buffer containing the plain text
 * @crypt: Output buffer
 * 
 * Returns: 0 on success, -1 on failure.
 *
 * Function encrypts data using the AES algorithm. The function processes a block of plaintext and transforms it 
 * into ciphertext using the given context.
 */
int aes_encrypt(void *ctx, const u8 *plain, u8 *crypt)
{
  size_t output_length = 0;
  size_t output_size   = PSA_CIPHER_UPDATE_OUTPUT_SIZE(PSA_KEY_TYPE_AES, PSA_ALG_ECB_NO_PADDING, AES_BLOCK_SIZE);
  psa_status_t status  = psa_cipher_update(((struct multi_part_aes_ctx *)ctx)->operation,
                                          plain,
                                          AES_BLOCK_SIZE,
                                          crypt,
                                          output_size,
                                          &output_length);
  if (status != PSA_SUCCESS) {
    aes_operation_cleanup(((struct multi_part_aes_ctx *)ctx)->key_id, ((struct multi_part_aes_ctx *)ctx)->operation);
    SL_DEBUG_LOG("Failed to update cypher encryption: %d\n", status);
    return -1;
  }
  status = psa_cipher_finish(((struct multi_part_aes_ctx *)ctx)->operation, crypt, 0, &output_length);
  if (status != PSA_SUCCESS) {
    aes_operation_cleanup(((struct multi_part_aes_ctx *)ctx)->key_id, ((struct multi_part_aes_ctx *)ctx)->operation);
    SL_DEBUG_LOG("Failed to finish encryption: %d\n", status);
    return -1;
  }
  aes_operation_cleanup(((struct multi_part_aes_ctx *)ctx)->key_id, ((struct multi_part_aes_ctx *)ctx)->operation);
  return 0;
}

/**
 * aes_encrypt_deinit - Cleanup operation for AES encryption
 * @ctx: AES operation context
 * 
 * Returns: Does not return any value.
 *
 * Function free up the resources allocated for AES encryption operation
 */
void aes_encrypt_deinit(void *ctx)
{
  AES_CLEANUP_OP(ctx);
}

/**
 * aes_decrypt_init - Init operation for multi-part AES decryption
 * @key: Key for AES operation 
 * @len: Length of key in bytes
 * 
 * Returns: Return a void pointer
 *
 * Function setup the key and AES decryption operation state object for PSA API and returns a void pointer
 * to the AES operation state object 
 */
void *aes_decrypt_init(const u8 *key, size_t len)
{
  psa_status_t status = psa_crypto_init();
  if (status != PSA_SUCCESS) {
    SL_DEBUG_LOG("Failed to initialize PSA Crypto: %d\n", status);
    return NULL;
  }
  psa_key_attributes_t key_attributes = psa_key_attributes_init();
  psa_set_key_type(&key_attributes, PSA_KEY_TYPE_AES);
  psa_set_key_algorithm(&key_attributes, PSA_ALG_ECB_NO_PADDING);
  psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_DECRYPT);
  psa_set_key_bits(&key_attributes, len * 8);
  struct multi_part_aes_ctx *aes = (struct multi_part_aes_ctx *)os_malloc(sizeof(struct multi_part_aes_ctx));
  if (aes == NULL) {
    SL_DEBUG_LOG("malloc() failed: \n");
    return NULL;
  }
  status = psa_import_key(&key_attributes, key, len, &aes->key_id);
  if (status != PSA_SUCCESS) {
    os_free(aes);
    SL_DEBUG_LOG("Failed to import key (error %d)\n", (int)status);
    return NULL;
  }
  aes->operation = (psa_cipher_operation_t *)os_malloc(sizeof(psa_cipher_operation_t));
  if (aes->operation == NULL) {
    psa_destroy_key(aes->key_id);
    os_free(aes);
    SL_DEBUG_LOG("malloc() failed: \n");
    return NULL;
  }
  *aes->operation = psa_cipher_operation_init();
  status          = psa_cipher_decrypt_setup(aes->operation, aes->key_id, PSA_ALG_ECB_NO_PADDING);
  if (status != PSA_SUCCESS) {
    aes_operation_cleanup(aes->key_id, aes->operation);
    os_free(aes->operation);
    os_free(aes);
    SL_DEBUG_LOG("Failed to setup cypher decryption: %d\n", status);
    return NULL;
  }
  return (void *)aes;
}

/**
 * aes_decrypt - AES decryption for a single block 
 * @ctx: AES operation context
 * @crypt: Buffer containing the cipher text
 * @plain: Output buffer to store decrypted data 
 * 
 * Returns: 0 on success, -1 on failure.
 *
 * Function decrypts data using the AES algorithm. The function processes a block of cipher and transforms it 
 * into plain text with the context given.
 */
int aes_decrypt(void *ctx, const u8 *crypt, u8 *plain)
{
  size_t output_length = 0;
  size_t output_size   = PSA_CIPHER_UPDATE_OUTPUT_SIZE(PSA_KEY_TYPE_AES, PSA_ALG_ECB_NO_PADDING, AES_BLOCK_SIZE);
  psa_status_t status  = psa_cipher_update(((struct multi_part_aes_ctx *)ctx)->operation,
                                          crypt,
                                          AES_BLOCK_SIZE,
                                          plain,
                                          output_size,
                                          &output_length);
  if (status != PSA_SUCCESS) {
    aes_operation_cleanup(((struct multi_part_aes_ctx *)ctx)->key_id, ((struct multi_part_aes_ctx *)ctx)->operation);
    SL_DEBUG_LOG("Failed to update cypher decryption: %d\n", status);
    return -1;
  }
  // psa_cipher_finish() will be called in aes_decrypt_deinit() as block size input do not need cipher finish to complete the operation.
  return 0;
}

/**
 * aes_decrypt_deinit - Cleanup operation for AES decryption
 * @ctx: AES operation context
 * 
 * Returns: Does not return any value.
 *
 * Function free up the resources allocated for AES decryption operation
 */
void aes_decrypt_deinit(void *ctx)
{
  size_t output_length          = 0;
  uint8_t plain[AES_BLOCK_SIZE] = { 0 };

  if (ctx == NULL) {
    return;
  }

  psa_cipher_finish(((struct multi_part_aes_ctx *)ctx)->operation, &plain, 0, &output_length);
  aes_operation_cleanup(((struct multi_part_aes_ctx *)ctx)->key_id, ((struct multi_part_aes_ctx *)ctx)->operation);
  AES_CLEANUP_OP(ctx);
}

/**
 * pbkdf2_hmac -  PBKDF2 (Password-Based Key Derivation Function 2) function using PSA APIs
 * @passphrase: Passphrase to derive key from 
 * @salt: Salt used in the PBKDF2
 * @alg: Hash algorithm utilized in the PBKDF2
 * @salt_len: Length of salt in bytes
 * @iterations: Iteration count
 * @buf: Output buffer
 * @buflen: Output buffer length
 * 
 * Returns: 0 on success, -1 on failure.
 *
 * This function derives a secure key of the specified length using a password, underlaying hash algorithm 
 * to be used is passed as argument.
 */
static int pbkdf2_hmac(const char *passphrase,
                       const uint8_t *salt,
                       psa_algorithm_t alg,
                       size_t salt_len,
                       int iterations,
                       uint8_t *buf,
                       size_t buflen)
{
  psa_status_t status = psa_crypto_init();
  if (status != PSA_SUCCESS) {
    SL_DEBUG_LOG("Failed to initialize PSA Crypto: %d\n", status);
    return -1;
  }
  psa_key_derivation_operation_t operation = psa_key_derivation_operation_init();
  status                                   = psa_key_derivation_setup(&operation, PSA_ALG_PBKDF2_HMAC(alg));
  if (status != PSA_SUCCESS) {
    psa_key_derivation_abort(&operation);
    SL_DEBUG_LOG("Failed to setup key derivation: %d\n", status);
    return -1;
  }

  status = psa_key_derivation_input_integer(&operation, PSA_KEY_DERIVATION_INPUT_COST, iterations);
  if (status != PSA_SUCCESS) {
    psa_key_derivation_abort(&operation);
    SL_DEBUG_LOG("Failed to set iteration count: %d\n", status);
    return -1;
  }
  status = psa_key_derivation_input_bytes(&operation, PSA_KEY_DERIVATION_INPUT_SALT, salt, salt_len);
  if (status != PSA_SUCCESS) {
    psa_key_derivation_abort(&operation);
    SL_DEBUG_LOG("Failed to set salt: %d\n", status);
    return -1;
  }
  status = psa_key_derivation_input_bytes(&operation,
                                          PSA_KEY_DERIVATION_INPUT_PASSWORD,
                                          (const uint8_t *)passphrase,
                                          strlen(passphrase));
  if (status != PSA_SUCCESS) {
    psa_key_derivation_abort(&operation);
    SL_DEBUG_LOG("Failed to set passphrase: %d\n", status);
    return -1;
  }
  status = psa_key_derivation_output_bytes(&operation, buf, buflen);
  if (status != PSA_SUCCESS) {
    psa_key_derivation_abort(&operation);
    SL_DEBUG_LOG("Failed to derive key: %d\n", status);
    return -1;
  }
  psa_key_derivation_abort(&operation);
  return 0;
}

/**
 * pbkdf2_sha1 - PBKDF2 function using SHA-1 as hash function 
 * @passphrase: Passphrase to derive key from 
 * @ssid: SSID given as salt for PBKDF operation 
 * @ssid_len: Length of SSID
 * @iterations: Iteration count 
 * @buf: Output buffer
 * @buflen: Output buffer length
 * 
 * Returns: 0 on success, -1 on failure.
 *
 * Function implements the PBKDF2 algorithm using SHA-1 as the underlying hash function. 
 * It derives a secure cryptographic key from a password by applying the SHA-1 hash function 
 * multiple times (iterations) along with a unique salt value.
 */
int pbkdf2_sha1(const char *passphrase, const u8 *ssid, size_t ssid_len, int iterations, u8 *buf, size_t buflen)
{

  return pbkdf2_hmac(passphrase, ssid, PSA_ALG_SHA_1, ssid_len, iterations, buf, buflen);
}

/*PSA APIs end here*/

/*wrapper APIs*/
int hmac_md5(const uint8_t *key, size_t key_len, const uint8_t *data, size_t data_len, uint8_t *mac)
{
  return hmac_md5_vector(key, key_len, 1, &data, &data_len, mac);
}

int hmac_sha1(const u8 *key, size_t key_len, const u8 *data, size_t data_len, u8 *mac)
{
  return hmac_sha1_vector(key, key_len, 1, &data, &data_len, mac);
}

int hmac_sha256(const u8 *key, size_t key_len, const u8 *data, size_t data_len, u8 *mac)
{
  return hmac_sha256_vector(key, key_len, 1, &data, &data_len, mac);
}

int omac1_aes_128_vector(const u8 *key, size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
  return omac1_aes_vector(key, 16, num_elem, addr, len, mac);
}

int omac1_aes_128(const u8 *key, const u8 *data, size_t data_len, u8 *mac)
{
  return omac1_aes_128_vector(key, 1, &data, &data_len, mac);
}

/*wrapper APIs end*/

/*Bignum APIs start*/

/**
 * crypto_bignum_init - init function for bignum 
 * 
 * Returns: Returns: Pointer to allocated bignum or %NULL on failure.
 *
 * Function allocates memory for bignum and makes bignum ready to be used
 * but does not define a value for bignum
 */
struct crypto_bignum *crypto_bignum_init(void)
{
  mbedtls_mpi *mpi = os_malloc(sizeof(mbedtls_mpi));
  if (!mpi) {
    return NULL;
  }
  mbedtls_mpi_init(mpi);
  return (struct crypto_bignum *)mpi;
}

/**
 * crypto_bignum_to_bin - Export bignum to unsigned binary data
 * @a: the source bignum to export
 * @buf: Output buffer
 * @buflen: Output buffer length
 * @padlen: Length in octets to pad the result to or 0 to indicate no padding
 * 
 * Returns: Number of octets written on success, -1 on failure.
 *
 */
int crypto_bignum_to_bin(const struct crypto_bignum *a, u8 *buf, size_t buflen, size_t padlen)
{
  int num_bytes = 0, offset = 0;

  if (padlen > buflen) {
    return -1;
  }
  num_bytes = mbedtls_mpi_size((const mbedtls_mpi *)a);
  if ((size_t)num_bytes > buflen) {
    return -1;
  }
  if (padlen > (size_t)num_bytes) {
    offset = padlen - num_bytes;
  } else {
    offset = 0;
  }

  os_memset(buf, 0, offset);
  mbedtls_mpi_write_binary((const mbedtls_mpi *)a, buf + offset, num_bytes);

  return num_bytes + offset;
}

/**
 * crypto_bignum_deinit - Free bignum
 * @n: Bignum from crypto_bignum_init() or crypto_bignum_init_set()
 * @clear: Whether to clear the value from memory
 * 
 * Does not return any value
 * Deinit function that free bignum
 */
void crypto_bignum_deinit(struct crypto_bignum *n, int clear)
{
  if (!n) {
    return;
  }
  if (clear) {
    mbedtls_mpi_free((mbedtls_mpi *)n);
  } else {
    os_free(((mbedtls_mpi *)n)->private_p);
  }
  os_free(n);
}

/**
 * crypto_bignum_add - Bignum addition
 * @a: Bignum
 * @b: Bignum
 * @c: Bignum; used to store the result of a + b
 * Returns: 0 on success, -1 on failure
 * 
 * Function adds two bignums 'a' and 'b' and store result in 'c'
 */
int crypto_bignum_add(const struct crypto_bignum *a, const struct crypto_bignum *b, struct crypto_bignum *c)
{
  return ((mbedtls_mpi_add_mpi((mbedtls_mpi *)c, (const mbedtls_mpi *)a, (const mbedtls_mpi *)b) == 0) ? 0 : -1);
}

/**
 * crypto_bignum_rshift - right shift operation on bignum
 * @a: Bignum
 * @n: number of bits
 * @r: Bignum; used to store the result of a >> n
 * 
 * Returns: 0 on success, -1 on failure
 * 
 */
int crypto_bignum_rshift(const struct crypto_bignum *a, int n, struct crypto_bignum *r)
{
  if (mbedtls_mpi_safe_cond_assign((mbedtls_mpi *)r, (const mbedtls_mpi *)a, 1) != 0) {
    return -1;
  }
  return ((mbedtls_mpi_shift_r((mbedtls_mpi *)r, n) == 0) ? 0 : -1);
}

/**
 * crypto_bignum_is_zero - check if the given bignum is zero
 * @a: Bignum
 * 
 * Returns: 1 if bignum is zero, 0 if not
 */
int crypto_bignum_is_zero(const struct crypto_bignum *a)
{
  return ((mbedtls_mpi_cmp_int((const mbedtls_mpi *)a, 0) == 0) ? 1 : 0);
}

/**
 * crypto_bignum_is_one - Check if the given bignum is one
 * @a: Bignum
 * 
 * Returns: 1 if bignum is one, 0 if not
 */
int crypto_bignum_is_one(const struct crypto_bignum *a)
{
  return ((mbedtls_mpi_cmp_int((const mbedtls_mpi *)a, 1) == 0) ? 1 : 0);
}

/**
 * crypto_bignum_exptmod - Modular exponentiation operation
 * @a: Bignum; base
 * @b: Bignum; exponent
 * @c: Bignum; modulus
 * @d: Bignum; used to store the result of a^b (mod c)
 * 
 * Returns: 0 on success, -1 on failure
 * 
 */
int crypto_bignum_exptmod(const struct crypto_bignum *a,
                          const struct crypto_bignum *b,
                          const struct crypto_bignum *c,
                          struct crypto_bignum *d)
{
  return (
    (mbedtls_mpi_exp_mod((mbedtls_mpi *)d, (const mbedtls_mpi *)a, (const mbedtls_mpi *)b, (const mbedtls_mpi *)c, NULL)
     == 0)
      ? 0
      : -1);
}

/**
 * crypto_bignum_mod - Perform a modular reduction. C = A mod B
 * @a: bignum to compute the residue of.
 * @b: base of the modular reduction.
 * @c: destination bignum for the residue value.
 * 
 * Returns: 0 on success, -1 on failure
 * 
 */
int crypto_bignum_mod(const struct crypto_bignum *a, const struct crypto_bignum *b, struct crypto_bignum *c)
{
  return ((mbedtls_mpi_mod_mpi((mbedtls_mpi *)c, (const mbedtls_mpi *)a, (const mbedtls_mpi *)b) == 0) ? 0 : -1);
}

/**
 * crypto_bignum_inverse - Inverse a bignum so that a * c = 1 (mod b)
 * @a: bignum to calculate the modular inverse of.
 * @b: base of the modular inversion.
 * @c: destination bignum.
 * 
 * Returns: 0 on success, -1 on failure
 * 
 */
int crypto_bignum_inverse(const struct crypto_bignum *a, const struct crypto_bignum *b, struct crypto_bignum *c)
{
  return ((mbedtls_mpi_inv_mod((mbedtls_mpi *)c, (const mbedtls_mpi *)a, (const mbedtls_mpi *)b) == 0) ? 0 : -1);
}

/**
 * crypto_bignum_mulmod - modular multiplication d = a * b (mod c)
 * @a: bignum, first factor
 * @b: bignum, second factor
 * @c: base of mod 
 * @d: bignum to store result
 * 
 * Returns: 0 on success, -1 on failure
 * 
 */
int crypto_bignum_mulmod(const struct crypto_bignum *a,
                         const struct crypto_bignum *b,
                         const struct crypto_bignum *c,
                         struct crypto_bignum *d)
{
  int ret = 0;
  mbedtls_mpi tmp;
  mbedtls_mpi_init(&tmp);
  MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&tmp, (const mbedtls_mpi *)a, (const mbedtls_mpi *)b));
  MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi((mbedtls_mpi *)d, (const mbedtls_mpi *)&tmp, (const mbedtls_mpi *)c));
cleanup:
  mbedtls_mpi_free(&tmp);
  return ((ret == 0) ? 0 : -1);
}

/**
 * crypto_bignum_init_set - Allocate memory for bignum and set the value
 * @buf: Buffer with unsigned binary value
 * @len: Length of buf in octets
 * 
 * Returns: Pointer to allocated bignum or %NULL on failure
 */
struct crypto_bignum *crypto_bignum_init_set(const u8 *buf, size_t len)
{
  mbedtls_mpi *mpi = os_malloc(sizeof(mbedtls_mpi));
  if (!mpi) {
    return NULL;
  }
  mbedtls_mpi_init(mpi);
  if (mbedtls_mpi_read_binary(mpi, buf, len) != 0) {
    crypto_bignum_deinit((struct crypto_bignum *)mpi, 1);
    return NULL;
  }
  return (struct crypto_bignum *)mpi;
}

/**
 * crypto_bignum_cmp - Compare two bignums
 * @a: left-hand bignum
 * @b: right-hand bignum
 * 
 * Returns: 1 if a is greater than b. -1 if a is lesser than b. 0 if a is equal to b.
 */
int crypto_bignum_cmp(const struct crypto_bignum *a, const struct crypto_bignum *b)
{
  return mbedtls_mpi_cmp_mpi((const mbedtls_mpi *)a, (const mbedtls_mpi *)b);
}

/**
 * crypto_bignum_sub - Perform a signed subtraction of bignums: C = A - B
 * @a: bignum,minuend
 * @b: bignum, subtrahend
 * @c: destination bignum
 * 
 * Returns:  0 on success, -1 on failure
 */
int crypto_bignum_sub(const struct crypto_bignum *a, const struct crypto_bignum *b, struct crypto_bignum *c)
{
  return ((mbedtls_mpi_sub_mpi((mbedtls_mpi *)c, (const mbedtls_mpi *)a, (const mbedtls_mpi *)b) == 0) ? 0 : -1);
}

/**
 * crypto_bignum_init_uint - Allocate memory for bignum and set the value (uint)
 * @val: Value to set
 *
 * Returns: Pointer to allocated bignum or %NULL on failure
 */
struct crypto_bignum *crypto_bignum_init_uint(unsigned int val)
{
  mbedtls_mpi *a = (mbedtls_mpi *)crypto_bignum_init();
  if (!a) {
    return NULL;
  }
#ifdef MBEDTLS_IS_BIG_ENDIAN
  if (mbedtls_mpi_read_binary(a, (const unsigned char *)&val, 4) != 0)
#else
  if (mbedtls_mpi_read_binary_le(a, (const unsigned char *)&val, 4) != 0)
#endif
  {
    crypto_bignum_deinit((struct crypto_bignum *)a, 1);
    return NULL;
  }
  return (struct crypto_bignum *)a;
}

/**
 * crypto_bignum_bits - Return the number of bits up to and including the most significant bit of value
 * @a: bignum
 *
 * Returns: number of bits up to and including the most significant bit of value
 */
int crypto_bignum_bits(const struct crypto_bignum *a)
{
  return mbedtls_mpi_bitlen((mbedtls_mpi *)a);
}

/**
 * crypto_bignum_addmod - modular addition d = a + b (mod c)
 * @a: bignum, first summand
 * @b: bignum, second summand
 * @c: base of mod 
 * @d: bignum to store result
 * 
 * Returns: 0 on success, -1 on failure
 * 
 */
int crypto_bignum_addmod(const struct crypto_bignum *a,
                         const struct crypto_bignum *b,
                         const struct crypto_bignum *c,
                         struct crypto_bignum *d)
{
  int ret = 0;
  mbedtls_mpi tmp;
  mbedtls_mpi_init(&tmp);
  MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&tmp, (mbedtls_mpi *)a, (mbedtls_mpi *)b));
  MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi((mbedtls_mpi *)d, (const mbedtls_mpi *)&tmp, (const mbedtls_mpi *)c));
cleanup:
  mbedtls_mpi_free(&tmp);
  return ((ret == 0) ? 0 : -1);
}

/**
 * crypto_bignum_sqrmod - c = a^2 (mod b)
 * @a: bignum
 * @b: base of mod 
 * @c: bignum to store result
 * 
 * Returns: 0 on success, -1 on failure
 * 
 */
int crypto_bignum_sqrmod(const struct crypto_bignum *a, const struct crypto_bignum *b, struct crypto_bignum *c)
{
  int ret = 0;
  mbedtls_mpi tmp;
  mbedtls_mpi_init(&tmp);
  MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&tmp, (mbedtls_mpi *)a, (mbedtls_mpi *)a));
  MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi((mbedtls_mpi *)c, (const mbedtls_mpi *)&tmp, (const mbedtls_mpi *)b));
cleanup:
  mbedtls_mpi_free(&tmp);
  return ((ret == 0) ? 0 : -1);
}

/**
 * crypto_bignum_legendre - Compute the Legendre symbol (a/p)
 * @a: bignum
 * @p: bignum, prime number
 * 
 * Returns: Legendre symbol -1,0,1 on success; -2 on calculation failure
 * 
 */
int crypto_bignum_legendre(const struct crypto_bignum *a, const struct crypto_bignum *p)
{
  int res           = -2;
  unsigned int mask = 0;
  int ret           = 0;

  mbedtls_mpi exp, tmp;
  mbedtls_mpi_init(&exp);
  mbedtls_mpi_init(&tmp);
  /* exp = (p-1) / 2 */
  MBEDTLS_MPI_CHK(mbedtls_mpi_sub_int(&exp, (const mbedtls_mpi *)p, 1));
  MBEDTLS_MPI_CHK(mbedtls_mpi_shift_r(&exp, 1));
  MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(&tmp, (const mbedtls_mpi *)a, &exp, (const mbedtls_mpi *)p, NULL));

  /* Return 1 if tmp == 1, 0 if tmp == 0, or -1 otherwise. Need to use
	 * constant time selection to avoid branches here. */
  res  = -1;
  mask = const_time_eq(mbedtls_mpi_cmp_int(&tmp, 1), 0);
  res  = const_time_select_int(mask, 1, res);
  mask = const_time_eq(mbedtls_mpi_cmp_int(&tmp, 0), 0);
  res  = const_time_select_int(mask, 0, res);

cleanup:
  mbedtls_mpi_free(&exp);
  mbedtls_mpi_free(&tmp);
  return res;
}

/*Bignum APIs end*/

/*ECC APIs start*/

/**
 * crypto_ec_init - Initialize and load ECC curve
 * @group: group id of the ECC curve
 * 
 * Returns: pointer to crypto_ec
 * BP-224 and P-224 are not supported
 */
struct crypto_ec *crypto_ec_init(int group)
{
  int ret = 0;
  mbedtls_ecp_group_id curve_id;

  switch (group) {
    case 19:
      curve_id = MBEDTLS_ECP_DP_SECP256R1;
      break;
    case 20:
      curve_id = MBEDTLS_ECP_DP_SECP384R1;
      break;
    case 21:
      curve_id = MBEDTLS_ECP_DP_SECP521R1;
      break;
    case 25:
      curve_id = MBEDTLS_ECP_DP_SECP192R1;
      break;
    // case 26:
    //   curve_id = MBEDTLS_ECP_DP_SECP224R1; // Not supported since Prime P mod 4 !=3
    //   break;
    // case 27:
    // 	curve_id = ECC_BRAINPOOLP224R1;      // FIXME: 224-bit Brainpool curve is not supported in mbedtls
    // 	break;
    case 28:
      curve_id = MBEDTLS_ECP_DP_BP256R1;
      break;
    case 29:
      curve_id = MBEDTLS_ECP_DP_BP384R1;
      break;
    case 30:
      curve_id = MBEDTLS_ECP_DP_BP512R1;
      break;
    default:
      return NULL;
  }

  struct crypto_ec *ec = (struct crypto_ec *)os_zalloc(sizeof(struct crypto_ec));
  if (!ec) {
    return NULL;
  }
  ec->grp = (mbedtls_ecp_group *)os_zalloc(sizeof(mbedtls_ecp_group));
  if (!(ec->grp)) {
    os_free(ec);
    return NULL;
  }
  mbedtls_ecp_group_init(ec->grp);
  ec->A = (mbedtls_mpi *)os_zalloc(sizeof(mbedtls_mpi));
  if (!(ec->A)) {
    os_free(ec->grp);
    os_free(ec);
    return NULL;
  }

  MBEDTLS_MPI_CHK(mbedtls_ecp_group_load(ec->grp, curve_id));
  mbedtls_mpi_init(ec->A);

  /*The default implementation only initializes A without setting it to it's value for curves 
    with A = -3(SECP256R1,SECP384R1, etc), in which case you need to load A by yourself when 
    using domain parameters*/
  if (mbedtls_ecp_group_a_is_minus_3((mbedtls_ecp_group *)ec->grp)) {
    MBEDTLS_MPI_CHK(mbedtls_mpi_sub_int(ec->A, &((mbedtls_ecp_group *)ec->grp)->P, 3));
  } else {
    MBEDTLS_MPI_CHK(mbedtls_mpi_copy(ec->A, &((mbedtls_ecp_group *)ec->grp)->A));
  }
  return ec;
cleanup:
  crypto_ec_deinit(ec);
  return NULL;
}

/**
 * crypto_ec_deinit - Free components used by elliptic curve grp
 * @e: elliptic curve grp
 * 
 * Returns: does not return any value
 * 
 */
void crypto_ec_deinit(struct crypto_ec *e)
{
  mbedtls_ecp_group_free(e->grp);
  os_free(e->grp);
  mbedtls_mpi_free(e->A);
  os_free(e->A);
  os_free(e);
}

/**
 * crypto_ec_point_init - Init function for elliptic curve point
 * @e: elliptic curve grp
 * 
 * Returns: pointer to new elliptic curve point
 * Allocate memory and initialse point as zero
 */
struct crypto_ec_point *crypto_ec_point_init(struct crypto_ec *e)
{
  if (!e) {
    return NULL;
  }
  mbedtls_ecp_point *ecp_pt = os_malloc(sizeof(mbedtls_ecp_point));
  if (!ecp_pt) {
    return NULL;
  }
  mbedtls_ecp_point_init(ecp_pt);
  return (struct crypto_ec_point *)ecp_pt;
}

/**
 * crypto_ec_point_deinit - deinit function for elliptic curve point
 * @p: elliptic curve point
 * @clear: clear flag
 * 
 * Returns: does not return any value
 * Clear and free the elliptic curve point
 */
void crypto_ec_point_deinit(struct crypto_ec_point *p, int clear)
{
  (void)clear;
  mbedtls_ecp_point_free((mbedtls_ecp_point *)p);
  os_free(p);
}

/**
 * crypto_ec_prime_len_bits - get number of bits prime number from elliptic curve group
 * @e: elliptic curve grp
 * 
 * Returns: number of bits 
 */
size_t crypto_ec_prime_len_bits(struct crypto_ec *e)
{
  return (e->grp->pbits);
}

/**
 * crypto_ec_get_prime - get prime number from elliptic curve group
 * @e: elliptic curve grp
 * 
 * Returns: bignum pointer
 */
const struct crypto_bignum *crypto_ec_get_prime(struct crypto_ec *e)
{
  return (const struct crypto_bignum *)&(e->grp->P);
}

/**
 * crypto_ec_get_order - get order of elliptic curve
 * @e: elliptic curve grp
 * 
 * Returns: bignum pointer to order
 */
const struct crypto_bignum *crypto_ec_get_order(struct crypto_ec *e)
{
  return (const struct crypto_bignum *)&(e->grp->N);
}

/**
 * crypto_ec_prime_len - get length of prime in bytes
 * @e: elliptic curve grp
 * 
 * Returns: number of bytes
 */
size_t crypto_ec_prime_len(struct crypto_ec *e)
{
  return (mbedtls_mpi_size(&(e->grp->P)));
}

/**
 * crypto_ec_order_len - get size of order in bytes
 * @e: elliptic curve grp
 * 
 * Returns: number of bytes
 */
size_t crypto_ec_order_len(struct crypto_ec *e)
{
  return (mbedtls_mpi_size(&(e->grp->N)));
}

/**
 * crypto_ec_get_b - get B value in the equation of Short Weierstrass curve 
 * @e: elliptic curve grp
 * 
 * Returns: bignum pointer
 */
const struct crypto_bignum *crypto_ec_get_b(struct crypto_ec *e)
{
  return (const struct crypto_bignum *)&(e->grp->B);
}

/**
 * crypto_ec_get_a - get A value in the equation of Short Weierstrass curve 
 * @e: elliptic curve grp
 * 
 * Returns: bignum pointer
 */
const struct crypto_bignum *crypto_ec_get_a(struct crypto_ec *e)
{
  return (const struct crypto_bignum *)(e->A);
}

/**
 * crypto_ec_point_from_bin - import a elliptic curve point from unsinged binary data
 * @e: elliptic curve grp
 * @val: buffer containing binary data
 * 
 * Returns: pointer to ec point on success, NULL on failure
 */
struct crypto_ec_point *crypto_ec_point_from_bin(struct crypto_ec *e, const uint8_t *val)
{
  int ret      = 0;
  size_t p_len = mbedtls_mpi_size(&(e->grp->P));
  uint8_t *buf = (uint8_t *)os_malloc((2 * p_len + 1));
  if (!buf) {
    return NULL;
  }

  /*uncompressed point is represented as 0x04∣∣X∣∣Y, 0x04 is the prefix added to specify the format
   X and Y are the coordinates of the point*/
  buf[0] = 0x04;
  os_memcpy(buf + 1, val, 2 * p_len);
  mbedtls_ecp_point *p = (mbedtls_ecp_point *)crypto_ec_point_init(e);
  if (!p) {
    os_free(buf);
    return NULL;
  }
  ret = mbedtls_ecp_point_read_binary(e->grp, p, (const unsigned char *)buf, 2 * p_len + 1);
  os_free(buf);
  if (ret == 0) {
    return (struct crypto_ec_point *)p;
  }
  crypto_ec_point_deinit((struct crypto_ec_point *)p, 1);
  return NULL;
}

/**
 * crypto_ec_point_to_bin - export a elliptic curve point to binary buffer
 * @e: elliptic curve grp
 * @point: elliptic curve point
 * @x: buffer for X coordinate
 * @y: buffer for Y coordinate
 * 
 * Returns: 0 on success and -1 on failure
 */
int crypto_ec_point_to_bin(struct crypto_ec *e, const struct crypto_ec_point *point, uint8_t *x, uint8_t *y)
{
  int ret      = 0;
  size_t p_len = mbedtls_mpi_size(&(e->grp->P));
  size_t o_len = 0;
  if (x == NULL || y == NULL) {
    return -1;
  }
  uint8_t *buf = (uint8_t *)os_malloc(2 * p_len + 1);
  if (!buf) {
    return -1;
  }
  MBEDTLS_MPI_CHK(mbedtls_ecp_point_write_binary(e->grp,
                                                 (const mbedtls_ecp_point *)point,
                                                 MBEDTLS_ECP_PF_UNCOMPRESSED,
                                                 &o_len,
                                                 buf,
                                                 2 * p_len + 1));
  if (o_len == 1) {
    os_free(buf);
    return -1;
  }
  os_memcpy(x, buf + 1, p_len);
  os_memcpy(y, buf + 1 + p_len, p_len);

  os_free(buf);
  return 0;
cleanup:
  os_free(buf);
  return -1;
}

/**
 * crypto_ec_point_is_on_curve - checks if a point is on the curve
 * @e: elliptic curve grp
 * @p: elliptic curve point
 * 
 * Returns: 1 if point is on the curve, 0 otherwise
 */
int crypto_ec_point_is_on_curve(struct crypto_ec *e, const struct crypto_ec_point *p)
{
  return (mbedtls_ecp_check_pubkey(e->grp, (const mbedtls_ecp_point *)p) == 0 ? 1 : 0);
}

/**
 * crypto_ec_point_compute_y_sqr - Computes Y^2 value from X
 * @e: elliptic curve grp
 * @x: X coordinate in Y^2 = X^3 + aX + b
 * 
 * Returns: bignum pointer on success NULL on failure
 */
struct crypto_bignum *crypto_ec_point_compute_y_sqr(struct crypto_ec *e, const struct crypto_bignum *x)
{
  mbedtls_mpi tmp, *y_sqr;
  int ret = 0;
  y_sqr   = (mbedtls_mpi *)os_malloc(sizeof(mbedtls_mpi));
  if (!y_sqr) {
    return NULL;
  }
  mbedtls_mpi_init(&tmp);
  mbedtls_mpi_init(y_sqr);

  /* y^2 = x^3 + ax + b */

  MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&tmp, (const mbedtls_mpi *)x, (const mbedtls_mpi *)x));
  MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&tmp, &tmp, (const mbedtls_mpi *)x));
  MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(y_sqr, e->A, (const mbedtls_mpi *)x));
  MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&tmp, &tmp, y_sqr));
  MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&tmp, &tmp, &(e->grp->B)));
  MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(y_sqr, &tmp, &(e->grp->P)));
  mbedtls_mpi_free(&tmp);
  return (struct crypto_bignum *)y_sqr;

cleanup:
  mbedtls_mpi_free(&tmp);
  mbedtls_mpi_free(y_sqr);
  os_free(y_sqr);
  return NULL;
}

/**
 * crypto_ec_point_solve_y_coord - Computes Y value from X and parity of Y
 * @e: elliptic curve grp
 * @p: elliptic curve point
 * @x: X coordinate in Y^2 = X^3 + aX + b
 * @y_bit: parity of Y coodrdanate
 * 
 * Returns: 0 on success and -1 on failure
 * calculates Y from X and parity of Y, then imports elliptic curve point from X and Y
 */
int crypto_ec_point_solve_y_coord(struct crypto_ec *e,
                                  struct crypto_ec_point *p,
                                  const struct crypto_bignum *x,
                                  int y_bit)
{
  mbedtls_mpi y_coord;
  mbedtls_mpi_init(&y_coord);

  /*check if prime P of the ECC group satisfy the property P mod 4 = 3
    return -1 if not satisfied*/
  if (mbedtls_mpi_get_bit(&e->grp->P, 0) != 1 || mbedtls_mpi_get_bit(&e->grp->P, 1) != 1) {
    return -1;
  }
  mbedtls_mpi exp;
  mbedtls_mpi_init(&exp);

  int ret;

  y_coord = *(mbedtls_mpi *)crypto_ec_point_compute_y_sqr(e, x);
  /* w = y^2 */ /* Y contains y^2 intermediate result */
                /* exp = ((p+1)/4) */
  MBEDTLS_MPI_CHK(mbedtls_mpi_add_int(&exp, &e->grp->P, 1));
  MBEDTLS_MPI_CHK(mbedtls_mpi_shift_r(&exp, 2));
  /* sqrt(w) = w^((p+1)/4) mod p   (for prime p where p = 3 mod 4) */
  MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(&y_coord, &y_coord /*y^2*/, &exp, &e->grp->P, NULL));

  /* check parity bit match or else invert Y */
  /* This quick inversion implementation is valid because Y != 0 for all
       * Short Weierstrass curves supported by mbedtls, as each supported curve
       * has an order that is a large prime, so each supported curve does not
       * have any point of order 2, and a point with Y == 0 would be of order 2 */
  if (mbedtls_mpi_get_bit(&y_coord, 0) != y_bit) {
    MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(&y_coord, &e->grp->P, &y_coord));
  }
  MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&((mbedtls_ecp_point *)p)->private_X, (mbedtls_mpi *)x));
  MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&((mbedtls_ecp_point *)p)->private_Y, &y_coord));
  MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&((mbedtls_ecp_point *)p)->private_Z, 1));
cleanup:
  mbedtls_mpi_free(&exp);
  mbedtls_mpi_free(&y_coord);
  return ((ret == 0) ? 0 : -1);
}

/**
 * crypto_ec_point_add - ecc point addition
 * @e: elliptic curve grp
 * @a: elliptic curve point
 * @b: elliptic curve point
 * @c: point to store result
 * 
 * Returns: 0 on success, -1 on failure
 * add two ECC points c = a + b
 */
int crypto_ec_point_add(struct crypto_ec *e,
                        const struct crypto_ec_point *a,
                        const struct crypto_ec_point *b,
                        struct crypto_ec_point *c)
{
  int ret = 0;
  if (mbedtls_ecp_is_zero((mbedtls_ecp_point *)a)) {
    ret = mbedtls_ecp_copy((mbedtls_ecp_point *)c, (const mbedtls_ecp_point *)b);
    return ((ret == 0) ? 0 : -1);
  }
  if (mbedtls_ecp_is_zero((mbedtls_ecp_point *)b)) {
    ret = mbedtls_ecp_copy((mbedtls_ecp_point *)c, (const mbedtls_ecp_point *)a);
    return ((ret == 0) ? 0 : -1);
  }

  mbedtls_mpi m;
  mbedtls_mpi_init(&m);
  mbedtls_mpi_lset(&m, 1);
  ret = mbedtls_ecp_muladd(e->grp,
                           (mbedtls_ecp_point *)c,
                           (const mbedtls_mpi *)&m,
                           (const mbedtls_ecp_point *)a,
                           (const mbedtls_mpi *)&m,
                           (const mbedtls_ecp_point *)b);
  mbedtls_mpi_free(&m);
  return ((ret == 0) ? 0 : -1);
}

/**
 * crypto_ec_point_invert - invert a elliptic curve point
 * @e: elliptic curve grp
 * @p: elliptic curve point
 * 
 * Returns: 0 on success, -1 on failure
 * 
 * inverse of a point (X,Y) is (X,-Y)
 */
int crypto_ec_point_invert(struct crypto_ec *e, struct crypto_ec_point *p)
{
  return (mbedtls_mpi_sub_mpi(&((mbedtls_ecp_point *)p)->private_Y, &(e->grp->P), &((mbedtls_ecp_point *)p)->private_Y)
              == 0
            ? 0
            : -1);
}

/**
 * crypto_ec_point_is_at_infinity - check if point is a at infinity
 * @e: elliptic curve grp
 * @point: elliptic curve point
 * 
 * Returns: 1 if point is a point at infinity 0 otherwise
 */
int crypto_ec_point_is_at_infinity(struct crypto_ec *e, const struct crypto_ec_point *p)
{
  (void)e;
  if (!p) {
    return -1;
  }
  return (mbedtls_ecp_is_zero((mbedtls_ecp_point *)p) == 0 ? 1 : 0);
}

/**
 * self_rng - RNG function
 * @ctx: RNG context
 * @out: output buffer
 * @len: size of buffer or output in bytes
 * 
 * Returns: 0 on success, -1 on failure
 */
static int self_rng(void *ctx, unsigned char *out, size_t len)
{
  (void)ctx;
  if (crypto_get_random(out, len)) {
    return -1;
  }
  return 0;
}

/**
 * crypto_ec_point_mul - ecc point multiplication
 * @e: elliptic curve grp
 * @p: elliptic curve point
 * @b: scalar value
 * @res: point to store result
 * 
 * Returns: 0 on success, -1 on failure
 */
int crypto_ec_point_mul(struct crypto_ec *e,
                        const struct crypto_ec_point *p,
                        const struct crypto_bignum *b,
                        struct crypto_ec_point *res)
{
  int ret = 0;
  mbedtls_mpi tmp;
  mbedtls_mpi_init(&tmp);
  MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&tmp, (const mbedtls_mpi *)b, &(e->grp->N)));

  /*check if P is point at infinity or scalar is multiple of order N (tmp = 0), then set res as point at infinity*/
  if ((mbedtls_ecp_is_zero((mbedtls_ecp_point *)p)) || (mbedtls_mpi_cmp_int(&tmp, 0) == 0)) {
    mbedtls_mpi_free(&tmp);
    mbedtls_ecp_set_zero((mbedtls_ecp_point *)res);
    return 0;
  }
  MBEDTLS_MPI_CHK(mbedtls_ecp_mul(e->grp, (mbedtls_ecp_point *)res, &tmp, (mbedtls_ecp_point *)p, self_rng, NULL));
cleanup:
  mbedtls_mpi_free(&tmp);
  return ((ret == 0) ? 0 : -1);
}

/**
 * crypto_ec_point_cmp - ecc point comparison
 * @e: elliptic curve grp
 * @a: elliptic curve point
 * @b: scalar value
 * 
 * Returns: 0 if points are same, 1 otherwise
 */
int crypto_ec_point_cmp(const struct crypto_ec *e, const struct crypto_ec_point *a, const struct crypto_ec_point *b)
{
  (void)e;
  return (mbedtls_ecp_point_cmp((mbedtls_ecp_point *)a, (mbedtls_ecp_point *)b) == 0 ? 0 : 1);
}

/*ECC APIs end*/
/*dummy APIs start here*/

int rc4_skip(const u8 *key, size_t keylen, size_t skip, u8 *data, size_t data_len)
{
  return 0;
}

int crypto_mod_exp_hw(const NN_DIGIT *base,
                      uint32 base_len,
                      const NN_DIGIT *power,
                      uint32 power_len,
                      const NN_DIGIT *modulus,
                      uint32 modulus_len,
                      NN_DIGIT *result,
                      uint32 *result_len)
{
  return 0;
}

int md4_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
  return 0;
}

/*dummy APIs end here*/