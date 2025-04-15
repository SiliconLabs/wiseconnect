/********************************************************************************
 * @file  app_xapi_net_cred.c
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
#include "sl_status.h"
#include "sl_wifi_bgapi_cmd_rx_handler.h"
#include "si91x_device.h"
#include "sl_net.h"
#include "sl_wifi.h"
#include "sl_wifi_callback_framework.h"
#include "sl_si91x_driver.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sl_net_wifi_types.h"

DEFINE_BGAPI_CLASS(wifi, net_cred, WIFI, NET_CRED, NULL, NULL);

/******************************************************
*               Function Definitions
******************************************************/

void app_wifi_cmd_net_cred_set(app_wifi_cmd_net_cred_set_t *cmd_input)
{
  sl_status_t status = SL_STATUS_FAIL;

  // Check if the input command is NULL or has an invalid credential ID
  if (cmd_input == NULL) {
    app_wifi_rsp_net_cred_set(SL_STATUS_INVALID_PARAMETER);
  }

  // Check if the credential ID is the default WiFi AP credential ID and the type is PSK
  if ((cmd_input->id == SL_NET_DEFAULT_WIFI_AP_CREDENTIAL_ID) && (cmd_input->type == SL_NET_WIFI_PSK)) {
    // Set the network credential using the provided ID, type, and credential data
    status = sl_net_set_credential(cmd_input->id,
                                   cmd_input->type,
                                   (const void *)(cmd_input->credential.data),
                                   cmd_input->credential.len);
  } else if ((cmd_input->id == SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID) && (cmd_input->type == SL_NET_WIFI_PSK)) {
    // Set the network credential using the provided ID, type, and credential data
    status = sl_net_set_credential(cmd_input->id,
                                   cmd_input->type,
                                   (const void *)(cmd_input->credential.data),
                                   cmd_input->credential.len);
  } else {
    // If the provided ID or type is not supported, return a 'Not Supported' status
    status = SL_STATUS_NOT_SUPPORTED;
  }

  // Send the response with the status of the operation
  app_wifi_rsp_net_cred_set(status);
}

void app_wifi_cmd_net_cred_get(app_wifi_cmd_net_cred_get_t *cmd_input)
{
  uint8_t credential[256];                                       // Buffer to store the network credential.
  uint32_t credential_length               = sizeof(credential); // Set the initial length of the credential buffer.
  sl_status_t status                       = SL_STATUS_FAIL;     // Initialize the status to a failure state.
  sl_net_credential_type_t credential_type = cmd_input->type;    // Get the credential type from the command input.

  // Check if the credential ID is the default WiFi AP credential ID and the credential type is PSK
  if ((cmd_input->id == SL_NET_DEFAULT_WIFI_AP_CREDENTIAL_ID) && (cmd_input->type == SL_NET_WIFI_PSK)) {
    // Get the network credential using the provided credential ID and type
    status = sl_net_get_credential(cmd_input->id, &credential_type, &credential, &credential_length);
  } else if ((cmd_input->id == SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID) && (cmd_input->type == SL_NET_WIFI_PSK)) {
    // Get the network credential using the provided credential ID and type
    status = sl_net_get_credential(cmd_input->id, &credential_type, &credential, &credential_length);
  } else {
    status = SL_STATUS_NOT_SUPPORTED;
    // If the provided ID or type is not supported, return a 'Not Supported' status
    app_wifi_rsp_net_cred_get(status, 0, 0, NULL);
    return;
  }

  // Send the response back with the status, length of the credential, and the credential data
  app_wifi_rsp_net_cred_get(status, credential_length, credential_length, credential);
}

void app_wifi_cmd_net_cred_delete(app_wifi_cmd_net_cred_delete_t *cmd_input)
{
  sl_status_t status = SL_STATUS_FAIL;

  // Check if the credential ID is the default Wi-Fi AP credential ID
  // and if the type is Wi-Fi PSK (Pre-Shared Key)
  if ((cmd_input->id == SL_NET_DEFAULT_WIFI_AP_CREDENTIAL_ID) && (cmd_input->type == SL_NET_WIFI_PSK)) {

    // If the condition is met, delete the Wi-Fi credential using the provided ID and type
    status = sl_net_delete_credential(cmd_input->id, cmd_input->type);
  } else if ((cmd_input->id == SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID) && (cmd_input->type == SL_NET_WIFI_PSK)) {

    // If the condition is met, delete the Wi-Fi credential using the provided ID and type
    status = sl_net_delete_credential(cmd_input->id, cmd_input->type);
  } else {
    // If the provided ID or type is not supported, return a 'Not Supported' status
    status = SL_STATUS_NOT_SUPPORTED;
  }

  // Send a response to indicate whether the credential deletion was successful or not
  app_wifi_rsp_net_cred_delete(status);
}
