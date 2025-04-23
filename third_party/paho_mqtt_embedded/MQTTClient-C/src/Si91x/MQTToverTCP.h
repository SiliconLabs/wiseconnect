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

#ifndef MQTTOVERTCP_H
#define MQTTOVERTCP_H

#include <stdint.h>
#include "sl_utility.h"
#include "cmsis_os2.h"

#define WAIT_TIMEOOUT                          5000
#define SLI_917_WAIT_FOREVER                   0
#define WIFI_BLOCKED_TIMEOUT_SF                1
#define DEFAULT_TIMEOUT                        SLI_TX_EVENT_WAIT_TIME
#define SLI_TX_EVENT_WAIT_TIME                 (30000 + TX_WAIT_TIME)
#define TX_WAIT_TIME                           0
#define SLI_SOCKET_RECVFROM_RESPONSE_WAIT_TIME ((SLI_917_WAIT_FOREVER * WIFI_BLOCKED_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define SINGLE_PKT_TCP_STREAM_TIMEOUT          WAIT_TIMEOOUT + SLI_SOCKET_RECVFROM_RESPONSE_WAIT_TIME

typedef struct Timer Timer;
struct Timer {
  uint32_t systick_period;
  uint32_t end_time;
};

typedef struct Network Network;
struct Network {
  int socket;
  int (*mqttread)(Network *, unsigned char *, int, int);
  int (*mqttwrite)(Network *, unsigned char *, int, int);
  void (*disconnect)(Network *);
};
uint32_t osKernelGetTickCount(void);
char expired(Timer *);
void countdown_ms_mqtt(Timer *, unsigned int);
void countdown(Timer *, unsigned int);
int left_ms_mqtt(Timer *);
void InitTimer(Timer *);
int ConnectNetwork(Network *n, uint8_t flags, char *addr, int dst_port, int src_port, bool ssl);
#endif
