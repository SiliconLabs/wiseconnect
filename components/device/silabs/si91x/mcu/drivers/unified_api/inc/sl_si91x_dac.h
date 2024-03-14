/***************************************************************************/ /**
 * @file sl_si91x_dac.h
 * @brief DAC API implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_SI91X_DAC_H_
#define SL_SI91X_DAC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_status.h"
#include "rsi_dac.h"

/***************************************************************************/ /**
 * @addtogroup DAC Digital to Analog Converter
 * @ingroup SI91X_PERIPHERAL_APIS
 * @{
 *
 ******************************************************************************/

/***************************************************************************/ /**
 * Typedef for user supplied callback function which is called when a DAC samples completes
 *
 * @param[in] event  DAC event for different interrupt.

 ******************************************************************************/
typedef void (*sl_dac_callback_t)(uint8_t event);                             // Call back function pointer

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/
/// @brief Enumeration for DAC operation mode
typedef enum {
  SL_DAC_FIFO_MODE,                  ///< operation mode as fifo mode
  SL_DAC_STATIC_MODE,                ///< operation mode as static mode
  SL_DAC_OUTPUT_REF_VOLTAGE_FOR_ADC, ///< operation mode as dac output reference voltage for ADC
  SL_DAC_OPERATION_MODE_LAST,        ///< Last member of enum for validation
} sl_dac_operation_mode_t;

/// @brief Enumeration for DAC callback events
typedef enum {
  SL_DAC_FIFO_MODE_EVENT = 1, ///< Callback event for FIFO mode.
  SL_DAC_STATIC_MODE_EVENT,   ///< Callback event for Static mode
} sl_dac_callback_event_t;

/// @brief Enumeration for ADC channel this enum only used on Reference voltage for ADC mode of DAC.
typedef enum {
  SL_DAC_ADC_CHANNEL_0,  ///< ADC channel 0
  SL_DAC_ADC_CHANNEL_1,  ///< ADC channel 1
  SL_DAC_ADC_CHANNEL_2,  ///< ADC channel 2
  SL_DAC_ADC_CHANNEL_3,  ///< ADC channel 3
  SL_DAC_ADC_CHANNEL_4,  ///< ADC channel 4
  SL_DAC_ADC_CHANNEL_5,  ///< ADC channel 5
  SL_DAC_ADC_CHANNEL_6,  ///< ADC channel 6
  SL_DAC_ADC_CHANNEL_7,  ///< ADC channel 7
  SL_DAC_ADC_CHANNEL_8,  ///< ADC channel 8
  SL_DAC_ADC_CHANNEL_9,  ///< ADC channel 9
  SL_DAC_ADC_CHANNEL_10, ///< ADC channel 10
  SL_DAC_ADC_CHANNEL_11, ///< ADC channel 11
  SL_DAC_ADC_CHANNEL_12, ///< ADC channel 12
  SL_DAC_ADC_CHANNEL_13, ///< ADC channel 13
  SL_DAC_ADC_CHANNEL_14, ///< ADC channel 14
  SL_DAC_ADC_CHANNEL_15, ///< ADC channel 15
} sl_channel_id_for_adc_t;

/// @brief Structure to hold the versions number of peripheral API
typedef struct {
  uint8_t release; ///< Release version number
  uint8_t major;   ///< sqa version number
  uint8_t minor;   ///< dev version number
} sl_dac_version_t;

/// @brief Structure to hold the clock configuration parameters
typedef struct {
  uint16_t division_factor;         ///< Division Factor
  uint32_t soc_pll_clock;           ///< SoC PLL clock frequency
  uint32_t soc_pll_reference_clock; ///< SoC PLL reference clock frequency
} sl_dac_clock_config_t;

