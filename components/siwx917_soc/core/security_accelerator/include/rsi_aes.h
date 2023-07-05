/*******************************************************************************
* @file  rsi_aes.h
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

#define DYNAMIC_TYPE_TMP_BUFFER 38

#define AesSetKey            wc_AesSetKey
#define AesSetIV             wc_AesSetIV
#define AesCbcEncrypt        wc_AesCbcEncrypt
#define AesCbcDecrypt        wc_AesCbcDecrypt
#define AesCbcDecryptWithKey wc_AesCbcDecryptWithKey
#define AesEncrypt           wc_AesEncrypt
#define AesDecrypt           wc_AesDecrypt

typedef struct rsi_aes_req_s {
  uint8_t algorithm_type;
  uint8_t algorithm_sub_type;
  uint16_t total_msg_length;
  uint8_t encrypt_decryption;
  uint8_t key_length;
  uint8_t key[32];
  uint8_t IV[16];
  uint8_t msg[1400];
} rsi_aes_req_t;

#define AES_ENC_TYPE   1 /* cipher unique type */
#define AES_BLOCK_SIZE 16

typedef struct Aes {
  /* AESNI needs key first, rounds 2nd, not sure why yet */
  uint32_t key[60];
  uint32_t rounds;

  uint32_t reg[AES_BLOCK_SIZE / sizeof(uint32_t)]; /* for CBC mode */
  uint32_t tmp[AES_BLOCK_SIZE / sizeof(uint32_t)]; /* same         */

  int keylen;

  void *heap; /* memory hint to use */
} Aes;

int16_t rsi_crypto_aes(rsi_aes_req_t *crypto, uint16_t *output_length, uint8_t *output, uint8_t *crypto_data);

void wc_AesEncrypt(Aes *aes, const uint8_t *inBlock, uint8_t *outBlock);
void wc_AesDecrypt(Aes *aes, const uint8_t *inBlock, uint8_t *outBlock);
int wc_AesCbcEncrypt(Aes *aes, uint8_t *out, const uint8_t *in, uint32_t sz);
int wc_AesCbcDecrypt(Aes *aes, uint8_t *out, const uint8_t *in, uint32_t sz);
