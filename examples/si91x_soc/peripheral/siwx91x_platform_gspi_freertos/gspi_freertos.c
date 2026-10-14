/***************************************************************************/ /**
 * @file gspi_freertos.c
 * @brief GSPI FreeRTOS example: master transfer/send/receive with semaphore
 *        synchronization and loopback data comparison.
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
/* GSPI driver and project config */
#include "sl_si91x_gspi.h"
#include "sl_si91x_gspi_common_config.h"
#include "gspi_freertos.h"
#include "rsi_debug.h"
/* FreeRTOS / CMSIS-RTOS2 */
#include "cmsis_os2.h"
#include "rsi_rom_clks.h"
/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define GSPI_BUFFER_SIZE             1024      // Size of buffer
#define GSPI_INTF_PLL_CLK            180000000 // Intf pll clock frequency
#define GSPI_INTF_PLL_REF_CLK        40000000  // Intf pll reference clock frequency
#define GSPI_SOC_PLL_CLK             20000000  // Soc pll clock frequency
#define GSPI_SOC_PLL_REF_CLK         40000000  // Soc pll reference clock frequency
#define GSPI_INTF_PLL_500_CTRL_VALUE 0xD900    // Intf pll control value
#define GSPI_SOC_PLL_MM_COUNT_LIMIT  0xA4      // Soc pll count limit
#define GSPI_DVISION_FACTOR          0         // Division factor
#define GSPI_SWAP_READ_DATA          1         // true to enable and false to disable swap read
#define GSPI_SWAP_WRITE_DATA         0         // true to enable and false to disable swap write
#define GSPI_BITRATE                 10000000  // Bitrate for setting the clock division factor
#define GSPI_BIT_WIDTH               8         // Default Bit width
#define GSPI_MAX_BIT_WIDTH           16        // Maximum Bit width
#define SYNC_TIME                    5000      // Delay (ms) to sync master and slave
#define RECEIVE_SYNC_TIME            500       // Delay (ms) to settle the slave after send
/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
/* RX/TX buffers for loopback */
static uint8_t gspi_data_in[GSPI_BUFFER_SIZE];
static uint8_t gspi_data_out[GSPI_BUFFER_SIZE];
/* Used for frame length > 8-bit to compute transfer count */
static uint16_t gspi_division_factor       = 1;
static sl_gspi_handle_t gspi_driver_handle = NULL;

/* State machine: transfer, send-only, receive-only, or completed */
typedef enum {
  SL_GSPI_TRANSFER_DATA,
  SL_GSPI_RECEIVE_DATA,
  SL_GSPI_SEND_DATA,
  SL_GSPI_TRANSMISSION_COMPLETED,
} gspi_mode_enum_t;
static gspi_mode_enum_t current_mode = SL_GSPI_TRANSFER_DATA;

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static sl_status_t gspi_app_compare_data(void);
static void gspi_event_callback_handler(uint32_t event);
/* Set true when starting a new transfer/send/receive */
static boolean_t begin_transmission = true;
static void gspi_task(void *argument);
static sl_status_t gspi_init_function(void);
/* Signalled from callback when GSPI transfer completes */
static osSemaphoreId_t gspi_transfer_sem;
static const osThreadAttr_t gspi_thread_attributes = {
  .name       = "gspi_task",
  .stack_size = 2048,
  .priority   = osPriorityLow1,
};

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * @brief  Entry point: creates the GSPI FreeRTOS task.
 * @return None
 ******************************************************************************/
void gspi_example_init(void)
{
  /* Task runs init, then state-machine loop until all enabled GSPI phases complete */
  osThreadId_t thread_id = osThreadNew((osThreadFunc_t)gspi_task, NULL, &gspi_thread_attributes);
  if (thread_id == NULL) {
    /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
    SL_PRINT_STRING_ERROR("Failed to create gspi thread\r\n");
    return;
  }
}

/*******************************************************************************
 * Function to compare the loop back data, i.e., after transfer it will compare
 * the send and receive data
 *
 * @param none
 * @return SL_STATUS_OK on success, SL_STATUS_FAIL on mismatch
 ******************************************************************************/
static sl_status_t gspi_app_compare_data(void)
{
  // If the data width is not standard (8-bit) then the data should be masked.
  // The extra bits of the integer should be always zero.
  // For example, if bit width is 7, then from 8-15 all bits should be zero in a 16 bit integer.
  // So mask has value according to the data width and it is applied to the data.
  uint16_t data_index   = 0;
  uint32_t frame_length = 0;
  uint16_t mask         = (uint16_t)~0;
  frame_length          = sl_si91x_gspi_get_frame_length();
  /* Build mask so only valid frame bits are compared (e.g. 7-bit or 16-bit) */
  mask = mask >> (GSPI_MAX_BIT_WIDTH - frame_length);
  for (data_index = 0; data_index < GSPI_BUFFER_SIZE; data_index++) {
    if ((gspi_data_in[data_index] & mask) != (gspi_data_out[data_index] & mask)) {
      return SL_STATUS_FAIL;
    }
  }
  SL_PRINT_STRING_ERROR("Data comparison successful, Loop Back Test Passed\r\n");
  return SL_STATUS_OK;
}

