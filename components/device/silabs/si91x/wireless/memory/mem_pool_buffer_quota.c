/***************************************************************************/ /**
 * @file
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
#include "sli_mem_pool.h"
#include "sl_status.h"
#include "sl_wifi_host_interface.h"
#include "sl_si91x_host_interface.h"
#include "sl_constants.h"
#include "cmsis_os2.h"
#include "sl_core.h"
#include <string.h>
#include "sl_rsi_utility.h"

#define SLI_BUFFER_TYPE    4
#define SLI_WATERMARKLEVEL 10
static sli_mem_pool_handle_t mem_pool;
static const sl_wifi_buffer_configuration_t *configuration;
void *allocated_wifi_buffer                       = NULL;
static uint8_t buffer_allocation[SLI_BUFFER_TYPE] = { 0, 0, 0, 0 };
static uint8_t quota[SLI_BUFFER_TYPE];

#ifndef SL_WIFI_BUFFERS_FREE_WAIT_TIME
#define SL_WIFI_BUFFERS_FREE_WAIT_TIME 1000 // wait for 1 second to free all the wi-fi buffer
#endif

/*---------------Static Function Declaration---------------------------------------*/
static void sl_si91x_convert_config_structure_to_array(const sl_wifi_buffer_configuration_t *config);
static sl_status_t sl_si91x_check_for_valid_config(const sl_wifi_buffer_configuration_t *config);
static void sl_si91x_buffer_type_deallocation(sl_wifi_buffer_type_t type);
static void sl_si91x_buffer_type_allocation(sl_wifi_buffer_type_t type);
static sl_status_t sl_si91x_check_for_buffer_availability(sl_wifi_buffer_type_t type);
static bool sl_si91x_check_for_buffer_empty(void);
/*---------------------------------------------------------------------------------*/

sl_status_t sli_si91x_host_init_buffer_manager(const sl_wifi_buffer_configuration_t *config)
{
  SL_VERIFY_POINTER_OR_RETURN(config, SL_STATUS_NULL_POINTER);
  sl_si91x_convert_config_structure_to_array(config);
  // Checking for correctness in configuration
  sl_status_t result = sl_si91x_check_for_valid_config(config);
  VERIFY_STATUS_AND_RETURN(result);
  configuration     = config;
  void *pool_buffer = configuration->buffer_memory;
  uint8_t block_count =
    configuration->tx_buffer_quota + configuration->rx_buffer_quota + configuration->control_buffer_quota;
  uint32_t buffer_size = configuration->block_size * block_count;
  if (configuration->buffer_memory == NULL) {
    allocated_wifi_buffer = malloc(buffer_size);
    if (allocated_wifi_buffer == NULL) {
      return SL_STATUS_ALLOCATION_FAILED;
    }
    pool_buffer = allocated_wifi_buffer;
  }

  sli_mem_pool_create(&mem_pool, (uint16_t)configuration->block_size, block_count, pool_buffer, buffer_size);
  return SL_STATUS_OK;
}

sl_status_t sli_si91x_host_deinit_buffer_manager(void)
{
  // Get the current tick count to track elapsed time for deinit
  uint32_t start = osKernelGetTickCount();
  bool result;

  // Ensure all buffer types have been deallocated (buffers are empty)
  do {
    result = sl_si91x_check_for_buffer_empty(); // Check if buffers are empty
  } while ((result == false) && (sl_si91x_host_elapsed_time(start) < SL_WIFI_BUFFERS_FREE_WAIT_TIME)
           && (osDelay(2) == 0)); // Retry until all buffers are deallocated or timeout occurs

  // If some buffers are still not freed, log an error and return failure
  if (!result) {
    SL_DEBUG_LOG("\r\n Invalid operation, some buffers are not freed");
    return SL_STATUS_FAIL;
  }

  // If allocated_wifi_buffer is not NULL, free it and set the pointer to NULL
  if (allocated_wifi_buffer != NULL) {
    free(allocated_wifi_buffer);
    allocated_wifi_buffer = NULL;
  }

  // Clear the memory pool structure to reset all entries
  memset(&mem_pool, 0, sizeof(mem_pool));

  // Return success if buffer deinitialization completed successfully
  return SL_STATUS_OK;
}

