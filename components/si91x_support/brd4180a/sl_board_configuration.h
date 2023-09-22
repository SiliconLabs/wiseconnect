/*******************************************************************************
* @file  sl_board_configuration.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/
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

#define SLEEP_CONFIRM_PIN  PIN(D, 2)
#define WAKE_INDICATOR_PIN PIN(D, 3)
#define RESET_PIN          PIN(B, 0)
#define INTERRUPT_PIN      PIN(B, 1)

#define SPI_CLOCK_PIN PIN(C, 2)
#define SPI_MOSI_PIN  PIN(C, 0)
#define SPI_MISO_PIN  PIN(C, 1)
#define SPI_CS_PIN    PIN(C, 3)

#define PACKET_PENDING_INT_PRI 3

#define NETWORK_INTERFACE_VALID(x) (x == SL_NET_WIFI_CLIENT_INTERFACE) || (x == SL_NET_WIFI_AP_INTERFACE)