/*******************************************************************************
 * @brief  GSPI event callback: releases semaphore on transfer complete so task
 *         can continue (used for sync between ISR and gspi_task).
 * @param  event  GSPI event (SL_GSPI_TRANSFER_COMPLETE, DATA_LOST, MODE_FAULT)
 * @return None
 ******************************************************************************/
static void gspi_event_callback_handler(uint32_t event)
{
  switch (event) {
    case SL_GSPI_TRANSFER_COMPLETE:
      /* Unblock gspi_task which is waiting in osSemaphoreAcquire */
      if (gspi_transfer_sem != NULL) {
        (void)osSemaphoreRelease(gspi_transfer_sem);
      }
      break;
    case SL_GSPI_DATA_LOST:
      /* Optional: log or handle data loss */
      break;
    case SL_GSPI_MODE_FAULT:
      /* Optional: log or handle mode fault */
      break;
  }
}
/*******************************************************************************
 * @brief  One-time GSPI init: driver init, config, callback, sync delay.
 *         Sets current_mode from header macros (SL_USE_TRANSFER/SEND/RECEIVE).
 * @return SL_STATUS_OK on success, error code on failure
 ******************************************************************************/
static sl_status_t gspi_init_function(void)
{
  sl_status_t status = 0;
  sl_gspi_version_t version;
  sl_gspi_status_t gspi_status;
  sl_gspi_control_config_t config;
  /* Fill control config used by set_configuration */
  config.bit_width         = GSPI_BIT_WIDTH;
  config.bitrate           = GSPI_BITRATE;
  config.clock_mode        = SL_GSPI_MODE_0;
  config.slave_select_mode = SL_GSPI_MASTER_HW_OUTPUT;
  config.swap_read         = GSPI_SWAP_READ_DATA;
  config.swap_write        = GSPI_SWAP_WRITE_DATA;

  // Filling the data out array with integer values
  for (uint16_t i = 0; i < GSPI_BUFFER_SIZE; i++) {
    gspi_data_out[i] = (uint8_t)(i + 1);
  }

  // Version information of GSPI driver
  version = sl_si91x_gspi_get_version();
  SL_PRINT_STRING_ERROR("GSPI version is fetched successfully\r\n");
  SL_PRINT_STRING_ERROR("API version is %d.%d.%d\r\n", version.release, version.major, version.minor);

  status = sl_si91x_gspi_init(SL_GSPI_MASTER, &gspi_driver_handle);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_gspi_init: Error Code : %lu\r\n", status);
    return status;
  }
  SL_PRINT_STRING_ERROR("GSPI initialization is successful\r\n");

  // Fetching the status of GSPI i.e., busy, data lost and mode fault
  gspi_status = sl_si91x_gspi_get_status(gspi_driver_handle);
  SL_PRINT_STRING_ERROR("GSPI status is fetched successfully\r\n");
  SL_PRINT_STRING_ERROR("Busy: %d\r\n", gspi_status.busy);
  SL_PRINT_STRING_ERROR("Data_Lost: %d\r\n", gspi_status.data_lost);
  SL_PRINT_STRING_ERROR("Mode_Fault: %d\r\n", gspi_status.mode_fault);

  status = sl_si91x_gspi_set_configuration(gspi_driver_handle, &config);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_gspi_control: Error Code : %lu\r\n", status);
    return status;
  }
  SL_PRINT_STRING_ERROR("GSPI configuration is successful\r\n");

  // Register user callback function
  status = sl_si91x_gspi_register_event_callback(gspi_driver_handle, gspi_event_callback_handler);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_gspi_register_event_callback: Error Code : %lu\r\n", status);
    return status;
  }
  SL_PRINT_STRING_ERROR("GSPI user event callback registered successfully\r\n");

  // Fetching and printing the current clock division factor
  SL_PRINT_STRING_ERROR("Current Clock division factor is %lu\r\n",
                        sl_si91x_gspi_get_clock_division_factor(gspi_driver_handle));
  // Fetching and printing the current frame length
  SL_PRINT_STRING_ERROR("Current Frame Length is %lu\r\n", sl_si91x_gspi_get_frame_length());
  /* For frame length > 8 bits, transfer count is in 16-bit units */
  if (sl_si91x_gspi_get_frame_length() > GSPI_BIT_WIDTH) {
    gspi_division_factor = sizeof(uint16_t);
  }

  /* Allow slave to be ready before first transfer */
  osDelay(SYNC_TIME);

  /* Set initial state from gspi_freertos.h macros */
  if (SL_USE_TRANSFER) {
    current_mode = SL_GSPI_TRANSFER_DATA;
  } else if (SL_USE_SEND) {
    current_mode = SL_GSPI_SEND_DATA;
  } else {
    current_mode = SL_GSPI_RECEIVE_DATA;
  }

  return SL_STATUS_OK;
}
/*******************************************************************************
 * @brief  GSPI FreeRTOS task: runs init, then steps the transfer/send/receive
 *         state machine in a loop until all phases enabled by macros complete.
 *         Uses a semaphore to wait for completion from the ISR callback each phase.
 * @param  argument  Unused (NULL)
 * @return None
 ******************************************************************************/
