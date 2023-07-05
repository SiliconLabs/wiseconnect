/*******************************************************************************
* @file  rsi_gspi.h
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

/**============================================================================
 * @brief This files contains functions prototypes related to EGPIO peripheral
 * @section Description
 * This file contains the list of function prototypes for the usart and low level function definitions
 * Following are list of API's which need to be defined in this file.
 ============================================================================**/

// Includes Files

#include "GSPI.h"
#include "rsi_udma_wrapper.h"

#ifndef RSI_GSPI_H
#define RSI_GSPI_H

#ifdef __cplusplus
extern "C" {
#endif

int32_t GSPI_Initialize(ARM_SPI_SignalEvent_t cb_event,
                        const GSPI_RESOURCES *gspi,
                        UDMA_RESOURCES *udma,
                        RSI_UDMA_DESC_T *UDMA_Table,
                        RSI_UDMA_HANDLE_T *udmaHandle,
                        uint32_t *mem);
int32_t GSPI_Uninitialize(const GSPI_RESOURCES *gspi, UDMA_RESOURCES *udma);
int32_t GSPI_PowerControl(ARM_POWER_STATE state, const GSPI_RESOURCES *gspi);
int32_t GSPI_Control(uint32_t control,
                     uint32_t arg,
                     const GSPI_RESOURCES *gspi,
                     uint32_t base_clock,
                     uint8_t gspi_slavenumber);
int32_t GSPI_Send(const void *data,
                  uint32_t num,
                  const GSPI_RESOURCES *gspi,
                  UDMA_RESOURCES *udma,
                  UDMA_Channel_Info *chnl_info,
                  RSI_UDMA_HANDLE_T udmaHandle);
int32_t GSPI_Receive(void *data,
                     uint32_t num,
                     const GSPI_RESOURCES *gspi,
                     UDMA_RESOURCES *udma,
                     UDMA_Channel_Info *chnl_info,
                     RSI_UDMA_HANDLE_T udmaHandle);
int32_t GSPI_Transfer(const void *data_out,
                      void *data_in,
                      uint32_t num,
                      const GSPI_RESOURCES *gspi,
                      UDMA_RESOURCES *udma,
                      UDMA_Channel_Info *chnl_info,
                      RSI_UDMA_HANDLE_T udmaHandle);
uint32_t GSPI_GetDataCount(const GSPI_RESOURCES *gspi);
ARM_SPI_STATUS GSPI_GetStatus(const GSPI_RESOURCES *gspi);
void GSPI_UDMA_Tx_Event(uint32_t event, uint8_t dmaCh, GSPI_RESOURCES *gspi);
void GSPI_UDMA_Rx_Event(uint32_t event, uint8_t dmaCh, GSPI_RESOURCES *gspi);
void GSPI_IRQHandler(const GSPI_RESOURCES *gspi);
uint32_t GSPI_GetFrameLength(void);
int32_t GSPI_SwapReadWriteByte(boolean_t read, boolean_t write);
uint32_t GSPI_GetTxCount(void);
uint8_t GSPI_GetInitState(void);
// Static Inline function to set the spi memory map pll
static inline void GSPI_SetMemoryMapPll(uint16_t value)
{
  SPI_MEM_MAP_PLL(INTF_PLL_500_CTRL_REG9) = value;
}

#ifdef __cplusplus
}
#endif

#endif // RSI_GSPI_H
