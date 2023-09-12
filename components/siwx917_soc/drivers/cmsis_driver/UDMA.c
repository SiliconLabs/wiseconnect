/*******************************************************************************
* @file  UDMA.c
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

#include "rsi_ccp_user_config.h"

#include "UDMA.h"
#include "rsi_packing.h"

#ifdef ADC_MULTICHANNEL_WITH_EXT_DMA 
#include "rsi_adc.h"
extern adc_commn_config_t adc_commn_config;
#endif

#ifdef DAC_FIFO_MODE_EN
#include "rsi_dac.h"
extern dac_config_t dac_callback_fun;
#endif	

#if defined(A11_ROM)
#include "rsi_rom_table_si91x.h"
#endif

#include "rsi_udma_wrapper.h"

//UDMA Defines////
RSI_UDMA_HANDLE_T udmaHandle0,udmaHandle1;
uint32_t dma_rom_buff0[30], dma_rom_buff1[30];


#if  ((UDMA0_SRAM_BASE & (~0x3FF)) != UDMA0_SRAM_BASE)
#error "Invalid UDMA0 sram base address"
#endif

#if  ((UDMA1_SRAM_BASE & (~0x3FF)) != UDMA1_SRAM_BASE)
#error "Invalid UDMA1 sram base address"
#endif

/* IAR support */
#if defined(__ICCARM__)
#pragma location = UDMA0_SRAM_BASE
RSI_UDMA_DESC_T UDMA0_Table[CONTROL_STRUCT0];
#pragma location = UDMA1_SRAM_BASE
RSI_UDMA_DESC_T UDMA1_Table[CONTROL_STRUCT1];
#endif

/* DMA descriptors must be aligned to 16 bytes */
#if defined(__CC_ARM)
RSI_UDMA_DESC_T UDMA0_Table[CONTROL_STRUCT0] __attribute__ ((at(UDMA0_SRAM_BASE)));
RSI_UDMA_DESC_T UDMA1_Table[CONTROL_STRUCT1] __attribute__ ((at(UDMA1_SRAM_BASE)));
#endif /* defined (__CC_ARM) */

#if defined( __GNUC__ )
RSI_UDMA_DESC_T __attribute__ ((section(".udma_addr0"))) UDMA0_Table[CONTROL_STRUCT0];
RSI_UDMA_DESC_T __attribute__ ((section(".udma_addr1"))) UDMA1_Table[CONTROL_STRUCT1];
#endif /* defined (__GNUC__) */

UDMA_Channel_Info udma0_chnl_info[32] = { 0U } ;
UDMA_Channel_Info udma1_chnl_info[12] = { 0U } ;

#ifdef RTE_UDMA0
/* I2C0 Resources */
UDMA_RESOURCES UDMA0_Resources = {
		(UDMA0_Type *)UDMA0,
		UDMA0_IRQn,
		UDMA0_Table          // SRAM base address
};
#endif /* RTE_UDMA0 */

#ifdef RTE_UDMA1

/* I2C1 Resources */
UDMA_RESOURCES UDMA1_Resources = {
		(UDMA0_Type *)UDMA1,
		UDMA1_IRQn,
		UDMA1_Table          // SRAM base address
};
#endif /* RTE_UDMA1 */

/*==============================================*/
/**
 * @fn          void uDMAx_IRQHandler(UDMA_RESOURCES *udma, RSI_UDMA_DESC_T *UDMA_Table, UDMA_Channel_Info *chnl_info)
 * @brief       DMA interrupt handler
 * @return      none
 */
