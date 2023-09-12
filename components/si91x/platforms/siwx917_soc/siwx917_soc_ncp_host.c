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

#define VERIFY_STATUS(s)   \
  do {                     \
    if (s != SL_STATUS_OK) \
      return s;            \
  } while (0);

typedef struct {
  sl_wifi_buffer_t *head;
  sl_wifi_buffer_t *tail;
  osMutexId_t mutex;
  uint32_t flag;
} si91x_packet_queue_t;

osEventFlagsId_t si91x_events       = 0;
osEventFlagsId_t si91x_bus_events   = 0;
osEventFlagsId_t si91x_async_events = 0;
osThreadId_t si91x_thread           = 0;
osMutexId_t si91x_bus_mutex         = 0;
osThreadId_t si91x_event_thread     = 0;

static si91x_packet_queue_t cmd_queues[SI91X_QUEUE_MAX];

extern void si91x_bus_thread(void *args);
extern void si91x_event_handler_thread(void *args);
extern int16_t rsi_mem_rd(uint32_t addr, uint16_t len, uint8_t *dBuf);

void sl_si91x_host_enable_high_speed_bus()
{
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
}

uint32_t sl_si91x_host_get_wake_indicator(void)
{
  return 1;
}

sl_status_t sl_si91x_host_init(void)
{
  int i;
  sl_status_t status = SL_STATUS_OK;
  memset(cmd_queues, 0, sizeof(cmd_queues));
  if (NULL == si91x_events) {
    si91x_events = osEventFlagsNew(NULL);
  }

  if (NULL == si91x_bus_events) {
    si91x_bus_events = osEventFlagsNew(NULL);
  }

  if (NULL == si91x_async_events) {
    si91x_async_events = osEventFlagsNew(NULL);
  }
  sl_si91x_ta_events_init();
  if (NULL == si91x_thread) {
    const osThreadAttr_t attr = {

      .name       = "si91x_bus",
      .priority   = osPriorityRealtime,
      .stack_mem  = 0,
      .stack_size = 1536,
      .cb_mem     = 0,
      .cb_size    = 0,
      .attr_bits  = 0u,
      .tz_module  = 0u,
    };
    si91x_thread = osThreadNew(si91x_bus_thread, NULL, &attr);
  }

  if (NULL == si91x_event_thread) {
    const osThreadAttr_t attr = {
      .name       = "si91x_event",
      .priority   = osPriorityRealtime1,
      .stack_mem  = 0,
      .stack_size = 1536,
      .cb_mem     = 0,
      .cb_size    = 0,
      .attr_bits  = 0u,
      .tz_module  = 0u,
    };
    si91x_event_thread = osThreadNew(si91x_event_handler_thread, NULL, &attr);
  }

  if (NULL == si91x_bus_mutex) {
    si91x_bus_mutex = osMutexNew(NULL);
  }

  for (i = 0; i < SI91X_QUEUE_MAX; i++) {
    cmd_queues[i].head  = NULL;
    cmd_queues[i].tail  = NULL;
    cmd_queues[i].mutex = osMutexNew(NULL);
    cmd_queues[i].flag  = (1 << i);
  }
  return status;
}

sl_status_t sl_si91x_host_deinit(void)
{
  int i;

  // Deallocate all threads, mutexes and event handlers
  if (NULL != si91x_thread) {
    osThreadTerminate(si91x_thread);
    si91x_thread = NULL;
  }

  if (NULL != si91x_event_thread) {
    osThreadTerminate(si91x_event_thread);
    si91x_event_thread = NULL;
  }

  if (NULL != si91x_events) {
    osEventFlagsDelete(si91x_events);
    si91x_events = NULL;
  }

  if (NULL != si91x_bus_events) {
    osEventFlagsDelete(si91x_bus_events);
    si91x_bus_events = NULL;
  }

  if (NULL != si91x_async_events) {
    osEventFlagsDelete(si91x_async_events);
    si91x_async_events = NULL;
  }

  if (NULL != si91x_bus_mutex) {
    osMutexDelete(si91x_bus_mutex);
    si91x_bus_mutex = NULL;
  }

  for (i = 0; i < SI91X_QUEUE_MAX; i++) {
    osMutexDelete(cmd_queues[i].mutex);
    cmd_queues[i].mutex = NULL;
  }
  return SL_STATUS_OK;
}

sl_si91x_host_timestamp_t sl_si91x_host_get_timestamp(void)
{
  return osKernelGetTickCount();
}

sl_si91x_host_timestamp_t sl_si91x_host_elapsed_time(uint32_t starting_timestamp)
{
  return (osKernelGetTickCount() - starting_timestamp);
}

void sl_si91x_host_delay_ms(uint32_t delay_milliseconds)
{
  osDelay(delay_milliseconds);
}

void sl_si91x_host_set_event(uint32_t event_mask)
{
  osEventFlagsSet(si91x_events, event_mask);
}