sl_status_t sli_si91x_host_allocate_buffer(sl_wifi_buffer_t **buffer,
                                           sl_wifi_buffer_type_t type,
                                           uint32_t buffer_size,
                                           uint32_t wait_duration_ms)
{
  UNUSED_PARAMETER(buffer_size); // Unused parameter kept for consistency or future use

  // Validate input parameter
  if (buffer == NULL) {
    return SL_STATUS_INVALID_PARAMETER; // Return error if buffer is a NULL pointer
  }

  uint32_t start_time = osKernelGetTickCount(); // Capture the current system tick count to measure elapsed time
  uint32_t delay      = 2;                      // Initial delay duration in milliseconds
  sl_status_t result;

  do {
    // Check if buffer quota is available for the given type
    result = sl_si91x_check_for_buffer_availability(type);
    if (result == SL_STATUS_OK) {
      // Try to allocate memory for the buffer from the memory pool
      *buffer = sli_mem_pool_alloc(&mem_pool);
      if (*buffer != NULL) {
        break; // Exit the loop if allocation is successful
      }
    }

    osDelay(delay);                        // Wait to give other tasks CPU time before retrying allocation
    delay = (delay < 50) ? delay * 2 : 50; // Exponential backoff for delay with a maximum cap at 50 ms

  } while (sl_si91x_host_elapsed_time(start_time) <= wait_duration_ms); // Continue until time expires

  // If no buffer was allocated after the wait duration, return an error
  if (*buffer == NULL) {
    return SL_STATUS_ALLOCATION_FAILED;
  }

  // Initialize allocated buffer fields for the caller's usage
  (*buffer)->type      = (uint8_t)type;                                        // Assign buffer type
  (*buffer)->node.node = NULL;                                                 // Set node pointer to NULL
  (*buffer)->length    = configuration->block_size - sizeof(sl_wifi_buffer_t); // Set usable length

  sl_si91x_buffer_type_allocation(type); // Update internal buffer allocation counter for the type

  return SL_STATUS_OK; // Successfully allocated and initialized buffer
}

void *sl_si91x_host_get_buffer_data(sl_wifi_buffer_t *buffer, uint16_t offset, uint16_t *data_length)
{
  if (offset >= buffer->length) {
    return NULL;
  }
  if (data_length) {
    *data_length = (uint16_t)(buffer->length) - offset;
  }
  return (void *)&buffer->data[offset];
}

void sli_si91x_host_free_buffer(sl_wifi_buffer_t *buffer)
{
  if (buffer == NULL) {
    return;
  }
  sli_mem_pool_free(&mem_pool, buffer);
  // Decreasing the count of the current allocation of the buffer type
  sl_si91x_buffer_type_deallocation(buffer->type);
}

static void sl_si91x_convert_config_structure_to_array(const sl_wifi_buffer_configuration_t *config)
{

  quota[SL_WIFI_CONTROL_BUFFER]  = config->control_buffer_quota;
  quota[SL_WIFI_RX_FRAME_BUFFER] = config->rx_buffer_quota;
  quota[SL_WIFI_TX_FRAME_BUFFER] = config->tx_buffer_quota;
  return;
}

//This functions checks whether buffers are available as per quota !!

static sl_status_t sl_si91x_check_for_buffer_availability(sl_wifi_buffer_type_t type)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  if (buffer_allocation[type] < quota[type]) {
    CORE_EXIT_CRITICAL();
    return SL_STATUS_OK;
  }
  CORE_EXIT_CRITICAL();
  return SL_STATUS_FULL;
}

static sl_status_t sl_si91x_check_for_valid_config(const sl_wifi_buffer_configuration_t *config)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  if (config->control_buffer_quota < SLI_WATERMARKLEVEL || config->rx_buffer_quota < SLI_WATERMARKLEVEL
      || config->tx_buffer_quota < SLI_WATERMARKLEVEL) {
    CORE_EXIT_CRITICAL();
    SL_DEBUG_LOG("Quota for buffer types should be atleast 10");
    return SL_STATUS_INVALID_PARAMETER;
  }
  CORE_EXIT_CRITICAL();
  return SL_STATUS_OK;
}

static void sl_si91x_buffer_type_allocation(sl_wifi_buffer_type_t type)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  buffer_allocation[type]++;
  CORE_EXIT_CRITICAL();
  return;
}

static void sl_si91x_buffer_type_deallocation(sl_wifi_buffer_type_t type)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  if (buffer_allocation[type] > 0) {
    buffer_allocation[type]--;
  }
  CORE_EXIT_CRITICAL();
  return;
}

static bool sl_si91x_check_for_buffer_empty(void)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  for (int i = 0; i < SLI_BUFFER_TYPE; i++) {
    if (buffer_allocation[i] != 0) {
      CORE_EXIT_CRITICAL();
      return false;
    }
  }
  CORE_EXIT_CRITICAL();
  return true;
}
