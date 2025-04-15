
/********************************************************************************
 * @file sl_wifi_ap_bgapi_cmd_lut_handler.c 
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

#include <stdio.h>

#include "sli_wifi_api.h"
#include "sl_bgapi.h"
#include "bgapi.h"

void app_wifi_cmd_ap_disconnect_client(const void *);
void app_wifi_cmd_ap_get_client_info(const void *);

const struct bgapi_cmd sli_bgapi_cmd_table_wifi_ap[] = {
  {
    .handler = (sl_bgapi_handler)app_wifi_cmd_ap_disconnect_client,
    .class_id = SLI_WIFI_AP_CLASS_ID,
    .command_id = APP_WIFI_AP_DISCONNECT_CLIENT_COMMAND_ID,
    .metadata = {
      .sensitive = 0,
    }
  },
  {
    .handler = (sl_bgapi_handler)app_wifi_cmd_ap_get_client_info,
    .class_id = SLI_WIFI_AP_CLASS_ID,
    .command_id = APP_WIFI_AP_GET_CLIENT_INFO_COMMAND_ID,
    .metadata = {
      .sensitive = 0,
    }
  },
  { 0 }
};