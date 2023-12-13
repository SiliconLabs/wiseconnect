/***************************************************************************/ /**
* @file  ulp_ssi_master_example.c
* @brief Ulp SSI Master example.
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
#include "ulp_ssi_master_example.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define BUFFER_SIZE          1024     // Length of data to be sent through SPI
#define SSI_BIT_WIDTH        8        // SSI bit width
#define SSI_BAUDRATE         10000000 // SSI baudrate
#define MAX_BIT_WIDTH        16       // Maximum Bit width
#define RECEIVE_SAMPLE_DELAY 0        // By default sample delay is 0
#define ULP_BANK_OFFSET      0x800    // ULP Memory bank offset value.
#define TX_BUF_MEMORY        (ULP_SRAM_START_ADDR + (1 * ULP_BANK_OFFSET))
#define RX_BUF_MEMORY        (ULP_SRAM_START_ADDR + (2 * ULP_BANK_OFFSET))

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void callback_event_handler(uint32_t event);
static void compare_loopback_data(void);

/*******************************************************************************
 **********************  Local variables   *************************************
 ******************************************************************************/
static uint8_t data_out[BUFFER_SIZE]     = { '\0' };
static uint8_t data_in[BUFFER_SIZE]      = { '\0' };
static sl_ssi_handle_t ssi_driver_handle = NULL;
boolean_t transfer_complete              = false;
boolean_t begin_transmission             = true;
static uint32_t slave_number             = SSI_SLAVE_0;

/// @brief Enumeration for different transmission scenarios
typedef enum {
  SL_TRANSFER_DATA,
  SL_RECEIVE_DATA,
  SL_SEND_DATA,
  SL_TRANSMISSION_COMPLETED,
} ssi_mode_enum_t;
static ssi_mode_enum_t current_mode = SL_TRANSFER_DATA;

extern void hardware_setup(void);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * @fn         ssi_master_example_init()
 * @brief      Main Application Function
 * @return     None
 ******************************************************************************/
void ssi_master_example_init(void)
{
  /* At this stage the micro-controller clock setting is already configured,
    * this is done through SystemInit() function which is called from startup
    * file (startup_rs1xxxx.s) before to branch to application main.
    * To reconfigure the default setting of SystemInit() function, refer to
    * startup_rs1xxxx.c file
    */

  uint16_t i            = 0;
  sl_status_t sl_status = 0;
  sl_ssi_version_t version;
  // Configuring the user configuration structure
  sl_ssi_control_config_t config;
  config.bit_width            = SSI_BIT_WIDTH;
  config.device_mode          = SL_SSI_ULP_MASTER_ACTIVE;
  config.clock_mode           = SL_SSI_PERIPHERAL_CPOL0_CPHA0;
  config.baud_rate            = SSI_BAUDRATE;
  config.receive_sample_delay = RECEIVE_SAMPLE_DELAY;
  // Filled data into input buffer
  for (i = 0; i < BUFFER_SIZE; i++) {
    data_out[i] = (uint8_t)(i + 1);
  }
  memcpy((uint8_t *)TX_BUF_MEMORY, &data_out, sizeof(data_out[0]) * BUFFER_SIZE);
  do {
    // Version information of SSI driver
    version = sl_si91x_ssi_get_version();
    DEBUGOUT("SSI version is fetched successfully \n");
    DEBUGOUT("API version is %d.%d.%d\n", version.release, version.major, version.minor);
    // Switching MCU from PS4 to PS2 state(low power state)
    // In this mode, whatever be the timer clock source value, it will run with 20MHZ only,
    // as it trims higher clock frequencies to 20MHZ.
    // To use timer in high power mode, don't call hardware_setup()
    hardware_setup();
    DEBUGINIT();
    // Initialize the SSI driver
    sl_status = sl_si91x_ssi_init(config.device_mode, &ssi_driver_handle);
    if (sl_status != SL_STATUS_OK) {
      DEBUGOUT("SSI Initialization Failed, Error Code : %lu \n", sl_status);
      break;
    }
    DEBUGOUT("SSI Initialization Success \n");
    // Configure the SSI to Master, 16-bit mode @10000 kBits/sec
    sl_status = sl_si91x_ssi_set_configuration(ssi_driver_handle, &config, slave_number);
    if (sl_status != SL_STATUS_OK) {
      DEBUGOUT("Failed to Set Configuration Parameters to SSI, Error Code : %lu \n", sl_status);
      break;
    }
    DEBUGOUT("Set Configuration Parameters to SSI \n");
    // Register the user callback
    sl_status = sl_si91x_ssi_register_event_callback(ssi_driver_handle, callback_event_handler);
    if (sl_status != SL_STATUS_OK) {
      DEBUGOUT("SSI register event callback Failed, Error Code : %lu \n", sl_status);
      break;
    }
    DEBUGOUT("SSI register event callback Success \n");
    // Fetching and printing the current clock division factor
    DEBUGOUT("Current Clock division factor is %lu \n", sl_si91x_ssi_get_clock_division_factor(ssi_driver_handle));
    // Fetching and printing the current frame length
    DEBUGOUT("Current Frame Length is %lu \n", sl_si91x_ssi_get_frame_length(ssi_driver_handle));
    // As per the macros enabled in the header file, it will configure the current mode.
    if (SL_USE_TRANSFER) {
      current_mode = SL_TRANSFER_DATA;
      break;
    }
    if (SL_USE_SEND) {
      current_mode = SL_SEND_DATA;
      break;
    }
    current_mode = SL_RECEIVE_DATA;
  } while (false);
}
/*******************************************************************************
 * Implementation of state machine for Transfer data, receive data and
 * send data.
 ******************************************************************************/
