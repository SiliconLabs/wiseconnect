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
/**************************************************************************/  /**
 * @file unity_usart.c
 * @brief USART_Setup function that enables USART for Unity test reporting
 * @version 1.0.0
 ******************************************************************************
 * # License
 * <b>(C) Copyright 2014 Energy Micro AS, http://www.energymicro.com</b>
 ******************************************************************************
 *
 * This source code is the property of Energy Micro AS. The source and compiled
 * code may only be used on Energy Micro "EFM32" microcontrollers.
 *
 * This copyright notice may not be removed from the source code nor changed.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
 * obligation to support this Software. Energy Micro AS is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Energy Micro AS will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 *****************************************************************************/
//#include "em_cmu.h"
//#include "em_gpio.h"
//#include "em_usart.h"
//#include "em_leuart.h"
#include "rsi_board.h"

#include "unity_conf.h"
#include "unity_usart.h"
/* Make sure to include unity_conf.h before the following because
   RETARGET_PERIPHERAL_ENABLE is potentially defined in unity_conf.h. */
#if defined(RETARGET_PERIPHERAL_ENABLE)
#include "bsp.h"
#endif

#if defined(UNITY_OUTPUT_SERIAL)

#if (_SILICON_LABS_32B_SERIES == 2)

#define REFFREQ 20000000UL
/**************************************************************************/ /**
 * @brief Configure USART for Unity test reporting
 *****************************************************************************/
void UNITY_UsartSetup(void)
{

#if defined(RETARGET_PERIPHERAL_ENABLE)
  RETARGET_PERIPHERAL_ENABLE();
#endif
  uint32_t clkdiv;

  // Configure USART for basic async operation
  USART0->EN = 1;

  /* Configure GPIO pins */
  GPIO_PinModeSet(UNITY_UART_TX_PORT, UNITY_UART_TX_PIN, gpioModePushPull, 1);
  //GPIO_PinModeSet(UNITY_UART_RX_PORT, UNITY_UART_RX_PIN, gpioModeInput, 0);

  GPIO->USARTROUTE[0].ROUTEEN = GPIO_USART_ROUTEEN_TXPEN;

  // Route TX to PC0
  GPIO->USARTROUTE[0].TXROUTE = (UNITY_UART_TX_PORT << _GPIO_USART_TXROUTE_PORT_SHIFT)
                                | (UNITY_UART_TX_PIN << _GPIO_USART_TXROUTE_PIN_SHIFT);

  // Configure USART for basic async operation
  USART0->CTRL |= USART_CTRL_MVDIS;
  USART0->FRAME = USART_FRAME_DATABITS_EIGHT | USART_FRAME_STOPBITS_ONE | USART_FRAME_PARITY_NONE;

  // Calculate and set CLKDIV with fractional bits
  clkdiv = 4 * REFFREQ + (16 * 115200) / 2;
  clkdiv /= (16 * 115200);
  clkdiv -= 4;
  clkdiv *= 64;

  clkdiv &= _USART_CLKDIV_MASK;

  USART0->CTRL &= ~_USART_CTRL_OVS_MASK;
  USART0->CTRL |= USART_CTRL_OVS_X16;
  USART0->CLKDIV = clkdiv;

  // Finally enable it
  USART0->CMD = USART_CMD_TXEN;
}

#else
/**************************************************************************/ /**
 * @brief Configure USART for Unity test reporting
 *****************************************************************************/
void UNITY_UsartSetup(void)
{
#if !defined(UNITY_UART_TYPE_USART_SI917)
  CMU_ClockEnable(cmuClock_GPIO, true);
#endif

#if defined(RETARGET_PERIPHERAL_ENABLE)
  RETARGET_PERIPHERAL_ENABLE();
#endif

#if defined(UNITY_TEST_REPORT_VCOM) || defined(VCOM_ENABLE_PORT)
  GPIO_PinModeSet(VCOM_ENABLE_PORT, VCOM_ENABLE_PIN, gpioModePushPull, 1);
#endif

#if !defined(UNITY_UART_TYPE_USART_SI917)
  /* Configure GPIO pins */
  GPIO_PinModeSet(UNITY_UART_TX_PORT, UNITY_UART_TX_PIN, gpioModePushPull, 1);
  GPIO_PinModeSet(UNITY_UART_RX_PORT, UNITY_UART_RX_PIN, gpioModeInput, 0);

  /* Enable peripheral clocks */
  CMU_ClockEnable(cmuClock_HFPER, true);
#endif

#if defined(UNITY_UART_TYPE_USART) || defined(UNITY_UART_TYPE_UART)
  CMU_ClockEnable(UNITY_UART_CLOCK, true);

  USART_TypeDef *usart         = UNITY_UART;
  USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT;

  /* Configure USART for basic async operation */
  USART_InitAsync(usart, &init);

  /* Enable pins at UNITY UART location */
#if defined(USART_ROUTEPEN_RXPEN)
  usart->ROUTEPEN  = USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;
  usart->ROUTELOC0 = (usart->ROUTELOC0 & ~(_USART_ROUTELOC0_TXLOC_MASK | _USART_ROUTELOC0_RXLOC_MASK))
                     | (UNITY_UART_TX_LOC << _USART_ROUTELOC0_TXLOC_SHIFT)
                     | (UNITY_UART_RX_LOC << _USART_ROUTELOC0_RXLOC_SHIFT);
#else
  usart->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | UNITY_UART_LOC;
#endif

#elif defined(UNITY_UART_TYPE_LEUART)
  LEUART_TypeDef *leuart   = UNITY_UART;
  LEUART_Init_TypeDef init = LEUART_INIT_DEFAULT;

  /* Setup clock. */
  CMU_ClockEnable(cmuClock_CORELE, true);
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_CORELEDIV2);
  CMU_ClockEnable(UNITY_UART_CLOCK, true);
  CMU_ClockDivSet(UNITY_UART_CLOCK, cmuClkDiv_1);

  /* Configure LEUART */
  init.baudrate = 115200;
  LEUART_Init(leuart, &init);

  /* Enable pins at default location */
  leuart->ROUTE = LEUART_ROUTE_RXPEN | LEUART_ROUTE_TXPEN | UNITY_UART_LOC;

#elif defined(UNITY_UART_TYPE_USART_SI917)
  Board_Debug_Init();
#else
#error "No UART/UART/LEUART is defined."
#endif
}
#endif

#if defined(UNITY_UART_TYPE_USART_SI917)
void UNITY_UsartTx(char c)
{
  char str[2];
  str[0] = c;
  str[1] = '\0';
  if (c == '\n') {
    DEBUGOUT("\r");
  }
  DEBUGOUT(str);
}
#else
void UNITY_UsartTx(char c)
{
  if (c == '\n') {
    UNITY_TX(UNITY_UART, '\r');
  }
  UNITY_TX(UNITY_UART, c);
}
#endif

void UNITY_UsartFlush(void)
{
  // WARNING: This function will hang if called before any data is sent. This is
  // because we are using the TXC flag for compatibility with older devices
  // (since emlib does not have a generic flush function).

  while (!UNITY_UART_DONE) {
    __NOP();
  }
}

#endif
