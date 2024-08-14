/***************************************************************************/ /**
 * @file sl_si91x_adc.h
 * @brief ADC API implementation
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

#ifndef SL_SI91X_ADC_H_
#define SL_SI91X_ADC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_status.h"
#include "rsi_adc.h"
/***************************************************************************/ /**
 * @addtogroup ADC Analog to Digital Converter
 * @ingroup SI91X_PERIPHERAL_APIS
 * @{
 ******************************************************************************/
// -----------------------------------------------------------------------------
// Data Types

typedef adc_ch_config_t sl_adc_channel_config_t;     ///< Renamed ADC channel configuration structure
typedef adc_config_t sl_adc_config_t;                ///< Renamed ADC configuration structure
typedef adc_inter_config_t sl_adc_internal_config_t; ///< Renamed ADC internal configuration structure

/***************************************************************************/ /**
* Typedef for user supplied callback function, which is called when ADC sample completes
* @param[in]   channel      ADC channel number
* @param[in]   event        ADC event for different interrupt.

******************************************************************************/
typedef void (*sl_adc_callback_t)(uint8_t channel, uint8_t event);

// -----------------------------------------------------------------------------
//Macros for event
#define SL_INTERNAL_DMA          INTERNAL_DMA             // Internal dma event
#define SL_ADC_STATIC_MODE_EVENT ADC_STATIC_MODE_CALLBACK // Static mode event
#define SIGN_BIT                 BIT(11)                  // Sign/magnitude bit

/// @brief Enumeration for ADC input type
typedef enum {
  SL_ADC_SINGLE_ENDED,    ///< Input type single ended
  SL_ADC_DIFFERENTIAL,    ///< Input type differential
  SL_ADC_INPUT_TYPE_LAST, ///< Last member of enum for validation
} sl_adc_input_type_typedef_t;

/// @brief Enumeration for ADC operation mode
typedef enum {
  SL_ADC_FIFO_MODE   = ADC_FIFOMODE_ENABLE,   ///< operation mode as fifo mode
  SL_ADC_STATIC_MODE = ADC_STATICMODE_ENABLE, ///< operation mode as static mode
  SL_ADC_OPERATION_MODE_LAST,                 ///< Last member of enum for validation
} sl_adc_operation_mode_typedef_t;

/// @brief Enumeration for ADC DMA type
typedef enum {
  SL_ADC_INTERNAL_DMA = INTERNAL_DMA_EN, ///< Internal DMA type
  SL_ADC_EXTERNAL_DMA = EXTERNAL_DMA_EN, ///< External DMA type
  SL_ADC_DMA_TYPE_LAST,                  ///< Last member of enum for validation
} sl_adc_dma_type_typedef_t;

/// @brief Enumeration for ADC multiple channel selection
typedef enum {
  SL_ADC_SINGLE_CHNL = DYNAMIC_MODE_DI, ///< Dynamic mode disable
  SL_ADC_MULTI_CHNL  = DYNAMIC_MODE_EN, ///< Dynamic mode enable
  SL_ADC_CHANNEL_TYPE_LAST,             ///< Last member of enum for validation
} sl_adc_channel_type_typedef_t;

/// @brief Enumeration for ADC channel.
typedef enum {
  SL_ADC_CHANNEL_1,  ///< ADC channel 1
  SL_ADC_CHANNEL_2,  ///< ADC channel 2
  SL_ADC_CHANNEL_3,  ///< ADC channel 3
  SL_ADC_CHANNEL_4,  ///< ADC channel 4
  SL_ADC_CHANNEL_5,  ///< ADC channel 5
  SL_ADC_CHANNEL_6,  ///< ADC channel 6
  SL_ADC_CHANNEL_7,  ///< ADC channel 7
  SL_ADC_CHANNEL_8,  ///< ADC channel 8
  SL_ADC_CHANNEL_9,  ///< ADC channel 9
  SL_ADC_CHANNEL_10, ///< ADC channel 10
  SL_ADC_CHANNEL_11, ///< ADC channel 11
  SL_ADC_CHANNEL_12, ///< ADC channel 12
  SL_ADC_CHANNEL_13, ///< ADC channel 13
  SL_ADC_CHANNEL_14, ///< ADC channel 14
  SL_ADC_CHANNEL_15, ///< ADC channel 15
  SL_ADC_CHANNEL_16, ///< ADC channel 16
} sl_adc_channel_id_t;

