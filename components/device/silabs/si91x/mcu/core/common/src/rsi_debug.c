/*******************************************************************************
 * @file  rsi_debug.c
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

#include "rsi_chip.h"
#include "rsi_debug.h"
#include "USART.h"
#include "rsi_ccp_common.h"

void ARM_UART_SignalEvent(uint32_t event);

#ifdef SLI_SI91X_MCU_CONFIG_RADIO_BOARD_BASE_VER
#define M4_UART1_INSTANCE 0U //!Select m4 uart1 for prints
#if ((defined(ULP_MODE_EXECUTION)) || (defined(SLI_SI91X_MCU_CONFIG_RADIO_BOARD_VER2)))
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

ARM_USART_CAPABILITIES drv_ulp_capabilities;

volatile uint32_t send_done = 0, recv_done = 0;
#ifdef SLI_SI91X_MCU_INTR_BASED_RX_ON_UART // Add this macro to receive data in interrupt based
uint8_t rx_char;
#endif

void ARM_UART_SignalEvent(uint32_t event)
{
  switch (event) {
    case ARM_USART_EVENT_SEND_COMPLETE:
      send_done++;
      break;
    case ARM_USART_EVENT_RECEIVE_COMPLETE:
#ifdef SLI_SI91X_MCU_INTR_BASED_RX_ON_UART
      UARTdrv->Receive((void *)&rx_char, 1);
      cache_uart_rx_data(rx_char);
#endif
      recv_done++;
      break;
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

/**
 * @fn         void Board_Debug_Init(void)
 * @brief      Initializes board UART for output, required for printf redirection.
 * @return     none
 */
void Board_Debug_Init(void)
{

  UARTdrv->Uninitialize();

  UARTdrv->Initialize(ARM_UART_SignalEvent);

  UARTdrv->PowerControl(ARM_POWER_FULL);

  /* Enable Receiver and Transmitter lines */
  UARTdrv->Control(ARM_USART_CONTROL_TX, 1);

  UARTdrv->Control(ARM_USART_CONTROL_RX, 1);

  UARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS | ARM_USART_DATA_BITS_8 | ARM_USART_PARITY_NONE | ARM_USART_STOP_BITS_1
                     | ARM_USART_FLOW_CONTROL_NONE,
                   BOARD_BAUD_VALUE);

#ifdef SLI_SI91X_MCU_INTR_BASED_RX_ON_UART
  UARTdrv->Receive((void *)&rx_char, 1);
#else
#if defined(M4_UART1_INSTANCE) && (M4_UART1_INSTANCE == 1)
  NVIC_DisableIRQ(USART0_IRQn);
#endif

#if defined(M4_UART2_INSTANCE) && (M4_UART2_INSTANCE == 1)
  NVIC_DisableIRQ(UART1_IRQn);
#endif

#if defined(ULP_UART_INSTANCE) && (ULP_UART_INSTANCE == 1)
  NVIC_DisableIRQ(ULPSS_UART_IRQn);
#endif
#endif

  return;
}

#if defined(__GNUC__)

#if defined(__REDLIB_INTERFACE_VERSION__) && (__REDLIB_INTERFACE_VERSION__ >= 20000)
/* We are using new Redlib_v2 semihosting interface */
#define WRITEFUNC __sys_write
#define READFUNC  __sys_readc
#else
/* We are using original Redlib semihosting interface */
#define WRITEFUNC __write
#define READFUNC  __readc
#endif

#if defined(DEBUG_ENABLE)
#if defined(DEBUG_SEMIHOSTING)
/* Do nothing, semihosting is enabled by default in LPCXpresso */
#endif /* defined(DEBUG_SEMIHOSTING) */
#endif /* defined(DEBUG_ENABLE) */

