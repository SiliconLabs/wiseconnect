/*************************************************************************
 *
 * Copyright (c) 2019 Redpine Signals Incorporated. All Rights Reserved.
 *
 * NOTICE:  All  information  contained  herein is, and  remains  the  property of
 * Redpine Signals Incorporated. The intellectual and technical concepts contained
 * herein  are  proprietary to  Redpine Signals Incorporated and may be covered by
 * U.S. and Foreign Patents, patents in process, and are protected by trade secret
 * or copyright law. Dissemination  of this  information or reproduction  of  this
 * material is strictly forbidden unless prior written permission is obtained from
 * Redpine Signals Incorporated.
 */
/**
 * Includes
 */
#include "em_usart.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "cmsis_os2.h"
#include "sl_board_configuration.h"
#include "dmadrv.h"
#include "sl_status.h"
#include "sl_constants.h"

unsigned int rx_ldma_channel;
unsigned int tx_ldma_channel;

static uint32_t dummy_buffer;

static bool dma_callback(unsigned int channel, unsigned int sequenceNo, void *userParam);

// LDMA descriptor and transfer configuration structures for USART TX channel
LDMA_Descriptor_t ldmaTXDescriptor;
LDMA_TransferCfg_t ldmaTXConfig;

// LDMA descriptor and transfer configuration structures for USART RX channel
LDMA_Descriptor_t ldmaRXDescriptor;
LDMA_TransferCfg_t ldmaRXConfig;

static osSemaphoreId_t transfer_done_semaphore = NULL;

void sl_si91x_host_enable_high_speed_bus()
{
  USART2->CTRL_SET |= USART_CTRL_SMSDELAY | USART_CTRL_SSSEARLY;
  USART_BaudrateSyncSet(USART2, 0, 20000000);
}

void sl_si91x_host_gpio_init(void)
{
  // Enable clock (not needed on xG21)
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure SPI bus pins
  GPIO_PinModeSet(SPI_MISO_PIN.port, SPI_MISO_PIN.pin, gpioModeInput, 0);
  GPIO_PinModeSet(SPI_MOSI_PIN.port, SPI_MOSI_PIN.pin, gpioModePushPull, 0);
  GPIO_PinModeSet(SPI_CLOCK_PIN.port, SPI_CLOCK_PIN.pin, gpioModePushPullAlternate, 0);
  GPIO_PinModeSet(SPI_CS_PIN.port, SPI_CS_PIN.pin, gpioModePushPull, 1);
}

