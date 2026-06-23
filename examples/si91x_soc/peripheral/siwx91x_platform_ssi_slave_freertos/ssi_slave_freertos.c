/***************************************************************************/ /**
* @file  ssi_slave_freertos.c
* @brief SSI Slave example.
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
// Include Files

#include "sl_si91x_ssi.h"
#include "rsi_debug.h"
#include "rsi_rom_clks.h"
#include "ssi_slave_freertos.h"
#include "sl_si91x_clock_manager.h"
#include "cmsis_os2.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define SSI_SLAVE_BUFFER_SIZE      1024     // Length of data to be sent through SPI
#define SSI_SLAVE_INTF_PLL_REF_CLK 40000000 // PLL Ref Clock frequency
#define SSI_SLAVE_BIT_WIDTH        8        // SSI bit width
#define SSI_SLAVE_BAUDRATE         10000000 // SSI baudrate
#define SSI_SLAVE_MAX_BIT_WIDTH    16       // Maximum Bit width
#define SYNC_TIME                  5000     // Delay to sync master and slave (ms)
/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void ssi_slave_callback_event_handler(uint32_t event);
static sl_status_t ssi_app_compare_data(void);
static sl_status_t ssi_slave_init_function(void);
static void ssi_slave_task(void *argument);

/*******************************************************************************
 **********************  Local variables   *************************************
 ******************************************************************************/
static uint8_t ssi_slave_tx_buffer[SSI_SLAVE_BUFFER_SIZE] = { '\0' };
static uint8_t ssi_slave_rx_buffer[SSI_SLAVE_BUFFER_SIZE] = { '\0' };
static sl_ssi_handle_t ssi_driver_handle                  = NULL;
static uint16_t size_factor                               = 1;
static uint32_t ssi_slave_number                          = SSI_SLAVE_0;
static osSemaphoreId_t ssi_transfer_sem;
static const osThreadAttr_t ssi_thread_attributes = {
  .name       = "ssi_slave",
  .stack_size = 2048,
  .priority   = osPriorityLow1,
};

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * @fn         ssi_slave_example_init()
 * @brief      SSI slave example entry point. Creates the SSI slave FreeRTOS task
 *             which initializes the peripheral and runs transfer/receive/send
 *             flows based on header macros.
 * @return     None
 ******************************************************************************/
void ssi_slave_example_init(void)
{
  osThreadId_t thread_id = osThreadNew((osThreadFunc_t)ssi_slave_task, NULL, &ssi_thread_attributes);
  if (thread_id == NULL) {
    /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */

    SL_PRINT_STRING_ERROR("Failed to create SSI slave thread\r\n");
    return;
  }
}
/*******************************************************************************
 * @fn         ssi_slave_init_function()
 * @brief      SSI slave peripheral initialization.
 *             Configures clock, driver, configuration, semaphore, and callback.
 * @return     sl_status_t  SL_STATUS_OK on success;
 *                          SL_STATUS_ALLOCATION_FAILED on semaphore failure;
 *                          sl_status code on other failures.
 ******************************************************************************/
