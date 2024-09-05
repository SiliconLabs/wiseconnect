/***************************************************************************/
/**
 * @file  sl_si91x_analog_comparator.h
 * @brief Analog Comparator API implementation
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

#ifndef SL_SI91X_ANALOG_COMPARATOR_H
#define SL_SI91X_ANALOG_COMPARATOR_H

#ifdef __cplusplus
extern "C" {
#endif

//// Includes
#include "sl_status.h"
#include "base_types.h"

/***************************************************************************/
/**
 * @addtogroup ANALOGCOMP Analog Comparator
 * @ingroup SI91X_PERIPHERAL_APIS
 * @{ 
 * 
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Macros for analog comparator parameters
// -----------------------------------------------------------------------------

// Data Types
/***************************************************************************/
/**
 * @brief Typedef for the function pointer of the callback function.
 ******************************************************************************/
typedef void (*sl_analog_comparator_callback_t)(void);

/**
 * @brief Enumeration to represent the analog comparator number.
 */
typedef enum {
  SL_COMPARATOR_1 = 1, ///< Analog comparator 1
  SL_COMPARATOR_2,     ///< Analog comparator 2
  SL_COMPARATOR_LAST,  ///< Last member of enum for validation
} sl_analog_comparator_number_t;

/**
 * @brief Enumeration to represent the types of analog comparator inputs.
 */
typedef enum {
  SL_COMPARATOR_GPIO_INPUT_0,            ///< Select GPIO input for comparator non-inverting input
  SL_COMPARATOR_GPIO_INPUT_1,            ///< Select GPIO input for comparator non-inverting input
  SL_COMPARATOR_DAC_OUTPUT,              ///< Selects DAC output as comparator input
  SL_COMPARATOR_REFERENCE_BUFFER_OUTPUT, ///< Selects reference buffer output as comparator input
  SL_COMPARATOR_REFERENCE_SCALER_OUTPUT, ///< Selects reference scaler output as comparator input
  SL_COMPARATOR_RESISTOR_BANK_OUTPUT,    ///< Selects resistor bank output as comparator input
  SL_COMPARATOR_OPAMP1_OUTPUT,           ///< Selects OPAMP1 output as comparator input
  SL_COMPARATOR_OPAMP2_OUTPUT,           ///< Selects OPAMP2 output as comparator input
  SL_COMPARATOR_OPAMP3_OUTPUT,           ///< Selects OPAMP3 output as comparator input
  SL_COMPARATOR_INPUT_LAST,              ///< Last member of enum for validation
} sl_analog_comparator_inputs_t;

/**
 * @brief Enumeration to represent the hysteresis control values.
 */
typedef enum {
  SL_COMPARATOR_HYSTERSIS_VALUE_0,    ///< Hysteresis control value 0
  SL_COMPARATOR_HYSTERSIS_VALUE_1,    ///< Hysteresis control value 1
  SL_COMPARATOR_HYSTERSIS_VALUE_2,    ///< Hysteresis control value 2
  SL_COMPARATOR_HYSTERSIS_VALUE_3,    ///< Hysteresis control value 3
  SL_COMPARATOR_HYSTERSIS_VALUE_LAST, ///< Last member of enum for validation
} sl_analog_comparator_hystersis_values_t;

/***************************************************************************/
/**
 * @brief Enumeration to represent BOD threshold values for resistor bank output.
 * 
 * @note Values of the enumerator are calculated using VBAT = 3.5066 volts and these values vary from board to board.
 */
