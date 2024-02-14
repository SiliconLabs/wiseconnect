/***************************************************************************/ /**
 * @file sl_si91x_ro_temperature_sensor.h
 * @brief RO temperature sensor API implementation
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

#ifndef SL_SI91X_RO_TEMPERATURE_SENSOR_H_
#define SL_SI91X_RO_TEMPERATURE_SENSOR_H_

#ifdef __cplusplus
extern "C" {
#endif

//// Includes
#include "sl_status.h"
#include "rsi_temp_sensor.h"

/***************************************************************************/ /**
 * @addtogroup ROTEMPSENSOR RO Temperature Sensor
 * @ingroup SI91X_PERIPHERAL_APIS
 * @{
 *
 ******************************************************************************/
/*******************************************************************************
 ******************************   Defines / Macros   ***************************
 ******************************************************************************/

/*******************************************************************************
 ********************************   Local Variables   **************************
 ******************************************************************************/

/*******************************************************************************
 ***************************  Global  VARIABLES ********************************
 ******************************************************************************/

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/
///@brief RO temperature sensor reference clock
typedef enum {
  SL_RO_REF_CLK,  ///< Enable RO kHz clock from analog as reference clock
  SL_RO_FSM_CLK,  ///< Enable MCU FSM clock as reference clock
  SL_RO_CLK_LAST, ///< Last member of enum for validation
} sl_ro_reference_clock_t;

///@brief Enable/Disable RO temperature sensor
typedef enum {
  SL_RO_TEMPERATURE_DISABLE, ///< Temperature sensing disable
  SL_RO_TEMPERATURE_ENABLE,  ///< Temperature sensing enable
  SL_RO_TEMPERATURE_LAST,    ///< Last member of enum for validation
} sl_ro_temperature_state_t;

///@brief Enable/Disable BJT based temperature update
typedef enum {
  SL_RO_TEMP_BJT_UPDATE_ENABLE, ///< Enable BJT based temperature through RO calculation
  SL_RO_TEMP_SPI_UPDATE_ENABLE, ///< Disable BJT based temperature through RO calculation and Enable update through SPI.
  SL_RO_TEMP_BJT_UPDATE_LAST,   ///< Last member of enum for validation
} sl_ro_temperature_update_t;

///@brief Enable/Disable RO periodic temperature checking
typedef enum {
  SL_RO_TEMPERATURE_PERIODIC_DISABLE, ///< Periodic temperature checking disable
  SL_RO_TEMPERATURE_PERIODIC_ENABLE,  ///< Periodic temperature checking enable
  SL_RO_TEMPERATURE_PERIODIC_LAST,    ///< Last member of enum for validation
} sl_ro_temperature_periodic_check_t;

///@brief RO temperature periodic check time
typedef enum {
  SL_RO_TEMPERATURE_TRIGGER_1SEC, ///< RO temperature update for every 1second
  SL_RO_TEMPERATURE_TRIGGER_2SEC, ///< RO temperature update for every 2seconds
  SL_RO_TEMPERATURE_TRIGGER_4SEC, ///< RO temperature update for every 4seconds
  SL_RO_TEMPERATURE_TRIGGER_5SEC, ///< RO temperature update for every 5seconds
  SL_RO_TEMPERATURE_TRIGGER_LAST, ///< Last member of enum for validation
} sl_ro_temperature_trigger_time_t;

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************/ /**
 * @brief Set reference clock count for RO temperature sensor
 * @details This API is used to set the count of reference clock on which ptat clock counts.
 *
 * @param[in] count Count of reference clock on which ptat clock counts
 *
 * @return     Status 0 if successful, else error code:
 *             - SL_STATUS_OK on success
 *             - SL_STATUS_INVALID_PARAMETER (0x0021) , The parameter is invalid argument
 *             - SL_STATUS_NULL_POINTER (0x0022) , The parameter is null pointer
 ******************************************************************************/
sl_status_t sl_si91x_ro_temperature_set_count(uint32_t count);

/***************************************************************************/ /**
 * @brief Select reference clock for RO temperature sensor
 * @details This API is used to select the reference clock to the temperature sensor
 *
 * @param[in] ref_clk Reference clock selection:
 *                    - 0 - reference RO clock from analog
 *                    - 1 - MCU FSM clock
 *
 * @return Status 0 if successful, else error code:
 *         - SL_STATUS_OK on success \n
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) , The parameter is invalid argument
 *         - SL_STATUS_NULL_POINTER (0x0022) , The parameter is null pointer
 ******************************************************************************/
sl_status_t sl_si91x_ro_temperature_select_reference_clock(sl_ro_reference_clock_t ref_clk);

