/******************************************************************************
* @file  rsi_rom_gpdma.h
*******************************************************************************
* # License
* <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* SPDX-License-Identifier: Zlib
*
* The licensor of this software is Silicon Laboratories Inc.
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgment in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
******************************************************************************/

#ifndef __RSI_ROM_GPDMA_H_
#define __RSI_ROM_GPDMA_H_

/**
 * \ingroup   RSI_SPECIFIC_DRIVERS
 * \defgroup GPDMA_DRIVER 
 *  @{
 *
 */
#include "rsi_ccp_user_config.h"
#include "rsi_packing.h"
#include "rsi_gpdma.h"
#if defined(A11_ROM)
#include "rsi_rom_table_si91x.h"
#else
#include "rsi_rom_table_RS1xxxx.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @fn          STATIC INLINE uint32_t  RSI_GPDMA_GetMemSize(void)
 * @brief		    Get memory size in bytes needed for GPDMA controller driver context
 * @return 		  Size in bytes.
 */
STATIC INLINE uint32_t RSI_GPDMA_GetMemSize(void)
{
#if defined(GPDMA_ROMDRIVER_PRESENT)
  return ROMAPI_GPDMA_API->gpdma_get_mem_size();
#else
  return gpdma_get_mem_size();
#endif
}

/**
 * @fn          STATIC INLINE RSI_GPDMA_HANDLE_T RSI_GPDMA_Init(void *mem, const RSI_GPDMA_INIT_T *pInit)

 * @brief		    This API is used to initialize driver context parameters
 * @param[in]   mem   : Pointer to memory area used to driver context
 * @param[in]   pInit :	Pointer to DMA controller init data
 * @return 		  NULL on error, or a pointer to the device context handle
 */
STATIC INLINE RSI_GPDMA_HANDLE_T RSI_GPDMA_Init(void *mem, const RSI_GPDMA_INIT_T *pInit)
{
#if defined(GPDMA_ROMDRIVER_PRESENT)
  return ROMAPI_GPDMA_API->gpdma_init(mem, pInit);
#else
  return gpdma_init(mem, pInit);
#endif
}

/**
 * @fn          STATIC INLINE void  RSI_GPDMA_RegisterCallback(RSI_GPDMA_HANDLE_T pHandle,uint32_t cbIndex, void *pCB)
 * @brief		    This API is used to regiser callbacks
 * @param[in]   pHandle  : Pointer to driver context handle
 * @param[in]   cbIndex  : Index for various callback pointers
 * @param[in]   pCB      : Pointer to callback function
 * @return 		  none
 */
STATIC INLINE void RSI_GPDMA_RegisterCallback(RSI_GPDMA_HANDLE_T pHandle, uint32_t cbIndex, gpdmaTransferCompleteCB pCB)
{
#if defined(GPDMA_ROMDRIVER_PRESENT)
  ROMAPI_GPDMA_API->gpdma_register_callback(pHandle, cbIndex, pCB);
#else
  gpdma_register_callback(pHandle, cbIndex, pCB);
#endif
}

/**
 * @fn          STATIC INLINE rsi_error_t RSI_GPDMA_AbortChannel(RSI_GPDMA_HANDLE_T pHandle,uint8_t dmaCh)
 * @brief		    This API is used to Abort the channel transfer
 * @param[in]   pHandle  : Pointer to driver context handle
 * @param[in]   dmaCh    : DMA channel number(0-7)
 * @return 		  return \ref RSI_OK
 */
STATIC INLINE rsi_error_t RSI_GPDMA_AbortChannel(RSI_GPDMA_HANDLE_T pHandle, uint8_t dmaCh)
{
#if defined(GPDMA_ROMDRIVER_PRESENT)
  return ROMAPI_GPDMA_API->gpdma_abort_channel(pHandle, dmaCh);
#else
  return gpdma_abort_channel(pHandle, dmaCh);
#endif
}

/**
 * @fn          STATIC INLINE rsi_error_t RSI_GPDMA_SetupChannel(RSI_GPDMA_HANDLE_T pHandle, RSI_GPDMA_CHA_CFG_T *pCfg)

 * @brief		    This API is used to configures required parameters for a channel.
 * @param[in]   pHandle  : Pointer to driver context handle
 * @param[in]   pCfg     : \ref RSI_GPDMA_CHA_CFG_T  Pointer to DMA channel configuration structure
 * @return 		  \ref ERROR_GPDMA_CHNL_BUSY      : If selecetd channel is busy
                \n \ref ERROR_GPDMA_INVALID_ARG : If channel number is invalid
                \n \ref RSI_OK                  : If process is done successfully
 */
