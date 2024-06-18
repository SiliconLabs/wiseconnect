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
#pragma once

#include "sl_status.h"
#include "sl_wifi_types.h"
#include "sl_wifi_host_interface.h"
#include "sl_si91x_types.h"
#include "sl_wifi_device.h"
#include <stdbool.h>
#include <stdint.h>

typedef sl_status_t (*sl_si91x_host_rx_irq_handler)(void);
typedef void (*sl_si91x_host_rx_done_handler)(void);

typedef struct {
  sl_si91x_host_rx_irq_handler rx_irq;
  sl_si91x_host_rx_done_handler rx_done;
} sl_si91x_host_init_configuration;

/**
 * @brief 
 *  This API will make RST GPIO to low.
 */
void sl_si91x_host_hold_in_reset(void);

/**
 * @brief 
 *  This API used to release the device from reset state.
 */
void sl_si91x_host_release_from_reset(void);

/**
 * @brief 
 *  This API used to allocate all threads, mutexes and event handlers
 * @return sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details. 
 */
sl_status_t sl_si91x_host_init(sl_si91x_host_init_configuration *config);

/**
  * @brief 
  *  This API used to deallocate all threads, mutexes and event handlers.
  * @return sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
  */
sl_status_t sl_si91x_host_deinit(void);

// ---------------
/**
 * @brief 
 *  This API is responsible for configuring a high speed communication bus.
 */
void sl_si91x_host_enable_high_speed_bus();

/**
 * @brief 
 *  This API is used for processing the data frames.
 * @param interface 
 *  [sl_wifi_interface_t](../wiseconnect-api-reference-guide-wi-fi/sl-wifi-constants#sl-wifi-interface-t) Wi-Fi interface on which the data frame needs to be processed.
 * @param buffer 
 *  pointer to a structure of type [sl_wifi_buffer_t](../wiseconnect-api-reference-guide-wi-fi/sl-wifi-buffer-t) containing the data frame to be processed.
 * @return sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details. 
 */
sl_status_t sl_si91x_host_process_data_frame(sl_wifi_interface_t interface, sl_wifi_buffer_t *buffer);

/**
 * @brief 
 *  Enable interrupts
 * 
 */
void sl_si91x_host_enable_bus_interrupt(void);

/**
 * @brief 
 *  Disable interrupts
 */
void sl_si91x_host_disable_bus_interrupt(void);

/**
 * @brief 
 *  Sets sleep Indication GPIO to HIGH.
 * 
 */
void sl_si91x_host_set_sleep_indicator(void);

/**
* @addtogroup EXTERNAL_HOST_INTERFACE_FUNCTIONS
* @{ 
*/

/**
 * @brief 
 *  Sets sleep Indication GPIO to LOW
 */
void sl_si91x_host_clear_sleep_indicator(void);

/** @} */

/**
 * @brief 
 * Reads Wakeup Indication GPIO value
 * 
 * @return uint32_t 
 */
uint32_t sl_si91x_host_get_wake_indicator(void);

sl_status_t sl_si91x_host_spi_transfer(
  const void *tx_buffer,
  void *rx_buffer,
  uint16_t buffer_length); /*Function used for data transfer between TA and MCU over SPI*/

sl_status_t sl_si91x_host_uart_transfer(
  const void *tx_buffer,
  void *rx_buffer,
  uint16_t buffer_length); /*Function used for data transfer between TA and MCU over UART/USART*/

void sl_si91x_host_flush_uart_rx(void); /*Function used to flush all the old data in the uart/usart rx stream*/

void sl_si91x_host_uart_enable_hardware_flow_control(void); /*Function to enable Hardware Flow Control on host*/

/**
 * @brief Check whether the current CPU operation mode is handler mode
 * 
 * @return true 
 * @return false 
 */
bool sl_si91x_host_is_in_irq_context(void);