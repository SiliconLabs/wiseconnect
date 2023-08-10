/*******************************************************************************
* @file  rsi_spi.h
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
 * @brief   This files contains functions prototypes related to EGPIO peripheral
 * @section Description :
 * This file contains the list of function prototypes for the usart and low level function definitions
 * Following are list of API's which need to be defined in this file.
============================================================================**/

// Include Files

#include "SPI.h"
#include "rsi_udma_wrapper.h"

#ifndef RSI_GSPI_H
#define RSI_GSPI_H

#ifdef __cplusplus
extern "C" {
#endif

int32_t SPI_Initialize(ARM_SPI_SignalEvent_t cb_event,
                       const SPI_RESOURCES *spi,
                       UDMA_RESOURCES *udma,
                       RSI_UDMA_DESC_T *UDMA_Table,
                       RSI_UDMA_HANDLE_T *udmaHandle,
                       uint32_t *mem);
int32_t SPI_Uninitialize(const SPI_RESOURCES *spi, UDMA_RESOURCES *udma);
int32_t SPI_PowerControl(ARM_POWER_STATE state, const SPI_RESOURCES *spi);
int32_t SPI_Send(const void *data,
                 uint32_t num,
                 const SPI_RESOURCES *spi,
                 UDMA_RESOURCES *udma,
                 UDMA_Channel_Info *chnl_info,
                 RSI_UDMA_HANDLE_T udmaHandle);
int32_t SPI_Receive(void *data,
                    uint32_t num,
                    const SPI_RESOURCES *spi,
                    UDMA_RESOURCES *udma,
                    UDMA_Channel_Info *chnl_info,
                    RSI_UDMA_HANDLE_T udmaHandle);
int32_t SPI_Transfer(const void *data_out,
                     void *data_in,
                     uint32_t num,
                     const SPI_RESOURCES *spi,
                     UDMA_RESOURCES *udma,
                     UDMA_Channel_Info *chnl_info,
                     RSI_UDMA_HANDLE_T udmaHandle);
uint32_t SPI_GetDataCount(const SPI_RESOURCES *spi);
int32_t SPI_Control(uint32_t control,
                    uint32_t arg,
                    const SPI_RESOURCES *spi,
                    uint32_t base_clock,
                    uint8_t spi_slavenumber);
ARM_SPI_STATUS SPI_GetStatus(const SPI_RESOURCES *spi);
void SPI_UDMA_Tx_Event(uint32_t event, uint8_t dmaCh, SPI_RESOURCES *spi);
void SPI_UDMA_Rx_Event(uint32_t event, uint8_t dmaCh, SPI_RESOURCES *spi);
void SPI_IRQHandler(const SPI_RESOURCES *spi);
uint32_t SSI_GetClockDivisionFactor(uint8_t ssi_instance);
uint32_t SSI_GetFrameLength(uint8_t ssi_instance);
uint8_t SSI_GetInitState(uint8_t ssi_instance);
uint32_t SSI_GetTxCount(uint8_t ssi_instance);

// Static Inline function to set the spi memory map pll
static inline void SSI_SetMemoryMapPll(uint16_t value)
{
  SPI_MEM_MAP_PLL(INTF_PLL_500_CTRL_REG9) = value;
}

#ifdef __cplusplus
}
#endif

#endif // RSI_SPI_H