static sl_status_t ssi_slave_init_function(void)
{
  uint16_t i            = 0;
  sl_status_t sl_status = 0;
  sl_ssi_version_t ssi_version;

  // Configuring the user configuration structure (zero-init to avoid garbage in unused fields)
  sl_ssi_control_config_t ssi_slave_config = { 0 };
  ssi_slave_config.bit_width               = SSI_SLAVE_BIT_WIDTH;
  ssi_slave_config.device_mode             = SL_SSI_SLAVE_ACTIVE;
  ssi_slave_config.clock_mode              = SL_SSI_PERIPHERAL_CPOL0_CPHA0;
  ssi_slave_config.baud_rate               = SSI_SLAVE_BAUDRATE;

  // Fill data into input buffer for loopback test
  for (i = 0; i < SSI_SLAVE_BUFFER_SIZE; i++) {
    ssi_slave_tx_buffer[i] = (uint8_t)(i + 1);
  }

  // Set the clock for the SSI interface based on configuration
  SL_PRINT_STRING_ERROR("Configuring clock for SSI interface...\r\n");
  SL_PRINT_STRING_ERROR("Setting SSI interface clock: ssi_clk <= 4 * (maximum sclk_in "
                        "frequency)\r\n");
  sl_status = sl_si91x_clock_manager_set_pll_freq(INTF_PLL, SSI_SLAVE_BAUDRATE * 4, SSI_SLAVE_INTF_PLL_REF_CLK);
  if (sl_status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Failed to set SSI interface clock, Error Code : %lu\r\n", sl_status);
    return sl_status;
  }
  SL_PRINT_STRING_ERROR("SSI interface clock set successfully\r\n");

  // Version information of SSI driver
  ssi_version = sl_si91x_ssi_get_version();
  SL_PRINT_STRING_ERROR("SSI version is fetched successfully\r\n");
  SL_PRINT_STRING_ERROR("API version is %d.%d.%d\r\n", ssi_version.release, ssi_version.major, ssi_version.minor);

  // Initialize the SSI driver
  sl_status = sl_si91x_ssi_init(ssi_slave_config.device_mode, &ssi_driver_handle);
  if (sl_status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("SSI Initialization Failed, Error Code : %lu\r\n", sl_status);
    return sl_status;
  }
  SL_PRINT_STRING_ERROR("SSI Initialization Success\r\n");

  // Configure the SSI to Slave mode
  sl_status = sl_si91x_ssi_set_configuration(ssi_driver_handle, &ssi_slave_config, ssi_slave_number);
  if (sl_status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Failed to Set Configuration Parameters to SSI, Error Code : %lu\r\n", sl_status);
    return sl_status;
  }
  SL_PRINT_STRING_ERROR("Set Configuration Parameters to SSI\r\n");

  // Create semaphore before registering callback
  ssi_transfer_sem = osSemaphoreNew(1U, 0U, NULL);
  if (ssi_transfer_sem == NULL) {
    SL_PRINT_STRING_ERROR("SSI semaphore create failed\r\n");
    return SL_STATUS_ALLOCATION_FAILED;
  }

  // Register the user callback
  sl_status = sl_si91x_ssi_register_event_callback(ssi_driver_handle, ssi_slave_callback_event_handler);
  if (sl_status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("SSI register event callback Failed, Error Code : %lu\r\n", sl_status);
    return sl_status;
  }
  SL_PRINT_STRING_ERROR("SSI register event callback Success\r\n");

  // Fetching and printing the current clock division factor and frame length
  SL_PRINT_STRING_ERROR("Current Clock division factor is %lu\r\n",
                        sl_si91x_ssi_get_clock_division_factor(ssi_driver_handle));
  SL_PRINT_STRING_ERROR("Current Frame Length is %lu\r\n", sl_si91x_ssi_get_frame_length(ssi_driver_handle));

  // Set size_factor to 2 for 16-bit frame length (each element is 2 bytes)
  if (sl_si91x_ssi_get_frame_length(ssi_driver_handle) > SSI_SLAVE_BIT_WIDTH) {
    size_factor = sizeof(uint16_t);
  }

  return SL_STATUS_OK;
}
/*******************************************************************************
 * @fn         ssi_slave_task()
 * @brief      SSI slave task (single pass). Runs transfer / receive / send per
 *             macros, then exits.
 * @param[in]  argument  Task argument (unused)
 * @return     None
 ******************************************************************************/