/// @brief Structure to hold the threshold detection configuration parameters
typedef struct {
  uint16_t threshold1;      ///< Threshold_1
  uint16_t threshold2;      ///< Threshold_2
  uint8_t *threshold1_cond; ///< Threshold_1 condition like Equal, Grater, Lesser
  uint8_t *threshold2_cond; ///< Threshold_2 condition like Equal, Grater, Lesser
  uint8_t range;
} sl_adc_threshold_config_t;

/// @brief Structure to hold the fifo threshold configuration parameters
typedef struct {
  uint8_t num_of_channel_en; ///< Number of channel enable
  uint8_t a_empty_threshold; ///< AEMPTY Threshold
  uint8_t a_full_threshold;  ///< AFULL Threshold
  uint8_t dma_type;          ///< Internal or External DMA
} sl_adc_fifo_thrld_config_t;

/// @brief Structure to hold the clock configuration parameters
typedef struct {
  uint16_t division_factor;         ///< Division Factor
  uint32_t soc_pll_clock;           ///< SoC PLL clock frequency
  uint32_t soc_pll_reference_clock; ///< SoC PLL reference clock frequency
} sl_adc_clock_config_t;

/// @brief Structure to hold the versions number of peripheral API
typedef struct {
  uint8_t release; ///< Release version number
  uint8_t major;   ///< sqa version number
  uint8_t minor;   ///< dev version number
} sl_adc_version_t;

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************/ /**
 * @brief Set the clock for the ADC peripheral and configure the PLL 
 * clock to the value specified by the user in the clock configuration structure.
 * @details This API will set the clock for ADC controller.
 *       - The ADC clock configurations are listed below:
 *         - soc_pll_clock (Frequency range from 1 MHZ to 180 MHz).
 *         - soc_pll_reference_clock (Frequency range from 15MHZ to 65 MHz).
 *         - division_factor (0 to 63).
 * @param[in] clock_configuration : clock structure variables  ( \ref sl_adc_clock_config_t)
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success
 *         - SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 *         - SL_STATUS_FAIL (0x0001) - The function is failed
 *         - SL_STATUS_NOT_INITIALIZED (0x0011) - Clock is not initialized
 ******************************************************************************/
sl_status_t sl_si91x_adc_configure_clock(sl_adc_clock_config_t *clock_configuration);

/***************************************************************************/ /**
 * @brief Initialize the ADC peripheral. 
 * @details The ADC clock source sets and passes the Vref value to set the reference voltage for the ADC, 
 * based on the sampling rate. Power on the ADC block and calibrate the ADC offset and gain.
 * @pre Pre-condition:
 * - \ref sl_si91x_adc_configure_clock - Only for FIFO mode on M4 state 
 * @param[in]  adc_channel_config  : ADC channels configuration structure variable
 * @param[in]  adc_config    : ADC operation configuration structure variable
 * @param[in]  vref_value    : Reference voltage(reange from 1.8v to 3.6v)
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
 *         - SL_STATUS_BUSY (0x0004) - The function is already active 
 *         - SL_STATUS_INVALID_COUNT (0x002B) - Mismatch count 
 *         - SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
 ******************************************************************************/
sl_status_t sl_si91x_adc_init(sl_adc_channel_config_t adc_channel_config, sl_adc_config_t adc_config, float vref_value);

/***************************************************************************/ /**
 * @brief Configures the ADC channel parameters.
 * @details 
 *	- The configurations are listed below:
 *		- Number of channel 
 *		- ADC operation mode (Static mode/FIFO mode)
 *		- Sample Length 
 *		- Input Type (Single ended/ differential type)
 *		- Sampling Rate (1 - 2.5 Ms/Sec).
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock 
 * - \ref sl_si91x_adc_init
 * @param[in]  adc_channel_config  : ADC channels configuration structure variable.
 * @param[in]  adc_config    : ADC operation configuration structure variable.
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
 *         - SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
 * @note if Static mode is configured, sample length should be '1'.
 ******************************************************************************/
