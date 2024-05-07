/***************************************************************************/ /**
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

/***************************************************************************/ /**
 * @addtogroup ANALOGCOMP Analog Comparator
 * @ingroup SI91X_PERIPHERAL_APIS
 * @{ 
 * 
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Macros for analog comparator parameters
// -----------------------------------------------------------------------------
// Data Types
/***************************************************************************/ /**
 * Typedef for the function pointer of the callback function
 ******************************************************************************/
typedef void (*sl_analog_comparator_callback_t)(void);

/// @brief Enumeration to represent the analog comparator number
typedef enum {
  SL_COMPARATOR_1 = 1, ///< Analog comparator 1
  SL_COMPARATOR_2,     ///< Analog comparator 2
  SL_COMPARATOR_LAST,  ///< Last member of enum for validation
} sl_analog_comparator_number_t;

/// @brief Enumeration to represent analog comparator inputs
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

/// @brief Enumeration to represent the hysteresis control values
typedef enum {
  SL_COMPARATOR_HYSTERSIS_VALUE_0,    ///< For hysteresis control value 0
  SL_COMPARATOR_HYSTERSIS_VALUE_1,    ///< For hysteresis control value 1
  SL_COMPARATOR_HYSTERSIS_VALUE_2,    ///< For hysteresis control value 2
  SL_COMPARATOR_HYSTERSIS_VALUE_3,    ///< For hysteresis control value 3
  SL_COMPARATOR_HYSTERSIS_VALUE_LAST, ///< Last member of enum for validation
} sl_analog_comparator_hystersis_values_t;

/// @brief Enumeration to represent BOD threshold values for resistor bank output
typedef enum {
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_2_33_VOLT, ///< threshold value to get 2.33V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_2_27_VOLT, ///< threshold value to get 2.27V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_2_21_VOLT, ///< threshold value to get 2.21V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_2_15_VOLT, ///< threshold value to get 2.15V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_2_10_VOLT, ///< threshold value to get 2.10V as resistor bank
                                               ///< output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_2_05_VOLT, ///< threshold value to get 2.05V as resistor bank
                                               ///< output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_2_00_VOLT, ///< threshold value to get 2.0V as resistor bank
                                               ///< output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_95_VOLT, ///< threshold value to get 1.95V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_91_VOLT, ///< threshold value to get 1.91V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_86_VOLT, ///< threshold value to get 1.86V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_82_VOLT, ///< threshold value to get 1.82V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_79_VOLT, ///< threshold value to get 1.79V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_75_VOLT, ///< threshold value to get 1.75V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_71_VOLT, ///< threshold value to get 1.71V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_68_VOLT, ///< threshold value to get 1.68V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_64_VOLT, ///< threshold value to get 1.64V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_61_VOLT, ///< threshold value to get 1.61V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_58_VOLT, ///< threshold value to get 1.58V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_55_VOLT, ///< threshold value to get 1.55V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_53_VOLT, ///< threshold value to get 1.53V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_50_VOLT, ///< threshold value to get 1.50V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_47_VOLT, ///< threshold value to get 1.47V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_45_VOLT, ///< threshold value to get 1.45V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_42_VOLT, ///< threshold value to get 1.42V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_40_VOLT, ///< threshold value to get 1.40V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_38_VOLT, ///< threshold value to get 1.38V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_35_VOLT, ///< threshold value to get 1.35V as resistor bank
                                               ///< output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_33_VOLT, ///< threshold value to get 1.33V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_31_VOLT, ///< threshold value to get 1.31V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_29_VOLT, ///< threshold value to get 1.29V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_27_VOLT, ///< threshold value to get 1.27V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_25_VOLT, ///< threshold value to get 1.25V as resistor
                                               ///< bank output voltage
  SL_COMPARATOR_THRESHOLD_VALUE_LAST,          ///< Last member of enum for validation
} sl_analog_comparator_threshold_values_t;
/// @brief Enumeration to represent scale factor values for reference scaler output
typedef enum {
  SL_COMPARATOR_SCALE_FACTOR_VALUE_FOR_0_1_VOLT, ///< scale factor value to get 0.1V as
                                                 ///< reference scaler output voltage
  SL_COMPARATOR_SCALE_FACTOR_VALUE_FOR_0_2_VOLT, ///< scale factor value to get 0.2V as
                                                 ///< reference scaler output voltage
  SL_COMPARATOR_SCALE_FACTOR_VALUE_FOR_0_3_VOLT, ///< scale factor value to get 0.3V as
                                                 ///< reference scaler output voltage
  SL_COMPARATOR_SCALE_FACTOR_VALUE_FOR_0_4_VOLT, ///< scale factor value to get 0.4V as
                                                 ///< reference scaler output voltage
  SL_COMPARATOR_SCALE_FACTOR_VALUE_FOR_0_5_VOLT, ///< scale factor value to get 0.5V as
                                                 ///< reference scaler output voltage
  SL_COMPARATOR_SCALE_FACTOR_VALUE_FOR_0_6_VOLT, ///< scale factor value to get 0.6V as
                                                 ///< reference scaler output voltage
  SL_COMPARATOR_SCALE_FACTOR_VALUE_FOR_0_7_VOLT, ///< scale factor value to get 0.7V as
                                                 ///< reference scaler output voltage
  SL_COMPARATOR_SCALE_FACTOR_VALUE_FOR_0_8_VOLT, ///< scale factor value to get 0.8V as
                                                 ///< reference scaler output voltage
  SL_COMPARATOR_SCALE_FACTOR_VALUE_FOR_0_9_VOLT, ///< scale factor value to get 0.9V as
                                                 ///< reference scaler output voltage
  SL_COMPARATOR_SCALE_FACTOR_VALUE_FOR_1_VOLT,   ///< scale factor value to get 1V as
                                                 ///< reference scaler output voltage
  SL_COMPARATOR_SCALE_FACTOR_VALUE_FOR_1_1_VOLT, ///< scale factor value to get 1.1V as
                                                 ///< reference scaler output voltage
  SL_COMPARATOR_SCALE_FACTOR_VALUE_LAST,         ///< Last member of enum for validation
} sl_analog_comparator_scale_factor_values_t;

