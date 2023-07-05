/*******************************************************************************
* @file  rsi_aes_hw_9116.c
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
#include "rsi_crypto.h"

/*==============================================*/
/**
 * @fn          void aes_setkey_hw_9116(const byte *userKey, uint8_t aes_mode)
 * @brief       This API is used to load the key for AES
 * @param[in]   userKey, pointer to AES key 
 * @param[in]   aes_mode, AES mode
 *              - AES_128
 *              - AES_192
 *              - AES_256
 * @return      none
 */
//crypto_non_rom_bss_t                 *crypto_non_rom_bss_ptr;
void aes_setkey_hw_9116(const byte *userKey, uint8_t aes_mode)
{
  uint8_t key_size, i;

  HW_WRITE_REG_U16(AES_CTRL_REG_ADDR, 0);

  //! Set the key expansion bit and enable AES.
  //! Set AES mode
  HW_WRITE_REG_U16(AES_CTRL_REG_ADDR, (AES_KEY_EXP_ENABLE | AES_ENABLE | (aes_mode << 9)));

  if (aes_mode == AES_128) {
    key_size = 16;
  } else if (aes_mode == AES_192) {
    key_size = 24;
  } else {
    key_size = 32;
  }

  for (i = 0; i < key_size; i += 4) {
    //! Check for FIFO is not full
    while (HW_READ_REG_U16(AES_STS_REG_ADDR) & AES_DIN_FIFO_FULL)
      ;

    HW_WRITE_REG_U32(AES_DIN_FIFO_DATA_L_ADDR, *(uint32_t *)&userKey[i]);
  }

  //! Poll for AES done after key loading
  while (!(HW_READ_REG_U16(AES_STS_REG_ADDR) & AES_DONE))
    ;
}

/*==============================================*/
/**
 * @fn          void aes_hw_9116(const byte *in, word32 sz, byte direction, byte mode, uint8_t aes_mode, byte *out)
 * @brief       This API is used to perform AES encrpytion/decryption for a given input
 * @param[in]   in, pointer to input
 * @param[in]   sz, size of the input in bytes
 * @param[in]   direction, To select encryption or decryption
 *              - AES_ENCRYPTION
 *              - AES_DECRYPTION
 * @param[in]   mode, AES CBC or ECB mode
 *              - AES_CBC  
 *              - AES_ECB
 * @param[in]   aes_mode, AES mode
 *              - AES_128
 *              - AES_192
 *              - AES_256
 * @param[out]  out, pointer to output after encryption or decryption
 * @return      none 
 */
