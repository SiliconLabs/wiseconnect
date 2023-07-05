/***************************************************************************/ /**
 * @file
 * @brief TCP Client Example Application
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

#include "sl_si91x_socket.h"
#include "errno.h"
#include "netdb.h"
#include "sl_wifi_callback_framework.h"
#include "sl_status.h"
#include "sl_board_configuration.h"
#include "cmsis_os2.h"
#include "sl_wifi.h"
#include "socket.h"
#include "sl_utility.h"
#include "sl_net.h"
#include "sl_net_si91x.h"
#include <string.h>
#include <stdint.h>

/******************************************************
 *                      Macros
 ******************************************************/
/******************************************************
 *                    Constants
 ******************************************************/
#define SERVER_IP         "192.168.0.247"
#define SERVER_PORT       5001
#define NUMBER_OF_PACKETS 1000
#define DATA              "hello from tcp client"
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
void send_data_to_tcp_server();

/******************************************************
 *               Function Definitions
 ******************************************************/

void app_init(const void *unused)
{
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

static void application_start(void *argument)
{
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

  send_data_to_tcp_server();
}

void send_data_to_tcp_server()
{
  int client_socket = -1;
  int return_value  = 0;
  int sent_bytes;
  int packet_count     = 0;
  sl_ipv4_address_t ip = { 0 };

  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);

  convert_string_to_sl_ipv4_address(SERVER_IP, &ip);

  server_address.sin_family      = AF_INET;
  server_address.sin_port        = SERVER_PORT;
  server_address.sin_addr.s_addr = ip.value;

  //!Create socket
  client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) {
    printf("\r\nSocket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  printf("\r\nTCP Socket Create Success\r\n");

  //! Socket connect
  return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (return_value < 0) {
    printf("\r\nSocket connect failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  printf("\r\nTCP Socket Connect Success\r\n");

  //! send data
  while (packet_count < NUMBER_OF_PACKETS) {
    sent_bytes = send(client_socket, DATA, strlen(DATA), 0);
    if (sent_bytes < 0) {
      printf("\r\nSend failed with bsd error:%d\r\n", errno);
      close(client_socket);
      return;
    }
    packet_count++;
  }

  printf("\r\n%d packets sent success\r\n", packet_count);

  //!Socket close
  close(client_socket);
  printf("\r\nSocket close success\r\n");
  return;
}