#if !defined(DEBUG_SEMIHOSTING)
int WRITEFUNC(int iFileHandle, char *pcBuffer, int iLength);
int WRITEFUNC(int iFileHandle, char *pcBuffer, int iLength)
{
  (void)iFileHandle;
#if defined(DEBUG_ENABLE)
  int i;
  for (i = 0; i < iLength; i++) {
    Board_UARTPutChar(pcBuffer[i]);
  }
#endif

  return iLength;
}
#endif
/* Called by bottom level of scanf routine within RedLib C library to read
   a character. With the default semihosting stub, this would read the character
   from the debugger console window (which acts as stdin). But this version reads
   the character from the LPC1768/RDB1768 UART. */
int READFUNC(void);
int READFUNC(void)
{
#if defined(DEBUG_ENABLE)
  int c = Board_UARTGetChar();
  return c;

#else
  return (int)-1;
#endif
}
#endif // defined( __GNUC__ )

#if defined(__CC_ARM)

#include <stdio.h>
#include <rt_misc.h>

#if defined(DEBUG_ENABLE)
#if defined(DEBUG_SEMIHOSTING)
#define ITM_Port8(n)  (*((volatile unsigned char *)(0xE0000000 + 4 * n)))
#define ITM_Port16(n) (*((volatile unsigned short *)(0xE0000000 + 4 * n)))
#define ITM_Port32(n) (*((volatile unsigned long *)(0xE0000000 + 4 * n)))

#define DEMCR  (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA 0x01000000

/* Write to SWO */
void _ttywrch(int ch)
{
  if (DEMCR & TRCENA) {
    while (ITM_Port32(0) == 0) {
    }
    ITM_Port8(0) = ch;
  }
}

#else
static void BoardOutChar(char ch)
{
#ifndef SIM_9118
  Board_UARTPutChar(ch);
#else
  *(volatile uint8_t *)0x800 = ch;
#endif
}
#endif /* defined(DEBUG_SEMIHOSTING) */
#endif /* defined(DEBUG_ENABLE) */

#ifndef DEBUG_FILE_HANDLE
struct __FILE {
  int handle;
};

FILE __stdout;
FILE __stdin;
FILE __stderr;
void *_sys_open(const char *name, int openmode)
{
  return NULL;
}

int fputc(int c, FILE *f)
{
#if defined(DEBUG_ENABLE)
#if defined(DEBUG_SEMIHOSTING)
  _ttywrch(c);
#else
  BoardOutChar((char)c);
#endif
#endif
  return 0;
}

int fgetc(FILE *f)
{
#if defined(DEBUG_ENABLE) && !defined(DEBUG_SEMIHOSTING)
  return Board_UARTGetChar();
#else
  return 0;
#endif
}
void _sys_exit(int return_code)
{
label:
  __WFI();
  goto label; /* endless loop */
}

int ferror(FILE *f)
{
  return EOF;
}
#endif

#endif /* defined (__CC_ARM) */

/** @addtogroup SOC
* @{
*/
/**
 * @fn           void Board_UARTPutSTR(uint8_t *ptr)
 * @brief        Prints a string to the UART.
 * @param[in]    ptr  : Terminated string to output
 * @return       none
 */
void Board_UARTPutSTR(uint8_t *ptr)
{
  int i;
  for (i = 0; ptr[i] != '\0'; i++) {
    send_done = 0;
    UARTdrv->Send(&ptr[i], 1);
    while (send_done == 0)
      ;
  }
  return;
}

/**
 * @fn        uint8_t Board_UARTGetChar(void)
 * @brief     Get a single character from the UART, required for scanf input.
 * @return    EOF if not character was received, or character value
 *
 */
uint8_t Board_UARTGetChar(void)
{
  uint8_t rev[1] = { 0 };
  recv_done      = 0;
  UARTdrv->Receive(&rev, 1);
  while (recv_done == 0) {
#if defined(M4_UART1_INSTANCE) && (M4_UART1_INSTANCE == 1)
    RSI_M4SSUsart0Handler();
#endif
#if defined(M4_UART2_INSTANCE) && (M4_UART2_INSTANCE == 1)
    RSI_M4SSUart1Handler();
#endif
#if defined(ULP_UART_INSTANCE) && (ULP_UART_INSTANCE == 1)
    RSI_ULPUartHandler();
#endif
  }
  return rev[0];
}

