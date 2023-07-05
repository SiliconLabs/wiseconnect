/***************************************************************************/ /**
 * @file
 * @brief SSL Client Example Application
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
#include <string.h>
#include "sl_status.h"
#include "sl_board_configuration.h"
#include "cmsis_os2.h"

#include "errno.h"
#include "sl_wifi.h"
#include "sl_net.h"
#include "sl_tls.h"
#include "socket.h"
#include "sl_utility.h"
#include "sl_net_si91x.h"
#include "sl_wifi_callback_framework.h"

//! Include SSL CA certificate
#include "cacert.pem.h"

/******************************************************
 *                      Macros
 ******************************************************/
#define VERFIY_AND_GOTO_IF_TRUE(expression, label) \
  {                                                \
    if (expression) {                              \
      PRINT_STATUS(ERROR_TAG, SL_STATUS_FAIL);     \
      goto label;                                  \
    }                                              \
  };

/******************************************************
 *                    Constants
 ******************************************************/
//! Server IP address
#define SERVER_IP_ADDR "192.168.1.108"

//! Server port number
#define SERVER_PORT 5001

//! Load certificate to device flash
//! Certificate should be loaded once and need not be loaded for every boot up
#define LOAD_CERTIFICATE 1

//! Number of packet to send or receive
#define NUMBER_OF_PACKETS 1000

//! Data to be sent over SSL socket
#define DATA_BUFFER "Hello from SSL client!!!"

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
static sl_status_t clear_and_load_certificates_in_flash(void);
sl_status_t ssl_client_application(void);

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

#if LOAD_CERTIFICATE
  status = clear_and_load_certificates_in_flash();
  if (status != SL_STATUS_OK) {
    printf("\r\nUnexpected error while loading certificate: 0x%lX\r\n", status);
    return;
  }
#endif

  status = ssl_client_application();
  if (status != SL_STATUS_OK) {
    printf("\r\nUnexpected error while creating SSL sockets, Status: 0x%lX, Errno: %d\r\n", status, errno);
    return;
  }

  printf("\r\nSSL Client Application Demonstration Completed\r\n");
}

sl_status_t clear_and_load_certificates_in_flash(void)
{
  sl_tls_store_configuration_t tls_configuration = { 0 };
  //! Erase SSL CA certificate
  sl_status_t status = sl_tls_clear_global_ca_store();
  if (status != SL_STATUS_OK) {
    printf("\r\nClient certificate erase failed, Error Code : 0x%lX\r\n", status);
    return status;
  }

  tls_configuration.cacert             = (uint8_t *)cacert;
  tls_configuration.cacert_length      = (sizeof(cacert) - 1);
  tls_configuration.cacert_type        = SL_TLS_SSL_CA_CERTIFICATE;
  tls_configuration.use_secure_element = false;

  //! Loading SSL CA certificate in to FLASH
  status = sl_tls_set_global_ca_store(tls_configuration);
  if (status != SL_STATUS_OK) {
    printf("\r\nLoading SSL CA certificate in to FLASH Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  printf("\r\nLoading SSL CA certificate in to FLASH Success\r\n");

  return status;
}

sl_status_t ssl_client_application(void)
{
  int32_t socket_status            = -1;
  int32_t client_socket            = -1;
  int32_t packet_count             = 0;
  sl_ipv4_address_t server_address = { 0 };
  struct sockaddr_in server_addr   = { 0 };

  //! Converting and storing given IP address into SL IPV4
  convert_string_to_sl_ipv4_address(SERVER_IP_ADDR, &server_address);

  //! Create SSL Socket
  client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) {
    printf("\r\nSocket creation failed with bsd error: %d\r\n", errno);
    return SL_STATUS_FAIL;
  }
  printf("\r\nSSL Socket Create Success\r\n");

  //! Setting SSL socket option
  socket_status = setsockopt(client_socket, SOL_TCP, TCP_ULP, TLS, sizeof(TLS));
  if (socket_status < 0) {
    printf("\r\nSet socket failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return SL_STATUS_FAIL;
  }

  server_addr.sin_family      = AF_INET;              //! Set server address family
  server_addr.sin_port        = SERVER_PORT;          //! Set server port number
  server_addr.sin_addr.s_addr = server_address.value; //! Set SSL server IP address

  //! Connect to server socket
  socket_status = connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (socket_status < 0) {
    printf("\r\nSocket Connect failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return SL_STATUS_FAIL;
  }
  printf("\r\nSocket Connect Success\r\n");

  while (packet_count < NUMBER_OF_PACKETS) {
    //! Send data on socket
    socket_status = send(client_socket, DATA_BUFFER, strlen(DATA_BUFFER), 0);
    if (socket_status < 0) {
      printf("\r\nSend failed with bsd error: %d\r\n", errno);
      close(client_socket);
      return SL_STATUS_FAIL;
    }
    packet_count++;
  }

  printf("\r\n%ld packets sent success\r\n", packet_count);

  close(client_socket);
  printf("\r\nSocket close success\r\n");

  return SL_STATUS_OK;
}
