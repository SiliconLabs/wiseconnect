/***************************************************************************/ /**
 * @file
 * @brief Memory LCD interface
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

#include "sl_memlcd.h"
#include "sl_memlcd_display.h"
#include "sl_sleeptimer.h"
#include "RTE_Device_917.h"
#include "rsi_chip.h"
#include "rsi_board.h"
#include "sl_memlcd_spi.h"
#include "sl_si91x_ulp_timer.h"
#include "rsi_timers.h"
#include "rsi_rom_timer.h"
#include "rsi_rom_clks.h"
#include "rsi_rom_ulpss_clk.h"
#include "clock_update.h"
#include "sl_status.h"
#include "rsi_pll.h"
#include "base_types.h"

#define SL_MEMLCD_EXTCOMIN_PORT 0

#define SL_MEMLCD_SPI_CS_PORT   0
#define SL_MEMLCD_EXTCOMIN_PORT 0
#define SL_MEMLCD_SPI_CS_PIN    10
#define SL_MEMLCD_EXTCOMIN_PIN  3

#define CMD_UPDATE    0x01
#define CMD_ALL_CLEAR 0x04

/* Concatenate preprocessor tokens A and B. */
#define SL_CONCAT(A, B) A##B

#define ONE_USEC 32 // Ticks required for every one microsecond

#if defined(SL_MEMLCD_EXTCOMIN_PORT)
typedef struct sl_sleeptimer_timer_handle sl_sleeptimer_timer_handle_t;
/** Timer used for periodic maintenance of the display. */
static sl_sleeptimer_timer_handle_t extcomin_timer;
static void extcomin_toggle(sl_sleeptimer_timer_handle_t *handle, void *data);
#endif

/** Memory lcd instance. This variable will be initialized in the
 *  @ref sl_memlcd_configure() function. */
static sl_memlcd_t memlcd_instance;

/** Flag to monitor is this driver has been initialized. The memlcd_instance
 *  is only valid after initialized=true. */
static bool initialized = false;

/*******************************************************************************
 * Function to generate delay
 *
 * @param time_us (uint16_t) time in second
 * @return none
 ******************************************************************************/
static void delay_us(uint16_t time_us)
{
  uint32_t start_time;
  uint32_t temp;
  sl_si91x_ulp_timer_restart(TIMER_0);
  sl_si91x_ulp_timer_get_count(TIMER_0, &start_time);
  while (true) {
    sl_si91x_ulp_timer_get_count(TIMER_0, &temp);
    if ((temp - start_time) > (time_us * ONE_USEC)) {
      break;
    }
  }
}

sl_status_t sl_memlcd_configure(struct sl_memlcd_t *device)
{
  /* Initialize the SPI bus. */
  sli_memlcd_spi_init(device->spi_freq);

  /* Setup GPIOs */
  RSI_EGPIO_SetPinMux(EGPIO1, SL_MEMLCD_SPI_CS_PORT, SL_MEMLCD_SPI_CS_PIN, EGPIO_PIN_MUX_MODE0);
  // Set output direction
  RSI_EGPIO_SetDir(EGPIO1, SL_MEMLCD_SPI_CS_PORT, SL_MEMLCD_SPI_CS_PIN, EGPIO_CONFIG_DIR_OUTPUT);
  //clearing the GPIO pin
  RSI_EGPIO_SetPin(EGPIO1, SL_MEMLCD_SPI_CS_PORT, SL_MEMLCD_SPI_CS_PIN, 0);

#if defined(SL_MEMLCD_EXTCOMIN_PORT)
  // NPSS GPIO
  RSI_NPSSGPIO_InputBufferEn(SL_MEMLCD_EXTCOMIN_PIN, 1U);

  RSI_NPSSGPIO_SetPinMux(SL_MEMLCD_EXTCOMIN_PIN, 0);

  RSI_NPSSGPIO_SetDir(SL_MEMLCD_EXTCOMIN_PIN, NPSS_GPIO_DIR_OUTPUT);

  RSI_NPSSGPIO_SetPin(SL_MEMLCD_EXTCOMIN_PIN, 0U);
#endif

  memlcd_instance = *device;
  initialized     = true;
  sl_memlcd_power_on(device, true);
  sl_memlcd_clear(device);

  return SL_STATUS_OK;
}

sl_status_t sl_memlcd_refresh(const struct sl_memlcd_t *device)
{
  sli_memlcd_spi_init(device->spi_freq);
  return SL_STATUS_OK;
}

