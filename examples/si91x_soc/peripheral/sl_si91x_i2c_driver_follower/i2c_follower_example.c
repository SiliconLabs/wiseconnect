/***************************************************************************/ /**
* @file i2c_follower_example.c
* @brief I2C DRIVER examples functions
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
#include "i2c_follower_example.h"
#include "rsi_debug.h"
#include "sl_i2c_instances.h"
#include "sl_si91x_i2c.h"
#include "sl_si91x_peripheral_i2c.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define NON_BLOCKING_APPLICATION     DISABLE // Enable this macro when transfer type is DMA
#define BLOCKING_APPLICATION         ENABLE  // Enable this macro when transfer type is Interrupt type
#define I2C_INSTANCE_USED            2       // update this macro with i2c instance number used for application
#define OWN_I2C_ADDR                 0x50    // Own I2C address
#define ZERO_ADDRESS_VALUE           0x00    // for zero address value
#define MAX_BUFFER_SIZE_BLOCKING     80000   // Maximum buffer size for RX and TX length when transferring without DMA
#define MAX_BUFFER_SIZE_NON_BLOCKING 30000   // Maximum buffer size for RX and TX length when transferring with DMA
#define I2C_SIZE_BUFFERS             15      // Size of data buffer
#define I2C_RX_LENGTH                I2C_SIZE_BUFFERS // Number of bytes to receive
#define I2C_TX_LENGTH                I2C_SIZE_BUFFERS // Number of bytes to send
#define I2C_OFFSET_LENGTH            1                // Offset length
#define I2C_INTERNAL_PULLUP          1                // Internal Pull-up enable
#define I2C_TX_FIFO_THRESHOLD        0                // FIFO threshold
#define I2C_RX_FIFO_THRESHOLD        0                // FIFO threshold
#define INITIAL_VALUE                0                // for 0 initial value
#define ONE                          0x1              // for value one
#define INSTANCE_ZERO                0                // For 0 value
#define INSTANCE_ONE                 1                // For 0 value
#define INSTANCE_TWO                 2                // For 0 value

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
sl_i2c_status_t i2c_status;
sl_i2c_instance_t i2c_instance = I2C_INSTANCE_USED;
#if NON_BLOCKING_APPLICATION
volatile uint8_t i2c_read_buffer[I2C_SIZE_BUFFERS];
#else
static uint8_t i2c_read_buffer[I2C_SIZE_BUFFERS + I2C_OFFSET_LENGTH];
#endif
#if NON_BLOCKING_APPLICATION
static uint32_t i2c_write_buffer[I2C_SIZE_BUFFERS];
#else
static uint8_t i2c_write_buffer[I2C_SIZE_BUFFERS];
#endif
static i2c_action_enum_t current_mode                             = I2C_RECEIVE_DATA;
boolean_t i2c_send_data_flag                                      = false;
boolean_t i2c_receive_data_flag                                   = false;
boolean_t i2c_transfer_complete                                   = false;
boolean_t i2c_7bit_address_transfer_complete                      = false;
boolean_t i2c_10bit_address_transfer_complete                     = false;
boolean_t i2c_10bit_address_transfer_complete_with_repeated_start = false;
boolean_t i2c_diver_ack                                           = false;
boolean_t i2c_diver_nack                                          = false;
boolean_t i2c_driver_aribitration_lost                            = false;
boolean_t i2c_driver_bus_error                                    = false;
boolean_t i2c_driver_bus_hold                                     = false;
boolean_t i2c_driver_sda_error                                    = false;
boolean_t i2c_driver_sca_error                                    = false;
boolean_t i2c_driver_dma_error                                    = false;
sl_i2c_dma_config_t p_dma_config;

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void i2c_follower_callback(sl_i2c_instance_t i2c_instance, uint32_t status);
static void compare_data(void);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/*******************************************************************************
 * I2C example initialization function
 ******************************************************************************/
