/*******************************************************************************
* @file  rsi_hmac.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
/*************************************************************************
 *
 */

#include "stdint.h"
#include "rsi_sha.h"
#include "sha_hmac_common.h"

#define BAD_FUNC_ARG -173
#define NULL         0
#define RSI_SHA1     1
#define RSI_SHA256   2
#define RSI_SHA384   3
#define RSI_SHA512   4

#define HmacSetKey wc_HmacSetKey
#define HmacUpdate wc_HmacUpdate
#define HmacFinal  wc_HmacFinal

typedef struct rsi_hmac_sha_req_s {
  uint8_t algorithm_type;
  uint8_t algorithm_sub_type;
  uint16_t total_msg_length;
  uint8_t key_length;
} rsi_hmac_sha_req_t;

enum {

  IPAD = 0x36,
  OPAD = 0x5C,

/* Select the largest available hash for the buffer size. */
#if defined(HMAC_512)
  MAX_DIGEST_SIZE = SHA512_DIGEST_SIZE,
  HMAC_BLOCK_SIZE = SHA512_BLOCK_SIZE
#elif defined(HMAC_384)
  MAX_DIGEST_SIZE = SHA384_DIGEST_SIZE,
  HMAC_BLOCK_SIZE = SHA384_BLOCK_SIZE
#elif defined(HMAC_256)
  MAX_DIGEST_SIZE = SHA256_DIGEST_SIZE,
  HMAC_BLOCK_SIZE = SHA256_BLOCK_SIZE
#elif defined(HMAC_1)
  MAX_DIGEST_SIZE = SHA_DIGEST_SIZE,
  HMAC_BLOCK_SIZE = SHA_BLOCK_SIZE
#else
#error "You have to have some kind of hash if you want to use HMAC."
#endif
};

/* hash union */
typedef union {
  Sha sha;
  Sha256 sha256;
  Sha384 sha384;
  Sha512 sha512;
} Hash;

/* Hmac digest */
typedef struct Hmac {
  Hash hash;
  uint32_t ipad[HMAC_BLOCK_SIZE / sizeof(uint32_t)]; /* same block size all*/
  uint32_t opad[HMAC_BLOCK_SIZE / sizeof(uint32_t)];
  uint32_t innerHash[MAX_DIGEST_SIZE / sizeof(uint32_t)];
  void *heap;             /* heap hint */
  uint8_t macType;        /* md5 sha or sha256 */
  uint8_t innerHashKeyed; /* keyed flag */

} Hmac;

/* does init */
int wc_HmacSetKey(Hmac *, int type, const uint8_t *key, uint32_t keySz);
int wc_HmacUpdate(Hmac *, const uint8_t *, uint32_t);
int wc_HmacFinal(Hmac *, uint8_t *);
int wc_HmacSizeByType(int type);
int wolfSSL_GetHmacMaxSize(void);

int16_t rsi_crypto_HMAC(rsi_hmac_sha_req_t *crypto, uint16_t *output_length, uint8_t *output, uint8_t *sha_data);

int hmac_vector(int type,
                const uint8_t *key,
                unsigned int key_len,
                unsigned int num_elem,
                const uint8_t *addr[],
                const unsigned int *len,
                uint8_t *mac);
int hmac_sha1_vector(const uint8_t *key,
                     unsigned int key_len,
                     unsigned int num_elem,
                     const uint8_t *addr[],
                     const unsigned int *len,
                     uint8_t *mac);
int hmac_sha1(const uint8_t *key, unsigned int key_len, const uint8_t *data, unsigned int data_len, uint8_t *mac);
int hmac_sha256_vector(const uint8_t *key,
                       unsigned int key_len,
                       unsigned int num_elem,
                       const uint8_t *addr[],
                       const unsigned int *len,
                       uint8_t *mac);
int hmac_sha256(const uint8_t *key, unsigned int key_len, const uint8_t *data, unsigned int data_len, uint8_t *mac);
int hmac_sha384_vector(const uint8_t *key,
                       unsigned int key_len,
                       unsigned int num_elem,
                       const uint8_t *addr[],
                       const unsigned int *len,
                       uint8_t *mac);
int hmac_sha512_vector(const uint8_t *key,
                       unsigned int key_len,
                       unsigned int num_elem,
                       const uint8_t *addr[],
                       const unsigned int *len,
                       uint8_t *mac);

#ifdef __cplusplus
} /* extern "C" */
#endif
