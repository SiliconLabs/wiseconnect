/***************************************************************************/ /**
 * @file  bsd_socket_commands.c
 * @brief BSD Socket AT Commands
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
#include <string.h>
#include "console.h"
#include "sl_utility.h"
#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
#include "socket.h"
#include "errno.h"
#include "select.h"
#endif
#ifdef SLI_SI91X_LWIP_HOSTED_NETWORK_STACK
#include "lwip/sockets.h"
int errno;
#endif
#include "sl_constants.h"
#include "netdb.h"
#include "sl_net.h"
#include "sl_wifi.h"
#include "sl_si91x_socket.h"
#include "sl_si91x_socket_utility.h"
#include "at_utility.h"

/******************************************************
 *                    Macros
 ******************************************************/
#define UDP_MAX_RECEIVE_DATA_LENGTH 1470
#define TCP_MAX_RECEIVE_DATA_LENGTH 1460
#define VERIFY_BSD_STATUS(status) \
  do {                            \
    if (status < 0) {             \
      print_errno();              \
      return SL_STATUS_FAIL;      \
    }                             \
  } while (0)

/******************************************************
 *               Static Function Declarations
 ******************************************************/

static inline void print_errno(void);

/******************************************************
 *               Extern Functions
 ******************************************************/
/******************************************************
 *               Function Declarations
 ******************************************************/
uint8_t nfds = 0;

static uint8_t remote_terminate_flag        = 0;
static uint8_t skip_remote_terminate_prints = 0;

/******************************************************
 *               Function Definitions
 ******************************************************/

void remote_terminate(int socket_id, uint16_t port_number, uint32_t bytes_sent)
{
  if (skip_remote_terminate_prints) {
    remote_terminate_flag        = 1;
    skip_remote_terminate_prints = 0;
  } else {
    printf("Remote Terminate on socket %d, port %d , bytes_sent %ld\r\n", socket_id, port_number, bytes_sent);
  }
}

sl_status_t bsd_socket_create_handler(console_args_t *arguments)
{
  const int32_t domain   = (int32_t)arguments->arg[0];
  const int32_t type     = (int32_t)arguments->arg[1];
  const int32_t protocol = (int32_t)arguments->arg[2];

  int32_t socket_fd = socket(domain, type, protocol);
  VERIFY_BSD_STATUS(socket_fd);
  PRINT_AT_CMD_SUCCESS;

  printf("%lu", socket_fd);
  return SL_STATUS_OK;
}

sl_status_t bsd_socket_bind_handler(console_args_t *arguments)
{
  sl_ipv4_address_t ip = { 0 };
  char *ip_address;
  const int32_t socket_fd = (int32_t)arguments->arg[0];
#ifdef SLI_SI91X_LWIP_HOSTED_NETWORK_STACK
  const uint16_t port = htons((uint16_t)arguments->arg[2]);
#else
  const uint16_t port = (uint16_t)arguments->arg[2];
#endif
  ip_address         = (char *)arguments->arg[1];
  uint8_t ip_version = GET_OPTIONAL_COMMAND_ARG(arguments, 3, SL_IPV4_VERSION, uint8_t);
  int32_t status     = -1;

  if (ip_version == SL_IPV4_VERSION) {
    struct sockaddr_in socket_address = { 0 };
    convert_string_to_sl_ipv4_address(ip_address, &ip);
    socket_address.sin_family = AF_INET;
    socket_address.sin_port   = port;
    memcpy(&socket_address.sin_addr.s_addr, &ip.value, SL_IPV4_ADDRESS_LENGTH);

    status = bind(socket_fd, (struct sockaddr *)&socket_address, sizeof(struct sockaddr_in));
    VERIFY_BSD_STATUS(status);

  } else {
    struct sockaddr_in6 server_address6 = { 0 };
    server_address6.sin6_family         = AF_INET6;
    server_address6.sin6_port           = port;
    status = bind(socket_fd, (struct sockaddr *)&server_address6, sizeof(struct sockaddr_in6));
    VERIFY_BSD_STATUS(status);
  }

  PRINT_AT_CMD_SUCCESS;
  return SL_STATUS_OK;
}

