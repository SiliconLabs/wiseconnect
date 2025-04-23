/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Allan Stockdill-Mander - initial API and implementation and/or initial documentation
 *******************************************************************************/

#include "MQTToverTCP.h"
#include "sl_constants.h"
#include <string.h>
#include <stdint.h>
#include "cmsis_os2.h"
#include "socket.h"
#include "sl_net.h"
#include "errno.h"
#include "sl_si91x_socket_types.h"

//For TLS
#include "sl_si91x_socket_constants.h"
#include "sl_si91x_constants.h"

char expired(Timer *timer)
{
  long left = timer->end_time - osKernelGetTickCount();
  return (left < 0);
}

void countdown_ms_mqtt(Timer *timer, unsigned int timeout)
{
  timer->end_time = osKernelGetTickCount() + timeout;
}

void countdown(Timer *timer, unsigned int timeout)
{
  timer->end_time = osKernelGetTickCount() + (timeout * 1000);
}

int left_ms_mqtt(Timer *timer)
{
  long left = timer->end_time - osKernelGetTickCount();
  return (left < 0) ? 0 : left;
}

void InitTimer(Timer *timer)
{
  timer->end_time = 0;
}

static int mqtt_read(Network *n, unsigned char *buffer, int len, int timeout_ms)
{
  struct timeval timeout;
  timeout.tv_sec  = timeout_ms / 1000;
  timeout.tv_usec = (timeout_ms % 1000) * 1000;

  // Set socket receive timeout
  setsockopt(n->socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

  int bytes = recv(n->socket, buffer, len, 0);
  return (bytes < 0) ? -1 : bytes;
}

static int mqtt_write(Network *n, unsigned char *buffer, int len, int timeout_ms)
{
  UNUSED_PARAMETER(timeout_ms);
  return send(n->socket, buffer, len, 0);
}

static void mqtt_disconnect(Network *n)
{
  close(n->socket);
}

void NewNetwork(Network *n)
{
  n->socket     = -1;
  n->mqttread   = mqtt_read;
  n->mqttwrite  = mqtt_write;
  n->disconnect = mqtt_disconnect;
}

int ConnectNetwork(Network *n, uint8_t flags, char *addr, int dst_port, int src_port, bool ssl)
{
  UNUSED_PARAMETER(flags);
  int type = SOCK_STREAM;
  int rc   = -1;
  int status;
#ifdef SLI_SI91X_ENABLE_IPV6
  struct sockaddr_in6 server_address_v6 = { 0 };
  struct sockaddr_in6 clientAddr_v6     = { 0 };
  socklen_t socket_length_v6            = sizeof(struct sockaddr_in6);
#else
  struct sockaddr_in server_address = { 0 };
  struct sockaddr_in client_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
#endif

#ifdef SLI_SI91X_ENABLE_IPV6
  server_address_v6.sin6_family = AF_INET6;
  server_address_v6.sin6_port   = dst_port;

  memcpy(&server_address_v6.sin6_addr.__u6_addr.__u6_addr8, addr, SL_IPV6_ADDRESS_LENGTH);

  if (ssl)
    n->socket = socket(AF_INET, type, IPPROTO_TCP);
  else
    n->socket = socket(AF_INET, type, IPPROTO_TCP);

  if (n->socket < 0) {
    printf("\r\nSocket creation failed with bsd error: %d\r\n", errno);
    return -1;
  }

  clientAddr_v6.sin6_family = AF_INET6;
  clientAddr_v6.sin6_port   = src_port;

#else
  //! Set family type
  server_address.sin_family = AF_INET;
  //! Set local port number
  server_address.sin_port = dst_port;

  memcpy(&server_address.sin_addr.s_addr, addr, sizeof(server_address.sin_addr.s_addr));

  if (ssl)
    n->socket = socket(AF_INET, type, IPPROTO_TCP);
  else
    n->socket = socket(AF_INET, type, IPPROTO_TCP);

  if (n->socket < 0) {
    printf("\r\nSocket creation failed with bsd error: %d\r\n", errno);
    return -1;
  }
  client_address.sin_family = AF_INET;
  client_address.sin_port   = src_port;
#endif

  //To enable SSL/TLS
  if (ssl) {
    int socket_return_value = 0;
    socket_return_value     = setsockopt(n->socket, SOL_SOCKET, TCP_ULP, TLS, sizeof(TLS));
    if (socket_return_value < 0) {
      printf("\r\nSet Socket option failed with bsd error: %d\r\n", errno);
      close(n->socket);
    }
  }

#ifdef SLI_SI91X_ENABLE_IPV6
  status = bind(n->socket, (struct sockaddr *)&clientAddr_v6, socket_length_v6);
#else
  //! Bind socket
  status = bind(n->socket, (struct sockaddr *)&client_address, socket_length);
#endif

  if (status != SL_STATUS_OK) {
    printf("\r\nSocket bind failed with bsd error: %d\r\n", errno);
    mqtt_disconnect(n);
    return status;
  }

#ifdef SLI_SI91X_ENABLE_IPV6
  rc = connect(n->socket, (struct sockaddr *)&server_address_v6, socket_length_v6);
#else
  rc     = connect(n->socket, (struct sockaddr *)&server_address, socket_length);
#endif
  if (rc == -1) {
    printf("\r\nSocket Connect failed with bsd error: %d\r\n", errno);
    ;
    close(n->socket);
    return rc;
  }
  printf("\nSocket connection success \n");
  return status;
}