void sl_si91x_host_set_bus_event(uint32_t event_mask)
{
  osEventFlagsSet(si91x_bus_events, event_mask);
}

void sl_si91x_host_set_async_event(uint32_t event_mask)
{
  osEventFlagsSet(si91x_async_events, event_mask);
}

sl_status_t sl_si91x_host_add_to_queue(sl_si91x_queue_type_t queue, sl_wifi_buffer_t *buffer)
{
  sl_wifi_buffer_t *packet = (sl_wifi_buffer_t *)buffer;
  osMutexAcquire(cmd_queues[queue].mutex, 0xFFFFFFFFUL);
  packet->node.node = NULL;

  if (cmd_queues[queue].tail == NULL) {
    cmd_queues[queue].head = packet;
    cmd_queues[queue].tail = packet;
  } else {
    cmd_queues[queue].tail->node.node = (sl_slist_node_t *)packet;
    cmd_queues[queue].tail            = packet;
  }

  osMutexRelease(cmd_queues[queue].mutex);
  return SL_STATUS_OK;
}

sl_status_t sl_si91x_host_add_to_queue_with_atomic_action(sl_si91x_queue_type_t queue,
                                                          sl_wifi_buffer_t *buffer,
                                                          void *user_data,
                                                          sl_si91x_host_atomic_action_function_t handler)
{
  sl_wifi_buffer_t *packet = (sl_wifi_buffer_t *)buffer;

  osMutexAcquire(cmd_queues[queue].mutex, 0xFFFFFFFFUL);
  if (NULL != handler) {
    handler(user_data);
  }
  packet->node.node = NULL;

  if (cmd_queues[queue].tail == NULL) {
    cmd_queues[queue].head = packet;
    cmd_queues[queue].tail = packet;
  } else {
    cmd_queues[queue].tail->node.node = (sl_slist_node_t *)packet;
    cmd_queues[queue].tail            = packet;
  }

  osMutexRelease(cmd_queues[queue].mutex);
  return SL_STATUS_OK;
}

sl_status_t sl_si91x_host_remove_from_queue(sl_si91x_queue_type_t queue, sl_wifi_buffer_t **buffer)
{
  sl_wifi_buffer_t *packet = NULL;
  osMutexAcquire(cmd_queues[queue].mutex, 0xFFFFFFFFUL);

  if (cmd_queues[queue].tail == NULL) {
    osMutexRelease(cmd_queues[queue].mutex);
    return SL_STATUS_EMPTY;
  }

  packet                 = cmd_queues[queue].head;
  cmd_queues[queue].head = (sl_wifi_buffer_t *)packet->node.node;
  packet->node.node      = NULL;

  if (NULL == cmd_queues[queue].head) {
    cmd_queues[queue].tail = NULL;
  }

  *buffer = (sl_wifi_buffer_t *)packet;
  osMutexRelease(cmd_queues[queue].mutex);

  return SL_STATUS_OK;
}

sl_status_t sl_si91x_host_remove_node_from_queue(sl_si91x_queue_type_t queue,
                                                 sl_wifi_buffer_t **buffer,
                                                 void *user_data,
                                                 sl_si91x_compare_function_t compare_function)
{
  sl_wifi_buffer_t *packet   = NULL;
  sl_wifi_buffer_t *data     = NULL;
  sl_wifi_buffer_t *previous = NULL;
  sl_status_t status         = SL_STATUS_NOT_FOUND;

  osMutexAcquire(cmd_queues[queue].mutex, 0xFFFFFFFFUL);

  if (cmd_queues[queue].tail == NULL) {
    osMutexRelease(cmd_queues[queue].mutex);
    return SL_STATUS_EMPTY;
  }

  packet   = cmd_queues[queue].head;
  previous = NULL;

  while (NULL != packet) {
    if (true == compare_function((sl_wifi_buffer_t *)packet, user_data)) {
      if (NULL == previous) {
        cmd_queues[queue].head = (sl_wifi_buffer_t *)packet->node.node;
      } else {
        previous->node.node = packet->node.node;
      }

      if (cmd_queues[queue].tail == packet) {
        cmd_queues[queue].tail = previous;
      }

      packet->node.node = NULL;
      data              = packet;
      status            = SL_STATUS_OK;
      break;
    }

    previous = packet;
    packet   = (sl_wifi_buffer_t *)packet->node.node;
  }

  if (NULL == cmd_queues[queue].head) {
    cmd_queues[queue].tail = NULL;
  }

  *buffer = (sl_wifi_buffer_t *)data;
  osMutexRelease(cmd_queues[queue].mutex);

  return status;
}

