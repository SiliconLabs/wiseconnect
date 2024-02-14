/***************************************************************************/ /**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
#include <stdint.h>
#include <stddef.h>
#include "sl_si91x_status.h"
#include "sl_si91x_types.h"
#include "sl_si91x_constants.h"
#include "sl_si91x_spi_constants.h"
#include "sl_si91x_host_interface.h"
#include "sl_status.h"
#include "sl_additional_status.h"
#include "sl_wifi_constants.h"
#include "sl_constants.h"
#include "sl_rsi_utility.h"

// This macro converts a 32-bit value from host to little-endian byte order
#define htole32(x) (x)

// This macro converts a 16-bit value from host to little-endian byte order
#define htole16(x) (x)

void sli_submit_rx_buffer(void);

sl_status_t sl_si91x_bus_set_interrupt_mask(uint32_t mask)
{
  sl_status_t status;
  // Create a buffer to store interrupt configuration data, depending on the system's bit support
#ifdef RSI_BIT_32_SUPPORT
  uint8_t buffer[4] = { 0, 0, 0, 0 };
#else
  uint8_t buffer[2] = { 0, 0 };
#endif

  // Read the current interrupt mask configuration from memory
  status = sl_si91x_bus_read_memory(RSI_INT_MASK_REG_ADDR, sizeof(buffer), buffer);
  VERIFY_STATUS(status);

  // Modify the interrupt configuration based on the provided mask
  buffer[1] = buffer[1] & 0xFC; // Clear lower 2 bits
  buffer[1] = buffer[1] | mask; // Set the specified interrupt mask

  // Write the updated mask value back to memory
  status = sl_si91x_bus_write_memory(RSI_INT_MASK_REG_ADDR, sizeof(buffer), buffer);
  return status;
}

// Function for clearing an interrupt
sl_status_t si91x_bus_clear_interrupt(uint32_t interrupt_clear_value)
{
#ifdef RSI_BIT_32_SUPPORT
  // Write the provided interrupt clear value to the specified memory address (32-bit version)
  return sl_si91x_bus_write_memory(RSI_INT_CLR_REG_ADDR, 4, &interrupt_clear_value);
#else
  // Write the provided interrupt clear value to the specified memory address (16-bit version)
  return sl_si91x_bus_write_memory(RSI_INT_CLR_REG_ADDR, 2, (uint8_t *)&interrupt_clear_value);
#endif
}

sl_status_t sl_si91x_bus_enable_high_speed(void)
{
  // It writes specific data to a register at address 0x08 with a size of 2 bytes
  return sl_si91x_bus_write_register(0x08, 2, 0x3);
}

sl_status_t si91x_req_wakeup(void)
{
  // Wake device, if needed
  sl_si91x_host_set_sleep_indicator();
  uint32_t timestamp = sl_si91x_host_get_timestamp();
  do {
    if (sl_si91x_host_get_wake_indicator()) {
      sl_si91x_ulp_wakeup_init();
      break;
    }
    if (sl_si91x_host_elapsed_time(timestamp) > 1000) {
      return SL_STATUS_TIMEOUT;
    }
  } while (1);
  return SL_STATUS_OK;
}

void sli_submit_rx_buffer(void)
{
}
