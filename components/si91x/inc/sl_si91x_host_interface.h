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

#define NCP_HOST_DATA_TX_EVENT           (1 << 1) // Notify there is something to transmit
#define NCP_HOST_BUS_RX_EVENT            (1 << 2) // Triggered by IRQ to indicate something to read
#define NCP_HOST_COMMON_RESPONSE_EVENT   (1 << 3) // Indicates RX response received for COMMON command type
#define NCP_HOST_WLAN_RESPONSE_EVENT     (1 << 4) // Indicates synchronous RX response received for WLAN command type
#define NCP_HOST_WLAN_NOTIFICATION_EVENT (1 << 5) // Indicates asynchronous RX response received for WLAN command type
#define NCP_HOST_NETWORK_RESPONSE_EVENT  (1 << 6) // Indicates synchronous RX response received for NETWORK command type
#define NCP_HOST_NETWORK_NOTIFICATION_EVENT \
  (1 << 7)                                      // Indicates asynchronous RX response received for NETWORK command type
#define NCP_HOST_SOCKET_RESPONSE_EVENT (1 << 8) // Indicates RX response received for SOCKET command type
#define NCP_HOST_SOCKET_NOTIFICATION_EVENT \
  (1 << 9)                                   // Indicates asynchronous RX response received for SOCKET command type
#define NCP_HOST_BT_RESPONSE_EVENT (1 << 10) // Indicates RX response received for BLE command type

typedef enum { NCP_SWITCH_ON, NCP_SWITCH_OFF } si91x_host_switch_t;

typedef struct {
  sl_wifi_performance_profile_t wifi_performance_profile;
  sl_bt_performance_profile_t bt_performance_profile;
  sl_si91x_coex_mode_t coex_mode;
} sl_si91x_performance_profile_t;

typedef uint32_t sl_si91x_host_timestamp_t;

typedef void (*sl_si91x_host_atomic_action_function_t)(void *user_data);
typedef bool (*sl_si91x_compare_function_t)(sl_wifi_buffer_t *node, void *user_data);
typedef void (*sl_si91x_node_free_function_t)(sl_wifi_buffer_t *node);

void sl_si91x_host_hold_in_reset(void);
void sl_si91x_host_release_from_reset(void);

sl_status_t sl_si91x_host_init(void);
sl_status_t sl_si91x_host_deinit(void);

void sl_si91x_host_delay_ms(uint32_t delay_milliseconds);
sl_si91x_host_timestamp_t sl_si91x_host_get_timestamp(void);
sl_si91x_host_timestamp_t sl_si91x_host_elapsed_time(uint32_t starting_timestamp);

// Event API
void sl_si91x_host_set_event(uint32_t event_mask);
void sl_si91x_host_set_bus_event(uint32_t event_mask);
void sl_si91x_host_set_async_event(uint32_t event_mask);
uint32_t si91x_host_wait_for_event(uint32_t event_mask, uint32_t timeout);
uint32_t si91x_host_wait_for_bus_event(uint32_t event_mask, uint32_t timeout);
uint32_t si91x_host_wait_for_async_event(uint32_t event_mask, uint32_t timeout);
uint32_t si91x_host_clear_events(uint32_t event_mask);
uint32_t si91x_host_clear_bus_events(uint32_t event_mask);
uint32_t si91x_host_clear_async_events(uint32_t event_mask);

// ---------------
sl_status_t sl_si91x_host_allocate_buffer(sl_wifi_buffer_t **buffer,
                                          sl_wifi_buffer_type_t type,
                                          uint32_t buffer_size,
                                          uint32_t wait_duration_ms);
void *sl_si91x_host_get_buffer_data(sl_wifi_buffer_t *buffer, uint16_t offset, uint16_t *data_length);
void sl_si91x_host_free_buffer(sl_wifi_buffer_t *buffer, sl_wifi_buffer_type_t type);
// ---------------

