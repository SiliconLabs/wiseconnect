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

#ifndef SL_SI91X_NCP_UART_BAUDRATE
// ToDo: This Macro is depricated and should be removed in upcoming releases.
//       Keeping this macro functionality intact due to backward compatability.
#if SL_SI91X_UART_HIGH_SPEED_ENABLE == 1
#define SL_SI91X_NCP_UART_BAUDRATE 921600
#else
#define SL_SI91X_NCP_UART_BAUDRATE 115200
#endif
#endif

#ifndef SL_NCP_UART_INTERFACE

#include "spidrv.h"
#include "sl_spidrv_instances.h"
#include "sl_spidrv_exp_config.h"
#include "si91x_ncp_spi_config.h"

#else

#include "si91x_ncp_uart_config.h"
#include "sl_uartdrv_instances.h"
#include "sl_uartdrv_usart_exp_config.h"

#endif

#ifndef SL_NCP_UART_INTERFACE
// use SPI handle for EXP header (configured in project settings)
extern SPIDRV_Handle_t sl_spidrv_exp_handle;
#define SPI_HANDLE sl_spidrv_exp_handle
static uint8_t dummy_buffer[1800] = { 0 };

#else

#define UART_HANDLE SL_UARTDRV_USART_EXP_PERIPHERAL
static UARTDRV_Handle_t uartdrv_handle = NULL;
static bool ncp_initialized            = false;

#endif

unsigned int rx_ldma_channel;
unsigned int tx_ldma_channel;

// LDMA descriptor and transfer configuration structures for USART TX channel
LDMA_Descriptor_t ldmaTXDescriptor[LDMA_DESCRIPTOR_ARRAY_LENGTH];
LDMA_TransferCfg_t ldmaTXConfig;

// LDMA descriptor and transfer configuration structures for USART RX channel
LDMA_Descriptor_t ldmaRXDescriptor[LDMA_DESCRIPTOR_ARRAY_LENGTH];
LDMA_TransferCfg_t ldmaRXConfig;

static osSemaphoreId_t transfer_done_semaphore      = NULL;
osMutexId_t ncp_transfer_mutex                      = 0;
static sl_si91x_host_init_configuration init_config = { 0 };

#ifdef SL_NCP_UART_INTERFACE
static bool rx_dma_callback(unsigned int channel, unsigned int sequenceNo, void *userParam)
{
  UNUSED_PARAMETER(channel);
  UNUSED_PARAMETER(sequenceNo);
  UNUSED_PARAMETER(userParam);

  if (NULL != init_config.rx_done) {
    init_config.rx_done();
  }

  return false;
}

static bool dma_callback(unsigned int channel, unsigned int sequenceNo, void *userParam)
{
  UNUSED_PARAMETER(channel);
  UNUSED_PARAMETER(sequenceNo);
  UNUSED_PARAMETER(userParam);
  osSemaphoreRelease(transfer_done_semaphore);
  return false;
}

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

