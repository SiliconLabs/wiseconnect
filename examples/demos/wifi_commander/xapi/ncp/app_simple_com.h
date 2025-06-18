/***************************************************************************/ /**
 * @file
 * @brief Simple Communication Interface (UART)
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef APP_SIMPLE_COM_H
#define APP_SIMPLE_COM_H

/***********************************************************************************************/ /**
 * @addtogroup simple_com
 * @{
 **************************************************************************************************/

#include <stdint.h>
#include "sl_status.h"

/**************************************************************************/ /**
 * Simple Comm Init.
 *****************************************************************************/
void app_simple_com_init(void);

/**************************************************************************/ /**
 * Step function (used in CPC mode)
 *****************************************************************************/
void app_simple_com_step(void);

/**************************************************************************/ /**
 * Transmit function
 *****************************************************************************/
void app_simple_com_transmit(uint32_t len, const uint8_t *data);

/**************************************************************************/ /**
 * Transmit completed callback
 *****************************************************************************/
void app_simple_com_transmit_cb(sl_status_t status);

/**************************************************************************/ /**
 * Receive function (used in UART mode)
 *****************************************************************************/
void app_simple_com_receive(void);

/**************************************************************************/ /**
 * Receive completed callback
 *****************************************************************************/
void app_simple_com_receive_cb(sl_status_t status, uint32_t len, uint8_t *data);

/**************************************************************************/ /**
 * OS initialization function - if the OS is present
 *****************************************************************************/
void app_simple_com_os_task_init(void);

/**************************************************************************/ /**
 * Function to trigger the OS task to proceed - if the OS is present
 *****************************************************************************/
void app_simple_com_os_task_proceed(void);

/** @} (end addtogroup simple_com) */
#endif // APP_SIMPLE_COM_H