sl_status_t sl_si91x_adc_set_channel_configuration(sl_adc_channel_config_t adc_channel_config,
                                                   sl_adc_config_t adc_config);

/***************************************************************************/ /**
 * @brief Register the user callback function.
 * @details When an event occurs, the function with the specified parameter is invoked. 
 * To unregister the callback, use the \ref sl_si91x_adc_unregister_event_callback 
 * function before invoking this function again; otherwise, the error code \ref SL_STATUS_BUSY will be returned.
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * - \ref sl_si91x_adc_init
 * - \ref sl_si91x_adc_set_channel_configuration
 * @param[in] callback_event Pointer to the function which needs to be called at the time of interrupt
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 *         - SL_STATUS_BUSY (0x0004) - Driver is busy
 ******************************************************************************/
sl_status_t sl_si91x_adc_register_event_callback(sl_adc_callback_t callback_event);

/***************************************************************************/ /**
 * @brief Un-register the user callback function.
 * @details It is mandatory to call this function before registering the callback again.
 * @pre Pre-condition:
 * - \ref sl_si91x_adc_register_event_callback 
 * @param  none
 * @return none
 ******************************************************************************/
void sl_si91x_adc_unregister_event_callback(void);

/***************************************************************************/ /**
 * @brief Configure the ADC sampling rate for the ADC channels.
 * @details It adjusts the channel offset and frequency for each channel to determine the sample rate. 
 * To meet the Nyquist sampling rate criteria, the channel swallow factor must be not less than '3'. 
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * - \ref sl_si91x_adc_init
 * - \ref sl_si91x_adc_set_channel_configuration
 * @param[in]  adc_internal_config  :  Channel offset and frequency for each channel to set sampling rate.
 * @param[in]  channel_num   :  Channel number
 * @return    sl_status_t    :  Returns 'SL_STATUS_OK' on successful execution.
 ******************************************************************************/
sl_status_t sl_si91x_adc_configure_channel_sampling_rate(sl_adc_internal_config_t adc_internal_config,
                                                         uint8_t channel_num);

/***************************************************************************/ /**
 * @brief Configure the ADC ping and pong memory location and length.
 * @details Configure the ping and pong memory locations, 
 * as well as the ping and pong memory lengths. It is only applicable for the FIFO mode 
 * of ADC operation.  FIFO mode has a dual-buffer cyclic mode that prevents data loss 
 * when the buffer is full.  In dual buffer cyclic mode, if the Ping buffer for a given 
 * channel is full, incoming sampled data is written into the Pong buffer, allowing the 
 * controller to read back samples from the Ping buffer during this period. 
 * This explains why each channel has two start addresses, two buffer lengths, and two valid signals.
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock 
 * - \ref sl_si91x_adc_init
 * - \ref sl_si91x_adc_set_channel_configuration
 * @param[in]  adc_internal_config  :  ADC internal trigger configuration structure variable.
 * @param[in]  channel_num   :  Channel number
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_configure_ping_pong_memory_address(sl_adc_internal_config_t adc_internal_config,
                                                            uint8_t channel_num);

/***************************************************************************/ /**
 * @brief Enable ping pong for the corresponding ADC channels.
 * @note This will be applicable only for FIFO mode of ADC operation.
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * - \ref sl_si91x_adc_init
 * - \ref sl_si91x_adc_set_channel_configuration
 * - \ref sl_si91x_adc_configure_ping_pong_memory_address
 * @param[in]  channel_num   :  Channel number
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_enable_ping_pong(uint8_t channel_num);

/***************************************************************************/ /**
 * @brief Disable ping pong for the corresponding ADC channels
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * - \ref sl_si91x_adc_init
 * - \ref sl_si91x_adc_set_channel_configuration
 * - \ref sl_si91x_adc_configure_ping_pong_memory_address
 * - \ref sl_si91x_adc_enable_ping_pong
 * - \ref sl_si91x_adc_start
 * @param[in]  channel_num   :  Channel number
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_disable_ping_pong(uint8_t channel_num);

/***************************************************************************/ /**
 * @brief Enable DMA for the corresponding ADC channels.
 * @note This will be applicable only for FIFO mode of ADC operation.
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * - \ref sl_si91x_adc_init
 * - \ref sl_si91x_adc_channel_enable
 * @param[in]  channel_num   :  Channel number
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_internal_per_channel_dma_enable(uint8_t channel_num);

/***************************************************************************/ /**
 * @brief Disable DMA for the corresponding ADC channels.
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * - \ref sl_si91x_adc_init
 * - \ref sl_si91x_adc_set_channel_configuration
 * - \ref sl_si91x_adc_channel_enable
 * - \ref sl_si91x_adc_internal_per_channel_dma_enable
 * - \ref sl_si91x_adc_start
 * @param[in]  channel_num   :  Channel number
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_internal_per_channel_dma_disable(uint8_t channel_num);

/***************************************************************************/ /**
 * @brief This API will configure the ADC in Static Mode.
 * @details It could be configured to choose positive and negative input channels, as well as input type.
 * @note  This will be applicable only for static mode of ADC operation.
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * - \ref sl_si91x_adc_init
 * - \ref sl_si91x_adc_set_channel_configuration
 * @param[in]  adc_channel_config   :  ADC channels configuration structure variable.
 * @param[in]  channel_num          :  Channel number
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
 *         - SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
 ******************************************************************************/