/// @brief Structure to hold the parameters of Analog comparator configurations
typedef struct {
  uint8_t comparator_number; ///< comparator number, \ref sl_analog_comparator_number_t for possible values
  uint8_t
    non_inverting_input;   ///< comparator non inverting input, \ref sl_analog_comparator_inputs_t for possible values
  uint8_t inverting_input; ///< comparator non inverting input, \ref sl_analog_comparator_inputs_t for possible values
  uint8_t hystersis_value; ///< comparator hystersis control value \ref sl_analog_comparator_hystersis_values_t
  boolean_t is_filter_enabled; ///< true to enable and false to disable filter for comparator
} sl_analog_comparator_config_t;

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
* @brief Initialize Analog Comparator
* @details Enables system core clock and Auxiliary clock with 32MHZ RC clock.
* Configures reference LDO voltage as 2.8v
*
* @return    None
*******************************************************************************/
void sl_si91x_analog_comparator_init(void);

/***************************************************************************/ /**
 * @brief Configure Analog Comparator
 * @details Use this function to configure analog comparator number, select non-inverting and inverting inputs, 
 * enable or disable hysteresis, and enable or disable filter for comparator outputs.
 * Also configure input pins.
 * @pre  Pre-condition: 
 *  - \ref sl_si91x_analog_comparator_init()
 * @param[in] comparator_config_ptr Pointer to analog comparator configuration structure \ref sl_analog_comparator_config_t
 * @return    Status 0 if successful, else error code:
 *            - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Analog comparator configuration structure member has invalid value.
 *            - \ref SL_STATUS_OK (0x0000) - Success, analog comparator parameters configured properly.
*******************************************************************************/
sl_status_t sl_si91x_analog_comparator_set_configurations(sl_analog_comparator_config_t *comparator_config_ptr);

/***************************************************************************/ /**
 * @brief Register Analog Comparator interrupt callback
 * @details Enables its interrupts as per comparator number
 * @pre  Pre-conditions:
 *       - \ref sl_si91x_analog_comparator_init()
 *       - \ref sl_si91x_analog_comparator_set_configurations()
 *       - \ref sl_si91x_analog_comparator_unregister_callback(), if already registered for any interrupt
 * @param[in]  comparator_number For comparator Number, see \ref sl_analog_comparator_number_t for possible values.
 * @param[in]  on_comparator_callback Callback function pointer, to be invoked when a comparator interrupt occurs.
 * @return Status 0 if successful, else error code:
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - on_comparator_callback parameter is a null pointer.
 *         - \ref SL_STATUS_BUSY (0x0004) - The callback is already registered, unregister
 *                                   previous callback before registering new one.
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - comparator_number parameter has invalid value.
 *         - \ref SL_STATUS_OK (0x0000) - Successfully registered timer timer-out callback.
*******************************************************************************/
sl_status_t sl_si91x_analog_comparator_register_callback(sl_analog_comparator_number_t comparator_number,
                                                         sl_analog_comparator_callback_t on_comparator_callback);

/***************************************************************************/ /**
 * @brief Unregister Analog Comparator interrupt callback
 * @details Disables its interrupts as per comparator number
 * @pre  Pre-conditions:
 *       - \ref sl_si91x_analog_comparator_init()
 *       - \ref sl_si91x_analog_comparator_set_configurations()
 *       - \ref sl_si91x_analog_comparator_register_callback()
 * @param[in]  comparator_number For comparator Number, see \ref sl_analog_comparator_number_t for possible values.
 * 
 * @return Status 0 if successful, else error code:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - comparator_number parameter has invalid value.
 *         - \ref SL_STATUS_OK (0x0000) - Successfully registered timer timer-out callback.
*******************************************************************************/
sl_status_t sl_si91x_analog_comparator_unregister_callback(sl_analog_comparator_number_t comp_number);

