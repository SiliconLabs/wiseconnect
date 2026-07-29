/***************************************************************************/ /**
* @file  ssi_master_freertos.c
* @brief SSI master FreeRTOS example with GPIO button synchronization.
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

#include <stdbool.h>
#include "sl_si91x_ssi.h"
#include "rsi_debug.h"
#include "rsi_rom_clks.h"
#include "ssi_master_freertos.h"
#include "sl_si91x_clock_manager.h"
#include "sl_si91x_driver_gpio.h"
#include "sl_si91x_ssi_primary_config.h"
#include "cmsis_os2.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define SSI_MASTER_BUFFER_SIZE          1024     // Length of data to be sent through SPI
#define SSI_MASTER_INTF_PLL_REF_CLK     40000000 // PLL Ref Clock frequency
#define SSI_MASTER_BIT_WIDTH            8        // SSI bit width
#define SSI_MASTER_BAUDRATE             10000000 // SSI baudrate
#define SSI_MASTER_MAX_BIT_WIDTH        16       // Maximum Bit width
#define SSI_MASTER_RECEIVE_SAMPLE_DELAY 0        // By default sample delay is 0
#define SSI_MASTER_SYNC_POLL_DELAY_MS   10       // Polling delay for button-based sync
#define SSI_MASTER_SYNC_SETTLE_DELAY_MS 50       // Settling delay after button release
#define PRIMARY_SECONDARY_SYNC_PIN      RTE_UULP_GPIO_2_PIN
/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void ssi_master_callback_event_handler(uint32_t event);
static sl_status_t ssi_app_compare_data(void);
static sl_status_t ssi_master_init_function(void);
static void ssi_master_task(void *argument);
static sl_status_t master_sync_wait(bool first_sync);

/*******************************************************************************
 **********************  Local variables   *************************************
 ******************************************************************************/
static uint8_t ssi_master_tx_buffer[SSI_MASTER_BUFFER_SIZE] = { '\0' };
static uint8_t ssi_master_rx_buffer[SSI_MASTER_BUFFER_SIZE] = { '\0' };
static sl_ssi_handle_t ssi_driver_handle                    = NULL;
static uint16_t size_factor                                 = 1;
static uint32_t ssi_slave_number                            = SSI_SLAVE_0;
static osSemaphoreId_t ssi_transfer_sem;
static const osThreadAttr_t ssi_thread_attributes = {
  .name       = "ssi_master",
  .stack_size = 2048,
  .priority   = osPriorityLow1,
};

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * @fn         ssi_master_example_init()
 * @brief      Main application entry point. Creates and starts the SSI master
 *             FreeRTOS task.
 * @param      None
 * @return     None
 ******************************************************************************/
void ssi_master_example_init(void)
{
  osThreadId_t thread_id = osThreadNew((osThreadFunc_t)ssi_master_task, NULL, &ssi_thread_attributes);
  if (thread_id == NULL) {
    /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
    SL_PRINT_STRING_ERROR("Failed to create SSI master thread\r\n");
    return;
  }
}
/*******************************************************************************
 * @fn         ssi_master_init_function()
 * @brief      SSI master peripheral initialization.
 *             Configures clock, driver, and callback for SSI master mode.
 * @param      None
 * @return     sl_status_t SL_STATUS_OK on success, SL_STATUS_ALLOCATION_FAILED
 *             on semaphore failure, or other sl_status code on failure.
 ******************************************************************************/
