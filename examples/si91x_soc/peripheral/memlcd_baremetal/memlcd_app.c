/***************************************************************************/ /**
 * @file
 * @brief Sleeptimer sample application
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

#include "sl_sleeptimer.h"
#include "sl_sleeptimer_config.h"
#include "app.h"
#include "rsi_board.h"
#include "glib.h"
#include "dmd.h"
#include "RTE_Device_917.h"
#include "sl_status.h"
#include "rsi_ccp_user_config.h"
#include "rsi_chip.h"
#include "em_assert.h"

#define SL_BOARD_ENABLE_DISPLAY_PIN  0
#define SL_BOARD_ENABLE_DISPLAY_PORT 0

static GLIB_Context_t glibContext;
static int currentLine = 0;

void memlcd_app_init(void)
{
  sl_status_t status;

  // Enabling LCD display
  RSI_NPSSGPIO_InputBufferEn(SL_BOARD_ENABLE_DISPLAY_PIN, 1U);
  RSI_NPSSGPIO_SetPinMux(SL_BOARD_ENABLE_DISPLAY_PIN, 0);
  RSI_NPSSGPIO_SetDir(SL_BOARD_ENABLE_DISPLAY_PIN, 0);
  RSI_NPSSGPIO_SetPin(SL_BOARD_ENABLE_DISPLAY_PIN, 1U);

  /* Initialize the DMD support for memory lcd display */
  status = DMD_init(0);
  EFM_ASSERT(status == DMD_OK);

  /* Initialize the glib context */
  status = GLIB_contextInit(&glibContext);
  EFM_ASSERT(status == GLIB_OK);

  glibContext.backgroundColor = White;
  glibContext.foregroundColor = Black;

  /* Fill lcd with background color */
  GLIB_clear(&glibContext);

  /* Use Narrow font */
  GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNarrow6x8);

  /* Draw text on the memory lcd display*/

  GLIB_drawStringOnLine(&glibContext, "MEMLCD Sample App", currentLine++, GLIB_ALIGN_LEFT, 5, 5, true);
  GLIB_drawStringOnLine(&glibContext, "Hello There!", currentLine++, GLIB_ALIGN_LEFT, 5, 5, true);

  DMD_updateDisplay();
}