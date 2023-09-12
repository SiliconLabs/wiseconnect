/*******************************************************************************
* @file  rsi_udma_wrapper.h
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
/**===========================================================================
 * @brief This files contains functions prototypes releated to UDMA peripheral
 * @section Description :
 * This file contains the list of function prototypes for the UDMA and low level function definations 
 * Following are list of API's which need to be defined in this file.
============================================================================**/

// Include Files

#include "rsi_ccp_common.h"
#include "rsi_error.h"
#include "rsi_packing.h"
#include "UDMA.h"

#ifndef RSI_UDMA_WRAPPER_H
#define RSI_UDMA_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#define UNUSED_PARAMETER(x) (void)(x)

typedef struct {
  uint32_t SrcAddr;
  uint32_t DestAddr;
  uint32_t Size;
  uint32_t Cnt;
  UDMA_SignalEvent_t cb_event;
} UDMA_Channel_Info;

RSI_UDMA_HANDLE_T uDMAx_Initialize(UDMA_RESOURCES *udma,
                                   RSI_UDMA_DESC_T *UDMA_Table,
                                   RSI_UDMA_HANDLE_T udmaHandle,
                                   uint32_t *mem);
int32_t uDMAx_Uninitialize(UDMA_RESOURCES *udma);
int32_t uDMAx_ChannelConfigure(UDMA_RESOURCES *udma,
                               uint8_t ch,
                               uint32_t src_addr,
                               uint32_t dest_addr,
                               uint32_t size,
                               RSI_UDMA_CHA_CONFIG_DATA_T control,
                               RSI_UDMA_CHA_CFG_T *config,
                               UDMA_SignalEvent_t cb_event,
                               UDMA_Channel_Info *chnl_info,
                               RSI_UDMA_HANDLE_T udmaHandle);
int32_t uDMAx_ChannelEnable(uint8_t ch, UDMA_RESOURCES *udma, RSI_UDMA_HANDLE_T udmaHandle);
int32_t uDMAx_DMAEnable(UDMA_RESOURCES *udma, RSI_UDMA_HANDLE_T udmaHandle);
int32_t uDMAx_ChannelDisable(uint8_t ch, UDMA_RESOURCES *udma, RSI_UDMA_HANDLE_T udmaHandle);
uint32_t uDMAx_ChannelGetCount(uint8_t ch,
                               RSI_UDMA_CHA_CONFIG_DATA_T control,
                               RSI_UDMA_CHA_CFG_T config,
                               UDMA_RESOURCES *udma,
                               RSI_UDMA_HANDLE_T udmaHandle);
void uDMAx_IRQHandler(UDMA_RESOURCES *udma, RSI_UDMA_DESC_T *UDMA_Table, UDMA_Channel_Info *chnl_info);

#ifdef __cplusplus
}
#endif

#endif // RSI_UDMA_WRAPPER_H
