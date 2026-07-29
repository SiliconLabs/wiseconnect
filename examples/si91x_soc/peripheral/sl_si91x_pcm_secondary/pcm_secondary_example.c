/***************************************************************************/ /**
 * @file pcm_example.c
 * @brief pcm examples functions
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
#include "sl_si91x_pcm.h"
#include "sl_si91x_pcm_config.h"
#include "pcm_secondary_example.h"
#include "rsi_debug.h"
#include "sl_si91x_driver_gpio.h"
#include "sl_si91x_clock_manager.h"
/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define PCM_SECONDARY_BUFFER_SIZE 1024                   // Transmit/Receive buffer size
#define PCM_INSTANCE              SL_SI91X_PCM0_INSTANCE // PCM instance
#define FRAME_SIZE_ALIGNMENT      4                      // Max frame offset
#define FRAME_OFFSET              1

// PCM instance mapping
#if PCM_INSTANCE == PCM0
#define PCM_INSTANCE_CONFIG_(config) SL_PCM0##_##config
#else
#define PCM_INSTANCE_CONFIG_(config) SL_ULP_PCM##_##config
#endif

#define PRIMARY_SECONDARY_SYNC_PIN RTE_UULP_GPIO_2_PIN
/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
typedef uint16_t pcm_data_size_t;
pcm_data_size_t pcm_secondary_data_in[PCM_SECONDARY_BUFFER_SIZE + FRAME_SIZE_ALIGNMENT];
pcm_data_size_t pcm_secondary_data_out[PCM_SECONDARY_BUFFER_SIZE];
static sl_i2s_handle_t pcm_handle             = NULL;
static uint8_t pcm_secondary_send_complete    = 0;
static uint8_t pcm_secondary_receive_complete = 0;
typedef enum { SEND_DATA, RECEIVE_DATA, WAIT_STATE, INVALID_STATE } transfer_state_t;
static uint8_t pcm_resolution;
static int pcm_sampling_frequency;
static uint16_t mode;
/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/

static void callback_event(uint32_t event);
static void compare_loop_back_data(void);
static void remove_pcm_frame_offset(pcm_data_size_t data_buffer[PCM_SECONDARY_BUFFER_SIZE + FRAME_OFFSET]);
static sl_status_t secondary_sync_wait(bool first_sync);
/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * PCM secondary device initialization function
 ******************************************************************************/
void pcm_secondary_example_init(void)
{
  sl_status_t status;
  pcm_sampling_frequency = PCM_INSTANCE_CONFIG_(SAMPLING_RATE);
  pcm_resolution         = PCM_INSTANCE_CONFIG_(RESOLUTION);
  mode                   = PCM_INSTANCE_CONFIG_(MODE);

  // Filling the data out array with integer values
  for (uint16_t i = 0; i < PCM_SECONDARY_BUFFER_SIZE; i++) {
    pcm_secondary_data_out[i] = i;
  }
  do {

    //Initialize PCM peripheral and store driver handle in i2s_driver_handle
    status = sl_si91x_pcm_init(PCM_INSTANCE, &pcm_handle);
    if (status != SL_STATUS_OK) {
      /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
      SL_PRINT_STRING_ERROR("PCM Initialization fail\r\n");
      break;
    }
    SL_PRINT_STRING_ERROR("PCM Initialization success\r\n");
    //Register user callback handler
    status = sl_si91x_pcm_register_event_callback(pcm_handle, callback_event);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("PCM user callback register fail\r\n");
      break;
    } else {
      SL_PRINT_STRING_ERROR("PCM user callback register success\r\n");
    }

    status = sl_si91x_pcm_set_configuration(pcm_handle, pcm_sampling_frequency, pcm_resolution, mode);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("PCM configuration set fail\r\n");
      break;
    } else {
      SL_PRINT_STRING_ERROR("PCM configuration set success\r\n");
    }

  } while (false);

  // Synchronize secondary device with primary device using button press detection
  // Secondary waits for button press on primary before starting PCM operations
  status = secondary_sync_wait(true);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Secondary device synchronization failed with error code: 0x%lx\r\n", status);
  } else {
    SL_PRINT_STRING_ERROR("Secondary device synchronized successfully with primary device\r\n");
  }
}
/*******************************************************************************
 * Function will run continuously in while loop
 ******************************************************************************/
void pcm_secondary_example_process_action(void)
{
  sl_status_t status;
  static transfer_state_t state = SEND_DATA;
  switch (state) {
    case SEND_DATA:
      do {
        //Configure PCM transmit DMA channel
        if (sl_si91x_pcm_transmit_data(pcm_handle, pcm_secondary_data_out, PCM_SECONDARY_BUFFER_SIZE)) {
          SL_PRINT_STRING_ERROR("PCM transmit start fail\r\n");
          state = INVALID_STATE;
          break;
        } else {
          SL_PRINT_STRING_ERROR("PCM transmit start success\r\n");
        }
      } while (false);
      state = WAIT_STATE;
      break;

    case RECEIVE_DATA:
      do {
        // synchronize secondary receive with primary transmission.
        status = secondary_sync_wait(false);
        if (status != SL_STATUS_OK) {
          SL_PRINT_STRING_ERROR("Secondary device synchronization failed with error code: 0x%lx\r\n", status);
        }
        if (sl_si91x_pcm_receive_data(pcm_handle,
                                      pcm_secondary_data_in,
                                      (PCM_SECONDARY_BUFFER_SIZE + FRAME_SIZE_ALIGNMENT))) {
          SL_PRINT_STRING_ERROR("PCM receive start fail\r\n");
          state = INVALID_STATE;
          break;
        } else {
          SL_PRINT_STRING_ERROR("PCM receive start success\r\n");
        }
      } while (false);
      state = WAIT_STATE;
      break;

    case WAIT_STATE:
      if (pcm_secondary_send_complete) {
        pcm_secondary_send_complete = 0;
        state                       = RECEIVE_DATA;
      }
      if (pcm_secondary_receive_complete) {
        compare_loop_back_data();
        pcm_secondary_receive_complete = 0;
      }
      break;

    default:
      break;
  }
}

