/*******************************************************************************
* @file  rsi_crypto_hardware_AES.c
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

//Include Files

#include "rsi_aes_hw_9116.h"
#include "rsi_aes.h"

/*==============================================*/
/**
 * @fn          int16_t rsi_crypto_aes(rsi_aes_req_t *crypto, uint16_t *output_length, uint8_t *output, uint8_t *crypto_data)
 * @brief       This API is used to encrypt and decrypt the key for AES
 * @param[in]   crypto, Pointer to crypto
 * @param[in]   crypto_data, Pointer to crypto data
 * @param[out]  output_length, Pointer to output length
 * @param[out]  output, Pointer to output
 * @return      Zero - if success
 *              Non zero - if fails
 */
int16_t rsi_crypto_aes(rsi_aes_req_t *crypto, uint16_t *output_length, uint8_t *output, uint8_t *crypto_data)
{
  uint16_t key_length  = 0, i;
  uint16_t text_length = 0;
  uint16_t ret         = 0;
  uint8_t *key         = NULL;
  uint8_t iv[17];
  uint8_t *text = NULL;
  uint8_t aes_mode;

  Aes aes;

  memset(&aes, 0, sizeof(Aes));
  //! Extract key length
  key_length = crypto->key_length;

  //! Extract text length
  text_length = crypto->total_msg_length;
  //! Extract key value
  key = crypto->key;

  //! Extract text value
  text = crypto_data;

  if (crypto->algorithm_sub_type == RSI_AES_CTR) {
    if (key_length == 16) {
      aes_mode = AES_128;
    } else if (key_length == 24) {
      aes_mode = AES_192;
    } else {
      aes_mode = AES_256;
    }

    aes_setkey_hw_9116((const byte *)key, aes_mode);

    aes_load_A0((const byte *)crypto->IV);

    if (crypto->encrypt_decryption == RSI_AES_ENCRYPT) {
      aes_hw_9116(text, text_length, AES_ENCRYPTION, AES_CTR, aes_mode, output);
    } else if (crypto->encrypt_decryption == RSI_AES_DECRYPT) {
      aes_hw_9116(text, text_length, AES_DECRYPTION, AES_CTR, aes_mode, output);
    } else {
      return -1;
    }
    return 0;

  } else {
    if (crypto->algorithm_sub_type == RSI_AES_CBC) {

      memcpy(iv, crypto->IV, 16);
      iv[16] = '\0';
    }

    if (crypto->encrypt_decryption == RSI_AES_ENCRYPT) {
      if (crypto->algorithm_sub_type == RSI_AES_CBC) {
        ret = AesSetKey(&aes, key, key_length, iv, 0);
      } else {
        ret = AesSetKey(&aes, key, key_length, NULL, 0);
      }
    } else if (crypto->encrypt_decryption == RSI_AES_DECRYPT) {
      if (crypto->algorithm_sub_type == RSI_AES_CBC) {
        ret = AesSetKey(&aes, key, key_length, iv, 1);
      } else {
        ret = AesSetKey(&aes, key, key_length, NULL, 1);
      }
    } else {
    }

    if (ret == 0) {
      if (crypto->algorithm_sub_type == RSI_AES_CBC) {
        if (crypto->encrypt_decryption == RSI_AES_ENCRYPT) {
          ret = AesCbcEncrypt(&aes, output, text, text_length);

        } else if (crypto->encrypt_decryption == RSI_AES_DECRYPT) {
          ret = AesCbcDecrypt(&aes, output, text, text_length);
        }
      } else if (crypto->algorithm_sub_type == RSI_AES_ECB) {
        if (crypto->encrypt_decryption == RSI_AES_ENCRYPT) {
          for (i = 0; i < text_length; i += 16) {
            AesEncrypt(&aes, text + i, output + i);
          }
        } else if (crypto->encrypt_decryption == RSI_AES_DECRYPT) {
          for (i = 0; i < text_length; i += 16) {
            AesDecrypt(&aes, text + i, output + i);
          }
        }
      }
    } else {
      return -1;
    }
    return 0;
  }
}
