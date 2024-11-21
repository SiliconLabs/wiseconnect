/***************************************************************************/ /**
* @file i2c_leader_example_non_blocking.c
* @brief I2C Leader Non Blocking example functions
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
#include "i2c_leader_example_non_blocking.h"
#include "rsi_debug.h"
#include "sl_i2c_instances.h"
#include "sl_si91x_i2c.h"
#include "sl_si91x_peripheral_i2c.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define INSTANCE_ZERO 0 // For instance 0
#define INSTANCE_ONE  1 // For instance 1
#define INSTANCE_TWO  2 // For ulp instance (instance 2)
#define I2C_INSTANCE_USED \
  INSTANCE_TWO // Update this macro with i2c instance number used for application, INSTANCE_ZERO for instance 0, INSTANCE_ONE for instance 1 and INSTANCE_TWO for ulp instance (instance 2).
#define FOLLOWER_I2C_ADDR            0x50  // I2C follower address
#define MAX_BUFFER_SIZE_NON_BLOCKING 30000 // Maximum buffer size for RX and TX length when transferring with DMA
#define I2C_BUFFER_SIZE              1024  // Size of data buffer
#define I2C_TX_FIFO_THRESHOLD        0     // FIFO threshold
#define I2C_RX_FIFO_THRESHOLD        0     // FIFO threshold
#define INITIAL_VALUE                0     // Initial value of buffer
#define BUFFER_OFFSET                0x1   // Buffer offset

/*******************************************************************************
 ******************************  Data Types  ***********************************
 ******************************************************************************/
// Enum for different transmission scenarios
typedef enum {
  I2C_SEND_DATA,              // Send mode
  I2C_RECEIVE_DATA,           // Receive mode
  I2C_TRANSMISSION_COMPLETED, // Transmission completed mode
} i2c_action_enum_t;

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
static sl_i2c_instance_t i2c_instance = I2C_INSTANCE_USED;
static uint8_t i2c_read_buffer[I2C_BUFFER_SIZE];
static uint32_t i2c_dma_write_buffer[I2C_BUFFER_SIZE];
static i2c_action_enum_t current_mode           = I2C_SEND_DATA;
static boolean_t i2c_send_data_flag             = false;
static boolean_t i2c_receive_data_flag          = false;
static volatile boolean_t i2c_transfer_complete = false;
static volatile boolean_t i2c_driver_dma_error  = false;
static sl_i2c_dma_config_t p_dma_config;
static sl_i2c_config_t sl_i2c_config;

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void i2c_leader_callback(sl_i2c_instance_t instance, uint32_t status);
static void compare_data(void);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/*******************************************************************************
 * I2C example initialization function
 ******************************************************************************/
void i2c_leader_example_init(void)
{
  sl_i2c_status_t i2c_status;

  // Update structure name as per instance used, to register I2C callback
#if (I2C_INSTANCE_USED == INSTANCE_ZERO)
  sl_i2c_config = sl_i2c_i2c0_config;
#elif (I2C_INSTANCE_USED == INSTANCE_ONE)
  sl_i2c_config = sl_i2c_i2c1_config;
#elif (I2C_INSTANCE_USED == INSTANCE_TWO)
  sl_i2c_config = sl_i2c_i2c2_config;
#endif

  sl_i2c_config.i2c_callback = i2c_leader_callback;
  // Initializing I2C instance (update i2c config-strucure name as per instance used)
  i2c_status = sl_i2c_driver_init(i2c_instance, &sl_i2c_config);
  DEBUGINIT();
  if (i2c_status != SL_I2C_SUCCESS) {
    DEBUGOUT("sl_i2c_driver_init : Invalid Parameters, Error Code : %u \n", i2c_status);
  } else {
    DEBUGOUT("Successfully initialized and configured i2c leader\n");
  }
  // Configuring RX and TX FIFO thresholds
  i2c_status = sl_i2c_driver_configure_fifo_threshold(i2c_instance, I2C_TX_FIFO_THRESHOLD, I2C_RX_FIFO_THRESHOLD);
  if (i2c_status != SL_I2C_SUCCESS) {
    DEBUGOUT("sl_i2c_driver_configure_fifo_threshold : Invalid Parameters, Error Code : %u \n", i2c_status);
  } else {
    DEBUGOUT("Successfully configured i2c TX & RX FIFO thresholds\n");
  }
  // Enabling combined format transfer, by enabling repeated start
  i2c_status = sl_i2c_driver_enable_repeated_start(i2c_instance, true);
  if (i2c_status != SL_I2C_SUCCESS) {
    DEBUGOUT("sl_i2c_driver_enable_repeated_start : Invalid Parameters, Error Code : %u \n", i2c_status);
  } else {
    DEBUGOUT("Successfully enabled repeated start\n");
  }
  // Updating DMA RX and TX channel numbers as per I2C instance
  if (sl_i2c_config.transfer_type == SL_I2C_USING_DMA) {
    if (i2c_instance == SL_I2C0) {
      p_dma_config.dma_tx_channel = SL_I2C0_DMA_TX_CHANNEL;
      p_dma_config.dma_rx_channel = SL_I2C0_DMA_RX_CHANNEL;
    }
    if (i2c_instance == SL_I2C1) {
      p_dma_config.dma_tx_channel = SL_I2C1_DMA_TX_CHANNEL;
      p_dma_config.dma_rx_channel = SL_I2C1_DMA_RX_CHANNEL;
    }
    if (i2c_instance == SL_ULP_I2C) {
      p_dma_config.dma_tx_channel = SL_ULP_I2C_DMA_TX_CHANNEL;
      p_dma_config.dma_rx_channel = SL_ULP_I2C_DMA_RX_CHANNEL;
    }
  }
  // Generating a buffer with values that needs to be sent.
  for (uint32_t loop = INITIAL_VALUE; loop < I2C_BUFFER_SIZE; loop++) {
    i2c_dma_write_buffer[loop] = (uint8_t)(loop + BUFFER_OFFSET);
  }
  i2c_send_data_flag = true;
}

