/***************************************************************************/ /**
 * @file
 * @brief Board configuration for BRD4120A (EFR32xG26 PK6028A) hosting a
 *        SiWx91x NCP module via the EXP header.
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#pragma once

#ifdef SL_UART
#include "sl_device_registers.h"
#include "sl_uart.h"

#define DEFAULT_UART USART0

#define SL_UART_VCOM_PORT SL_GPIO_PORT_B
#define SL_UART_VCOM_PIN  0
static const sl_gpio_t vcom_enable_pin = { SL_UART_VCOM_PORT, SL_UART_VCOM_PIN };

#define DEFAULT_UART_PIN_CONFIG &default_uart_pin_configuration

/* Aliases consumed by the legacy sl_uart driver (see sl_uart.h). They map
 * the generic UART_CLOCK / UART_RX_IRQ symbols expected by that driver onto
 * the USART0 peripheral that DEFAULT_UART is bound to above. */
#define UART_CLOCK  cmuClock_USART0
#define UART_RX_IRQ USART0_RX_IRQn

/* Note: This creates a static instance for each C file that includes this header and references the variable */
static const sl_uart_pin_configuration_t default_uart_pin_configuration = {
  .tx_port     = SL_GPIO_PORT_A,
  .tx_pin      = 8,
  .rx_port     = SL_GPIO_PORT_A,
  .rx_pin      = 9,
  .cts_port    = SL_GPIO_PORT_A,
  .cts_pin     = 10,
  .rts_port    = SL_GPIO_PORT_A,
  .rts_pin     = 0,
  .uart_number = 0,
};
#endif

typedef struct {
  unsigned char port;
  unsigned char pin;
} sl_pin_t;

#define PIN(port_id, pin_id)                 \
  (sl_pin_t)                                 \
  {                                          \
    .port = gpioPort##port_id, .pin = pin_id \
  }

/* SiWx91x NCP control GPIOs.
 * The four pins below are taken from the BRD4120A (EFR32xG26 PK6028A) EXP header
 * routing in the gecko-sdk brd4120a_brd4002a/legacy_ncp_spi_config.h plus two
 * free EXP-header GPIOs (PB5/PB7).  Adjust to match your physical wiring. */
#define SLEEP_CONFIRM_PIN  PIN(B, 6)
#define WAKE_INDICATOR_PIN PIN(B, 8)
#define RESET_PIN          PIN(D, 2)
#define INTERRUPT_PIN      PIN(D, 3)
#define VCOM_EN_PIN        PIN(B, 0)

/* USART0 routed to the EXP UART pads of BRD4120A. */
#define NCP_UART_TX_PIN PIN(A, 8)
#define NCP_UART_RX_PIN PIN(A, 9)

/* USART0 routed to the EXP SPI pads of BRD4120A (matches gecko-sdk
 * brd4120a_brd4002a/legacy_ncp_spi_config.h). */
#define SPI_CLOCK_PIN PIN(C, 13)
#define SPI_MOSI_PIN  PIN(C, 12)
#define SPI_MISO_PIN  PIN(C, 7)
#define SPI_CS_PIN    PIN(C, 8)

#if !defined(SL_DMA_SIGNAL_USART0_TXBL) && defined(ldmaPeripheralSignal_USART0_TXBL)
#define SL_DMA_SIGNAL_USART0_TXBL ldmaPeripheralSignal_USART0_TXBL
#endif
#if !defined(SL_DMA_SIGNAL_USART0_RXDATAV) && defined(ldmaPeripheralSignal_USART0_RXDATAV)
#define SL_DMA_SIGNAL_USART0_RXDATAV ldmaPeripheralSignal_USART0_RXDATAV
#endif

#define NCP_USART                   USART0
#define NCP_USART_CMU_CLOCK         cmuClock_USART0
#define NCP_USART_LDMA_TX           SL_DMA_SIGNAL_USART0_TXBL
#define NCP_USART_LDMA_RX           SL_DMA_SIGNAL_USART0_RXDATAV
#define NCP_USART_ROUTE_INDEX       0
#define SLI_NCP_UART_RX_IRQ_HANDLER USART0_RX_IRQHandler

#define SPI_USART             USART0
#define SPI_USART_CMU_CLOCK   cmuClock_USART0
#define SPI_USART_LDMA_TX     ldmaPeripheralSignal_USART0_TXBL
#define SPI_USART_LDMA_RX     ldmaPeripheralSignal_USART0_RXDATAV
#define SPI_USART_ROUTE_INDEX 0

#ifdef SL_NCP_UART_INTERFACE
#define NCP_RX_IRQ USART0_RX_IRQn
#else
#define NCP_RX_IRQ GPIO_ODD_IRQn
#endif
#define PACKET_PENDING_INT_PRI 3
