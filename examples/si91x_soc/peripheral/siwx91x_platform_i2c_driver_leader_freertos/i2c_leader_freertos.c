/***************************************************************************/ /**
 * @file i2c_leader_freertos.c
 * @brief I2C Leader Blocking FreeRTOS example
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
#include "i2c_leader_freertos.h"
#include "cmsis_os2.h"
#include "rsi_debug.h"
#include "sl_i2c_instances.h"
#include "sl_si91x_i2c.h"
#include "sl_si91x_peripheral_i2c.h"
#include "sl_component_catalog.h"
#include "sl_status.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define FOLLOWER_I2C_ADDR     0x50 // I2C follower address
#define I2C_BUFFER_SIZE       1024 // Size of data buffer
#define I2C_TX_FIFO_THRESHOLD 0    // FIFO threshold
#define I2C_RX_FIFO_THRESHOLD 0    // FIFO threshold
#define INITIAL_VALUE         0    // Initial value of buffer
#define BUFFER_OFFSET         0x1  // Buffer offset

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
static sl_i2c_instance_t i2c_instance;            // Active I2C peripheral instance (I2C0/I2C1/ULP_I2C)
static uint8_t i2c_read_buffer[I2C_BUFFER_SIZE];  // Buffer for data received from the follower
static uint8_t i2c_write_buffer[I2C_BUFFER_SIZE]; // Buffer for data sent to the follower
static sl_i2c_config_t sl_i2c_config;             // I2C driver configuration (populated from instance config)

/* FreeRTOS task attributes for the I2C leader task */
static const osThreadAttr_t i2c_leader_thread_attributes = {
  .name       = "i2c_leader_task",
  .stack_size = 2048,
  .priority   = osPriorityLow1,
};

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static sl_status_t i2c_leader_init_function(void);
static void i2c_leader_task(void *argument);
static sl_status_t i2c_app_compare_data(void);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * @brief  Entry point: creates the I2C leader FreeRTOS task.
 * @return None
 ******************************************************************************/
void i2c_leader_example_init(void)
{
  osThreadId_t thread_id = osThreadNew((osThreadFunc_t)i2c_leader_task, NULL, &i2c_leader_thread_attributes);
  if (thread_id == NULL) {

    /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
    SL_PRINT_STRING_ERROR("Failed to create I2C leader task\r\n");
  }
}

/*******************************************************************************
 * @brief  Initializes the I2C peripheral in leader mode.
 * @return sl_status_t SL_STATUS_OK on success, or an error code.
 ******************************************************************************/
static sl_status_t i2c_leader_init_function(void)
{
  sl_i2c_status_t i2c_status = 0;

  /* Select I2C instance based on which component is present */
#if defined(SL_CATALOG_I2C_I2C0_PRESENT)
  sl_i2c_config = sl_i2c_i2c0_config;
  i2c_instance  = SL_I2C0;
#elif defined(SL_CATALOG_I2C_I2C1_PRESENT)
  sl_i2c_config = sl_i2c_i2c1_config;
  i2c_instance  = SL_I2C1;
#elif defined(SL_CATALOG_I2C_I2C2_PRESENT)
  sl_i2c_config = sl_i2c_i2c2_config;
  i2c_instance  = SL_ULP_I2C;
#endif

  /* Initialize I2C driver */
  i2c_status = sl_i2c_driver_init(i2c_instance, &sl_i2c_config);
  DEBUGINIT();
  if (i2c_status != SL_I2C_SUCCESS) {
    SL_PRINT_STRING_ERROR("sl_i2c_driver_init: Error Code : %u\r\n", i2c_status);
    return SL_STATUS_FAIL;
  }
  SL_PRINT_STRING_ERROR("Successfully initialized I2C leader\r\n");

  /* Configure RX and TX FIFO thresholds */
  i2c_status = sl_i2c_driver_configure_fifo_threshold(i2c_instance, I2C_TX_FIFO_THRESHOLD, I2C_RX_FIFO_THRESHOLD);
  if (i2c_status != SL_I2C_SUCCESS) {
    SL_PRINT_STRING_ERROR("sl_i2c_driver_configure_fifo_threshold: Error Code : %u\r\n", i2c_status);
    return SL_STATUS_FAIL;
  }
  SL_PRINT_STRING_ERROR("Successfully configured I2C TX & RX FIFO thresholds\r\n");

  /* Enable repeated start for combined-format (write-then-read) transfer */
  i2c_status = sl_i2c_driver_enable_repeated_start(i2c_instance, true);
  if (i2c_status != SL_I2C_SUCCESS) {
    SL_PRINT_STRING_ERROR("sl_i2c_driver_enable_repeated_start: Error Code : %u\r\n", i2c_status);
    return SL_STATUS_FAIL;
  }
  SL_PRINT_STRING_ERROR("Successfully enabled repeated start\r\n");

  /* Fill TX buffer with test pattern */
  for (uint32_t loop = INITIAL_VALUE; loop < I2C_BUFFER_SIZE; loop++) {
    i2c_write_buffer[loop] = (uint8_t)(loop + BUFFER_OFFSET);
  }

  return SL_STATUS_OK;
}

