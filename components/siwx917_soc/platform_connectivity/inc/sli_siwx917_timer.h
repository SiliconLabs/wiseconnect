/*******************************************************************************
* @file  sli_siwx917_timer.h
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

#include <stdint.h>

/******************************************************
 * *                      Macros
 * ******************************************************/
#define sl_si91x_timer_NODE_0 0

/******************************************************
 * *                 Type Definitions
 * ******************************************************/

typedef struct {
  uint32_t start_time;
  uint32_t timeout;
} sl_si91x_timer_t;

/******************************************************
 * *               Function Declarations
 * ******************************************************/
void sl_si91x_timer_expiry_interrupt_handler(void);
uint32_t sl_si91x_timer_read_counter(void);
void sl_si91x_timer_init(sl_si91x_timer_t *timer, uint32_t duration);
int32_t sl_si91x_timer_expired(sl_si91x_timer_t *timer);
uint32_t sl_si91x_timer_left(sl_si91x_timer_t *timer);

uint32_t rsi_hal_gettickcount(void);
