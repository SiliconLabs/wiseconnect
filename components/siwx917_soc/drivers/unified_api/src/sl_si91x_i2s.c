/***************************************************************************/ /**
 * @file sl_si91x_i2s.c
 * @brief I2S API implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "rsi_i2s.h"
#include "rsi_board.h"
#include "rsi_rom_clks.h"
#include "clock_update.h"
#include "sl_si91x_i2s.h"

/*******************************************************************************
 ***************************  DEFINES / MACROS   ********************************
 ******************************************************************************/
#define I2S0_INSTANCE           0  //I2S0 instance number
#define I2S1_INSTANCE           1  //I2S1 instance number
#define I2S_RESOLUTION_COUNT    5  //Number of supported I2S resolutions
#define I2S_SAMPLING_RATE_COUNT 11 //Number of supported I2S sampling rates

#define I2S_RELEASE_VERSION 1 // GSPI Release version
#define I2S_SQA_VERSION     0 // GSPI SQA version
#define I2S_DEV_VERSION     0 // GSPI Developer version

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
static sl_i2s_signal_event_t i2s0_user_callback = NULL;
static const void *local_i2s0_handle            = NULL;
static sl_i2s_signal_event_t i2s1_user_callback = NULL;
static const void *local_i2s1_handle            = NULL;
extern sl_i2s_driver_t Driver_SAI0;
extern sl_i2s_driver_t Driver_SAI1;

/*******************************************************************************
 ***************************  Local Types  ********************************
 ******************************************************************************/

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/
static sl_status_t convert_arm_to_sl_error_code(int32_t error);
static sl_status_t convert_rsi_to_sl_error_code(int32_t error);
static void i2s0_callback_event_handler(uint32_t event);
static void i2s1_callback_event_handler(uint32_t event);

/*******************************************************************************
 **********************  Local Function Definition****************************
 ******************************************************************************/

/*******************************************************************************
***********************  Global function Definitions *************************
 ******************************************************************************/
/*******************************************************************************
 * Initialize I2S channels and pins. Also initialize DMA peripheral for I2S transfer
 ******************************************************************************/
