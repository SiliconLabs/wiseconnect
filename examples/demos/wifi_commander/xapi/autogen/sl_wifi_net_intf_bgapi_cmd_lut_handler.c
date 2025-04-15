
/********************************************************************************
 * @file sl_wifi_net_intf_bgapi_cmd_lut_handler.c 
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

void app_wifi_cmd_net_intf_init(const void *);
void app_wifi_cmd_net_intf_deinit(const void *);
void app_wifi_cmd_net_intf_up(const void *);
void app_wifi_cmd_net_intf_down(const void *);
void app_wifi_cmd_net_intf_set_device_config(const void *);
void app_wifi_cmd_net_intf_get_init_status(const void *);
void app_wifi_cmd_net_intf_get_network_status(const void *);
void app_wifi_cmd_net_intf_get_device_config(const void *);
void app_wifi_cmd_net_intf_set_si91x_boot_config(const void *);
void app_wifi_cmd_net_intf_get_si91x_boot_config(const void *);
void app_wifi_cmd_net_intf_load_default_device_config(const void *);

const struct bgapi_cmd sli_bgapi_cmd_table_wifi_net_intf[] = {
  {
    .handler = (sl_bgapi_handler)app_wifi_cmd_net_intf_init,
    .class_id = SLI_WIFI_NET_INTF_CLASS_ID,
    .command_id = APP_WIFI_NET_INTF_INIT_COMMAND_ID,
    .metadata = {
      .sensitive = 0,
    }
  },
  {
    .handler = (sl_bgapi_handler)app_wifi_cmd_net_intf_deinit,
    .class_id = SLI_WIFI_NET_INTF_CLASS_ID,
    .command_id = APP_WIFI_NET_INTF_DEINIT_COMMAND_ID,
    .metadata = {
      .sensitive = 0,
    }
  },
  {
    .handler = (sl_bgapi_handler)app_wifi_cmd_net_intf_up,
    .class_id = SLI_WIFI_NET_INTF_CLASS_ID,
    .command_id = APP_WIFI_NET_INTF_UP_COMMAND_ID,
    .metadata = {
      .sensitive = 0,
    }
  },
  {
    .handler = (sl_bgapi_handler)app_wifi_cmd_net_intf_down,
    .class_id = SLI_WIFI_NET_INTF_CLASS_ID,
    .command_id = APP_WIFI_NET_INTF_DOWN_COMMAND_ID,
    .metadata = {
      .sensitive = 0,
    }
  },
  {
    .handler = (sl_bgapi_handler)app_wifi_cmd_net_intf_set_device_config,
    .class_id = SLI_WIFI_NET_INTF_CLASS_ID,
    .command_id = APP_WIFI_NET_INTF_SET_DEVICE_CONFIG_COMMAND_ID,
    .metadata = {
      .sensitive = 0,
    }
  },
  {
    .handler = (sl_bgapi_handler)app_wifi_cmd_net_intf_get_init_status,
    .class_id = SLI_WIFI_NET_INTF_CLASS_ID,
    .command_id = APP_WIFI_NET_INTF_GET_INIT_STATUS_COMMAND_ID,
    .metadata = {
      .sensitive = 0,
    }
  },
  {
    .handler = (sl_bgapi_handler)app_wifi_cmd_net_intf_get_network_status,
    .class_id = SLI_WIFI_NET_INTF_CLASS_ID,
    .command_id = APP_WIFI_NET_INTF_GET_NETWORK_STATUS_COMMAND_ID,
    .metadata = {
      .sensitive = 0,
    }
  },
  {
    .handler = (sl_bgapi_handler)app_wifi_cmd_net_intf_get_device_config,
    .class_id = SLI_WIFI_NET_INTF_CLASS_ID,
    .command_id = APP_WIFI_NET_INTF_GET_DEVICE_CONFIG_COMMAND_ID,
    .metadata = {
      .sensitive = 0,
    }
  },
  {
    .handler = (sl_bgapi_handler)app_wifi_cmd_net_intf_set_si91x_boot_config,
    .class_id = SLI_WIFI_NET_INTF_CLASS_ID,
    .command_id = APP_WIFI_NET_INTF_SET_SI91X_BOOT_CONFIG_COMMAND_ID,
    .metadata = {
      .sensitive = 0,
    }
  },
  {
    .handler = (sl_bgapi_handler)app_wifi_cmd_net_intf_get_si91x_boot_config,
    .class_id = SLI_WIFI_NET_INTF_CLASS_ID,
    .command_id = APP_WIFI_NET_INTF_GET_SI91X_BOOT_CONFIG_COMMAND_ID,
    .metadata = {
      .sensitive = 0,
    }
  },
  {
    .handler = (sl_bgapi_handler)app_wifi_cmd_net_intf_load_default_device_config,
    .class_id = SLI_WIFI_NET_INTF_CLASS_ID,
    .command_id = APP_WIFI_NET_INTF_LOAD_DEFAULT_DEVICE_CONFIG_COMMAND_ID,
    .metadata = {
      .sensitive = 0,
    }
  },
  { 0 }
};