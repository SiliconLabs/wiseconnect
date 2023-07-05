/*******************************************************************************
* @file  rsi_aes_hw_9116.h
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

typedef unsigned char byte;
typedef unsigned short word16;
typedef unsigned int word32;
typedef unsigned long long word64;

#define NULL 0

#define RSI_AES_CBC     1
#define RSI_AES_ECB     2
#define RSI_AES_CTR     3
#define RSI_AES_ENCRYPT 1
#define RSI_AES_DECRYPT 2

#define AES_128 0
#define AES_192 1
#define AES_256 2

#define HW_WRITE_REG_U16(addr, val) *(volatile uint16_t *)(addr) = (val)
#define HW_READ_REG_U16(addr)       *(volatile uint16_t *)(addr)
#define HW_WRITE_REG_U32(addr, val) *(volatile uint32_t *)(addr) = (val)
#define HW_READ_REG_U32(addr)       *(volatile uint32_t *)(addr)
#ifndef BIT
#define BIT(x) ((uint32_t)1 << (x))
#endif

#define AES_REG_BASE_ADDR 0x20400000

#define AES_CTRL_REG_ADDR         (AES_REG_BASE_ADDR + 0x24)
#define AES_STS_REG_ADDR          (AES_REG_BASE_ADDR + 0x28)
#define AES_A0_REG_0_ADDR         (AES_REG_BASE_ADDR + 0x2C)
#define AES_A0_REG_1_ADDR         (AES_REG_BASE_ADDR + 0x30)
#define AES_A0_REG_2_ADDR         (AES_REG_BASE_ADDR + 0x34)
#define AES_A0_REG_3_ADDR         (AES_REG_BASE_ADDR + 0x38)
#define AES_FIFO_THRESHOLD_ADDR   (AES_REG_BASE_ADDR + 0x20) // [3:0]= AEMPTY; [7:4] = AFULL
#define AES_DIN_FIFO_DATA_L_ADDR  (AES_REG_BASE_ADDR + 0x40)
#define AES_DIN_FIFO_DATA_H_ADDR  (AES_REG_BASE_ADDR + 0x42)
#define AES_DOUT_FIFO_DATA_L_ADDR (AES_REG_BASE_ADDR + 0x44)
#define AES_DOUT_FIFO_DATA_H_ADDR (AES_REG_BASE_ADDR + 0x46)

/* Defines related to AES_CTRL_REG_ADDR */
#define AES_ENABLE                  BIT(0)
#define AES_ECB_CBC_MODE_SELECT     BIT(1) /* Set -> ECB; Reset -> CBC */
#define AES_ENC_ENABLE              BIT(2) /* Set -> Encryption; Reset -> Decryption */
#define AES_KEY_EXP_ENABLE          BIT(3)
#define AES_LOAD_IV_ENABLE          BIT(4)
#define AES_CHK_DOUT_FIFO_EMPTY     BIT(5)
#define AES_COUNTER_MODE_ENABLE     BIT(6)
#define AES_MSG_DIGEST_MODE         BIT(7)
#define AES_DISABLE_CYCLE_REDUCTION BIT(8)

/* Defines related to AES_STS_REG_ADDR */
#define AES_DONE            BIT(0)
#define AES_DIN_FIFO_FULL   BIT(2)
#define AES_DIN_FIFO_EMPTY  BIT(3)
#define AES_DOUT_FIFO_FULL  BIT(4)
#define AES_DOUT_FIFO_EMPTY BIT(5)

#define AES_ENCRYPTION 1
#define AES_DECRYPTION 2
#define AES_CBC        0
#define AES_ECB        1
#define AES_CTR        2
#define AES_BLOCK_SIZE 16

//! Proto types
void aes_setkey_hw_9116(const byte *userKey, uint8_t aes_mode);
void aes_hw_9116(const byte *in, word32 sz, byte direction, byte mode, uint8_t aes_mode, byte *out);
void aes_setIV_hw_9116(const byte *iv, uint8_t aes_mode);
void aes_load_A0(const byte *A0);
