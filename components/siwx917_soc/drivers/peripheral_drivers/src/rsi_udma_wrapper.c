/*******************************************************************************
* @file  rsi_udma_wrapper.c
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
// Include Files

#include "rsi_ccp_user_config.h"
#if !defined(A11_ROM) || !defined(ROMDRIVER_PRESENT)
#include "rsi_rom_clks.h"
#include "rsi_rom_ulpss_clk.h"
#include "rsi_rom_udma.h"
#include "UDMA.h"

#ifdef __cplusplus
extern "C" {
#endif
/** @addtogroup SOC18
* @{
*/
/*==============================================*/
/**
 * @fn          RSI_UDMA_HANDLE_T uDMAx_Initialize(UDMA_RESOURCES *udma,
 *                                  RSI_UDMA_DESC_T *UDMA_Table,
 *                                  RSI_UDMA_HANDLE_T udmaHandle,
 *                                  uint32_t *mem)
 * @brief       This API is used to Initialize UDMA peripheral
 * @param[in]   udma       : Pointer to UDMA resources
 * @param[in]   UDMA_Table : Pointer to UDMA descriptor
 * @param[in]   udmaHandle : Pointer to UDMA Handle
 * @param[in]   mem        : Pointer to memory buffer
 * @return      Zero - if success
 *              Non zero - if fails
 */
RSI_UDMA_HANDLE_T uDMAx_Initialize(UDMA_RESOURCES *udma,
                                   RSI_UDMA_DESC_T *UDMA_Table,
                                   RSI_UDMA_HANDLE_T udmaHandle,
                                   uint32_t *mem)
{
  RSI_UDMA_INIT_T udmaInit0 = { 0 }, udmaInit1 = { 0 };

  if (udma->reg == UDMA0) {
#if defined(CHIP_917)
    RSI_PS_M4ssPeriPowerUp(M4SS_PWRGATE_ULP_EFUSE_PERI);
#else
    RSI_PS_M4ssPeriPowerUp(M4SS_PWRGATE_ULP_PERI1);
#endif
    RSI_CLK_PeripheralClkEnable(M4CLK, UDMA_CLK, ENABLE_STATIC_CLK);
  }
  if (udma->reg == UDMA1) {
    RSI_PS_UlpssPeriPowerUp(ULPSS_PWRGATE_ULP_UDMA);
    RSI_ULPSS_PeripheralEnable(ULPCLK, ULP_UDMA_CLK, ENABLE_STATIC_CLK);
  }
  if (udma->reg == UDMA0) {
    udmaInit0.base      = (uint32_t)UDMA0;
    udmaInit0.sramBase  = (uint32_t)UDMA_Table;
    udmaInit0.pUserData = (void *)NULL;
    udmaHandle          = RSI_UDMA_Init(mem, &udmaInit0); //doubt in mem
    if (udmaHandle == NULL) {
      return 0;
    }
  }
  if (udma->reg == UDMA1) {
    udmaInit1.base = (uint32_t)UDMA1;

    if (M4_ULP_SLP_STATUS_REG & ULP_MODE_SWITCHED_NPSS) {
      if (((uint32_t)UDMA_Table >= ULP_SRAM_START_ADDR) && ((uint32_t)UDMA_Table <= ULP_SRAM_END_ADDR)) {
        udmaInit1.sramBase = (uint32_t)UDMA_Table;
      } else {
        return 0;
      }
    } else {
      udmaInit1.sramBase = (uint32_t)UDMA_Table;
    }
    udmaInit1.pUserData = (void *)NULL;
    udmaHandle          = RSI_UDMA_Init(mem, &udmaInit1);
    if (udmaHandle == NULL) {
      return 0;
    }
  }
  // Clear all DMA interrupt flags
  NVIC_ClearPendingIRQ(udma->udma_irq_num);
  NVIC_EnableIRQ(udma->udma_irq_num);

  return udmaHandle;
}

