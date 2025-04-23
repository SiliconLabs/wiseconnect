/******************************************************************************
* @file  sl_si91x_opamp.h
* @brief OPAMP API header file
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
#ifndef SL_SI91X_OPAMP_H
#define SL_SI91X_OPAMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "si91x_device.h" // Device-specific definitions
#include "sl_status.h"    // Status codes
#include "rsi_opamp.h"    // OPAMP driver functions

// -----------------------------------------------------------------------------
// Macros for opamp parameters
// -----------------------------------------------------------------------------

/***************************************************************************/
/**
 * @brief Enumeration to represent the opamp number.
 * @details This enumeration defines the available OPAMP instances in the hardware.
 *          Each OPAMP instance corresponds to a specific hardware block.
 *
 *****************************************************************/
typedef enum {
  SL_OPAMP_1 = 1, ///< OPAMP 1 - First OPAMP instance
  SL_OPAMP_2,     ///< OPAMP 2 - Second OPAMP instance
  SL_OPAMP_3,     ///< OPAMP 3 - Third OPAMP instance
  SL_OPAMP_LAST   ///< Last member of enum for validation purposes
} sl_opamp_number_t;

/***************************************************************************/
/**
 * @brief Enumeration to represent OPAMP features.
 * @details This enumeration defines the various features and configurations supported
 *          by the OPAMP module. These features allow the OPAMP to be used in different
 *          operational modes depending on the application requirements.
 *
 *****************************************************************/
typedef enum {
  SL_OPAMP_UNITY_GAIN_FEATURES,                   ///< Unity gain - OPAMP operates as a buffer.
  SL_OPAMP_TRANS_IMPEDANCE_AMPILIER,              ///< Trans-Impedance Amplifier - Converts current to voltage.
  SL_OPAMP_INVERTING_PROGRAMMABLE_GAIN_AMPLIFIER, ///< Inverting Programmable Gain Amplifier - Amplifies signal with inversion.
  SL_OPAMP_NON_INVERTING_PROGRAMMABLE_GAIN_AMPLIFIER, ///< Non-inverting Programmable Gain Amplifier - Amplifies signal without inversion.
  SL_OPAMP_INVERTING_PROGRAMMABLE_HYST_COMP, ///< Inverting Programmable Hysteresis Comparator - Comparator with hysteresis.
  SL_OPAMP_NON_INVERTING_PROGRAMMABLE_HYST_COMP, ///< Non-inverting Programmable Hysteresis Comparator - Comparator with hysteresis.
  SL_OPAMP_INSTRUMENTATION_AMPLIFIER, ///< Instrumentation Amplifier - High precision differential amplifier.
  SL_OPAMP_CASCADED_INVERTING_PROGRAMMABLE_GAIN_AMPLIFIER, ///< Cascaded Inverting Programmable Gain Amp - Multi-stage amplification.
  SL_OPAMP_CASCADED_NON_INVERTING_PROGRAMMABLE_GAIN_AMPLIFIER, ///< Cascaded Non-inverting Programmable Gain Amp - Multi-stage amplification.
  SL_OPAMP_TWO_OPAMPS_DIFFERENTIAL_AMPLIFIER, ///< Two OPAMPs Differential Amplifier - Differential signal amplification.
  SL_OPAMP_CONFIGURATION_LAST                 ///< Last member of enum for validation purposes
} sl_opamp_feature_config_t;

/***************************************************************************/
/**
 * @brief Structure to hold the OPAMP instances and respective features.
 * @details This structure defines the configuration parameters and instances. 
 *          It is used to specify the OPAMP instance
 *          and the desired feature configuration.
 *
 *****************************************************************/
typedef struct {
  uint8_t opamp_number;               ///< OPAMP number, \ref sl_opamp_number_t for possible values.
  sl_opamp_feature_config_t features; ///< OPAMP features, \ref sl_opamp_feature_config_t for possible values.
} sl_opamp_config_t;

/***************************************************************************/
/**
 * @brief Structure to hold the OPAMP GPIOs input and output.
 * @details This structure defines the configuration parameters for the OPAMP GPIO inputs
 *          and outputs, including pin selections.
 *
 *****************************************************************/
typedef struct {
  uint8_t vin_p_input; ///< OPAMP GPIO Vinp input.
  uint8_t vin_n_input; ///< OPAMP GPIO Vinn input.
  uint8_t vout_output; ///< OPAMP GPIO Vout output.
} sl_opamp_pin_config_t;

/***************************************************************************/
/**
 * @brief Initialize the OPAMP module.
 * @details This API initializes the OPAMP module and prepares it for operation.
 *          It sets up the necessary hardware configurations and enables the OPAMP.
 *          This function must be called before using any other OPAMP-related APIs.
 * 
 * @return Status of the OPAMP initialization operation:
 *         - SL_STATUS_OK if the initialization was successful.
 *         - SL_STATUS_FAIL if the initialization failed.
 *
 * For more information on the status documentation, see 
 * [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_opamp_init(void);

/***************************************************************************/
/**
 * @brief Deinitialize the OPAMP module.
 * @details This API deinitializes the OPAMP module and releases any resources used.
 *          It disables the OPAMP and resets its configuration to default values.
 *          This function should be called to clean up resources when OPAMP is no longer needed.
 * 
 * @pre Pre-condition: 
 *      - \ref sl_si91x_opamp_init() must be called before using this function.
 * 
 * @return None.
 ******************************************************************************/
void sl_si91x_opamp_deinit(void);

/***************************************************************************/
/**
 * @brief Configure the OPAMP settings and selected GPIOs pins for input and output.
 * @details This API sets up the configuration multiplexing for the OPAMP peripheral,
 *          ensuring the correct GPIO pins are configured for OPAMP operation.
 *          It maps the OPAMP input and output pins to the appropriate hardware pins.
 * 
 * @pre Pre-condition: 
 *      - \ref sl_si91x_opamp_init() must be called before using this function.
 * 
 * @param[in] opamp_config_ptr Pointer to OPAMP configuration structure \ref sl_opamp_pin_config_t.
 * 
 * @return Status code indicating the result:
 *         - SL_STATUS_OK if the OPAMP parameters were configured properly.
 *         - SL_STATUS_INVALID_PARAMETER if the configuration structure contains invalid values.
 *
 * For more information on the status documentation, see 
 * [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_opamp_pin_init(sl_opamp_pin_config_t *opamp_config_ptr);

/***************************************************************************/
/**
 * @brief Configure the OPAMP.
 * @details This API configures the OPAMP by selecting features such as unity gain,
 *          non-inverting and inverting inputs, enabling/disabling hysteresis, and
 *          cascaded configurations. It also configures the input pins for proper
 *          operation.
 *
 * @pre Pre-condition: 
 *      - \ref sl_si91x_opamp_init() must be called before using this function.
 * 
 * @param[in] opamp_config Pointer to the OPAMP configuration structure.
 * 
 * @return Status of the OPAMP configuration operation:
 *         - SL_STATUS_OK if the configuration was successful.
 *         - SL_STATUS_INVALID_PARAMETER if the configuration structure contains invalid values.
 *
 * For more information on the status documentation, see 
 * [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_opamp_set_configuration(sl_opamp_config_t *opamp_config);

#ifdef __cplusplus
}
#endif

#endif /* SL_SI91X_OPAMP_H */
