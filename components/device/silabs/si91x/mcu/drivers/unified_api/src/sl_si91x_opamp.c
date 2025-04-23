/******************************************************************************
* @file sl_si91x_opamp.c
* @brief OPAMP API implementation
*******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_si91x_opamp.h"
#include "clock_update.h"
#include "rsi_bod.h"
#include "rsi_rom_ulpss_clk.h"
#include "aux_reference_volt_config.h"
#include "sl_si91x_driver_gpio.h"
#include "sl_si91x_opamp_common_config.h"
#include "sl_si91x_opamp_init.h"
#include "rsi_dac.h"
/*******************************************************************************
  ***************************  LOCAL MACROS   ***********************************
  ******************************************************************************/
// Reference voltage of OPAMP, Min is 1.6V and Max is 3.3V
#define SLI_OPAMP_MIN_BATTERY 1.6f ///< Minimum battery voltage
#define SLI_OPAMP_MAX_BATTERY 3.3f ///< Maximum battery voltage

// Defines macros for OPAMP pin selections, input/output configurations, and operational modes.
#define SL_OPAMP_LP_MODE           0 // OPAMP low power enable
#define SL_OPAMP_OUTPUT_MUX_ENABLE 1 // OPAMP output mux enable
#define SL_OPAMP_OUT_MUX_SEL       0 // OPAMP Unity gain/ Non-Inv PGA
#define SL_OPAMP_INV_OUT_MUX_SEL   1 // OPAMP Inverting PGA / Hystt
#define SL_OPAMP_DYN_MODE_EN       0 // OPAMP Dynamic mode
#define SL_OPAMP_CHANNEL_NO        0 // adc channel no in dynamic mode

#define SL_OPAMP_R1_SELECT      1 // OPAMP r1 value
#define SL_OPAMP_R2_SELECT      1 // OPAMP r2 value
#define SL_OPAMP_HYST_R2_SELECT 2 // OPAMP Hyst r2 value

typedef sl_gpio_t sl_si91x_gpio_t;

/*******************************************************************************
 ***********************  LOCAL FUNCTION Definitions *************************
  ******************************************************************************/

static void sli_si91x_opamp_configure_unity_gain_features(uint8_t opamp_config_inst);
static void sli_si91x_opamp_configure_inverting_pga(uint8_t opamp_config_inst);
static void sli_si91x_opamp_configure_non_inverting_pga(uint8_t opamp_config_inst);
static void sli_si91x_opamp_configure_inverting_hyst_comp(uint8_t opamp_config_inst);
static void sli_si91x_opamp_configure_non_inverting_hyst_comp(uint8_t opamp_config_inst);
static sl_status_t get_battery_status(float *battery_status);

/*******************************************************************************
 * @brief: Initializes OPAMP peripheral
 * @details: Enables system core clock and Auxiliary clock with MHZ RC clock
 * Configures OPAMP reference voltage as 2.5V or 3.3V
 *******************************************************************************/
sl_status_t sl_si91x_opamp_init(void)
{
  sl_status_t status;
  status               = SL_STATUS_OK;
  float battery_status = 0.0f;
  float vref_value     = ((float)OPAMP_REF_VOLT / 1000); // OPAMP Reference voltage from UC change into float
  // Get battery status
  status = get_battery_status(&battery_status);
  if (status != SL_STATUS_OK) {
    return status;
  }
  // Enable Powerup of AUX
  RSI_PS_UlpssPeriPowerUp(ULPSS_PWRGATE_ULP_AUX);

  // Check if the analog power state is off (0), and if so, configure the AUX clock source
  // to use the ULP AUX MHz RC clock in static mode. This ensures the AUX clock is properly
  // set up before enabling the OPAMP peripheral.

  if (analog_get_power_state() == 0) {
    RSI_ULPSS_AuxClkConfig(ULPCLK, ENABLE_STATIC_CLK, ULP_AUX_MHZ_RC_CLK);
  }
  analog_set_power_state(OPAMP_BIT_POS, ANALOG_POWERED_ON);

  // enable ptat currents to analog peripherals
  ULP_SPI_MEM_MAP(BG_SCDC_PROG_REG_1) |= BIT(3);
  // Configure the opamp reference voltage as 2.5 V or 3.3V
  RSI_AUX_RefVoltageConfig(vref_value, battery_status);
  return status;
}
/*******************************************************************************
 * @brief: Get battery level status
 * @details: Function to get the current battery status
 *******************************************************************************/