/*==============================================*/
/**
 * @fn          int32_t uDMAx_Uninitialize(UDMA_RESOURCES *udma)
 * @brief       This API is used to De-initialize UDMA peripheral
 * @param[in]   udma : Pointer to UDMA resources
 * @return      Zero - if success
 *              Non zero - if fails
 */
int32_t uDMAx_Uninitialize(UDMA_RESOURCES *udma)
{
  // Disable DMA clock,Disable and Clear DMA IRQ
  if (udma->reg == UDMA0) {
    RSI_CLK_PeripheralClkDisable(M4CLK, UDMA_CLK);
  }
  if (udma->reg == UDMA1) {
    RSI_PS_UlpssPeriPowerDown(ULPSS_PWRGATE_ULP_UDMA);
    RSI_ULPSS_PeripheralDisable(ULPCLK, ULP_UDMA_CLK);
  }
  NVIC_DisableIRQ(udma->udma_irq_num);
  NVIC_ClearPendingIRQ(udma->udma_irq_num);
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn          int32_t uDMAx_ChannelConfigure(UDMA_RESOURCES *udma,
 *                              uint8_t ch,
 *                              uint32_t src_addr,
 *                              uint32_t dest_addr,
 *                              uint32_t size,
 *                              RSI_UDMA_CHA_CONFIG_DATA_T control,
 *                              RSI_UDMA_CHA_CFG_T *config,
 *                              UDMA_SignalEvent_t cb_event,
 *                              UDMA_Channel_Info *chnl_info,
 *                              RSI_UDMA_HANDLE_T udmaHandle)
 * @brief       This API is used to Configure UDMA channel for next transfer
 * @param[in]   udma      : Pointer to UDMA resources
 * @param[in]   ch        : Channel number (0..7)
 * @param[in]   src_addr  : Source address
 * @param[in]   dest_addr : Destination address
 * @param[in]   size      : Amount of data to transfer
 * @param[in]   control   : Channel control
 * @param[in]   config    : Channel configuration
 * @param[in]   cb_event  : Channel callback pointer
 * @param[in]   chnl_info : Pointer to UDMA channel info
 * @param[in]   udmaHandle: udma Handler
 * @return      Zero - if success
 *              Non zero - if fails
 */
int32_t uDMAx_ChannelConfigure(UDMA_RESOURCES *udma,
                               uint8_t ch,
                               uint32_t src_addr,
                               uint32_t dest_addr,
                               uint32_t size,
                               RSI_UDMA_CHA_CONFIG_DATA_T control,
                               RSI_UDMA_CHA_CFG_T *config,
                               UDMA_SignalEvent_t cb_event,
                               UDMA_Channel_Info *chnl_info,
                               RSI_UDMA_HANDLE_T udmaHandle)
{
  uint32_t len0 = 0, len1 = 0;
  if ((udma->reg == UDMA0) || (udma->reg == UDMA1)) {
    chnl_info[ch].cb_event = cb_event;
  }
  if (M4_ULP_SLP_STATUS_REG & ULP_MODE_SWITCHED_NPSS) { //PS2 state
    if (control.dstInc == UDMA_DST_INC_NONE) {
      if ((src_addr >= ULP_SRAM_START_ADDR) && (src_addr <= ULP_SRAM_END_ADDR)) {
        chnl_info[ch].SrcAddr = (src_addr);
      } else {
        return ERROR_UDMA_SRC_ADDR;
      }
    }
    if (control.srcInc == UDMA_SRC_INC_NONE) {
      if ((dest_addr >= ULP_SRAM_START_ADDR) && (dest_addr <= ULP_SRAM_END_ADDR)) {
        chnl_info[ch].DestAddr = (dest_addr);
      } else {
        return ERROR_UDMA_DST_ADDR;
      }
    }
  }
  if ((udma->reg == UDMA0) || (udma->reg == UDMA1)) {
    // Clear DMA interrupts
    RSI_UDMA_InterruptClear(udmaHandle, ch);
    RSI_UDMA_ErrorStatusClear(udmaHandle);
    RSI_UDMA_SetupChannel(udmaHandle, config);

    RSI_UDMA_SetupChannelTransfer(udmaHandle,
                                  (RSI_UDMA_CHA_CFG_T *)config,
                                  control,
                                  (void *)src_addr,
                                  (void *)dest_addr);
  }
  // Save channel information
  if (udma->reg == UDMA0) {
    len0                   = control.totalNumOfDMATrans;
    chnl_info[ch].SrcAddr  = (src_addr);
    chnl_info[ch].DestAddr = (dest_addr);
    chnl_info[ch].Size     = size;
    chnl_info[ch].Cnt      = (++len0);
  }
  if (udma->reg == UDMA1) {
    len1                   = control.totalNumOfDMATrans;
    chnl_info[ch].SrcAddr  = (src_addr);
    chnl_info[ch].DestAddr = (dest_addr);
    chnl_info[ch].Size     = size;
    chnl_info[ch].Cnt      = (++len1);
  }
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn          int32_t uDMAx_ChannelEnable(uint8_t ch, UDMA_RESOURCES *udma, RSI_UDMA_HANDLE_T udmaHandle)
 * @brief       This API is used to Enable UDMA channel
 * @param[in]   ch    : Channel number (0..7)
 * @param[in]   udma  : Pointer to UDMA resources
 * @param[in]   udmaHandle : udma Handler
 * @return      Zero - if success
 *              Non zero - if fails
 */
int32_t uDMAx_ChannelEnable(uint8_t ch, UDMA_RESOURCES *udma, RSI_UDMA_HANDLE_T udmaHandle)
{
  UNUSED_PARAMETER(udma);
  RSI_UDMA_ChannelEnable(udmaHandle, ch);
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn          int32_t uDMAx_DMAEnable(UDMA_RESOURCES *udma, RSI_UDMA_HANDLE_T udmaHandle)
 * @brief       This API is used to Enable UDMA DMA
 * @param[in]   udma      : Pointer to UDMA resources
 * @param[in]   udmaHandle: udma Handler
 * @return      Zero - if success
 *              Non zero - if fails
 */
int32_t uDMAx_DMAEnable(UDMA_RESOURCES *udma, RSI_UDMA_HANDLE_T udmaHandle)
{
  UNUSED_PARAMETER(udma);
  RSI_UDMA_UDMAEnable(udmaHandle);
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn          int32_t uDMAx_ChannelDisable(uint8_t ch, UDMA_RESOURCES *udma, RSI_UDMA_HANDLE_T udmaHandle)
 * @brief       This API is used to Disable UDMA channel
 * @param[in]   ch : Channel number (0..7)
 * @param[in]   udma : Pointer to UDMA resources
 * @param[in]   udmaHandle : udma Handler
 * @return      Zero - if success
 *              Non zero - if fails
 */
int32_t uDMAx_ChannelDisable(uint8_t ch, UDMA_RESOURCES *udma, RSI_UDMA_HANDLE_T udmaHandle)
{
  UNUSED_PARAMETER(udma);
  RSI_UDMA_ChannelDisable(udmaHandle, ch);
  return RSI_OK;
}

/*==============================================*/
/**
 * @fn          uint32_t uDMAx_ChannelGetCount(uint8_t ch,
 *                              RSI_UDMA_CHA_CONFIG_DATA_T control,
 *                              RSI_UDMA_CHA_CFG_T config,
 *                              UDMA_RESOURCES *udma,
 *                              RSI_UDMA_HANDLE_T udmaHandle)                                       
 * @brief       This API Gets the current transfer size for a UDMA channel control structure
 * @param[in]	ch	    : Channel number
 * @param[in]	control : structure variable of type RSI_UDMA_CHA_CONFIG_DATA
 * @param[in]   config  : structure variable of type RSI_UDMA_CHA_CFG_T
 * @param[in]   udma    : Pointer to UDMA resources
 * @param[in]   udmaHandle : UDMA Handle
 * @return      RSI_OK - if success
 */
uint32_t uDMAx_ChannelGetCount(uint8_t ch,
                               RSI_UDMA_CHA_CONFIG_DATA_T control,
                               RSI_UDMA_CHA_CFG_T config,
                               UDMA_RESOURCES *udma,
                               RSI_UDMA_HANDLE_T udmaHandle)
{
  UNUSED_PARAMETER(ch);
  UNUSED_PARAMETER(udma);
  return (RSI_UDMA_GetChannelTransferLength(udmaHandle, &config, control));
}

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
            UDMA_Table[ch].vsUDMAChaConfigData1.totalNumOfDMATrans = (dma_len);
            UDMA_Table[ch].vsUDMAChaConfigData1.transferType       = UDMA_MODE_BASIC;
          }
          if (udma->reg == UDMA1) {
            UDMA_Table[ch].pSrcEndAddr = (void *)((uint32_t)UDMA_Table[ch].pSrcEndAddr + (size << src_inc));
            UDMA_Table[ch].vsUDMAChaConfigData1.totalNumOfDMATrans = (dma_len);
            UDMA_Table[ch].vsUDMAChaConfigData1.transferType       = UDMA_MODE_BASIC;
          }
        } else {
          if (udma->reg == UDMA0) {
            UDMA_Table[ch].pSrcEndAddr                             = (void *)((uint32_t)UDMA_Table[ch].pSrcEndAddr);
            UDMA_Table[ch].vsUDMAChaConfigData1.totalNumOfDMATrans = (dma_len);
            UDMA_Table[ch].vsUDMAChaConfigData1.transferType       = UDMA_MODE_BASIC;
          }
          if (udma->reg == UDMA1) {
            UDMA_Table[ch].pSrcEndAddr                             = (void *)((uint32_t)UDMA_Table[ch].pSrcEndAddr);
            UDMA_Table[ch].vsUDMAChaConfigData1.totalNumOfDMATrans = (dma_len);
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
            UDMA_Table[ch].vsUDMAChaConfigData1.totalNumOfDMATrans = dma_len;
            UDMA_Table[ch].vsUDMAChaConfigData1.transferType       = UDMA_MODE_BASIC;
          }
          if (udma->reg == UDMA1) {
            UDMA_Table[ch].pDstEndAddr = (void *)((uint32_t)UDMA_Table[ch].pDstEndAddr + (size << dst_inc));
            UDMA_Table[ch].vsUDMAChaConfigData1.totalNumOfDMATrans = (dma_len);
            UDMA_Table[ch].vsUDMAChaConfigData1.transferType       = UDMA_MODE_BASIC;
          }
        } else {
          if (udma->reg == UDMA0) {
            UDMA_Table[ch].pDstEndAddr                             = (void *)((uint32_t)UDMA_Table[ch].pDstEndAddr);
            UDMA_Table[ch].vsUDMAChaConfigData1.totalNumOfDMATrans = dma_len;
            UDMA_Table[ch].vsUDMAChaConfigData1.transferType       = UDMA_MODE_BASIC;
          }
          if (udma->reg == UDMA1) {
            UDMA_Table[ch].pDstEndAddr                             = (void *)((uint32_t)UDMA_Table[ch].pDstEndAddr);
            UDMA_Table[ch].vsUDMAChaConfigData1.totalNumOfDMATrans = (dma_len);
            UDMA_Table[ch].vsUDMAChaConfigData1.transferType       = UDMA_MODE_BASIC;
          }
        }
        // Enable DMA Channel
        udma->reg->CHNL_ENABLE_SET = (1U << ch);
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
/** @} */

/*ROM API Structure
const ROM_UDMA_WRAPPER_API_T udma_wrapper_api = {
		&uDMAx_Initialize,
		&uDMAx_Uninitialize,
		&uDMAx_ChannelConfigure,
		&uDMAx_ChannelEnable,
		&uDMAx_DMAEnable,
		&uDMAx_ChannelDisable,	
		&uDMAx_ChannelGetCount,
		&uDMAx_IRQHandler,  
};
*/
#ifdef __cplusplus
}
#endif

#else
typedef int dummy; // To remove empty translation unit warning.
#endif //  A11_ROM ||  ROMDRIVER_PRESENT
