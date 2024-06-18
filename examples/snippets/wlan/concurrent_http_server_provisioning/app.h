/***************************************************************************/ /**
 * @file app.h
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef APP_H
#define APP_H

/***************************************************************************/ /**
 * Initialize application.
 ******************************************************************************/
void app_init(void);

#include <stdio.h>
#include <stdint.h>

#define APP_BUF_SIZE 1600

//! Enumeration for states in application
typedef enum app_state_e {
  INITIAL_STATE = 0,       // This state is to bring up module as AP.
  PROVISION_STATE,         // This state is to get Third-party AP credentials through HTTP SERVER & AP DOWN
  STA_BRINGUP_STATE,       // This state is to bring module as CONC_STA
  AP_BRINGUP_STATE,        // This state is to bring module as CONC_AP & start HTTP SERVER
  DATA_TRANSFER_STATE,     // This state is to perform data transfer on STA & AP Vaps
  WAIT_STATE,              // This state is to keep the module in idle state
  MODULE_DISCONNECT_STATE, // This state is to stop HTTP SERVER & CONC_AP upon rejoin failure
} app_state_t;

//! WLAN application control block
typedef struct app_cb_s {
  volatile app_state_t state;   //! WLAN application state
  uint32_t length;              //! length of buffer to copy
  uint8_t buffer[APP_BUF_SIZE]; //! application buffer
  uint8_t buf_in_use;           //! to check application buffer availability
  uint32_t event_map;           //! application events bit map
} app_cb_t;

/***************************************************************************/ /**
 * App ticking function.
 ******************************************************************************/
void app_process_action(void);

#endif // APP_H
