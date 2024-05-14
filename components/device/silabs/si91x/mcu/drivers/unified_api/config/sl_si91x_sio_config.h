/***************************************************************************/ /**
 * @file sl_si91x_sio_config.h
 * @brief SL SIO Config.
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_SIO_CONFIG_H
#define SL_SIO_CONFIG_H

#include "sl_si91x_sio.h"
#include "sl_sio_board.h"

// <<< Use Configuration Wizard in Context Menu >>>
#ifdef __cplusplus
extern "C" {
#endif

//  <e>SIO UC Configuration
//  <i> Enable: Peripheral configuration is taken straight from the configuration set in the universal configuration (UC).
//  <i> Disable: If the application demands it to be modified during runtime, use the sl_si91x_sio_spi_init API to modify the peripheral configuration.
//  <i> Default: 1
#define SIO_UC 1

// <h>SPI Configuration

// <o SL_SIO_SPI_CLK_FREQUENCY> Clock <9600-7372800>
// <i> Default: 1000000
#define SL_SIO_SPI_CLK_FREQUENCY 1000000

// <o SL_SIO_SPI_MODE> Mode
// <SL_SIO_SPI_MODE_0=> Mode 0
// <SL_SIO_SPI_MODE_3=> Mode 3
// <i> Default: SL_SIO_SPI_MODE_3
#define SL_SIO_SPI_MODE SL_SIO_SPI_MODE_3

// <o SL_SIO_SPI_CLK_CH> Clock Channel
// <SL_SIO_CH_0=> Channel 0
// <SL_SIO_CH_1=> Channel 1
// <SL_SIO_CH_2=> Channel 2
// <SL_SIO_CH_3=> Channel 3
// <SL_SIO_CH_4=> Channel 4
// <SL_SIO_CH_5=> Channel 5
// <SL_SIO_CH_6=> Channel 6
// <SL_SIO_CH_7=> Channel 7
// <i> Default: SL_SIO_CH_1
#define SL_SIO_SPI_CLK_CH SL_SIO_CH_1

// <o SL_SIO_SPI_MOSI_CH> MOSI Channel
// <SL_SIO_CH_0=> Channel 0
// <SL_SIO_CH_1=> Channel 1
// <SL_SIO_CH_2=> Channel 2
// <SL_SIO_CH_3=> Channel 3
// <SL_SIO_CH_4=> Channel 4
// <SL_SIO_CH_5=> Channel 5
// <SL_SIO_CH_6=> Channel 6
// <SL_SIO_CH_7=> Channel 7
// <i> Default: SL_SIO_CH_3
#define SL_SIO_SPI_MOSI_CH SL_SIO_CH_3

// <o SL_SIO_SPI_MISO_CH> MISO Channel
// <SL_SIO_CH_0=> Channel 0
// <SL_SIO_CH_1=> Channel 1
// <SL_SIO_CH_2=> Channel 2
// <SL_SIO_CH_3=> Channel 3
// <SL_SIO_CH_4=> Channel 4
// <SL_SIO_CH_5=> Channel 5
// <SL_SIO_CH_6=> Channel 6
// <SL_SIO_CH_7=> Channel 7
// <i> Default: SL_SIO_CH_2
#define SL_SIO_SPI_MISO_CH SL_SIO_CH_2

// <o SL_SIO_SPI_CS_CH> CS Channel
// <SL_SIO_CH_0=> Channel 0
// <SL_SIO_CH_1=> Channel 1
// <SL_SIO_CH_2=> Channel 2
// <SL_SIO_CH_3=> Channel 3
// <SL_SIO_CH_4=> Channel 4
// <SL_SIO_CH_5=> Channel 5
// <SL_SIO_CH_6=> Channel 6
// <SL_SIO_CH_7=> Channel 7
// <i> Default: SL_SIO_CH_0
#define SL_SIO_SPI_CS_CH SL_SIO_CH_0

// <o SL_SIO_SPI_BIT_LEN> Data Width
// <SL_SIO_SPI_BIT_8=> Data Width 8
// <SL_SIO_SPI_BIT_16=> Data Width 16
// <i> Default: SL_SIO_SPI_BIT_16
#define SL_SIO_SPI_BIT_LEN SL_SIO_SPI_BIT_16

// <o SL_SIO_SPI_MSB_LSB_FIRST> MSB/LSB First
// <SL_SIO_SPI_MSB_FIRST=> MSB First
// <SL_SIO_SPI_LSB_FIRST=> LSB First
// <i> Default: SL_SIO_SPI_MSB_FIRST
#define SL_SIO_SPI_MSB_LSB_FIRST SL_SIO_SPI_MSB_FIRST
// </h>

// <h>UART Configuration

// <o SL_SIO_UART_BAUD_RATE> Baud Rate <4800-128000>
// <i> Default: 115200
#define SL_SIO_UART_BAUD_RATE 115200

// <o SL_SIO_UART_BIT_LENGTH> Bit Length
// <SL_SIO_UART_BIT_8=> Bit Length 8
// <SL_SIO_UART_BIT_16=> Bit Length 9
// <i> Default: SL_SIO_UART_BIT_8
#define SL_SIO_UART_BIT_LENGTH SL_SIO_UART_BIT_8

// <o SL_SIO_UART_PARITY> Parity
// <SL_SIO_UART_EVEN_PARITY=> Even Parity 0
// <SL_SIO_UART_ODD_PARITY=> Odd Parity 1
// <i> Default: SL_SIO_UART_EVEN_PARITY
#define SL_SIO_UART_PARITY SL_SIO_UART_EVEN_PARITY

// <o SL_SIO_UART_RX_CH> Receive Channel
// <SL_SIO_CH_0=> Channel 0
// <SL_SIO_CH_1=> Channel 1
// <SL_SIO_CH_2=> Channel 2
// <SL_SIO_CH_3=> Channel 3
// <SL_SIO_CH_4=> Channel 4
// <SL_SIO_CH_5=> Channel 5
// <SL_SIO_CH_6=> Channel 6
// <SL_SIO_CH_7=> Channel 7
// <i> Default: SL_SIO_CH_2
#define SL_SIO_UART_RX_CH SL_SIO_CH_2

// <o SL_SIO_UART_TX_CH> Transmit channel
// <SL_SIO_CH_0=> Channel 0
// <SL_SIO_CH_1=> Channel 1
// <SL_SIO_CH_2=> Channel 2
// <SL_SIO_CH_3=> Channel 3
// <SL_SIO_CH_4=> Channel 4
// <SL_SIO_CH_5=> Channel 5
// <SL_SIO_CH_6=> Channel 6
// <SL_SIO_CH_7=> Channel 7
// <i> Default: SL_SIO_CH_3
#define SL_SIO_UART_TX_CH SL_SIO_CH_3

// <o SL_SIO_UART_STOP_BIT> Stop Bit
// <SL_SIO_UART_STOP_BIT_1=> Stop Bit 1
// <SL_SIO_UART_STOP_BIT_2=> Stop Bit 2
// <i> Default: SL_SIO_UART_STOP_BIT_1
#define SL_SIO_UART_STOP_BIT SL_SIO_UART_STOP_BIT_1

// </h>

// </e>

#define SL_SIO_I2C_SAMPLE_RATE 100000

#define SL_SIO_I2C_SCL_CHANNEL SL_SIO_CH_6

#define SL_SIO_I2C_SDA_CHANNEL SL_SIO_CH_7

#ifdef __cplusplus
}
#endif
// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <sio signal=SIO0,SIO1,SIO2,SIO3,SIO4,SIO5,SIO6,SIO7> SL_SIO
// $[SIO_SL_SIO]
#ifndef SL_SIO_PERIPHERAL
#define SL_SIO_PERIPHERAL SIO
#endif

// SIO SIO0 on GPIO_6
#ifndef SL_SIO_SIO0_PORT
#define SL_SIO_SIO0_PORT 0
#endif
#ifndef SL_SIO_SIO0_PIN
#define SL_SIO_SIO0_PIN 6
#endif
#ifndef SL_SIO_0_LOC
#define SL_SIO_0_LOC 0
#endif

// SIO SIO1 on GPIO_26
#ifndef SL_SIO_SIO1_PORT
#define SL_SIO_SIO1_PORT 0
#endif
#ifndef SL_SIO_SIO1_PIN
#define SL_SIO_SIO1_PIN 26
#endif
#ifndef SL_SIO_1_LOC
#define SL_SIO_1_LOC 1
#endif

// SIO SIO2 on GPIO_27
#ifndef SL_SIO_SIO2_PORT
#define SL_SIO_SIO2_PORT 0
#endif
#ifndef SL_SIO_SIO2_PIN
#define SL_SIO_SIO2_PIN 27
#endif
#ifndef SL_SIO_2_LOC
#define SL_SIO_2_LOC 1
#endif

// SIO SIO3 on GPIO_28
#ifndef SL_SIO_SIO3_PORT
#define SL_SIO_SIO3_PORT 0
#endif
#ifndef SL_SIO_SIO3_PIN
#define SL_SIO_SIO3_PIN 28
#endif
#ifndef SL_SIO_3_LOC
#define SL_SIO_3_LOC 1
#endif

// SIO SIO4 on GPIO_29
#ifndef SL_SIO_SIO4_PORT
#define SL_SIO_SIO4_PORT 0
#endif
#ifndef SL_SIO_SIO4_PIN
#define SL_SIO_SIO4_PIN 29
#endif
#ifndef SL_SIO_4_LOC
#define SL_SIO_4_LOC 1
#endif

// SIO SIO5 on GPIO_11
#ifndef SL_SIO_SIO5_PORT
#define SL_SIO_SIO5_PORT 0
#endif
#ifndef SL_SIO_SIO5_PIN
#define SL_SIO_SIO5_PIN 11
#endif
#ifndef SL_SIO_5_LOC
#define SL_SIO_5_LOC 0
#endif

// SIO SIO6 on ULP_GPIO_6/GPIO_70
#ifndef SL_SIO_SIO6_PORT
#define SL_SIO_SIO6_PORT 0
#endif
#ifndef SL_SIO_SIO6_PIN
#define SL_SIO_SIO6_PIN 6
#endif
#ifndef SL_SIO_6_LOC
#define SL_SIO_6_LOC 0
#endif

// SIO SIO7 on ULP_GPIO_7/GPIO_71
#ifndef SL_SIO_SIO7_PORT
#define SL_SIO_SIO7_PORT 0
#endif
#ifndef SL_SIO_SIO7_PIN
#define SL_SIO_SIO7_PIN 7
#endif
#ifndef SL_SIO_7_LOC
#define SL_SIO_7_LOC 1
#endif
// [SIO_SL_SIO]$
// <<< sl:end pin_tool >>>

#if defined(SL_SIO_SPI_CLK_FREQUENCY)
stc_sio_spi_cfg_t pstcSpiConfigUc = {
  .u32SpiClockFrq = SL_SIO_SPI_CLK_FREQUENCY,
  .u8BitOrder     = SL_SIO_SPI_MSB_LSB_FIRST,
  .u8SpiClkCh     = SL_SIO_SPI_CLK_CH,
  .u8SpiMosiCh    = SL_SIO_SPI_MOSI_CH,
  .u8SpiMisoCh    = SL_SIO_SPI_MISO_CH,
  .u8SpiCsCh      = SL_SIO_SPI_CS_CH,
  .u8BitLen       = SL_SIO_SPI_BIT_LEN,
  .u8Mode         = SL_SIO_SPI_MODE,
};

#if (SL_SIO_0_LOC == 2)
#define SL_SI91X_SIO0_PIN (SL_SIO_SIO0_PIN + 64)
#else
#define SL_SI91X_SIO0_PIN SL_SIO_SIO0_PIN
#endif
#if (SL_SIO_1_LOC == 2)
#define SL_SI91X_SIO1_PIN (SL_SIO_SIO1_PIN + 64)
#else
#define SL_SI91X_SIO1_PIN SL_SIO_SIO1_PIN
#endif
#if ((SL_SIO_2_LOC == 2) || (SL_SIO_2_LOC == 3))
#define SL_SI91X_SIO2_PIN (SL_SIO_SIO2_PIN + 64)
#else
#define SL_SI91X_SIO2_PIN SL_SIO_SIO2_PIN
#endif
#define SL_SI91X_SIO3_PIN SL_SIO_SIO3_PIN
#define SL_SI91X_SIO4_PIN SL_SIO_SIO4_PIN
#define SL_SI91X_SIO5_PIN SL_SIO_SIO5_PIN
#define SL_SI91X_SIO6_PIN (SL_SIO_SIO6_PIN + 64)
#define SL_SI91X_SIO7_PIN (SL_SIO_SIO7_PIN + 64)

#if (SL_SIO_SPI_CLK_CH == SL_SIO_CH_0)
#define SL_SI91X_SIO_SPI_CLK_PORT SL_SIO_SIO0_PORT
#define SL_SI91X_SIO_SPI_CLK_PIN  SL_SI91X_SIO0_PIN
#define SL_SI91X_SIO_SPI_CLK_MUX  SL_SI91X_SIO_0_MUX
#define SL_SI91X_SIO_SPI_CLK_PAD  SL_SI91X_SIO_0_PAD

#elif (SL_SIO_SPI_CLK_CH == SL_SIO_CH_1)
#define SL_SI91X_SIO_SPI_CLK_PORT SL_SIO_SIO1_PORT
#define SL_SI91X_SIO_SPI_CLK_PIN  SL_SI91X_SIO1_PIN
#define SL_SI91X_SIO_SPI_CLK_MUX  SL_SI91X_SIO_1_MUX
#define SL_SI91X_SIO_SPI_CLK_PAD  SL_SI91X_SIO_1_PAD

#elif (SL_SIO_SPI_CLK_CH == SL_SIO_CH_2)
#define SL_SI91X_SIO_SPI_CLK_PORT SL_SIO_SIO2_PORT
#define SL_SI91X_SIO_SPI_CLK_PIN  SL_SI91X_SIO2_PIN
#define SL_SI91X_SIO_SPI_CLK_MUX  SL_SI91X_SIO_2_MUX
#define SL_SI91X_SIO_SPI_CLK_PAD  SL_SI91X_SIO_2_PAD

#elif (SL_SIO_SPI_CLK_CH == SL_SIO_CH_3)
#define SL_SI91X_SIO_SPI_CLK_PORT SL_SIO_SIO3_PORT
#define SL_SI91X_SIO_SPI_CLK_PIN  SL_SI91X_SIO3_PIN
#define SL_SI91X_SIO_SPI_CLK_MUX  SL_SI91X_SIO_3_MUX
#define SL_SI91X_SIO_SPI_CLK_PAD  SL_SI91X_SIO_3_PAD

#elif (SL_SIO_SPI_CLK_CH == SL_SIO_CH_4)
#define SL_SI91X_SIO_SPI_CLK_PORT SL_SIO_SIO4_PORT
#define SL_SI91X_SIO_SPI_CLK_PIN  SL_SI91X_SIO4_PIN
#define SL_SI91X_SIO_SPI_CLK_MUX  SL_SI91X_SIO_4_MUX
#define SL_SI91X_SIO_SPI_CLK_PAD  SL_SI91X_SIO_4_PAD

#elif (SL_SIO_SPI_CLK_CH == SL_SIO_CH_5)
#define SL_SI91X_SIO_SPI_CLK_PORT SL_SIO_SIO5_PORT
#define SL_SI91X_SIO_SPI_CLK_PIN  SL_SI91X_SIO5_PIN
#define SL_SI91X_SIO_SPI_CLK_MUX  SL_SI91X_SIO_5_MUX
#define SL_SI91X_SIO_SPI_CLK_PAD  SL_SI91X_SIO_5_PAD

#elif (SL_SIO_SPI_CLK_CH == SL_SIO_CH_6)
#define SL_SI91X_SIO_SPI_CLK_PORT SL_SIO_SIO6_PORT
#define SL_SI91X_SIO_SPI_CLK_PIN  SL_SI91X_SIO6_PIN
#define SL_SI91X_SIO_SPI_CLK_MUX  SL_SI91X_SIO_6_MUX
#define SL_SI91X_SIO_SPI_CLK_PAD  SL_SI91X_SIO_6_PAD

#elif (SL_SIO_SPI_CLK_CH == SL_SIO_CH_7)
#define SL_SI91X_SIO_SPI_CLK_PORT SL_SIO_SIO7_PORT
#define SL_SI91X_SIO_SPI_CLK_PIN  SL_SI91X_SIO7_PIN
#define SL_SI91X_SIO_SPI_CLK_MUX  SL_SI91X_SIO_7_MUX
#define SL_SI91X_SIO_SPI_CLK_PAD  SL_SI91X_SIO_7_PAD
#endif

#if (SL_SIO_SPI_CS_CH == SL_SIO_CH_0)
#define SL_SI91X_SIO_SPI_CS_PORT SL_SIO_SIO0_PORT
#define SL_SI91X_SIO_SPI_CS_PIN  SL_SI91X_SIO0_PIN
#define SL_SI91X_SIO_SPI_CS_MUX  SL_SI91X_SIO_0_MUX
#define SL_SI91X_SIO_SPI_CS_PAD  SL_SI91X_SIO_0_PAD

#elif (SL_SIO_SPI_CS_CH == SL_SIO_CH_1)
#define SL_SI91X_SIO_SPI_CS_PORT SL_SIO_SIO1_PORT
#define SL_SI91X_SIO_SPI_CS_PIN  SL_SI91X_SIO1_PIN
#define SL_SI91X_SIO_SPI_CS_MUX  SL_SI91X_SIO_1_MUX
#define SL_SI91X_SIO_SPI_CS_PAD  SL_SI91X_SIO_1_PAD

#elif (SL_SIO_SPI_CS_CH == SL_SIO_CH_2)
#define SL_SI91X_SIO_SPI_CS_PORT SL_SIO_SIO2_PORT
#define SL_SI91X_SIO_SPI_CS_PIN  SL_SI91X_SIO2_PIN
#define SL_SI91X_SIO_SPI_CS_MUX  SL_SI91X_SIO_2_MUX
#define SL_SI91X_SIO_SPI_CS_PAD  SL_SI91X_SIO_2_PAD

#elif (SL_SIO_SPI_CS_CH == SL_SIO_CH_3)
#define SL_SI91X_SIO_SPI_CS_PORT SL_SIO_SIO3_PORT
#define SL_SI91X_SIO_SPI_CS_PIN  SL_SI91X_SIO3_PIN
#define SL_SI91X_SIO_SPI_CS_MUX  SL_SI91X_SIO_3_MUX
#define SL_SI91X_SIO_SPI_CS_PAD  SL_SI91X_SIO_3_PAD

#elif (SL_SIO_SPI_CS_CH == SL_SIO_CH_4)
#define SL_SI91X_SIO_SPI_CS_PORT SL_SIO_SIO4_PORT
#define SL_SI91X_SIO_SPI_CS_PIN  SL_SI91X_SIO4_PIN
#define SL_SI91X_SIO_SPI_CS_MUX  SL_SI91X_SIO_4_MUX
#define SL_SI91X_SIO_SPI_CS_PAD  SL_SI91X_SIO_4_PAD

#elif (SL_SIO_SPI_CS_CH == SL_SIO_CH_5)
#define SL_SI91X_SIO_SPI_CS_PORT SL_SIO_SIO5_PORT
#define SL_SI91X_SIO_SPI_CS_PIN  SL_SI91X_SIO5_PIN
#define SL_SI91X_SIO_SPI_CS_MUX  SL_SI91X_SIO_5_MUX
#define SL_SI91X_SIO_SPI_CS_PAD  SL_SI91X_SIO_5_PAD

#elif (SL_SIO_SPI_CS_CH == SL_SIO_CH_6)
#define SL_SI91X_SIO_SPI_CS_PORT SL_SIO_SIO6_PORT
#define SL_SI91X_SIO_SPI_CS_PIN  SL_SI91X_SIO6_PIN
#define SL_SI91X_SIO_SPI_CS_MUX  SL_SI91X_SIO_6_MUX
#define SL_SI91X_SIO_SPI_CS_PAD  SL_SI91X_SIO_6_PAD

#elif (SL_SIO_SPI_CS_CH == SL_SIO_CH_7)
#define SL_SI91X_SIO_SPI_CS_PORT SL_SIO_SIO7_PORT
#define SL_SI91X_SIO_SPI_CS_PIN  SL_SI91X_SIO7_PIN
#define SL_SI91X_SIO_SPI_CS_MUX  SL_SI91X_SIO_7_MUX
#define SL_SI91X_SIO_SPI_CS_PAD  SL_SI91X_SIO_7_PAD
#endif

#if (SL_SIO_SPI_MOSI_CH == SL_SIO_CH_0)
#define SL_SI91X_SIO_SPI_MOSI_PORT SL_SIO_SIO0_PORT
#define SL_SI91X_SIO_SPI_MOSI_PIN  SL_SI91X_SIO0_PIN
#define SL_SI91X_SIO_SPI_MOSI_MUX  SL_SI91X_SIO_0_MUX
#define SL_SI91X_SIO_SPI_MOSI_PAD  SL_SI91X_SIO_0_PAD

#elif (SL_SIO_SPI_MOSI_CH == SL_SIO_CH_1)
#define SL_SI91X_SIO_SPI_MOSI_PORT SL_SIO_SIO1_PORT
#define SL_SI91X_SIO_SPI_MOSI_PIN  SL_SI91X_SIO1_PIN
#define SL_SI91X_SIO_SPI_MOSI_MUX  SL_SI91X_SIO_1_MUX
#define SL_SI91X_SIO_SPI_MOSI_PAD  SL_SI91X_SIO_1_PAD

#elif (SL_SIO_SPI_MOSI_CH == SL_SIO_CH_2)
#define SL_SI91X_SIO_SPI_MOSI_PORT SL_SIO_SIO2_PORT
#define SL_SI91X_SIO_SPI_MOSI_PIN  SL_SI91X_SIO2_PIN
#define SL_SI91X_SIO_SPI_MOSI_MUX  SL_SI91X_SIO_2_MUX
#define SL_SI91X_SIO_SPI_MOSI_PAD  SL_SI91X_SIO_2_PAD

#elif (SL_SIO_SPI_MOSI_CH == SL_SIO_CH_3)
#define SL_SI91X_SIO_SPI_MOSI_PORT SL_SIO_SIO3_PORT
#define SL_SI91X_SIO_SPI_MOSI_PIN  SL_SI91X_SIO3_PIN
#define SL_SI91X_SIO_SPI_MOSI_MUX  SL_SI91X_SIO_3_MUX
#define SL_SI91X_SIO_SPI_MOSI_PAD  SL_SI91X_SIO_3_PAD

#elif (SL_SIO_SPI_MOSI_CH == SL_SIO_CH_4)
#define SL_SI91X_SIO_SPI_MOSI_PORT SL_SIO_SIO4_PORT
#define SL_SI91X_SIO_SPI_MOSI_PIN  SL_SI91X_SIO4_PIN
#define SL_SI91X_SIO_SPI_MOSI_MUX  SL_SI91X_SIO_4_MUX
#define SL_SI91X_SIO_SPI_MOSI_PAD  SL_SI91X_SIO_4_PAD

#elif (SL_SIO_SPI_MOSI_CH == SL_SIO_CH_5)
#define SL_SI91X_SIO_SPI_MOSI_PORT SL_SIO_SIO5_PORT
#define SL_SI91X_SIO_SPI_MOSI_PIN  SL_SI91X_SIO5_PIN
#define SL_SI91X_SIO_SPI_MOSI_MUX  SL_SI91X_SIO_5_MUX
#define SL_SI91X_SIO_SPI_MOSI_PAD  SL_SI91X_SIO_5_PAD

#elif (SL_SIO_SPI_MOSI_CH == SL_SIO_CH_6)
#define SL_SI91X_SIO_SPI_MOSI_PORT SL_SIO_SIO6_PORT
#define SL_SI91X_SIO_SPI_MOSI_PIN  SL_SI91X_SIO6_PIN
#define SL_SI91X_SIO_SPI_MOSI_MUX  SL_SI91X_SIO_6_MUX
#define SL_SI91X_SIO_SPI_MOSI_PAD  SL_SI91X_SIO_6_PAD

#elif (SL_SIO_SPI_MOSI_CH == SL_SIO_CH_7)
#define SL_SI91X_SIO_SPI_MOSI_PORT SL_SIO_SIO7_PORT
#define SL_SI91X_SIO_SPI_MOSI_PIN  SL_SI91X_SIO7_PIN
#define SL_SI91X_SIO_SPI_MOSI_MUX  SL_SI91X_SIO_7_MUX
#define SL_SI91X_SIO_SPI_MOSI_PAD  SL_SI91X_SIO_7_PAD
#endif

#if (SL_SIO_SPI_MISO_CH == SL_SIO_CH_0)
#define SL_SI91X_SIO_SPI_MISO_PORT SL_SIO_SIO0_PORT
#define SL_SI91X_SIO_SPI_MISO_PIN  SL_SI91X_SIO0_PIN
#define SL_SI91X_SIO_SPI_MISO_MUX  SL_SI91X_SIO_0_MUX
#define SL_SI91X_SIO_SPI_MISO_PAD  SL_SI91X_SIO_0_PAD

#elif (SL_SIO_SPI_MISO_CH == SL_SIO_CH_1)
#define SL_SI91X_SIO_SPI_MISO_PORT SL_SIO_SIO1_PORT
#define SL_SI91X_SIO_SPI_MISO_PIN  SL_SI91X_SIO1_PIN
#define SL_SI91X_SIO_SPI_MISO_MUX  SL_SI91X_SIO_1_MUX
#define SL_SI91X_SIO_SPI_MISO_PAD  SL_SI91X_SIO_1_PAD

#elif (SL_SIO_SPI_MISO_CH == SL_SIO_CH_2)
#define SL_SI91X_SIO_SPI_MISO_PORT SL_SIO_SIO2_PORT
#define SL_SI91X_SIO_SPI_MISO_PIN  SL_SI91X_SIO2_PIN
#define SL_SI91X_SIO_SPI_MISO_MUX  SL_SI91X_SIO_2_MUX
#define SL_SI91X_SIO_SPI_MISO_PAD  SL_SI91X_SIO_2_PAD

#elif (SL_SIO_SPI_MISO_CH == SL_SIO_CH_3)
#define SL_SI91X_SIO_SPI_MISO_PORT SL_SIO_SIO3_PORT
#define SL_SI91X_SIO_SPI_MISO_PIN  SL_SI91X_SIO3_PIN
#define SL_SI91X_SIO_SPI_MISO_MUX  SL_SI91X_SIO_3_MUX
#define SL_SI91X_SIO_SPI_MISO_PAD  SL_SI91X_SIO_3_PAD

#elif (SL_SIO_SPI_MISO_CH == SL_SIO_CH_4)
#define SL_SI91X_SIO_SPI_MISO_PORT SL_SIO_SIO4_PORT
#define SL_SI91X_SIO_SPI_MISO_PIN  SL_SI91X_SIO4_PIN
#define SL_SI91X_SIO_SPI_MISO_MUX  SL_SI91X_SIO_4_MUX
#define SL_SI91X_SIO_SPI_MISO_PAD  SL_SI91X_SIO_4_PAD

#elif (SL_SIO_SPI_MISO_CH == SL_SIO_CH_5)
#define SL_SI91X_SIO_SPI_MISO_PORT SL_SIO_SIO5_PORT
#define SL_SI91X_SIO_SPI_MISO_PIN  SL_SI91X_SIO5_PIN
#define SL_SI91X_SIO_SPI_MISO_MUX  SL_SI91X_SIO_5_MUX
#define SL_SI91X_SIO_SPI_MISO_PAD  SL_SI91X_SIO_5_PAD

#elif (SL_SIO_SPI_MISO_CH == SL_SIO_CH_6)
#define SL_SI91X_SIO_SPI_MISO_PORT SL_SIO_SIO6_PORT
#define SL_SI91X_SIO_SPI_MISO_PIN  SL_SI91X_SIO6_PIN
#define SL_SI91X_SIO_SPI_MISO_MUX  SL_SI91X_SIO_6_MUX
#define SL_SI91X_SIO_SPI_MISO_PAD  SL_SI91X_SIO_6_PAD

#elif (SL_SIO_SPI_MISO_CH == SL_SIO_CH_7)
#define SL_SI91X_SIO_SPI_MISO_PORT SL_SIO_SIO7_PORT
#define SL_SI91X_SIO_SPI_MISO_PIN  SL_SI91X_SIO7_PIN
#define SL_SI91X_SIO_SPI_MISO_MUX  SL_SI91X_SIO_7_MUX
#define SL_SI91X_SIO_SPI_MISO_PAD  SL_SI91X_SIO_7_PAD
#endif

sl_sio_spi_t sl_sio_spi_init = {
  .spi_cs_port   = SL_SI91X_SIO_SPI_CS_PORT,
  .spi_cs_pin    = SL_SI91X_SIO_SPI_CS_PIN,
  .spi_cs_mux    = SL_SI91X_SIO_SPI_CS_MUX,
  .spi_cs_pad    = SL_SI91X_SIO_SPI_CS_PAD,
  .spi_clk_port  = SL_SI91X_SIO_SPI_CLK_PORT,
  .spi_clk_pin   = SL_SI91X_SIO_SPI_CLK_PIN,
  .spi_clk_mux   = SL_SI91X_SIO_SPI_CLK_MUX,
  .spi_clk_pad   = SL_SI91X_SIO_SPI_CLK_PAD,
  .spi_mosi_port = SL_SI91X_SIO_SPI_MOSI_PORT,
  .spi_mosi_pin  = SL_SI91X_SIO_SPI_MOSI_PIN,
  .spi_mosi_mux  = SL_SI91X_SIO_SPI_MOSI_MUX,
  .spi_mosi_pad  = SL_SI91X_SIO_SPI_MOSI_PAD,
  .spi_miso_port = SL_SI91X_SIO_SPI_MISO_PORT,
  .spi_miso_pin  = SL_SI91X_SIO_SPI_MISO_PIN,
  .spi_miso_mux  = SL_SI91X_SIO_SPI_MISO_MUX,
  .spi_miso_pad  = SL_SI91X_SIO_SPI_MISO_PAD,
};
#endif

#if defined(SL_SIO_UART_BAUD_RATE)
stc_sio_uart_config_t UartInitstcUc = {
  .u32BaudRate        = SL_SIO_UART_BAUD_RATE,
  .u8Bitlen           = SL_SIO_UART_BIT_8,
  .u8Parity           = SL_SIO_UART_EVEN_PARITY,
  .u8SioUartRxChannel = SL_SIO_UART_RX_CH,
  .u8SioUartTxChannel = SL_SIO_UART_TX_CH,
  .u8StopBits         = SL_SIO_UART_STOP_BIT,
};

#if (SL_SIO_UART_TX_CH == SL_SIO_CH_0)
#define SL_SI91X_SIO_UART_TX_PORT SL_SIO_SIO0_PORT
#define SL_SI91X_SIO_UART_TX_PIN  SL_SI91X_SIO0_PIN
#define SL_SI91X_SIO_UART_TX_MUX  SL_SI91X_SIO_0_MUX
#define SL_SI91X_SIO_UART_TX_PAD  SL_SI91X_SIO_0_PAD

#elif (SL_SIO_UART_TX_CH == SL_SIO_CH_1)
#define SL_SI91X_SIO_UART_TX_PORT SL_SIO_SIO1_PORT
#define SL_SI91X_SIO_UART_TX_PIN  SL_SI91X_SIO1_PIN
#define SL_SI91X_SIO_UART_TX_MUX  SL_SI91X_SIO_1_MUX
#define SL_SI91X_SIO_UART_TX_PAD  SL_SI91X_SIO_1_PAD

#elif (SL_SIO_UART_TX_CH == SL_SIO_CH_2)
#define SL_SI91X_SIO_UART_TX_PORT SL_SIO_SIO2_PORT
#define SL_SI91X_SIO_UART_TX_PIN  SL_SI91X_SIO2_PIN
#define SL_SI91X_SIO_UART_TX_MUX  SL_SI91X_SIO_2_MUX
#define SL_SI91X_SIO_UART_TX_PAD  SL_SI91X_SIO_2_PAD

#elif (SL_SIO_UART_TX_CH == SL_SIO_CH_3)
#define SL_SI91X_SIO_UART_TX_PORT SL_SIO_SIO3_PORT
#define SL_SI91X_SIO_UART_TX_PIN  SL_SI91X_SIO3_PIN
#define SL_SI91X_SIO_UART_TX_MUX  SL_SI91X_SIO_3_MUX
#define SL_SI91X_SIO_UART_TX_PAD  SL_SI91X_SIO_3_PAD

#elif (SL_SIO_UART_TX_CH == SL_SIO_CH_4)
#define SL_SI91X_SIO_UART_TX_PORT SL_SIO_SIO4_PORT
#define SL_SI91X_SIO_UART_TX_PIN  SL_SI91X_SIO4_PIN
#define SL_SI91X_SIO_UART_TX_MUX  SL_SI91X_SIO_4_MUX
#define SL_SI91X_SIO_UART_TX_PAD  SL_SI91X_SIO_4_PAD

#elif (SL_SIO_UART_TX_CH == SL_SIO_CH_5)
#define SL_SI91X_SIO_UART_TX_PORT SL_SIO_SIO5_PORT
#define SL_SI91X_SIO_UART_TX_PIN  SL_SI91X_SIO5_PIN
#define SL_SI91X_SIO_UART_TX_MUX  SL_SI91X_SIO_5_MUX
#define SL_SI91X_SIO_UART_TX_PAD  SL_SI91X_SIO_5_PAD

#elif (SL_SIO_UART_TX_CH == SL_SIO_CH_6)
#define SL_SI91X_SIO_UART_TX_PORT SL_SIO_SIO6_PORT
#define SL_SI91X_SIO_UART_TX_PIN  SL_SI91X_SIO6_PIN
#define SL_SI91X_SIO_UART_TX_MUX  SL_SI91X_SIO_6_MUX
#define SL_SI91X_SIO_UART_TX_PAD  SL_SI91X_SIO_6_PAD

#elif (SL_SIO_UART_TX_CH == SL_SIO_CH_7)
#define SL_SI91X_SIO_UART_TX_PORT SL_SIO_SIO7_PORT
#define SL_SI91X_SIO_UART_TX_PIN  SL_SI91X_SIO7_PIN
#define SL_SI91X_SIO_UART_TX_MUX  SL_SI91X_SIO_7_MUX
#define SL_SI91X_SIO_UART_TX_PAD  SL_SI91X_SIO_7_PAD
#endif

#if (SL_SIO_UART_RX_CH == SL_SIO_CH_0)
#define SL_SI91X_SIO_UART_RX_PORT SL_SIO_SIO0_PORT
#define SL_SI91X_SIO_UART_RX_PIN  SL_SI91X_SIO0_PIN
#define SL_SI91X_SIO_UART_RX_MUX  SL_SI91X_SIO_0_MUX
#define SL_SI91X_SIO_UART_RX_PAD  SL_SI91X_SIO_0_PAD

#elif (SL_SIO_UART_RX_CH == SL_SIO_CH_1)
#define SL_SI91X_SIO_UART_RX_PORT SL_SIO_SIO1_PORT
#define SL_SI91X_SIO_UART_RX_PIN  SL_SI91X_SIO1_PIN
#define SL_SI91X_SIO_UART_RX_MUX  SL_SI91X_SIO_1_MUX
#define SL_SI91X_SIO_UART_RX_PAD  SL_SI91X_SIO_1_PAD

#elif (SL_SIO_UART_RX_CH == SL_SIO_CH_2)
#define SL_SI91X_SIO_UART_RX_PORT SL_SIO_SIO2_PORT
#define SL_SI91X_SIO_UART_RX_PIN  SL_SI91X_SIO2_PIN
#define SL_SI91X_SIO_UART_RX_MUX  SL_SI91X_SIO_2_MUX
#define SL_SI91X_SIO_UART_RX_PAD  SL_SI91X_SIO_2_PAD

#elif (SL_SIO_UART_RX_CH == SL_SIO_CH_3)
#define SL_SI91X_SIO_UART_RX_PORT SL_SIO_SIO3_PORT
#define SL_SI91X_SIO_UART_RX_PIN  SL_SI91X_SIO3_PIN
#define SL_SI91X_SIO_UART_RX_MUX  SL_SI91X_SIO_3_MUX
#define SL_SI91X_SIO_UART_RX_PAD  SL_SI91X_SIO_3_PAD

#elif (SL_SIO_UART_RX_CH == SL_SIO_CH_4)
#define SL_SI91X_SIO_UART_RX_PORT SL_SIO_SIO4_PORT
#define SL_SI91X_SIO_UART_RX_PIN  SL_SI91X_SIO4_PIN
#define SL_SI91X_SIO_UART_RX_MUX  SL_SI91X_SIO_4_MUX
#define SL_SI91X_SIO_UART_RX_PAD  SL_SI91X_SIO_4_PAD

#elif (SL_SIO_UART_RX_CH == SL_SIO_CH_5)
#define SL_SI91X_SIO_UART_RX_PORT SL_SIO_SIO5_PORT
#define SL_SI91X_SIO_UART_RX_PIN  SL_SI91X_SIO5_PIN
#define SL_SI91X_SIO_UART_RX_MUX  SL_SI91X_SIO_5_MUX
#define SL_SI91X_SIO_UART_RX_PAD  SL_SI91X_SIO_5_PAD

#elif (SL_SIO_UART_RX_CH == SL_SIO_CH_6)
#define SL_SI91X_SIO_UART_RX_PORT SL_SIO_SIO6_PORT
#define SL_SI91X_SIO_UART_RX_PIN  SL_SI91X_SIO6_PIN
#define SL_SI91X_SIO_UART_RX_MUX  SL_SI91X_SIO_6_MUX
#define SL_SI91X_SIO_UART_RX_PAD  SL_SI91X_SIO_6_PAD

#elif (SL_SIO_UART_RX_CH == SL_SIO_CH_7)
#define SL_SI91X_SIO_UART_RX_PORT SL_SIO_SIO7_PORT
#define SL_SI91X_SIO_UART_RX_PIN  SL_SI91X_SIO7_PIN
#define SL_SI91X_SIO_UART_RX_MUX  SL_SI91X_SIO_7_MUX
#define SL_SI91X_SIO_UART_RX_PAD  SL_SI91X_SIO_7_PAD
#endif

sl_sio_uart_t sl_sio_uart_init = {
  .uart_tx_port = SL_SI91X_SIO_UART_TX_PORT,
  .uart_tx_pin  = SL_SI91X_SIO_UART_TX_PIN,
  .uart_tx_mux  = SL_SI91X_SIO_UART_TX_MUX,
  .uart_tx_pad  = SL_SI91X_SIO_UART_TX_PAD,
  .uart_rx_port = SL_SI91X_SIO_UART_RX_PORT,
  .uart_rx_pin  = SL_SI91X_SIO_UART_RX_PIN,
  .uart_rx_mux  = SL_SI91X_SIO_UART_RX_MUX,
  .uart_rx_pad  = SL_SI91X_SIO_UART_RX_PAD,
};
#endif

#if defined(SL_SIO_I2C_SAMPLE_RATE)
stc_sio_i2c_config_t i2cConfigUc = {
  .u32SampleRate = SL_SIO_I2C_SAMPLE_RATE,
  .u8SioI2cScl   = SL_SIO_I2C_SCL_CHANNEL,
  .u8SioI2cSda   = SL_SIO_I2C_SDA_CHANNEL,
};

#if (SL_SIO_I2C_SCL_CHANNEL == SL_SIO_CH_0)
#define SL_SI91X_SIO_I2C_SCL_PORT SL_SIO_SIO0_PORT
#define SL_SI91X_SIO_I2C_SCL_PIN  SL_SI91X_SIO0_PIN
#define SL_SI91X_SIO_I2C_SCL_MUX  SL_SI91X_SIO_0_MUX
#define SL_SI91X_SIO_I2C_SCL_PAD  SL_SI91X_SIO_0_PAD

#elif (SL_SIO_I2C_SCL_CHANNEL == SL_SIO_CH_1)
#define SL_SI91X_SIO_I2C_SCL_PORT SL_SIO_SIO1_PORT
#define SL_SI91X_SIO_I2C_SCL_PIN  SL_SI91X_SIO1_PIN
#define SL_SI91X_SIO_I2C_SCL_MUX  SL_SI91X_SIO_1_MUX
#define SL_SI91X_SIO_I2C_SCL_PAD  SL_SI91X_SIO_1_PAD

#elif (SL_SIO_I2C_SCL_CHANNEL == SL_SIO_CH_2)
#define SL_SI91X_SIO_I2C_SCL_PORT SL_SIO_SIO2_PORT
#define SL_SI91X_SIO_I2C_SCL_PIN  SL_SI91X_SIO2_PIN
#define SL_SI91X_SIO_I2C_SCL_MUX  SL_SI91X_SIO_2_MUX
#define SL_SI91X_SIO_I2C_SCL_PAD  SL_SI91X_SIO_2_PAD

#elif (SL_SIO_I2C_SCL_CHANNEL == SL_SIO_CH_3)
#define SL_SI91X_SIO_I2C_SCL_PORT SL_SIO_SIO3_PORT
#define SL_SI91X_SIO_I2C_SCL_PIN  SL_SI91X_SIO3_PIN
#define SL_SI91X_SIO_I2C_SCL_MUX  SL_SI91X_SIO_3_MUX
#define SL_SI91X_SIO_I2C_SCL_PAD  SL_SI91X_SIO_3_PAD

#elif (SL_SIO_I2C_SCL_CHANNEL == SL_SIO_CH_4)
#define SL_SI91X_SIO_I2C_SCL_PORT SL_SIO_SIO4_PORT
#define SL_SI91X_SIO_I2C_SCL_PIN  SL_SI91X_SIO4_PIN
#define SL_SI91X_SIO_I2C_SCL_MUX  SL_SI91X_SIO_4_MUX
#define SL_SI91X_SIO_I2C_SCL_PAD  SL_SI91X_SIO_4_PAD

#elif (SL_SIO_I2C_SCL_CHANNEL == SL_SIO_CH_5)
#define SL_SI91X_SIO_I2C_SCL_PORT SL_SIO_SIO5_PORT
#define SL_SI91X_SIO_I2C_SCL_PIN  SL_SI91X_SIO5_PIN
#define SL_SI91X_SIO_I2C_SCL_MUX  SL_SI91X_SIO_5_MUX
#define SL_SI91X_SIO_I2C_SCL_PAD  SL_SI91X_SIO_5_PAD

#elif (SL_SIO_I2C_SCL_CHANNEL == SL_SIO_CH_6)
#define SL_SI91X_SIO_I2C_SCL_PORT SL_SIO_SIO6_PORT
#define SL_SI91X_SIO_I2C_SCL_PIN  SL_SI91X_SIO6_PIN
#define SL_SI91X_SIO_I2C_SCL_MUX  SL_SI91X_SIO_6_MUX
#define SL_SI91X_SIO_I2C_SCL_PAD  SL_SI91X_SIO_6_PAD

#elif (SL_SIO_I2C_SCL_CHANNEL == SL_SIO_CH_7)
#define SL_SI91X_SIO_I2C_SCL_PORT SL_SIO_SIO7_PORT
#define SL_SI91X_SIO_I2C_SCL_PIN  SL_SI91X_SIO7_PIN
#define SL_SI91X_SIO_I2C_SCL_MUX  SL_SI91X_SIO_7_MUX
#define SL_SI91X_SIO_I2C_SCL_PAD  SL_SI91X_SIO_7_PAD
#endif

#if (SL_SIO_I2C_SDA_CHANNEL == SL_SIO_CH_0)
#define SL_SI91X_SIO_I2C_SDA_PORT SL_SIO_SIO0_PORT
#define SL_SI91X_SIO_I2C_SDA_PIN  SL_SI91X_SIO0_PIN
#define SL_SI91X_SIO_I2C_SDA_MUX  SL_SI91X_SIO_0_MUX
#define SL_SI91X_SIO_I2C_SDA_PAD  SL_SI91X_SIO_0_PAD

#elif (SL_SIO_I2C_SDA_CHANNEL == SL_SIO_CH_1)
#define SL_SI91X_SIO_I2C_SDA_PORT SL_SIO_SIO1_PORT
#define SL_SI91X_SIO_I2C_SDA_PIN  SL_SI91X_SIO1_PIN
#define SL_SI91X_SIO_I2C_SDA_MUX  SL_SI91X_SIO_1_MUX
#define SL_SI91X_SIO_I2C_SDA_PAD  SL_SI91X_SIO_1_PAD

#elif (SL_SIO_I2C_SDA_CHANNEL == SL_SIO_CH_2)
#define SL_SI91X_SIO_I2C_SDA_PORT SL_SIO_SIO2_PORT
#define SL_SI91X_SIO_I2C_SDA_PIN  SL_SI91X_SIO2_PIN
#define SL_SI91X_SIO_I2C_SDA_MUX  SL_SI91X_SIO_2_MUX
#define SL_SI91X_SIO_I2C_SDA_PAD  SL_SI91X_SIO_2_PAD

#elif (SL_SIO_I2C_SDA_CHANNEL == SL_SIO_CH_3)
#define SL_SI91X_SIO_I2C_SDA_PORT SL_SIO_SIO3_PORT
#define SL_SI91X_SIO_I2C_SDA_PIN  SL_SI91X_SIO3_PIN
#define SL_SI91X_SIO_I2C_SDA_MUX  SL_SI91X_SIO_3_MUX
#define SL_SI91X_SIO_I2C_SDA_PAD  SL_SI91X_SIO_3_PAD

#elif (SL_SIO_I2C_SDA_CHANNEL == SL_SIO_CH_4)
#define SL_SI91X_SIO_I2C_SDA_PORT SL_SIO_SIO4_PORT
#define SL_SI91X_SIO_I2C_SDA_PIN  SL_SI91X_SIO4_PIN
#define SL_SI91X_SIO_I2C_SDA_MUX  SL_SI91X_SIO_4_MUX
#define SL_SI91X_SIO_I2C_SDA_PAD  SL_SI91X_SIO_4_PAD

#elif (SL_SIO_I2C_SDA_CHANNEL == SL_SIO_CH_5)
#define SL_SI91X_SIO_I2C_SDA_PORT SL_SIO_SIO5_PORT
#define SL_SI91X_SIO_I2C_SDA_PIN  SL_SI91X_SIO5_PIN
#define SL_SI91X_SIO_I2C_SDA_MUX  SL_SI91X_SIO_5_MUX
#define SL_SI91X_SIO_I2C_SDA_PAD  SL_SI91X_SIO_5_PAD

#elif (SL_SIO_I2C_SDA_CHANNEL == SL_SIO_CH_6)
#define SL_SI91X_SIO_I2C_SDA_PORT SL_SIO_SIO6_PORT
#define SL_SI91X_SIO_I2C_SDA_PIN  SL_SI91X_SIO6_PIN
#define SL_SI91X_SIO_I2C_SDA_MUX  SL_SI91X_SIO_6_MUX
#define SL_SI91X_SIO_I2C_SDA_PAD  SL_SI91X_SIO_6_PAD

#elif (SL_SIO_I2C_SDA_CHANNEL == SL_SIO_CH_7)
#define SL_SI91X_SIO_I2C_SDA_PORT SL_SIO_SIO7_PORT
#define SL_SI91X_SIO_I2C_SDA_PIN  SL_SI91X_SIO7_PIN
#define SL_SI91X_SIO_I2C_SDA_MUX  SL_SI91X_SIO_7_MUX
#define SL_SI91X_SIO_I2C_SDA_PAD  SL_SI91X_SIO_7_PAD
#endif

sl_sio_i2c_t sl_sio_i2c_init = {
  .i2c_scl_port = SL_SI91X_SIO_I2C_SCL_PORT,
  .i2c_scl_pin  = SL_SI91X_SIO_I2C_SCL_PIN,
  .i2c_scl_mux  = SL_SI91X_SIO_I2C_SCL_MUX,
  .i2c_scl_pad  = SL_SI91X_SIO_I2C_SCL_PAD,
  .i2c_sda_port = SL_SI91X_SIO_I2C_SDA_PORT,
  .i2c_sda_pin  = SL_SI91X_SIO_I2C_SDA_PIN,
  .i2c_sda_mux  = SL_SI91X_SIO_I2C_SDA_MUX,
  .i2c_sda_pad  = SL_SI91X_SIO_I2C_SDA_PAD,
};
#endif

#endif //SL_SIO_CONFIG_H