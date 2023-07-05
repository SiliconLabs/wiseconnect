/***************************************************************************/ /**
 * # License
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is Third Party Software licensed by Silicon Labs from a third party
 * and is governed by the sections of the MSLA applicable to Third Party
 * Software and the additional terms set forth below.
 *
 ******************************************************************************/
/* ==========================================
    Unity Project - A Test Framework for C
    Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
    [Released under MIT License. Please refer to license.txt for details]
========================================== */

#ifndef UNITY_CONF_H
#define UNITY_CONF_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(HOST_BUILD)

#if !defined(__weak)
#define __weak
#endif

#else

//#include "em_device.h"
#include "si91x_device.h"
#include "core_cm4.h"

// Use SWO as default
#if defined(GPIO_ROUTE_SWOPEN) || defined(GPIO_ROUTEPEN_SWVPEN) || defined(GPIO_TRACEROUTEPEN_SWVPEN)
#if !defined(UNITY_OUTPUT_NONE) && !defined(UNITY_OUTPUT_STDOUT) && !defined(UNITY_OUTPUT_SERIAL) \
  && !defined(UNITY_OUTPUT_SWO) && !defined(UNITY_OUTPUT_IOSTREAM) && !defined(UNITY_OUTPUT_RTT)
#define UNITY_OUTPUT_SWO
#endif
#endif

#if defined(UNITY_OUTPUT_SERIAL)

#if defined(SLSTK3301A_EFM32TG11)

#define UNITY_UART_TYPE_UART
#define UNITY_TX           USART_Tx
#define UNITY_UART         UART0
#define UNITY_UART_DONE    (UART0->STATUS & UART_STATUS_TXC)
#define UNITY_UART_RX_PIN  15
#define UNITY_UART_RX_PORT gpioPortC
#define UNITY_UART_TX_PIN  14
#define UNITY_UART_TX_PORT gpioPortC
#define UNITY_UART_TX_LOC  _UART_ROUTELOC0_TXLOC_LOC3
#define UNITY_UART_RX_LOC  _UART_ROUTELOC0_RXLOC_LOC3
#define UNITY_UART_CLOCK   cmuClock_UART0

#elif defined(EFM32ZG_STK3200)

#define UNITY_UART_TYPE_LEUART
#define UNITY_TX           LEUART_Tx
#define UNITY_UART         LEUART0
#define UNITY_UART_DONE    (LEUART0->STATUS & LEUART_STATUS_TXC)
#define UNITY_UART_RX_PIN  5
#define UNITY_UART_RX_PORT gpioPortD
#define UNITY_UART_TX_PIN  4
#define UNITY_UART_TX_PORT gpioPortD
#define UNITY_UART_LOC     USART_ROUTE_LOCATION_LOC0
#define UNITY_UART_CLOCK   cmuClock_LEUART0

#elif defined(SLSTK3400A_EFM32HG) || defined(SLWSTK6240A_EZR32HG) || defined(SLWSTK6241A_EZR32HG) \
  || defined(SLWSTK6242A_EZR32HG) || defined(SLWSTK6243A_EZR32HG) || defined(SLWSTK6244A_EZR32HG)

#define UNITY_UART_TYPE_LEUART
#define UNITY_TX           LEUART_Tx
#define UNITY_UART         LEUART0
#define UNITY_UART_DONE    (LEUART0->STATUS & LEUART_STATUS_TXC)
#define UNITY_UART_RX_PIN  5
#define UNITY_UART_RX_PORT gpioPortD
#define UNITY_UART_TX_PIN  4
#define UNITY_UART_TX_PORT gpioPortD
#define UNITY_UART_LOC     LEUART_ROUTE_LOCATION_LOC0
#define UNITY_UART_CLOCK   cmuClock_LEUART0

#elif defined(EFR32MG21_BRD4171A)

#define UNITY_UART_TYPE_USART
#define UNITY_TX           USART_Tx
#define UNITY_UART         USART0
#define UNITY_UART_DONE    (USART0->STATUS & USART_STATUS_TXC)
#define UNITY_UART_RX_PIN  6
#define UNITY_UART_RX_PORT gpioPortA
#define UNITY_UART_TX_PIN  5
#define UNITY_UART_TX_PORT gpioPortA

