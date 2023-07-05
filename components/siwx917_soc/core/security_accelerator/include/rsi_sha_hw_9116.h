/*******************************************************************************
* @file  rsi_sha_hw_9116.h
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

#define AES_ENCRYPTION 1
#define AES_DECRYPTION 2

#define AES_KEY_SIZE_128 16
#define AES_KEY_SIZE_192 24
#define AES_KEY_SIZE_256 32

#define ECDH_192 1
#define ECDH_224 2
#define ECDH_256 4

#define MAX_DATA_SIZE_BYTES 1400 /*Data size*/

#define SHA_BASE_ADDRESS_9116       0x20400200
#define SHA_CTRL_REG_9116           *(volatile uint32_t *)(SHA_BASE_ADDRESS_9116 + 0x00)
#define SHA_STS_REG_9116            *(volatile uint32_t *)(SHA_BASE_ADDRESS_9116 + 0x04)
#define SHA_DIN_FIFO_DATA_9116      *(volatile uint32_t *)(SHA_BASE_ADDRESS_9116 + 0x80)
#define SHA_DIN_FIFO_DATA_9116_ADDR (volatile uint32_t *)(SHA_BASE_ADDRESS_9116 + 0x80)

#define SHA_A_L *(volatile uint32_t *)(SHA_BASE_ADDRESS_9116 + 0x08)
#define SHA_A_H *(volatile uint32_t *)(SHA_BASE_ADDRESS_9116 + 0x0C)
#define SHA_B_L *(volatile uint32_t *)(SHA_BASE_ADDRESS_9116 + 0x10)
#define SHA_B_H *(volatile uint32_t *)(SHA_BASE_ADDRESS_9116 + 0x14)
#define SHA_C_L *(volatile uint32_t *)(SHA_BASE_ADDRESS_9116 + 0x18)
#define SHA_C_H *(volatile uint32_t *)(SHA_BASE_ADDRESS_9116 + 0x1C)
#define SHA_D_L *(volatile uint32_t *)(SHA_BASE_ADDRESS_9116 + 0x20)
#define SHA_D_H *(volatile uint32_t *)(SHA_BASE_ADDRESS_9116 + 0x24)
#define SHA_E_L *(volatile uint32_t *)(SHA_BASE_ADDRESS_9116 + 0x28)
#define SHA_E_H *(volatile uint32_t *)(SHA_BASE_ADDRESS_9116 + 0x2C)
#define SHA_F_L *(volatile uint32_t *)(SHA_BASE_ADDRESS_9116 + 0x30)
#define SHA_F_H *(volatile uint32_t *)(SHA_BASE_ADDRESS_9116 + 0x34)
#define SHA_G_L *(volatile uint32_t *)(SHA_BASE_ADDRESS_9116 + 0x38)
#define SHA_G_H *(volatile uint32_t *)(SHA_BASE_ADDRESS_9116 + 0x3C)
#define SHA_H_L *(volatile uint32_t *)(SHA_BASE_ADDRESS_9116 + 0x40)
#define SHA_H_H *(volatile uint32_t *)(SHA_BASE_ADDRESS_9116 + 0x44)

#define SHA_MODE_160 0
#define SHA_MODE_224 1
#define SHA_MODE_256 2
#define SHA_MODE_384 3
#define SHA_MODE_512 4

//! Control bits
#define SHA_CTRL_ENABLE BIT(2)

//! Status bite
#define SHA_STS_FIFO_FULL BIT(7)
#define SHA_STS_BUSY      BIT(8)
#define SHA_STS_DONE      BIT(0)

#define SHA_OPERATION_AUTOMIC 0
#define SHA_OPERATION_UPDATE  1
#define SHA_OPERATION_FINAL   2

int sha_hw_9116(uint32_t *msg_data,
                uint32_t *digest,
                uint8_t sha_mode,
                uint8_t sha_operation,
                uint8_t dma_use,
                uint32_t length);