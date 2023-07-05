/*******************************************************************************
* @file  aes.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "rsi_aes.h"
#include "rsi_aes_hw_9116.h"
/*==============================================*/
/**
 * @fn          void wc_AesEncrypt(Aes *aes, const uint8_t *inBlock, uint8_t *outBlock)
 * @brief       This API is used to encrypt the key for AES
 * @param[in]   aes, pointer to aes 
 * @param[in]   inBlock, Pointer to input block
 * @param[out]  outBlock, Pointer to output block
 * @return      none
 */
void wc_AesEncrypt(Aes *aes, const uint8_t *inBlock, uint8_t *outBlock)
{
  uint8_t aes_mode;
  uint8_t *tmp = 0;
  if (aes->keylen == 16) {
    aes_mode = AES_128;
  } else if (aes->keylen == 24) {
    aes_mode = AES_192;
  } else {
    aes_mode = AES_256;
  }
  tmp = (uint8_t *)malloc(AES_BLOCK_SIZE);

  if (tmp == NULL)
    return;

  memcpy(tmp, inBlock, AES_BLOCK_SIZE);

  aes_setkey_hw_9116((const byte *)aes->key, aes_mode);

  aes_hw_9116(tmp, AES_BLOCK_SIZE, AES_ENCRYPTION, AES_ECB, aes_mode, tmp);

  memcpy(outBlock, tmp, AES_BLOCK_SIZE);
  free(tmp);

  return;
}
/*==============================================*/
/**
 * @fn          void wc_AesDecrypt(Aes *aes, const uint8_t *inBlock, uint8_t *outBlock)
 * @brief       This API is used to decrypt the key for AES
 * @param[in]   aes, pointer to aes 
 * @param[in]   inBlock, Pointer to input block
 * @param[out]  outBlock, Pointer to output block
 * @return      none
 */
void wc_AesDecrypt(Aes *aes, const uint8_t *inBlock, uint8_t *outBlock)
{
  uint8_t aes_mode;

  if (aes->keylen == 16) {
    aes_mode = AES_128;
  } else if (aes->keylen == 24) {
    aes_mode = AES_192;
  } else {
    aes_mode = AES_256;
  }
  /* if input and output same will overwrite input iv */
  memcpy(aes->tmp, inBlock, AES_BLOCK_SIZE);

  aes_setkey_hw_9116((const byte *)aes->key, aes_mode);

  aes_hw_9116(inBlock, AES_BLOCK_SIZE, AES_DECRYPTION, AES_ECB, aes_mode, outBlock);

  return;
}

/*==============================================*/
/**
 * @fn          int wc_AesSetIV(Aes *aes, const byte *iv)
 * @brief       This API is used to set iv for AES and this API is shared between software and hardware.
 * @param[in]   aes, pointer to aes 
 * @param[in]   iv, Pointer to iv
 * @return      Zero - If success
 *              Non zero - If fails
 */
int wc_AesSetIV(Aes *aes, const byte *iv)
{
  if (aes == NULL)
    return -1;

  if (iv)
    memcpy(aes->reg, iv, AES_BLOCK_SIZE);
  else
    memset(aes->reg, 0, AES_BLOCK_SIZE);

  return 0;
}

/*==============================================*/
/**
 * @fn          int wc_InitAes_h(Aes *aes, void *h)
 * @brief       This API is used to set the heap hint for AES
 * @param[in]   aes, pointer to aes 
 * @param[in]   h, Pointer to h 
 * @return      Zero - If success
 *              Non zero - If fails
 */
int wc_InitAes_h(Aes *aes, void *h)
{
  if (aes == NULL)
    return -1;

  aes->heap = h;

  return 0;
}

/*==============================================*/
/**
 * @fn          static int wc_AesSetKeyLocal(Aes *aes, const byte *userKey, word32 keylen, const byte *iv, int dir)
 * @brief       This API is used to set the local key for AES
 * @param[in]   aes, pointer to aes 
 * @param[in]   userkey, Pointer to AES key
 * @param[in]   keylen, key length
 * @param[in]   iv, Pointer to iv
 * @return      Zero - If success
 */