STATIC INLINE rsi_error_t RSI_GPDMA_SetupChannel(RSI_GPDMA_HANDLE_T pHandle, RSI_GPDMA_CHA_CFG_T *pCfg)
{
#if defined(GPDMA_ROMDRIVER_PRESENT)
  return ROMAPI_GPDMA_API->gpdma_setup_channel(pHandle, pCfg);
#else
  return gpdma_setup_channel(pHandle, pCfg);
#endif
}

/**
 * @fn         STATIC INLINE rsi_error_t RSI_GPDMA_BuildDescriptors(RSI_GPDMA_HANDLE_T pHandle,RSI_GPDMA_DESC_T   *pXferCfg,
                                                  RSI_GPDMA_DESC_T   *pDesc,RSI_GPDMA_DESC_T   *pDescPrev )
 * @brief		   Sets and build the descriptor for data transfer.
 * @param[in]	 pHandle     : Pointer to driver context handle
 * @param[in]	 pXferCfg    : \ref RSI_GPDMA_DESC_T  In this structure define channel configuration structure pointer,define all structure member.
 * @param[in]  pDesc       : \ref RSI_GPDMA_DESC_T  GPDMA descriptor structure pointer.
 * @param[in]  pDescPrev   : \ref RSI_GPDMA_DESC_T  previous GPDMA descriptor.
 * @return     \ref ERROR_GPDMA_INVALID_ARG : If any descriptor parameter is invalid
               \n \ref RSI_OK               : If process is done successfully
 */
STATIC INLINE rsi_error_t RSI_GPDMA_BuildDescriptors(RSI_GPDMA_HANDLE_T pHandle,
                                                     RSI_GPDMA_DESC_T *pXferCfg,
                                                     RSI_GPDMA_DESC_T *pDesc,
                                                     RSI_GPDMA_DESC_T *pDescPrev)
{
#if defined(GPDMA_ROMDRIVER_PRESENT)
  return ROMAPI_GPDMA_API->gpdma_build_descriptors(pHandle, pXferCfg, pDesc, pDescPrev);
#else
  return gpdma_build_descriptors(pHandle, pXferCfg, pDesc, pDescPrev);
#endif
}

/**
 * @fn         STATIC INLINE rsi_error_t RSI_GPDMA_SetupChannelTransfer( RSI_GPDMA_HANDLE_T pHandle,
                                                       uint8_t dmaCh, RSI_GPDMA_DESC_T *pDesc)

 * @brief		   Sets the control parameters for a GPDMA channel control structure.
 * @param[in]	 pHandle  : Pointer to driver context handle
 * @param[in]	 dmaCh    :	DMA channel number(0-7)
 * @param[in]  pDesc    : \ref RSI_GPDMA_DESC_T GPDMA descriptor structure pointer.
 * @return     \ref ERROR_GPDMA_INVALID_TRANS_LEN   : If transSize parameter of descriptor is invalid
               \n \ref ERROR_GPDMA_INVALID_XFERMODE : If transType parameter of descriptor is invalid
               \n \ref ERROR_GPDMA_FLW_CTRL       : If dmaFlwCtrl parameter of descriptor is invalid
               \n \ref ERROR_GPDMA_INVALIDCHNLNUM : If DMA channel number is invalid
               \n \ref ERROR_GPDMA_INVALID_ARG : If any other descriptor parameter is invalid
               \n \ref RSI_OK                  : If process is done successfully
 */
STATIC INLINE rsi_error_t RSI_GPDMA_SetupChannelTransfer(RSI_GPDMA_HANDLE_T pHandle,
                                                         uint8_t dmaCh,
                                                         RSI_GPDMA_DESC_T *pDesc)
{
#if defined(GPDMA_ROMDRIVER_PRESENT)
  return ROMAPI_GPDMA_API->gpdma_setup_channelTransfer(pHandle, dmaCh, pDesc);
#else
  return gpdma_setup_channelTransfer(pHandle, dmaCh, pDesc);
#endif
}

/**
 * @fn          STATIC INLINE void  RSI_GPDMA_InterruptHandler(RSI_GPDMA_HANDLE_T pHandle)
 * @brief		    This API is used to handle DMA interrupts for all channels.
 * @param[in]   pHandle  : Pointer to driver context handle
 * @return      none
 */
STATIC INLINE void RSI_GPDMA_InterruptHandler(RSI_GPDMA_HANDLE_T pHandle)
{
#if defined(GPDMA_ROMDRIVER_PRESENT)
  ROMAPI_GPDMA_API->gpdma_interrupt_handler(pHandle);
#else
  gpdma_interrupt_handler(pHandle);
#endif
}

/**
 * @fn          STATIC INLINE void  RSI_GPDMA_DeInit(RSI_GPDMA_HANDLE_T pHandle, RSI_GPDMA_CHA_CFG_T *pCfg)

 * @brief		    This API is used to Uninitialize driver context parameters.
 * @param[in]   pHandle  : Pointer to driver context handle
 * @param[in]   pCfg     : Pointer to DMA channel configuration structure
 * @return 		  none
 */
