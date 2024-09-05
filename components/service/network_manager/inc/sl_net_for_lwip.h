/******************************************************************************
 * Copyright 2019, Silicon Laboratories Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#pragma once

#include "sl_net_types.h"
#include "sl_wifi_types.h"
#include "sl_wifi_device.h"
#include "sl_si91x_types.h"
#include "lwip/netif.h"

/** \addtogroup SL_NET_TYPES
 * @{ */

/**
 * @brief LwIP network context structure.
 * 
 * @details
 * This structure holds the LwIP network interface context used for Wi-Fi operations.
 * It encapsulates the LwIP network interface structure.
 * 
 * @note
 * This context is specific to LwIP and is used internally by SDK.
 */
typedef struct {
  struct netif netif; ///< lwIP network interfaces
} sl_net_wifi_lwip_context_t;
/** @} */

//! @cond Doxygen_Suppress
/// LwIP inteface API
typedef struct {
  sl_status_t (*init)(const sl_wifi_device_configuration_t *configuration,
                      sl_net_wifi_lwip_context_t *workspace,
                      sl_event_handler_t event_handler);        ///< Interface init  handler
  sl_status_t (*deinit)(sl_net_wifi_lwip_context_t *workspace); ///< Interface de-init  handler
  sl_status_t (*up)(sl_net_profile_id_t profile_id);            ///< Interface up handler
  sl_status_t (*down)(void);                                    ///< Interface down handler
} sl_net_wifi_lwip_interface_api_t;

/// extern variables
extern sl_net_wifi_lwip_interface_api_t wifi_client;
//! @endcond