typedef enum {
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_2_33_VOLT, ///< Threshold value to get 2.33V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_2_27_VOLT, ///< Threshold value to get 2.27V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_2_21_VOLT, ///< Threshold value to get 2.21V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_2_15_VOLT, ///< Threshold value to get 2.15V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_2_10_VOLT, ///< Threshold value to get 2.10V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_2_05_VOLT, ///< Threshold value to get 2.05V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_2_00_VOLT, ///< Threshold value to get 2.00V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_95_VOLT, ///< Threshold value to get 1.95V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_91_VOLT, ///< Threshold value to get 1.91V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_86_VOLT, ///< Threshold value to get 1.86V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_82_VOLT, ///< Threshold value to get 1.82V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_79_VOLT, ///< Threshold value to get 1.79V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_75_VOLT, ///< Threshold value to get 1.75V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_71_VOLT, ///< Threshold value to get 1.71V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_68_VOLT, ///< Threshold value to get 1.68V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_64_VOLT, ///< Threshold value to get 1.64V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_61_VOLT, ///< Threshold value to get 1.61V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_58_VOLT, ///< Threshold value to get 1.58V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_55_VOLT, ///< Threshold value to get 1.55V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_53_VOLT, ///< Threshold value to get 1.53V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_50_VOLT, ///< Threshold value to get 1.50V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_47_VOLT, ///< Threshold value to get 1.47V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_45_VOLT, ///< Threshold value to get 1.45V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_42_VOLT, ///< Threshold value to get 1.42V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_40_VOLT, ///< Threshold value to get 1.40V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_38_VOLT, ///< Threshold value to get 1.38V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_35_VOLT, ///< Threshold value to get 1.35V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_33_VOLT, ///< Threshold value to get 1.33V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_31_VOLT, ///< Threshold value to get 1.31V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_29_VOLT, ///< Threshold value to get 1.29V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_27_VOLT, ///< Threshold value to get 1.27V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_25_VOLT, ///< Threshold value to get 1.25V as resistor bank output voltage.
  SL_COMPARATOR_THRESHOLD_VALUE_LAST,          ///< Last member of enum for validation.
} sl_analog_comparator_threshold_values_t;

/**
 * @brief Enumeration to represent scale factor values for reference scaler output.
 */
typedef enum {
  SL_COMPARATOR_SCALE_FACTOR_VALUE_FOR_0_1_VOLT, ///< Scale factor value to get 0.1V as reference scaler output voltage
  SL_COMPARATOR_SCALE_FACTOR_VALUE_FOR_0_2_VOLT, ///< Scale factor value to get 0.2V as reference scaler output voltage
  SL_COMPARATOR_SCALE_FACTOR_VALUE_FOR_0_3_VOLT, ///< Scale factor value to get 0.3V as reference scaler output voltage
  SL_COMPARATOR_SCALE_FACTOR_VALUE_FOR_0_4_VOLT, ///< Scale factor value to get 0.4V as reference scaler output voltage
  SL_COMPARATOR_SCALE_FACTOR_VALUE_FOR_0_5_VOLT, ///< Scale factor value to get 0.5V as reference scaler output voltage
  SL_COMPARATOR_SCALE_FACTOR_VALUE_FOR_0_6_VOLT, ///< Scale factor value to get 0.6V as reference scaler output voltage
  SL_COMPARATOR_SCALE_FACTOR_VALUE_FOR_0_7_VOLT, ///< Scale factor value to get 0.7V as reference scaler output voltage
  SL_COMPARATOR_SCALE_FACTOR_VALUE_FOR_0_8_VOLT, ///< Scale factor value to get 0.8V as reference scaler output voltage
  SL_COMPARATOR_SCALE_FACTOR_VALUE_FOR_0_9_VOLT, ///< Scale factor value to get 0.9V as reference scaler output voltage
  SL_COMPARATOR_SCALE_FACTOR_VALUE_FOR_1_VOLT,   ///< Scale factor value to get 1V as reference scaler output voltage
  SL_COMPARATOR_SCALE_FACTOR_VALUE_FOR_1_1_VOLT, ///< Scale factor value to get 1.1V as reference scaler output voltage
  SL_COMPARATOR_SCALE_FACTOR_VALUE_LAST,         ///< Last member of enum for validation
} sl_analog_comparator_scale_factor_values_t;

/***************************************************************************/
/**
 * @brief Structure to hold the parameters of Analog comparator configurations.
 */
typedef struct {
  uint8_t comparator_number; ///< Comparator number, \ref sl_analog_comparator_number_t for possible values.
  uint8_t
    non_inverting_input;   ///< Comparator non-inverting input, \ref sl_analog_comparator_inputs_t for possible values.
  uint8_t inverting_input; ///< Comparator inverting input, \ref sl_analog_comparator_inputs_t for possible values.
  uint8_t hystersis_value; ///< Comparator hysteresis control value, \ref sl_analog_comparator_hystersis_values_t.
  boolean_t is_filter_enabled; ///< True to enable and false to disable filter for comparator.
} sl_analog_comparator_config_t;

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************/
/**
 * @brief  To initialize the Analog Comparator and configure the necessary clocks and reference voltage.
 * 
 * @details This API enables the system core clock and auxiliary clock with a 32MHz RC clock.
 *          It also configures the reference LDO voltage to 3.3V.
 *
 * @note This function must be called before using any other analog comparator functions.
 * @note The system core clock must be properly configured.
 ******************************************************************************/