STATIC INLINE void RSI_GPDMA_DeInit(RSI_GPDMA_HANDLE_T pHandle, RSI_GPDMA_CHA_CFG_T *pCfg)
{
#if defined(GPDMA_ROMDRIVER_PRESENT)
  ROMAPI_GPDMA_API->gpdma_deInit(pHandle, pCfg);
#else
  gpdma_deInit(pHandle, pCfg);
#endif
}
/**
 * @fn          STATIC INLINE rsi_error_t RSI_GPDMA_DMAChannelTrigger(RSI_GPDMA_HANDLE_T pHandle ,uint8_t dmaCh)

 * @brief		    This API is used to enable the required channel of GPDMA
 * @param[in]   pHandle  : Pointer to driver context handle
 * @param[in]   dmaCh    : Channel number(0 to 7)
 * @return 		  \ref ERROR_GPDMA_INVALIDCHNLNUM : If DMA channel number is invalid
                \n \ref RSI_OK                  : If process is done successfully
 */
STATIC INLINE rsi_error_t RSI_GPDMA_DMAChannelTrigger(RSI_GPDMA_HANDLE_T pHandle, uint8_t dmaCh)
{
#if defined(GPDMA_ROMDRIVER_PRESENT)
  return ROMAPI_GPDMA_API->gpdma_dma_channel_trigger(pHandle, dmaCh);
#else
  return gpdma_dma_channel_trigger(pHandle, dmaCh);
#endif
}

/**
 * @fn          STATIC INLINE uint32_t  RSI_GPDMA_ChannelIsEnabled(RSI_GPDMA_HANDLE_T pHandle ,uint8_t dmaCh)

 * @brief		    This API is used to check the required GPDMA channel is enabled or not.
 * @param[in]	  pHandle  : Pointer to driver context handle
 * @param[in]	  dmaCh    : Channel number(0 to 7)
 * @return 		  \ref ERROR_GPDMA_INVALIDCHNLNUM : If DMA channel number is invalid
                \n \ref RSI_OK                  : If process is done successfully
 */
STATIC INLINE uint32_t RSI_GPDMA_ChannelIsEnabled(RSI_GPDMA_HANDLE_T pHandle, uint8_t dmaCh)
{
#if defined(GPDMA_ROMDRIVER_PRESENT)
  return ROMAPI_GPDMA_API->gpdma_channel_is_enabled(pHandle, dmaCh);
#else
  return gpdma_channel_is_enabled(pHandle, dmaCh);
#endif
}

/**
 * @fn          STATIC INLINE rsi_error_t RSI_GPDMA_InterruptDisable(RSI_GPDMA_HANDLE_T pHandle, RSI_GPDMA_CHA_CFG_T *pCfg)
 * @brief		    This API is used to disable the interrupt for required GPDMA channel.
 * @param[in]   pHandle  : Pointer to driver context handle
 * @param[in]   pCfg     : Pointer to DMA channel configuration structure \ref RSI_GPDMA_CHA_CFG_T
 * @return 		  \ref ERROR_GPDMA_INVALIDCHNLNUM : If DMA channel number is invalid
                \n \ref RSI_OK                  : If process is done successfully
 */
STATIC INLINE rsi_error_t RSI_GPDMA_InterruptDisable(RSI_GPDMA_HANDLE_T pHandle, RSI_GPDMA_CHA_CFG_T *pCfg)
{
#if defined(GPDMA_ROMDRIVER_PRESENT)
  return ROMAPI_GPDMA_API->gpdma_interrupt_disable(pHandle, pCfg);
#else
  return gpdma_interrupt_disable(pHandle, pCfg);
#endif
}

/**
 * @fn          STATIC INLINE rsi_error_t RSI_GPDMA_InterruptEnable(RSI_GPDMA_HANDLE_T pHandle, RSI_GPDMA_CHA_CFG_T *pCfg)
 * @brief		    This API is used to enable interrupt flags for required GPDMA channel
 * @param[in]   pHandle  : Pointer to driver context handle
 * @param[in]   pCfg     : Pointer to DMA channel configuration structure  \ref RSI_GPDMA_CHA_CFG_T
 * @return 		  \ref ERROR_GPDMA_INVALIDCHNLNUM : If DMA channel number is invalid
                \n \ref RSI_OK                  : If process is done successfully
 */
STATIC INLINE rsi_error_t RSI_GPDMA_InterruptEnable(RSI_GPDMA_HANDLE_T pHandle, RSI_GPDMA_CHA_CFG_T *pCfg)
{
#if defined(GPDMA_ROMDRIVER_PRESENT)
  return ROMAPI_GPDMA_API->gpdma_interrupt_enable(pHandle, pCfg);
#else
  return gpdma_interrupt_enable(pHandle, pCfg);
#endif
}

