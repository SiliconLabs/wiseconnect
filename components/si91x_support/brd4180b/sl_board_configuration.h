#pragma once

typedef struct {
  unsigned char port;
  unsigned char pin;
} sl_pin_t;

#define PIN(port_id, pin_id)                 \
  (sl_pin_t)                                 \
  {                                          \
    .port = gpioPort##port_id, .pin = pin_id \
  }

#define SLEEP_CONFIRM_PIN  PIN(B, 1)
#define WAKE_INDICATOR_PIN PIN(B, 0)
#define RESET_PIN          PIN(D, 2)
#define INTERRUPT_PIN      PIN(D, 3)

#define SPI_CLOCK_PIN PIN(C, 2)
#define SPI_MOSI_PIN  PIN(C, 0)
#define SPI_MISO_PIN  PIN(C, 1)
#define SPI_CS_PIN    PIN(C, 3)

#define PACKET_PENDING_INT_PRI 3

#define NETWORK_INTERFACE_VALID(x) \
  (x == SL_NET_WIFI_CLIENT_INTERFACE) || (x == SL_NET_WIFI_AP_INTERFACE)