sl_status_t sl_si91x_adc_configure_static_mode(sl_adc_channel_config_t adc_channel_config, uint8_t channel_num);

/***************************************************************************/ /**
 * @brief Configure the ADC in FIFO Mode.
 * @details ADC data input will be sampled and written to the ADC FIFO in this mode.
 * @note This will be applicable only for FIFO mode of ADC operation.
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * - \ref sl_si91x_adc_init
 * - \ref sl_si91x_adc_set_channel_configuration
 * - \ref sl_si91x_adc_configure_ping_pong_memory_address
 * - \ref sl_si91x_adc_enable_ping_pong
 * - \ref sl_si91x_adc_channel_enable
 * - \ref sl_si91x_adc_internal_per_channel_dma_enable
 * @param[in]  adc_channel_config   :  ADC channels configuration structure variable.
 * @param[in]  channel_num   :  Channel number
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
 *         - SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
 ******************************************************************************/
sl_status_t sl_si91x_adc_configure_fifo_mode(sl_adc_channel_config_t adc_channel_config, uint8_t channel_num);

/***************************************************************************/ /**
 * @brief This API will enables the ADC channel.
 * @details Data will be sampled from ADC only when corresponding channel is enabled.
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock 
 * - \ref sl_si91x_adc_init
 * - \ref sl_si91x_adc_set_channel_configuration
 * @param[in]  channel_num   :  Channel number
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_channel_enable(uint8_t channel_num);

/***************************************************************************/ /**
 * @brief Disable the ADC channel.
 * @details Data sampling will be stoped when corresponding channel is disabled.
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * - \ref sl_si91x_adc_init
 * - \ref sl_si91x_adc_set_channel_configuration
 * - \ref sl_si91x_adc_channel_enable
 * - \ref sl_si91x_adc_start
 * @param[in]  channel_num   :  Channel number
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_channel_disable(uint8_t channel_num);

/***************************************************************************/ /**
 * @brief This API will Power On and off for ADC.
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock 
 * - \ref sl_si91x_adc_init 
 * @param[in]  state       :  \b ADC_POWER_ON - To powerup adc powergates,
 *                            \b ADC_POWER_OFF - To powerdown adc powergates
 *                           ( \ref POWER_STATE )
 * @return status 0 if successful,
 *         - SL_STATUS_OK (0x0000) - Success
 ******************************************************************************/
sl_status_t sl_si91x_adc_set_power_mode(POWER_STATE state);

/***************************************************************************/ /**
 * @brief Enable or Disable the Noise averaging mode to ADC.
 * @details If bypass noise averaging is enabled, the sampled data will be more accurate. 
 * If bypass the noise averaging disable would cause '1' additional clock cycle Latency.
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * - \ref sl_si91x_adc_init
 * - \ref sl_si91x_adc_set_channel_configuration
 * @param[in]  state       :  1 - To enable bypass the noise averaging mode,
 *                            0 - To disable bypass the noise averaging mode
 * @return Returns 'SL_STATUS_OK' on successful execution.
 ******************************************************************************/
