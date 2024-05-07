/***************************************************************************/ /**
 * @file sl_si91x_si70xx.h
 * @brief SI70xx sensor API implementation
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
#ifndef SL_SI91X_SI70XX_H
#define SL_SI91X_SI70XX_H

#ifdef __cplusplus
extern "C" {
#endif

//// Includes
#include "sl_status.h"
#include "sl_si91x_i2c.h"

/***************************************************************************/ /**
 * @addtogroup SI70XX Si70XX Sensor
 * @ingroup SI91X_HARDWARE_DRIVER_APIS
 * @{
 *
 ******************************************************************************/
/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define SI7006_ADDR 0X40 ///< I2C device address for Si7006
#define SI7013_ADDR 0x41 ///< I2C device address for Si7013
#define SI7020_ADDR 0X40 ///< I2C device address for Si7020
#define SI7021_ADDR 0x40 ///< I2C device address for Si7021
#define I2C_BASE    I2C2 ///< I2C2 base

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/
///@brief Enum for Si70xx commands
typedef enum sl_si70xx_commands {
  SL_HUMIDITY_HM     = 0xE5, ///< Measure Relative Humidity, Hold Master Mode
  SL_HUMIDITY_NHM    = 0xF5, ///< Measure Relative Humidity, No Hold Master Mode
  SL_TEMPERATURE_HM  = 0xE3, ///< Measure Temperature, Hold Master Mode
  SL_TEMPERATURE_NHM = 0xF3, ///< Measure Temperature, No Hold Master Mode
  SL_TEMPERATURE_AH  = 0xE0, ///< Read Temperature Value from Previous RH Measurement
  SL_SI70XX_RESET    = 0xFE, ///< Si70XX Reset
  SL_W_RHT_U_REG     = 0xE6, ///< Write RH/T User Register 1
  SL_R_RHT_U_REG     = 0xE7, ///< Read RH/T User Register 1
  SL_W_HEATER_C_REG  = 0x51, ///< Write Heater Control Register
  SL_R_HEATER_C_REG  = 0x11, ///< Read Heater Control Register
  SL_EID_BYTEL1      = 0xFA, ///< Read Electronic ID 1st Byte, first part
  SL_EID_BYTEL2      = 0x0F, ///< Read Electronic ID 1st Byte, second part
  SL_EID_BYTE21      = 0xFC, ///< Read Electronic ID 2nd Byte, first part
  SL_EID_BYTE22      = 0xC9, ///< Read Electronic ID 2nd Byte, second part
  SL_FIRMWARE_REV1   = 0x84, ///< Read Firmware Revision, first part
  SL_FIRMWARE_REV2   = 0xB8  ///< Read Firmware Revision, second part
} sl_si70xx_commands_t;

///@brief Enum for Temperature, Humidity measurement
typedef enum sl_si70xx_measurement_type {
  SL_HUMIDITY,        ///< Enumerator for humidity selection
  SL_TEMPERATURE,     ///< Enumerator for temperature selection
  SL_LAST_MEASUREMENT ///< Last enum for validation
} sl_si70xx_measurement_type_t;

///@brief Enum for electronic ID
typedef enum sl_si70xx_eid_type {
  SL_EID_FIRST_BYTE,  ///< Enumerator for sending electronic ID first byte
  SL_EID_SECOND_BYTE, ///< Enumerator for sending electronic ID second byte
  SL_LAST_EID         ///< Last enum for validation
} sl_si70xx_eid_type_t;

///@brief Enum for User, Heater control registers
typedef enum sl_si70xx_registers {
  SL_RH_T_USER_REG,      ///< Enumerator for write RH/T user register selection
  SL_HEATER_CONTROL_REG, ///< Enumerator for heater control register selection
  SL_LAST_CONTROL_REG    ///< Last enum for validation
} sl_si70xx_registers_t;

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************/ /**
* Initialize the Si70xx sensor.
*  @pre Pre-conditions:
*  - \ref sl_si91x_si70xx_reset() 
*  @param[in] i2c_instance : I2C peripheral to use.
*  @param[in] addr : I2C address to probe.
*  @param[in] eid : electronic ID of type \ref sl_si70xx_eid_type_t.
*  @return The following values are returned:
*  - \ref SL_STATUS_OK on success 
*  - \ref SL_STATUS_INITIALIZATION (0x0010) - no Si70xx device present 
*  - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument 
*
******************************************************************************/
sl_status_t sl_si91x_si70xx_init(sl_i2c_instance_t i2c_instance, uint8_t addr, sl_si70xx_eid_type_t eid);