static sl_status_t ssi_master_init_function(void)
{
  uint16_t i            = 0;
  sl_status_t sl_status = 0;
  sl_ssi_version_t ssi_version;
  // Configuring the user configuration structure (zero-init to avoid garbage in unused fields)
  sl_ssi_control_config_t ssi_master_config = { 0 };
  ssi_master_config.bit_width               = SSI_MASTER_BIT_WIDTH;
  ssi_master_config.device_mode             = SL_SSI_MASTER_ACTIVE;
  ssi_master_config.clock_mode              = SL_SSI_PERIPHERAL_CPOL0_CPHA0;
  ssi_master_config.baud_rate               = SSI_MASTER_BAUDRATE;
  ssi_master_config.receive_sample_delay    = SSI_MASTER_RECEIVE_SAMPLE_DELAY;
  ssi_master_config.transfer_mode           = SL_SSI_PRIMARY_TRANSFER_MODE;

  // Filled data into input buffer
  for (i = 0; i < SSI_MASTER_BUFFER_SIZE; i++) {
    ssi_master_tx_buffer[i] = (uint8_t)(i + 1);
  }
  // Set the Clock for SSI interface based on configuration
  SL_PRINT_STRING_ERROR("Configuring Clock for SSI interface...\r\n");
  SL_PRINT_STRING_ERROR("Setting SSI interface clock: sclk_out <= ssi_clk/2\r\n");
#if (SSI_PRIMARY_UC == ENABLE)
  sl_status = sl_si91x_clock_manager_set_pll_freq(INTF_PLL, SL_SSI_PRIMARY_BAUD * 2, SSI_MASTER_INTF_PLL_REF_CLK);
#else
  sl_status = sl_si91x_clock_manager_set_pll_freq(INTF_PLL, SSI_MASTER_BAUDRATE * 2, SSI_MASTER_INTF_PLL_REF_CLK);
#endif
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
  sl_status = sl_si91x_ssi_init(ssi_master_config.device_mode, &ssi_driver_handle);
  if (sl_status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("SSI Initialization Failed, Error Code : %lu\r\n", sl_status);
    return sl_status;
  }
  SL_PRINT_STRING_ERROR("SSI Initialization Success\r\n");

  // Configure the SSI to Master, 8-bit mode @10000 kBits/sec
  sl_status = sl_si91x_ssi_set_configuration(ssi_driver_handle, &ssi_master_config, ssi_slave_number);
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
  sl_status = sl_si91x_ssi_register_event_callback(ssi_driver_handle, ssi_master_callback_event_handler);
  if (sl_status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("SSI register event callback Failed, Error Code : %lu\r\n", sl_status);
    return sl_status;
  }
  SL_PRINT_STRING_ERROR("SSI register event callback Success\r\n");
  // Fetching and printing the current clock division factor
  SL_PRINT_STRING_ERROR("Current Clock division factor is %lu\r\n",
                        sl_si91x_ssi_get_clock_division_factor(ssi_driver_handle));
  // Fetching and printing the current frame length
  SL_PRINT_STRING_ERROR("Current Frame Length is %lu\r\n", sl_si91x_ssi_get_frame_length(ssi_driver_handle));
  if (sl_si91x_ssi_get_frame_length(ssi_driver_handle) > SSI_MASTER_BIT_WIDTH) {
    size_factor = sizeof(uint16_t);
  }

  return SL_STATUS_OK;
}
/*******************************************************************************
 * @fn         ssi_master_task()
 * @brief      SSI master task (single pass). Runs transfer / send / receive
 *             sections per header macros, then exits.
 * @param[in]  argument Task argument (unused)
 * @return     None
 ******************************************************************************/
static void ssi_master_task(void *argument)
{
  (void)argument;
  sl_status_t status = 0;

  status = ssi_master_init_function();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("SSI initialization failed, Error Code : %lu, exiting task\r\n", status);
    osThreadExit();
  }

  SL_PRINT_STRING_ERROR("Reset Slave\r\n");
  status = master_sync_wait(true);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("SSI master synchronization failed, Error Code : %lu\r\n", status);
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("SSI master synchronized successfully with slave\r\n");

  /* Transfer phase (full-duplex): simultaneous TX and RX */
  if (SSI_MASTER_TRANSFER) {
    sl_si91x_ssi_set_slave_number((uint8_t)ssi_slave_number);
    status = sl_si91x_ssi_transfer_data(ssi_driver_handle,
                                        ssi_master_tx_buffer,
                                        ssi_master_rx_buffer,
                                        sizeof(ssi_master_tx_buffer) / size_factor);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_ssi_transfer_data: Error Code : %lu\r\n", status);
      osThreadExit();
    }
    SL_PRINT_STRING_ERROR("SSI transfer begin successfully\r\n");
    (void)osSemaphoreAcquire(ssi_transfer_sem, osWaitForever);
    SL_PRINT_STRING_ERROR("SSI transfer completed successfully\r\n");
    if (ssi_app_compare_data() != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("SSI master data comparison failed\r\n");
    }
  }

  /* Send phase (half-duplex): TX only, no RX */
  if (SSI_MASTER_SEND) {
    sl_si91x_ssi_set_slave_number((uint8_t)ssi_slave_number);
    status =
      sl_si91x_ssi_send_data(ssi_driver_handle, ssi_master_tx_buffer, sizeof(ssi_master_tx_buffer) / size_factor);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_ssi_send_data: Error Code : %lu\r\n", status);
      osThreadExit();
    }
    SL_PRINT_STRING_ERROR("SSI send begin successfully\r\n");
    (void)osSemaphoreAcquire(ssi_transfer_sem, osWaitForever);
    SL_PRINT_STRING_ERROR("SSI send completed\r\n");
  }

  /* Receive phase (half-duplex): RX only, with optional button resync */
  if (SSI_MASTER_RECEIVE) {
    if (SSI_MASTER_TRANSFER || SSI_MASTER_SEND) {
      status = master_sync_wait(false);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("SSI master synchronization failed, Error Code : %lu\r\n", status);
        osThreadExit();
      }
    }
    sl_si91x_ssi_set_slave_number((uint8_t)ssi_slave_number);
    status =
      sl_si91x_ssi_receive_data(ssi_driver_handle, ssi_master_rx_buffer, sizeof(ssi_master_rx_buffer) / size_factor);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_ssi_receive_data: Error Code : %lu\r\n", status);
      osThreadExit();
    }
    SL_PRINT_STRING_ERROR("SSI receive begin successfully\r\n");
    (void)osSemaphoreAcquire(ssi_transfer_sem, osWaitForever);
    SL_PRINT_STRING_ERROR("SSI receive completed\r\n");
    if (ssi_app_compare_data() != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("SSI master data comparison failed\r\n");
    }
  }

  osThreadExit();
}