void uDMAx_IRQHandler(UDMA_RESOURCES *udma, RSI_UDMA_DESC_T *UDMA_Table, UDMA_Channel_Info *chnl_info)
{
 volatile uint32_t ch = 0, size = 0;
    volatile uint32_t intr = 0, src_inc = 0, dst_inc = 0, dma_len = 0;
    for (ch = 0; ch < UDMA_NUMBER_OF_CHANNELS; ch++) {
      intr = udma->reg->UDMA_DONE_STATUS_REG;
      if (intr & (1U << ch)) {
        // Clear interrupt flag
        udma->reg->UDMA_DONE_STATUS_REG = (1U << ch);
        if ((chnl_info[ch].Cnt != chnl_info[ch].Size) || (chnl_info[ch].Cnt != chnl_info[ch].Size)) {
          if (udma->reg == UDMA0) {
            // Data waiting to transfer
            size = chnl_info[ch].Size - (chnl_info[ch].Cnt);
            // Max DMA transfer size = 4k
            if (size >= DESC_MAX_LEN) {
              size    = DESC_MAX_LEN;
              dma_len = 0x3FF;
              chnl_info[ch].Cnt += size;
            }
            if ((size > 0) && (size < DESC_MAX_LEN)) {
              dma_len = size - 1;
              chnl_info[ch].Cnt += size;
            }
          }
          if (udma->reg == UDMA1) {
            // Data waiting to transfer
            size = chnl_info[ch].Size - (chnl_info[ch].Cnt);

            // Max DMA transfer size = 4k
            if (size >= DESC_MAX_LEN) {
              size    = DESC_MAX_LEN;
              dma_len = 0x3FF;
              chnl_info[ch].Cnt += size;
            }
            if ((size > 0) && (size < DESC_MAX_LEN)) {
              dma_len = size - 1;
              chnl_info[ch].Cnt += size;
            }
          }
          // Source Address Increment
          if (udma->reg == UDMA0) {
            src_inc = UDMA_Table[ch].vsUDMAChaConfigData1.srcInc;
          }
          if (udma->reg == UDMA1) {
            src_inc = UDMA_Table[ch].vsUDMAChaConfigData1.srcInc;
          }
          if (src_inc != UDMA_SRC_INC_NONE) {
            if (udma->reg == UDMA0) {
              UDMA_Table[ch].pSrcEndAddr = (void *)((uint32_t)UDMA_Table[ch].pSrcEndAddr + (size << src_inc));
              UDMA_Table[ch].vsUDMAChaConfigData1.totalNumOfDMATrans = (unsigned int)(dma_len & 0x03FF);
              UDMA_Table[ch].vsUDMAChaConfigData1.transferType       = UDMA_MODE_BASIC;
            }
            if (udma->reg == UDMA1) {
              UDMA_Table[ch].pSrcEndAddr = (void *)((uint32_t)UDMA_Table[ch].pSrcEndAddr + (size << src_inc));
              UDMA_Table[ch].vsUDMAChaConfigData1.totalNumOfDMATrans = (unsigned int)(dma_len & 0x03FF);
              UDMA_Table[ch].vsUDMAChaConfigData1.transferType       = UDMA_MODE_BASIC;
            }
          } else {
            if (udma->reg == UDMA0) {
              UDMA_Table[ch].pSrcEndAddr                             = (void *)((uint32_t)UDMA_Table[ch].pSrcEndAddr);
              UDMA_Table[ch].vsUDMAChaConfigData1.totalNumOfDMATrans = (unsigned int)(dma_len & 0x03FF);
              UDMA_Table[ch].vsUDMAChaConfigData1.transferType       = UDMA_MODE_BASIC;
            }
            if (udma->reg == UDMA1) {
              UDMA_Table[ch].pSrcEndAddr                             = (void *)((uint32_t)UDMA_Table[ch].pSrcEndAddr);
              UDMA_Table[ch].vsUDMAChaConfigData1.totalNumOfDMATrans = (unsigned int)(dma_len & 0x03FF);
              UDMA_Table[ch].vsUDMAChaConfigData1.transferType       = UDMA_MODE_BASIC;
            }
          }
          // Destination Address Increment
          if (udma->reg == UDMA0) {
            dst_inc = UDMA_Table[ch].vsUDMAChaConfigData1.dstInc;
          }
          if (udma->reg == UDMA1) {
            dst_inc = UDMA_Table[ch].vsUDMAChaConfigData1.dstInc;
          }
          if (dst_inc != UDMA_DST_INC_NONE) {
            if (udma->reg == UDMA0) {
              UDMA_Table[ch].pDstEndAddr = (void *)((uint32_t)UDMA_Table[ch].pDstEndAddr + (size << dst_inc));
              UDMA_Table[ch].vsUDMAChaConfigData1.totalNumOfDMATrans = (unsigned int)(dma_len & 0x03FF);
              UDMA_Table[ch].vsUDMAChaConfigData1.transferType       = UDMA_MODE_BASIC;
            }
            if (udma->reg == UDMA1) {
              UDMA_Table[ch].pDstEndAddr = (void *)((uint32_t)UDMA_Table[ch].pDstEndAddr + (size << dst_inc));
              UDMA_Table[ch].vsUDMAChaConfigData1.totalNumOfDMATrans = (unsigned int)(dma_len & 0x03FF);
              UDMA_Table[ch].vsUDMAChaConfigData1.transferType       = UDMA_MODE_BASIC;
            }
          } else {
            if (udma->reg == UDMA0) {
              UDMA_Table[ch].pDstEndAddr                             = (void *)((uint32_t)UDMA_Table[ch].pDstEndAddr);
              UDMA_Table[ch].vsUDMAChaConfigData1.totalNumOfDMATrans = (unsigned int)(dma_len & 0x03FF);
              UDMA_Table[ch].vsUDMAChaConfigData1.transferType       = UDMA_MODE_BASIC;
            }
            if (udma->reg == UDMA1) {
              UDMA_Table[ch].pDstEndAddr                             = (void *)((uint32_t)UDMA_Table[ch].pDstEndAddr);
              UDMA_Table[ch].vsUDMAChaConfigData1.totalNumOfDMATrans = (unsigned int)(dma_len & 0x03FF);
              UDMA_Table[ch].vsUDMAChaConfigData1.transferType       = UDMA_MODE_BASIC;
            }
          }
          // Enable DMA Channel
          udma->reg->CHNL_ENABLE_SET = (1U << ch);
      #ifdef SOFTWARE_TRIGGER
        if(!(udma->reg->UDMA_DONE_STATUS_REG && (1U << ch)))
            {
              udma->reg->CHNL_SW_REQUEST |=(1U << ch);
            }
      #endif
        } else {
          // All Data has been transferred
          // Clear Channel active flag
          //Clear_Channel_active_flag (ch);
          // Signal Event
          if (udma->reg == UDMA0) {
            if (chnl_info[ch].cb_event) {
              chnl_info[ch].cb_event(UDMA_EVENT_XFER_DONE, ch);
            }
          }
          if (udma->reg == UDMA1) {
            if (chnl_info[ch].cb_event) {
              chnl_info[ch].cb_event(UDMA_EVENT_XFER_DONE, ch);
            }
          }
        }
      } else {
        // DMA error interrupt
        if (udma->reg->ERR_CLR & (1U << ch)) {
          udma->reg->ERR_CLR_b.ERR_CLR = 0x1;
          // Clear Channel active flag
          //Clear_Channel_active_flag (ch);
          // Clear interrupt flag
          udma->reg->UDMA_DONE_STATUS_REG = (1U << ch);
          // Signal Event
          if (chnl_info[ch].cb_event) {
            chnl_info[ch].cb_event(UDMA_EVENT_ERROR, ch);
          }
          if (chnl_info[ch].cb_event) {
            chnl_info[ch].cb_event(UDMA_EVENT_ERROR, ch);
          }
        }
      }
    }
  }