void sl_si91x_analog_comparator_init(void);

/***************************************************************************/
/**
 * @brief To configure the analog comparator settings and initialize the selected comparator.
 * 
 * @details This API configures the analog comparator by selecting the comparator number, non-inverting and inverting inputs, 
 *          enabling or disabling hysteresis, and enabling or disabling the filter for comparator outputs.
 *          It also configures the input pins.
 * 
 * @pre Pre-condition: 
 *      - \ref sl_si91x_analog_comparator_init()
 * 
 * @param[in] comparator_config_ptr Pointer to analog comparator configuration structure \ref sl_analog_comparator_config_t
 * 
 * @return sl_status_t Status code indicating the result:
 *         - SL_STATUS_OK (0x0000) - Success, analog comparator parameters configured properly.
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Analog comparator configuration structure member has an invalid value.
 * 
 * For more information on the status documentation, see [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_analog_comparator_set_configurations(sl_analog_comparator_config_t *comparator_config_ptr);

/***************************************************************************/
/**
 * @brief To register the Analog Comparator interrupt callback and enable its interrupts as per comparator number.
 * 
 * @details This API registers a callback function to be invoked when a comparator interrupt occurs and enables the interrupts for the specified comparator number.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_analog_comparator_init()
 *      - \ref sl_si91x_analog_comparator_set_configurations()
 *      - \ref sl_si91x_analog_comparator_unregister_callback(), if already registered for any interrupt
 * 
 * @param[in] comparator_number For comparator number, see \ref sl_analog_comparator_number_t for possible values.
 * @param[in] on_comparator_callback Callback function pointer @ref sl_analog_comparator_callback_t, to be invoked when a comparator interrupt occurs.
 * 
 * @return sl_status_t Status code indicating the result:
 *         - SL_STATUS_OK (0x0000) - Successfully registered comparator interrupt callback.
 *         - SL_STATUS_BUSY (0x0004) - The callback is already registered. Unregister the existing callback before registering a new one.
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - @ref sl_analog_comparator_callback_t comparator_number parameter has an invalid value.
 *         - SL_STATUS_NULL_POINTER (0x0022) - @ref sl_analog_comparator_callback_t on_comparator_callback parameter is a null pointer.
 * 
 * For more information on the status documentation, see [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_analog_comparator_register_callback(sl_analog_comparator_number_t comparator_number,
                                                         sl_analog_comparator_callback_t on_comparator_callback);

/***************************************************************************/
/**
 * @brief To unregister the Analog Comparator interrupt callback and disable its interrupts as per comparator number.
 * 
 * @details This API disables the interrupts for the specified comparator number and unregisters the callback function.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_analog_comparator_init()
 *      - \ref sl_si91x_analog_comparator_set_configurations()
 *      - \ref sl_si91x_analog_comparator_register_callback()
 * 
 * @param[in] comp_number For comparator number, see \ref sl_analog_comparator_number_t for possible values.
 * 
 * @return sl_status_t Status code indicating the result:
 *         - SL_STATUS_OK (0x0000) - Successfully unregistered comparator interrupt callback.
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - comparator_number parameter has an invalid value.
 * 
 * For more information on the status documentation, see [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_analog_comparator_unregister_callback(sl_analog_comparator_number_t comp_number);

/***************************************************************************/
/**
 * @brief To configure the Analog Comparator resistor bank threshold.
 * 
 * @details This API configures the resistor bank output voltage based on the specified threshold value.
 *          Use this input when one of the comparator inputs is the resistor bank output.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_analog_comparator_init()
 *      - \ref sl_si91x_analog_comparator_set_configurations()
 *      - \ref sl_si91x_analog_comparator_register_callback()
 * 
 * @param[in] threshold_value For comparator resistor bank, see \ref sl_analog_comparator_threshold_values_t for possible values.
 * 
 * @return sl_status_t Status code indicating the result:
 *         - SL_STATUS_OK (0x0000) - Successfully configured the resistor bank threshold.
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - threshold_value parameter has an invalid value.
 * 
 * For more information on status codes, see [SL STATUS DOCUMENTATION](
 * https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_analog_comparator_set_resistor_bank_threshold(
  sl_analog_comparator_threshold_values_t threshold_value);

/***************************************************************************/
/**
 * @brief To configure the reference scaler for the Analog Comparator and set the scale factor.
 * 
 * @details This API sets the scale factor for the analog comparator module's reference scaler.
 *          As per this scale factor, the reference scaler output voltage will be set.
 *          Use this input when one of the comparator inputs is the reference scaler output.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_analog_comparator_init()
 *      - \ref sl_si91x_analog_comparator_set_configurations()
 *      - \ref sl_si91x_analog_comparator_register_callback()
 * 
 * @param[in] scale_factor_value For comparator's reference scale, see \ref sl_analog_comparator_scale_factor_values_t for possible values.
 * 
 * @return sl_status_t Status code indicating the result:
 *         - SL_STATUS_OK (0x0000) - Successfully configured the reference scaler output.
 *           For more information on the status documentation, see [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - scale_factor_value parameter has an invalid value.
 * For more information on the status documentation, see [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_analog_comparator_set_reference_scaler_output(
  sl_analog_comparator_scale_factor_values_t scale_factor_value);

/***************************************************************************/
/**
 * @brief To de-initialize the Analog Comparator.
 * 
 * @details This API de-initializes the analog comparator by disabling the peripheral clock & comparator interrupts.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_analog_comparator_init()
 ******************************************************************************/
