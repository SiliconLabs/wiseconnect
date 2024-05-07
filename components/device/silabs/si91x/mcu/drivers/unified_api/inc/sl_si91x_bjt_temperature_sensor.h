/***************************************************************************/ /**
* @file sl_si91x_bjt_temperature_sensor.h
* @brief BJT temperature sensor API implementation
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

#ifndef SL_SI91X_BJT_TEMPERATURE_SENSOR_H_
#define SL_SI91X_BJT_TEMPERATURE_SENSOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_si91x_adc.h"
#include "sl_status.h"

/***************************************************************************/ /**
* @addtogroup BJTTEMPSENSOR BJT Temperature Sensor
* @ingroup SI91X_PERIPHERAL_APIS
* @{
******************************************************************************/

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/
//@brief BJT temperature sensor type conversion
typedef enum {
  SL_BJT_DEGREE_CELSIUS, ///< Temperature in degree celsius
  SL_BJT_FAHRENHEIT,     ///< Temperature in fahrenheit
  SL_BJT_KELVIN,         ///< Temperature in kelvin
} sl_bjt_temperature_sensor_enum_t;

//@brief Enable/Disable BJT temperature sensor
typedef enum {
  SL_BJT_TEMPERATURE_SENSOR_DISABLE, ///< Temperature sensing disable
  SL_BJT_TEMPERATURE_SENSOR_ENABLE,  ///< Temperature sensing enable
  SL_BJT_TEMPERATURE_SENSOR_LAST,    ///< Last member of enum for validation
} sl_bjt_temperature_sensor_state_t;

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************/ /**
* @brief Initialize the BJT temperature sensor 
* @details Performs ADC initialization and configuration, sets the register callback and starts the ADC.
*
* @param[in] sl_bjt_temperature_sensor_channel_config BJT channels configuration structure channel number, sampling rate and sample length.
* @param[in] sl_bjt_temperature_sensor_config BJT configuration sets the operation mode and number of channels.
* @return  Status 0 if successful, else error code:
*          - SL_STATUS_OK (0x0000) - Success
*          - SL_STATUS_FAIL (0x0001) - Fail
*          - SL_STATUS_INVALID_PARAMETER(0x0021) - invalid parameter
*          - SL_STATUS_INVALID_RANGE(0x0028) - invalid range
*          - SL_STATUS_INVALID_COUNT(0x002B) - invalid count
******************************************************************************/
sl_status_t sl_si91x_bjt_temperature_sensor_init(adc_ch_config_t sl_bjt_temperature_sensor_channel_config,
                                                 adc_config_t sl_bjt_temperature_sensor_config);

/***************************************************************************/ /**
* @brief Read data from the BJT temperature sensor
* @pre Pre-condition:
* - \ref sl_si91x_bjt_temperature_sensor_init
* @param[out] *temp_data Temperature measurement data to convert
* @return Status 0 if successful, else error code:
*         - SL_STATUS_OK (0x0000) - Success
*         - SL_STATUS_FAIL (0x0001) - Fail
*         - SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer
*         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
*         - SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
******************************************************************************/
sl_status_t sl_si91x_bjt_temperature_sensor_read_data(double *temp_data);

/***************************************************************************/ /**
* @brief  Enable or disable the BJT temperature sensor
* @pre Pre-conditions:
* - \ref sl_si91x_bjt_temperature_sensor_init
* - \ref sl_si91x_bjt_temperature_sensor_set_channel_configuration
* @param[in] state Value indicating whether to enable or disable the BJT temperature sensor:
*                  - 1 - Enable
*                  - 0 - Disable
* @return Status 0 if successful, else error code:
*         - SL_STATUS_OK on success
*         - SL_STATUS_INVALID_PARAMETER (0x0021) , The parameter is an invalid argument
 ******************************************************************************/
sl_status_t sl_si91x_bjt_temperature_sensor_state(sl_bjt_temperature_sensor_state_t state);

/***************************************************************************/ /**
* @brief  Configure the BJT temperature sensor
* @pre Pre-condition:
* - \ref sl_si91x_bjt_temperature_sensor_init
* @param[in] sl_bjt_temperature_sensor_channel_config BJT channel configuring positive input to BJT value
* @return Status 0 if successful, else error code:
*         - SL_STATUS_OK on success
*         - SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer
*         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
*         - SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
******************************************************************************/
sl_status_t sl_si91x_bjt_temperature_sensor_set_channel_configuration(
  adc_ch_config_t sl_bjt_temperature_sensor_channel_config);