static sl_status_t get_battery_status(float *battery_status)
{
  if (battery_status == NULL) {
    return SL_STATUS_NULL_POINTER; // Return error if pointer is NULL
  }
  // Get the battery voltage value (float) from the RSI function
  float vbatt = RSI_BOD_SoftTriggerGetBatteryStatus();

  // Check if the battery voltage is within the valid range
  if ((vbatt >= SLI_OPAMP_MIN_BATTERY) && (vbatt <= SLI_OPAMP_MAX_BATTERY)) {
    *battery_status = vbatt; // Store the calculated battery voltage in pointer
    return SL_STATUS_OK;     // Return success status
  } else {
    return SL_STATUS_INVALID_PARAMETER; // Return invalid parameter status
  }
}
/*******************************************************************************
 * @brief: Set OPAMP Pin Configuration 
 * @details: Validates input parameters and initialize OPAMP pins
 *******************************************************************************/
sl_status_t sl_si91x_opamp_pin_init(sl_opamp_pin_config_t *opamp_config_ptr)
{
  sl_status_t status;
  status = SL_STATUS_OK;
  // Validate the structure pointer
  if (opamp_config_ptr == NULL) {
    status = SL_STATUS_NULL_POINTER; // Return error if pointer is NULL
  }

  sl_si91x_gpio_pin_config_t sl_gpio_pin_vinp = { { 0, opamp_config_ptr->vin_p_input }, GPIO_INPUT };
  sl_si91x_gpio_pin_config_t sl_gpio_pin_vinn = { { 0, opamp_config_ptr->vin_n_input }, GPIO_INPUT };
  sl_si91x_gpio_pin_config_t sl_gpio_pin_out  = { { 0, opamp_config_ptr->vout_output }, GPIO_OUTPUT };

  sl_si91x_gpio_t gpio_vinp = { 0, opamp_config_ptr->vin_p_input };
  sl_si91x_gpio_t gpio_vinn = { 0, opamp_config_ptr->vin_n_input };
  sl_si91x_gpio_t gpio_out  = { 0, opamp_config_ptr->vout_output };

  sl_gpio_mode_t agpio_mode = AGPIO_MODE;
  sl_gpio_mode_t tgpio_mode = TGPIO_MODE;

  // Configure GPIO pins for OPAMP Pin config.
  status = sl_gpio_driver_init(); // Initialize the GPIO driver
  if (status != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }
  // Configure the positive input pin
  status = sl_gpio_set_configuration(sl_gpio_pin_vinp); // Set GPIO configuration for VINP
  if (status != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }
  if ((opamp_config_ptr->vin_p_input == TGPIO_PIN2) || (opamp_config_ptr->vin_p_input == TGPIO_PIN4)) {
    // Set pin mode to TGPIO mode for specific pins
    status = sl_gpio_driver_set_pin_mode(&gpio_vinn, tgpio_mode, 0);
    if (status != SL_STATUS_OK) {
      return SL_STATUS_FAIL;
    }
  } else {
    // Set pin mode to AGPIO mode for other pins
    status = sl_gpio_driver_set_pin_mode(&gpio_vinp, agpio_mode, 0);
    if (status != SL_STATUS_OK) {
      return SL_STATUS_FAIL;
    }
  }
  // Configure the negative input pin
  status = sl_gpio_set_configuration(sl_gpio_pin_vinn); // Set GPIO configuration for VINN
  if (status != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }
  if ((opamp_config_ptr->vin_n_input == TGPIO_PIN2) || (opamp_config_ptr->vin_n_input == TGPIO_PIN4)) {
    // Set pin mode to TGPIO mode for specific pins
    status = sl_gpio_driver_set_pin_mode(&gpio_vinn, tgpio_mode, 0);
    if (status != SL_STATUS_OK) {
      return SL_STATUS_FAIL;
    }
  } else {
    // Set pin mode to AGPIO mode for other pins
    status = sl_gpio_driver_set_pin_mode(&gpio_vinn, agpio_mode, 0);
    if (status != SL_STATUS_OK) {
      return SL_STATUS_FAIL;
    }
  }
  // Configure the output pin
  status = sl_gpio_set_configuration(sl_gpio_pin_out); // Set GPIO configuration for VOUT
  if (status != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }
  status = sl_si91x_gpio_driver_select_pad_driver_disable_state(sl_gpio_pin_out.port_pin.pin,
                                                                (sl_si91x_gpio_driver_disable_state_t)GPIO_HZ);
  if (status != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }                                                                                 // Set pad driver disable state
  status = sl_si91x_gpio_driver_disable_pad_receiver(sl_gpio_pin_out.port_pin.pin); // Disable pad receiver
  if (status != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }
  if ((opamp_config_ptr->vout_output == TGPIO_PIN5) || (opamp_config_ptr->vout_output == TGPIO_PIN2)) {
    // Set pin mode to TGPIO mode for specific pins
    status = sl_gpio_driver_set_pin_mode(&gpio_out, tgpio_mode, 0);
    if (status != SL_STATUS_OK) {
      return SL_STATUS_FAIL;
    }
  } else {
    // Set pin mode to AGPIO mode for other pins
    status = sl_gpio_driver_set_pin_mode(&gpio_out, agpio_mode, 0);
    if (status != SL_STATUS_OK) {
      return SL_STATUS_FAIL;
    }
  }

  return status; // Return success status
}