/***************************************************************************/ /**
*  Check whether an Si7006/13/20/21 is present on the I2C bus (or) not.
*  @pre Pre-conditions:
*  - \ref sl_si91x_si70xx_reset() 
*  @param[in] i2c_instance : I2C peripheral to use.
*  @param[in] addr : I2C address to probe.
*  @param[in] eid : electronic ID of type \ref sl_si70xx_eid_type_t. If SL_EID_FIRST_BYTE is selected,
*                    then EID 1st byte is considered. If SL_EID_SECOND_BYTE is selected, then
*                    EID 2nd byte is considered. For EID 1st byte, EID 2nd byte commands please
*                    look into datasheet.
*   Write device ID from SNB_3 if device responds. Pass in NULL to discard.
*   Should be 0x0D for Si7013, 0x14 for Si7020 or 0x15 for Si7021
*  @return The following values are returned:
*  - \ref SL_STATUS_OK on success 
*  - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument 
*
******************************************************************************/
sl_status_t sl_si91x_si70xx_is_present(sl_i2c_instance_t i2c_instance, uint8_t addr, sl_si70xx_eid_type_t eid);

/***************************************************************************/ /**
*  Measure relative humidity and temperature from Si7006/13/20/21 sensor.
*  @pre Pre-conditions:
*  - \ref sl_si91x_si70xx_reset() 
*  @param[in] i2c_instance : I2C peripheral to use.
*  @param[in] addr : I2C address to probe.
*  @param[out] humid_data : The relative humidity in percentage obtained after doing conversion as per formula
*     						mentioned in datasheet.
*  @param[out] temp_data : The temperature in milliCelsius.
*  @return The following values are returned:
*  - \ref SL_STATUS_OK on success 
*  - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument 
*  - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer 
*
******************************************************************************/
sl_status_t sl_si91x_si70xx_measure_rh_and_temp(sl_i2c_instance_t i2c_instance,
                                                uint8_t addr,
                                                uint32_t *humid_data,
                                                int32_t *temp_data);

/***************************************************************************/ /**
*  Read Firmware Revision from Si7006/13/20/21 sensor.
*  @pre Pre-conditions:
*  - \ref sl_si91x_si70xx_reset() 
*  @param[in] i2c_instance : I2C peripheral to use.
*  @param[in] addr : I2C address to probe.
*  @param[out] firmware_revision : Internal firmware revision.
*  @return The following values are returned:
*  - \ref SL_STATUS_OK on success 
*  - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument 
*  - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer 
*
******************************************************************************/
sl_status_t sl_si91x_si70xx_get_firmware_revision(sl_i2c_instance_t i2c_instance,
                                                  uint8_t addr,
                                                  uint8_t *firmware_revision);

/***************************************************************************/ /**
*  Reads temperature value from previous relative humidity measurement from Si7006/13/20/21 sensor.
*  @pre Pre-conditions:
*  - \ref sl_si91x_si70xx_reset() 
*  @param[in] i2c_instance : I2C peripheral to use.
*  @param[in] addr : I2C address to probe.
*  @param[out] humid_data : The relative humidity in percent (multiplied by 1000).
*  @param[out] temp_data : The temperature in milliCelsius.
*  @return The following values are returned:
*  - \ref SL_STATUS_OK on success 
*  - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument 
*  - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer 
*
******************************************************************************/
sl_status_t sl_si91x_si70xx_read_temp_from_rh(sl_i2c_instance_t i2c_instance,
                                              uint8_t addr,
                                              uint32_t *humid_data,
                                              int32_t *temp_data);

/***************************************************************************/ /**
*  Start a no hold measurement of relative humidity (or) temperature from Si7006/13/20/21 sensor.
*  @pre Pre-conditions:
*  - \ref sl_si91x_si70xx_reset() 
*  @param[in] i2c_instance : I2C peripheral to use.
*  @param[in] addr : I2C address to probe.
*  @param[in] type : measurement value of type \ref sl_si70xx_measurement_type_t.
*  @param[out] data : The data read from the sensor.
*  @return The following values are returned:
*  - \ref SL_STATUS_OK on success 
*  - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument 
*
******************************************************************************/
sl_status_t sl_si91x_si70xx_start_no_hold_measure_rh_or_temp(sl_i2c_instance_t i2c_instance,
                                                             uint8_t addr,
                                                             sl_si70xx_measurement_type_t type,
                                                             uint32_t *data);

/***************************************************************************/ /**
*  Measure relative humidity from Si7006/13/20/21 sensor.
*  @pre Pre-conditions:
*  - \ref sl_si91x_si70xx_reset() 
*  @param[in] i2c_instance : I2C peripheral to use.
*  @param[in] addr : I2C address to probe.
*  @param[out] humid_data : The relative humidity measurement.
*  @return The following values are returned:
*  - \ref SL_STATUS_OK on success 
*  - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument 
*  - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer 
*
******************************************************************************/
sl_status_t sl_si91x_si70xx_measure_humidity(sl_i2c_instance_t i2c_instance, uint8_t addr, uint32_t *humid_data);