sl_status_t bsd_socket_listen_handler(console_args_t *arguments)
{
  const int32_t socket_fd = (int32_t)arguments->arg[0];
  const int32_t backlog   = (int32_t)arguments->arg[1];
  int32_t status          = -1;

  status = listen(socket_fd, backlog);
  VERIFY_BSD_STATUS(status);

  PRINT_AT_CMD_SUCCESS;
  return SL_STATUS_OK;
}

sl_status_t bsd_socket_accept_handler(console_args_t *arguments)
{
  const int32_t socket_fd                  = (int32_t)arguments->arg[0];
  struct sockaddr_in remote_socket_address = { 0 };
  socklen_t socket_length                  = sizeof(struct sockaddr_in);
  uint8_t ip_version                       = GET_OPTIONAL_COMMAND_ARG(arguments, 1, SL_IPV4_VERSION, uint8_t);
  int32_t status                           = -1;

  if (ip_version == SL_IPV4_VERSION) {
    status = accept(socket_fd, ((struct sockaddr *)&remote_socket_address), &socket_length);
  } else {
    status = accept(socket_fd, NULL, NULL);
  }

  VERIFY_BSD_STATUS(status);

  PRINT_AT_CMD_SUCCESS;
  printf("Socket ID: %lu\n", status);
  if ((ip_version == SL_IPV4_VERSION) && (socket_length == sizeof(struct sockaddr_in))) {
    const uint8_t *ip_address = (const uint8_t *)&remote_socket_address.sin_addr.s_addr;
    printf("%u.%u.%u.%u:%u",
           ip_address[0],
           ip_address[1],
           ip_address[2],
           ip_address[3],
           remote_socket_address.sin_port);
  }

  return SL_STATUS_OK;
}

sl_status_t bsd_socket_receive_from_handler(console_args_t *arguments)
{
  int32_t sock_fd                                 = (int32_t)arguments->arg[0];
  struct sockaddr_in address                      = { 0 };
  socklen_t address_length                        = sizeof(address);
  sl_ipv4_address_t ip                            = { 0 };
  uint8_t buffer[UDP_MAX_RECEIVE_DATA_LENGTH + 1] = { 0 };

  int32_t status = recvfrom(sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&address, &address_length);
  VERIFY_BSD_STATUS(status);

  PRINT_AT_CMD_SUCCESS;
  memcpy(&ip.value, &address.sin_addr.s_addr, SL_IPV4_ADDRESS_LENGTH);
  SL_DEBUG_LOG("%u.%u.%u.%u:%u", ip.bytes[0], ip.bytes[1], ip.bytes[2], ip.bytes[3], address.sin_port);

  SL_DEBUG_LOG("Received:%s", buffer);
  return SL_STATUS_OK;
}

sl_status_t bsd_socket_receive_handler(console_args_t *arguments)
{
  int32_t sock_fd                                 = (int32_t)arguments->arg[0];
  uint8_t buffer[TCP_MAX_RECEIVE_DATA_LENGTH + 1] = { 0 };

  int32_t status = recv(sock_fd, buffer, sizeof(buffer), 0);
  VERIFY_BSD_STATUS(status);

  PRINT_AT_CMD_SUCCESS;
  printf("Received:%s", buffer);
  return SL_STATUS_OK;
}