/*******************************************************************************
 * @brief  FreeRTOS task (single shot): send/receive/compare once, exit.
 * @param  argument  Unused task argument.
 ******************************************************************************/
static void i2c_leader_task(void *argument)
{
  (void)argument;
  sl_i2c_status_t i2c_status = 0;

  /* Initialize I2C peripheral in leader mode */
  sl_status_t status = i2c_leader_init_function();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2C leader init failed\r\n");
    osThreadExit();
  }

  /* Retry blocking transfers while the driver returns SL_I2C_TIMEOUT (2 s window per attempt). */
  do {
    i2c_status = sl_i2c_driver_send_data_blocking(i2c_instance, FOLLOWER_I2C_ADDR, i2c_write_buffer, I2C_BUFFER_SIZE);
  } while (i2c_status == SL_I2C_TIMEOUT);
  if (i2c_status != SL_I2C_SUCCESS) {
    SL_PRINT_STRING_ERROR("sl_i2c_driver_send_data_blocking: Error Code : %u\r\n", i2c_status);
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("I2C leader send completed\r\n");

  i2c_status = sl_i2c_driver_enable_repeated_start(i2c_instance, false);
  if (i2c_status != SL_I2C_SUCCESS) {
    SL_PRINT_STRING_ERROR("sl_i2c_driver_enable_repeated_start: Error Code : %u\r\n", i2c_status);
  }

  do {
    i2c_status = sl_i2c_driver_receive_data_blocking(i2c_instance, FOLLOWER_I2C_ADDR, i2c_read_buffer, I2C_BUFFER_SIZE);
  } while (i2c_status == SL_I2C_TIMEOUT);
  if (i2c_status != SL_I2C_SUCCESS) {
    SL_PRINT_STRING_ERROR("sl_i2c_driver_receive_data_blocking: Error Code : %u\r\n", i2c_status);
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("I2C leader receive completed\r\n");

  if (i2c_app_compare_data() != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("I2C leader data comparison failed\r\n");
  }

  i2c_status = sl_i2c_driver_enable_repeated_start(i2c_instance, true);
  if (i2c_status != SL_I2C_SUCCESS) {
    SL_PRINT_STRING_ERROR("sl_i2c_driver_enable_repeated_start: Error Code : %u\r\n", i2c_status);
  }

  osThreadExit();
}

/*******************************************************************************
 * @brief  Compares the TX and RX buffers and prints the result.
 * @return sl_status_t SL_STATUS_OK on match, SL_STATUS_FAIL on mismatch.
 ******************************************************************************/
static sl_status_t i2c_app_compare_data(void)
{
  for (uint32_t data_index = 0; data_index < I2C_BUFFER_SIZE; data_index++) {
    if (i2c_write_buffer[data_index] != i2c_read_buffer[data_index]) {
      return SL_STATUS_FAIL;
    }
  }
  SL_PRINT_STRING_ERROR("Leader-Follower read-write data comparison successful, "
                        "Test Case Passed\r\n");
  return SL_STATUS_OK;
}
