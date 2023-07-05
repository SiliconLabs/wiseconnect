/***************************************************************************/ /**
 * @file
 * @brief UDP Client Example Application
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

#include "sl_net.h"
#include "cmsis_os2.h"
#include "sl_board_configuration.h"
#include "sl_status.h"
#include "errno.h"
#include "sl_ip_types.h"
#include "sl_wifi.h"
#include "sl_net.h"
#include "sl_status.h"
#include "socket.h"
#include "sl_utility.h"
#include "sl_wifi_callback_framework.h"
#include <string.h>

/******************************************************
 *                      Macros
 ******************************************************/
/******************************************************
 *                    Constants
 ******************************************************/

//! Server IP address.
#define SERVER_IP_ADDR "172.20.10.3"

//! Server port numbers
#define SERVER_PORT 5001

//! Number of packet to send or receive
#define NUMBER_OF_PACKETS 1000

#define DATA "hello from udp client"

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
sl_status_t send_data(void);

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

  status = sl_net_init(SL_NET_DEFAULT_WIFI_CLIENT_INTERFACE, &sl_wifi_default_client_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("Failed to start Wi-Fi Client interface: 0x%lx\r\n", status);
    return;
  }
  status = sl_net_up(SL_NET_DEFAULT_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE);
  if (status != SL_STATUS_OK) {
    printf("Failed to bring Wi-Fi client interface up: 0x%lx\r\n", status);
    return;
  }
  printf("Wi-Fi client connected\r\n");

  status = send_data();
  if (status != SL_STATUS_OK) {
    printf("\r\n send data from udp client failed with status 0x%lx\r\n", status);
  }

  printf("\r\nUDP Client Application Completed\r\n");

  while (1) {
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
    // Let the CPU go to sleep if the system allows it.
    sl_power_manager_sleep();
#endif
  }
}

sl_status_t send_data()
{
  int sent_bytes                    = -1;
  int client_socket                 = -1;
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  int32_t total_number_of_packets   = 0;
  struct sockaddr_in server_address = { 0 };
  sl_ipv4_address_t ip_address      = { 0 };

  server_address.sin_family = AF_INET;
  server_address.sin_port   = SERVER_PORT;
  convert_string_to_sl_ipv4_address(SERVER_IP_ADDR, &ip_address);
  server_address.sin_addr.s_addr = ip_address.value;

  client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (client_socket < 0) {
    printf("\r\nSocket creation failed with bsd error: %d\r\n", errno);
    return SL_STATUS_FAIL;
  }
  printf("\r\nClient socket created\r\n");

  while (total_number_of_packets < NUMBER_OF_PACKETS) {
    sent_bytes = sendto(client_socket, DATA, sizeof(DATA), 0, (struct sockaddr *)&server_address, socket_length);
    if (sent_bytes < 0) {
      printf("\r\nSend failed with bsd error: %d\r\n", errno);
      close(client_socket);
      return SL_STATUS_FAIL;
    }
    total_number_of_packets++;
  }
  printf("\r\nTotal number of packets sent %ld\r\n", total_number_of_packets);

  close(client_socket);
  printf("\r\nSocket close success\r\n");

  return SL_STATUS_OK;
}