#elif defined(SI917_BRD4325A)

#define UNITY_UART_TYPE_USART_SI917
#define UNITY_TX           USART_Tx
#define UNITY_UART         USART0
#define UNITY_UART_DONE    1
#define UNITY_UART_RX_PIN  6
#define UNITY_UART_RX_PORT gpioPortA
#define UNITY_UART_TX_PIN  5
#define UNITY_UART_TX_PORT gpioPortA

#endif

#if defined(SLWSTK6240A_EZR32HG) || defined(SLWSTK6241A_EZR32HG) || defined(SLWSTK6242A_EZR32HG) \
  || defined(SLWSTK6243A_EZR32HG) || defined(SLWSTK6244A_EZR32HG)
#define VCOM_ENABLE_PORT gpioPortC
#define VCOM_ENABLE_PIN  8
#endif

#endif /* #if defined( UNITY_OUTPUT_SERIAL ) */

/*
 * Select test output function
 *
 * UNITY_OUTPUT_NONE    =>  No output
 * UNITY_OUTPUT_STDOUT  =>  stdio
 * UNITY_OUTPUT_SWO     =>  SWO
 * UNITY_OUTPUT_SERIAL  =>  UART/USART/LEUART
 * UNITY_OUTPUT_IOSTREAM => iostream
 * UNITY_OUTPUT_RTT     =>  RTT
 * <anything else>      => stdio
 */
#if defined(UNITY_OUTPUT_NONE)
#define UNITY_OUTPUT_CHAR(a) /* No output    */
#elif defined(UNITY_OUTPUT_STDOUT)
#define UNITY_OUTPUT_CHAR(a) putchar(a) /* stdio output */
#elif defined(UNITY_OUTPUT_SERIAL)
#include "unity_usart.h"
#define UNITY_OUTPUT_CHAR(a) UNITY_UsartTx(a) /* UART output  */
#elif defined(UNITY_OUTPUT_SWO)
#include "unity_swo.h"
#define UNITY_OUTPUT_CHAR(a) ITM_SendChar(a) /* SWO output   */
#elif defined(UNITY_OUTPUT_RTT)
#include "SEGGER_RTT.h"
#define UNITY_OUTPUT_CHAR(a) SEGGER_RTT_PutChar(0, a) /* RTT output   */
#elif defined(UNITY_OUTPUT_IOSTREAM)
#include "sl_iostream.h"
#define UNITY_OUTPUT_CHAR(a) sl_iostream_putchar(SL_IOSTREAM_STDOUT, a);
#elif defined(UNITY_OUTPUT_RTL)
#define UNITY_OUTPUT_CHAR(a) (*(volatile uint8_t *)0x800 = a) /* RTL output*/
#else
#define UNITY_OUTPUT_CHAR(a) putchar(a) /* stdio output */
#endif

#if defined(__GNUC__) /* GNU Compiler */
#define __weak __attribute__((weak))
#endif

__STATIC_INLINE void UNITY_Init(void)
{
#if defined(UNITY_OUTPUT_SERIAL)
  void UNITY_UsartSetup(void);
  UNITY_UsartSetup();
#elif defined(UNITY_OUTPUT_SWO)
  void SWO_Setup(void);
  SWO_Setup();
#elif defined(UNITY_OUTPUT_RTT)
  SEGGER_RTT_Init();
#endif
}

__STATIC_INLINE void UNITY_Flush(void)
{
#if defined(UNITY_OUTPUT_SERIAL)
  void UNITY_UsartFlush(void);
  UNITY_UsartFlush();
#elif defined(UNITY_OUTPUT_SWO)
  void SWO_Flush(void);
  SWO_Flush();
#endif
}

/**************************************************************************/ /**
 * @brief Sets up Unity and begins test group
 *****************************************************************************/
void sl_unity_start_test(const char *test_file);

/**************************************************************************/ /**
 * @brief Ends Unity test group and waits for ever
 *****************************************************************************/
void sl_unity_stop_test(void);

void UNITY_UsartTx(char c);
#endif

void setUp(void);
void tearDown(void);

#ifdef __cplusplus
}
#endif

#endif