static int wc_AesSetKeyLocal(Aes *aes, const byte *userKey, word32 keylen, const byte *iv, int dir)
{
  if (userKey) {
    memcpy(aes->key, userKey, keylen);
    aes->keylen = keylen;
  }
  if (iv) {
    memcpy(aes->reg, iv, AES_BLOCK_SIZE);
  }

  return 0;
}

/*==============================================*/
/**
 * @fn          int wc_AesSetKey(Aes *aes, const byte *userKey, word32 keylen, const byte *iv, int dir)
 * @brief       This API is used to set the key for AES
 * @param[in]   aes, pointer to aes 
 * @param[in]   userkey, Pointer to AES key
 * @param[in]   keylen, key length
 * @param[in]   iv, Pointer to iv
 * @return      Zero - If success
 *              Non zero - If fails            
 */
int wc_AesSetKey(Aes *aes, const byte *userKey, word32 keylen, const byte *iv, int dir)
{
  if (!((keylen == 16) || (keylen == 24) || (keylen == 32)))
    return -1;
  return wc_AesSetKeyLocal(aes, userKey, keylen, iv, dir);
}

/*==============================================*/
/**
 * @fn          int wc_AesCbcEncrypt(Aes *aes, uint8_t *out, const uint8_t *in, word32 sz)
 * @brief       This API is used to Cbc Encrypt key for AES
 * @param[in]   aes, pointer to aes 
 * @param[in]   in, Pointer to input
 * @param[in]   sz, size of the input
 * @param[out]  out, Pointer to output 
 * @return      Zero - If success
 *              Non zero - If fails            
 */
int wc_AesCbcEncrypt(Aes *aes, uint8_t *out, const uint8_t *in, word32 sz)
{
  uint8_t aes_mode;
  uint8_t *tmp;

  if (aes->keylen == 16) {
    aes_mode = AES_128;
  } else if (aes->keylen == 24) {
    aes_mode = AES_192;
  } else {
    aes_mode = AES_256;
  }
  tmp = (uint8_t *)malloc(sz);
  memcpy(tmp, in, sz);

  aes_setkey_hw_9116((const byte *)aes->key, aes_mode);

  aes_setIV_hw_9116((const byte *)aes->reg, aes_mode);

  aes_hw_9116(tmp, sz, AES_ENCRYPTION, AES_CBC, aes_mode, tmp);

  /* store iv for next call */
  memcpy(aes->reg, tmp + sz - AES_BLOCK_SIZE, AES_BLOCK_SIZE);

  memcpy(out, tmp, sz);
  free(tmp);

  return 0;
}

/*==============================================*/
/**
 * @fn          int wc_AesCbcDecrypt(Aes *aes, uint8_t *out, const uint8_t *in, word32 sz)
 * @brief       This API is used to Cbc Decrypt key for AES
 * @param[in]   aes, pointer to aes 
 * @param[in]   in, Pointer to input
 * @param[in]   sz, size of the input 
 * @param[out]  out, Pointer to output
 * @return      Zero - If success
 *              Non zero - If fails          
 */
int wc_AesCbcDecrypt(Aes *aes, uint8_t *out, const uint8_t *in, word32 sz)
{
  uint8_t aes_mode;

  if (aes->keylen == 16) {
    aes_mode = AES_128;
  } else if (aes->keylen == 24) {
    aes_mode = AES_192;
  } else {
    aes_mode = AES_256;
  }
  /* if input and output same will overwrite input iv */
  memcpy(aes->tmp, in + sz - AES_BLOCK_SIZE, AES_BLOCK_SIZE);

  aes_setkey_hw_9116((const byte *)aes->key, aes_mode);

  aes_setIV_hw_9116((const byte *)aes->reg, aes_mode);

  aes_hw_9116(in, sz, AES_DECRYPTION, AES_CBC, aes_mode, out);
  /* store iv for next call */
  memcpy(aes->reg, aes->tmp, AES_BLOCK_SIZE);

  return 0;
}