sl_status_t bsd_socket_send_handler(console_args_t *arguments)
{
  const char *buffer = NULL;
  int32_t sock_fd    = (int32_t)arguments->arg[0];

  buffer = GET_OPTIONAL_COMMAND_ARG(arguments, 1, NULL, char *);

  int flags = 0;

  if (buffer == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  int32_t status = send(sock_fd, buffer, strlen(buffer), flags);

  VERIFY_BSD_STATUS(status);

  PRINT_AT_CMD_SUCCESS;
  SL_DEBUG_LOG("%lu bytes sent", status);

  return SL_STATUS_OK;
}

sl_status_t bsd_socket_send_to_handler(console_args_t *arguments)
{
  const char *buffer         = NULL;
  struct sockaddr_in address = { 0 };
  sl_ipv4_address_t ip       = { 0 };
  int32_t sock_fd            = (int32_t)arguments->arg[0];

  buffer = GET_OPTIONAL_COMMAND_ARG(arguments, 4, NULL, char *);

  uint16_t data_length = GET_OPTIONAL_COMMAND_ARG(arguments, 3, 0, socklen_t);

  if ((buffer == NULL) || (data_length == 0)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (data_length > strlen(buffer)) {
    data_length = strlen(buffer);
  }

  convert_string_to_sl_ipv4_address((char *)arguments->arg[1], &ip);
  memcpy(&address.sin_addr.s_addr, &ip.value, SL_IPV4_ADDRESS_LENGTH);

#ifdef SLI_SI91X_LWIP_HOSTED_NETWORK_STACK
  address.sin_port = htons((in_port_t)GET_OPTIONAL_COMMAND_ARG(arguments, 2, 0, uint16_t));
#else
  address.sin_port    = (in_port_t)GET_OPTIONAL_COMMAND_ARG(arguments, 2, 0, uint16_t);
#endif
  address.sin_family = AF_INET;

  int32_t status = sendto(sock_fd, buffer, data_length, 0, (const struct sockaddr *)&address, sizeof(address));
  VERIFY_BSD_STATUS(status);

  PRINT_AT_CMD_SUCCESS;
  SL_DEBUG_LOG("%lu bytes sent", status);

  return SL_STATUS_OK;
}

sl_status_t bsd_socket_connect_handler(console_args_t *arguments)
{
  struct sockaddr_in address          = { 0 };
  struct sockaddr_in6 server_address6 = { 0 };
  sl_ipv4_address_t ip                = { 0 };
  uint8_t address_buffer[SL_IPV6_ADDRESS_LENGTH];
  uint8_t ip_length = 0;
  int32_t status;
  int32_t sock_fd = (int32_t)arguments->arg[0];
#ifdef SLI_SI91X_LWIP_HOSTED_NETWORK_STACK
  address.sin_port = htons((in_port_t)arguments->arg[2]);
#else
  address.sin_port    = (in_port_t)arguments->arg[2];
#endif
  status = convert_string_to_sl_ipv4_address((char *)arguments->arg[1], &ip);
  if (status == SL_STATUS_OK) {
    ip_length = SL_IPV4_ADDRESS_LENGTH;
  }

  if (ip_length == SL_IPV4_ADDRESS_LENGTH) {
    address.sin_family = AF_INET;
    memcpy(&address.sin_addr.s_addr, &ip.value, SL_IPV4_ADDRESS_LENGTH);
    status = connect(sock_fd, (struct sockaddr *)&address, sizeof(address));
  } else {
    server_address6.sin6_family = AF_INET6;
    server_address6.sin6_port   = (in_port_t)arguments->arg[3];
    status                      = sl_inet_pton6((char *)arguments->arg[1],
                           (char *)arguments->arg[1] + strlen((char *)arguments->arg[1]),
                           address_buffer,
                           (unsigned int *)server_address6.sin6_addr.__u6_addr.__u6_addr32);
    if (status != 0x1) {
      printf("\nIPv6 conversion failed.\n");
    }
    status = connect(sock_fd, (struct sockaddr *)&server_address6, sizeof(server_address6));
  }
  VERIFY_BSD_STATUS(status);

  PRINT_AT_CMD_SUCCESS;
  return SL_STATUS_OK;
}

sl_status_t bsd_socket_close_handler(console_args_t *arguments)
{
  int32_t sock_fd = (int32_t)arguments->arg[0];

  int32_t status = close(sock_fd);
  VERIFY_BSD_STATUS(status);

  PRINT_AT_CMD_SUCCESS;
  return SL_STATUS_OK;
}

sl_status_t bsd_socket_get_host_by_name_handler(console_args_t *arguments)
{
#ifndef SLI_SI91X_LWIP_HOSTED_NETWORK_STACK
  char *host_name = (char *)arguments->arg[0];

  struct hostent *host_ent = gethostbyname(host_name);
  if (host_ent == NULL) {
    return SL_STATUS_FAIL;
  }

  PRINT_AT_CMD_SUCCESS;
  print_sl_ip_address((sl_ip_address_t *)host_ent->h_addr);

  printf("\r\nhost_name: %s, h_errno:%d, type:%d, length:%d",
         host_name,
         h_errno,
         host_ent->h_addrtype,
         host_ent->h_length);

  return SL_STATUS_OK;
#else
  //! Not supported in LWIP(external stack)..
  return SL_STATUS_NOT_SUPPORTED;
#endif
}

sl_status_t bsd_socket_get_sock_name(console_args_t *arguments)
{
  int status = 0;

  int socket_id                     = (int32_t)arguments->arg[0];
  struct sockaddr_in socket_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);

  status = getsockname(socket_id, (struct sockaddr *)&socket_address, &socket_length);

  VERIFY_BSD_STATUS(status);

  PRINT_AT_CMD_SUCCESS;
  if (socket_length == sizeof(struct sockaddr_in)) {
    const uint8_t *ip_address = (uint8_t *)&socket_address.sin_addr.s_addr;

    printf("%u.%u.%u.%u", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    printf(":%d\r\n", socket_address.sin_port);
  }

  return SL_STATUS_OK;
}

sl_status_t bsd_socket_select_handler(console_args_t *arguments)
{
  fd_set readfds;
  nfds                    = (uint8_t)GET_OPTIONAL_COMMAND_ARG(arguments, 0, 5, uint32_t);
  struct timeval timeout  = { .tv_sec = GET_OPTIONAL_COMMAND_ARG(arguments, 1, 5, uint32_t), .tv_usec = 0 };
  uint8_t timeout_disable = (uint8_t)GET_OPTIONAL_COMMAND_ARG(arguments, 2, 0, uint8_t);
  int32_t retval          = SL_STATUS_OK;

  FD_ZERO(&readfds);
  for (int i = 0; i < nfds; i++) {
    FD_SET(i, &readfds);
  }

  if (timeout_disable) {
    retval = select(nfds + 1, &readfds, NULL, NULL, NULL);
  } else {
    retval = select(nfds + 1, &readfds, NULL, NULL, &timeout);
  }
  VERIFY_BSD_STATUS(retval);

  PRINT_AT_CMD_SUCCESS;
  if (retval == 0) {
    printf("Timeout");
  } else {
    printf("Data is available on sockets: ");
    for (int i = 0; i < nfds; i++) {
      if (FD_ISSET(i, &readfds)) {
        printf("%d, ", i);
      }
      FD_CLR(i, &readfds);
    }
  }
  return SL_STATUS_OK;
}

sl_status_t bsd_socket_get_opt_handler(console_args_t *arguments)
{
  int32_t socket       = (int32_t)arguments->arg[0];
  int32_t option_name  = (int32_t)arguments->arg[1];
  int32_t option_value = 0;
  socklen_t length     = sizeof(option_value);

  int32_t socket_status = getsockopt(socket, SOL_SOCKET, option_name, &option_value, &length);
  VERIFY_BSD_STATUS(socket_status);

  printf("%ld", option_value);

  PRINT_AT_CMD_SUCCESS;
  return SL_STATUS_OK;
}

sl_status_t bsd_socket_get_peer_name(console_args_t *arguments)
{
  int status = 0;

  int socket_id                            = (int32_t)arguments->arg[0];
  struct sockaddr_in remote_socket_address = { 0 };
  socklen_t socket_length                  = sizeof(struct sockaddr_in);

  status = getpeername(socket_id, (struct sockaddr *)&remote_socket_address, &socket_length);

  VERIFY_BSD_STATUS(status);

  if (socket_length == sizeof(struct sockaddr_in)) {
    const uint8_t *ip_address = (uint8_t *)&remote_socket_address.sin_addr.s_addr;

    printf("\n\rIP address %d:%d:%d:%d", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    printf("\n\rPort %d", remote_socket_address.sin_port);
  }

  PRINT_AT_CMD_SUCCESS;
  return SL_STATUS_OK;
}

sl_status_t bsd_socket_setsockopt_handler(console_args_t *arguments)
{
  int32_t socket           = (int32_t)GET_OPTIONAL_COMMAND_ARG(arguments, 0, 0, int32_t);
  int32_t option_level     = (int32_t)GET_OPTIONAL_COMMAND_ARG(arguments, 1, SOL_SOCKET, int32_t);
  int32_t option_name      = (int32_t)GET_OPTIONAL_COMMAND_ARG(arguments, 2, 0, uint32_t);
  const char *option_value = (char *)GET_OPTIONAL_COMMAND_ARG(arguments, 3, NULL, char *);
  uint16_t option_val      = 0;
  int32_t socket_status    = 0;

  if (option_name == TCP_ULP) {
    socket_status = setsockopt(socket, option_level, option_name, option_value, (strlen(option_value) + 1));
  } else if (option_name == SO_RCVTIMEO) {
    sl_si91x_time_value timeout = { 0 };
    timeout.tv_sec              = (int32_t)GET_OPTIONAL_COMMAND_ARG(arguments, 3, 0, uint32_t);
    timeout.tv_usec             = ((int32_t)GET_OPTIONAL_COMMAND_ARG(arguments, 4, 0, uint32_t)) * 1000;
    socket_status               = setsockopt(socket, option_level, option_name, &timeout, sizeof(timeout));
  } else {
    option_val    = (uint16_t)(atoi(option_value));
    socket_status = setsockopt(socket, option_level, option_name, &option_val, sizeof(option_val));
  }
  if (socket_status < 0) {
    SL_DEBUG_LOG("Set socket option failed with bsd error: %d\n", errno);
    return SL_STATUS_FAIL;
  } else {
    PRINT_AT_CMD_SUCCESS;
    return SL_STATUS_OK;
  }
}

sl_status_t bsd_socket_set_option_handler(console_args_t *arguments)
{
  int32_t socket              = (int32_t)arguments->arg[0];
  int32_t option_level        = (int32_t)arguments->arg[1];
  int32_t option_name         = (int32_t)arguments->arg[2];
  int32_t socket_status       = 0;
  sl_si91x_time_value timeout = { 0 };

  timeout.tv_sec  = (int32_t)arguments->arg[3];
  timeout.tv_usec = ((int32_t)arguments->arg[4]) * 1000;

  socket_status = setsockopt(socket, option_level, option_name, &timeout, sizeof(timeout));
  if (socket_status < 0) {
    printf("Set socket option failed with bsd error: %d\n", errno);
    return SL_STATUS_FAIL;
  }

  PRINT_AT_CMD_SUCCESS;
  return SL_STATUS_OK;
}

sl_status_t si91x_set_custom_sync_sockopt(console_args_t *arguments)
{
  int32_t socket                = (int32_t)arguments->arg[0];
  int32_t option_level          = (int32_t)arguments->arg[1];
  uint32_t option_name          = arguments->arg[2];
  uint8_t ssl_certificate_index = (uint8_t)arguments->arg[3];

  int32_t socket_status =
    setsockopt(socket, option_level, option_name, &ssl_certificate_index, sizeof(ssl_certificate_index));
  VERIFY_BSD_STATUS(socket_status);

  PRINT_AT_CMD_SUCCESS;
  return SL_STATUS_OK;
}

static inline void print_errno(void)
{
  SL_DEBUG_LOG("errno %d", errno);
}

/* Function to get no of set bits in binary
representation of positive integer n */
unsigned int countSetBits(unsigned int number)
{
  unsigned int count = 0;
  while (number) {
    count += number & 1;
    number >>= 1;
  }
  return count;
}

sl_status_t set_socket_config_handler(console_args_t *arguments)
{
  sl_si91x_socket_config_t socket_configuration        = { 0 };
  socket_configuration.total_sockets                   = sl_cli_get_argument_uint8(arguments, 0);
  socket_configuration.total_tcp_sockets               = sl_cli_get_argument_uint8(arguments, 1);
  socket_configuration.total_udp_sockets               = sl_cli_get_argument_uint8(arguments, 2);
  socket_configuration.tcp_tx_only_sockets             = sl_cli_get_argument_uint8(arguments, 3);
  socket_configuration.tcp_rx_only_sockets             = sl_cli_get_argument_uint8(arguments, 4);
  socket_configuration.udp_tx_only_sockets             = sl_cli_get_argument_uint8(arguments, 5);
  socket_configuration.udp_rx_only_sockets             = sl_cli_get_argument_uint8(arguments, 6);
  socket_configuration.tcp_rx_high_performance_sockets = sl_cli_get_argument_uint8(arguments, 7);
  socket_configuration.tcp_rx_window_size_cap          = sl_cli_get_argument_uint8(arguments, 8);
  socket_configuration.tcp_rx_window_div_factor        = sl_cli_get_argument_uint8(arguments, 9);
  return sl_si91x_config_socket(socket_configuration);
}

sl_status_t set_remote_termination_handler()
{
  sl_si91x_set_remote_termination_callback(remote_terminate);
  PRINT_AT_CMD_SUCCESS;
  return SL_STATUS_OK;
}