static void gspi_task(void *argument)
{
  (void)argument;

  sl_status_t init_status = gspi_init_function();
  if (init_status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("GSPI initialization failed: Error Code : %lu, exiting task\r\n", init_status);
    osThreadExit();
  }

  /* Create semaphore before starting transfers so the callback can release it */
  gspi_transfer_sem = osSemaphoreNew(1U, 0U, NULL);
  if (gspi_transfer_sem == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create gspi semaphore\r\n");
    osThreadExit();
  }

  /* Loop until current_mode reaches SL_GSPI_TRANSMISSION_COMPLETED (one or more
     iterations when SL_USE_TRANSFER / SL_USE_SEND / SL_USE_RECEIVE chain phases). */
  while (1) {
    sl_status_t status;
    switch (current_mode) {
      /* Full-duplex transfer: TX and RX in one operation */
      case SL_GSPI_TRANSFER_DATA:
        if (begin_transmission == true) {
          sl_si91x_gspi_set_slave_number(GSPI_SLAVE_0);
          status = sl_si91x_gspi_transfer_data(gspi_driver_handle,
                                               gspi_data_out,
                                               gspi_data_in,
                                               sizeof(gspi_data_out) / gspi_division_factor);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_gspi_transfer_data: Error Code : %lu\r\n", status);
            current_mode = SL_GSPI_TRANSMISSION_COMPLETED;
            break;
          }
          SL_PRINT_STRING_ERROR("GSPI transfer begin successfully\r\n");
          begin_transmission = false;
        }
        /* Block until callback releases semaphore on transfer complete */
        if (gspi_transfer_sem != NULL) {
          (void)osSemaphoreAcquire(gspi_transfer_sem, osWaitForever);
        }
        SL_PRINT_STRING_ERROR("GSPI transfer completed successfully\r\n");
        if (gspi_app_compare_data() != SL_STATUS_OK) {
          SL_PRINT_STRING_ERROR("GSPI data comparison failed\r\n");
        }
        /* Next phase from header config */
        if (SL_USE_SEND) {
          current_mode       = SL_GSPI_SEND_DATA;
          begin_transmission = true;
          break;
        }
        if (SL_USE_RECEIVE) {
          current_mode       = SL_GSPI_RECEIVE_DATA;
          begin_transmission = true;
          break;
        }
        current_mode = SL_GSPI_TRANSMISSION_COMPLETED;
        break;

      /* Send-only (no RX data) */
      case SL_GSPI_SEND_DATA:
        if (begin_transmission) {
          sl_si91x_gspi_set_slave_number(GSPI_SLAVE_0);
          status =
            sl_si91x_gspi_send_data(gspi_driver_handle, gspi_data_out, sizeof(gspi_data_out) / gspi_division_factor);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_gspi_send_data: Error Code : %lu\r\n", status);
            current_mode = SL_GSPI_TRANSMISSION_COMPLETED;
            break;
          }
          SL_PRINT_STRING_ERROR("GSPI send begin successfully\r\n");
          begin_transmission = false;
        }
        if (gspi_transfer_sem != NULL) {
          (void)osSemaphoreAcquire(gspi_transfer_sem, osWaitForever);
        }
        if (SL_USE_RECEIVE) {
          current_mode       = SL_GSPI_RECEIVE_DATA;
          begin_transmission = true;
          SL_PRINT_STRING_ERROR("GSPI send completed\r\n");
          break;
        }
        SL_PRINT_STRING_ERROR("GSPI send completed\r\n");
        current_mode = SL_GSPI_TRANSMISSION_COMPLETED;
        break;

      /* Receive-only (TX dummy); sync delay before starting RX */
      case SL_GSPI_RECEIVE_DATA:
        if (begin_transmission == true) {
          osDelay(RECEIVE_SYNC_TIME);
          sl_si91x_gspi_set_slave_number(GSPI_SLAVE_0);
          status =
            sl_si91x_gspi_receive_data(gspi_driver_handle, gspi_data_in, sizeof(gspi_data_in) / gspi_division_factor);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_gspi_receive_data: Error Code : %lu\r\n", status);
            current_mode = SL_GSPI_TRANSMISSION_COMPLETED;
            break;
          }
          SL_PRINT_STRING_ERROR("GSPI receive begin successfully\r\n");
          begin_transmission = false;
        }
        if (gspi_transfer_sem != NULL) {
          (void)osSemaphoreAcquire(gspi_transfer_sem, osWaitForever);
        }
        SL_PRINT_STRING_ERROR("GSPI receive completed successfully\r\n");
        if (gspi_app_compare_data() != SL_STATUS_OK) {
          SL_PRINT_STRING_ERROR("GSPI data comparison failed\r\n");
        }
        current_mode = SL_GSPI_TRANSMISSION_COMPLETED;
        break;

      /* Idle state; exit task after all phases complete */
      case SL_GSPI_TRANSMISSION_COMPLETED:
        osThreadExit();
        break;
    }
  }
}