/**
 * @fn          void Board_UARTPutChar(uint8_t ch)
 * @brief       Sends a single character on the UART, required for printf redirection.
 * @param[in]   ch  : character to send
 * @return      none
 */
void Board_UARTPutChar(uint8_t ch)
{
  send_done = 0;
  UARTdrv->Send(&ch, sizeof(ch));
  while (send_done == 0) {
#if defined(M4_UART1_INSTANCE) && (M4_UART1_INSTANCE == 1)
    RSI_M4SSUsart0Handler();
#endif
#if defined(M4_UART2_INSTANCE) && (M4_UART2_INSTANCE == 1)
    RSI_M4SSUart1Handler();
#endif
#if defined(ULP_UART_INSTANCE) && (ULP_UART_INSTANCE == 1)
    RSI_ULPUartHandler();
#endif
  }

  return;
}
/** @} */

#ifdef SLI_SI91X_DBG_MIDDLEWARE_EN
int stdin_getchar(void)
{
  int32_t ch = -1;

  do {
#ifdef DEBUG_UART
    ch = Board_UARTGetChar();
#endif
  } while (ch == -1);
  return (ch);
}
int stdout_putchar(int ch)
{
#ifdef DEBUG_UART
  (Board_UARTPutChar(ch));
#endif
  return (ch);
}
int stderr_putchar(int ch)
{
#ifdef DEBUG_UART
  Board_UARTPutChar(ch);
#endif
  return (ch);
}

#endif // SLI_SI91X_DBG_MIDDLEWARE_EN

/* IAR support */
#if defined(__ICCARM__)
/*******************
 *
 * Copyright 1998-2003 IAR Systems.  All rights reserved.
 *
 * $Revision: 30870 $
 *
 * This is a template implementation of the "__write" function used by
 * the standard library.  Replace it with a system-specific
 * implementation.
 *
 * The "__write" function should output "size" number of bytes from
 * "buffer" in some application-specific way.  It should return the
 * number of characters written, or _LLIO_ERROR on failure.
 *
 * If "buffer" is zero then __write should perform flushing of
 * internal buffers, if any.  In this case "handle" can be -1 to
 * indicate that all handles should be flushed.
 *
 * The template implementation below assumes that the application
 * provides the function "MyLowLevelPutchar".  It should return the
 * character written, or -1 on failure.
 *
 ********************/

#include <yfuns.h>

#if defined(DEBUG_UART) && !defined(DEBUG_SEMIHOSTING)

//_STD_BEGIN

#pragma module_name = "?__write"

/*
   If the __write implementation uses internal buffering, uncomment
   the following line to ensure that we are called with "buffer" as 0
   (i.e. flush) when the application terminates. */
size_t __write(int handle, const unsigned char *buffer, size_t size)
{
#if defined(DEBUG_UART)
  size_t nChars = 0;

  if (buffer == 0) {
    /*
       This means that we should flush internal buffers.  Since we
       don't we just return.  (Remember, "handle" == -1 means that all
       handles should be flushed.)
     */
    return 0;
  }

  /* This template only writes to "standard out" and "standard err",
     for all other file handles it returns failure. */
  if ((handle != _LLIO_STDOUT) && (handle != _LLIO_STDERR)) {
    return _LLIO_ERROR;
  }

  for (/* Empty */; size != 0; --size) {
    Board_UARTPutChar(*buffer++);
    ++nChars;
  }

  return nChars;
#else
  return size;
#endif /* defined(DEBUG_UART) */
}

//_STD_END
#endif

#endif /* defined (__ICCARM__) */
