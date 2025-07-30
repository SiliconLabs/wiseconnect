/***************************************************************************/ /**
 * @file sli_hal_si91x.c
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sli_hal_si91x_constants.h"
#include "sli_queue_manager.h"
#include "sli_routing_utility.h"
#include "sli_hal_si91x_packet.h"
#include "sli_hal_si91x.h"
#include "sl_constants.h"
#include "sl_rsi_utility.h"
#include "sl_si91x_host_interface.h"
#include "sl_status.h"
#include "cmsis_os2.h"

/******************************************************
 *               Macros
******************************************************/
#define SLI_HAL_SI91X_THREAD_STACK 1636                   ///< Thread stack size
#define SLI_HAL_SI91X_THREAD_NAME  "sli_hal_si91x_thread" ///< Thread name for HAL

#define SLI_HAL_SI91X_LOG_MESSAGE_ON_ERROR(return_value, expected_value, message) \
  do {                                                                            \
    if ((return_value) != (expected_value)) {                                     \
      SL_DEBUG_LOG(message, return_value);                                        \
    }                                                                             \
  } while (0)
/******************************************************
 *               Function Declarations
******************************************************/
// Command Engine RX packet handler
// TODO: Update the with appropriate handler after refactoring of Command Engine is done.
extern sl_status_t sli_si91x_set_rx_event(void *packet,
                                          uint32_t packet_size,
                                          sli_routing_utility_packet_status_handler_t packet_status_handler,
                                          void *context);

static void sli_hal_si91x_wait_for_event_listener();

static void sli_hal_si91x_clean_up_resources();

static sl_status_t sli_hal_si91x_send_packet(void *packet,
                                             sli_queue_t *tx_queue,
                                             uint32_t event_flag,
                                             uint32_t packet_size,
                                             sli_routing_utility_packet_status_handler_t packet_status_handler,
                                             void *context);
static void sli_hal_si91x_send_packet_to_bus(sl_wifi_buffer_t *buffer);
/******************************************************
 *               Variable Definitions
 ******************************************************/
// TX and RX queue handles
static sli_queue_t wifi_tx_queue_handle;
static sli_queue_t ble_tx_queue_handle;
static sli_queue_t rx_queue_handle;

// Event listener thread ID
static osThreadId_t hal_thread_ID = NULL;

// Event flags
static osEventFlagsId_t sli_hal_si91x_events = NULL;

// Routing entries for HAL
static sli_routing_entry_t hal_si91x_routing_entires[SLI_HAL_SI91X_PACKET_MAX] = { [SLI_HAL_SI91X_PACKET] = {
                                                                                     .destination_packet_handler =
                                                                                       sli_si91x_set_rx_event,
                                                                                     .packet_status_handler = NULL,
                                                                                     .packet_type =
                                                                                       SLI_HAL_SI91X_PACKET,
                                                                                   } };

// Routing table for HAL
sli_routing_table_t hal_si91x_routing_table = { .routing_table      = hal_si91x_routing_entires,
                                                .routing_table_size = SLI_HAL_SI91X_PACKET_MAX };
/******************************************************
 *              Static Function Definitions
******************************************************/

static sl_status_t sli_hal_si91x_send_packet(void *packet,
                                             sli_queue_t *tx_queue,
                                             uint32_t event_flag,
                                             uint32_t packet_size,
                                             sli_routing_utility_packet_status_handler_t packet_status_handler,
                                             void *context)
{

  // Calling this method after calling deinit is fatal.
  UNUSED_PARAMETER(context);
  UNUSED_PARAMETER(packet_size);

  // TODO: What needs to be done with packet_status_handler
  if (packet_status_handler != NULL) {
  }

  sl_status_t status = sli_queue_manager_enqueue(tx_queue, (sl_slist_node_t *)packet);

  VERIFY_STATUS_AND_RETURN(status);
  osEventFlagsSet(sli_hal_si91x_events, event_flag);

  return SL_STATUS_IN_PROGRESS;
}