#if (SL_SI91X_DMA == 0)
void IRQ033_Handler(void)
{
#ifdef SOFTWARE_TRIGGER
     uDMAx_IRQHandler (&UDMA0_Resources,UDMA0_Table,udma0_chnl_info);
#else
 #if defined(A11_ROM) && defined(ROMDRIVER_PRESENT)
 ROMAPI_UDMA_WRAPPER_API->uDMAx_IRQHandler (&UDMA0_Resources,UDMA0_Table,udma0_chnl_info);
 #else
 uDMAx_IRQHandler (&UDMA0_Resources,UDMA0_Table,udma0_chnl_info);
 #endif
#endif
}

void IRQ010_Handler (void) 
{
#if defined(DAC_FIFO_MODE_EN) || defined(ADC_MULTICHANNEL_WITH_EXT_DMA) 
	volatile uint32_t intr = 0;
	intr = UDMA1_Resources.reg->UDMA_DONE_STATUS_REG;	 
		
	if((intr & BIT(DAC_UDMA_CHANNEL)) || (intr & BIT(ADC_UDMA_CHANNEL)))
	{
		if(intr & BIT(DAC_UDMA_CHANNEL))
		{
#ifdef DAC_FIFO_MODE_EN 			
			RSI_UDMA_InterruptClear(udmaHandle1, DAC_UDMA_CHANNEL);	
			dac_callback_fun.callback_event(DAC_UDMA_PING_PONG_CONFIG);
#endif			
		}
		else
		{
#ifdef ADC_MULTICHANNEL_WITH_EXT_DMA 				
			RSI_UDMA_InterruptClear(udmaHandle1, ADC_UDMA_CHANNEL);	
			adc_commn_config.call_back_event(ADC_CHNL0_INTR , EXTERNAL_DMA_RECONFIG);
#endif			
		}
	}
	else
	{
#ifdef SOFTWARE_TRIGGER
          uDMAx_IRQHandler (&UDMA1_Resources,UDMA1_Table,udma1_chnl_info);
#else
    #if defined(A11_ROM) && defined(ROMDRIVER_PRESENT)
      ROMAPI_UDMA_WRAPPER_API->uDMAx_IRQHandler (&UDMA1_Resources,UDMA1_Table,udma1_chnl_info);
    #else
      uDMAx_IRQHandler (&UDMA1_Resources,UDMA1_Table,udma1_chnl_info);
    #endif
#endif	
		
	}		
#else
#ifdef SOFTWARE_TRIGGER
      uDMAx_IRQHandler (&UDMA1_Resources,UDMA1_Table,udma1_chnl_info);
#else
  #if defined(A11_ROM) && defined(ROMDRIVER_PRESENT)
  ROMAPI_UDMA_WRAPPER_API->uDMAx_IRQHandler (&UDMA1_Resources,UDMA1_Table,udma1_chnl_info);
  #else
  uDMAx_IRQHandler (&UDMA1_Resources,UDMA1_Table,udma1_chnl_info);
  #endif
#endif	
	
#endif			
}
#endif // SL_SI91X_USART_DMA