static void efx32_ncp_uart_init(uint32_t baudrate, bool hfc)
{
  USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT;
  init.baudrate                = baudrate;

  if (true == hfc) {
    init.hwFlowControl = usartHwFlowControlCtsAndRts;
  }

  // Disable the VCOM so that USART Can be used for NCP connection
  GPIO_PinModeSet(VCOM_EN_PIN.port, VCOM_EN_PIN.pin, gpioModePushPull, 0);
  GPIO_PinOutClear(VCOM_EN_PIN.port, VCOM_EN_PIN.pin);

  uartdrv_handle = sl_uartdrv_get_default();
  UARTDRV_DeInit(uartdrv_handle);

  // Enable oscillator to NCP USART module
  CMU_ClockEnable(NCP_USART_CMU_CLOCK, true);
  CMU_ClockEnable(cmuClock_GPIO, true);

  // UART Rx
  GPIO_PinModeSet(SL_UARTDRV_USART_EXP_RX_PORT, SL_UARTDRV_USART_EXP_RX_PIN, gpioModeWiredAndPullUp, 0);
  // UART Tx
  GPIO_PinModeSet(SL_UARTDRV_USART_EXP_TX_PORT, SL_UARTDRV_USART_EXP_TX_PIN, gpioModePushPull, 1);

  if (true == hfc) {
    // UART CTS
    GPIO_PinModeSet(NCP_UART_CTS_PIN.port, NCP_UART_CTS_PIN.pin, gpioModeInput, 0);
    // UART RTS
    GPIO_PinModeSet(NCP_UART_RTS_PIN.port, NCP_UART_RTS_PIN.pin, gpioModePushPull, 1);
  }

  // Initialize UART asynchronous mode and route pins
  USART_InitAsync(UART_HANDLE, &init);

  GPIO->USARTROUTE[NCP_USART_ROUTE_INDEX].RXROUTE = (SL_UARTDRV_USART_EXP_RX_PORT << _GPIO_USART_RXROUTE_PORT_SHIFT)
                                                    | (SL_UARTDRV_USART_EXP_RX_PIN << _GPIO_USART_RXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[NCP_USART_ROUTE_INDEX].TXROUTE = (SL_UARTDRV_USART_EXP_TX_PORT << _GPIO_USART_TXROUTE_PORT_SHIFT)
                                                    | (SL_UARTDRV_USART_EXP_TX_PIN << _GPIO_USART_TXROUTE_PIN_SHIFT);

  if (true == hfc) {
    GPIO->USARTROUTE[NCP_USART_ROUTE_INDEX].CTSROUTE = (NCP_UART_CTS_PIN.port << _GPIO_USART_CTSROUTE_PORT_SHIFT)
                                                       | (NCP_UART_CTS_PIN.pin << _GPIO_USART_CTSROUTE_PIN_SHIFT);
    GPIO->USARTROUTE[NCP_USART_ROUTE_INDEX].RTSROUTE = (NCP_UART_RTS_PIN.port << _GPIO_USART_RTSROUTE_PORT_SHIFT)
                                                       | (NCP_UART_RTS_PIN.pin << _GPIO_USART_RTSROUTE_PIN_SHIFT);
  }

  // Enable USART interface pins
  if (true == hfc) {
    GPIO->USARTROUTE[NCP_USART_ROUTE_INDEX].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN | GPIO_USART_ROUTEEN_TXPEN
                                                      | GPIO_USART_ROUTEEN_RTSPEN;
  } else {
    GPIO->USARTROUTE[NCP_USART_ROUTE_INDEX].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN | GPIO_USART_ROUTEEN_TXPEN;
  }

  DMADRV_Init();
  DMADRV_AllocateChannel(&rx_ldma_channel, NULL);
  DMADRV_AllocateChannel(&tx_ldma_channel, NULL);

  // Clear interrupt
  NVIC_ClearPendingIRQ(NCP_RX_IRQ);

  // Enable receive data valid interrupt
  USART_IntEnable(UART_HANDLE, USART_IEN_RXDATAV);
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

static void spi_dma_callback(struct SPIDRV_HandleData *handle, Ecode_t transferStatus, int itemsTransferred)
{
  UNUSED_PARAMETER(handle);
  UNUSED_PARAMETER(transferStatus);
  UNUSED_PARAMETER(itemsTransferred);
  osSemaphoreRelease(transfer_done_semaphore);
  return;
}

static void efx32_spi_init(void)
{
  SPIDRV_SetBitrate(SPI_HANDLE, 12500000);

  // Configure SPI bus pins
  GPIO_PinModeSet(SL_SPIDRV_EXP_RX_PORT, SL_SPIDRV_EXP_RX_PIN, gpioModeInput, 0);
  GPIO_PinModeSet(SL_SPIDRV_EXP_TX_PORT, SL_SPIDRV_EXP_TX_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(SL_SPIDRV_EXP_CLK_PORT, SL_SPIDRV_EXP_CLK_PIN, gpioModePushPullAlternate, 0);
  GPIO_PinModeSet(SL_SPIDRV_EXP_CS_PORT, SL_SPIDRV_EXP_CS_PIN, gpioModePushPull, 1);

  // configure packet pending interrupt priority
  NVIC_SetPriority(NCP_RX_IRQ, PACKET_PENDING_INT_PRI);
  GPIOINT_CallbackRegister(SI91X_NCP_INTERRUPT_PIN, gpio_interrupt);
  GPIO_PinModeSet(SI91X_NCP_INTERRUPT_PORT, SI91X_NCP_INTERRUPT_PIN, gpioModeInputPullFilter, 0);

  // Check if the boot option is set to LOAD_DEFAULT_NWP_FW_ACTIVE_LOW
  if (init_config.boot_option == LOAD_DEFAULT_NWP_FW_ACTIVE_LOW)
    // Configure the GPIO external interrupt for active low configuration
    GPIO_ExtIntConfig(SI91X_NCP_INTERRUPT_PORT, SI91X_NCP_INTERRUPT_PIN, SI91X_NCP_INTERRUPT_PIN, false, true, true);
  else
    // Configure the GPIO external interrupt for active high configuration
    GPIO_ExtIntConfig(SI91X_NCP_INTERRUPT_PORT, SI91X_NCP_INTERRUPT_PIN, SI91X_NCP_INTERRUPT_PIN, true, false, true);

  return;
}

Ecode_t si91x_SPIDRV_MTransfer(SPIDRV_Handle_t handle,
                               const void *txBuffer,
                               void *rxBuffer,
                               int count,
                               SPIDRV_Callback_t callback)
{
  USART_TypeDef *usart = handle->initData.port;
  uint8_t *tx          = (txBuffer != NULL) ? (uint8_t *)txBuffer : dummy_buffer;
  uint8_t *rx          = (rxBuffer != NULL) ? (uint8_t *)rxBuffer : dummy_buffer;

  if (count < 16) {
    while (count > 0) {
      while (!(usart->STATUS & USART_STATUS_TXBL)) {
      }
      usart->TXDATA = (uint32_t)*tx;
      while (!(usart->STATUS & USART_STATUS_TXC)) {
      }
      *rx = (uint8_t)usart->RXDATA;
      if (txBuffer != NULL) {
        tx++;
      }
      if (rxBuffer != NULL) {
        rx++;
      }
      count--;
    }
    //callback(handle, ECODE_EMDRV_SPIDRV_OK, 0);
    return ECODE_EMDRV_SPIDRV_OK;
  } else {
    SPIDRV_MTransfer(handle, tx, rx, count, callback);
  }

  return ECODE_EMDRV_SPIDRV_BUSY;
}
#endif

void sl_si91x_host_set_sleep_indicator(void)
{
  GPIO_PinOutSet(SI91X_NCP_SLEEP_CONFIRM_PORT, SI91X_NCP_SLEEP_CONFIRM_PIN);
}

void sl_si91x_host_clear_sleep_indicator(void)
{
  GPIO_PinOutClear(SI91X_NCP_SLEEP_CONFIRM_PORT, SI91X_NCP_SLEEP_CONFIRM_PIN);
}

uint32_t sl_si91x_host_get_wake_indicator(void)
{
  return GPIO_PinInGet(SI91X_NCP_WAKE_INDICATOR_PORT, SI91X_NCP_WAKE_INDICATOR_PIN);
}

sl_status_t sl_si91x_host_init(sl_si91x_host_init_configuration *config)
{
  init_config.rx_irq      = config->rx_irq;
  init_config.rx_done     = config->rx_done;
  init_config.boot_option = config->boot_option;

  // Enable clock (not needed on xG21)
  CMU_ClockEnable(cmuClock_GPIO, true);

  if (ncp_transfer_mutex == 0) {
    ncp_transfer_mutex = osMutexNew(NULL);
  }

  if (transfer_done_semaphore == NULL) {
    transfer_done_semaphore = osSemaphoreNew(1, 0, NULL);
  }

#ifdef SL_NCP_UART_INTERFACE
  efx32_ncp_uart_init(115200, false);
#else
  efx32_spi_init();
#endif

  // Start reset line low
  GPIO_PinModeSet(SI91X_NCP_RESET_PORT, SI91X_NCP_RESET_PIN, gpioModeWiredAnd, 0);

  // Configure interrupt, sleep and wake confirmation pins
  GPIO_PinModeSet(SI91X_NCP_SLEEP_CONFIRM_PORT, SI91X_NCP_SLEEP_CONFIRM_PIN, gpioModeWiredOrPullDown, 1);
  GPIO_PinModeSet(SI91X_NCP_WAKE_INDICATOR_PORT, SI91X_NCP_WAKE_INDICATOR_PIN, gpioModeWiredOrPullDown, 0);

  return SL_STATUS_OK;
}

sl_status_t sl_si91x_host_deinit(void)
{
#ifdef SL_NCP_UART_INTERFACE
  ncp_initialized = false;
#endif
  return SL_STATUS_OK;
}

void sl_si91x_host_enable_high_speed_bus()
{
#ifdef SL_NCP_UART_INTERFACE
  efx32_ncp_uart_init(SL_SI91X_NCP_UART_BAUDRATE, false);
#else
  //SPI_USART->CTRL_SET |= USART_CTRL_SMSDELAY | USART_CTRL_SSSEARLY;
#endif

  return;
}

#ifndef SL_NCP_UART_INTERFACE
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
  osMutexAcquire(ncp_transfer_mutex, 0xFFFFFFFFUL);

  if (ECODE_EMDRV_SPIDRV_BUSY
      == si91x_SPIDRV_MTransfer(SPI_HANDLE, tx_buffer, rx_buffer, buffer_length, spi_dma_callback)) {
    if (osSemaphoreAcquire(transfer_done_semaphore, 1000) != osOK) {
      BREAKPOINT();
    }
  }

  osMutexRelease(ncp_transfer_mutex);
  return SL_STATUS_OK;
}
#endif

sl_status_t sl_si91x_host_uart_transfer(const void *tx_buffer, void *rx_buffer, uint16_t buffer_length)
{
#ifndef SL_NCP_UART_INTERFACE
  UNUSED_PARAMETER(tx_buffer);
  UNUSED_PARAMETER(rx_buffer);
  UNUSED_PARAMETER(buffer_length);
  return SL_STATUS_OK;
#else
  uint16_t i         = 0;
  uint8_t *buffer    = NULL;
  sl_status_t status = SL_STATUS_OK;

  osMutexAcquire(ncp_transfer_mutex, 0xFFFFFFFFUL);
  if (NULL != tx_buffer) {
    if ((buffer_length <= 16) || (false == ncp_initialized)) {
      buffer = (uint8_t *)tx_buffer;
      for (i = 0; i < buffer_length; i++) {
        USART_Tx(UART_HANDLE, buffer[i]);
      }
    } else {
      ldmaTXDescriptor[0] =
        (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(tx_buffer, &(UART_HANDLE->TXDATA), buffer_length);
      // Transfer a byte on free space in the USART buffer
      ldmaTXConfig = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(NCP_USART_LDMA_TX);
      // Start TX channel
      DMADRV_LdmaStartTransfer(tx_ldma_channel,
                               &ldmaTXConfig,
                               (LDMA_Descriptor_t *)&ldmaTXDescriptor,
                               dma_callback,
                               NULL);

      if (osSemaphoreAcquire(transfer_done_semaphore, 1000) != osOK) {
        BREAKPOINT();
      }
    }
  }

  if (NULL != rx_buffer) {
    if ((buffer_length <= 16) || (false == ncp_initialized)) {
      buffer = (uint8_t *)rx_buffer;
      for (i = 0; i < buffer_length; i++) {
        buffer[i] = USART_Rx(UART_HANDLE);
      }
    } else {
      ldmaRXDescriptor[0] =
        (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_P2M_BYTE(&(UART_HANDLE->RXDATA), rx_buffer, buffer_length);
      // Transfer a byte on receive data valid
      ldmaRXConfig = (LDMA_TransferCfg_t)LDMA_TRANSFER_CFG_PERIPHERAL(NCP_USART_LDMA_RX);
      // Start RX channel
      DMADRV_LdmaStartTransfer(rx_ldma_channel,
                               &ldmaRXConfig,
                               (LDMA_Descriptor_t *)&ldmaRXDescriptor,
                               rx_dma_callback,
                               NULL);

      status = SL_STATUS_IN_PROGRESS;
    }
  }

  osMutexRelease(ncp_transfer_mutex);
  return status;
#endif
}

void sl_si91x_host_flush_uart_rx(void)
{
#ifdef SL_NCP_UART_INTERFACE
  uint8_t data       = 0;
  uint32_t timestamp = 0;

  while (1) {
    timestamp = osKernelGetTickCount();
    while (!(UART_HANDLE->STATUS & USART_STATUS_RXDATAV)) {
      if ((osKernelGetTickCount() - timestamp) > 2000) {
        return;
      }
    }
    data = (uint8_t)UART_HANDLE->RXDATA;
    data ^= data;
  }
#endif

  return;
}

void sl_si91x_host_uart_enable_hardware_flow_control(void)
{
#ifdef SL_NCP_UART_INTERFACE
  efx32_ncp_uart_init(SL_SI91X_NCP_UART_BAUDRATE, true);
#endif

  return;
}

void sl_si91x_host_hold_in_reset(void)
{
  GPIO_PinOutClear(SI91X_NCP_RESET_PORT, SI91X_NCP_RESET_PIN);
}

void sl_si91x_host_release_from_reset(void)
{
  GPIO_PinOutSet(SI91X_NCP_RESET_PORT, SI91X_NCP_RESET_PIN);
}

void sl_si91x_host_enable_bus_interrupt(void)
{
#ifdef SL_NCP_UART_INTERFACE
  ncp_initialized = true;
#endif
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
