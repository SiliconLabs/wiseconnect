#pragma once

#include <stddef.h>

#define DEFAULT_UART            NULL
#define DEFAULT_UART_PIN_CONFIG NULL

#define NETWORK_INTERFACE_VALID(x) \
  (x == SL_NET_WIFI_CLIENT_INTERFACE) || (x == SL_NET_WIFI_AP_INTERFACE)
