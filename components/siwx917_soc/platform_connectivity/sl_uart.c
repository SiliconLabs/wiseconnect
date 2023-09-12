/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */
#include "sl_status.h"
#include "sl_uart.h"
#include "sl_board_configuration.h"
#include "sl_constants.h"
#include "USART.h"
//#include "rsi_board.h"
#include "sli_siwx917_soc.h"
#include <stddef.h>
#include <string.h>

#ifdef rtos_FEATURE_REQUIRED
#define WAIT() sl_thread_sleep(1);
#else
#define WAIT() __asm("NOP");
#endif

#ifdef SI917_RADIO_BOARD
#define M4_UART1_INSTANCE 0U //!Select m4 uart1 for prints
#if ((M4_PS2_STATE) || (SI917_RADIO_BOARD_V2))
#define ULP_UART_INSTANCE 1U //!Select m4 uart2 for prints
#else
#define M4_UART2_INSTANCE 1U //!Select ulp uart for prints
#endif
#else
#define M4_UART1_INSTANCE 1U //!Select m4 uart1 for prints
#define M4_UART2_INSTANCE 0U //!Select m4 uart2 for prints
#define ULP_UART_INSTANCE 0U //!Select ulp uart for prints
#endif
#define BOARD_BAUD_VALUE 115200 //UART baud rate

#if defined(M4_UART1_INSTANCE) && (M4_UART1_INSTANCE == 1)
extern ARM_DRIVER_USART Driver_USART0;
static ARM_DRIVER_USART *UARTdrv = &Driver_USART0;
#endif

#if defined(M4_UART2_INSTANCE) && (M4_UART2_INSTANCE == 1)
extern ARM_DRIVER_USART Driver_UART1;
static ARM_DRIVER_USART *UARTdrv = &Driver_UART1;
#endif

#if defined(ULP_UART_INSTANCE) && (ULP_UART_INSTANCE == 1)
extern ARM_DRIVER_USART Driver_ULP_UART;
static ARM_DRIVER_USART *UARTdrv = &Driver_ULP_UART;
#endif

ARM_USART_CAPABILITIES drv_capabilities;

volatile bool tx_done        = false;
static bool uart_initialised = false;
//volatile uint32_t uart_data_received = 0;
//uint8_t circular_rx_buffer[256];
//uint32_t uart_data_processed = 0;
char rx_char;

static void ARM_UART_SignalEvent(uint32_t event);
__WEAK void cache_uart_rx_data(const char character);

// read capabilities

/*==============================================*/
/**
 * @fn          void Read_Capabilities(void)
 * @brief       read capabilities
 * @param[in]   void
 * @return      None
 */
//static void Read_Capabilities(void)
//{
//  drv_capabilities = UARTdrv->GetCapabilities();
//}

/*==============================================*/
/**
 * @fn          void ARM_USART_SignalEvent(uint32_t event)
 * @brief       event callback handler
 * @param[in]   event -transfer events based on the switch case
 * @return      None
 */
static void ARM_UART_SignalEvent(uint32_t event)
{
  switch (event) {
    case ARM_USART_EVENT_SEND_COMPLETE:
      tx_done = true;
      break;
    case ARM_USART_EVENT_RECEIVE_COMPLETE: {
      UARTdrv->Receive((void *)&rx_char, 1);
      cache_uart_rx_data(rx_char);
      break;
    }
    case ARM_USART_EVENT_TRANSFER_COMPLETE:
      break;
    case ARM_USART_EVENT_TX_COMPLETE:
      break;
    case ARM_USART_EVENT_TX_UNDERFLOW:
      break;
    case ARM_USART_EVENT_RX_OVERFLOW:
      break;
    case ARM_USART_EVENT_RX_TIMEOUT:
      break;
    case ARM_USART_EVENT_RX_BREAK:
      break;
    case ARM_USART_EVENT_RX_FRAMING_ERROR:
      break;
    case ARM_USART_EVENT_RX_PARITY_ERROR:
      break;
    case ARM_USART_EVENT_CTS:
      break;
    case ARM_USART_EVENT_DSR:
      break;
    case ARM_USART_EVENT_DCD:
      break;
    case ARM_USART_EVENT_RI:
      break;
  }
}