void i2c_follower_example_init(void)
{
#if (I2C_INSTANCE_USED == INSTANCE_ZERO)
  // Update structure name as per instance used, to register I2C callback
  sl_i2c_i2c0_config.i2c_callback = i2c_follower_callback;
  // Initializing I2C instance (update i2c config-strucure name as per instance used)
  i2c_status = sl_i2c_driver_init(i2c_instance, &sl_i2c_i2c0_config);
#endif
#if (I2C_INSTANCE_USED == INSTANCE_ONE)
  // Update structure name as per instance used, to register I2C callback
  sl_i2c_i2c1_config.i2c_callback = i2c_follower_callback;
  // Initializing I2C instance (update i2c config-strucure name as per instance used)
  i2c_status = sl_i2c_driver_init(i2c_instance, &sl_i2c_i2c1_config);
#endif
#if (I2C_INSTANCE_USED == INSTANCE_TWO)
  // Update structure name as per instance used, to register I2C callback
  sl_i2c_i2c2_config.i2c_callback = i2c_follower_callback;
  // Initializing I2C instance (update i2c config-strucure name as per instance used)
  i2c_status = sl_i2c_driver_init(i2c_instance, &sl_i2c_i2c2_config);
#endif
  if (i2c_status != SL_I2C_SUCCESS) {
    DEBUGOUT("sl_i2c_driver_init : Invalid Parameters, Error Code : %u \n", i2c_status);
  } else {
    DEBUGOUT("Successfully initialized i2c follower\n");
  }
  // Configuring follower mask address
  i2c_status = sl_i2c_driver_set_follower_address(i2c_instance, OWN_I2C_ADDR);
  if (i2c_status != SL_I2C_SUCCESS) {
    DEBUGOUT("sl_i2c_driver_init : Invalid Parameters, Error Code : %u \n", i2c_status);
  } else {
    DEBUGOUT("Successfully configured i2c follower address\n");
  }
  // Configuring RX and TX FIFO thresholds
  i2c_status = sl_i2c_driver_configure_fifo_threshold(i2c_instance, I2C_TX_FIFO_THRESHOLD, I2C_RX_FIFO_THRESHOLD);
  if (i2c_status != SL_I2C_SUCCESS) {
    DEBUGOUT("sl_i2c_driver_configure_fifo_threshold : Invalid Parameters, Error Code : %u \n", i2c_status);
  } else {
    DEBUGOUT("Successfully configured i2c TX & RX FIFO thresholds\n");
  }
  // Updating DMA RX and TX channel numbers as per I2C instance
#if (NON_BLOCKING_APPLICATION)
  if (i2c_instance == SL_I2C0) {
    p_dma_config.dma_tx_channel = SL_I2C0_DMA_TX_CHANNEL;
    p_dma_config.dma_rx_channel = SL_I2C0_DMA_RX_CHANNEL;
  }
  if (i2c_instance == SL_I2C1) {
    p_dma_config.dma_tx_channel = SL_I2C1_DMA_TX_CHANNEL;
    p_dma_config.dma_rx_channel = SL_I2C1_DMA_RX_CHANNEL;
  }
  if (i2c_instance == SL_I2C2) {
    p_dma_config.dma_tx_channel = SL_I2C2_DMA_TX_CHANNEL;
    p_dma_config.dma_rx_channel = SL_I2C2_DMA_RX_CHANNEL;
  }
#endif
  // Generating a buffer with values that needs to be sent.
  for (uint32_t loop = INITIAL_VALUE; loop < I2C_SIZE_BUFFERS; loop++) {
    i2c_write_buffer[loop] = (uint8_t)(loop + ONE);
    i2c_read_buffer[loop]  = INITIAL_VALUE;
  }
  i2c_receive_data_flag = true;
}

/*******************************************************************************
 *  Follower callback function
 ******************************************************************************/