/*******************************************************************************
 * @brief: Configures OPAMP parameters
 * @details: Function to set OPAMP configuration
 *******************************************************************************/

sl_status_t sl_si91x_opamp_set_configuration(sl_opamp_config_t *opamp_config)
{
  sl_status_t status;
  status = SL_STATUS_OK;
  // Validate opamp number
  if ((opamp_config->opamp_number >= SL_OPAMP_LAST) || (opamp_config->features >= SL_OPAMP_CONFIGURATION_LAST)) {
    status = SL_STATUS_INVALID_PARAMETER; // Return error if invalid parameter
  }

  switch (opamp_config->features) {
    case SL_OPAMP_UNITY_GAIN_FEATURES:
      sli_si91x_opamp_configure_unity_gain_features(opamp_config->opamp_number);
      break;

    case SL_OPAMP_INVERTING_PROGRAMMABLE_GAIN_AMPLIFIER:
      sli_si91x_opamp_configure_inverting_pga(opamp_config->opamp_number);
      break;

    case SL_OPAMP_NON_INVERTING_PROGRAMMABLE_GAIN_AMPLIFIER:
      sli_si91x_opamp_configure_non_inverting_pga(opamp_config->opamp_number);
      break;

    case SL_OPAMP_INVERTING_PROGRAMMABLE_HYST_COMP:
      sli_si91x_opamp_configure_inverting_hyst_comp(opamp_config->opamp_number);
      break;

    case SL_OPAMP_NON_INVERTING_PROGRAMMABLE_HYST_COMP:
      sli_si91x_opamp_configure_non_inverting_hyst_comp(opamp_config->opamp_number);
      break;

    default:
      return SL_STATUS_INVALID_PARAMETER;
  }
  return status;
}

