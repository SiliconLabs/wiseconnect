/***************************************************************************/ /**
* @file i2c_leader_example.c
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
#include "i2c_app.h"
#include "app.h"
#include "rsi_debug.h"
#include "sl_i2c_instances.h"
#include "sl_si91x_i2c.h"
#include "sl_si91x_peripheral_i2c.h"
#include "cmsis_os2.h"
#include "sl_component_catalog.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define NON_BLOCKING_APPLICATION     DISABLE // Enable this macro when transfer type is DMA
#define BLOCKING_APPLICATION         ENABLE  // Enable this macro when transfer type is Interrupt type
#define I2C_INSTANCE_USED            2       // update this macro with i2c instance number used for application
#define FOLLOWER_I2C_ADDR            0x50    // I2C follower address
#define MAX_BUFFER_SIZE_BLOCKING     80000   // Maximum buffer size for RX and TX length when transferring without DMA
#define MAX_BUFFER_SIZE_NON_BLOCKING 30000   // Maximum buffer size for RX and TX length when transferring with DMA
#define SIZE_BUFFERS                 15      // Size of data buffer
#define RX_LENGTH                    SIZE_BUFFERS // Number of bytes to receive
#define TX_LENGTH                    SIZE_BUFFERS // Number of bytes to send
#define OFFSET_LENGTH                1            // Offset length
#define TX_FIFO_THRESHOLD            0            // FIFO threshold
#define RX_FIFO_THRESHOLD            0            // FIFO threshold
#define INITIAL_VALUE                0            // for 0 initial value
#define ONE                          0x1          // for value one
#define INSTANCE_ZERO                0            // For 0 value
#define INSTANCE_ONE                 1            // For 0 value
#define INSTANCE_TWO                 2            // For 0 value
/*******************************************************************************
 ******************************  Data Types  ***********************************
 ******************************************************************************/
// Enum for different transmission scenarios
typedef enum {
  SEND_DATA,              // Send mode
  RECEIVE_DATA,           // Receive mode
  TRANSMISSION_COMPLETED, // Transmission completed mode
} i2c_action_enum_t;

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
sl_i2c_status_t i2c_status;
sl_i2c_instance_t i2c_instance = I2C_INSTANCE_USED;
#if NON_BLOCKING_APPLICATION
static uint8_t read_buffer[SIZE_BUFFERS];
#else
static uint8_t read_buffer[SIZE_BUFFERS + OFFSET_LENGTH];
#endif
#if NON_BLOCKING_APPLICATION
volatile uint32_t write_buffer[SIZE_BUFFERS];
#else
static uint8_t write_buffer[SIZE_BUFFERS];
#endif
static i2c_action_enum_t current_mode                             = SEND_DATA;
boolean_t send_data_flag                                          = false;
boolean_t receive_data_flag                                       = false;
volatile boolean_t i2c_transfer_complete                          = false;
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

osEventFlagsId_t event_flags_id; // event flags id used for i2c-timer event
uint32_t event_flag;
/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void i2c_leader_callback(sl_i2c_instance_t i2c_instance, uint32_t status);
static void compare_data(void);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * I2C initialization function
 ******************************************************************************/
void i2c_app(void)
{
  while (1) {
#if defined(SL_CATALOG_KERNEL_PRESENT)
    event_flag = osEventFlagsWait(event_flags_id, EVENT_FLAGS_I2C_TIMER_MASKBIT, osFlagsWaitAny, osWaitForever);
#endif // SL_CATALOG_KERNEL_PRESENT
    // handle event
    while (event_flag & EVENT_FLAGS_I2C_TIMER_MASKBIT) {
      i2c_leader_example_process_action();
    }
  }
}
void i2c_init(void)
{
#if (I2C_INSTANCE_USED == INSTANCE_ZERO)
  // Update structure name as per instance used, to register I2C callback
  sl_i2c_i2c0_config.i2c_callback = i2c_leader_callback;
  // Initializing I2C instance (update i2c config-strucure name as per instance used)
  i2c_status = sl_i2c_driver_init(i2c_instance, &sl_i2c_i2c0_config);
#endif
#if (I2C_INSTANCE_USED == INSTANCE_ONE)
  // Update structure name as per instance used, to register I2C callback
  sl_i2c_i2c1_config.i2c_callback = i2c_leader_callback;
  // Initializing I2C instance (update i2c config-strucure name as per instance used)
  i2c_status = sl_i2c_driver_init(i2c_instance, &sl_i2c_i2c1_config);
#endif
#if (I2C_INSTANCE_USED == INSTANCE_TWO)
  // Update structure name as per instance used, to register I2C callback
  sl_i2c_i2c2_config.i2c_callback = i2c_leader_callback;
  // Initializing I2C instance (update i2c config-strucure name as per instance used)
  i2c_status = sl_i2c_driver_init(i2c_instance, &sl_i2c_i2c2_config);
#endif
  if (i2c_status != SL_I2C_SUCCESS) {
    DEBUGOUT("sl_i2c_driver_init : Invalid Parameters, Error Code : %u \n", i2c_status);
  } else {
    DEBUGOUT("Successfully initialized and configured i2c leader\n");
  }
  // Configuring RX and TX FIFO thresholds
  i2c_status = sl_i2c_driver_configure_fifo_threshold(i2c_instance, TX_FIFO_THRESHOLD, RX_FIFO_THRESHOLD);
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
  for (uint32_t loop = INITIAL_VALUE; loop < SIZE_BUFFERS; loop++) {
    write_buffer[loop] = (uint8_t)(loop + ONE);
    read_buffer[loop]  = INITIAL_VALUE;
  }
  send_data_flag = true;

  event_flags_id = osEventFlagsNew(NULL);
  if (event_flags_id == NULL) {
    ; // Event Flags object not created, handle failure
  }
}