sl_status_t sl_uart_init(sl_usart_peripheral_t *uart, uint32_t baud_rate, const sl_uart_pin_configuration_t *config)
{
  UARTdrv->Uninitialize();

  UARTdrv->Initialize(ARM_UART_SignalEvent);

  UARTdrv->PowerControl(ARM_POWER_FULL);

  /* Enable Receiver and Transmitter lines */
  UARTdrv->Control(ARM_USART_CONTROL_TX, 1);
  UARTdrv->Control(ARM_USART_CONTROL_RX, 1);

  UARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS | ARM_USART_DATA_BITS_8 | ARM_USART_PARITY_NONE | ARM_USART_STOP_BITS_1
                     | ARM_USART_FLOW_CONTROL_NONE,
                   baud_rate);

#if defined(M4_UART1_INSTANCE) && (M4_UART1_INSTANCE == 1)
  //  NVIC_DisableIRQ(USART0_IRQn);
  NVIC_EnableIRQ(USART0_IRQn);
  NVIC_SetPriority(USART0_IRQn, 7);
#endif

#if defined(M4_UART2_INSTANCE) && (M4_UART2_INSTANCE == 1)
  //  NVIC_DisableIRQ(UART1_IRQn);
  NVIC_EnableIRQ(UART1_IRQn);
  NVIC_SetPriority(UART1_IRQn, 7);
#endif

#if defined(ULP_UART_INSTANCE) && (ULP_UART_INSTANCE == 1)
  //  NVIC_DisableIRQ(ULPSS_UART_IRQn);
  NVIC_EnableIRQ(ULPSS_UART_IRQn);
  NVIC_SetPriority(ULPSS_UART_IRQn, 7);
#endif

  // Prime receiver with an RX request
  UARTdrv->Receive((void *)&rx_char, 1);
  uart_initialised = true;
  return SL_STATUS_OK;
}

sl_status_t sl_uart_print(sl_usart_peripheral_t *uart, const char *string)
{
  UNUSED_PARAMETER(uart);
  if (uart_initialised == false) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  // sends data
  tx_done        = false;
  int32_t status = UARTdrv->Send((const void *)string, strlen(string));
  if (status != ARM_DRIVER_OK) {
    return SL_STATUS_FAIL;
  }
  while (tx_done == false)
    ;
  return SL_STATUS_OK;
}

sl_status_t sl_uart_tx_byte(sl_usart_peripheral_t *uart, uint8_t byte)
{
  UNUSED_PARAMETER(uart);
  if (uart_initialised == false) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  // sends data
  tx_done        = false;
  int32_t status = UARTdrv->Send((const void *)&byte, 1);
  if (status != ARM_DRIVER_OK) {
    return SL_STATUS_FAIL;
  }
  while (tx_done == false)
    ;
  return SL_STATUS_OK;
}

//volatile uint32_t recv_done = 0;
sl_status_t sl_uart_rx_byte(sl_usart_peripheral_t *uart, uint8_t *byte)
{
  UNUSED_PARAMETER(uart);
  UARTdrv->Receive(byte, 1);
  return SL_STATUS_OK;
}

void _putchar(char character)
{
  if (uart_initialised != false) {
    sl_uart_tx_byte(DEFAULT_UART, character);
  }
}

int putchar(int character)
{
  _putchar(character);
  return character;
}

char _getchar(void)
{
  uint8_t ch = 0;
  return ch;
}

long _write(int file, const char *buffer, unsigned long size)
{
  UNUSED_PARAMETER(file);
  if (uart_initialised == false) {
    return 0;
  }
  // sends data
  tx_done        = false;
  int32_t status = UARTdrv->Send((const void *)buffer, size);
  if (status != ARM_DRIVER_OK) {
    return SL_STATUS_FAIL;
  }
  while (tx_done == false)
    ;
  return size;
}

__WEAK void cache_uart_rx_data(const char character)
{
  UNUSED_PARAMETER(character);
}