sl_status_t sl_si91x_adc_set_noise_average_mode(boolean_t state);

/***************************************************************************/ /**
 * @brief This API will enable the temperature sensor. 
 * @details Enable BJT based Temperature sensor as an input to ADC. 
 * @note - Operation mode of ADC should be Static for this input selection.
 *  - If the ADC input sample is obtained from a temperature sensor, the positive input selection value must be '23'.
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock 
 * - \ref sl_si91x_adc_init
 * - \ref sl_si91x_adc_set_channel_configuration
 * @param  None
 * @return Returns 'SL_STATUS_OK' on successful execution.
 ******************************************************************************/
sl_status_t sl_si91x_adc_temperature_sensor_enable(void);

/***************************************************************************/ /**
 * @brief Configuring ADC fifo threshold.
 * @details The maximum FIFO depth is 16, and the threshold value may be configured.
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * - \ref sl_si91x_adc_init
 * - \ref sl_si91x_adc_set_channel_configuration
 * @param[in]  adc_config    : ADC operation configuration structure variable.
 * @param[in]  adc_fifo_threshold : ADC fifo structure variable like an empty fifo, a full fifo threshold level.
 *                                  ( \ref sl_adc_fifo_thrld_config_t )
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
 *         - SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
 ******************************************************************************/
sl_status_t sl_si91x_adc_fifo_threshold_configuration(sl_adc_config_t adc_config,
                                                      sl_adc_fifo_thrld_config_t adc_fifo_threshold);

/***************************************************************************/ /**
 * @brief Configure the ADC threshold to compare threshold value with ADC data.
 * @note This API applies when the ADC mode of operation is static.
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * - \ref sl_si91x_adc_init
 * - \ref sl_si91x_adc_set_channel_configuration
 * @param[in]  adc_threshold  :  ADC threshold configuration structure variables.
 * @return Returns 'SL_STATUS_OK' on successful execution.
 ******************************************************************************/
sl_status_t sl_si91x_adc_threshold_configuration(sl_adc_threshold_config_t adc_threshold);

/***************************************************************************/ /**
 * @brief Read the ADC samples data for FIFO mode of operation.
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * - \ref sl_si91x_adc_init
 * - \ref sl_si91x_adc_set_channel_configuration
 * - \ref sl_si91x_adc_register_event_callback
 * - \ref sl_si91x_adc_start
 * @param[in]  adc_channel_config   :  ADC channels configuration structure variable.
 * @param[in]  channel_num  :  Channel number
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
 *         - SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
 ******************************************************************************/
sl_status_t sl_si91x_adc_read_data(sl_adc_channel_config_t adcchconfig, uint8_t channel_num);

/***************************************************************************/ /**
 * @brief This API will read the ADC sampled data for static mode of operation.
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * - \ref sl_si91x_adc_init 
 * - \ref sl_si91x_adc_set_channel_configuration
 * - \ref sl_si91x_adc_register_event_callback
 * - \ref sl_si91x_adc_start
 * @param[in]  adc_channel_config   :  ADC channels configuration structure variable.
 * @param[in]  adc_config    : ADC operation configuration structure variable.
 * @param[out]  adc_value     :  Store the reading data on adc_value.
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
 *         - SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
 ******************************************************************************/
sl_status_t sl_si91x_adc_read_data_static(sl_adc_channel_config_t adc_channel_config,
                                          sl_adc_config_t adc_config,
                                          uint16_t *adc_value);

/***************************************************************************/ /**
 * @brief To get the channel sampling rate value which is configured to ADC.
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * - \ref sl_si91x_adc_init
 * - \ref sl_si91x_adc_set_channel_configuration
 * - \ref sl_si91x_adc_start
 * @param[in]  channel_num   :  Channel number
 * @return Returns sampling rate what it get from register
 ******************************************************************************/
uint32_t sl_si91x_adc_get_sampling_rate(uint8_t channel_num);

