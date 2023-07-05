/*******************************************************************************
* @file  rsi_spi.c
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
/*************************************************************************
 *
 */

/**
 * Includes
 */

#include "rsi_ccp_user_config.h"
#ifndef ROMDRIVER_PRESENT
#include "rsi_rom_clks.h"
#include "rsi_rom_egpio.h"
#include "rsi_rom_udma_wrapper.h"
#include "SPI.h"
#include "rsi_spi.h"

/** @addtogroup SOC15
* @{
*/
/**
 * @fn           int32_t SPI_Initialize(ARM_SPI_SignalEvent_t cb_event, const SPI_RESOURCES *spi)
 * @brief        Initialize SPI Interface.
 * @param[in]    cb_event  : Pointer to the ARM_SPI_SignalEvent
 * @param[in]    spi       : Pointer to the spi resources
 * @return 		   excecution status 
 */

int32_t SPI_Initialize(ARM_SPI_SignalEvent_t cb_event,
                       const SPI_RESOURCES *spi,
                       UDMA_RESOURCES *udma,
                       RSI_UDMA_DESC_T *UDMA_Table,
                       RSI_UDMA_HANDLE_T *udmaHandle,
                       uint32_t *mem)
{
  if (spi->info->state & SPI_INITIALIZED) {
    return ARM_DRIVER_OK;
  }

  // Initialize SPI Run-Time Resources
  spi->info->cb_event          = cb_event;
  spi->info->status.busy       = 0U;
  spi->info->status.data_lost  = 0U;
  spi->info->status.mode_fault = 0U;

  // Clear transfer information
  memset(spi->xfer, 0, sizeof(SPI_TRANSFER_INFO));
  if ((spi->instance_mode == SPI_MASTER_MODE) || (spi->instance_mode == SPI_SLAVE_MODE)) {

    //CLK
    if (spi->io.sck->pin > 64) {
      RSI_EGPIO_UlpPadReceiverEnable(spi->io.sck->pin - 64);
      RSI_EGPIO_SetPinMux(EGPIO1, 0, (spi->io.sck->pin - 64), EGPIO_PIN_MUX_MODE6);
    }
    RSI_EGPIO_SetPinMux(EGPIO, spi->io.sck->port, spi->io.sck->pin, spi->io.sck->mode);
    if (spi->io.sck->pad_sel != 0) {
      RSI_EGPIO_PadSelectionEnable(spi->io.sck->pad_sel);
    }
    RSI_EGPIO_PadReceiverEnable(spi->io.sck->pin);
    if (spi->io.sck->pin >= 25 && spi->io.sck->pin <= 30)
      RSI_EGPIO_HostPadsGpioModeEnable(spi->io.sck->pin);

    //CS0
    if (spi->io.cs0 != NULL) {
      if (spi->io.cs0->pin > 64) {
        RSI_EGPIO_UlpPadReceiverEnable(spi->io.cs0->pin - 64);
        RSI_EGPIO_SetPinMux(EGPIO1, 0, (spi->io.cs0->pin - 64), EGPIO_PIN_MUX_MODE6);
      }
      RSI_EGPIO_SetPinMux(EGPIO, spi->io.cs0->port, spi->io.cs0->pin, spi->io.cs0->mode);
      if (spi->io.cs0->pad_sel != 0) {
        RSI_EGPIO_PadSelectionEnable(spi->io.cs0->pad_sel);
      }
      RSI_EGPIO_PadReceiverEnable(spi->io.cs0->pin);
      if (spi->io.cs0->pin >= 25 && spi->io.cs0->pin <= 30)
        RSI_EGPIO_HostPadsGpioModeEnable(spi->io.cs0->pin);
    }
    //CS1
    if (spi->io.cs1 != NULL) {
      if (spi->io.cs1->pin > 64) {
        RSI_EGPIO_UlpPadReceiverEnable(spi->io.cs1->pin - 64);
        RSI_EGPIO_SetPinMux(EGPIO1, 0, (spi->io.cs1->pin - 64), EGPIO_PIN_MUX_MODE6);
      }
      RSI_EGPIO_SetPinMux(EGPIO, spi->io.cs1->port, spi->io.cs1->pin, spi->io.cs1->mode);
      if (spi->io.cs1->pad_sel != 0) {
        RSI_EGPIO_PadSelectionEnable(spi->io.cs1->pad_sel);
      }
      RSI_EGPIO_PadReceiverEnable(spi->io.cs1->pin);
      if (spi->io.cs1->pin >= 25 && spi->io.cs1->pin <= 30)
        RSI_EGPIO_HostPadsGpioModeEnable(spi->io.cs1->pin);
    }
    //CS2
    if (spi->io.cs2 != NULL) {
      if (spi->io.cs2->pin > 64) {
        RSI_EGPIO_UlpPadReceiverEnable(spi->io.cs2->pin - 64);
        RSI_EGPIO_SetPinMux(EGPIO1, 0, (spi->io.cs2->pin - 64), EGPIO_PIN_MUX_MODE6);
      }
      RSI_EGPIO_SetPinMux(EGPIO, spi->io.cs2->port, spi->io.cs2->pin, spi->io.cs2->mode);
      if (spi->io.cs2->pad_sel != 0) {
        RSI_EGPIO_PadSelectionEnable(spi->io.cs2->pad_sel);
      }
      RSI_EGPIO_PadReceiverEnable(spi->io.cs2->pin);
      if (spi->io.cs2->pin >= 25 && spi->io.cs2->pin <= 30)
        RSI_EGPIO_HostPadsGpioModeEnable(spi->io.cs2->pin);
    }
    //CS3
    if (spi->io.cs3 != NULL) {
      if (spi->io.cs3->pin > 64) {
        RSI_EGPIO_UlpPadReceiverEnable(spi->io.cs3->pin - 64);
        RSI_EGPIO_SetPinMux(EGPIO1, 0, (spi->io.cs3->pin - 64), EGPIO_PIN_MUX_MODE6);
      }
      RSI_EGPIO_SetPinMux(EGPIO, spi->io.cs3->port, spi->io.cs3->pin, spi->io.cs3->mode);
      if (spi->io.cs3->pad_sel != 0) {
        RSI_EGPIO_PadSelectionEnable(spi->io.cs3->pad_sel);
      }
      RSI_EGPIO_PadReceiverEnable(spi->io.cs3->pin);
      if (spi->io.cs3->pin >= 25 && spi->io.cs3->pin <= 30)
        RSI_EGPIO_HostPadsGpioModeEnable(spi->io.cs3->pin);
    }
    //MOSI
    if (spi->io.mosi->pin > 64) {
      RSI_EGPIO_UlpPadReceiverEnable(spi->io.mosi->pin - 64);
      RSI_EGPIO_SetPinMux(EGPIO1, 0, (spi->io.mosi->pin - 64), EGPIO_PIN_MUX_MODE6);
    }
    RSI_EGPIO_SetPinMux(EGPIO, spi->io.mosi->port, spi->io.mosi->pin, spi->io.mosi->mode);
    if (spi->io.mosi->pad_sel != 0) {
      RSI_EGPIO_PadSelectionEnable(spi->io.mosi->pad_sel);
    }
    RSI_EGPIO_PadReceiverEnable(spi->io.mosi->pin);
    if (spi->io.mosi->pin >= 25 && spi->io.mosi->pin <= 30)
      RSI_EGPIO_HostPadsGpioModeEnable(spi->io.mosi->pin);

    //MISO
    if (spi->io.miso->pin > 64) {
      RSI_EGPIO_UlpPadReceiverEnable(spi->io.miso->pin - 64);
      RSI_EGPIO_SetPinMux(EGPIO1, 0, (spi->io.miso->pin - 64), EGPIO_PIN_MUX_MODE6);
    }
    RSI_EGPIO_SetPinMux(EGPIO, spi->io.miso->port, spi->io.miso->pin, spi->io.miso->mode);
    if (spi->io.miso->pad_sel != 0) {
      RSI_EGPIO_PadSelectionEnable(spi->io.miso->pad_sel);
    }
    RSI_EGPIO_PadReceiverEnable(spi->io.miso->pin);
    if (spi->io.miso->pin >= 25 && spi->io.miso->pin <= 30)
      RSI_EGPIO_HostPadsGpioModeEnable(spi->io.miso->pin);
  }
  if (spi->instance_mode == SPI_ULP_MASTER_MODE) {

    //clock
    RSI_EGPIO_UlpPadReceiverEnable(spi->io.sck->pin);
    RSI_EGPIO_SetPinMux(EGPIO1, spi->io.sck->port, spi->io.sck->pin, spi->io.sck->mode);

    //CS Selection
    if (spi->io.cs0 != NULL) {
      RSI_EGPIO_UlpPadReceiverEnable(spi->io.cs0->pin);
      RSI_EGPIO_SetPinMux(EGPIO1, spi->io.cs0->port, spi->io.cs0->pin, spi->io.cs0->mode);
    }
    if (spi->io.cs1 != NULL) {
      RSI_EGPIO_UlpPadReceiverEnable(spi->io.cs1->pin);
      RSI_EGPIO_SetPinMux(EGPIO1, spi->io.cs1->port, spi->io.cs1->pin, spi->io.cs1->mode);
    }
    if (spi->io.cs2 != NULL) {
      RSI_EGPIO_UlpPadReceiverEnable(spi->io.cs2->pin);
      RSI_EGPIO_SetPinMux(EGPIO1, spi->io.cs2->port, spi->io.cs2->pin, spi->io.cs2->mode);
    }
    if (spi->io.cs3 != NULL) {
      RSI_EGPIO_UlpPadReceiverEnable(spi->io.cs3->pin);
      RSI_EGPIO_SetPinMux(EGPIO1, spi->io.cs3->port, spi->io.cs3->pin, spi->io.cs3->mode);
    }

    //mosi
    RSI_EGPIO_UlpPadReceiverEnable(spi->io.mosi->pin);
    RSI_EGPIO_SetPinMux(EGPIO1, spi->io.mosi->port, spi->io.mosi->pin, spi->io.mosi->mode);

    //miso
    RSI_EGPIO_UlpPadReceiverEnable(spi->io.miso->pin);
    RSI_EGPIO_SetPinMux(EGPIO1, spi->io.miso->port, spi->io.miso->pin, spi->io.miso->mode);
  }
  if ((spi->rx_dma != NULL) || (spi->tx_dma != NULL)) {
    // Enable DMA instance
    // DMA0 used for SSI_MASTER and SSI_SLAVE
    *udmaHandle = UDMAx_Initialize(udma, UDMA_Table, udmaHandle, mem);
  }
  spi->info->state = SPI_INITIALIZED;

  return ARM_DRIVER_OK;
}

