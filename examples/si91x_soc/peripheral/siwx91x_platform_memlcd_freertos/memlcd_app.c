/***************************************************************************/ /**
 * @file memlcd_app.c
 * @brief MEMLCD + GLIB drawing for FreeRTOS periodic refresh
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

#include <stdio.h>

#include "glib.h"
#include "sl_memlcd.h"
#include "dmd.h"
#include "sl_assert.h"

#include "memlcd_app.h"

#define END_OF_SCREEN 12

volatile int currentLine = 0;

static GLIB_Context_t glibContext;

void memlcd_app_init(void)
{
  sl_status_t status;

  sl_memlcd_display_enable();

  status = DMD_init(0);
  EFM_ASSERT(status == DMD_OK);

  status = GLIB_contextInit(&glibContext);
  EFM_ASSERT(status == GLIB_OK);

  glibContext.backgroundColor = White;
  glibContext.foregroundColor = Black;

  GLIB_clear(&glibContext);
  GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNarrow6x8);

  GLIB_drawStringOnLine(&glibContext, "MEMLCD Sample App", currentLine++, GLIB_ALIGN_LEFT, 5, 5, true);

  DMD_updateDisplay();
}

void update_memlcd_app()
{
  if (currentLine == END_OF_SCREEN) {
    GLIB_clear(&glibContext);
    GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNarrow6x8);
    currentLine = 0;
    GLIB_drawStringOnLine(&glibContext, "MEMLCD Sample App", currentLine++, GLIB_ALIGN_LEFT, 5, 5, true);
  }
  GLIB_drawStringOnLine(&glibContext, "Hello There!", currentLine++, GLIB_ALIGN_LEFT, 5, 5, true);

  DMD_updateDisplay();
}