/**
 * @fn          STATIC INLINE rsi_error_t RSI_GPDMA_ErrorStatusClear( RSI_GPDMA_HANDLE_T pHandle, RSI_GPDMA_CHA_CFG_T *pCfg)

 * @brief	      This API is used to clear the errors for the required GPDMA channel.
 * @param[in]   pHandle  : Pointer to driver context handle 
 * @param[in]   pCfg     : Pointer to DMA channel configuration structure \ref RSI_GPDMA_CHA_CFG_T
 * @return 		 \ref ERROR_GPDMA_INVALIDCHNLNUM : If DMA channel number is invalid
               \n \ref RSI_OK                  : If process is done successfully
 */
STATIC INLINE rsi_error_t RSI_GPDMA_ErrorStatusClear(RSI_GPDMA_HANDLE_T pHandle, RSI_GPDMA_CHA_CFG_T *pCfg)
{
#if defined(GPDMA_ROMDRIVER_PRESENT)
  return ROMAPI_GPDMA_API->gpdma_error_status_clear(pHandle, pCfg);
#else
  return gpdma_error_status_clear(pHandle, pCfg);
#endif
}

/**
 * @fn          STATIC INLINE uint32_t  RSI_GPDMA_GetErrorStatus(RSI_GPDMA_HANDLE_T pHandle,
                                                  RSI_GPDMA_CHA_CFG_T *pCfg)
 * @brief		    This API is used to get the error status of required GPDMA channel
 * @param[in]   pHandle  : Pointer to driver context handle
 * @param[in]   pCfg     : Pointer to DMA channel configuration structure \ref RSI_GPDMA_CHA_CFG_T
 * @return 		  Return error status of either \ref HRESP_ ERR or \ref GPDMAC_ERR
 */
STATIC INLINE uint32_t RSI_GPDMA_GetErrorStatus(RSI_GPDMA_HANDLE_T pHandle, RSI_GPDMA_CHA_CFG_T *pCfg)
{
#if defined(GPDMA_ROMDRIVER_PRESENT)
  return ROMAPI_GPDMA_API->gpdma_get_error_status(pHandle, pCfg);
#else
  return gpdma_get_error_status(pHandle, pCfg);
#endif
}

/**
 * @fn          STATIC INLINE rsi_error_t RSI_GPDMA_InterruptClear( RSI_GPDMA_HANDLE_T pHandle,
                                                  RSI_GPDMA_CHA_CFG_T *pCfg)
 * @brief		    This API is used to clear the interrupts of required GPDMA channel
 * @param[in]   pHandle  : Pointer to driver context handle
 * @param[in]   pCfg     : Pointer to DMA channel configuration structure \ref RSI_GPDMA_CHA_CFG_T
 * @return 		  \ref ERROR_GPDMA_INVALIDCHNLNUM : If DMA channel number is invalid
                \n \ref RSI_OK                  : If process is done successfully
 */
STATIC INLINE rsi_error_t RSI_GPDMA_InterruptClear(RSI_GPDMA_HANDLE_T pHandle, RSI_GPDMA_CHA_CFG_T *pCfg)
{
#if defined(GPDMA_ROMDRIVER_PRESENT)
  return ROMAPI_GPDMA_API->gpdma_interrupt_clear(pHandle, pCfg);
#else
  return gpdma_interrupt_clear(pHandle, pCfg);
#endif
}
/**
 * @fn            STATIC INLINE uint32_t  RSI_GPDMA_InterruptStatus(RSI_GPDMA_HANDLE_T pHandle,
                                                     RSI_GPDMA_CHA_CFG_T *pCfg)
 * @brief		      This API is used to get the interrupt status of required GPDMA channel
 * @param[in]     pHandle  : Pointer to driver context handle
 * @param[in]     pCfg     : Pointer to DMA channel configuration structure \ref RSI_GPDMA_CHA_CFG_T
 * @return 		    Returns the interrupt status of any one of the below flags
                            - \ref LINK_LIST_DONE  : Next link list pointer points to zero
									          - \ref PHRL_END_OF_TFR : Indicates last burst or single transfer
                  \n \ref ERROR_GPDMA_INVALIDCHNLNUM : If DMA channel number is invalid
                  \n \ref RSI_OK                     : If process is done successfully
 */
STATIC INLINE uint32_t RSI_GPDMA_InterruptStatus(RSI_GPDMA_HANDLE_T pHandle, RSI_GPDMA_CHA_CFG_T *pCfg)
{
#if defined(GPDMA_ROMDRIVER_PRESENT)
  return ROMAPI_GPDMA_API->gpdma_interrupt_status(pHandle, pCfg);
#else
  return gpdma_interrupt_status(pHandle, pCfg);
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* __RSI_ROM_GPDMA_H_ */

/* @} end of RSI_GPDMA_DRIVERS */