/*******************************************************************************
 * Function will run continuously in while loop
 ******************************************************************************/
void i2c_leader_example_process_action(void)
{
  // In switch case, according to the current mode, the transmission is
  // executed.
  // First leader sends data to follower, using I2C send
  // Then leader receives same data from follower, using I2C receive.
  switch (current_mode) {
    case SEND_DATA:
      if (send_data_flag) {
        //  Validation for executing the API only once.
#if (BLOCKING_APPLICATION)
        i2c_status = sl_i2c_driver_send_data_blocking(i2c_instance, FOLLOWER_I2C_ADDR, write_buffer, TX_LENGTH);
        if (i2c_status != SL_I2C_SUCCESS) {
          DEBUGOUT("sl_i2c_driver_send_data_blocking : Invalid Parameters, "
                   "Error Code : %u \n",
                   i2c_status);
        }
#endif
#if (NON_BLOCKING_APPLICATION)
        i2c_status =
          sl_i2c_driver_send_data_non_blocking(i2c_instance, FOLLOWER_I2C_ADDR, write_buffer, TX_LENGTH, &p_dma_config);
        if (i2c_status != SL_I2C_SUCCESS) {
          DEBUGOUT("sl_i2c_driver_send_data_non_blocking : Invalid Parameters, "
                   "Error Code : %u \n",
                   i2c_status);
        }
#endif
        send_data_flag = false;
      }
      // It waits till i2c_transfer_complete is true in callback.
      /*      DEBUGOUT("IN Fun :: i2c_transfer_complete BEFORE \n");*/
      if (i2c_transfer_complete) {
        receive_data_flag     = true;
        i2c_transfer_complete = false;
        current_mode          = RECEIVE_DATA;
      }
#if (NON_BLOCKING_APPLICATION)
      if (i2c_driver_dma_error) {
        DEBUGOUT("Data is not transferred to Follower successfully \n");
        i2c_driver_dma_error = false;
      }
#endif
      break;
    case RECEIVE_DATA:
      if (receive_data_flag) {
        // Validation for executing the API only once.
#if (BLOCKING_APPLICATION)
        i2c_status = sl_i2c_driver_receive_data_blocking(i2c_instance, FOLLOWER_I2C_ADDR, read_buffer, RX_LENGTH);
        if (i2c_status != SL_I2C_SUCCESS) {
          DEBUGOUT("sl_i2c_driver_receive_data_blocking : Invalid Parameters, Error "
                   "Code : %u \n",
                   i2c_status);
        }
#endif
#if (NON_BLOCKING_APPLICATION)
        i2c_status = sl_i2c_driver_receive_data_non_blocking(i2c_instance,
                                                             FOLLOWER_I2C_ADDR,
                                                             read_buffer,
                                                             RX_LENGTH,
                                                             &p_dma_config);
        if (i2c_status != SL_I2C_SUCCESS) {
          DEBUGOUT("sl_i2c_driver_receive_data_non_blocking : Invalid Parameters, Error "
                   "Code : %u \n",
                   i2c_status);
        }
#endif
        receive_data_flag = false;
      }
      // It waits till i2c_transfer_complete is true in callback.
      if (i2c_transfer_complete) {
        send_data_flag        = true;
        i2c_transfer_complete = false;
        current_mode          = TRANSMISSION_COMPLETED;
        // After the receive is completed, input and output data is compared and
        // output is printed on console.
        compare_data();
      }
#if (NON_BLOCKING_APPLICATION)
      if (i2c_driver_dma_error) {
        DEBUGOUT("Data is not received from Follower successfully \n");
        i2c_driver_dma_error = false;
      }
#endif
      break;
    case TRANSMISSION_COMPLETED:
      current_mode = SEND_DATA;
      event_flag   = 0;
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
  uint32_t data_index;
  for (data_index = 0; data_index < SIZE_BUFFERS; data_index++) {
    // If the read_buffer and buffer are same, it will be continued else, the
    // for loop will be break.
    if (write_buffer[data_index] != read_buffer[data_index]) {
      break;
    }
  }
  if (data_index == SIZE_BUFFERS) {
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
void i2c_leader_callback(sl_i2c_instance_t i2c_instance, uint32_t status)
{
  // to avoid unused variable warning
  (void)i2c_instance;

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