void ssi_master_example_process_action(void)
{
  sl_status_t status;
  // In this switch case, according to the macros enabled in header file, it starts to execute the APIs
  // Assuming all the macros are enabled, after transfer, receive will be executed and after receive
  // send will be executed.
  switch (current_mode) {
    case SL_TRANSFER_DATA:
      if (begin_transmission == true) {
        // Validation for executing the API only once
        sl_si91x_ssi_set_slave_number(slave_number);
        status = sl_si91x_ssi_transfer_data(ssi_driver_handle,
                                            (uint8_t *)TX_BUF_MEMORY,
                                            (uint8_t *)RX_BUF_MEMORY,
                                            BUFFER_SIZE);
        if (status != SL_STATUS_OK) {
          // If it fails to execute the API, it will not execute rest of the things
          DEBUGOUT("sl_si91x_ssi_transfer_data: Error Code : %lu \n", status);
          current_mode = SL_TRANSMISSION_COMPLETED;
          break;
        }
        DEBUGOUT("SSI transfer begin successfully \n");
        begin_transmission = false;
      }
      if (transfer_complete) {
        transfer_complete = false;
        memcpy(&data_in, (uint8_t *)RX_BUF_MEMORY, sizeof(data_out[0]) * BUFFER_SIZE);
        DEBUGOUT("SSI transfer completed successfully \n");
        // After comparing the loopback transfer, it compares the data_out and
        // data_in.
        compare_loopback_data();
        if (SL_USE_SEND) {
          // If send macro is enabled, current mode is set to send
          current_mode       = SL_SEND_DATA;
          begin_transmission = true;
          break;
        }
        if (SL_USE_RECEIVE) {
          // If receive macro is enabled, current mode is set to receive
          current_mode       = SL_RECEIVE_DATA;
          begin_transmission = true;
          break;
        }
        // If above macros are not enabled, current mode is set to complete
        current_mode = SL_TRANSMISSION_COMPLETED;
      }
      break;
    case SL_SEND_DATA:
      if (begin_transmission) {
        // Validation for executing the API only once
        sl_si91x_ssi_set_slave_number(slave_number);
        status = sl_si91x_ssi_send_data(ssi_driver_handle, (uint8_t *)TX_BUF_MEMORY, BUFFER_SIZE);
        if (status != SL_STATUS_OK) {
          // If it fails to execute the API, it will not execute rest of the things
          DEBUGOUT("sl_si91x_ssi_send_data: Error Code : %lu \n", status);
          current_mode = SL_TRANSMISSION_COMPLETED;
          break;
        }
        DEBUGOUT("SSI send begin successfully \n");
        begin_transmission = false;
      }
      //Waiting till the send is completed
      if (transfer_complete) {
        // If DMA is enabled, it will wait untill transfer_complete flag is set.
        transfer_complete = false;
        if (SL_USE_RECEIVE) {
          // If send macro is enabled, current mode is set to send
          current_mode       = SL_RECEIVE_DATA;
          begin_transmission = true;
          DEBUGOUT("SSI send completed \n");
          break;
        }
        DEBUGOUT("SSI send completed \n");
        // If send macro is not enabled, current mode is set to completed.
        current_mode = SL_TRANSMISSION_COMPLETED;
      }
      break;
    case SL_RECEIVE_DATA:
      if (begin_transmission == true) {
        // Validation for executing the API only once
        sl_si91x_ssi_set_slave_number(slave_number);
        status = sl_si91x_ssi_receive_data(ssi_driver_handle, (uint8_t *)RX_BUF_MEMORY, BUFFER_SIZE);
        if (status != SL_STATUS_OK) {
          // If it fails to execute the API, it will not execute rest of the things
          DEBUGOUT("sl_si91x_ssi_receive_data: Error Code : %lu \n", status);
          current_mode = SL_TRANSMISSION_COMPLETED;
          break;
        }
        DEBUGOUT("SSI receive begin successfully \n");
        begin_transmission = false;
        //Waiting till the receive is completed
      }
      if (transfer_complete) {
        // If DMA is enabled, it will wait until transfer_complete flag is set.
        memcpy(&data_in, (uint8_t *)RX_BUF_MEMORY, sizeof(data_out[0]) * BUFFER_SIZE);
        transfer_complete = false;
        DEBUGOUT("SSI receive completed \n");
        compare_loopback_data();
        // At last current mode is set to completed.
        current_mode = SL_TRANSMISSION_COMPLETED;
      }
      break;
    case SL_TRANSMISSION_COMPLETED:
      break;
  }
}