/// @brief Structure to hold the DAC configuration parameters
typedef struct {
  uint8_t operating_mode;
  uint8_t dac_fifo_threshold;
  uint8_t adc_channel;
  uint32_t dac_sample_rate;
  uint8_t dac_pin;
  uint8_t dac_port;
} sl_dac_config_t;

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
 * Initialize the DAC.
 * This API will power_ON to DAC peripheral and set the clock for DAC controller.
 * - The DAC clock configurations are listed below:
 *   - soc_pll_clock (Frequency range from 1 MHZ to 180 MHz)
 *   - soc_pll_reference_clock (Frequency range from 15MHZ to 65 MHz)
 *   - division_factor (0 to 63).
 * Default value is recommended for DAC clock configurations.
 *
 * @param[in] dac_clock clock structure variables ( \ref sl_dac_clock_config_t)
 *
 * @return Status 0 if successful, else error code:
 *         - SL_STATUS_OK (0x0000) - Success
 *         - SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 *         - SL_STATUS_FAIL (0x0001) - The function failed
 *         - SL_STATUS_NOT_INITIALIZED (0x0011) - Clock is not initialized
 ******************************************************************************/
sl_status_t sl_si91x_dac_init(sl_dac_clock_config_t *dac_clock);

/***************************************************************************/ /**
 * Configure DAC control parameters.
 * - The configurations are listed below:
 *   - Operation mode (Static/FIFO/Reference voltage for ADC)
 *   - Sample rate (16sps to 5 Msps)
 *   - DAC FIFO Threshold value(0 to 7).
 *
 * @pre Pre-conditions:
 *      - \ref sl_si91x_dac_init
 *
 * @param[in]  dac_config DAC configuration structure variable ( \ref sl_dac_config_t)
 * @param[in]  vref_value Reference voltage.
 * @return Status 0 if successful, else error code:
 *         - SL_STATUS_OK (0x0000) - Success
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 *         - SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
 ******************************************************************************/
sl_status_t sl_si91x_dac_set_configuration(sl_dac_config_t dac_config, float vref_value);

/***************************************************************************/ /**
 * Write digital input samples to DAC. This API will write digital sample to both
 * dynamic and static mode of DAC. For static mode sample length  is '1'. 
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_dac_init
 *      - \ref sl_si91x_dac_set_configuration
 *
 * @param[in]  data Input samples buffer pointer.
 * @param[in]  length Number of samples to play in DAC
 * @return Status 0 if successful, else error code:
 *         - SL_STATUS_OK (0x0000) - Success
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 *         - SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
 ******************************************************************************/
sl_status_t sl_si91x_dac_write_data(int16_t *data, uint16_t length);

/***************************************************************************/ /**
 * Read DAC analog output data. Reads the analog output data of the DAC 
 * in dynamic and static mode, returning the last data played on DAC.
 *
 * @pre Pre-conditions:
 *      - \ref sl_si91x_dac_init
 *      - \ref sl_si91x_dac_set_configuration
 *      - \ref sl_si91x_dac_write_data
 *      - \ref sl_si91x_dac_start
 *
 * @param[out] dac_output_data DAC data will be read from DR and store in dac_output_data variable.
 * @return Status 0 for successful execution (SL_STATUS_OK)
 ******************************************************************************/
sl_status_t sl_si91x_dac_read_data(uint16_t *dac_output_data);

/***************************************************************************/ /**
 * Get the DAC sample clock.
 * This API will return the DAC sample clock.
 *
 * @pre Pre-conditions:
 *      - \ref sl_si91x_dac_init
 *      - \ref sl_si91x_dac_set_configuration
 *
 * @param[in] sample_rate Sample rate to sample input data per sec.
 * @param[out] sample_clock Sample frequency value will return in this variable.
 * @return Status 0 if successful, else error code:
 *         - SL_STATUS_OK (0x0000) - Success
 *         - SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
 ******************************************************************************/
sl_status_t sl_si91x_dac_get_achieved_sample_clock(uint32_t sample_rate, uint32_t *sample_clock);

