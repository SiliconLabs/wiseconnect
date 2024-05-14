/*******************************************************************************
* @file  efx32_ncp_host.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_wifi_constants.h"
#include "sl_si91x_host_interface.h"
#include "sl_board_configuration.h"
#include "sl_status.h"
#include "em_usart.h"
#include "em_cmu.h"
#include "em_core.h"
#include "em_gpio.h"
#include "cmsis_os2.h"
#include "dmadrv.h"
#include "gpiointerrupt.h"
#include "sl_si91x_status.h"
#include "sl_rsi_utility.h"
#include "sl_constants.h"
#include <stdbool.h>
#include <string.h>
#include "FreeRTOS.h"

#define LDMA_MAX_TRANSFER_LENGTH     4096
#define LDMA_DESCRIPTOR_ARRAY_LENGTH (LDMA_MAX_TRANSFER_LENGTH / 2048)

static bool dma_callback(unsigned int channel, unsigned int sequenceNo, void *userParam);

unsigned int rx_ldma_channel;
unsigned int tx_ldma_channel;
osMutexId_t ncp_transfer_mutex = 0;

static uint32_t dummy_buffer;
static sl_si91x_host_init_configuration init_config = { 0 };

uint8_t dummy_buffer_test[2500];

// LDMA descriptor and transfer configuration structures for USART TX channel
LDMA_Descriptor_t ldmaTXDescriptor[LDMA_DESCRIPTOR_ARRAY_LENGTH];
LDMA_TransferCfg_t ldmaTXConfig;

// LDMA descriptor and transfer configuration structures for USART RX channel
LDMA_Descriptor_t ldmaRXDescriptor[LDMA_DESCRIPTOR_ARRAY_LENGTH];
LDMA_TransferCfg_t ldmaRXConfig;

static osSemaphoreId_t transfer_done_semaphore = NULL;

static bool dma_callback(unsigned int channel, unsigned int sequenceNo, void *userParam)
{
  UNUSED_PARAMETER(channel);
  UNUSED_PARAMETER(sequenceNo);
  UNUSED_PARAMETER(userParam);
  osSemaphoreRelease(transfer_done_semaphore);
  return false;
}

#ifdef SL_NCP_UART_INTERFACE
void NCP_UART_RX_IRQ_HANDLER(void)
{
  NVIC_DisableIRQ(NCP_RX_IRQ);

  if (NULL != init_config.rx_irq) {
    if (SL_STATUS_IN_PROGRESS == init_config.rx_irq()) {
      return;
    }
  }

  NVIC_ClearPendingIRQ(NCP_RX_IRQ);
  NVIC_EnableIRQ(NCP_RX_IRQ);

  return;
}

static void efx32_ncp_uart_init(void)
{
  USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT;
  init.baudrate                = 115200;

  // Disable the VCOM so that USART Can be used for NCP connection
  GPIO_PinModeSet(VCOM_EN_PIN.port, VCOM_EN_PIN.pin, gpioModePushPull, 0);
  GPIO_PinOutClear(VCOM_EN_PIN.port, VCOM_EN_PIN.pin);

  // Enable oscillator to NCP USART module
  CMU_ClockEnable(NCP_USART_CMU_CLOCK, true);

  // UART Rx
  GPIO_PinModeSet(NCP_UART_RX_PIN.port, NCP_UART_RX_PIN.pin, gpioModeWiredAndPullUp, 0);
  // UART Tx
  GPIO_PinModeSet(NCP_UART_TX_PIN.port, NCP_UART_TX_PIN.pin, gpioModePushPull, 1);

  // Initialize UART asynchronous mode and route pins
  USART_InitAsync(NCP_USART, &init);

  GPIO->USARTROUTE[NCP_USART_ROUTE_INDEX].RXROUTE = (NCP_UART_RX_PIN.port << _GPIO_USART_RXROUTE_PORT_SHIFT)
                                                    | (NCP_UART_RX_PIN.pin << _GPIO_USART_RXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[NCP_USART_ROUTE_INDEX].TXROUTE = (NCP_UART_TX_PIN.port << _GPIO_USART_TXROUTE_PORT_SHIFT)
                                                    | (NCP_UART_TX_PIN.pin << _GPIO_USART_TXROUTE_PIN_SHIFT);

  // Enable USART interface pins
  GPIO->USARTROUTE[NCP_USART_ROUTE_INDEX].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN | GPIO_USART_ROUTEEN_TXPEN;

  // Clear interrupt
  NVIC_ClearPendingIRQ(NCP_RX_IRQ);

  // Enable receive data valid interrupt
  USART_IntEnable(NCP_USART, USART_IEN_RXDATAV);
}

#else
static void gpio_interrupt(uint8_t interrupt_number)
{
  UNUSED_PARAMETER(interrupt_number);

  if (NULL != init_config.rx_irq) {
    init_config.rx_irq();
  }

  return;
}

static void efx32_spi_init(void)
{
  // Default asynchronous initializer (master mode, 1 Mbps, 8-bit data)
  USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;

  init.msbf         = true; // MSB first transmission for SPI compatibility
  init.autoCsEnable = true; // Allow the USART to assert CS
  init.baudrate     = 12500000;

  // Configure SPI bus pins
  GPIO_PinModeSet(SPI_MISO_PIN.port, SPI_MISO_PIN.pin, gpioModeInput, 0);
  GPIO_PinModeSet(SPI_MOSI_PIN.port, SPI_MOSI_PIN.pin, gpioModePushPull, 0);
  GPIO_PinModeSet(SPI_CLOCK_PIN.port, SPI_CLOCK_PIN.pin, gpioModePushPullAlternate, 0);
  GPIO_PinModeSet(SPI_CS_PIN.port, SPI_CS_PIN.pin, gpioModePushPull, 1);
  // Enable clock (not needed on xG21)
  CMU_ClockEnable(SPI_USART_CMU_CLOCK, true);

  /*
   * Route USART RX, TX, and CLK to the specified pins.  Note that CS is
   * not controlled by USART so there is no write to the corresponding
   * USARTROUTE register to do this.
   */
  GPIO->USARTROUTE[SPI_USART_ROUTE_INDEX].RXROUTE = (SPI_MISO_PIN.port << _GPIO_USART_RXROUTE_PORT_SHIFT)
                                                    | (SPI_MISO_PIN.pin << _GPIO_USART_RXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[SPI_USART_ROUTE_INDEX].TXROUTE = (SPI_MOSI_PIN.port << _GPIO_USART_TXROUTE_PORT_SHIFT)
                                                    | (SPI_MOSI_PIN.pin << _GPIO_USART_TXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[SPI_USART_ROUTE_INDEX].CLKROUTE = (SPI_CLOCK_PIN.port << _GPIO_USART_CLKROUTE_PORT_SHIFT)
                                                     | (SPI_CLOCK_PIN.pin << _GPIO_USART_CLKROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[SPI_USART_ROUTE_INDEX].CSROUTE = (SPI_CS_PIN.port << _GPIO_USART_CSROUTE_PORT_SHIFT)
                                                    | (SPI_CS_PIN.pin << _GPIO_USART_CSROUTE_PIN_SHIFT);

  // Enable USART interface pins
  GPIO->USARTROUTE[SPI_USART_ROUTE_INDEX].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN | // MISO
                                                    GPIO_USART_ROUTEEN_TXPEN | // MOSI
                                                    GPIO_USART_ROUTEEN_CLKPEN | GPIO_USART_ROUTEEN_CSPEN;

  // Set slew rate for alternate usage pins
  GPIO_SlewrateSet(SPI_CLOCK_PIN.port, 7, 7);

  // Configure and enable USART
  USART_InitSync(SPI_USART, &init);

  SPI_USART->TIMING |= /*USART_TIMING_TXDELAY_ONE | USART_TIMING_CSSETUP_ONE |*/ USART_TIMING_CSHOLD_ONE;

  //SPI_USART->CTRL_SET |= USART_CTRL_SMSDELAY;

  // configure packet pending interrupt priority
  NVIC_SetPriority(NCP_RX_IRQ, PACKET_PENDING_INT_PRI);
  GPIOINT_CallbackRegister(INTERRUPT_PIN.pin, gpio_interrupt);
  GPIO_PinModeSet(INTERRUPT_PIN.port, INTERRUPT_PIN.pin, gpioModeInputPullFilter, 0);
  GPIO_ExtIntConfig(INTERRUPT_PIN.port, INTERRUPT_PIN.pin, INTERRUPT_PIN.pin, true, false, true);

  return;
}
#endif

void sl_si91x_host_set_sleep_indicator(void)
{
  GPIO_PinOutSet(SLEEP_CONFIRM_PIN.port, SLEEP_CONFIRM_PIN.pin);
}

void sl_si91x_host_clear_sleep_indicator(void)
{
  GPIO_PinOutClear(SLEEP_CONFIRM_PIN.port, SLEEP_CONFIRM_PIN.pin);
}

uint32_t sl_si91x_host_get_wake_indicator(void)
{
  return GPIO_PinInGet(WAKE_INDICATOR_PIN.port, WAKE_INDICATOR_PIN.pin);
}

sl_status_t sl_si91x_host_init(sl_si91x_host_init_configuration *config)
{
  init_config.rx_irq  = config->rx_irq;
  init_config.rx_done = config->rx_done;

  // Enable clock (not needed on xG21)
  CMU_ClockEnable(cmuClock_GPIO, true);

  if (transfer_done_semaphore == NULL) {
    transfer_done_semaphore = osSemaphoreNew(1, 0, NULL);
  }

  if (ncp_transfer_mutex == 0) {
    ncp_transfer_mutex = osMutexNew(NULL);
  }

#ifdef SL_NCP_UART_INTERFACE
  efx32_ncp_uart_init();
#else
  efx32_spi_init();
#endif

  // Start reset line low
  GPIO_PinModeSet(RESET_PIN.port, RESET_PIN.pin, gpioModePushPull, 0);

  // Configure interrupt, sleep and wake confirmation pins
  GPIO_PinModeSet(SLEEP_CONFIRM_PIN.port, SLEEP_CONFIRM_PIN.pin, gpioModeWiredOrPullDown, 1);
  GPIO_PinModeSet(WAKE_INDICATOR_PIN.port, WAKE_INDICATOR_PIN.pin, gpioModeWiredOrPullDown, 0);

  DMADRV_Init();
  DMADRV_AllocateChannel(&rx_ldma_channel, NULL);
  DMADRV_AllocateChannel(&tx_ldma_channel, NULL);

  return SL_STATUS_OK;
}

sl_status_t sl_si91x_host_deinit(void)
{
  return SL_STATUS_OK;
}

void sl_si91x_host_enable_high_speed_bus()
{
#ifdef SL_NCP_UART_INTERFACE
  USART_BaudrateAsyncSet(NCP_USART, 0, 921600, usartOVS16);
#else
  //SPI_USART->CTRL_SET |= USART_CTRL_SMSDELAY | USART_CTRL_SSSEARLY;
#endif

  return;
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
  int i;
  osMutexAcquire(ncp_transfer_mutex, 0xFFFFFFFFUL);

  if (buffer_length < 16) {
    uint8_t *tx = (tx_buffer != NULL) ? (uint8_t *)tx_buffer : (uint8_t *)&dummy_buffer;
    uint8_t *rx = (rx_buffer != NULL) ? (uint8_t *)rx_buffer : (uint8_t *)&dummy_buffer;
    while (buffer_length > 0) {
      while (!(SPI_USART->STATUS & USART_STATUS_TXBL)) {
      }
      SPI_USART->TXDATA = (uint32_t)*tx;
      while (!(SPI_USART->STATUS & USART_STATUS_TXC)) {
      }
      *rx = (uint8_t)SPI_USART->RXDATA;
      if (tx_buffer != NULL) {
        tx++;
      }
      if (rx_buffer != NULL) {
        rx++;
      }
      buffer_length--;
    }
  } else {
    if (buffer_length <= 2048) {
      if (tx_buffer == NULL) {
        dummy_buffer = 0;
        ldmaTXDescriptor[0] =
          (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_P2P_BYTE(&dummy_buffer, &(SPI_USART->TXDATA), buffer_length);
      } else {
        ldmaTXDescriptor[0] =
          (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(tx_buffer, &(SPI_USART->TXDATA), buffer_length);
      }
      if (rx_buffer == NULL) {
        ldmaRXDescriptor[0] =
          (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_P2P_BYTE(&(SPI_USART->RXDATA), &dummy_buffer, buffer_length);
      } else {
        ldmaRXDescriptor[0] =
          (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_P2M_BYTE(&(SPI_USART->RXDATA), rx_buffer, buffer_length);
      }
    }

    else {
      if (tx_buffer == NULL) {
        tx_buffer = (uint8_t *)&dummy_buffer_test;
      } else if (rx_buffer == NULL) {
        rx_buffer = (uint8_t *)&dummy_buffer_test;
      }
      //Transfer length is more than 2048 bytes. Initialize multiple LDMA Tx descriptor.
      for (i = 0; i < (LDMA_DESCRIPTOR_ARRAY_LENGTH - 1); i++) {
        ldmaRXDescriptor[i] =
          (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&(SPI_USART->RXDATA), (rx_buffer + (2048 * i)), 2048, 1);
        ldmaTXDescriptor[i] =
          (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2P_BYTE((tx_buffer + (2048 * i)), &(SPI_USART->TXDATA), 2048, 1);
      }
      ldmaRXDescriptor[i] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_P2M_BYTE(&(SPI_USART->RXDATA),
                                                                               (rx_buffer + (2048 * i)),
                                                                               (buffer_length - (2048 * i)));
      ldmaTXDescriptor[i] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_M2P_BYTE((tx_buffer + (2048 * i)),
                                                                               &(SPI_USART->TXDATA),
                                                                               (buffer_length - (2048 * i)));
    }
    // Transfer a byte on free space in the USART buffer
    ldmaTXConfig = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(SPI_USART_LDMA_TX);

    // Transfer a byte on receive data valid
    ldmaRXConfig = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(SPI_USART_LDMA_RX);

    // Start both channels
    DMADRV_LdmaStartTransfer(rx_ldma_channel, &ldmaRXConfig, ldmaRXDescriptor, dma_callback, NULL);
    DMADRV_LdmaStartTransfer(tx_ldma_channel, &ldmaTXConfig, ldmaTXDescriptor, NULL, NULL);

    if (osSemaphoreAcquire(transfer_done_semaphore, 1000) != osOK) {
      BREAKPOINT();
    }
  }

  osMutexRelease(ncp_transfer_mutex);
  return SL_STATUS_OK;
}

sl_status_t sl_si91x_host_uart_transfer(const void *tx_buffer, void *rx_buffer, uint16_t buffer_length)
{
  uint16_t i      = 0;
  uint8_t *buffer = NULL;

  osMutexAcquire(ncp_transfer_mutex, 0xFFFFFFFFUL);
  if (NULL != tx_buffer) {
    buffer = (uint8_t *)tx_buffer;
    for (i = 0; i < buffer_length; i++) {
      USART_Tx(NCP_USART, buffer[i]);
    }
  }

  if (NULL != rx_buffer) {
    buffer = (uint8_t *)rx_buffer;
    for (i = 0; i < buffer_length; i++) {
      buffer[i] = USART_Rx(NCP_USART);
    }
  }

  osMutexRelease(ncp_transfer_mutex);
  return SL_STATUS_OK;
}

void sl_si91x_host_flush_uart_rx(void)
{
  uint8_t data       = 0;
  uint32_t timestamp = 0;

  while (1) {
    timestamp = osKernelGetTickCount();
    while (!(NCP_USART->STATUS & USART_STATUS_RXDATAV)) {
      if ((osKernelGetTickCount() - timestamp) > 2000) {
        return;
      }
    }
    data = (uint8_t)NCP_USART->RXDATA;
    data ^= data;
  }

  return;
}

void sl_si91x_host_hold_in_reset(void)
{
  GPIO_PinModeSet(RESET_PIN.port, RESET_PIN.pin, gpioModePushPull, 1);
  GPIO_PinOutClear(RESET_PIN.port, RESET_PIN.pin);
}

void sl_si91x_host_release_from_reset(void)
{
  GPIO_PinModeSet(RESET_PIN.port, RESET_PIN.pin, gpioModeWiredOrPullDown, 1);
}

void sl_si91x_host_enable_bus_interrupt(void)
{
  NVIC_ClearPendingIRQ(NCP_RX_IRQ);
  NVIC_EnableIRQ(NCP_RX_IRQ);
}

void sl_si91x_host_disable_bus_interrupt(void)
{
  NVIC_DisableIRQ(NCP_RX_IRQ);
}

bool sl_si91x_host_is_in_irq_context(void)
{
  return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0U;
}
