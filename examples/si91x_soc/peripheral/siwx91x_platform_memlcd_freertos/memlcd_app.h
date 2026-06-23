/***************************************************************************/ /**
 * @file memlcd_app.h
 * @brief MEMLCD drawing helpers for the FreeRTOS example
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

#ifndef MEMLCD_APP_H
#define MEMLCD_APP_H

#include <stdint.h>

/***************************************************************************/ /**
 * One-time MEMLCD + GLIB setup and static title lines.
 ******************************************************************************/
void memlcd_app_init(void);

/***************************************************************************/ /**
 * Redraws titles and a status line showing @p tick_count (e.g. from
 * `osKernelGetTickCount()` in the MEMLCD task).
 *
 * @param tick_count Value to show on the status line
 ******************************************************************************/
void update_memlcd_app();

#endif /* MEMLCD_APP_H */