sl_status_t sl_memlcd_power_on(const struct sl_memlcd_t *device, bool on)
{
  (void)device;
  (void)on;

  sl_status_t status = SL_STATUS_OK;
  status             = sl_sleeptimer_init();

#if defined(SL_MEMLCD_EXTCOMIN_PORT)
  if (on) {
    uint32_t freq = sl_sleeptimer_get_timer_frequency();
    status        = sl_sleeptimer_restart_periodic_timer(&extcomin_timer,
                                                  freq / (device->extcomin_freq * 2),
                                                  extcomin_toggle,
                                                  NULL,
                                                  0,
                                                  SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
  } else {
    status = sl_sleeptimer_stop_timer(&extcomin_timer);
  }
#endif

  return status;
}

sl_status_t sl_memlcd_clear(const struct sl_memlcd_t *device)
{
  uint16_t cmd;
  cmd = CMD_ALL_CLEAR;

  /* Set SCS */
  RSI_EGPIO_SetPin(EGPIO1, SL_MEMLCD_SPI_CS_PORT, SL_MEMLCD_SPI_CS_PIN, 1);
  /* SCS setup time */
  delay_us(device->setup_us);
  /* Send command */
  sli_memlcd_spi_tx(&cmd, 2);
  sli_memlcd_spi_wait();

  /* SCS hold time */
  delay_us(device->hold_us);

  /* Clear SCS */
  RSI_EGPIO_SetPin(EGPIO1, SL_MEMLCD_SPI_CS_PORT, SL_MEMLCD_SPI_CS_PIN, 0);

  return SL_STATUS_OK;
}

sl_status_t sl_memlcd_draw(const struct sl_memlcd_t *device,
                           const void *data,
                           unsigned int row_start,
                           unsigned int row_count)
{
  unsigned int i;

  const uint8_t *p = data;
  uint16_t cmd;
  int row_len;
#if defined(SL_MEMLCD_LPM013M126A)
  uint16_t reversed_row;
#endif

  row_len = (device->width * device->bpp) / 8;
  row_start++;

  RSI_EGPIO_SetPin(EGPIO1, SL_MEMLCD_SPI_CS_PORT, SL_MEMLCD_SPI_CS_PIN, 1); //CS PIN IS A ULP PIN

  delay_us(device->setup_us);

#if defined(SL_MEMLCD_LPM013M126A)
  /* LPM013M126A uses MSB first for the row address */
  reversed_row = SL_RBIT16(row_start);

  /* Send update command and first line address */
  /* CMD_UPDATE is only 6 bits and the address line is 10 bits */
  cmd = CMD_UPDATE | reversed_row;
#else
  /* Send update command and first line address */
  cmd = CMD_UPDATE | (row_start << 8);
#endif

  sli_memlcd_spi_tx(&cmd, 2);

  /* Get start address to draw from */
  for (i = 0; i < row_count; i++) {
    /* Send pixels for this line */

    sli_memlcd_spi_tx(p, row_len);

    p += row_len;

    if (i == (row_count - 1)) {
      /* Dummy Data transfer at the end of update mode by multiple lines */
      cmd = 0xffff;
    } else {
#if defined(SL_MEMLCD_LPM013M126A)
      /* LPM013M126A uses MSB first for the row address*/
      reversed_row = SL_RBIT16(row_start + i + 1);

      /* Send multiple line address */
      /* Dummy data is only 6 bits and the address line is 10 bits */
      cmd = 0x3f | reversed_row;
#else
      /* Dummy data is 8 bits and the address line is 8 bit */
      cmd = 0xff | ((row_start + i + 1) << 8);
#endif
    }

    sli_memlcd_spi_tx(&cmd, 2);
  }

  sli_memlcd_spi_wait();
  delay_us(device->hold_us);

  RSI_EGPIO_SetPin(EGPIO1, SL_MEMLCD_SPI_CS_PORT, SL_MEMLCD_SPI_CS_PIN, 0);
  /* Clean up garbage RX data */
  /* This is important when paired with others slaves */
  sli_memlcd_spi_rx_flush();

  return SL_STATUS_OK;
}

const sl_memlcd_t *sl_memlcd_get(void)
{
  if (initialized) {
    return &memlcd_instance;
  } else {
    return NULL;
  }
}

/**************************************************************************/ /**
 * @brief
 *   Inverse polarity across the Liquid Crystal cells in the display.
 *
 * @detail
 *   This function inverses the polarity across the Liquid Crystal cells
 *   in the LCD. Must be called at least every second.
 *****************************************************************************/
#if defined(SL_MEMLCD_EXTCOMIN_PORT)
static void extcomin_toggle(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  (void)handle;
  (void)data;
  if (RSI_NPSSGPIO_GetPin(SL_MEMLCD_EXTCOMIN_PIN) == true) {
    RSI_NPSSGPIO_SetPin(SL_MEMLCD_EXTCOMIN_PIN, 0U);
  } else {
    RSI_NPSSGPIO_SetPin(SL_MEMLCD_EXTCOMIN_PIN, 1U);
  }
}
#endif