/***************************************************************************/ /**
 * Rewrite the DAC sample data. Rewrites the input sample data in DAC FIFO operating mode.
 * The rewrite is only applicable for FIFO mode, it will rewrite the
 * sample data in DAC continuously.
 *
 * @pre Pre-conditions:
 *      - \ref sl_si91x_dac_init
 *      - \ref sl_si91x_dac_set_configuration
 *      - \ref sl_si91x_dac_write_data
 *      - \ref sl_si91x_dac_start
 *
 * @param[in]  data Input samples buffer pointer.
 * @param[in]  length Number of samples to play in DAC.
 * @return Status 0 if successful, else error code:
 *         - SL_STATUS_OK (0x0000) - Success
 *         - SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer
 *         - SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
 ******************************************************************************/
sl_status_t sl_si91x_dac_rewrite_data(int16_t *data, uint16_t length);

/***************************************************************************/ /**
 * Register the user callback function.
 * At the time of events, the function passed in the parameter is called with the respective
 * event as the parameter.
 * Before calling this function again, it is mandatory to call the \ref sl_si91x_dac_unregister_event_callback
 * function to unregister the callback, otherwise it returns SL_STATUS_BUSY error code.
 *
 * @pre Pre-conditions:
 *      - \ref sl_si91x_dac_init
 *      - \ref sl_si91x_dac_set_configuration
 *
 * @param[in] callback_event Pointer to the function which needs to be called at the time of interrupt
 *             ( \ref sl_dac_callback_t)
 * @return Status 0 if successful, else error code:
 *         - SL_STATUS_OK (0x0000) - Success
 *         - SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer
 *         - SL_STATUS_BUSY (0x0004) - Driver is busy
 ******************************************************************************/
sl_status_t sl_si91x_dac_register_event_callback(sl_dac_callback_t callback_event);

/***************************************************************************/ /**
 * Unregister the user callback function.
 * It is mandatory to call this function before registering the callback again.
 *
 * @pre Pre-condition:
 *      - \ref sl_si91x_dac_register_event_callback
 *
 * @param[in] none
 * @return Status 0 for successful execution (SL_STATUS_OK)
 ******************************************************************************/
sl_status_t sl_si91x_dac_unregister_event_callback(void);

/***************************************************************************/ /**
 * Start the DAC operation. It will enable signal to DAC, enable path to
 * DAC controller, enable the DAC FIFO config and enable dynamic.
 *
 * @pre Pre-conditions:
 *      - \ref sl_si91x_dac_init
 *      - \ref sl_si91x_dac_set_configuration
 *      - \ref sl_si91x_dac_register_event_callback
 *      - \ref sl_si91x_dac_write_data
 *
 * @return Status 0 for successful execution (SL_STATUS_OK)
 ******************************************************************************/
sl_status_t sl_si91x_dac_start(void);

/***************************************************************************/ /**
 * Stop the DAC operation. It will stop signal to DAC, disconnect path to
 * DAC controller and disable the DAC FIFO config.
 *
 * @pre Pre-conditions:
 *      - \ref sl_si91x_dac_init
 *      - \ref sl_si91x_dac_set_configuration
 *      - \ref sl_si91x_dac_register_event_callback
 *      - \ref sl_si91x_dac_write_data
 *      - \ref sl_si91x_dac_start
 *
 * @return Status 0 for successful execution (SL_STATUS_OK)
 ******************************************************************************/
sl_status_t sl_si91x_dac_stop(void);

/***************************************************************************/ /**
 * De-initialize the DAC. It will shutdown/power_off the power of DAC.
 *
 * @pre Pre-condition:
 *      - \ref sl_si91x_dac_init
 *
 * @return Status 0 for successful execution (SL_STATUS_OK)
 ******************************************************************************/
sl_status_t sl_si91x_dac_deinit(void);

/***************************************************************************/ /**
 * Get the DAC version. Returns the release, SQA, and dev versions of the DAC.
 *
 * @param[in] none
 * @return (sl_dac_version_t) type structure
 ******************************************************************************/
sl_dac_version_t sl_si91x_dac_get_version(void);

/** @} (end addtogroup DAC) */

#ifdef __cplusplus
}
#endif

#endif /* SL_SI91X_DAC_H_ */