static void sli_hal_si91x_wait_for_event_listener()
{
  uint32_t events_received = 0;
  sl_slist_node_t *node    = NULL;

  while (1) {
    events_received |=
      osEventFlagsWait(sli_hal_si91x_events,
                       (SLI_HAL_SI91X_WIFI_TX_EVENT | SLI_HAL_SI91X_RX_EVENT | SLI_HAL_SI91X_BLE_TX_EVENT),
                       osFlagsWaitAny,
                       osWaitForever);

    if ((events_received & SLI_HAL_SI91X_WIFI_TX_EVENT)
        && (SL_STATUS_OK == sli_queue_manager_dequeue(&wifi_tx_queue_handle, (void **)&node))) {
      sli_hal_si91x_send_packet_to_bus((sl_wifi_buffer_t *)node);

      events_received &= ~SLI_HAL_SI91X_WIFI_TX_EVENT;
    }

    if (events_received & SLI_HAL_SI91X_BLE_TX_EVENT
        && (SL_STATUS_OK == sli_queue_manager_dequeue(&ble_tx_queue_handle, (void **)&node))) {
      sli_hal_si91x_send_packet_to_bus((sl_wifi_buffer_t *)node);

      events_received &= ~SLI_HAL_SI91X_BLE_TX_EVENT;
    }

    if ((events_received & SLI_HAL_SI91X_RX_EVENT)
        && (SL_STATUS_OK == sli_queue_manager_dequeue(&rx_queue_handle, (void **)&node))) {
      sl_wifi_buffer_t *rx_buffer = (sl_wifi_buffer_t *)node;

      sli_routing_utility_route_packet(&hal_si91x_routing_table,
                                       SLI_HAL_SI91X_PACKET,
                                       (void *)rx_buffer,
                                       sizeof(sl_wifi_buffer_t) + rx_buffer->length,
                                       NULL);
      events_received &= ~SLI_HAL_SI91X_RX_EVENT;
    }

    node = NULL;
  }
}

static void sli_hal_si91x_send_packet_to_bus(sl_wifi_buffer_t *buffer)
{
  sl_wifi_system_packet_t *packet = NULL;

  packet = sli_wifi_host_get_buffer_data(buffer, 0, NULL);

  sl_si91x_host_clear_sleep_indicator();

  sl_status_t status = sli_si91x_bus_write_frame(packet, packet->data, packet->length);

  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG("\r\n BUS_WRITE_ERROR \r\n");
    // TODO: Use error_queue once the PR is merged.
    // BREAKPOINT();
  }

  sl_si91x_host_clear_sleep_indicator();
  sli_si91x_host_free_buffer(buffer);
}

static void sli_hal_si91x_clean_up_resources(void)
{
  osStatus_t freertos_status;
  sl_status_t queue_deinit_status = SL_STATUS_OK;

  if (hal_thread_ID != NULL) {
    freertos_status = osThreadTerminate(hal_thread_ID);
    hal_thread_ID   = NULL;

    SLI_HAL_SI91X_LOG_MESSAGE_ON_ERROR(freertos_status, osOK, "Thread termination failed with status %d");
  }

  queue_deinit_status = sli_queue_manager_deinit(&wifi_tx_queue_handle, NULL);
  SLI_HAL_SI91X_LOG_MESSAGE_ON_ERROR(queue_deinit_status, SL_STATUS_OK, "Wi-Fi TX queue deinit failed with status %d");

  queue_deinit_status = sli_queue_manager_deinit(&ble_tx_queue_handle, NULL);
  SLI_HAL_SI91X_LOG_MESSAGE_ON_ERROR(queue_deinit_status, SL_STATUS_OK, "BLE TX queue deinit failed with status %d");

  queue_deinit_status = sli_queue_manager_deinit(&rx_queue_handle, NULL);
  SLI_HAL_SI91X_LOG_MESSAGE_ON_ERROR(queue_deinit_status, SL_STATUS_OK, "RX queue deinit failed with status %d");

  if (sli_hal_si91x_events != NULL) {
    freertos_status      = osEventFlagsDelete(sli_hal_si91x_events);
    sli_hal_si91x_events = NULL;

    SLI_HAL_SI91X_LOG_MESSAGE_ON_ERROR(freertos_status, osOK, "Event flags deletion failed with status %d");
  }
}