/*******************************************************************************
 * @fn         compare_loopback_data()
 * @brief      Internal private Function which validates input and output buffers
 *             and decides whether the test case passed/failed.
 * @return     None
*******************************************************************************/
static void compare_loopback_data(void)
{
  // If the data width is not standard (8-bit) then the data should be masked.
  // The extra bits of the integer should be always zero.
  // For example, if bit width is 7, then from 8-15 all bits should be zero in a 16 bit integer.
  // So mask has value according to the data width and it is applied to the data.
  uint16_t data_index;
  uint8_t frame_length = 0;
  uint16_t mask        = (uint16_t)~0;
  frame_length         = sl_si91x_ssi_get_frame_length(ssi_driver_handle);
  mask                 = mask >> (MAX_BIT_WIDTH - frame_length);
  for (data_index = 0; data_index < BUFFER_SIZE; data_index++) {
    data_in[data_index] &= mask;
    data_out[data_index] &= mask;
    if (data_out[data_index] != data_in[data_index]) {
      break;
    }
  }
  if (data_index == BUFFER_SIZE) {
    DEBUGOUT("Data comparison successful, Loop Back Test Passed \n");
  } else {
    DEBUGOUT("Data comparison failed, Loop Back Test failed \n");
  }
}

/*******************************************************************************
 * @brief  SSI callback handler
 * @param[in]event SSI transmit and receive events
 * @return   None
*******************************************************************************/
static void callback_event_handler(uint32_t event)
{
  switch (event) {
    case SSI_EVENT_TRANSFER_COMPLETE:
      transfer_complete = true;
      break;

    case SSI_EVENT_DATA_LOST:
      // Occurs in slave mode when data is requested/sent by master
      // but send/receive/transfer operation has not been started
      // and indicates that data is lost. Occurs also in master mode
      // when driver cannot transfer data fast enough.
      break;

    case SSI_EVENT_MODE_FAULT:
      // Occurs in master mode when Slave Select is deactivated and
      // indicates Master Mode Fault.
      break;
  }
}
