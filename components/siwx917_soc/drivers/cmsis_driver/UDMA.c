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

#ifdef ADC_MULTICHANNEL_ENABLE
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
RSI_UDMA_HANDLE_T udmaHandle0, udmaHandle1;
uint32_t dma_rom_buff0[30], dma_rom_buff1[30];

#if ((UDMA0_SRAM_BASE & (~0x3FF)) != UDMA0_SRAM_BASE)
#error "Invalid UDMA0 sram base address"
#endif

#if ((UDMA1_SRAM_BASE & (~0x3FF)) != UDMA1_SRAM_BASE)
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
RSI_UDMA_DESC_T UDMA0_Table[CONTROL_STRUCT0] __attribute__((at(UDMA0_SRAM_BASE)));
RSI_UDMA_DESC_T UDMA1_Table[CONTROL_STRUCT1] __attribute__((at(UDMA1_SRAM_BASE)));
#endif /* defined (__CC_ARM) */

#if defined(__GNUC__)
RSI_UDMA_DESC_T __attribute__((section(".udma_addr0"))) UDMA0_Table[CONTROL_STRUCT0];
RSI_UDMA_DESC_T __attribute__((section(".udma_addr1"))) UDMA1_Table[CONTROL_STRUCT1];
#endif /* defined (__GNUC__) */

UDMA_Channel_Info udma0_chnl_info[32] = { 0U };
UDMA_Channel_Info udma1_chnl_info[12] = { 0U };

#ifdef RTE_UDMA0
/* I2C0 Resources */
UDMA_RESOURCES UDMA0_Resources = {
  (UDMA0_Type *)UDMA0,
  UDMA0_IRQn,
  UDMA0_Table // SRAM base address
};
#endif /* RTE_UDMA0 */

#ifdef RTE_UDMA1

/* I2C1 Resources */
UDMA_RESOURCES UDMA1_Resources = {
  (UDMA0_Type *)UDMA1,
  UDMA1_IRQn,
  UDMA1_Table // SRAM base address
};
#endif /* RTE_UDMA1 */

#ifndef SL_SI91X_DMA
void IRQ033_Handler(void)
{
#if defined(A11_ROM) && defined(ROMDRIVER_PRESENT)
  ROMAPI_UDMA_WRAPPER_API->uDMAx_IRQHandler(&UDMA0_Resources, UDMA0_Table, udma0_chnl_info);
#else
  uDMAx_IRQHandler(&UDMA0_Resources, UDMA0_Table, udma0_chnl_info);
#endif
}

void IRQ010_Handler(void)
{
#if defined(DAC_FIFO_MODE_EN) || defined(ADC_MULTICHANNEL_ENABLE)
  volatile uint32_t intr = 0;
  intr                   = UDMA1_Resources.reg->UDMA_DONE_STATUS_REG;

  if ((intr & BIT(DAC_UDMA_CHANNEL)) || (intr & BIT(ADC_UDMA_CHANNEL))) {
    if (intr & BIT(DAC_UDMA_CHANNEL)) {
#ifdef DAC_FIFO_MODE_EN
      RSI_UDMA_InterruptClear(udmaHandle1, DAC_UDMA_CHANNEL);
      dac_callback_fun.callback_event(DAC_UDMA_PING_PONG_CONFIG);
#endif
    } else {
#ifdef ADC_MULTICHANNEL_ENABLE
      RSI_UDMA_InterruptClear(udmaHandle1, ADC_UDMA_CHANNEL);
      adc_commn_config.call_back_event(ADC_CHNL0_INTR, EXTERNAL_DMA_RECONFIG);
#endif
    }
  } else {
#if defined(A11_ROM) && defined(ROMDRIVER_PRESENT)
    ROMAPI_UDMA_WRAPPER_API->uDMAx_IRQHandler(&UDMA1_Resources, UDMA1_Table, udma1_chnl_info);
#else
    uDMAx_IRQHandler(&UDMA1_Resources, UDMA1_Table, udma1_chnl_info);
#endif
  }
#else
#if defined(A11_ROM) && defined(ROMDRIVER_PRESENT)
  ROMAPI_UDMA_WRAPPER_API->uDMAx_IRQHandler(&UDMA1_Resources, UDMA1_Table, udma1_chnl_info);
#else
  uDMAx_IRQHandler(&UDMA1_Resources, UDMA1_Table, udma1_chnl_info);
#endif
#endif
}
#endif /* SL_SI91X_DMA */