/*******************************************************************************/ /**
* @brief  Configure band gap for BJT temperature sensor
* @pre Pre-condition:
* - \ref sl_si91x_bjt_temperature_sensor_init
* @param[in] sl_bjt_temperature_sensor_channel_config Band gap channel configuring positive input to OPAMP value
* @return Status 0 if successful, else error code:
*         - SL_STATUS_OK (0x0000) - Success
*         - SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer
*         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
*         - SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
******************************************************************************/
sl_status_t sl_si91x_bjt_temperature_sensor_bg_set_channel_configuration(
  adc_ch_config_t sl_bjt_temperature_sensor_channel_config);

/******************************************************************************/ /**
* @brief  Convert the sensor reading to temperature in Celsius/Fahrenheit/Kelvin
* @pre Pre-conditions:
* - \ref sl_si91x_bjt_temperature_sensor_init
* - \ref sl_si91x_bjt_temperature_sensor_read_data
* @param[out] *temp_data Temperature measurement data to convert
* @param[in] current_temperature_mode Current temperature mode
* @return Status 0 if successful, else error code:
*         - SL_STATUS_OK (0x0000) - Success
*         - SL_STATUS_FAIL (0x0001) - Fail
*         - SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer
*         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
*****************************************************************************/
sl_status_t sl_si91x_get_bjt_temperature_sensor_conversion(double *temp_data,
                                                           sl_bjt_temperature_sensor_enum_t current_temperature_mode);

/***************************************************************************/ /**
* @brief De-initialize the BJT temperature sensor
* @pre Pre-condition:
* - \ref sl_si91x_bjt_temperature_sensor_init
* @param[in]  sl_bjt_temperature_sensor_config De-initializing the BJT configuration
* @return Status 0 if successful, else error code:
*         - SL_STATUS_OK (0x0000) - Success
*         - SL_STATUS_FAIL (0x0001) - Fail
******************************************************************************/
sl_status_t sl_si91x_bjt_temperature_sensor_deinit(adc_config_t sl_bjt_temperature_sensor_config);

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY !***********************
/// @addtogroup BJTTEMPSENSOR BJT Temperature Sensor
/// @{
///
///   @details
///
///
///   @n @section BJTTEMPSENSOR_intro Introduction
///
///   @li By taking use of its temperature-dependent characteristics, a Bipolar Junction Transistor (BJT)
///   can be utilized as a temperature sensor.
///
///   @li BJT temperature sensor peripheral will be used to transform a digital output into a temperature in degrees Celsius.
///
///   @li BJT Temperature Sensor first reads the Band-gap and then reads the BJT value after the successful calibration
///   it just computes the temperature value by taking the difference between BJT output and Band-gap output value.
///
///   @li By switching the current temperature mode, one can also obtain the temperature value in kelvin and fahrenheit.
///
///   @n @section BJTTEMPSENSOR_Config Configuration
///
///   @li By altering the macros in the mode below, one can change the temperature from
///   Celsius to Kelvin and from Celsius to Fahrenheit.
///
///   @li @ref sl_bjt_temperature_sensor_enum_t
///
///   @li By using the structured macros below, you can enable and disable the Bjt temperature sensor.
///
///   @li @ref sl_bjt_temperature_sensor_state_t
///
///   @n @section BJTTEMPSENSOR_Use Usage
///
///   The Bjt temperature sensor will first call the callback event after initializing
///   and configuring the channel settings and operating mode. It will enable, configure,
///   and read the BJT temperature value in the read data along with configuring and reading the band gap value.
///   Once the values are successfully read, the temperature in Celsius will be computed.
///
///		1. @ref sl_si91x_bjt_temperature_sensor_init
///
///		2. @ref sl_si91x_bjt_temperature_sensor_read_data
///
///		3. @ref sl_si91x_bjt_temperature_sensor_state
///
///		4. @ref sl_si91x_bjt_temperature_sensor_set_channel_configuration
///
///		5. @ref sl_si91x_bjt_temperature_sensor_bg_set_channel_configuration
///
///   @li In addition to the above mentioned apis, users can additionally modify the temperature mode
///   and deinitialize the callback event by using the conversion and deinit apis.
///
///   1. @ref sl_si91x_bjt_temperature_sensor_deinit
///
///   2. @ref sl_si91x_get_bjt_temperature_sensor_conversion
///
/** @} (end addtogroup BJTtemp) */

#ifdef __cplusplus
}
#endif

#endif /* SL_SI91X_BJT_TEMPERATURE_SENSOR_H_ */