/***************************************************************************/ /**
 * @brief  Enable/disable RO temperature sensor
 * @details  This API is used to enable / disable the temperature sensor
 *
 * @param[in] state Enable / disable the temperature sensor:
 *                  - 0 - Disable
 *                  - 1 - Enable
 *
 * @return Status 0 if successful, else error code:
 *         - SL_STATUS_OK on success
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) , The parameter is invalid argument
 *         - SL_STATUS_NULL_POINTER (0x0022) , The parameter is null pointer
 ******************************************************************************/
sl_status_t sl_si91x_ro_temperature_enable(sl_ro_temperature_state_t state);

/***************************************************************************/ /**
 * @brief  Set nominal value for RO temperature sensor
 * @details  This API is used to set the nominal value of the temperature sensor
 *
 * @param[in] value Calibrated temperature value
 *
 * @return Status 0 if successful, else error code:
 *         - SL_STATUS_OK on success
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) , The parameter is invalid argument
 *         - SL_STATUS_NULL_POINTER (0x0022) , The parameter is null pointer
 ******************************************************************************/
sl_status_t sl_si91x_ro_temperature_nominal(uint32_t value);

/***************************************************************************/ /**
 * @brief  Read value from RO temperature sensor
 * @details  This API is used to read the temperature value
 *
 * @param[out] temperature Temperature value reading
 *
 * @return Status 0 if successful, else error code:
 *         - SL_STATUS_OK on success
 *         - SL_STATUS_NULL_POINTER (0x0022) , The parameter is null pointer
 ******************************************************************************/
sl_status_t sl_si91x_ro_temperature_read(int32_t *temperature);

/***************************************************************************/ /**
 * @brief  Calculate temperature from RO temperature sensor reading
 * @details  This API is used to updating temperature through RO based calculation
 *
 * @param[in] enable Enable RO based BJT temperature update
 *
 * @return Status 0 if successful, else error code:
 *         - SL_STATUS_OK on success
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) , The parameter is invalid argument
 *         - SL_STATUS_NULL_POINTER (0x0022) , The parameter is null pointer
 ******************************************************************************/
sl_status_t sl_si91x_ro_temperature_based_update(sl_ro_temperature_update_t enable);

/***************************************************************************/ /**
 * @brief  Update temperature for RO temperature sensor
 * @details  This API is used to updating temperature
 *
 * @param[in] temperature Known temperature
 *
 * @return SL_STATUS_OK on success
 ******************************************************************************/
sl_status_t sl_si91x_ro_temperature_load(uint8_t temperature);

/***************************************************************************/ /**
 * @brief  Get reference clock count for RO temperature sensor
 * @details  This API is used to read the reference clock count
 *
 * @param[out] ref_count Count of f1 clock cycles
 *
 * @return Status 0 if successful, else error code:
 *         - SL_STATUS_OK on success
 *         - SL_STATUS_NULL_POINTER (0x0022) , The parameter is null pointer
 ******************************************************************************/
sl_status_t sl_si91x_ro_temperature_get_reference_clk_count(uint32_t *ref_count);

/***************************************************************************/ /**
 * @brief  Get ptat clock count for RO temperature sensor
 * @details  This API is used to read the ptat clock count
 *
 * @param[out] ptat_count Count of f2 clock cycles
 *
 * @return Status 0 if successful, else error code:
 *         - SL_STATUS_OK on success
 *         - SL_STATUS_NULL_POINTER (0x0022) , The parameter is null pointer
 ******************************************************************************/
sl_status_t sl_si91x_ro_temperature_get_ptat_clk_count(uint32_t *ptat_count);

/***************************************************************************/ /**
 * @brief  Update temperature periodically for RO temperature sensor
 * @details  This API is used to update the temperature periodically after some time
 *
 * @param[in] periodic_check Enable periodic checking of temperature
 * @param[in] trigger_time Periodic check time in sec:
 *                         - 0 - for every 1 sec
 *                         - 1 - for every 2 secs
 *                         - 2 - for every 4 secs
 *                         - 3 - for every 5 secs
 *
 * @return Status 0 if successful, else error code:
 *         - SL_STATUS_OK on success
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) , The parameter is invalid argument
 *         - SL_STATUS_NULL_POINTER (0x0022) , The parameter is null pointer
 ******************************************************************************/
sl_status_t sl_si91x_ro_temperature_periodic_update(sl_ro_temperature_periodic_check_t periodic_check,
                                                    sl_ro_temperature_trigger_time_t trigger_time);

/** @} (end addtogroup SL_SI91X_RO_TEMPERATURE_SENSOR) */

#ifdef __cplusplus
}
#endif

#endif /* SL_SI91X_RO_TEMPERATURE_SENSOR_H_ */