/*******************************************************************************
 * Function will run continuously in while loop
 ******************************************************************************/
void i2c_leader_example_process_action(void)
{
  sl_i2c_status_t i2c_status;
  // In switch case, according to the current mode, the transmission is
  // executed.
  // First leader sends data to follower, using I2C send
  // Then leader receives same data from follower, using I2C receive.
  switch (current_mode) {
    case I2C_SEND_DATA:
      if (i2c_send_data_flag) {
        //  Validation for executing the API only once.
        i2c_status = sl_i2c_driver_send_data_non_blocking(i2c_instance,
                                                          FOLLOWER_I2C_ADDR,
                                                          i2c_dma_write_buffer,
                                                          I2C_BUFFER_SIZE,
                                                          &p_dma_config);
        if (i2c_status != SL_I2C_SUCCESS) {
          DEBUGOUT("sl_i2c_driver_send_data_non_blocking : Invalid Parameters, Error Code : %u \n", i2c_status);
          i2c_send_data_flag = false;
          break;
        }
        i2c_send_data_flag = false;
      }
      // It waits till i2c_transfer_complete is true in callback.
      if (i2c_transfer_complete) {
        i2c_transfer_complete = false;
        i2c_receive_data_flag = true;
        current_mode          = I2C_RECEIVE_DATA;
      }
      if (i2c_driver_dma_error) {
        DEBUGOUT("Data is not sent to Follower \n");
        i2c_driver_dma_error  = false;
        i2c_receive_data_flag = true;
        current_mode          = I2C_RECEIVE_DATA;
      }
      break;

    case I2C_RECEIVE_DATA:
      if (i2c_receive_data_flag) {
        // Disabling repeated start before last cycle of transfer
        i2c_status = sl_i2c_driver_enable_repeated_start(i2c_instance, false);
        if (i2c_status != SL_I2C_SUCCESS) {
          DEBUGOUT("sl_i2c_driver_enable_repeated_start : Invalid Parameters, Error Code : %u \n", i2c_status);
        }
        // Validation for executing the API only once.
        i2c_status = sl_i2c_driver_receive_data_non_blocking(i2c_instance,
                                                             FOLLOWER_I2C_ADDR,
                                                             i2c_read_buffer,
                                                             I2C_BUFFER_SIZE,
                                                             &p_dma_config);
        if (i2c_status != SL_I2C_SUCCESS) {
          DEBUGOUT("sl_i2c_driver_receive_data_non_blocking : Invalid Parameters, Error Code : %u \n", i2c_status);
          i2c_receive_data_flag = false;
          break;
        }
        i2c_receive_data_flag = false;
      }
      // It waits till i2c_transfer_complete is true in callback.
      if (i2c_transfer_complete) {
        i2c_transfer_complete = false;
        current_mode          = I2C_TRANSMISSION_COMPLETED;
        // After the receive is completed, input and output data is compared and
        // output is printed on console.
        compare_data();
      } else if (i2c_driver_dma_error) {
        DEBUGOUT("Data is not received from Follower \n");
        i2c_driver_dma_error = false;
        current_mode         = I2C_TRANSMISSION_COMPLETED;
        break;
      }
    case I2C_TRANSMISSION_COMPLETED:
      // I2C will be Idle in this mode
      break;
    default:
      break;
  }
}

/*******************************************************************************
 * Function to compare the input and output data
 *
 * @param none
 * @return none
 ******************************************************************************/
static void compare_data(void)
{
  uint32_t data_index = 0;
  for (data_index = 0; data_index < I2C_BUFFER_SIZE; data_index++) {
    // If the i2c_read_buffer and buffer are same, it will be continued else, the
    // for loop will be break.
    if (i2c_dma_write_buffer[data_index] != i2c_read_buffer[data_index]) {
      break;
    }
  }
  if (data_index == I2C_BUFFER_SIZE) {
    DEBUGOUT("Leader-Follower read-write Data comparison is successful, Test "
             "Case Passed \n");
  } else {
    DEBUGOUT("Leader-Follower read-write Data comparison is not successful, "
             "Test Case Failed \n");
  }
}

/*******************************************************************************
 Leader callback function
 ******************************************************************************/
void i2c_leader_callback(sl_i2c_instance_t instance, uint32_t status)
{
  // to avoid unused variable warning
  (void)instance;

  switch (status) {
    case SL_I2C_DATA_TRANSFER_COMPLETE:
      i2c_transfer_complete = true;
      break;
    case SL_I2C_DMA_TRANSFER_ERROR:
      i2c_driver_dma_error = true;
      break;
    case SL_I2C_NACK:
      i2c_driver_dma_error = true;
      break;
    default:
      break;
  }
}