/***************************************************************************/ /**
*  Measure temperature from Si7006/13/20/21 sensor.
*  @pre Pre-conditions:
*  - \ref sl_si91x_si70xx_reset() 
*  @param[in] i2c_instance : I2C peripheral to use.
*  @param[in] addr : I2C address to probe.
*  @param[out] temp_data : The temperature measurement.
*  @return The following values are returned:
*  - \ref SL_STATUS_OK on success 
*  - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument 
*  - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer 
*
******************************************************************************/
sl_status_t sl_si91x_si70xx_measure_temperature(sl_i2c_instance_t i2c_instance, uint8_t addr, int32_t *temp_data);

/***************************************************************************/ /**
*  Initiates a si70xx software reset by the appropriate command.
*  @param[in] i2c_instance : I2C peripheral to use.
*  @param[in] addr : I2C address to probe.
*  @return The following values are returned:
*  - \ref SL_STATUS_OK on success 
*  - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument 
*
******************************************************************************/
sl_status_t sl_si91x_si70xx_reset(sl_i2c_instance_t i2c_instance, uint8_t addr);

/***************************************************************************/ /**
*  Reads the user register 1 and heater control register data
*  @pre Pre-conditions:
*  - \ref sl_si91x_si70xx_reset() 
*  @param[in] i2c_instance : I2C peripheral to use.
*  @param[in] addr : I2C address to probe.
*  @param[in] reg : Register of type \ref sl_si70xx_registers_t.
*  @param[out] data : The data read from the sensor.
*  @return The following values are returned:
*  - \ref SL_STATUS_OK on success 
*  - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument 
*  - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer 
******************************************************************************/
sl_status_t sl_si91x_si70xx_read_control_register(sl_i2c_instance_t i2c_instance,
                                                  uint8_t addr,
                                                  sl_si70xx_registers_t reg,
                                                  uint8_t *data);

/***************************************************************************/ /** 
*  Writes data to user register 1 and heater control register
*  @pre Pre-conditions:
*  - \ref sl_si91x_si70xx_reset() 
*  @param[in] i2c_instance : I2C peripheral to use.
*  @param[in] addr : I2C address to probe.
*  @param[in] reg : Register of type \ref sl_si70xx_registers_t.
*  @param[out] value : The value written into the register.
*  @return The following values are returned:
*  - \ref SL_STATUS_OK on success 
*  - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument 
*
******************************************************************************/
sl_status_t sl_si91x_si70xx_write_control_register(sl_i2c_instance_t i2c_instance,
                                                   uint8_t addr,
                                                   sl_si70xx_registers_t reg,
                                                   uint8_t value);

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY !***********************
/// @addtogroup SI70XX - RHT sensor
/// @{
///
///   @details
///
///   @n @section si70xx Introduction.
///
///   The Si70xx sensor driver provides a set of functions to interact with the Si70xx sensor series over the I2C bus. It allows
///   for easy integration of the sensor, providing access to features such as temperature and humidity measurements,
///   firmware revision retrieval, and device presence detection. All Si70xx functions are called through the generic driver.
///
///   @n @section si70xx Usage
///
///   Once I2C peripheral is initialised and configured, Si70xx sensor will be ready to be use. The common functions include the following:
///
///   @li @ref sl_si91x_si70xx_init
///   @li @ref sl_si91x_si70xx_is_present
///   @li @ref sl_si91x_si70xx_measure_rh_and_temp
///   @li @ref sl_si91x_si70xx_get_firmware_revision
///   @li @ref sl_si91x_si70xx_reset
///
///   @ref sl_si91x_si70xx_init initialises the Si70xx sensor. This function is called before using the other functions.
///
///   @ref sl_si91x_si70xx_is_present can be implemented by the application if required. This function will check if an Si70xx sensor
///   is present on the I2C bus. @ref sl_si91x_si70xx_measure_rh_and_temp will measure relative humidity and temperature from the sensor.
///   @ref sl_si91x_si70xx_get_firmware_revision will read the firmware revision from the sensor. @ref sl_si91x_si70xx_reset will
///   initiate a software reset of the sensor.
///
///   Kindly refer to the Function Documentation to see the usage of all the APIs in detail.
///
/// @} end group SI70XX - RHT sensor ********************************************************/

/** @} (end addtogroup SI70XX - RHT sensor) */

#ifdef __cplusplus
}
#endif

#endif /* SL_SI91X_SI70XX_H */