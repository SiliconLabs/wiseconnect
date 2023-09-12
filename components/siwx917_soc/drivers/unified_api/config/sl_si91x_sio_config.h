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

// <<< Use Configuration Wizard in Context Menu >>>
#ifdef __cplusplus
extern "C" {
#endif
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

// <h>I2C Configuration

// <o SL_SIO_I2C_SAMPLE_RATE> Sample Rate <100000-4000000>
// <i> Default: 100000
#define SL_SIO_I2C_SAMPLE_RATE 100000

// <o SL_SIO_I2C_SCL_CHANNEL> SCL Channel
// <SL_SIO_CH_0=> Channel 0
// <SL_SIO_CH_1=> Channel 1
// <SL_SIO_CH_2=> Channel 2
// <SL_SIO_CH_3=> Channel 3
// <SL_SIO_CH_4=> Channel 4
// <SL_SIO_CH_5=> Channel 5
// <SL_SIO_CH_6=> Channel 6
// <SL_SIO_CH_7=> Channel 7
// <i> Default: SL_SIO_CH_6
#define SL_SIO_I2C_SCL_CHANNEL SL_SIO_CH_6

// <o SL_SIO_I2C_SDA_CHANNEL> SDA Channel
// <SL_SIO_CH_0=> Channel 0
// <SL_SIO_CH_1=> Channel 1
// <SL_SIO_CH_2=> Channel 2
// <SL_SIO_CH_3=> Channel 3
// <SL_SIO_CH_4=> Channel 4
// <SL_SIO_CH_5=> Channel 5
// <SL_SIO_CH_6=> Channel 6
// <SL_SIO_CH_7=> Channel 7
// <i> Default: SL_SIO_CH_7
#define SL_SIO_I2C_SDA_CHANNEL SL_SIO_CH_7
// </h>
#ifdef __cplusplus
}
#endif
// <<< end of configuration section >>>

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
#endif

#if defined(SL_SIO_I2C_SAMPLE_RATE)
stc_sio_i2c_config_t i2cConfigUc = {
  .u32SampleRate = SL_SIO_I2C_SAMPLE_RATE,
  .u8SioI2cScl   = SL_SIO_I2C_SCL_CHANNEL,
  .u8SioI2cSda   = SL_SIO_I2C_SDA_CHANNEL,
};
#endif

#endif //SL_SIO_CONFIG_H