/*******************************************************************************
 * @brief: Configure OPAMP for Unity Gain Features
 * @details: Configures OPAMP for unity gain features based on the instance number
 *******************************************************************************/
void sli_si91x_opamp_configure_unity_gain_features(uint8_t opamp_config_inst)
{
  if (opamp_config_inst == SL_OPAMP_1) {
#ifdef SL_OPAMP1_VIN_SEL
    RSI_OPAMP1_UGB(SL_OPAMP1_VIN_SEL,
                   ENABLE,
                   SL_OPAMP_LP_MODE,
                   SL_OPAMP_OUTPUT_MUX_ENABLE,
                   SL_OPAMP_OUT_MUX_SEL,
                   SL_OPAMP_DYN_MODE_EN,
                   SL_OPAMP_CHANNEL_NO);
#endif
  } else if (opamp_config_inst == SL_OPAMP_2) {
#ifdef SL_OPAMP2_VIN_SEL
    RSI_OPAMP2_UGB(SL_OPAMP2_VIN_SEL,
                   ENABLE,
                   SL_OPAMP_LP_MODE,
                   SL_OPAMP_OUTPUT_MUX_ENABLE,
                   SL_OPAMP_DYN_MODE_EN,
                   SL_OPAMP_CHANNEL_NO);
#endif
  } else if (opamp_config_inst == SL_OPAMP_3) {
#ifdef SL_OPAMP3_VIN_SEL
    RSI_OPAMP3_UGB(SL_OPAMP3_VIN_SEL,
                   ENABLE,
                   SL_OPAMP_LP_MODE,
                   SL_OPAMP_OUTPUT_MUX_ENABLE,
                   SL_OPAMP_DYN_MODE_EN,
                   SL_OPAMP_CHANNEL_NO);
#endif
  }
}

/*******************************************************************************
 * @brief: Configure OPAMP for Inverting Programmable Gain Amplifier
 * @details: Configures OPAMP for inverting PGA based on the instance number
 *******************************************************************************/
void sli_si91x_opamp_configure_inverting_pga(uint8_t opamp_config_inst)
{
  if (opamp_config_inst == SL_OPAMP_1) {
#ifdef SL_OPAMP1_VIN_SEL
    RSI_OPAMP1_InvPGA(SL_OPAMP1_VREF_SEL,
                      SL_OPAMP1_VIN_SEL,
                      ENABLE,
                      SL_OPAMP_LP_MODE,
                      SL_OPAMP_R1_SELECT,
                      SL_OPAMP_R2_SELECT,
                      SL_OPAMP_OUTPUT_MUX_ENABLE,
                      SL_OPAMP_INV_OUT_MUX_SEL,
                      SL_OPAMP_DYN_MODE_EN,
                      SL_OPAMP_CHANNEL_NO);
#endif
  } else if (opamp_config_inst == SL_OPAMP_2) {
#ifdef SL_OPAMP2_VIN_SEL
    RSI_OPAMP2_InvPGA(SL_OPAMP2_VREF_SEL,
                      SL_OPAMP2_VIN_SEL,
                      ENABLE,
                      SL_OPAMP_LP_MODE,
                      SL_OPAMP_R1_SELECT,
                      SL_OPAMP_R2_SELECT,
                      SL_OPAMP_OUTPUT_MUX_ENABLE,
                      SL_OPAMP_DYN_MODE_EN,
                      SL_OPAMP_CHANNEL_NO);
#endif
  } else if (opamp_config_inst == SL_OPAMP_3) {
#ifdef SL_OPAMP3_VIN_SEL
    RSI_OPAMP3_InvPGA(SL_OPAMP3_VREF_SEL,
                      SL_OPAMP3_VIN_SEL,
                      ENABLE,
                      SL_OPAMP_LP_MODE,
                      SL_OPAMP_R1_SELECT,
                      SL_OPAMP_R2_SELECT,
                      SL_OPAMP_OUTPUT_MUX_ENABLE,
                      SL_OPAMP_DYN_MODE_EN,
                      SL_OPAMP_CHANNEL_NO);
#endif
  }
}