void sl_si91x_host_usart2_init(void)
{
  // Enable clock (not needed on xG21)
  CMU_ClockEnable(cmuClock_USART2, true);

  // Default asynchronous initializer (master mode, 1 Mbps, 8-bit data)
  USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;

  init.msbf         = true; // MSB first transmission for SPI compatibility
  init.autoCsEnable = true; // Allow the USART to assert CS
  init.baudrate     = 12500000;
  /*
	 * Route USART0 RX, TX, and CLK to the specified pins.  Note that CS is
	 * not controlled by USART0 so there is no write to the corresponding
	 * USARTROUTE register to do this.
	 */
  GPIO->USARTROUTE[2].RXROUTE = (SPI_MISO_PIN.port << _GPIO_USART_RXROUTE_PORT_SHIFT)
                                | (SPI_MISO_PIN.pin << _GPIO_USART_RXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[2].TXROUTE = (SPI_MOSI_PIN.port << _GPIO_USART_TXROUTE_PORT_SHIFT)
                                | (SPI_MOSI_PIN.pin << _GPIO_USART_TXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[2].CLKROUTE = (SPI_CLOCK_PIN.port << _GPIO_USART_CLKROUTE_PORT_SHIFT)
                                 | (SPI_CLOCK_PIN.pin << _GPIO_USART_CLKROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[2].CSROUTE = (SPI_CS_PIN.port << _GPIO_USART_CSROUTE_PORT_SHIFT)
                                | (SPI_CS_PIN.pin << _GPIO_USART_CSROUTE_PIN_SHIFT);

  // Enable USART interface pins
  GPIO->USARTROUTE[2].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN | // MISO
                                GPIO_USART_ROUTEEN_TXPEN | // MOSI
                                GPIO_USART_ROUTEEN_CLKPEN | GPIO_USART_ROUTEEN_CSPEN;

  // Set slew rate for alternate usage pins
  GPIO_SlewrateSet(SPI_CLOCK_PIN.port, 7, 7);

  // Configure and enable USART0
  USART_InitSync(USART2, &init);
}

sl_status_t sl_si91x_host_bus_init(void)
{
  sl_si91x_host_gpio_init();
  sl_si91x_host_usart2_init();

  USART2->TIMING |= /*USART_TIMING_TXDELAY_ONE | USART_TIMING_CSSETUP_ONE |*/ USART_TIMING_CSHOLD_ONE;

  //USART2->CTRL_SET |= USART_CTRL_SMSDELAY;
  if (NULL == transfer_done_semaphore) {
    transfer_done_semaphore = osSemaphoreNew(1, 0, NULL);
  }

  DMADRV_Init();
  DMADRV_AllocateChannel(&rx_ldma_channel, NULL);
  DMADRV_AllocateChannel(&tx_ldma_channel, NULL);
  return SL_STATUS_OK;
}

/*==================================================================*/
/**
 * @fn         sl_status_t sl_si91x_host_spi_transfer(const void *tx_buffer, void *rx_buffer, uint16_t buffer_length)
 * @param[in]  uint8_t *tx_buff, pointer to the buffer with the data to be transferred
 * @param[in]  uint8_t *rx_buff, pointer to the buffer to store the data received
 * @param[in]  uint16_t transfer_length, Number of bytes to send and receive
 * @param[in]  uint8_t mode, To indicate mode 8 BIT/32 BIT mode transfers.
 * @param[out] None
 * @return     0, 0=success
 * @section description
 * This API is used to transfer/receive data to the Wi-Fi module through the SPI interface.
 */
sl_status_t sl_si91x_host_spi_transfer(const void *tx_buffer, void *rx_buffer, uint16_t buffer_length)
{
  if (buffer_length < 16) {
    uint8_t *tx = (tx_buffer != NULL) ? (uint8_t *)tx_buffer : (uint8_t *)&dummy_buffer;
    uint8_t *rx = (rx_buffer != NULL) ? (uint8_t *)rx_buffer : (uint8_t *)&dummy_buffer;
    while (buffer_length > 0) {
      while (!(USART2->STATUS & USART_STATUS_TXBL)) {
      }
      USART2->TXDATA = (uint32_t)*tx;
      while (!(USART2->STATUS & USART_STATUS_TXC)) {
      }
      *rx = (uint8_t)USART2->RXDATA;
      if (tx_buffer != NULL) {
        tx++;
      }
      if (rx_buffer != NULL) {
        rx++;
      }
      buffer_length--;
    }
  } else {
    if (tx_buffer == NULL) {
      dummy_buffer = 0;
      ldmaTXDescriptor =
        (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_P2P_BYTE(&dummy_buffer, &(USART2->TXDATA), buffer_length);
    } else {
      ldmaTXDescriptor =
        (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(tx_buffer, &(USART2->TXDATA), buffer_length);
    }

    if (rx_buffer == NULL) {
      ldmaRXDescriptor =
        (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_P2P_BYTE(&(USART2->RXDATA), &dummy_buffer, buffer_length);
    } else {
      ldmaRXDescriptor =
        (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_P2M_BYTE(&(USART2->RXDATA), rx_buffer, buffer_length);
    }

    // Transfer a byte on free space in the USART buffer
    ldmaTXConfig = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_USART2_TXBL);

    // Transfer a byte on receive data valid
    ldmaRXConfig = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_USART2_RXDATAV);

    // Start both channels
    DMADRV_LdmaStartTransfer(rx_ldma_channel, &ldmaRXConfig, &ldmaRXDescriptor, dma_callback, NULL);
    DMADRV_LdmaStartTransfer(tx_ldma_channel, &ldmaTXConfig, &ldmaTXDescriptor, NULL, NULL);

    if (osSemaphoreAcquire(transfer_done_semaphore, 1000) != osOK) {
      __asm__("bkpt");
    }
  }
  return SL_STATUS_OK;
}

static bool dma_callback(unsigned int channel, unsigned int sequenceNo, void *userParam)
{
  UNUSED_PARAMETER(channel);
  UNUSED_PARAMETER(sequenceNo);
  UNUSED_PARAMETER(userParam);
  osSemaphoreRelease(transfer_done_semaphore);
  return false;
}