/*******************************************************************************
 * Function to compare the loop back data, i.e., after transfer it will compare
 * the send and receive data
 *
 * @param none
 * @return none
 ******************************************************************************/
static void compare_loop_back_data(void)
{
  uint16_t data_index = 0;

  //Remove the frame-sync offset
  remove_pcm_frame_offset(pcm_secondary_data_in);

  for (data_index = 0; data_index < PCM_SECONDARY_BUFFER_SIZE; data_index++) {
    // If the data in and data out are same, it will be continued else, the for
    // loop will be break.
    if (pcm_secondary_data_in[data_index] != pcm_secondary_data_out[data_index]) {

      break;
    }
  }

  if (data_index == PCM_SECONDARY_BUFFER_SIZE) {
    SL_PRINT_STRING_ERROR("Data comparison successful\r\n");
  } else {
    SL_PRINT_STRING_ERROR("Data comparison failed\r\n");
  }
}

/*******************************************************************************
 * Callback event function
 * It is responsible for the event which are triggered by GSPI interface
 * It updates the respective member of the structure as the event is triggered.
 * @param event - interrupt trigger event
 * @return none
 ******************************************************************************/
static void callback_event(uint32_t event)
{
  switch (event) {
    case SL_I2S_SEND_COMPLETE:
      pcm_secondary_send_complete = 1;
      break;
    case SL_I2S_RECEIVE_COMPLETE:
      pcm_secondary_receive_complete = 1;
      break;
    case SL_I2S_TX_UNDERFLOW:
      break;
    case SL_I2S_RX_OVERFLOW:
      break;
    case SL_I2S_FRAME_ERROR:
      break;
  }
}

/*******************************************************************************
 * Function to remove the frame-sync offset
 *
 * @param data_buffer - input receive buffer to remove data offset.
 * @return none
 ******************************************************************************/
static void remove_pcm_frame_offset(pcm_data_size_t data_buffer[PCM_SECONDARY_BUFFER_SIZE + FRAME_OFFSET])
{
  for (int i = 0; i < PCM_SECONDARY_BUFFER_SIZE; i++) {
    data_buffer[i] = data_buffer[i + FRAME_OFFSET];
  }
}

/*******************************************************************************
 * Synchronizes secondary device with primary device using UULP NPSS GPIO 2 pin (button 0).
 * 
 * This function configures GPIO pin as input to detect button press on the primary
 * device for synchronization. The secondary device waits until button 0 is pressed
 * on the primary device before starting PCM data transmission.
 *
 * @note This function should be called after PCM initialization and before starting
 *       data transmission to ensure proper synchronization between devices.
 * 
 * @return sl_status_t Status code indicating success or failure:
 *         - SL_STATUS_OK: Synchronization completed successfully
 *         - Error code: GPIO configuration or operation failed
 ******************************************************************************/
static sl_status_t secondary_sync_wait(bool first_sync)
{
  uint8_t pin_value  = 0;
  sl_status_t status = SL_STATUS_OK;
  if (first_sync) {
    // Enable GPIO ULP clock for UULP NPSS GPIO operation
    status = sl_si91x_gpio_driver_enable_clock((sl_si91x_gpio_select_clock_t)ULPCLK_GPIO);
    if (status != SL_STATUS_OK) {
      return status;
    }

    // Enable receiver for UULP NPSS GPIO pin to read input state
    status = sl_si91x_gpio_driver_select_uulp_npss_receiver(PRIMARY_SECONDARY_SYNC_PIN, GPIO_RECEIVER_EN);
    if (status != SL_STATUS_OK) {
      return status;
    }

    // Configure pin mux mode for UULP NPSS GPIO functionality
    status = sl_si91x_gpio_driver_set_uulp_npss_pin_mux(PRIMARY_SECONDARY_SYNC_PIN, NPSS_GPIO_PIN_MUX_MODE0);
    if (status != SL_STATUS_OK) {
      return status;
    }

    // Set GPIO pin direction as input to detect button press from primary device
    status =
      sl_si91x_gpio_driver_set_uulp_npss_direction(PRIMARY_SECONDARY_SYNC_PIN, (sl_si91x_gpio_direction_t)GPIO_INPUT);
    if (status != SL_STATUS_OK) {
      return status;
    }
  }
  // Wait for button 0 press on primary device to synchronize
  SL_PRINT_STRING_ERROR("Waiting for primary button 0 press to sync with primary.\r\n");

  while (1) {
    // Read the GPIO pin state
    pin_value = sl_si91x_gpio_driver_get_uulp_npss_pin(PRIMARY_SECONDARY_SYNC_PIN);

    // Check if button is pressed on primary device (active low)
    if (pin_value == 0) {
      SL_PRINT_STRING_ERROR("Button press detected, synchronization completed\r\n");
      sl_si91x_delay_ms(10);
      break;
    }
    sl_si91x_delay_ms(10);
  }
  while (pin_value == 0) {
    pin_value = sl_si91x_gpio_driver_get_uulp_npss_pin(PRIMARY_SECONDARY_SYNC_PIN);
    sl_si91x_delay_ms(10);
    ;
  }
  // Small delay to avoid excessive GPIO polling
  sl_si91x_delay_ms(10);

  return status;
}
