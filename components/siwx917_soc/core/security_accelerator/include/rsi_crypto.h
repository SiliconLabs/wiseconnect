/*******************************************************************************
* @file  rsi_crypto.h
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
#ifndef RSI_SHA_H
#define RSI_SHA_H
/******************************************************
 * *                      Macros
 * ******************************************************/

#include "stdint.h"
#include "dma.h"
#include "dh.h"

typedef struct rsi_sha_req_s {
  uint8_t algorithm_type;
  uint8_t algorithm_sub_type;
  uint16_t total_msg_length;
} rsi_sha_req_t;

typedef struct rsi_exp_req_s {
  uint32_t exponent_length;
  uint8_t prime[512];
  uint8_t base[512];
  uint8_t exponent[512];
} rsi_exp_req_t;

////sha
typedef struct crypto_non_rom_bss_s {
  struct wsc_cb_s *wsc;

  uint8_t firmware_mode;

  uint8_t sha_dma_use;

  uint8_t aes_dma_use;

  uint8_t sha_dma_channel;
  uint8_t aes_in_dma_channel;
  uint8_t aes_out_dma_channel;

  //! Funciton pointers

} crypto_non_rom_bss_t;

typedef const struct crypto_rom_bss_s {
  int (*calc_dh_key)(uint8_t *prime,
                     uint32_t prime_len,
                     NN_DIGIT *base,
                     uint32_t base_len,
                     uint8_t *x_orig,
                     uint32_t x_length,
                     NN_DIGIT *yy,
                     uint8_t interrupt);
  int (*calc_dh_key_4096)(uint8_t *prime,
                          uint32_t prime_len,
                          NN_DIGIT *base,
                          uint32_t base_len,
                          uint8_t *x_orig,
                          uint32_t x_length,
                          NN_DIGIT *yy,
                          uint8_t interrupt);
  int (*crypto_mod_exp_hw)(const NN_DIGIT *base,
                           uint32_t base_len,
                           const NN_DIGIT *power,
                           uint32_t power_len,
                           const NN_DIGIT *modulus,
                           uint32_t modulus_len,
                           NN_DIGIT *result,
                           uint32_t *result_len);
  uint8_t (*verify_dh_hw_support)(uint32_t base_len, uint32_t modulus_len, uint32_t power_len);
} crypto_rom_bss_t;

extern crypto_rom_bss_t *crypto_rom_bss_ptr;

extern crypto_non_rom_bss_t *crypto_non_rom_bss_ptr;

typedef struct dma_desc_s {
  //! source address
  uint32_t src_addr;
  //! destination address
  uint32_t dst_addr;
  //! control information
  uint32_t control;
  //! address of next descriptor
  uint32_t nxt_desc;

#define DMA_DESC_LEN_MSK        0x00001FFF
#define DMA_DESC_INT_EN         BIT(13)
#define DMA_DESC_EOC            BIT(14)
#define DMA_DESC_BUS_SZ16       BIT(15)
#define DMA_DESC_BSZ_OFST       16
#define DMA_DESC_MAX_TRANS_OFST 20
#define DMA_DESC_DST_CODE_OFST  26
#define DMA_DESC_SRC_CODE_OFST  29

/* Device codes */
#ifdef SDIO_MF
#define DEV_SDIO 2
#else
#define DEV_SDIO 1
#endif
#define DEV_SPI               1
#define DEV_I2C               2
#define DEV_I2S1              3
#define DEV_I2S2              4
#define DEV_SPI_PERI          5
#define DEV_UART1             6
#define DEV_UART2             7
#define DEV_RF_SPI            7
#define DEV_TA_MEMORY         0
#define DEV_IPSEC             0
#define DEV_ZB_BT_LMAC_FIFO   3
#define DEV_AES_FIFO          0
#define DEV_RFPROG1_DST       7
#define DEV_WLAN_BBP_PROG_DST 7
#define DEV_M4_MEMORY         0
#define DEV_TYP_DST(dev_code) ((uint32_t)dev_code << DMA_DESC_DST_CODE_OFST)
#define DEV_TYP_SRC(dev_code) ((uint32_t)dev_code << DMA_DESC_SRC_CODE_OFST)
} dma_desc_t;

//////////

#define FIRST_CHUNK  BIT(0)
#define MIDDLE_CHUNK BIT(1)
#define LAST_CHUNK   BIT(2)

#define SHA_1   1
#define SHA_256 2
#define SHA_384 3
#define SHA_512 4

#define SHA_1_DIGEST_LEN   20
#define SHA_256_DIGEST_LEN 32
#define SHA_384_DIGEST_LEN 48
#define SHA_512_DIGEST_LEN 64

#define CBC_MODE 1
#define ECB_MODE 2
#define CTR_MODE 3

#define AES_KEY_SIZE_128 16
#define AES_KEY_SIZE_192 24
#define AES_KEY_SIZE_256 32

#define ECDH_192 1
#define ECDH_224 2
#define ECDH_256 4

#define MAX_DATA_SIZE_BYTES 1400 /*Data size*/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
int32_t rsi_sha(uint8_t sha_mode, uint8_t *msg, uint16_t msg_length, uint8_t *digest);
int32_t rsi_hmac_sha(uint8_t hmac_sha_mode,
                     uint8_t *msg,
                     uint16_t msg_length,
                     uint8_t *key,
                     uint16_t key_length,
                     uint8_t *digest);
int32_t rsi_aes(uint8_t aes_mode,
                uint8_t enc_dec,
                uint8_t *msg,
                uint16_t msg_length,
                uint8_t *key,
                uint16_t key_length,
                uint8_t *iv,
                uint8_t *output);
int32_t rsi_exponentiation(uint8_t *prime,
                           uint32_t prime_length,
                           uint8_t *base,
                           uint32_t base_length,
                           uint8_t *exponent,
                           uint32_t exponent_length,
                           uint8_t *exp_result);
int32_t rsi_ecdh_point_double(uint8_t ecdh_mode,
                              uint8_t *sx,
                              uint8_t *sy,
                              uint8_t *sz,
                              uint8_t *rx,
                              uint8_t *ry,
                              uint8_t *rz);
int32_t rsi_ecdh_point_affine(uint8_t ecdh_mode,
                              uint8_t *sx,
                              uint8_t *sy,
                              uint8_t *sz,
                              uint8_t *rx,
                              uint8_t *ry,
                              uint8_t *rz);
int32_t rsi_ecdh_point_subtraction(uint8_t ecdh_mode,
                                   uint8_t *sx,
                                   uint8_t *sy,
                                   uint8_t *sz,
                                   uint8_t *tx,
                                   uint8_t *ty,
                                   uint8_t *tz,
                                   uint8_t *rx,
                                   uint8_t *ry,
                                   uint8_t *rz);
int32_t rsi_ecdh_point_addition(uint8_t ecdh_mode,
                                uint8_t *sx,
                                uint8_t *sy,
                                uint8_t *sz,
                                uint8_t *tx,
                                uint8_t *ty,
                                uint8_t *tz,
                                uint8_t *rx,
                                uint8_t *ry,
                                uint8_t *rz);
int32_t rsi_ecdh_point_multiplication(uint8_t ecdh_mode,
                                      uint8_t *d,
                                      uint8_t *sx,
                                      uint8_t *sy,
                                      uint8_t *sz,
                                      uint8_t *rx,
                                      uint8_t *ry,
                                      uint8_t *rz);
#endif
