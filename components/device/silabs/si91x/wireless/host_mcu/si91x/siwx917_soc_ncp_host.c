/*******************************************************************************
* @file  siwx917_soc_ncp_host.c
* @brief
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_wifi_constants.h"
#include "sl_si91x_host_interface.h"
#include "sl_board_configuration.h"
#include "sl_status.h"
#include "sl_constants.h"
#include "os_tick.h"   // CMSIS RTOS2
#include "cmsis_os2.h" // CMSIS RTOS2
#include "sl_si91x_types.h"
#include <stdbool.h>
#include <string.h>
#include "rsi_m4.h"
#include "sl_common.h"
#include "si91x_device.h"
#include "core_cm4.h"

extern int16_t rsi_mem_rd(uint32_t addr, uint16_t len, uint8_t *dBuf);
void sl_si91x_ta_events_init(void);
sl_status_t sl_si91x_bus_init(void);
sl_status_t sl_si91x_bus_set_interrupt_mask(uint32_t mask);
sl_status_t sl_si91x_bus_enable_high_speed(void);
sl_status_t sl_si91x_bus_write_memory(uint32_t addr, uint16_t length, const uint8_t *buffer);
sl_status_t sl_si91x_bus_read_memory(uint32_t addr, uint16_t length, uint8_t *buffer);

void sl_si91x_host_enable_high_speed_bus()
{
  //! Sets specific control register bits
  //! Sets the baud rate
}

sl_status_t sl_si91x_bus_rx_irq_handler(void)
{
  return SL_STATUS_OK;
}

void sl_si91x_bus_rx_done_handler(void)
{
  return;
}

sl_status_t sl_si91x_bus_init(void)
{
  return RSI_SUCCESS;
}

sl_status_t sl_si91x_bus_set_interrupt_mask(uint32_t mask)
{
  UNUSED_PARAMETER(mask);
  return SL_STATUS_OK;
}

sl_status_t sl_si91x_bus_enable_high_speed(void)
{
  return SL_STATUS_OK;
}

void sl_si91x_host_set_sleep_indicator(void)
{
  // sets a sleep indicator by activating a specific GPIO pin
}

uint32_t sl_si91x_host_get_wake_indicator(void)
{
  return 1;
}

sl_status_t sl_si91x_host_init(sl_si91x_host_init_configuration *config)
{
  UNUSED_PARAMETER(config);
  // Initialize SI91X TA events
  sl_si91x_ta_events_init();
  return SL_STATUS_OK;
}

sl_status_t sl_si91x_host_deinit(void)
{
  return SL_STATUS_OK;
}

void sl_si91x_host_hold_in_reset(void)
{
  // initializes and activates a GPIO pin to forcefully reset or disable a connected device.
}

void sl_si91x_host_release_from_reset(void)
{
  // Release the reset state of a connected device
}

void sl_si91x_host_enable_bus_interrupt(void)
{
  // allowing the system to respond to specific events
}

void sl_si91x_host_disable_bus_interrupt(void)
{
  // Preventing the system from responding to events
}

sl_status_t sl_si91x_bus_write_memory(uint32_t addr, uint16_t length, const uint8_t *buffer)
{
  UNUSED_PARAMETER(length);
  *(uint32_t *)addr = *(uint32_t *)buffer;
  return 0;
}

sl_status_t sl_si91x_bus_read_memory(uint32_t addr, uint16_t length, uint8_t *buffer)
{
  // Read memory
  rsi_mem_rd(addr, length, buffer);
  return 0;
}
bool sl_si91x_host_is_in_irq_context(void)
{
  return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0U;
}