/* This function is used to flush the pending packets from the specified queue */
sl_status_t sl_si91x_host_flush_nodes_from_queue(sl_si91x_queue_type_t queue,
                                                 void *user_data,
                                                 sl_si91x_compare_function_t compare_function,
                                                 sl_si91x_node_free_function_t node_free_function)
{
  sl_status_t status         = SL_STATUS_NOT_FOUND;
  sl_wifi_buffer_t *packet   = NULL;
  sl_wifi_buffer_t *data     = NULL;
  sl_wifi_buffer_t *previous = NULL;

  osMutexAcquire(cmd_queues[queue].mutex, 0xFFFFFFFFUL);

  if (cmd_queues[queue].tail == NULL) {
    osMutexRelease(cmd_queues[queue].mutex);
    return SL_STATUS_EMPTY;
  }

  packet   = cmd_queues[queue].head;
  previous = NULL;

  while (NULL != packet) {
    if (true == compare_function((sl_wifi_buffer_t *)packet, user_data)) {
      data = packet;
      if (NULL == previous) {
        cmd_queues[queue].head = (sl_wifi_buffer_t *)packet->node.node;
        packet                 = cmd_queues[queue].head;
      } else {
        previous->node.node = packet->node.node;
        packet              = (sl_wifi_buffer_t *)packet->node.node;
      }

      if (cmd_queues[queue].tail == data) {
        cmd_queues[queue].tail = previous;
      }

      data->node.node = NULL;
      status          = SL_STATUS_OK;
    } else {
      previous = packet;
      packet   = (sl_wifi_buffer_t *)packet->node.node;
    }

    if (data != NULL) {
      node_free_function(data);
      data = NULL;
    }
  }

  if (NULL == cmd_queues[queue].head) {
    cmd_queues[queue].tail = NULL;
  }

  osMutexRelease(cmd_queues[queue].mutex);
  return status;
}

uint32_t sl_si91x_host_queue_status(sl_si91x_queue_type_t queue)
{
  uint32_t status = 0;

  osMutexAcquire(cmd_queues[queue].mutex, 0xFFFFFFFFUL);
  status = ((NULL == cmd_queues[queue].tail) ? 0 : cmd_queues[queue].flag);
  osMutexRelease(cmd_queues[queue].mutex);

  return status;
}

uint32_t si91x_host_wait_for_event(uint32_t event_mask, uint32_t timeout)
{
  uint32_t result = osEventFlagsWait(si91x_events, event_mask, (osFlagsWaitAny | osFlagsNoClear), timeout);
  osEventFlagsClear(si91x_events, event_mask);
  if (result == (uint32_t)osErrorTimeout || result == (uint32_t)osErrorResource) {
    return 0;
  }
  return result;
}

uint32_t si91x_host_wait_for_bus_event(uint32_t event_mask, uint32_t timeout)
{
  uint32_t result = osEventFlagsWait(si91x_bus_events, event_mask, (osFlagsWaitAny | osFlagsNoClear), timeout);
  osEventFlagsClear(si91x_bus_events, event_mask);
  if (result == (uint32_t)osErrorTimeout || result == (uint32_t)osErrorResource) {
    return 0;
  }
  return result;
}

uint32_t si91x_host_wait_for_async_event(uint32_t event_mask, uint32_t timeout)
{
  uint32_t result = osEventFlagsWait(si91x_async_events, event_mask, (osFlagsWaitAny | osFlagsNoClear), timeout);
  osEventFlagsClear(si91x_async_events, event_mask);
  if (result == (uint32_t)osErrorTimeout || result == (uint32_t)osErrorResource) {
    return 0;
  }
  return result;
}

uint32_t si91x_host_clear_events(uint32_t event_mask)
{
  uint32_t result = osEventFlagsClear(si91x_events, event_mask);
  if (result == (uint32_t)osErrorResource) {
    return 0;
  }
  return result;
}

uint32_t si91x_host_clear_bus_events(uint32_t event_mask)
{
  uint32_t result = osEventFlagsClear(si91x_bus_events, event_mask);
  if (result == (uint32_t)osErrorResource) {
    return 0;
  }
  return result;
}

uint32_t si91x_host_clear_async_events(uint32_t event_mask)
{
  uint32_t result = osEventFlagsClear(si91x_async_events, event_mask);
  if (result == (uint32_t)osErrorResource) {
    return 0;
  }
  return result;
}

void sl_si91x_host_hold_in_reset(void)
{
}

void sl_si91x_host_release_from_reset(void)
{
}

void sl_si91x_host_enable_bus_interrupt(void)
{
}

void sl_si91x_host_disable_bus_interrupt(void)
{
}

sl_status_t sl_si91x_bus_write_memory(uint32_t addr, uint16_t length, const uint8_t *buffer)
{
  UNUSED_PARAMETER(length);
  *(uint32_t *)addr = *(uint32_t *)buffer;
  return 0;
}

sl_status_t sl_si91x_bus_read_memory(uint32_t addr, uint16_t length, uint8_t *buffer)
{
  rsi_mem_rd(addr, length, buffer);
  return 0;
}

sl_status_t sl_si91x_host_power_cycle(void)
{
  return SL_STATUS_OK;
}