void sl_si91x_analog_comparator_deinit(void);

/** @} end group ANALOGCOMP */

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY! ***********************
/*******************************************************************************/
/**
 *
 * @addtogroup ANALOGCOMP
 * @{
 *
 * @details
 *
 * @section ANALOGCOMP_Intro Introduction
 *
 * The Analog Comparator is a peripheral that compares two analog input voltages and triggers an interrupt when non-inverting is higher than inverting. It typically has two input pins for comparing analog voltages.
 *  
 * Analog comparators are often used in:
 * - **Battery Monitoring**: Detecting low battery conditions.
 * - **Threshold Detection**: Triggering events when signals exceed predefined thresholds.
 * - **Sensor Interface**: Comparing sensor outputs to reference voltages.
 * - **Wake-up Signals**: Bringing the micro-controller out of low-power modes.
 *
 * **Key Features**:
 * - Both comparators can take inputs from GPIOs.
 * - Each comparator has 9 different input sources, 2 of which are external GPIO pin inputs.
 * - Multiple comparison scenarios:
 *   - Compare external pin inputs.
 *   - Compare external pin input to internal voltages.
 *   - Compare internal voltages.
 * - The inputs of the two comparators can be independently programmed.
 * - Shared resources: The reference buffer, scaler, and resistor bank are shared between the comparators and can only be enabled when at least one comparator is active.
 *
 * @section ANALOGCOMP_Config Configuration
 *
 * The Analog Comparator can be configured using several parameters, including:
 * - **Non-Inverting Input Source**: Select the positive input for the comparator.
 * - **Inverting Input Source**: Select the negative input for the comparator.
 * - **Hysteresis**: Enable or disable hysteresis to prevent noise from causing rapid toggling of the output.
 * - **Filter Enable/Disable**: Control noise filtering on the input signal.
 *
 * These configurations are encapsulated in the @ref sl_analog_comparator_config_t structure and initialized using the @ref sl_si91x_analog_comparator_set_configurations API.
 *
 * For more details on configuration parameters, see the respective peripheral example readme document.
 *
 * @section ANALOGCOMP_Usage Usage
 *
 * After defining the Analog Comparator configuration structures and passing an instance of @ref sl_analog_comparator_config_t, the following functions can be used to initiate and configure the ACMP module. The typical flow for implementation is as follows:
 *
 * 1. Initialize the ACMP driver: @ref sl_si91x_analog_comparator_init
 * 2. Set comparator configurations: @ref sl_si91x_analog_comparator_set_configurations
 * 3. Set reference scaler output (if using a reference scaler): @ref sl_si91x_analog_comparator_set_reference_scaler_output
 * 4. Set resistor bank threshold (if using a resistor bank): @ref sl_si91x_analog_comparator_set_resistor_bank_threshold
 * 5. Register a callback for comparator events: @ref sl_si91x_analog_comparator_register_callback
 * 6. Deinitialize the ACMP driver: @ref sl_si91x_analog_comparator_deinit
 *
 */
/** @} end group ANALOGCOMP */

#ifdef __cplusplus
}
#endif

#endif /*SL_SI91X_ANALOG_COMPARATOR*/