/***************************************************************************/ /**
 * @brief Configure Analog Comparator resistor bank threshold
 * @details As per the threshold value, resistor bank output voltage will be configured.
 * Use this API when any GPIO or external input needs to compare with internal input (resistor bank).
 * @pre  Pre-conditions:
 *       - \ref sl_si91x_analog_comparator_init()
 *       - \ref sl_si91x_analog_comparator_set_configurations()
 *       - \ref sl_si91x_analog_comparator_register_callback()
 * @param[in]  threshold_value For comparator resistor bank, see \ref sl_analog_comparator_threshold_values_t for possible values.
 * @return Status 0 if successful, else error code:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - threshold_value parameter has invalid value.
 *         - \ref SL_STATUS_OK (0x0000) - Successfully registered timer timer-out callback.
*******************************************************************************/
sl_status_t sl_si91x_analog_comparator_set_resistor_bank_threshold(
  sl_analog_comparator_threshold_values_t threshold_value);

/***************************************************************************/ /**
 * @brief Configure Analog Comparator reference scaler
 * @details Sets the scale factor for the analog comparator module's reference scaler.
 * As per this scale factor, reference scalar output voltage will be set.
 * Use this API when any GPIO or external input needs to compare with internal input (reference scaler).
 * @pre  Pre-conditions:
 *       - \ref sl_si91x_analog_comparator_init()
 *       - \ref sl_si91x_analog_comparator_set_configurations()
 *       - \ref sl_si91x_analog_comparator_register_callback()
 * @param[in]  scale_factor_value For comparator's reference scale, see \ref sl_analog_comparator_scale_factor_values_t for possible values.
 * @return Status 0 if successful, else error code:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - scale_factor_value parameter has invalid value.
 *         - \ref SL_STATUS_OK (0x0000) - Successfully registered timer timer-out callback.
*******************************************************************************/
sl_status_t sl_si91x_analog_comparator_set_reference_scaler_output(
  sl_analog_comparator_scale_factor_values_t scale_factor_value);

/***************************************************************************/ /**
* @brief De-Initialize Analog Comparator
* @details Disables Peripheral clock and interrupts
* @return    None
*******************************************************************************/
void sl_si91x_analog_comparator_deinit(void);

/** @} (end addtogroup Analog Comparator) */

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY! ***********************
/// @addtogroup ANALOGCOMP Analog Comparator
/// @{
///
///   @details
///
///   @n @section ANALOGCOMP_Intro Introduction
///
///   An analog comparator in a micro-controller is a peripheral that compares two analog input voltages
///   and generates a digital output based on the comparison result. It typically has two input pins
///   for comparing analog voltages and an output pin for displaying the result of the comparison.
///
///   @li Analog comparators typically work in one of two modes:
///
///   @li **Voltage Comparison Mode**: In this mode, the comparator compares the voltages across its two input pins.
///   If the voltage at the non-inverting input (+) exceeds the voltage at the inverting input (-),
///   the output becomes high and interrupt occurs. The output becomes low if the voltage at the inverting input (-) is higher.
///
///   @li If the voltage at the inverting input (-) is higher, the output becomes low.
///
///   @li  **External and Internal Voltage comparison**: This mode lets you compare the external input voltage to internal voltage
///    generated using resister bank or reference scaler.
///
///   @li The comparator's output can be used to initiate interrupts, wake up the micro-controller from low-power mode,
///   or control other system components using analog voltage levels.
///
///  @li Analog comparators are commonly employed in applications such as battery monitoring,
///   threshold detection, sensor interface, and many more where analog signals must be compared
///   and acted on digitally by the micro-controller.
///
///   @li It can compare DAC and OPAMP outputs with external or Internal voltages.
///
///   @n @section ANALOGCOMP_Config Configuration
///
///   It sets the reference LDO voltage, set the non-inverting and inverting inputs,
///   configures the pins in case of external inputs.
///   Set the value of the scaler factor, if the input is a ref scaler.
///   Set the threshold value, if the input consists of a bank of resistors.
///   Once everything is configured, we will activate interrupts and registers callback.
///
///   @li  To configure the non-inverting-input or inverting -input, hysteresis value and set filter enable or disable
///   use the structure @ref sl_analog_comparator_config_t to update the required configuration
///   and call the API @ref sl_si91x_analog_comparator_set_configurations().
///
///   @li For more information on configuring available parameters refer to the respective peripheral example readme document.
///
///   @n @section ANALOGCOMP_Usage Usage
///
///   The common Analog Comparator functions can be used after the Analog Comparator Structures are specified,
///   passing an instance of   @ref sl_analog_comparator_config_t
///   These functions will initiate and configure the Analog Comparator below, which is the flow for implementation.
///
///  1.  @ref sl_si91x_analog_comparator_init
///  2.  @ref sl_si91x_analog_comparator_set_configurations
///  3.  @ref sl_si91x_analog_comparator_set_reference_scaler_output (If input is Reference scaler)
///  4.  @ref sl_si91x_analog_comparator_set_resistor_bank_threshold ( If input is Resistor bank)
///  5.  @ref sl_si91x_analog_comparator_register_callback
///  6.  @ref sl_si91x_analog_comparator_deinit
///
/// @} end group ANALOGCOMP ********************************************************/

#ifdef __cplusplus
}
#endif

#endif /*SL_SI91X_ANALOG_COMPARATOR*/