sl_status_t sl_si91x_host_get_credentials(sl_wifi_credential_id_t id, uint8_t type, sl_wifi_credential_t *cred);

sl_status_t sl_si91x_host_add_to_queue(sl_si91x_queue_type_t queue, sl_wifi_buffer_t *buffer);
sl_status_t sl_si91x_host_add_to_queue_with_atomic_action(sl_si91x_queue_type_t queue,
                                                          sl_wifi_buffer_t *buffer,
                                                          void *user_data,
                                                          sl_si91x_host_atomic_action_function_t handler);
sl_status_t sl_si91x_host_remove_from_queue(sl_si91x_queue_type_t queue, sl_wifi_buffer_t **buffer);
sl_status_t sl_si91x_host_remove_node_from_queue(sl_si91x_queue_type_t queue,
                                                 sl_wifi_buffer_t **buffer,
                                                 void *user_data,
                                                 sl_si91x_compare_function_t compare_function);
/* Function used to flush the pending TX packets from the specified queue */
sl_status_t sl_si91x_host_flush_nodes_from_queue(sl_si91x_queue_type_t queue,
                                                 void *user_data,
                                                 sl_si91x_compare_function_t compare_function,
                                                 sl_si91x_node_free_function_t node_free_function);
uint32_t sl_si91x_host_queue_status(sl_si91x_queue_type_t queue);

sl_status_t sl_si91x_host_spi_transfer(const void *tx_buffer, void *rx_buffer, uint16_t buffer_length);

sl_status_t sl_si91x_host_bus_init(void);
void sl_si91x_host_enable_high_speed_bus();
void sl_si91x_host_gpio_init(void);
void sl_si91x_host_usart2_init(void);

sl_status_t sl_si91x_host_process_data_frame(sl_wifi_interface_t interface, sl_wifi_buffer_t *buffer);

// These aren't host APIs. These should go into a wifi bus API header
sl_status_t sl_si91x_bus_read_memory(uint32_t addr, uint16_t length, uint8_t *buffer);
sl_status_t sl_si91x_bus_write_memory(uint32_t addr, uint16_t length, const uint8_t *buffer);
sl_status_t sl_si91x_bus_read_register(uint8_t address, uint8_t register_size, uint16_t *output);
sl_status_t sl_si91x_bus_write_register(uint8_t address, uint8_t register_size, uint16_t data);
sl_status_t sl_si91x_bus_read_frame(sl_wifi_buffer_t **buffer);
sl_status_t sl_si91x_bus_write_frame(sl_si91x_packet_t *packet, const uint8_t *payloadparam, uint16_t size_param);
sl_status_t sl_si91x_bus_init();
sl_status_t sl_si91x_bus_enable_high_speed();
sl_status_t sl_si91x_bus_read_interrupt_status(uint16_t *interrupt_status);
sl_status_t sl_si91x_bus_set_interrupt_mask(uint32_t mask);

void sl_si91x_host_enable_bus_interrupt(void);
void sl_si91x_host_disable_bus_interrupt(void);
void sl_si91x_host_set_sleep_indicator(void);
void sl_si91x_host_clear_sleep_indicator(void);
uint32_t sl_si91x_host_get_wake_indicator(void);
void sl_si91x_ulp_wakeup_init(void);

sl_status_t sl_si91x_semaphore_wait(sl_si91x_semaphore_handle_t *semaphore, uint32_t timeout_ms);
sl_status_t sl_si91x_semaphore_post(sl_si91x_semaphore_handle_t *semaphore);
sl_status_t sl_si91x_semaphore_create(sl_si91x_semaphore_handle_t *semaphore, uint32_t count);

// Unimplemented functions
bool sl_si91x_host_is_in_interrupt_context(void);
void sl_si91x_host_init_queue(sl_si91x_queue_type_t queue);
void sl_si91x_host_ldma_init(void);

/** @} */