/*******************************************************************************
 * @brief: Configure OPAMP for Non-Inverting Programmable Gain Amplifier
 * @details: Configures OPAMP for non-inverting PGA based on the instance number
 *******************************************************************************/
void sli_si91x_opamp_configure_non_inverting_pga(uint8_t opamp_config_inst)
{
  if (opamp_config_inst == SL_OPAMP_1) {
#ifdef SL_OPAMP1_VIN_SEL
    RSI_OPAMP1_NonInvPGA(SL_OPAMP1_VIN_SEL,
                         SL_OPAMP1_VREF_SEL,
                         ENABLE,
                         SL_OPAMP_LP_MODE,
                         SL_OPAMP_R1_SELECT,
                         SL_OPAMP_R2_SELECT,
                         SL_OPAMP_OUTPUT_MUX_ENABLE,
                         SL_OPAMP_INV_OUT_MUX_SEL,
                         SL_OPAMP_DYN_MODE_EN,
                         SL_OPAMP_CHANNEL_NO);
#endif
  } else if (opamp_config_inst == SL_OPAMP_2) {
#ifdef SL_OPAMP2_VIN_SEL
    RSI_OPAMP2_NonInvPGA(SL_OPAMP2_VIN_SEL,
                         SL_OPAMP2_VREF_SEL,
                         ENABLE,
                         SL_OPAMP_LP_MODE,
                         SL_OPAMP_R1_SELECT,
                         SL_OPAMP_R2_SELECT,
                         SL_OPAMP_OUTPUT_MUX_ENABLE,
                         SL_OPAMP_DYN_MODE_EN,
                         SL_OPAMP_CHANNEL_NO);
#endif
  } else if (opamp_config_inst == SL_OPAMP_3) {
#ifdef SL_OPAMP3_VIN_SEL
    RSI_OPAMP3_NonInvPGA(SL_OPAMP3_VIN_SEL,
                         SL_OPAMP3_VREF_SEL,
                         ENABLE,
                         SL_OPAMP_LP_MODE,
                         SL_OPAMP_R1_SELECT,
                         SL_OPAMP_R2_SELECT,
                         SL_OPAMP_OUTPUT_MUX_ENABLE,
                         SL_OPAMP_DYN_MODE_EN,
                         SL_OPAMP_CHANNEL_NO);
#endif
  }
}

/*******************************************************************************
 * @brief: Configure OPAMP for Inverting Programmable Hysteresis Comparator
 * @details: Configures OPAMP for inverting hysteresis comparator based on the instance number
 *******************************************************************************/
void sli_si91x_opamp_configure_inverting_hyst_comp(uint8_t opamp_config_inst)
{
  if (opamp_config_inst == SL_OPAMP_1) {
#ifdef SL_OPAMP1_VIN_SEL
    RSI_OPAMP1_InvCMP(SL_OPAMP1_VIN_SEL,
                      SL_OPAMP1_VREF_SEL,
                      ENABLE,
                      SL_OPAMP_LP_MODE,
                      SL_OPAMP_R1_SELECT,
                      SL_OPAMP_HYST_R2_SELECT,
                      SL_OPAMP_OUTPUT_MUX_ENABLE,
                      SL_OPAMP_INV_OUT_MUX_SEL,
                      SL_OPAMP_DYN_MODE_EN,
                      SL_OPAMP_CHANNEL_NO);
#endif
  } else if (opamp_config_inst == SL_OPAMP_2) {
#ifdef SL_OPAMP2_VIN_SEL
    RSI_OPAMP2_InvCMP(SL_OPAMP2_VIN_SEL,
                      SL_OPAMP2_VREF_SEL,
                      ENABLE,
                      SL_OPAMP_LP_MODE,
                      SL_OPAMP_R1_SELECT,
                      SL_OPAMP_HYST_R2_SELECT,
                      SL_OPAMP_OUTPUT_MUX_ENABLE,
                      SL_OPAMP_DYN_MODE_EN,
                      SL_OPAMP_CHANNEL_NO);
#endif
  } else if (opamp_config_inst == SL_OPAMP_3) {
#ifdef SL_OPAMP3_VIN_SEL
    RSI_OPAMP3_InvCMP(SL_OPAMP3_VIN_SEL,
                      SL_OPAMP3_VREF_SEL,
                      ENABLE,
                      SL_OPAMP_LP_MODE,
                      SL_OPAMP_R1_SELECT,
                      SL_OPAMP_HYST_R2_SELECT,
                      SL_OPAMP_OUTPUT_MUX_ENABLE,
                      SL_OPAMP_DYN_MODE_EN,
                      SL_OPAMP_CHANNEL_NO);
#endif
  }
}