void i2c_follower_example_process_action(void)
{
  // In switch case, according to the current mode, the transmission is
  // executed.
  // First follower receives data from leader, using I2C receive
  // Then follower send same data back to master, using I2C send.
  switch (current_mode) {
    case I2C_RECEIVE_DATA:
      if (i2c_receive_data_flag) {
        // Validation for executing the API only once.
#if (BLOCKING_APPLICATION)
        i2c_status =
          sl_i2c_driver_receive_data_blocking(i2c_instance, ZERO_ADDRESS_VALUE, i2c_read_buffer, I2C_RX_LENGTH);
        if (i2c_status != SL_I2C_SUCCESS) {
          DEBUGOUT("sl_i2c_driver_receive_data_blocking : Invalid Parameters, "
                   "Error Code : %u \n",
                   i2c_status);
        }
#endif
#if (NON_BLOCKING_APPLICATION)
        i2c_status = sl_i2c_driver_receive_data_non_blocking(i2c_instance,
                                                             ZERO_ADDRESS_VALUE,
                                                             i2c_read_buffer,
                                                             I2C_RX_LENGTH,
                                                             &p_dma_config);
        if (i2c_status != SL_I2C_SUCCESS) {
          DEBUGOUT("sl_i2c_driver_receive_data_non_blocking : Invalid Parameters, "
                   "Error Code : %u \n",
                   i2c_status);
        }
#endif
        i2c_receive_data_flag = false;
      }
      // It waits till i2c_transfer_complete is true in callback.
      if (i2c_transfer_complete) {
        i2c_transfer_complete = false;
        i2c_send_data_flag    = true;
        current_mode          = I2C_SEND_DATA;
      }
#if (NON_BLOCKING_APPLICATION)
      if (i2c_driver_dma_error) {
        DEBUGOUT("Data is not transferred to Follower successfully \n");
        i2c_driver_dma_error = false;
      }
#endif
      break;
    case I2C_SEND_DATA:
      if (i2c_send_data_flag) {
        // Validation for executing the API only once.
#if (BLOCKING_APPLICATION)
        i2c_status =
          sl_i2c_driver_send_data_blocking(i2c_instance, ZERO_ADDRESS_VALUE, i2c_write_buffer, I2C_TX_LENGTH);
        if (i2c_status != SL_I2C_SUCCESS) {
          DEBUGOUT("sl_i2c_driver_send_data_blocking : Invalid Parameters, "
                   "Error Code : %u \n",
                   i2c_status);
        }
#endif
#if (NON_BLOCKING_APPLICATION)
        i2c_status = sl_i2c_driver_send_data_non_blocking(i2c_instance,
                                                          ZERO_ADDRESS_VALUE,
                                                          i2c_write_buffer,
                                                          I2C_TX_LENGTH,
                                                          &p_dma_config);
        if (i2c_status != SL_I2C_SUCCESS) {
          DEBUGOUT("sl_i2c_driver_send_data_non_blocking : Invalid Parameters, "
                   "Error Code : %u \n",
                   i2c_status);
        }
#endif
        i2c_send_data_flag = false;
      }
      // It waits till i2c_transfer_complete is true in callback.
      if (i2c_transfer_complete) {
        i2c_send_data_flag    = true;
        i2c_transfer_complete = false;
        current_mode          = I2C_TRANSMISSION_COMPLETED;
        // After the receive is completed, input and output data is compared and
        // output is printed on console.
        compare_data();
      }
#if (NON_BLOCKING_APPLICATION)
      if (i2c_driver_dma_error) {
        DEBUGOUT("Data is not transferred to Follower successfully \n");
        i2c_driver_dma_error = false;
      }
#endif
      break;
    case I2C_TRANSMISSION_COMPLETED:
      // De-initializing i2c instance and unregistering callback
      i2c_status = sl_i2c_driver_deinit(i2c_instance);
      if (i2c_status != SL_I2C_SUCCESS) {
        DEBUGOUT("sl_i2c_driver_deinit : Invalid Parameters, "
                 "Error Code : %u \n",
                 i2c_status);
      }
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
  for (data_index = 0; data_index < I2C_SIZE_BUFFERS; data_index++) {
    // If the i2c_read_buffer and buffer are same, it will be continued else, the
    // for loop will be break.
    if (i2c_write_buffer[data_index] != i2c_read_buffer[data_index]) {
      break;
    }
  }
  if (data_index == I2C_SIZE_BUFFERS) {
    DEBUGOUT("Leader-Follower read-write Data comparison is successful, Test "
             "Case Passed \n");
  } else {
    DEBUGOUT("Leader-Follower read-write Data comparison is not successful, "
             "Test Case Failed \n");
  }
}

/*******************************************************************************
 * I2C Follower callback function
 ******************************************************************************/
void i2c_follower_callback(sl_i2c_instance_t i2c_instance, uint32_t status)
{
  switch (status) {
    case SL_I2C_DATA_TRANSFER_COMPLETE:
      i2c_transfer_complete = true;
      break;
    case SL_I2C_7BIT_ADD:
      i2c_7bit_address_transfer_complete = true;
      break;
    case SL_I2C_10BIT_ADD:
      i2c_10bit_address_transfer_complete = true;
      break;
    case SL_I2C_10BIT_ADD_WITH_REP_START:
      i2c_10bit_address_transfer_complete_with_repeated_start = true;
      break;
    case SL_I2C_ACKNOWLEDGE:
      i2c_diver_ack = true;
      break;
    case SL_I2C_NACK:
      i2c_diver_nack = true;
      break;
    case SL_I2C_ARIBITRATION_LOST:
      i2c_driver_aribitration_lost = true;
      break;
    case SL_I2C_BUS_ERROR:
      i2c_driver_bus_error = true;
      break;
    case SL_I2C_BUS_HOLD:
      i2c_driver_bus_hold = true;
      break;
    case SL_I2C_SDA_ERROR:
      i2c_driver_sda_error = true;
      break;
    case SL_I2C_SCL_ERROR:
      i2c_driver_sca_error = true;
      break;
    case SL_I2C_DMA_TRANSFER_ERROR:
      i2c_driver_dma_error = true;
      break;
    default:
      break;
  }
}
