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
#define I2C_SIZE_BUFFERS             1024    // Size of data buffer
#define I2C_RX_LENGTH                I2C_SIZE_BUFFERS // Number of bytes to receive
#define I2C_TX_LENGTH                I2C_SIZE_BUFFERS // Number of bytes to send
#define I2C_OFFSET_LENGTH            1                // Offset length
#define I2C_TX_FIFO_THRESHOLD        0                // FIFO threshold
#define I2C_RX_FIFO_THRESHOLD        0                // FIFO threshold
#define INITIAL_VALUE                0                // for 0 initial value
#define ONE                          0x1              // for value one
#define INSTANCE_ZERO                0                // For 0 value
#define INSTANCE_ONE                 1                // For 0 value
#define INSTANCE_TWO                 2                // For 0 value
#define MS_DELAY_COUNTER             4600             // Delay count
/*******************************************************************************
 ******************************  Data Types  ***********************************
 ******************************************************************************/
// Enum for different transmission scenarios
typedef enum {
  I2C_SEND_DATA,              // Send mode
  I2C_RECEIVE_DATA,           // Receive mode
  I2C_TRANSMISSION_COMPLETED, // Transmission completed mode
  I2C_IDLE_MODE               // Idle mode
} i2c_action_enum_t;

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
sl_i2c_instance_t i2c_instance = I2C_INSTANCE_USED;
static uint8_t i2c_read_buffer[I2C_SIZE_BUFFERS];
#if NON_BLOCKING_APPLICATION
uint32_t i2c_write_buffer[I2C_SIZE_BUFFERS];
#else
static uint8_t i2c_write_buffer[I2C_SIZE_BUFFERS];
#endif
static i2c_action_enum_t current_mode           = I2C_SEND_DATA;
static boolean_t i2c_send_data_flag             = false;
static boolean_t i2c_receive_data_flag          = false;
static volatile boolean_t i2c_transfer_complete = false;
static boolean_t i2c_driver_dma_error           = false;
sl_i2c_dma_config_t p_dma_config;

osEventFlagsId_t event_flags_id; // event flags id used for i2c-timer event
uint32_t event_flag;
/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void i2c_leader_callback(sl_i2c_instance_t instance, uint32_t status);
static void delay(uint32_t idelay);
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
  sl_i2c_status_t i2c_status;
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
  DEBUGINIT();
#endif
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
  }
  i2c_send_data_flag = true;

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
  sl_i2c_status_t i2c_status;
  // In switch case, according to the current mode, the transmission is
  // executed.
  // First leader sends data to follower, using I2C send
  // Then leader receives same data from follower, using I2C receive.
  switch (current_mode) {
    case I2C_SEND_DATA:
      if (i2c_send_data_flag) {
        //  Validation for executing the API only once.
#if (BLOCKING_APPLICATION)
        i2c_status = sl_i2c_driver_send_data_blocking(i2c_instance, FOLLOWER_I2C_ADDR, i2c_write_buffer, I2C_TX_LENGTH);
        if (i2c_status != SL_I2C_SUCCESS) {
          DEBUGOUT("sl_i2c_driver_send_data_blocking : Invalid Parameters, "
                   "Error Code : %u \n",
                   i2c_status);
          break;
        }
#endif
#if (NON_BLOCKING_APPLICATION)
        i2c_status = sl_i2c_driver_send_data_non_blocking(i2c_instance,
                                                          FOLLOWER_I2C_ADDR,
                                                          i2c_write_buffer,
                                                          I2C_TX_LENGTH,
                                                          &p_dma_config);
        if (i2c_status != SL_I2C_SUCCESS) {
          DEBUGOUT("sl_i2c_driver_send_data_non_blocking : Invalid Parameters, "
                   "Error Code : %u \n",
                   i2c_status);
          break;
        }
#endif
        i2c_send_data_flag = false;
      }
#if (BLOCKING_APPLICATION)
      i2c_receive_data_flag = true;
      current_mode          = I2C_RECEIVE_DATA;
#endif
#if (NON_BLOCKING_APPLICATION)
      // It waits till i2c_transfer_complete is true in callback.
      if (i2c_transfer_complete) {
        i2c_transfer_complete = false;
        i2c_receive_data_flag = true;
        current_mode          = I2C_RECEIVE_DATA;
      }
      if (i2c_driver_dma_error) {
        DEBUGOUT("Data is not transferred to Follower successfully \n");
        i2c_driver_dma_error = false;
        break;
      }
#endif
      break;

    case I2C_RECEIVE_DATA:
      if (i2c_receive_data_flag) {
        // Adding delay for synchronization before leader sends read request
        delay(5);
        // Disabling repeated start before last cycle of transfer
        i2c_status = sl_i2c_driver_enable_repeated_start(i2c_instance, false);
        if (i2c_status != SL_I2C_SUCCESS) {
          DEBUGOUT("sl_i2c_driver_enable_repeated_start : Invalid Parameters, Error Code : %u \n", i2c_status);
        }
        // Validation for executing the API only once.
#if (BLOCKING_APPLICATION)
        i2c_status =
          sl_i2c_driver_receive_data_blocking(i2c_instance, FOLLOWER_I2C_ADDR, i2c_read_buffer, I2C_RX_LENGTH);
        if (i2c_status != SL_I2C_SUCCESS) {
          DEBUGOUT("sl_i2c_driver_receive_data_blocking : Invalid Parameters, Error "
                   "Code : %u \n",
                   i2c_status);
          break;
        }
#endif
#if (NON_BLOCKING_APPLICATION)
        i2c_status = sl_i2c_driver_receive_data_non_blocking(i2c_instance,
                                                             FOLLOWER_I2C_ADDR,
                                                             i2c_read_buffer,
                                                             I2C_RX_LENGTH,
                                                             &p_dma_config);
        if (i2c_status != SL_I2C_SUCCESS) {
          DEBUGOUT("sl_i2c_driver_receive_data_non_blocking : Invalid Parameters, Error "
                   "Code : %u \n",
                   i2c_status);
          break;
        }
#endif
        i2c_receive_data_flag = false;
      }
#if (BLOCKING_APPLICATION)
      current_mode = I2C_TRANSMISSION_COMPLETED;
      // After the receive is completed, input and output data is compared and
      // output is printed on console.
      compare_data();
#endif
#if (NON_BLOCKING_APPLICATION)
      // It waits till i2c_transfer_complete is true in callback.
      if (i2c_transfer_complete) {
        i2c_transfer_complete = false;
        current_mode          = I2C_TRANSMISSION_COMPLETED;
        // After the receive is completed, input and output data is compared and
        // output is printed on console.
        compare_data();
      }
      if (i2c_driver_dma_error) {
        DEBUGOUT("Data is not received from Follower successfully \n");
        i2c_driver_dma_error = false;
        break;
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
      } else {
        current_mode = I2C_IDLE_MODE;
      }
      break;

    case I2C_IDLE_MODE:
    default:
      break;
  }
}
/*******************************************************************************
 * Delay function for 1ms
 ******************************************************************************/
static void delay(uint32_t idelay)
{
  for (uint32_t x = 0; x < MS_DELAY_COUNTER * idelay; x++) {
    __NOP();
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

    default:
      break;
  }
}
