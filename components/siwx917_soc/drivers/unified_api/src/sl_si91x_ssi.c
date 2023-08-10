/***************************************************************************/ /**
 * @file sl_si91x_ssi.c
 * @brief SSI Peripheral API implementation
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

#include "sl_si91x_ssi.h"
#include "sl_si91x_ssi_config.h"
#include "rsi_board.h"
#include "rsi_rom_clks.h"
#include "rsi_spi.h"

/*******************************************************************************
****************************  DEFINES / MACROS   ********************************
*******************************************************************************/
#define MAX_BAUDRATE        40000000 // Maximum bitrate
#define BIT_WIDTH_EIGHT     8        // Bit width 8
#define BIT_WIDTH_SIXTEEN   16       // Bit width 16
#define MANUAL_LOCK         1        // Manual Lock Enable
#define BYPASS_MANUAL_LOCK  1        // Bypass Manual Lock Enable
#define SSI_RELEASE_VERSION 0        // SSI Release version
#define SSI_SQA_VERSION     0        // SSI SQA version
#define SSI_DEV_VERSION     2        // SSI Developer version
#define MIN_BIT_WIDTH       4        // Minimum Bit width
#define MAX_SLAVE_BIT_WIDTH 16       // Maximum Slave Bit width
#define MAX_BIT_WIDTH       32       // Maximum Bit width
#define DOUBLE              2        // Double the baudrate
#define SSI_UC \
  1 /*!< SSI_UC is defined by default. when this macro (SSI_UC) is defined, peripheral
                                            configuration is directly taken from the configuration set in the universal configuration (UC).
                                            if the application requires the configuration to be changed in run-time, undefined this macro
                                            and change the peripheral configuration through the \ref sl_si91x_ssi_set_configuration API. */

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
static const void *local_ssi_handle        = NULL;
static sl_ssi_signal_event_t user_callback = NULL;
extern sl_ssi_driver_t Driver_SSI_MASTER;
extern sl_ssi_driver_t Driver_SSI_SLAVE;
extern sl_ssi_driver_t Driver_SSI_ULP_MASTER;

/*******************************************************************************
 ***************************  Local Types  ********************************
 ******************************************************************************/

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/
static sl_status_t convert_arm_to_sl_error_code(int32_t error);
static sl_status_t convert_rsi_to_sl_error_code(error_t error);
static sl_status_t get_ssi_handle(sl_ssi_instance_t instance, sl_ssi_handle_t *ssi_handle);
static sl_status_t validate_control_parameters(sl_ssi_control_config_t *control_configuration);
static sl_status_t validate_clock_parameters(sl_ssi_clock_config_t *clock_configuration);
static sl_status_t sl_si91x_ssi_slave_enable_gpio(sl_ssi_handle_t ssi_handle, boolean_t value);
static void callback_event_handler(uint32_t event);
static boolean_t validate_ssi_handle(sl_ssi_handle_t ssi_handle);

/*******************************************************************************
 **********************  Local Function Definition****************************
 ******************************************************************************/

/*******************************************************************************
***********************  Global function Definitions *************************
 ******************************************************************************/