/*******************************************************************************
 * @brief: Configure OPAMP for Non-Inverting Programmable Hysteresis Comparator
 * @details: Configures OPAMP for non-inverting hysteresis comparator based on the instance number
 *******************************************************************************/
void sli_si91x_opamp_configure_non_inverting_hyst_comp(uint8_t opamp_config_inst)
{
  if (opamp_config_inst == SL_OPAMP_1) {
#ifdef SL_OPAMP1_VIN_SEL
    RSI_OPAMP1_NonInvCMP(SL_OPAMP1_VIN_SEL,
                         SL_OPAMP1_VREF_SEL,
                         ENABLE,
                         SL_OPAMP_LP_MODE,
                         SL_OPAMP_R1_SELECT,
                         SL_OPAMP_HYST_R2_SELECT,
                         SL_OPAMP_OUTPUT_MUX_ENABLE,
                         SL_OPAMP_INV_OUT_MUX_SEL,
                         SL_OPAMP_DYN_MODE_EN,
                         SL_OPAMP_CHANNEL_NO);
#endif
  } else if (opamp_config_inst == SL_OPAMP_2) {
#ifdef SL_OPAMP2_VIN_SEL
    RSI_OPAMP2_NonInvCMP(SL_OPAMP2_VIN_SEL,
                         SL_OPAMP2_VREF_SEL,
                         ENABLE,
                         SL_OPAMP_LP_MODE,
                         SL_OPAMP_R1_SELECT,
                         SL_OPAMP_HYST_R2_SELECT,
                         SL_OPAMP_OUTPUT_MUX_ENABLE,
                         SL_OPAMP_DYN_MODE_EN,
                         SL_OPAMP_CHANNEL_NO);
#endif
  } else if (opamp_config_inst == SL_OPAMP_3) {
#ifdef SL_OPAMP3_VIN_SEL
    RSI_OPAMP3_NonInvCMP(SL_OPAMP3_VIN_SEL,
                         SL_OPAMP3_VREF_SEL,
                         ENABLE,
                         SL_OPAMP_LP_MODE,
                         SL_OPAMP_R1_SELECT,
                         SL_OPAMP_HYST_R2_SELECT,
                         SL_OPAMP_OUTPUT_MUX_ENABLE,
                         SL_OPAMP_DYN_MODE_EN,
                         SL_OPAMP_CHANNEL_NO);
#endif
  }
}

/*******************************************************************************
 * @brief: De-Initializes OPAMP peripheral
 * @details: Disables Peripheral clock and powers down AUX
 *******************************************************************************/
void sl_si91x_opamp_deinit(void)
{
  analog_set_power_state(OPAMP_BIT_POS, ANALOG_POWERED_OFF);

  // Disabling ULPSS AUX clock
  if (!analog_get_power_state()) {
    RSI_PS_UlpssPeriPowerDown(ULPSS_PWRGATE_ULP_AUX);
    RSI_ULPSS_PeripheralDisable(ULPCLK, ULP_AUX_CLK);
  }
}