/***************************************************************************/ /**
 * @brief Uninitialize the ADC. 
 * @details If the DMA (Direct Memory Access) is enabled, it also uninitializes the
 * DMA. It will power down to ADC block and disable the dynamic mode
 * if FIFO mode of ADC operation.
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock 
 * - \ref sl_si91x_adc_init 
 * @param[in]  adc_config    : ADC operation configuration structure variable.
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_deinit(sl_adc_config_t adc_config);

/***************************************************************************/ /**
 * @brief Enable the signal to ADC controller and enable the dynamic mode
 * for FIFO mode of ADC operation.
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * - \ref sl_si91x_adc_init
 * - \ref sl_si91x_adc_set_channel_configuration
 * - \ref sl_si91x_adc_register_event_callback
 * @param[in]  adc_config    : ADC operation configuration structure variable.
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_start(sl_adc_config_t adc_config);

/***************************************************************************/ /**
 * @brief Stop the signal for ADC controller and disable the dynamic mode
 * if FIFO mode of ADC operation.
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * - \ref sl_si91x_adc_init
 * - \ref sl_si91x_adc_set_channel_configuration
 * - \ref sl_si91x_adc_register_event_callback
 * - \ref sl_si91x_adc_start
 * @param[in]  adc_config    : ADC operation configuration structure variable.
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_stop(sl_adc_config_t adc_config);

/***************************************************************************/ /**
 * @brief Get the ADC version.
 * @note It returns the API version of ADC.
 * @param none
 * @return (sl_adc_version_t) type structure
 ******************************************************************************/
sl_adc_version_t sl_si91x_adc_get_version(void);

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY! ***********************
/// @addtogroup ADC Analog to Digital Converter
/// @{
///
///   @details
///
///   @n @section ADC_intro Introduction
///
///   ADC stands for Analog-to-Digital Converter.
///   An ADC is a peripheral or component of a controller that transforms analog signals (continuous voltage levels)
///   into digital data (discrete binary values) that the controller can handle.
///
///   @li This translation allows the controller to communicate with analog sensors or signals,
///   such as temperature sensors, light sensors, or analog joysticks, in digital systems or apps.
///
///   @li Analog-to-Digital Converters (ADCs) are integral components in controllers across various applications.
///   Here are some common use cases:
///
///   1. Sensor Data Acquisition
///
///   2. Audio Processing
///
///   3. Instrumentation and Measurement
///
///   4. Motor Control
///
///   5. Battery Monitoring and Management
///
///   6. Feedback Control Systems etc.,
///
///   @n @section ADC_Conf Configuration
///
///   Configuring an Analog-to-Digital Converter (ADC) typically involves several parameters and
///   settings to ensure proper operation and accurate conversion of analog signals into digital data.
///   The specific configuration details may vary depending on the ADC hardware and the requirements of your application,
///   but here are some few configurable items:
///
///   @li To configure the number of ADC channels and Mode of ADC operation first set the appropriate values into this structure
///   @ref sl_adc_config_t, and then use this structure to use the API @ref sl_si91x_adc_set_channel_configuration(),
///   which will configure the number of channels.
///
///   @li To configure the ADC channel such as input type @ref sl_adc_input_type_typedef_t, number of samples(1-1023),
///   Sampling Rate (1-2500000 samples per second) will configured through
///   @ref sl_adc_channel_config_t, once fill the structure need call the API : @ref sl_si91x_adc_set_channel_configuration().
///
///   @li For more information on configuring available parameters refer to the respective peripheral example readme document.
///
///   @n @section ADC_Usage Usage
///
///   The common ADC functions can be used after the ADC Structures are specified, passing an instance of
///   @ref sl_adc_config_t and  @ref sl_adc_channel_config_t.
///   These functions will initiate and configure the ADC below, which is the flow for implementation.
///   1. @ref sl_si91x_adc_configure_clock
///   2. @ref sl_si91x_adc_init
///   3. @ref sl_si91x_adc_set_channel_configuration
///   4. @ref sl_si91x_adc_register_event_callback
///   5. @ref sl_si91x_adc_start
///   6. @ref sl_si91x_adc_read_data
///   7. @ref sl_si91x_adc_deinit
///
/** @} (end addtogroup ADC) */
#ifdef __cplusplus
}
#endif

#endif /* SL_SI91X_ADC_H_ */