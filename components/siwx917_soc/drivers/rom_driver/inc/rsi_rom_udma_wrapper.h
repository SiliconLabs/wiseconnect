/*******************************************************************************
* @file  rsi_rom_udma_wrapper.h
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

#ifndef __RSI_ROM_UDMA_WRAPPER_H__
#define __RSI_ROM_UDMA_WRAPPER_H__

/**
 * \ingroup   RSI_SPECIFIC_DRIVERS
 * \defgroup RSI_UDMA_DRIVER RSI:RS1xxxx UDMA WRAPPER 
 *  @{
 *
 */
#include "rsi_ccp_user_config.h"
#include "rsi_packing.h"
#include "rsi_chip.h"

#include "rsi_udma_wrapper.h"
#include "UDMA.h"
#if defined(A11_ROM)
#include "rsi_rom_table_si91x.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif

STATIC INLINE RSI_UDMA_HANDLE_T UDMAx_Initialize(UDMA_RESOURCES *udma,
                                                 RSI_UDMA_DESC_T *UDMA_Table,
                                                 RSI_UDMA_HANDLE_T udmaHandle,
                                                 uint32_t *mem)
{
#if defined(ROMDRIVER_PRESENT) && defined(A11_ROM)
  return udmaHandle = ROMAPI_UDMA_WRAPPER_API->uDMAx_Initialize(udma, UDMA_Table, udmaHandle, mem);
#else
  return udmaHandle = uDMAx_Initialize(udma, UDMA_Table, udmaHandle, mem);
#endif
}

STATIC INLINE int32_t UDMAx_Uninitialize(UDMA_RESOURCES *udma)
{
#if defined(ROMDRIVER_PRESENT) && defined(A11_ROM)
  return ROMAPI_UDMA_WRAPPER_API->uDMAx_Uninitialize(udma);
#else
  return uDMAx_Uninitialize(udma);
#endif
}

STATIC INLINE int32_t UDMAx_ChannelConfigure(UDMA_RESOURCES *udma,
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

#if defined(ROMDRIVER_PRESENT) && defined(A11_ROM)
  return ROMAPI_UDMA_WRAPPER_API
    ->uDMAx_ChannelConfigure(udma, ch, src_addr, dest_addr, size, control, config, cb_event, chnl_info, udmaHandle);
#else
  return uDMAx_ChannelConfigure(udma, ch, src_addr, dest_addr, size, control, config, cb_event, chnl_info, udmaHandle);
#endif
}

STATIC INLINE int32_t UDMAx_ChannelEnable(uint8_t ch, UDMA_RESOURCES *udma, RSI_UDMA_HANDLE_T udmaHandle)
{
#if defined(ROMDRIVER_PRESENT) && defined(A11_ROM)

  return ROMAPI_UDMA_WRAPPER_API->uDMAx_ChannelEnable(ch, udma, udmaHandle);
#else
  return uDMAx_ChannelEnable(ch, udma, udmaHandle);
#endif
}

STATIC INLINE int32_t UDMAx_DMAEnable(UDMA_RESOURCES *udma, RSI_UDMA_HANDLE_T udmaHandle)
{
#if defined(ROMDRIVER_PRESENT) && defined(A11_ROM)
  return ROMAPI_UDMA_WRAPPER_API->uDMAx_DMAEnable(udma, udmaHandle);
#else
  return uDMAx_DMAEnable(udma, udmaHandle);
#endif
}

STATIC INLINE int32_t UDMAx_ChannelDisable(uint8_t ch, UDMA_RESOURCES *udma, RSI_UDMA_HANDLE_T udmaHandle)
{
#if defined(ROMDRIVER_PRESENT) && defined(A11_ROM)
  return ROMAPI_UDMA_WRAPPER_API->uDMAx_ChannelDisable(ch, udma, udmaHandle);
#else
  return uDMAx_ChannelDisable(ch, udma, udmaHandle);
#endif
}

STATIC INLINE uint32_t UDMAx_ChannelGetCount(uint8_t ch,
                                             RSI_UDMA_CHA_CONFIG_DATA_T control,
                                             RSI_UDMA_CHA_CFG_T config,
                                             UDMA_RESOURCES *udma,
                                             RSI_UDMA_HANDLE_T udmaHandle)
{
#if defined(ROMDRIVER_PRESENT) && defined(A11_ROM)
  return ROMAPI_UDMA_WRAPPER_API->uDMAx_ChannelGetCount(ch, control, config, udma, udmaHandle);
#else
  return uDMAx_ChannelGetCount(ch, control, config, udma, udmaHandle);
#endif
}
STATIC INLINE void UDMAx_IRQHandler(UDMA_RESOURCES *udma, RSI_UDMA_DESC_T *UDMA_Table, UDMA_Channel_Info *chnl_info)
{
#if defined(ROMDRIVER_PRESENT) && defined(A11_ROM)
  ROMAPI_UDMA_WRAPPER_API->uDMAx_IRQHandler(udma, UDMA_Table, chnl_info);
#else
  uDMAx_IRQHandler(udma, UDMA_Table, chnl_info);
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* __RSI_ROM_UDMA_WRAPPER_H__*/