sl_status_t sl_si91x_i2s_init(uint32_t i2s_instance, sl_i2s_handle_t *i2s_handle)
{
  sl_status_t status = SL_STATUS_OK;
  int32_t error_status;
  sl_i2s_handle_t i2s_temp_handle;

  do {
    if (i2s_handle == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    if (i2s_instance > I2S1_INSTANCE) {
      //Invalid i2S instance number
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // To validate status of I2S, if already enabled, returns error code.
    if (I2S_GetInitState(i2s_instance)) {
      //I2S instance already initialized
      status = SL_STATUS_BUSY;
      break;
    }
    if (i2s_instance == I2S0_INSTANCE) {
      //I2C0 instance
      i2s_temp_handle = &Driver_SAI0;
    }
    if (i2s_instance == I2S1_INSTANCE) {
      //I2C1 instance
      i2s_temp_handle = &Driver_SAI1;
    }
    *i2s_handle = i2s_temp_handle;
    // CMSIS API for initialization is called with the function address of
    // callback function, and the arm error code returned from the API is
    // converted to SL error code via convert_arm_to_sl_error_code function.
    if (i2s_instance == I2S0_INSTANCE) {
      error_status = ((sl_i2s_driver_t *)i2s_temp_handle)->Initialize(i2s0_callback_event_handler);
    } else {
      error_status = ((sl_i2s_driver_t *)i2s_temp_handle)->Initialize(i2s1_callback_event_handler);
    }
    status = convert_arm_to_sl_error_code(error_status);
  } while (false);

  return status;
}

/*******************************************************************************
 * Uninitialize DMA peripheral used for I2S transfer
 ******************************************************************************/
sl_status_t sl_si91x_i2s_deinit(sl_i2s_handle_t *i2s_handle)
{
  sl_status_t status = SL_STATUS_OK;
  int32_t error_status;

  if (i2s_handle == NULL) {
    status = SL_STATUS_NULL_POINTER;
  } else {
    if ((i2s_handle == &Driver_SAI0) || (i2s_handle == &Driver_SAI1)) {
      // CMSIS API for un-initialization is called and the arm error code returned from
      // the API is converted to SL error code via convert_arm_to_sl_error_code function.
      error_status = ((sl_i2s_driver_t *)i2s_handle)->Uninitialize();
      status       = convert_arm_to_sl_error_code(error_status);
    } else {
      //Invalid I2S handle
      status = SL_STATUS_INVALID_PARAMETER;
    }
  }

  return status;
}

/*******************************************************************************
 * To change the power mode of I2S
 * The following two modes are supported
 * I2S_POWER_OFF
 * I2S_FULL_POWER
 * In power off mode, I2S peripheral is disabled
 ******************************************************************************/
sl_status_t sl_si91x_i2s_configure_power_mode(sl_i2s_handle_t i2s_handle, sl_i2s_power_state_t state)
{
  sl_status_t status = SL_STATUS_OK;
  int32_t error_status;

  do {
    if (i2s_handle == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    if ((i2s_handle != &Driver_SAI0) && (i2s_handle != &Driver_SAI1)) {
      //Invalid I2S handle
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    if ((state < SL_I2S_POWER_OFF) || (state > SL_I2S_FULL_POWER)) {
      //Invalid power state
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // CMSIS API for power mode config is called and the arm error code returned from
    // the API is converted to SL error code via convert_arm_to_sl_error_code function.
    error_status = ((sl_i2s_driver_t *)i2s_handle)->PowerControl(state);
    status       = convert_arm_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * Enable I2S peripheral and configure Tx/Rx channels of I2S. Configure maximum FIFO
 * threshold level 
 ******************************************************************************/
sl_status_t sl_si91x_i2s_config_transmit_receive(sl_i2s_handle_t i2s_handle, sl_i2s_xfer_config_t *xfer_config)
{
  sl_status_t status = SL_STATUS_OK;
  int32_t error_status;
  uint8_t i2s_resolution[I2S_RESOLUTION_COUNT]        = { SL_I2S_RESOLUTION_12,
                                                          SL_I2S_RESOLUTION_16,
                                                          SL_I2S_RESOLUTION_20,
                                                          SL_I2S_RESOLUTION_24,
                                                          SL_I2S_RESOLUTION_32 };
  uint32_t i2s_sampling_rate[I2S_SAMPLING_RATE_COUNT] = {
    SL_I2S_SAMPLING_RATE_8000,  SL_I2S_SAMPLING_RATE_11025, SL_I2S_SAMPLING_RATE_16000, SL_I2S_SAMPLING_RATE_22050,
    SL_I2S_SAMPLING_RATE_24000, SL_I2S_SAMPLING_RATE_32000, SL_I2S_SAMPLING_RATE_44100, SL_I2S_SAMPLING_RATE_48000,
    SL_I2S_SAMPLING_RATE_88200, SL_I2S_SAMPLING_RATE_96000, SL_I2S_SAMPLING_RATE_192000
  };
  uint32_t res_validation;
  uint32_t sampling_validation;
  uint32_t transfer_init;

  do {
    if (i2s_handle == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    if ((i2s_handle != &Driver_SAI0) && (i2s_handle != &Driver_SAI1)) {
      //Invalid I2S handle
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    if ((xfer_config->mode != SL_I2S_MASTER) && (xfer_config->mode != SL_I2S_SLAVE)) {
      //Invalid I2S mode
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    if (xfer_config->protocol != SL_I2S_PROTOCOL) {
      //Invalid SAI protocol
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    if ((xfer_config->sync != SL_I2S_SYNC) && (xfer_config->sync != SL_I2S_ASYNC)) {
      //Invalid SYNC mode
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    if ((xfer_config->data_size != SL_I2S_DATA_SIZE16) && (xfer_config->data_size != SL_I2S_DATA_SIZE32)) {
      //Invalid SYNC mode
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    if ((xfer_config->transfer_type != SL_I2S_TRANSMIT) && (xfer_config->transfer_type != SL_I2S_RECEIVE)) {
      //Invalid transfer type
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    for (res_validation = 0; res_validation < I2S_RESOLUTION_COUNT; res_validation++) {
      if (xfer_config->resolution == i2s_resolution[res_validation]) {
        break;
      }
    }
    for (sampling_validation = 0; sampling_validation < I2S_SAMPLING_RATE_COUNT; sampling_validation++) {
      if (xfer_config->sampling_rate == i2s_sampling_rate[sampling_validation]) {
        break;
      }
    }
    if ((res_validation == I2S_RESOLUTION_COUNT) || (sampling_validation == I2S_SAMPLING_RATE_COUNT)) {
      //Invalid Resolution/Sampling rate
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // CMSIS API for Tx/Rx config is called and the arm error code returned from
    // the API is converted to SL error code via convert_arm_to_sl_error_code function.
    error_status = ((sl_i2s_driver_t *)i2s_handle)
                     ->Control(xfer_config->transfer_type | xfer_config->mode | xfer_config->protocol
                                 | xfer_config->sync | ARM_SAI_DATA_SIZE(xfer_config->data_size),
                               xfer_config->resolution,
                               xfer_config->sampling_rate);
    status = convert_arm_to_sl_error_code(error_status);
    if (status != SL_STATUS_OK) {
      break;
    }
    if (xfer_config->transfer_type == SL_I2S_TRANSMIT) {
      transfer_init = SL_I2S_TRANSMIT_CONTROL;
    } else {
      transfer_init = SL_I2S_RECEIVE_CONTROL;
    }
    error_status = ((sl_i2s_driver_t *)i2s_handle)->Control(transfer_init, 1, 0);
    status       = convert_arm_to_sl_error_code(error_status);
  } while (false);

  return status;
}
/*******************************************************************************
 * Configure I2S transmit. After configuring, I2S device generated sampling frequency
 * and send data on Tx line
 ******************************************************************************/
sl_status_t sl_si91x_i2s_transmit_data(sl_i2s_handle_t i2s_handle, const void *data, uint32_t size)
{
  sl_status_t status = SL_STATUS_OK;
  int32_t error_status;
  do {
    if ((data == NULL) || (i2s_handle == NULL)) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    if ((i2s_handle != &Driver_SAI0) && (i2s_handle != &Driver_SAI1)) {
      //Invalid I2S handle
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // CMSIS API for I2S send/receive is called and the arm error code returned from
    // the API is converted to SL error code via convert_arm_to_sl_error_code function.
    error_status = ((sl_i2s_driver_t *)i2s_handle)->Send(data, size);
  } while (false);
  return status;
}

/*******************************************************************************
 * Configure I2S receive. After configuring I2S for Rx, device will wait for 
 * sampling frequency to be generated and sample data on Rx line.
 ******************************************************************************/
sl_status_t sl_si91x_i2s_receive_data(sl_i2s_handle_t i2s_handle, const void *data, uint32_t size)
{
  sl_status_t status = SL_STATUS_OK;
  int32_t error_status;
  do {
    if ((data == NULL) || (i2s_handle == NULL)) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    if ((i2s_handle != &Driver_SAI0) && (i2s_handle != &Driver_SAI1)) {
      //Invalid I2S handle
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // CMSIS API for I2S send/receive is called and the arm error code returned from
    // the API is converted to SL error code via convert_arm_to_sl_error_code function.
    error_status = ((sl_i2s_driver_t *)i2s_handle)->Receive(data, size);
  } while (false);
  return status;
}

/*******************************************************************************
 * To register the user event callback
 * It registers the callback, i.e., stores the callback function address 
 * and pass to the variable that is called in Interrupt Handler.
 * If another callback is registered without unregistering previous callback then, it
 * returns an error code, so it is mandatory to unregister the callback before registering
 * another callback.
 * It will returns error if any callback is already registered.
 ******************************************************************************/
sl_status_t sl_si91x_i2s_register_event_callback(sl_i2s_handle_t i2s_handle, sl_i2s_signal_event_t callback_event)
{
  sl_status_t status = SL_STATUS_OK;
  do {
    // Validates the null pointer, if true returns error code
    if ((i2s_handle == NULL) || (callback_event == NULL)) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    if ((i2s_handle != &Driver_SAI0) && (i2s_handle != &Driver_SAI1)) {
      //Invalid I2S handle
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    if (i2s_handle == &Driver_SAI0) {
      // To validate the function pointer if the parameters is not NULL then, it
      // returns an error code
      if ((i2s0_user_callback != NULL) || (local_i2s0_handle != NULL)) {
        status = SL_STATUS_BUSY;
        break;
      }
      // User callback address is passed to the static variable which is called at the time of
      // interrupt
      i2s0_user_callback = callback_event;
      local_i2s0_handle  = i2s_handle;
    } else {
      // To validate the function pointer if the parameters is not NULL then, it
      // returns an error code
      if ((i2s1_user_callback != NULL) || (local_i2s1_handle != NULL)) {
        status = SL_STATUS_BUSY;
        break;
      }
      // User callback address is passed to the static variable which is called at the time of
      // interrupt
      i2s1_user_callback = callback_event;
      local_i2s1_handle  = i2s_handle;
    }
    // Returns SL_STATUS_OK if callback is successfully registered
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * To unregister the user event callback
 * It unregisters the callback, i.e., clear the callback function address
 * and pass NULL value to the variable
 ******************************************************************************/
sl_status_t sl_si91x_i2s_unregister_event_callback(sl_i2s_handle_t i2s_handle)
{
  sl_status_t status = SL_STATUS_OK;
  do {
    // Validates the null pointer, if true returns error code
    if (i2s_handle == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    if ((i2s_handle != &Driver_SAI0) && (i2s_handle != &Driver_SAI1)) {
      //Invalid I2S handle
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    if (i2s_handle == &Driver_SAI0) {
      i2s0_user_callback = NULL;
      local_i2s0_handle  = NULL;
    } else {
      i2s1_user_callback = NULL;
      local_i2s1_handle  = NULL;
    }
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * Used to abort Tx/Rx transfers.
 * This function also need to be used by master after transmitting data to stop
 * generating sampling frequency.
 ******************************************************************************/
sl_status_t sl_si91x_i2s_end_transfer(sl_i2s_handle_t i2s_handle, sl_i2s_xfer_type_t abort_type)
{
  sl_status_t status = SL_STATUS_OK;
  int32_t error_status;
  do {
    // Validates the null pointer, if true returns error code
    if (i2s_handle == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    if ((i2s_handle != &Driver_SAI0) && (i2s_handle != &Driver_SAI1)) {
      //Invalid I2S handle
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    if ((abort_type != SL_I2S_SEND_ABORT) && (abort_type != SL_I2S_RECEIVE_ABORT)) {
      //Only pass transmit/receive abort to xfer_type
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // CMSIS API for Tx/Rx config is called and the arm error code returned from
    // the API is converted to SL error code via convert_arm_to_sl_error_code function.
    error_status = ((sl_i2s_driver_t *)i2s_handle)->Control(abort_type, 1, 0);
    status       = convert_arm_to_sl_error_code(error_status);
  } while (false);
  return status;
}
/*******************************************************************************
 * To get the transmit data count of I2S
 ******************************************************************************/
uint32_t sl_si91x_i2s_get_transmit_data_count(sl_i2s_handle_t i2s_handle)
{
  return ((sl_i2s_driver_t *)i2s_handle)->GetTxCount();
}

/*******************************************************************************
 * To get the receive data count of I2S
 ******************************************************************************/
uint32_t sl_si91x_i2s_get_receive_data_count(sl_i2s_handle_t i2s_handle)
{
  return ((sl_i2s_driver_t *)i2s_handle)->GetRxCount();
}

/*******************************************************************************
 * To get the release, sqa and dev version of I2S
 * It returns the structure for gspi version.
 * Structure includes three members:
 * - Release version
 * - Major version (SQA version)
 * - Minor version (Dev version)
 ******************************************************************************/
sl_i2s_version_t sl_si91x_i2s_get_version(void)
{
  sl_i2s_version_t version;
  version.release = I2S_RELEASE_VERSION;
  version.major   = I2S_SQA_VERSION;
  version.minor   = I2S_DEV_VERSION;
  return version;
}

/*******************************************************************************
 * To get the transfer status I2S
 * It returns the result (sl_i2s_status_t) type structure fetched from 
 * GetStatus() API.
 * It updates five members of the structure i.e., tx_busy, rx_busy, tx_underflow, 
 * rx_overflow, frame_error
 ******************************************************************************/
sl_i2s_status_t sl_si91x_i2s_get_status(sl_i2s_handle_t i2s_handle)
{
  return ((sl_i2s_driver_t *)i2s_handle)->GetStatus();
}

/*******************************************************************************
 * To validate the ARM error code
 * While calling the CMSIS APIs, it returns the ARM Error codes.
 * This function converts the ARM error codes into SL error codes.
 * It takes argument as 32 bit signed integer and returns the SL error type.
 * It has a single switch statement which is mapped with the SL error code and
 * after successful conversion it breaks the switch statement.
 * If the error code is not listed, by default is SL_STATUS_FAIL.
 ******************************************************************************/
static sl_status_t convert_arm_to_sl_error_code(int32_t error)
{
  sl_status_t status;
  switch (error) {
    // Driver Errors
    case ARM_DRIVER_OK:
      status = SL_STATUS_OK;
      break;
    case ARM_DRIVER_ERROR:
      status = SL_STATUS_FAIL;
      break;
    case ARM_DRIVER_ERROR_BUSY:
      status = SL_STATUS_BUSY;
      break;
    case ARM_DRIVER_ERROR_TIMEOUT:
      status = SL_STATUS_TIMEOUT;
      break;
    case ARM_DRIVER_ERROR_UNSUPPORTED:
      status = SL_STATUS_NOT_SUPPORTED;
      break;
    case ARM_DRIVER_ERROR_PARAMETER:
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    case ARM_DRIVER_ERROR_SPECIFIC:
      status = SL_STATUS_FAIL;
      break;
    // I2S Errors
    case ARM_SAI_ERROR_SYNCHRONIZATION:
      status = SL_STATUS_ABORT;
      break;
    case ARM_SAI_ERROR_DATA_SIZE:
      status = SL_STATUS_INVALID_RANGE;
      break;
    case ARM_SAI_ERROR_BIT_ORDER:
      status = SL_STATUS_INVALID_RANGE;
      break;
    default:
      status = SL_STATUS_NOT_SUPPORTED;
      break;
  }
  return status;
}

/*******************************************************************************
 * To validate the RSI error code
 * While calling the RSI APIs, it returns the RSI Error codes.
 * This function converts the RSI error codes into SL error codes.
 * It takes argument as RSI error type and returns the SL error type.
 * It has a single switch statement which is mapped with the SL error code and
 * after successful conversion it breaks the switch statement.
 * If the error code is not listed, by default is SL_STATUS_FAIL.
 ******************************************************************************/
static sl_status_t convert_rsi_to_sl_error_code(int32_t error)
{
  sl_status_t status;
  switch (error) {
    case RSI_OK:
      status = SL_STATUS_OK;
      break;
    case INVALID_PARAMETERS:
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    case ERROR_INVALID_INPUT_FREQUENCY:
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    case ERROR_CLOCK_NOT_ENABLED:
      status = SL_STATUS_NOT_INITIALIZED;
      break;
    default:
      status = SL_STATUS_FAIL;
      break;
  }
  return status;
}

/*******************************************************************************
 * Static callback function for handling the i2s0 callback
 ******************************************************************************/
static void i2s0_callback_event_handler(uint32_t event)
{
  i2s0_user_callback(event);
}

/*******************************************************************************
 * Static callback function for handling the i2s1 callback
 ******************************************************************************/
static void i2s1_callback_event_handler(uint32_t event)
{
  i2s1_user_callback(event);
}