/*******************************************************************************
 * To configure clock with input clock config structure for SSI. It validates
 * the clock frequency validate_clock_paramters function. If the clock frequency
 * is in range, it activates the clock for SSI peripheral. RSI errors are converted
 * to SL errors via convert_rsi_to_sl_error_code function.
 *
 * @param[in] clock config structure
 * @return
 * *         returns status - SL_STATUS_OK if data is set successfully,
 *           error return code such as SL_STATUS_INVALID_PARAMETER otherwise.
*******************************************************************************/
sl_status_t sl_si91x_ssi_configure_clock(sl_ssi_clock_config_t *clock_config)
{
  int32_t error_status;
  sl_status_t status;
  uint32_t baud_rate;
/* SSI_UC is defined by default. when this macro (SSI_UC) is defined, peripheral
     * configuration is directly taken from the configuration set in the universal configuration (UC).
     * if the application requires the configuration to be changed in run-time, undefined this macro
     * and change the peripheral configuration through the sl_si91x_ssi_set_configuration API.
     */
#ifdef SSI_UC
  baud_rate = ssi_configuration.baud_rate;
#endif

  do {
    if (clock_config == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // validate the clock parameters entered by user, if not in range returns error code
    status = validate_clock_parameters(clock_config);
    if (status != SL_STATUS_OK) {
      break;
    }
    // to configure the memory map pll for SSI
    SSI_SetMemoryMapPll(clock_config->intf_pll_500_control_value);
    // RSI API to set INTF pll clock is called and the status is converted to the SL error code.
    error_status = RSI_CLK_SetIntfPllFreq(M4CLK, clock_config->intf_pll_clock, clock_config->intf_pll_reference_clock);
    status       = convert_rsi_to_sl_error_code(error_status);
    if (status != SL_STATUS_OK) {
      break;
    }
    // RSI API to set M4 clock is called and the status is converted to the SL error code.
    error_status = RSI_CLK_M4SocClkConfig(M4CLK, M4_INTFPLLCLK, clock_config->division_factor);
    status       = convert_rsi_to_sl_error_code(error_status);
    if (status != SL_STATUS_OK) {
      break;
    }
    /* The frequency of the SSI master bit-rate clock is one-half the frequency of SSI master input clock.
	   * This allows the shift control logic to capture data on one clock edge of bit-rate clock and propagate data on the opposite edge.
	   */
    clock_config->soc_pll_clock = (DOUBLE * baud_rate);
    // RSI API to set SoC pll clock is called and the status is converted to the SL error code.
    RSI_CLK_SocPllLockConfig(MANUAL_LOCK, BYPASS_MANUAL_LOCK, clock_config->soc_pll_mm_count_value);
    error_status = RSI_CLK_SetSocPllFreq(M4CLK, clock_config->soc_pll_clock, clock_config->soc_pll_reference_clock);
    status       = convert_rsi_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * To initialize ssi peripheral interface. Call this function for fetching SSI
 * handle according to the instance provided.  ARM errors are converted to SL errors via
 * convert_arm_to_sl_error_code function.
 *
 * @param[in] instance of SSI
 * @param[in] ssi_handle.
 * @return    status 0 if successful, else error code.
 *
 *         SL_STATUS_OK (0x0000) - Success, otherwise fail error code
*******************************************************************************/
sl_status_t sl_si91x_ssi_init(sl_ssi_instance_t instance, sl_ssi_handle_t *ssi_handle)
{
  sl_status_t status;
  int32_t error_status;
  sl_ssi_handle_t ssi_temp_handle;
  /* SSI_UC is defined by default. when this macro (SSI_UC) is defined, peripheral
   * configuration is directly taken from the configuration set in the universal configuration (UC).
   * if the application requires the configuration to be changed in run-time, undefined this macro
   * and change the peripheral configuration through the sl_si91x_ssi_set_configuration API.
   */
#ifdef SSI_UC
  instance = ssi_configuration.device_mode;
#endif
  do {
    if (SSI_GetInitState(instance) == ENABLE) {
      status = SL_STATUS_BUSY;
      break;
    }
    status = get_ssi_handle(instance, &ssi_temp_handle);
    if (status != SL_STATUS_OK) {
      break;
    }
    if (ssi_handle == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    *ssi_handle  = ssi_temp_handle;
    error_status = ((sl_ssi_driver_t *)ssi_temp_handle)->Initialize(callback_event_handler);
    status       = convert_arm_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * To uninitialize SSI peripheral interface. ARM errors are converted
 * to SL errors via convert_arm_to_sl_error_code function.
 *
 * @param[in] ssi_handle
 * @return    status 0 if successful, else error code.
 *
 *         SL_STATUS_OK (0x0000) - Success, otherwise fail error code
*******************************************************************************/
sl_status_t sl_si91x_ssi_deinit(sl_ssi_handle_t ssi_handle)
{
  sl_status_t status;
  int32_t error_status;
  do {
    // validate passed in parameter
    if (ssi_handle == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    if (!validate_ssi_handle(ssi_handle)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    error_status = ((sl_ssi_driver_t *)ssi_handle)->Uninitialize();
    status       = convert_arm_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * To configure SSI interface power.
 *
 * @param[in] ARM power state. The following 2 modes are supported:
 * ARM_POWER_OFF
 * ARM_POWER_FULL
 * @return
 * *         returns status - SL_STAUS_OK if power was set successfully,
 *  error return code such as SL_STATUS_FAIL otherwise.
 *  ARM errors are converted to SL errors via convert_arm_to_sl_error_code function.
*******************************************************************************/
sl_status_t sl_si91x_ssi_configure_power_mode(sl_ssi_handle_t ssi_handle, sl_ssi_power_state_t state)
{
  int32_t error_status;
  sl_status_t status;
  do {
    // validates the power mode, if invalid state or invalid handle passed in
    // then returns error status
    if (state > ARM_POWER_FULL || !validate_ssi_handle(ssi_handle)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    if (ssi_handle == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    error_status = ((sl_ssi_driver_t *)ssi_handle)->PowerControl(state);
    status       = convert_arm_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * SSI peripheral control and config.
 *
 * @param[in] ssi handle
 * @param[in] SSI Control config struct with fields for various params,
 * @return
 * *         returns status - SL_STATUS_OK if config operation was
 *           fine, error return code such as SL_STATUS_FAIL otherwise.
 *           ARM errors are converted to SL errors via convert_arm_to_sl_error_code
 *           function.
*******************************************************************************/
sl_status_t sl_si91x_ssi_set_configuration(sl_ssi_handle_t ssi_handle, sl_ssi_control_config_t *control_configuration)
{
  sl_status_t status;
  int32_t error_status;
  uint32_t input_mode = 0;
  /* SSI_UC is defined by default. when this macro (SSI_UC) is defined, peripheral
   * configuration is directly taken from the configuration set in the universal configuration (UC).
   * if the application requires the configuration to be changed in run-time, undefined this macro
   * and change the peripheral configuration through the sl_si91x_ssi_set_configuration API.
   */
#ifdef SSI_UC
  control_configuration = &ssi_configuration;
#endif
  do {
    // validate parameters passed in
    if (control_configuration == NULL || ssi_handle == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // validate ssi handle and baud rate
    if (!validate_ssi_handle(ssi_handle) || control_configuration->baud_rate > MAX_BAUDRATE) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // validate control parameters
    status = validate_control_parameters(control_configuration);
    if (status != SL_STATUS_OK) {
      break;
    }
    // bitwise or various parameters such as clock mode, master mode bit width and slave select mode
    // and pass it(input_mode) into control API.
    input_mode = (control_configuration->clock_mode | ARM_SPI_DATA_BITS(control_configuration->bit_width));
    if (control_configuration->device_mode == SL_SSI_SLAVE_ACTIVE) {
      input_mode |= control_configuration->slave_ssm;
      input_mode |= control_configuration->device_mode;
    } else {
      input_mode |= control_configuration->master_ssm;
      input_mode |= SL_SSI_MASTER_ACTIVE;
    }
    error_status = ((sl_ssi_driver_t *)ssi_handle)->Control(input_mode, control_configuration->baud_rate);
    status       = convert_arm_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * SSI peripheral receive with connected slave. This makes use of half duplex
 *            reception.
 *
 * @param[in] ssi handle
 * @param[in] pointer to Rx data buffer,
 * @param[in] length of input data
 * @return
 * *         returns status - SL_STATUS_OK if received data was
 *           fine, error return code such as SL_STATUS_FAIL otherwise.
 *           ARM errors are converted to SL errors via convert_arm_to_sl_error_code
 *           function.
*******************************************************************************/
sl_status_t sl_si91x_ssi_receive_data(sl_ssi_handle_t ssi_handle, void *data, uint32_t data_length)
{
  sl_status_t status;
  int32_t error_status;
  do {
    // validate input arguments
    if (data == NULL || ssi_handle == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // validate data length and ssi handle
    if (!data_length || !validate_ssi_handle(ssi_handle)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    if (ssi_handle != &Driver_SSI_SLAVE) {
      // Assert the slave select line for the connected slave
      status = sl_si91x_ssi_slave_enable_gpio(ssi_handle, ENABLE);
      if (status != SL_STATUS_OK) {
        break;
      }
    }
    error_status = ((sl_ssi_driver_t *)ssi_handle)->Receive(data, data_length);
    status       = convert_arm_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * SSI peripheral send with connected device. This makes use of half duplex
 *          transmission.
 *
 * @param[in] ssi handle
 * @param[in] pointer to Tx data buffer
 * @param[in] length of data
 * @return   returns status - SL_STATUS_OK if data sent was
 *           fine, error return code such as SL_STATUS_FAIL otherwise.
 *           ARM errors are converted to SL errors via convert_arm_to_sl_error_code
 *           function.
*******************************************************************************/
sl_status_t sl_si91x_ssi_send_data(sl_ssi_handle_t ssi_handle, const void *data, uint32_t data_length)
{
  sl_status_t status;
  int32_t error_status;
  do {
    // validate input arguments
    if (data == NULL || ssi_handle == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    if (!data_length || !validate_ssi_handle(ssi_handle)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }

    if (ssi_handle != &Driver_SSI_SLAVE) {
      // Assert the slave select line for the connected slave
      status = sl_si91x_ssi_slave_enable_gpio(ssi_handle, ENABLE);
      if (status != SL_STATUS_OK) {
        break;
      }
    }
    error_status = ((sl_ssi_driver_t *)ssi_handle)->Send(data, data_length);
    status       = convert_arm_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * To run SSI peripheral data transfer (bi-directional full duplex) with connected device.
 *
 * @param[in] ssi handle
 * @param[in] pointer to tx buffer
 * @param[in] pointer to rx buffer
 * @param[in] data length
 * @return
 * *         returns status - SL_STATUS_OK if data was
 *           sent and received OK, error return code such as SL_STATUS_FAIL otherwise.
 *           Data is sent out from Master on the MOSI line, the slave can simultaneously
 *           send data on MISO line(full duplex). ARM errors are converted to SL errors
 *           via convert_arm_to_sl_error_code function.
*******************************************************************************/
sl_status_t sl_si91x_ssi_transfer_data(sl_ssi_handle_t ssi_handle,
                                       const void *data_out,
                                       void *data_in,
                                       uint32_t data_length)
{
  sl_status_t status;
  int32_t error_status;
  do {
    // validate input arguments
    if ((data_out == NULL) || (data_in == NULL) || ssi_handle == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    if (!validate_ssi_handle(ssi_handle) || !data_length) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    if (ssi_handle != &Driver_SSI_SLAVE) {
      // Assert the slave select line for the connected slave
      status = sl_si91x_ssi_slave_enable_gpio(ssi_handle, ENABLE);
      if (status != SL_STATUS_OK) {
        break;
      }
    }
    error_status = ((sl_ssi_driver_t *)ssi_handle)->Transfer(data_out, data_in, data_length);
    status       = convert_arm_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * Get SSI peripheral Rx transferred data count with connected device.
 *
 * @param[in] ssi handle
 * @return
 * *     returns Rx transferred data count
*******************************************************************************/
uint32_t sl_si91x_ssi_get_rx_data_count(sl_ssi_handle_t ssi_handle)
{
  return ((sl_ssi_driver_t *)ssi_handle)->GetDataCount();
}

/*******************************************************************************
 * Get SSI peripheral Tx transferred data count with connected device.
 *
 * @param[in] ssi handle
 * @return
 * *     returns Rx transferred data count
*******************************************************************************/
uint32_t sl_si91x_ssi_get_tx_data_count(sl_ssi_handle_t ssi_handle)
{
  uint32_t tx_count = 0;
  do {
    if (ssi_handle == &Driver_SSI_MASTER) {
      tx_count = SSI_GetTxCount(SPI_MASTER_MODE);
      break;
    }
    if (ssi_handle == &Driver_SSI_SLAVE) {
      tx_count = SSI_GetTxCount(SPI_SLAVE_MODE);
      break;
    }
    if (ssi_handle == &Driver_SSI_ULP_MASTER) {
      tx_count = SSI_GetTxCount(SPI_ULP_MASTER_MODE);
      break;
    }
  } while (false);
  return tx_count;
}

/*******************************************************************************
 * To get SSI status
 *
 * @param[in] SSI handle
 * @return
 * *         returns status as a bit field - busy, data lost or mode fault
*******************************************************************************/
sl_ssi_status_t sl_si91x_ssi_get_status(sl_ssi_handle_t ssi_handle)
{
  return ((sl_ssi_driver_t *)ssi_handle)->GetStatus();
}

/*******************************************************************************
 * To get the driver version
 *
 * @param[in] None
 * @return
 * *         returns driver version
*******************************************************************************/
sl_ssi_version_t sl_si91x_ssi_get_version(void)
{
  sl_ssi_version_t version_struct;
  version_struct.release = SSI_RELEASE_VERSION;
  version_struct.major   = SSI_SQA_VERSION;
  version_struct.minor   = SSI_DEV_VERSION;
  return version_struct;
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
sl_status_t sl_si91x_ssi_register_event_callback(sl_ssi_handle_t ssi_handle, sl_ssi_signal_event_t callback_event)
{
  sl_status_t status;
  do {
    // Validates the null pointer, if true returns error code
    if ((ssi_handle == NULL) || (callback_event == NULL)) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // Validate the SSI handle address, if incorrect returns error code
    if (!validate_ssi_handle(ssi_handle)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // To validate the function pointer if the parameters is not NULL then, it
    // returns an error code
    if ((user_callback != NULL) || (local_ssi_handle != NULL)) {
      status = SL_STATUS_BUSY;
      break;
    }
    // User callback address is passed to the static variable which is called at the time of
    // interrupt
    user_callback    = callback_event;
    local_ssi_handle = ssi_handle;
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
void sl_si91x_gspi_unregister_event_callback(void)
{
  // Pass the NULL value to the static variable which is called at the time of
  // interrupt.
  // It is further validated in register callback API.
  user_callback    = NULL;
  local_ssi_handle = NULL;
}

/*******************************************************************************
 * To fetch the clock division factor
 * It reads the register which holds the clock division factor
 * and return the value of in 32 bit unsigned integer.
 ******************************************************************************/
uint32_t sl_si91x_ssi_get_clock_division_factor(sl_ssi_handle_t ssi_handle)
{
  uint32_t division_factor = 0;
  do {
    if (ssi_handle == &Driver_SSI_MASTER) {
      division_factor = SSI_GetClockDivisionFactor(SPI_MASTER_MODE);
      break;
    }
    if (ssi_handle == &Driver_SSI_SLAVE) {
      division_factor = SSI_GetClockDivisionFactor(SPI_SLAVE_MODE);
      break;
    }
    if (ssi_handle == &Driver_SSI_ULP_MASTER) {
      division_factor = SSI_GetClockDivisionFactor(SPI_ULP_MASTER_MODE);
      break;
    }
  } while (false);
  return division_factor;
}

/*******************************************************************************
 * To fetch the frame length i.e., bit width
 * It reads the register which holds the frame length
 * and return the value of in 32 bit unsigned integer.
 ******************************************************************************/
uint32_t sl_si91x_ssi_get_frame_length(sl_ssi_handle_t ssi_handle)
{
  uint32_t frame_length = 0;
  do {
    if (ssi_handle == &Driver_SSI_MASTER) {
      frame_length = SSI_GetFrameLength(SPI_MASTER_MODE);
      break;
    }
    if (ssi_handle == &Driver_SSI_SLAVE) {
      frame_length = SSI_GetFrameLength(SPI_SLAVE_MODE);
      break;
    }
    if (ssi_handle == &Driver_SSI_ULP_MASTER) {
      frame_length = SSI_GetFrameLength(SPI_ULP_MASTER_MODE);
      break;
    }
  } while (false);
  return frame_length;
}

/*******************************************************************************
 * To enable/disable slave select for the connected slave.
 * This function asserts/de-asserts the slave select line for the attached slave.
 *
 * @param[in] ssi handle
 * @param[in] boolean TRUE/FALSE
 * @return
 * *         returns SL_STATUS_OK if fine, error code otherwise
*******************************************************************************/
static sl_status_t sl_si91x_ssi_slave_enable_gpio(sl_ssi_handle_t ssi_handle, boolean_t value)
{
  sl_status_t status;
  int32_t error_status;
  do {
    if (ssi_handle == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    if (!validate_ssi_handle(ssi_handle)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // Assert/De-assert the slave select line according to the value flag passed in.
    if (value == TRUE) {
      error_status = ((sl_ssi_driver_t *)ssi_handle)->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);
    } else {
      error_status = ((sl_ssi_driver_t *)ssi_handle)->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
    }
    status = convert_arm_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*==============================================*/
/**
 * @fn     void callback_event_handler(uint32_t event)
 * @brief  SSI callback handler
 * @param[in] event SSI transmit and receive events
 * @return   None
 * On event transfer complete, the slave select line
 * for the connected slave is de-asserted
 */

static void callback_event_handler(uint32_t event)
{
  switch (event) {
    case SSI_EVENT_TRANSFER_COMPLETE:
      sl_si91x_ssi_slave_enable_gpio(local_ssi_handle, DISABLE);
      break;
    case SSI_EVENT_DATA_LOST:
      //      Occurs in slave mode when data is requested/sent by master
      //      but send/receive/transfer operation has not been started
      //      and indicates that data is lost. Occurs also in master mode
      //      when driver cannot transfer data fast enough.
      break;
    case SSI_EVENT_MODE_FAULT:
      //      Occurs in master mode when Slave Select is deactivated and
      //      indicates Master Mode Fault.
      break;
    default:
      break;
  }
  user_callback(event);
}

/*******************************************************************************
 * To validate ARM error code and map to sl_status_t type errors.
 *
 * ARM errors are converted to the SL errors via this function.
 * @param[in] int32_t error code
 * @return
 * *         returns SL_STATUS type errors.
*******************************************************************************/
static sl_status_t convert_arm_to_sl_error_code(int32_t error)
{
  sl_status_t status;
  switch (error) {
    //Driver Errors
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
    //SPI Errors
    case ARM_SPI_ERROR_MODE:
      status = SL_STATUS_INVALID_MODE;
      break;
    case ARM_SPI_ERROR_FRAME_FORMAT:
      status = SL_STATUS_INVALID_TYPE;
      break;
    case ARM_SPI_ERROR_DATA_BITS:
      status = SL_STATUS_INVALID_RANGE;
      break;
    case ARM_SPI_ERROR_BIT_ORDER:
      status = SL_STATUS_INVALID_RANGE;
      break;
    case ARM_SPI_ERROR_SS_MODE:
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    default:
      status = SL_STATUS_FAIL;
      break;
  }
  return status;
}

/*******************************************************************************
 * To validate RSI error code and map to sl_status_t type errors.
 *
 * RSI errors are converted to the SL_STATUS errors via this function.
 * @param[in] int error code
 * @return
 * *         returns SL_STATUS type errors.
*******************************************************************************/
static sl_status_t convert_rsi_to_sl_error_code(error_t error)
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
 * internal function which validates the control structure passed into master
 * control function below
 *
 * @param[in] pointer to control config struct.
 * @return    status 0 if successful, else error code.
 *
 *         SL_STATUS_OK (0x0000) - Success, otherwise SL_STATUS_INVALID_PARAMETER,
 *         SL_STATUS_NULL_POINTER fail error code
*******************************************************************************/
static sl_status_t validate_control_parameters(sl_ssi_control_config_t *control_configuration)
{
  sl_status_t status = SL_STATUS_OK;
  do {
    if (control_configuration == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }

    if (control_configuration->clock_mode > SL_SSI_PERIPHERAL_MICROWIRE) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    if ((control_configuration->bit_width < MIN_BIT_WIDTH) || (control_configuration->bit_width > MAX_BIT_WIDTH)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    if (control_configuration->device_mode == SL_SSI_SLAVE_ACTIVE
        && (control_configuration->bit_width >= MAX_SLAVE_BIT_WIDTH)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    if (control_configuration->baud_rate > MAX_BAUDRATE) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    if (control_configuration->device_mode >= SL_SSI_MASTER_MODE_LAST) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    if (control_configuration->master_ssm >= SL_SSI_MASTER_SLAVE_SELECT_MODE_LAST) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    if (control_configuration->slave_ssm >= SL_SSI_SLAVE_MODE_SELECT_LAST) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
  } while (false);
  return status;
}

/*******************************************************************************
 * To validate the clock parameters and return status accordingly.
 * It takes pointer to clock configuration structure as argument.
 * If the values in clock_configuration is out of range, it returns error code
 ******************************************************************************/
static sl_status_t validate_clock_parameters(sl_ssi_clock_config_t *clock_configuration)
{
  sl_status_t status;
  do {
    // To Validate different clock frequencies with its respective maximum and
    // minimum frequencies, if not in range, returns error code.
    if ((clock_configuration->intf_pll_clock < INTF_PLL_MIN_FREQUECY)
        || (clock_configuration->intf_pll_clock > INTF_PLL_MAX_FREQUECY)
        || (clock_configuration->intf_pll_reference_clock < INTF_PLL_MIN_FREQUECY)
        || (clock_configuration->intf_pll_reference_clock > INTF_PLL_MAX_FREQUECY)
        || (clock_configuration->soc_pll_clock < SOC_PLL_MIN_FREQUECY)
        || (clock_configuration->soc_pll_clock > SOC_PLL_MAX_FREQUECY)
        || (clock_configuration->soc_pll_reference_clock < SOC_PLL_MIN_FREQUECY)
        || (clock_configuration->soc_pll_reference_clock > SOC_PLL_MAX_FREQUECY)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // To Validate the division factor, if not in range,
    // returns error code.
    if (clock_configuration->division_factor >= SOC_MAX_CLK_DIVISION_FACTOR) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // Returns SL_STATUS_OK if the parameter are appropriate
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * To get the handle of SSI structure.
 * It should be called before calling any SSI function to get the appropriate handle.
 * It updates the address of the ssi_handle passed by the user.
 ******************************************************************************/
static sl_status_t get_ssi_handle(sl_ssi_instance_t instance, sl_ssi_handle_t *ssi_handle)
{
  sl_status_t status;
  do {
    if (instance >= SL_SSI_MASTER_MODE_LAST) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    if (ssi_handle == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    if (instance == SL_SSI_MASTER_ACTIVE) {
      *ssi_handle = &Driver_SSI_MASTER;
      status      = SL_STATUS_OK;
      break;
    }
    if (instance == SL_SSI_SLAVE_ACTIVE) {
      *ssi_handle = &Driver_SSI_SLAVE;
      status      = SL_STATUS_OK;
      break;
    }
    if (instance == SL_SSI_ULP_MASTER_ACTIVE) {
      *ssi_handle = &Driver_SSI_ULP_MASTER;
      status      = SL_STATUS_OK;
    }
  } while (false);
  return status;
}

/*******************************************************************************
 * internal function which validates the ssi handle passed in
 * *
 * @param[in] ssi handle.
 * @return    status true if successful, else false.
 *
*******************************************************************************/
static boolean_t validate_ssi_handle(sl_ssi_handle_t ssi_handle)
{
  boolean_t status;
  if (ssi_handle == &Driver_SSI_MASTER || ssi_handle == &Driver_SSI_SLAVE || ssi_handle == &Driver_SSI_ULP_MASTER) {
    status = true;
  } else {
    status = false;
  }
  return status;
}