static void ssi_slave_task(void *argument)
{
  (void)argument;
  sl_status_t status = 0;

  status = ssi_slave_init_function();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("SSI initialization failed, Error Code : %lu, exiting task\r\n", status);
    osThreadExit();
  }

  // Sync master and slave before starting transfers
  osDelay(SYNC_TIME);

  /* Transfer phase (full-duplex): TX and RX simultaneously */
  if (SSI_SLAVE_TRANSFER) {
    status = sl_si91x_ssi_transfer_data(ssi_driver_handle,
                                        ssi_slave_tx_buffer,
                                        ssi_slave_rx_buffer,
                                        sizeof(ssi_slave_tx_buffer) / size_factor);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_ssi_transfer_data: Error Code : %lu\r\n", status);
      osThreadExit();
    }
    SL_PRINT_STRING_ERROR("SSI transfer begin successfully\r\n");
    (void)osSemaphoreAcquire(ssi_transfer_sem, osWaitForever);
    SL_PRINT_STRING_ERROR("SSI transfer completed successfully\r\n");
    if (ssi_app_compare_data() != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("SSI slave data comparison failed\r\n");
    }
  }

  /* Receive phase (half-duplex): slave receives only */
  if (SSI_SLAVE_RECEIVE) {
    status =
      sl_si91x_ssi_receive_data(ssi_driver_handle, ssi_slave_rx_buffer, sizeof(ssi_slave_rx_buffer) / size_factor);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_ssi_receive_data: Error Code : %lu\r\n", status);
      osThreadExit();
    }
    SL_PRINT_STRING_ERROR("SSI receive begin successfully\r\n");
    (void)osSemaphoreAcquire(ssi_transfer_sem, osWaitForever);
    SL_PRINT_STRING_ERROR("SSI receive completed\r\n");
  }

  /* Send phase (half-duplex): slave sends only */
  if (SSI_SLAVE_SEND) {
    status = sl_si91x_ssi_send_data(ssi_driver_handle, ssi_slave_tx_buffer, sizeof(ssi_slave_tx_buffer) / size_factor);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_ssi_send_data: Error Code : %lu\r\n", status);
      osThreadExit();
    }
    SL_PRINT_STRING_ERROR("SSI send begin successfully\r\n");
    (void)osSemaphoreAcquire(ssi_transfer_sem, osWaitForever);
    SL_PRINT_STRING_ERROR("SSI send completed\r\n");
    if (ssi_app_compare_data() != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("SSI slave data comparison failed\r\n");
    }
  }

  osThreadExit();
}

/*******************************************************************************
 * @fn         ssi_app_compare_data()
 * @brief      Validates input and output buffers and determines whether the
 *             loopback test case passed or failed.
 * @return     sl_status_t  SL_STATUS_OK if all data matches;
 *                          SL_STATUS_FAIL if mismatch found.
 ******************************************************************************/
static sl_status_t ssi_app_compare_data(void)
{
  uint16_t ssi_data_index;
  uint32_t ssi_frame_length = 0;
  uint16_t ssi_mask         = (uint16_t)~0;

  ssi_frame_length = sl_si91x_ssi_get_frame_length(ssi_driver_handle);
  ssi_mask         = ssi_mask >> (SSI_SLAVE_MAX_BIT_WIDTH - ssi_frame_length);

  for (ssi_data_index = 0; ssi_data_index < SSI_SLAVE_BUFFER_SIZE; ssi_data_index++) {
    if ((ssi_slave_tx_buffer[ssi_data_index] & (uint8_t)ssi_mask)
        != (ssi_slave_rx_buffer[ssi_data_index] & (uint8_t)ssi_mask)) {
      return SL_STATUS_FAIL;
    }
  }

  SL_PRINT_STRING_ERROR("Data comparison successful, Test case Passed\r\n");
  return SL_STATUS_OK;
}

/*******************************************************************************
 * @fn         ssi_slave_callback_event_handler()
 * @brief      SSI callback handler. Handles transfer complete, data lost,
 *             and mode fault events. Releases semaphore on transfer complete.
 * @param[in]  event  SSI transmit and receive events
 * @return     None
 ******************************************************************************/
static void ssi_slave_callback_event_handler(uint32_t event)
{
  switch (event) {
    case SSI_EVENT_TRANSFER_COMPLETE:
      // Signal task that transfer completed
      if (ssi_transfer_sem != NULL) {
        (void)osSemaphoreRelease(ssi_transfer_sem);
      }
      break;

    case SSI_EVENT_DATA_LOST:
      break;

    case SSI_EVENT_MODE_FAULT:
      break;

    default:
      break;
  }
}