/**
 * @fn           int32_t SPI_Uninitialize(const SPI_RESOURCES *spi)
 * @brief        DeInitialize SPI Interface
 * @param[in]    spi       : Pointer to the spi resources
 * @return 		   excecution status 
 */
int32_t SPI_Uninitialize(const SPI_RESOURCES *spi, UDMA_RESOURCES *udma)
{
  spi->reg->SSIENR = SSI_DISABLE;
  // Clear SPI state
  spi->info->state = 0U;

  if ((spi->rx_dma != NULL) || (spi->tx_dma != NULL)) {
    // Diasable DMA instance
    UDMAx_Uninitialize(udma);
  }

  return ARM_DRIVER_OK;
}

/**
 * @fn           int32_t SPI_PowerControl(ARM_POWER_STATE state, const SPI_RESOURCES *spi)
 * @brief        Control SPI Interface Power.
 * @param[in]    state     : Power state
 * @param[in]    spi       : Pointer to the spi resources
 * @return 		   excecution status 
 */
int32_t SPI_PowerControl(ARM_POWER_STATE state, const SPI_RESOURCES *spi)
{
  switch (state) {
    case ARM_POWER_OFF:

      NVIC_DisableIRQ(spi->irq_num); // Disable SPI IRQ in NVIC
      spi->reg->SSIENR = SSI_DISABLE;

      // Reset SPI Run-Time Resources
      spi->info->status.busy       = 0U;
      spi->info->status.data_lost  = 0U;
      spi->info->status.mode_fault = 0U;

      // Clear pending SPI interrupts in NVIC
      NVIC_ClearPendingIRQ(spi->irq_num);

      // Clear transfer information
      memset(spi->xfer, 0, sizeof(SPI_TRANSFER_INFO));

      spi->info->state &= (uint8_t)(~SPI_POWERED); // SPI is not powered

      break;

    case ARM_POWER_FULL:

      if ((spi->info->state & SPI_INITIALIZED) == 0U) {
        return ARM_DRIVER_ERROR;
      }
      if ((spi->info->state & SPI_POWERED) != 0U) {
        return ARM_DRIVER_OK;
      }

      // Connect SPI BASE clock
      /* Static Clock gating is Enabled for M4 SOC Other Clock*/
      if (spi->instance_mode == SPI_MASTER_MODE) {
        RSI_CLK_SsiMstClkConfig(M4CLK, ENABLE_STATIC_CLK, spi->clock.spi_clk_src, spi->clock.divfact);
      } else if (spi->instance_mode == SPI_SLAVE_MODE) {
        RSI_CLK_PeripheralClkEnable(M4CLK, SSISLAVE_CLK, ENABLE_STATIC_CLK);
      } else if (spi->instance_mode == SPI_ULP_MASTER_MODE) {
        RSI_ULPSS_UlpSsiClkConfig(ULPCLK, ENABLE_STATIC_CLK, spi->clock.ulp_spi_clk_src, spi->clock.divfact);
      }

      // Reset SPI Run-Time Resources
      spi->info->status.busy       = 0U;
      spi->info->status.data_lost  = 0U;
      spi->info->status.mode_fault = 0U;
      spi->info->state |= SPI_POWERED; // SPI is powered

      NVIC_ClearPendingIRQ(spi->irq_num);
      NVIC_EnableIRQ(spi->irq_num); // Enable SPI IRQ in NVIC
      break;

    case ARM_POWER_LOW:
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return ARM_DRIVER_OK;
}

/**
 * @fn           int32_t SPI_Send(const void *data, uint32_t num, const SPI_RESOURCES *spi)
 * @brief        Start sending data to SPI transmitter.
 * @param[in]    data     : Pointer to buffer with data to send to SPI transmitter
 * @param[in]    num      : Number of data items to send
 * @param[in]    spi       : Pointer to the spi resources
 * @return 		   excecution status 
 */

int32_t SPI_Send(const void *data,
                 uint32_t num,
                 const SPI_RESOURCES *spi,
                 UDMA_RESOURCES *udma,
                 UDMA_Channel_Info *chnl_info,
                 RSI_UDMA_HANDLE_T udmaHandle)
{
  uint16_t data_width;
  RSI_UDMA_CHA_CONFIG_DATA_T control = { 0 };
  volatile int32_t stat;

  volatile uint32_t dummy_data = 0;

  spi->info->status.busy = 0U;

  if ((data == NULL) || (num == 0U)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  if (!(spi->info->state & SPI_CONFIGURED)) {
    return ARM_DRIVER_ERROR;
  }
  if (spi->info->status.busy) {
    return ARM_DRIVER_ERROR_BUSY;
  }
  spi->info->status.busy       = 1U;
  spi->info->status.data_lost  = 0U;
  spi->info->status.mode_fault = 0U;

  spi->xfer->tx_buf = (uint8_t *)data;
  spi->xfer->rx_buf = NULL;
  spi->xfer->num    = num;
  spi->xfer->rx_cnt = 0U;
  spi->xfer->tx_cnt = 0U;

  spi->reg->CTRLR0_b.TMOD = TRANSMIT_AND_RECEIVE;

  spi->reg->SSIENR = SSI_ENABLE;

  if ((spi->instance_mode == SPI_MASTER_MODE) || (spi->instance_mode == SPI_ULP_MASTER_MODE)) {
    data_width = spi->reg->CTRLR0_b.DFS_32;
  } else {
    data_width = spi->reg->CTRLR0_b.DFS;
  }
  if ((spi->rx_dma != NULL) || (spi->tx_dma != NULL)) {
    if (spi->rx_dma != NULL) {
      control.transferType = UDMA_MODE_BASIC;
      control.nextBurst    = 0;
      if (num < 1024) {
        control.totalNumOfDMATrans = (unsigned int)((num - 1) & 0x03FF);
      } else {
        control.totalNumOfDMATrans = 0x3FF;
      }
      control.rPower      = ARBSIZE_1;
      control.srcProtCtrl = 0x0;
      control.dstProtCtrl = 0x0;
      if (data_width <= (8U - 1U)) {
        // 8-bit data frame
        control.srcSize = SRC_SIZE_8;
        control.srcInc  = SRC_INC_NONE;
        control.dstSize = DST_SIZE_8;
        control.dstInc  = DST_INC_NONE;
      } else if (data_width <= (16U - 1U)) {
        // 16-bit data frame
        control.srcSize = SRC_SIZE_16;
        control.srcInc  = SRC_INC_NONE;
        control.dstSize = DST_SIZE_16;
        control.dstInc  = DST_INC_NONE;
      } else {
        // 32-bit data frame
        control.srcSize = SRC_SIZE_32;
        control.srcInc  = SRC_INC_NONE;
        control.dstSize = DST_SIZE_32;
        control.dstInc  = DST_INC_NONE;
      }
      spi->reg->DMACR_b.RDMAE    = 1;
      spi->reg->DMARDLR_b.DMARDL = 0;
      // Initialize and start SPI RX DMA Stream
      stat = UDMAx_ChannelConfigure(udma,
                                    spi->rx_dma->channel,
                                    (uint32_t) & (spi->reg->DR),
                                    (uint32_t) & (dummy_data),
                                    num,
                                    control,
                                    &spi->rx_dma->chnl_cfg,
                                    spi->rx_dma->cb_event,
                                    chnl_info,
                                    udmaHandle);
      if (stat == -1) {
        return ARM_DRIVER_ERROR;
      }
    }
    if (spi->tx_dma != NULL) {
      control.transferType = UDMA_MODE_BASIC;
      control.nextBurst    = 0;
      if (num < 1024) {
        control.totalNumOfDMATrans = (unsigned int)((num - 1) & 0x03FF);
      } else {
        control.totalNumOfDMATrans = 0x3FF;
      }
      control.rPower      = ARBSIZE_1;
      control.srcProtCtrl = 0x0;
      control.dstProtCtrl = 0x0;
      if (data_width <= (8U - 1U)) {
        // 8-bit data frame
        control.srcSize = SRC_SIZE_8;
        control.srcInc  = SRC_INC_8;
        control.dstSize = DST_SIZE_8;
        control.dstInc  = DST_INC_NONE;
      } else if (data_width <= (16U - 1U)) {
        // 16-bit data frame
        control.srcSize = SRC_SIZE_16;
        control.srcInc  = SRC_INC_16;
        control.dstSize = DST_SIZE_16;
        control.dstInc  = DST_INC_NONE;
      } else {
        // 32-bit data frame
        control.srcSize = SRC_SIZE_32;
        control.srcInc  = SRC_INC_32;
        control.dstSize = DST_SIZE_32;
        control.dstInc  = DST_INC_NONE;
      }
      spi->reg->DMACR_b.TDMAE    = 1;
      spi->reg->DMATDLR_b.DMATDL = 1;
      // Initialize and start SPI TX DMA Stream
      stat = UDMAx_ChannelConfigure(udma,
                                    spi->tx_dma->channel,
                                    (uint32_t)(spi->xfer->tx_buf),
                                    (uint32_t) & (spi->reg->DR),
                                    num,
                                    control,
                                    &spi->tx_dma->chnl_cfg,
                                    spi->tx_dma->cb_event,
                                    chnl_info,
                                    udmaHandle);
      if (stat == -1) {
        return ARM_DRIVER_ERROR;
      }
      UDMAx_ChannelEnable(spi->tx_dma->channel, udma, udmaHandle);
      UDMAx_ChannelEnable(spi->rx_dma->channel, udma, udmaHandle);
      UDMAx_DMAEnable(udma, udmaHandle);
    }
  } else {
    spi->reg->IMR |= (TXEIM | RXFIM);
  }
  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t SPI_Receive( void *data,
                                  uint32_t num,
																	const SPI_RESOURCES *spi )
 * @brief		     Start receiving data from SPI receiver.
 * @param[in]    data        : Pointer to buffer for data to receive from SPI receiver
 * @param[in]    num         : Number of data items to receive
 * @param[in]	   spi        : Pointer to the SPI resources
 * @return 		   excecution status
 */
int32_t SPI_Receive(void *data,
                    uint32_t num,
                    const SPI_RESOURCES *spi,
                    UDMA_RESOURCES *udma,
                    UDMA_Channel_Info *chnl_info,
                    RSI_UDMA_HANDLE_T udmaHandle)
{
  RSI_UDMA_CHA_CONFIG_DATA_T control = { 0 };
  uint16_t data_width;
  volatile int32_t stat;
  uint32_t dummy_data = 0;
  if ((data == NULL) || (num == 0U)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  if (!(spi->info->state & SPI_CONFIGURED)) {
    return ARM_DRIVER_ERROR;
  }
  if (spi->info->status.busy) {
    return ARM_DRIVER_ERROR_BUSY;
  }
  spi->info->status.busy       = 1U;
  spi->info->status.data_lost  = 0U;
  spi->info->status.mode_fault = 0U;

  spi->xfer->rx_buf       = NULL;
  spi->xfer->num          = num;
  spi->xfer->rx_buf       = (uint8_t *)data;
  spi->xfer->rx_cnt       = 0U;
  spi->xfer->tx_cnt       = 0U;
  spi->reg->CTRLR0_b.TMOD = RECEIVE_ONLY;

  spi->reg->SSIENR = SSI_ENABLE;

  if ((spi->instance_mode == SPI_MASTER_MODE) || (spi->instance_mode == SPI_ULP_MASTER_MODE)) {
    data_width = spi->reg->CTRLR0_b.DFS_32;
  } else {
    data_width = spi->reg->CTRLR0_b.DFS;
  }
  if ((spi->rx_dma != NULL) || (spi->tx_dma != NULL)) {
    if (spi->rx_dma != NULL) {
      control.transferType = UDMA_MODE_BASIC;
      control.nextBurst    = 0;
      if (num < 1024) {
        control.totalNumOfDMATrans = (unsigned int)((num - 1) & 0x03FF);
      } else {
        control.totalNumOfDMATrans = 0x3FF;
      }
      control.rPower      = ARBSIZE_1;
      control.srcProtCtrl = 0x0;
      control.dstProtCtrl = 0x0;
      if (data_width <= (8U - 1U)) {
        // 8-bit data frame
        control.srcSize = SRC_SIZE_8;
        control.srcInc  = SRC_INC_NONE;
        control.dstSize = DST_SIZE_8;
        control.dstInc  = DST_INC_8;
      } else if (data_width <= (16U - 1U)) {
        // 16-bit data frame
        control.srcSize = SRC_SIZE_16;
        control.srcInc  = SRC_INC_NONE;
        control.dstSize = DST_SIZE_16;
        control.dstInc  = DST_INC_16;
      } else {
        // 32-bit data frame
        control.srcSize = SRC_SIZE_32;
        control.srcInc  = SRC_INC_NONE;
        control.dstSize = DST_SIZE_32;
        control.dstInc  = DST_INC_32;
      }
      spi->reg->DMACR_b.RDMAE    = 1;
      spi->reg->DMARDLR_b.DMARDL = 0;
      // Initialize and start SPI RX DMA Stream
      stat = UDMAx_ChannelConfigure(udma,
                                    spi->rx_dma->channel,
                                    (uint32_t) & (spi->reg->DR),
                                    (uint32_t)(spi->xfer->rx_buf),
                                    num,
                                    control,
                                    &spi->rx_dma->chnl_cfg,
                                    spi->rx_dma->cb_event,
                                    chnl_info,
                                    udmaHandle);
      if (stat == -1) {
        return ARM_DRIVER_ERROR;
      }
    }
    if (spi->tx_dma != NULL) {
      control.transferType = UDMA_MODE_BASIC;
      control.nextBurst    = 0;
      if (num < 1024) {
        control.totalNumOfDMATrans = (unsigned int)((num - 1) & 0x03FF);
      } else {
        control.totalNumOfDMATrans = 0x3FF;
      }
      control.rPower      = ARBSIZE_1;
      control.srcProtCtrl = 0x0;
      control.dstProtCtrl = 0x0;
      if (data_width <= (8U - 1U)) {
        // 8-bit data frame
        control.srcSize = SRC_SIZE_8;
        control.srcInc  = SRC_INC_8;
        control.dstSize = DST_SIZE_8;
        control.dstInc  = DST_INC_NONE;
      } else if (data_width <= (16U - 1U)) {
        // 16-bit data frame
        control.srcSize = SRC_SIZE_16;
        control.srcInc  = SRC_INC_16;
        control.dstSize = DST_SIZE_16;
        control.dstInc  = DST_INC_NONE;
      } else {
        // 32-bit data frame
        control.srcSize = SRC_SIZE_32;
        control.srcInc  = SRC_INC_32;
        control.dstSize = DST_SIZE_32;
        control.dstInc  = DST_INC_NONE;
      }
      spi->reg->DMACR_b.TDMAE    = 1;
      spi->reg->DMATDLR_b.DMATDL = 1;
      // Initialize and start SPI TX DMA Stream
      stat = UDMAx_ChannelConfigure(udma,
                                    spi->tx_dma->channel,
                                    (uint32_t) & (dummy_data),
                                    (uint32_t) & (spi->reg->DR),
                                    num,
                                    control,
                                    &spi->tx_dma->chnl_cfg,
                                    spi->tx_dma->cb_event,
                                    chnl_info,
                                    udmaHandle);
      if (stat == -1) {
        return ARM_DRIVER_ERROR;
      }
      UDMAx_ChannelEnable(spi->tx_dma->channel, udma, udmaHandle);
      UDMAx_ChannelEnable(spi->rx_dma->channel, udma, udmaHandle);
      UDMAx_DMAEnable(udma, udmaHandle);
    }
  } else {
    // Interrupt mode
    // RX Buffer not empty interrupt enable
    spi->reg->IMR |= (TXEIM | RXFIM);
  }
  return ARM_DRIVER_OK;
}

/**
 * @fn          int32_t SPI_Transfer(const void *data_out,
																				void *data_in,
																				uint32_t num,
																				const SPI_RESOURCES *spi )
 * @brief		     Start sending/receiving data to/from SPI transmitter/receiver.
 * @param[in]    data_out    : Pointer to buffer with data to send to SPI transmitter
 * @param[in]    data_in     : Pointer to buffer for data to receive from SPI receiver
 * @param[in]    num         : Number of data items to transfer
 * @param[in]	   spi         : Pointer to the SPI resources
 * @return 		   excecution status
 */
int32_t SPI_Transfer(const void *data_out,
                     void *data_in,
                     uint32_t num,
                     const SPI_RESOURCES *spi,
                     UDMA_RESOURCES *udma,
                     UDMA_Channel_Info *chnl_info,
                     RSI_UDMA_HANDLE_T udmaHandle)
{
  RSI_UDMA_CHA_CONFIG_DATA_T control = { 0 };
  uint16_t data_width;
  volatile int32_t stat;

  if ((data_out == NULL) || (data_in == NULL) || (num == 0U)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  if (!(spi->info->state & SPI_CONFIGURED)) {
    return ARM_DRIVER_ERROR;
  }
  if (spi->info->status.busy) {
    return ARM_DRIVER_ERROR_BUSY;
  }
  spi->info->status.busy       = 1U;
  spi->info->status.data_lost  = 0U;
  spi->info->status.mode_fault = 0U;

  spi->xfer->rx_buf = (uint8_t *)data_in;
  ;
  spi->xfer->tx_buf = (uint8_t *)data_out;

  spi->xfer->num    = num;
  spi->xfer->rx_cnt = 0U;
  spi->xfer->tx_cnt = 0U;

  spi->reg->CTRLR0_b.TMOD = TRANSMIT_AND_RECEIVE;

  spi->reg->SSIENR = SSI_ENABLE;

  if ((spi->instance_mode == SPI_MASTER_MODE) || (spi->instance_mode == SPI_ULP_MASTER_MODE)) {
    data_width = spi->reg->CTRLR0_b.DFS_32;
  } else {
    data_width = spi->reg->CTRLR0_b.DFS;
  }
  if ((spi->rx_dma != NULL) || (spi->tx_dma != NULL)) {
    if (spi->rx_dma != NULL) {
      control.transferType = UDMA_MODE_BASIC;
      control.nextBurst    = 0;
      if (num < 1024) {
        control.totalNumOfDMATrans = (unsigned int)((num - 1) & 0x03FF);
      } else {
        control.totalNumOfDMATrans = 0x3FF;
      }
      control.rPower      = ARBSIZE_1;
      control.srcProtCtrl = 0x0;
      control.dstProtCtrl = 0x0;
      if (data_width <= (8U - 1U)) {
        // 8-bit data frame
        control.srcSize = SRC_SIZE_8;
        control.srcInc  = SRC_INC_NONE;
        control.dstSize = DST_SIZE_8;
        control.dstInc  = DST_INC_8;
      } else if (data_width <= (16U - 1U)) {
        // 16-bit data frame
        control.srcSize = SRC_SIZE_16;
        control.srcInc  = SRC_INC_NONE;
        control.dstSize = DST_SIZE_16;
        control.dstInc  = DST_INC_16;
      } else {
        // 32-bit data frame
        control.srcSize = SRC_SIZE_32;
        control.srcInc  = SRC_INC_NONE;
        control.dstSize = DST_SIZE_32;
        control.dstInc  = DST_INC_32;
      }
      spi->reg->DMACR_b.RDMAE    = 1;
      spi->reg->DMARDLR_b.DMARDL = 0;
      // Initialize and start SPI TX DMA Stream
      stat = UDMAx_ChannelConfigure(udma,
                                    spi->rx_dma->channel,
                                    (uint32_t) & (spi->reg->DR),
                                    (uint32_t)(spi->xfer->rx_buf),
                                    num,
                                    control,
                                    &spi->rx_dma->chnl_cfg,
                                    spi->rx_dma->cb_event,
                                    chnl_info,
                                    udmaHandle);
      if (stat == -1) {
        return ARM_DRIVER_ERROR;
      }
    }
    if (spi->tx_dma != NULL) {

      control.transferType = UDMA_MODE_BASIC;
      control.nextBurst    = 0;
      if (num < 1024) {
        control.totalNumOfDMATrans = (unsigned int)((num - 1) & 0x03FF);
      } else {
        control.totalNumOfDMATrans = 0x3FF;
      }
      control.rPower      = ARBSIZE_1;
      control.srcProtCtrl = 0x0;
      control.dstProtCtrl = 0x0;
      if (data_width <= (8U - 1U)) {
        // 8-bit data frame
        control.srcSize = SRC_SIZE_8;
        control.srcInc  = SRC_INC_8;
        control.dstSize = DST_SIZE_8;
        control.dstInc  = DST_INC_NONE;
      } else if (data_width <= (16U - 1U)) {
        // 16-bit data frame
        control.srcSize = SRC_SIZE_16;
        control.srcInc  = SRC_INC_16;
        control.dstSize = DST_SIZE_16;
        control.dstInc  = DST_INC_NONE;
      } else {
        // 32-bit data frame
        control.srcSize = SRC_SIZE_32;
        control.srcInc  = SRC_INC_32;
        control.dstSize = DST_SIZE_32;
        control.dstInc  = DST_INC_NONE;
      }
      spi->reg->DMACR_b.TDMAE    = 1;
      spi->reg->DMATDLR_b.DMATDL = 1;
      // Initialize and start SPI TX DMA Stream
      stat = UDMAx_ChannelConfigure(udma,
                                    spi->tx_dma->channel,
                                    (uint32_t)(spi->xfer->tx_buf),
                                    (uint32_t) & (spi->reg->DR),
                                    num,
                                    control,
                                    &spi->tx_dma->chnl_cfg,
                                    spi->tx_dma->cb_event,
                                    chnl_info,
                                    udmaHandle);
      if (stat == -1) {
        return ARM_DRIVER_ERROR;
      }
      UDMAx_ChannelEnable(spi->tx_dma->channel, udma, udmaHandle);
      UDMAx_ChannelEnable(spi->rx_dma->channel, udma, udmaHandle);
      UDMAx_DMAEnable(udma, udmaHandle);
    }
  } else {
    // Interrupt mode
    // Buffer not empty interrupt enable
    spi->reg->IMR |= TXEIM | RXFIM;
  }
  return ARM_DRIVER_OK;
}

/**
 * @fn          uint32_t SPI_GetDataCount(const SPI_RESOURCES *spi)
 * @brief		     Get transferred data count.
 * @param[in]	   SPI        : Pointer to the SPI resources
 * @return 		   number of data items transferred
 */
uint32_t SPI_GetDataCount(const SPI_RESOURCES *spi)
{
  if (!(spi->info->state & SPI_CONFIGURED)) {
    return 0U;
  }

  return spi->xfer->rx_cnt;
}

/**
 * @fn          int32_t SPI_Control(uint32_t control, uint32_t arg, const SPI_RESOURCES *spi,uint8_t spi_slavenumber)
 * @brief		     Control SPI Interface
 * @param[in]    control    : Operation
 * @param[in]    arg        : Argument of operation(optional)
 * @param[in]	   spi        : Pointer to the SPI resources
 * @return 		   excecution status
 */
int32_t SPI_Control(uint32_t control, uint32_t arg, const SPI_RESOURCES *spi, uint32_t base_clock, uint8_t slavenumber)
{
  uint32_t sck_divisor;
  uint32_t data_bits;
  uint32_t val   = 0;
  uint8_t cs_pin = 0;
  volatile uint32_t icr;
  if (!(spi->info->state & SPI_POWERED)) {
    return ARM_DRIVER_ERROR;
  }
  if ((spi->reg == SSISlave) && ((control & ARM_SPI_CONTROL_Msk) == ARM_SPI_MODE_MASTER)) {
    return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  if (((spi->reg == SSI0) || (spi->reg == SSI2)) && ((control & ARM_SPI_CONTROL_Msk) == ARM_SPI_MODE_SLAVE)) {
    return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  if ((control & ARM_SPI_CONTROL_Msk) == ARM_SPI_ABORT_TRANSFER) {
    icr = spi->reg->ICR; // Disable SPI interrupts
    (void)icr;
    memset(spi->xfer, 0, sizeof(SPI_TRANSFER_INFO));
    spi->info->status.busy = 0U;
    return ARM_DRIVER_OK;
  }
  if (spi->info->status.busy) {
    return ARM_DRIVER_ERROR_BUSY;
  }

  spi->reg->SSIENR = SSI_DISABLE;

  switch (control & ARM_SPI_CONTROL_Msk) {
    case ARM_SPI_MODE_INACTIVE:
      spi->info->mode &= ~ARM_SPI_CONTROL_Msk;
      spi->info->mode |= ARM_SPI_MODE_INACTIVE;
      spi->info->state &= (uint8_t)(~SPI_CONFIGURED);
      return ARM_DRIVER_OK;

    case ARM_SPI_MODE_MASTER:
      spi->info->mode &= ~ARM_SPI_CONTROL_Msk;
      spi->info->mode |= ARM_SPI_MODE_MASTER;
      spi->info->state |= SPI_CONFIGURED;
      goto set_speed;

    case ARM_SPI_MODE_SLAVE:
      spi->info->mode &= ~ARM_SPI_CONTROL_Msk;
      spi->info->mode |= ARM_SPI_MODE_SLAVE;
      spi->info->state |= SPI_CONFIGURED;
      break;

    case ARM_SPI_MODE_MASTER_SIMPLEX:
      return ARM_SPI_ERROR_MODE;

    case ARM_SPI_MODE_SLAVE_SIMPLEX:
      return ARM_SPI_ERROR_MODE;

    case ARM_SPI_SET_BUS_SPEED:
set_speed:
      sck_divisor             = (base_clock / arg);
      spi->reg->BAUDR_b.SCKDV = (uint8_t)sck_divisor;
      break;

    case ARM_SPI_GET_BUS_SPEED:
      return (int32_t)(base_clock / spi->reg->BAUDR_b.SCKDV);
    case ARM_SPI_SET_DEFAULT_TX_VALUE: // Set default Transmit value; arg = value
      spi->xfer->def_val = (uint16_t)(arg & 0xFFFF);
      return ARM_DRIVER_OK;

    case ARM_SPI_CONTROL_SS: // Control Slave Select; arg = 0:inactive, 1:active
      if (((spi->info->mode & ARM_SPI_CONTROL_Msk) != ARM_SPI_MODE_MASTER)) {
        return ARM_DRIVER_ERROR;
      }
      val = (spi->info->mode & ARM_SPI_SS_MASTER_MODE_Msk);
      if (arg == ARM_SPI_SS_INACTIVE) {
        if (val == ARM_SPI_SS_MASTER_HW_OUTPUT) {
          if ((spi->instance_mode == SPI_MASTER_MODE) || (spi->instance_mode == SPI_ULP_MASTER_MODE)) {
            /*cs*/
            spi->reg->SER &= ~(BIT(slavenumber));
          }
          if (spi->instance_mode == SPI_SLAVE_MODE) {
            spi->reg->SER = 0 << 0;
          }
        } else {
          switch (slavenumber) {
            case SPI_CS0:
              cs_pin = spi->io.cs0->pin;
              break;
            case SPI_CS1:
              cs_pin = spi->io.cs1->pin;
              break;
            case SPI_CS2:
              cs_pin = spi->io.cs2->pin;
              break;
            case SPI_CS3:
              cs_pin = spi->io.cs3->pin;
              break;
          }
          RSI_EGPIO_SetPin(EGPIO, 0, cs_pin, 1);
        }

      } else { //ARM_SPI_SS_ACTIVE
        if (val == ARM_SPI_SS_MASTER_HW_OUTPUT) {
          if ((spi->instance_mode == SPI_MASTER_MODE) || (spi->instance_mode == SPI_ULP_MASTER_MODE)) {
            spi->reg->SER |= (BIT(slavenumber));
          }
          if (spi->instance_mode == SPI_SLAVE_MODE) {
            spi->reg->SER = 0x1 << 0;
          }
        } else {
          switch (slavenumber) {
            case SPI_CS0:
              cs_pin = spi->io.cs0->pin;
              break;
            case SPI_CS1:
              cs_pin = spi->io.cs1->pin;
              break;
            case SPI_CS2:
              cs_pin = spi->io.cs2->pin;
              break;
            case SPI_CS3:
              cs_pin = spi->io.cs3->pin;
              break;
          }
          RSI_EGPIO_SetPin(EGPIO, 0, cs_pin, 0);
        }
      }
      return ARM_DRIVER_OK;
  }

  if ((spi->info->mode & ARM_SPI_CONTROL_Msk) == ARM_SPI_MODE_MASTER) {
    switch (control & ARM_SPI_SS_MASTER_MODE_Msk) {
      case ARM_SPI_SS_MASTER_UNUSED: // SPI Slave Select when Master: Not used (default)
        spi->reg->SER = 0;
        spi->info->mode &= ~ARM_SPI_SS_MASTER_MODE_Msk;
        spi->info->mode |= ARM_SPI_SS_MASTER_UNUSED;
        break;

      case ARM_SPI_SS_MASTER_HW_INPUT: // SPI Slave Select when Master: Hardware monitored Input
        spi->info->mode &= ~ARM_SPI_SS_MASTER_MODE_Msk;
        return ARM_SPI_ERROR_SS_MODE;

      case ARM_SPI_SS_MASTER_SW: // SPI Slave Select when Master: Software controlled
        spi->info->mode &= ~ARM_SPI_SS_MASTER_MODE_Msk;

        switch (slavenumber) {
          case SPI_CS0:
            cs_pin = spi->io.cs0->pin;
            if (spi->io.cs0->pad_sel != 0) {
              RSI_EGPIO_PadSelectionEnable(spi->io.cs0->pad_sel);
            }
            break;
          case SPI_CS1:
            cs_pin = spi->io.cs1->pin;
            if (spi->io.cs1->pad_sel != 0) {
              RSI_EGPIO_PadSelectionEnable(spi->io.cs1->pad_sel);
            }
            break;
          case SPI_CS2:
            cs_pin = spi->io.cs2->pin;
            if (spi->io.cs2->pad_sel != 0) {
              RSI_EGPIO_PadSelectionEnable(spi->io.cs2->pad_sel);
            }
            break;
          case SPI_CS3:
            cs_pin = spi->io.cs3->pin;
            if (spi->io.cs3->pad_sel != 0) {
              RSI_EGPIO_PadSelectionEnable(spi->io.cs3->pad_sel);
            }
            break;
        }
        if (cs_pin != NULL) {
          if (cs_pin > 64) {
            RSI_EGPIO_SetPinMux(EGPIO1, 0, (cs_pin - 64), EGPIO_PIN_MUX_MODE6);
          }
          RSI_EGPIO_SetPinMux(EGPIO, 0, cs_pin, 0);

          RSI_EGPIO_SetDir(EGPIO, 0, cs_pin, 0);
          RSI_EGPIO_SetPin(EGPIO, 0, cs_pin, 1);
          spi->info->mode |= ARM_SPI_SS_MASTER_SW;
        } else {
          return ARM_SPI_ERROR_SS_MODE;
        }
        break;

      case ARM_SPI_SS_MASTER_HW_OUTPUT: // SPI Slave Select when Master: Hardware controlled Output
        spi->info->mode &= ~ARM_SPI_SS_MASTER_MODE_Msk;
        if ((spi->io.cs0->pin != NULL) || (spi->io.cs1->pin != NULL) || (spi->io.cs2->pin != NULL)
            || (spi->io.cs3->pin != NULL)) {
          spi->info->mode |= ARM_SPI_SS_MASTER_HW_OUTPUT;
        } else {
          return ARM_SPI_ERROR_SS_MODE;
        }
        break;
      default:
        break;
    }
  }
  if ((spi->info->mode & ARM_SPI_CONTROL_Msk) == ARM_SPI_MODE_SLAVE) {
    switch (control & ARM_SPI_SS_SLAVE_MODE_Msk) {
      case ARM_SPI_SS_SLAVE_HW: // SPI Slave Select when Slave: Hardware monitored (default)
        spi->info->mode &= ~ARM_SPI_SS_SLAVE_MODE_Msk;
        if ((spi->io.cs0->pin != NULL) || (spi->io.cs1->pin != NULL) || (spi->io.cs2->pin != NULL)
            || (spi->io.cs3->pin != NULL)) {
          spi->info->mode |= ARM_SPI_SS_SLAVE_HW;
        } else {
          return ARM_SPI_ERROR_SS_MODE;
        }
        break;

      case ARM_SPI_SS_SLAVE_SW: // SPI Slave Select when Slave: Software controlled
        spi->info->mode &= ~ARM_SPI_SS_SLAVE_MODE_Msk;
        return ARM_SPI_ERROR_SS_MODE;
      default:
        break;
    }
  }

  // Frame format:
  switch (control & ARM_SPI_FRAME_FORMAT_Msk) {
    case ARM_SPI_CPOL0_CPHA0:
      spi->reg->CTRLR0_b.SCPOL = 0;
      spi->reg->CTRLR0_b.SCPH  = 0;
      break;

    case ARM_SPI_CPOL0_CPHA1:
      spi->reg->CTRLR0_b.SCPOL = 0;
      spi->reg->CTRLR0_b.SCPH  = 1;
      break;

    case ARM_SPI_CPOL1_CPHA0:
      spi->reg->CTRLR0_b.SCPOL = 1;
      spi->reg->CTRLR0_b.SCPH  = 0;
      break;

    case ARM_SPI_CPOL1_CPHA1:
      spi->reg->CTRLR0_b.SCPOL = 1;
      spi->reg->CTRLR0_b.SCPH  = 1;
      break;

    case ARM_SPI_TI_SSI:
      spi->reg->CTRLR0_b.FRF = TEXAS_INSTRUMENTS_SSP;
      break;

    case ARM_SPI_MICROWIRE:
      spi->reg->CTRLR0_b.FRF = NATIONAL_SEMICONDUCTORS_MICROWIRE;
      break;

    default:
      return ARM_SPI_ERROR_FRAME_FORMAT;
  }

  // Configure Number of Data Bits
  data_bits = ((control & ARM_SPI_DATA_BITS_Msk) >> ARM_SPI_DATA_BITS_Pos);
  if ((spi->instance_mode == 1) || (spi->instance_mode == 3)) {
    if ((data_bits >= 4U) && (data_bits <= 32U)) {
      spi->reg->CTRLR0_b.DFS_32 = (unsigned int)((data_bits - 1U) & 0x1F);
    } else {
      return ARM_SPI_ERROR_DATA_BITS;
    }
  } else {
    if ((data_bits >= 4U) && (data_bits <= 16U)) {
      spi->reg->CTRLR0_b.DFS = (unsigned int)((data_bits - 1U) & 0x0F);
    } else {
      return ARM_SPI_ERROR_DATA_BITS;
    }
  }
  // Configure Bit Order
  if ((control & ARM_SPI_BIT_ORDER_Msk) == ARM_SPI_LSB_MSB) {
    return ARM_SPI_ERROR_BIT_ORDER;
  }

  spi->reg->CTRLR0_b.SRL     = 0;
  spi->reg->CTRLR0_b.SPI_FRF = STANDARD_SPI_FORMAT;
  spi->reg->CTRLR0_b.FRF     = MOTOROLA_SPI;
  spi->reg->TXFTLR_b.TFT     = 0;
  spi->reg->RXFTLR_b.RFT     = 0;

  if ((spi->instance_mode == 1U) || (spi->instance_mode == 3U))
    spi->reg->IMR &= (uint32_t)(~(0x3F)); // Disable SPI interrupts
  else
    spi->reg->IMR &= (uint32_t)(~(0x1F)); // Disable SPI interrupts

  return ARM_DRIVER_OK;
}

/**
 * @fn          ARM_SPI_STATUS SPI_GetStatus(const SPI_RESOURCES *spi)
 * @brief		    Get SPI status.
 * @param[in]	  spi        : Pointer to the SPI resources
 * @return 		  ARM_SPI_STATUS
 */
ARM_SPI_STATUS SPI_GetStatus(const SPI_RESOURCES *spi)
{
  ARM_SPI_STATUS status;

  status.busy       = (unsigned int)(spi->info->status.busy & 0x01);
  status.data_lost  = (unsigned int)(spi->info->status.data_lost & 0x01);
  status.mode_fault = (unsigned int)(spi->info->status.mode_fault & 0x01);

  return status;
}

void SPI_UDMA_Tx_Event(uint32_t event, uint8_t dmaCh, SPI_RESOURCES *spi)
{
  (void)dmaCh;
  switch (event) {
    case UDMA_EVENT_XFER_DONE:
      // Update TX buffer info
      spi->xfer->tx_cnt = spi->xfer->num;
      if (spi->xfer->rx_buf == NULL) {
        if (spi->info->cb_event != NULL) {
          spi->info->cb_event(ARM_SPI_EVENT_TRANSFER_COMPLETE);
        }
      }
      break;
    case UDMA_EVENT_ERROR:
      break;
  }
}

void SPI_UDMA_Rx_Event(uint32_t event, uint8_t dmaCh, SPI_RESOURCES *spi)
{
  (void)dmaCh;
  switch (event) {
    case UDMA_EVENT_XFER_DONE:
      //spi->xfer->rx_cnt    = spi->xfer->num;
      spi->info->status.busy = 0U;
      break;
    case UDMA_EVENT_ERROR:
      break;
  }
  if (spi->info->cb_event != NULL) {
    spi->info->cb_event(ARM_SPI_EVENT_TRANSFER_COMPLETE);
  }
}

/**
 * @fn          void SPI_IRQHandler(const SPI_RESOURCES *spi)
 * @brief		    SPI IRQ Handler
 * @param[in]	  spi        : Pointer to the SPI resources
 * @return 		  none
 */
void SPI_IRQHandler(const SPI_RESOURCES *spi)
{
  uint8_t data_8bit, i;
  uint32_t data_16bit;
  uint32_t data_32bit;
  uint32_t event;
  uint16_t data_width;
  uint32_t isr;
  uint32_t sr;
  volatile uint32_t ovrrunclr;
  volatile uint32_t icr;
  event = 0U;

  isr = spi->reg->ISR;
  sr  = spi->reg->SR;
  icr = spi->reg->ICR;
  (void)icr;

  if ((spi->instance_mode == SPI_MASTER_MODE) || (spi->instance_mode == SPI_ULP_MASTER_MODE)) {
    data_width = spi->reg->CTRLR0_b.DFS_32;
  } else {
    data_width = spi->reg->CTRLR0_b.DFS;
  }
  if ((isr & BIT(1)) || (isr & BIT(3))) { // Read overrun
    // Overrun flag is set
    // Clear Overrun flag
    ovrrunclr = spi->reg->TXOICR;
    ovrrunclr = spi->reg->RXOICR;
    (void)ovrrunclr;
    spi->info->status.data_lost = 1U;
    event |= ARM_SPI_EVENT_DATA_LOST;
  }
  if ((((sr & BIT(3))) != 0U) && (((isr & BIT(4))) != 0U)) {
    // Receive Buffer Not Empty
    if (spi->xfer->rx_cnt < spi->xfer->num) {
      if (spi->xfer->rx_buf) {
        if (data_width <= (8U - 1U)) {
          // 8-bit data frame
          data_8bit              = *(volatile uint8_t *)(&spi->reg->DR);
          *(spi->xfer->rx_buf++) = data_8bit;
        } else if (data_width <= (16U - 1U)) {
          // 16-bit data frame
          data_16bit             = *(volatile uint32_t *)(&spi->reg->DR);
          *(spi->xfer->rx_buf++) = (uint8_t)data_16bit;
          *(spi->xfer->rx_buf++) = (uint8_t)(data_16bit >> 8U);
        } else {
          // 32-bit data frame
          data_32bit             = *(volatile uint32_t *)(&spi->reg->DR);
          *(spi->xfer->rx_buf++) = (uint8_t)data_32bit;
          *(spi->xfer->rx_buf++) = (uint8_t)(data_32bit >> 8U);
          *(spi->xfer->rx_buf++) = (uint8_t)(data_32bit >> 16U);
          *(spi->xfer->rx_buf++) = (uint8_t)(data_32bit >> 24U);
        }
      }
      spi->xfer->rx_cnt++;
      if (spi->xfer->rx_cnt == spi->xfer->num) {
        // Disable RX Buffer Not Empty Interrupt
        spi->reg->IMR &= (uint32_t)(~RXFIM);

        // Clear busy flag
        spi->info->status.busy = 0U;

        // Transfer completed
        event |= ARM_SPI_EVENT_TRANSFER_COMPLETE;
      }
    } else {
      // Unexpected transfer, data lost
      event |= ARM_SPI_EVENT_DATA_LOST;
    }
  }
  if ((((sr & BIT(2))) != 0U) && (((isr & BIT(0))) != 0U)) {
    if (spi->xfer->tx_cnt < spi->xfer->num) {
      if (data_width <= (8U - 1U)) {
        if (spi->xfer->tx_buf) {
          data_8bit                            = *(spi->xfer->tx_buf++);
          *(volatile uint8_t *)(&spi->reg->DR) = data_8bit;
        } else {
          *(volatile uint8_t *)(&spi->reg->DR) = 0x0; //dummy data
        }
      } else if (data_width <= (16U - 1U)) {
        if (spi->xfer->tx_buf != NULL) {
          data_16bit = *(spi->xfer->tx_buf++);
          data_16bit |= (uint16_t)(*(spi->xfer->tx_buf++) << 8U);
          *(volatile uint32_t *)(&spi->reg->DR) = data_16bit;
        } else {
          *(volatile uint32_t *)(&spi->reg->DR) = 0x0; //dummy data
        }
      } else {
        if (spi->xfer->tx_buf != NULL) {
          data_32bit = *(spi->xfer->tx_buf++);
          data_32bit |= *(spi->xfer->tx_buf++) << 8U;
          data_32bit |= *(spi->xfer->tx_buf++) << 16U;
          data_32bit |= *(spi->xfer->tx_buf++) << 24U;
          *(volatile uint32_t *)(&spi->reg->DR) = data_32bit;
        } else {
          *(volatile uint32_t *)(&spi->reg->DR) = 0x0; //dummy data
        }
      }
      do {
        sr = spi->reg->SR;
      } while (sr & BIT(0));

      for (i = 0; i < 1; i++)
        ;
      spi->xfer->tx_cnt++;
      if (spi->xfer->tx_cnt == spi->xfer->num) {
        // All data sent, disable TX Buffer Empty Interrupt
        spi->reg->IMR &= (uint32_t)(~TXEIM);
      }
    } else {
      // Unexpected transfer, data lost
      event |= ARM_SPI_EVENT_DATA_LOST;
    }
  }

  // Send event
  if ((event != 0U) && ((spi->info->cb_event != NULL))) {
    spi->info->cb_event(event);
  }
}
#endif
