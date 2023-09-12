/***************************************************************************/ /**
 * @file
 * @brief Station Ping Example Application
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "cmsis_os2.h"
#include "sl_net.h"
#include "sl_board_configuration.h"
#include "sl_net_ping.h"
#include "sl_utility.h"
#include "sl_si91x_driver.h"
#include "sl_wifi.h"
#include "sl_net_wifi_types.h"
#include "sl_net_default_values.h"
#include <string.h>

/******************************************************
 *                    Constants
 ******************************************************/
#define REMOTE_IP_ADDRESS "192.168.0.145"

#define CONNECT_WITH_PMK 0

#define PING_PACKET_SIZE 64

/******************************************************
 *               Variable Definitions
 ******************************************************/
const osThreadAttr_t thread_attributes = {
  .name       = "app",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 3072,
  .priority   = osPriorityLow,
  .tz_module  = 0,
  .reserved   = 0,
};

/******************************************************
 *               Function Declarations
 ******************************************************/
static void application_start(void *argument);
static sl_status_t ping_callback_handler(uint32_t event, sl_status_t status, void *data, uint32_t data_length);

/******************************************************
 *               Function Definitions
 ******************************************************/
void app_init(const void *unused)
{
  UNUSED_PARAMETER(unused);
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

static void application_start(void *argument)
{
  UNUSED_PARAMETER(argument);
  sl_status_t status;

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, NULL, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to start Wi-Fi Client interface: 0x%lX\r\n", status);
    return;
  }
  printf("\r\nWi-Fi client interface up success\r\n");

#if CONNECT_WITH_PMK
  uint8_t pairwise_master_key[32] = { 0 };
  sl_wifi_ssid_t ssid;
  uint8_t type = 3;
  ssid.length  = (uint8_t)strnlen(DEFAULT_WIFI_CLIENT_PROFILE_SSID, sizeof(ssid.value));
  memcpy(ssid.value, DEFAULT_WIFI_CLIENT_PROFILE_SSID, ssid.length);

  status = sl_wifi_get_pairwise_master_key(SL_NET_WIFI_CLIENT_INTERFACE,
                                           type,
                                           &ssid,
                                           DEFAULT_WIFI_CLIENT_CREDENTIAL,
                                           pairwise_master_key);
  if (status != SL_STATUS_OK) {
    printf("\r\nGet Pairwise Master Key Failed, Error Code : 0x%lX\r\n", status);
    return;
  }
  printf("\r\nGet Pairwise Master Key Success\r\n");

  status = sl_net_set_credential(SL_NET_DEFAULT_PMK_CREDENTIAL_ID,
                                 SL_NET_WIFI_PMK,
                                 pairwise_master_key,
                                 sizeof(sl_wifi_pmk_credential_t));
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed sl_net_set_credential: 0x%lX\r\n", status);
    return;
  }
  printf("\r\nsl_net_set_credential done\r\n");
#endif

  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to bring Wi-Fi client interface up: 0x%lX\r\n", status);
    return;
  }
  printf("\r\nWi-Fi client connected\r\n");

  sl_si91x_register_callback(SL_NET_PING_RESPONSE_EVENT, ping_callback_handler);

  sl_ip_address_t remote_ip_address = { 0 };
  sl_net_inet_addr(REMOTE_IP_ADDRESS, (uint32_t *)&remote_ip_address.ip.v4);
  remote_ip_address.type = SL_IPV4;

  while (1) {
    // Send ping
    status = sl_si91x_send_ping(remote_ip_address, PING_PACKET_SIZE);
    if (status != SL_STATUS_IN_PROGRESS) {
      printf("\r\nPing request failed with status 0x%lX\r\n", status);
      return;
    }

    // Sleep for 2 seconds
    osDelay(2000);
  }
}

static sl_status_t ping_callback_handler(uint32_t event, sl_status_t status, void *data, uint32_t data_length)
{
  UNUSED_PARAMETER(data_length);
  switch (event) {
    case SL_NET_PING_RESPONSE_EVENT: {
      sl_si91x_ping_response_t *response = (sl_si91x_ping_response_t *)data;
      if (status != SL_STATUS_OK) {
        printf("\r\nPing request failed!\r\n");
        return status;
      }
      printf("\r\n%u bytes received from %u.%u.%u.%u\r\n",
             response->ping_size,
             response->ping_address.ipv4_address[0],
             response->ping_address.ipv4_address[1],
             response->ping_address.ipv4_address[2],
             response->ping_address.ipv4_address[3]);
      break;
    }
  }

  return SL_STATUS_OK;
}