/******************************************************
 *               Function Definitions
******************************************************/
sl_status_t sli_hal_si91x_init(void)
{

  sli_queue_manager_init(&wifi_tx_queue_handle, SLI_BUFFER_MANAGER_QUEUE_NODE_POOL);
  sli_queue_manager_init(&ble_tx_queue_handle, SLI_BUFFER_MANAGER_QUEUE_NODE_POOL);
  sli_queue_manager_init(&rx_queue_handle, SLI_BUFFER_MANAGER_QUEUE_NODE_POOL);

  // Create and start HAL thread
  if (NULL == hal_thread_ID) {
    const osThreadAttr_t attr = {
      .name       = SLI_HAL_SI91X_THREAD_NAME,
      .priority   = osPriorityRealtime,
      .stack_mem  = 0,
      .stack_size = SLI_HAL_SI91X_THREAD_STACK,
      .cb_mem     = 0,
      .cb_size    = 0,
      .attr_bits  = 0u,
      .tz_module  = 0u,
    };

    hal_thread_ID = osThreadNew(&sli_hal_si91x_wait_for_event_listener, NULL, &attr);
  }

  if (NULL == hal_thread_ID) {
    sli_hal_si91x_clean_up_resources();

    SL_DEBUG_LOG("Creation of HAL thread failed");
    return SL_STATUS_FAIL;
  }

  sli_hal_si91x_events = osEventFlagsNew(NULL);

  if (sli_hal_si91x_events == NULL) {
    sli_hal_si91x_clean_up_resources();

    SL_DEBUG_LOG("Creation of HAL event flags failed");
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

sl_status_t sli_hal_si91x_deinit(void)
{
  // Calling this method after without calling init may cause issues.
  sli_hal_si91x_clean_up_resources();
  return SL_STATUS_OK;
}

sl_status_t sli_hal_si91x_command_send_packet(void *packet,
                                              uint32_t packet_size,
                                              sli_routing_utility_packet_status_handler_t packet_status_handler,
                                              void *context)
{
  return sli_hal_si91x_send_packet(packet,
                                   &wifi_tx_queue_handle,
                                   SLI_HAL_SI91X_WIFI_TX_EVENT,
                                   packet_size,
                                   packet_status_handler,
                                   context);
}

sl_status_t sli_hal_si91x_data_send_packet(void *packet,
                                           uint32_t packet_size,
                                           sli_routing_utility_packet_status_handler_t packet_status_handler,
                                           void *context)
{
  return sli_hal_si91x_send_packet(packet,
                                   &wifi_tx_queue_handle,
                                   SLI_HAL_SI91X_WIFI_TX_EVENT,
                                   packet_size,
                                   packet_status_handler,
                                   context);
}

sl_status_t sli_hal_si91x_common_send_packet(void *packet,
                                             uint32_t packet_size,
                                             sli_routing_utility_packet_status_handler_t packet_status_handler,
                                             void *context)
{
  return sli_hal_si91x_send_packet(packet,
                                   &wifi_tx_queue_handle,
                                   SLI_HAL_SI91X_WIFI_TX_EVENT,
                                   packet_size,
                                   packet_status_handler,
                                   context);
}

sl_status_t sli_hal_si91x_ble_send_packet(void *packet,
                                          uint32_t packet_size,
                                          sli_routing_utility_packet_status_handler_t packet_status_handler,
                                          void *context)
{
  return sli_hal_si91x_send_packet(packet,
                                   &ble_tx_queue_handle,
                                   SLI_HAL_SI91X_BLE_TX_EVENT,
                                   packet_size,
                                   packet_status_handler,
                                   context);
}