void aes_hw_9116(const byte *in, word32 sz, byte direction, byte mode, uint8_t aes_mode, byte *out)
{
  uint32_t i = 0, j = 0, k = 0;

  uint8_t aes_dma_use = 0;

  if (direction == AES_ENCRYPTION) {
    HW_WRITE_REG_U16(AES_CTRL_REG_ADDR, (AES_ENABLE | (aes_mode << 9)));
  } else {
    HW_WRITE_REG_U16(AES_CTRL_REG_ADDR, (AES_ENC_ENABLE | AES_ENABLE | (aes_mode << 9)));
  }

  if (mode == AES_ECB) {
    //! Setting ECB mode
    HW_WRITE_REG_U16(AES_CTRL_REG_ADDR, (HW_READ_REG_U16(AES_CTRL_REG_ADDR) | AES_ECB_CBC_MODE_SELECT));
  } else if (mode == AES_CTR) {
    HW_WRITE_REG_U16(AES_CTRL_REG_ADDR, (HW_READ_REG_U16(AES_CTRL_REG_ADDR) | AES_COUNTER_MODE_ENABLE));
  }

  if (aes_dma_use == 0) {
    for (i = 0; i < sz; i += AES_BLOCK_SIZE) {
      for (k = 0; k < 16; k += 4) {
        //! Check for FIFO is not full
        while (HW_READ_REG_U16(AES_STS_REG_ADDR) & AES_DIN_FIFO_FULL)
          ;

        HW_WRITE_REG_U32(AES_DIN_FIFO_DATA_L_ADDR, *(uint32_t *)&in[k + i]);
      }

      //! Poll for AES done
      while (!(HW_READ_REG_U16(AES_STS_REG_ADDR) & AES_DONE))
        ;

      for (j = 0; j < AES_BLOCK_SIZE; j += 4) {
        //! Check for FIFO is not full
        while (HW_READ_REG_U16(AES_STS_REG_ADDR) & AES_DOUT_FIFO_EMPTY)
          ;

        //! Read data
        *(uint32_t *)&out[j + i] = HW_READ_REG_U32(AES_DOUT_FIFO_DATA_L_ADDR);
      }
    }
  } else {
//TO DO: need to be fix with m4_dma
#if 0  
    volatile dma_desc_t desc, desc_out;
		uint8_t dma_in_channel;
		uint8_t dma_out_channel;
    //lib_memset((void *)&desc, sizeof(dma_desc_t));
    
    //lib_memset((void *)&desc_out, sizeof(dma_desc_t));
     memset((void *)&desc,0,sizeof(dma_desc_t));
		 memset((void *)&desc_out,0,sizeof(dma_desc_t));

    HW_WRITE_REG_U16(AES_FIFO_THRESHOLD_ADDR, ((1<<4) | 1));

    desc.src_addr = (uint32_t)in;

    desc.dst_addr = (AES_DIN_FIFO_DATA_L_ADDR | (BIT(31)));

    desc.control  = sz | (DEV_AES_FIFO << DMA_DESC_DST_CODE_OFST) | ((DMA_BEATS_1) << (DMA_DESC_BSZ_OFST) | (DMA_DESC_EOC));

    desc_out.src_addr = (AES_DOUT_FIFO_DATA_L_ADDR | (BIT(31)));

    desc_out.dst_addr = (uint32_t)out;

    desc_out.control  = sz | (DEV_AES_FIFO << DMA_DESC_DST_CODE_OFST) | ((DMA_BEATS_1) << (DMA_DESC_BSZ_OFST) | (DMA_DESC_EOC));

    dma_in_channel = crypto_non_rom_bss_ptr->aes_in_dma_channel;
    dma_out_channel = crypto_non_rom_bss_ptr->aes_out_dma_channel;

    //! Set the base address registers for Tx descriptors
    DMA_CHn_DESC1_BASE_L(dma_in_channel) = ((uint32_t)&desc & 0xFFFF);
    DMA_CHn_DESC1_BASE_H(dma_in_channel) = ((uint32_t)&desc >> 16);

    //! Set the base address registers for Tx descriptors
    DMA_CHn_DESC1_BASE_L(dma_out_channel) = ((uint32_t)&desc_out & 0xFFFF);
    DMA_CHn_DESC1_BASE_H(dma_out_channel) = ((uint32_t)&desc_out >> 16);

    DMA_CHn_CTRL0(dma_in_channel) =  DMA_CTRL0_LINK_LIST
      | DMA_CTRL0_COMP_INT_EN
      | DMA_CTRL0_TRANS_EN;

    DMA_CHn_CTRL0(dma_out_channel) =  DMA_CTRL0_LINK_LIST
      | DMA_CTRL0_COMP_INT_EN
      | DMA_CTRL0_TRANS_EN;

    while(!(DMA_CHn_STATUS(dma_in_channel) & DMA_STAT_TRANS_DONE0));

    DMA_CHn_STATUS(dma_in_channel) = 0x4;/* clear interrupt */


    while(!(DMA_CHn_STATUS(dma_out_channel) & DMA_STAT_TRANS_DONE0));

    DMA_CHn_STATUS(dma_out_channel) = 0x4;/* clear interrupt */
#endif
  }

  HW_WRITE_REG_U16(AES_CTRL_REG_ADDR, 0);
}
/*==============================================*/
/**
 * @fn          void aes_setIV_hw_9116(const byte *iv, uint8_t aes_mode)
 * @brief       This API is used to set IV for AES in CBC mode
 * @param[in]   iv, pointer IV 
 * @param[in]   aes_mode, AES mode
 *              - AES_128
 *              - AES_192
 *              - AES_256
 * @return      none
 */
void aes_setIV_hw_9116(const byte *iv, uint8_t aes_mode)
{
  uint8_t i = 0;

  //! Keep AES in Load IV mode
  HW_WRITE_REG_U16(AES_CTRL_REG_ADDR, (AES_LOAD_IV_ENABLE | AES_ENABLE | (aes_mode << 9)));

  for (i = 0; i < 16; i += 4) {
    //! Check for FIFO is not full
    while (HW_READ_REG_U16(AES_STS_REG_ADDR) & AES_DIN_FIFO_FULL)
      ;

    HW_WRITE_REG_U32(AES_DIN_FIFO_DATA_L_ADDR, *(uint32_t *)&iv[i]);
  }

  //! Poll for AES done after IV loading
  while (!(HW_READ_REG_U16(AES_STS_REG_ADDR) & AES_DONE))
    ;
}
/*==============================================*/
/**
 * @fn          void aes_load_A0(const byte *A0)
 * @brief       This API is used to load registers for AES
 * @param[in]   A0, Pointer to A0
 * @return      none
 */
void aes_load_A0(const byte *A0)
{
  HW_WRITE_REG_U32(AES_A0_REG_0_ADDR, *(uint32_t *)&A0[0]);
  HW_WRITE_REG_U32(AES_A0_REG_1_ADDR, *(uint32_t *)&A0[4]);
  HW_WRITE_REG_U32(AES_A0_REG_2_ADDR, *(uint32_t *)&A0[8]);
  HW_WRITE_REG_U32(AES_A0_REG_3_ADDR, *(uint32_t *)&A0[12]);
}