/*******************************************************************************
 * @fn         ssi_app_compare_data()
 * @brief      Internal function that validates input and output buffers and
 *             decides whether the loopback test case passed or failed.
 * @param      None
 * @return     sl_status_t SL_STATUS_OK if all data matches, SL_STATUS_FAIL if
 *             mismatch is found.
 ******************************************************************************/
static sl_status_t ssi_app_compare_data(void)
{
  uint16_t ssi_data_index;
  uint32_t ssi_frame_length = 0;
  uint16_t ssi_mask         = (uint16_t)~0;
  ssi_frame_length          = sl_si91x_ssi_get_frame_length(ssi_driver_handle);
  ssi_mask                  = ssi_mask >> (SSI_MASTER_MAX_BIT_WIDTH - ssi_frame_length);
  for (ssi_data_index = 0; ssi_data_index < SSI_MASTER_BUFFER_SIZE; ssi_data_index++) {
    if ((ssi_master_tx_buffer[ssi_data_index] & (uint8_t)ssi_mask)
        != (ssi_master_rx_buffer[ssi_data_index] & (uint8_t)ssi_mask)) {
      return SL_STATUS_FAIL;
    }
  }
  SL_PRINT_STRING_ERROR("Data comparison successful, Loop Back Test Passed\r\n");
  return SL_STATUS_OK;
}

/*******************************************************************************
 * @fn         ssi_master_callback_event_handler()
 * @brief      SSI master callback handler for transfer completion and events.
 * @param[in]  event SSI transmit and receive events (e.g. SSI_EVENT_TRANSFER_COMPLETE)
 * @return     None
 ******************************************************************************/
static void ssi_master_callback_event_handler(uint32_t event)
{
  switch (event) {
    case SSI_EVENT_TRANSFER_COMPLETE:
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

static sl_status_t master_sync_wait(bool first_sync)
{
  uint8_t pin_value  = 0;
  sl_status_t status = SL_STATUS_OK;

  if (first_sync) {
    status = sl_si91x_gpio_driver_enable_clock((sl_si91x_gpio_select_clock_t)ULPCLK_GPIO);
    if (status != SL_STATUS_OK) {
      return status;
    }

    status = sl_si91x_gpio_driver_select_uulp_npss_receiver(PRIMARY_SECONDARY_SYNC_PIN, GPIO_RECEIVER_EN);
    if (status != SL_STATUS_OK) {
      return status;
    }

    status = sl_si91x_gpio_driver_set_uulp_npss_pin_mux(PRIMARY_SECONDARY_SYNC_PIN, NPSS_GPIO_PIN_MUX_MODE0);
    if (status != SL_STATUS_OK) {
      return status;
    }

    status =
      sl_si91x_gpio_driver_set_uulp_npss_direction(PRIMARY_SECONDARY_SYNC_PIN, (sl_si91x_gpio_direction_t)GPIO_INPUT);
    if (status != SL_STATUS_OK) {
      return status;
    }
  }

  SL_PRINT_STRING_ERROR("Press button 0 on master to sync.\r\n");

  for (;;) {
    pin_value = sl_si91x_gpio_driver_get_uulp_npss_pin(PRIMARY_SECONDARY_SYNC_PIN);

    if (pin_value == 0U) {
      SL_PRINT_STRING_ERROR("Button press detected, synchronization completed\r\n");
      sl_si91x_delay_ms(SSI_MASTER_SYNC_POLL_DELAY_MS);
      break;
    }

    sl_si91x_delay_ms(SSI_MASTER_SYNC_POLL_DELAY_MS);
  }

  while (pin_value == 0U) {
    pin_value = sl_si91x_gpio_driver_get_uulp_npss_pin(PRIMARY_SECONDARY_SYNC_PIN);
    sl_si91x_delay_ms(SSI_MASTER_SYNC_POLL_DELAY_MS);
  }

  sl_si91x_delay_ms(SSI_MASTER_SYNC_SETTLE_DELAY_MS);
  return status;
}
