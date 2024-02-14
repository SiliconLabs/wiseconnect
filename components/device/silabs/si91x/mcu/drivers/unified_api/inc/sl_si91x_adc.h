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
typedef adc_extr_config_t sl_adc_external_config_t;  ///< Renamed ADC external trigger configuration structure

/***************************************************************************/ /**
     * Typedef for user supplied callback function, which is called when ADC sample completes
     *
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

/// @brief Enumeration for ADC external trigger type
typedef enum {
  SL_ULP_TIMER_EXT_TRIGGER = ULP_TIMER_EXT_TRIGGER, ///< ULP timer external trigger type
  SL_ULP_GPIO_EXT_TRIGGER  = ULP_GPIO_EXT_TRIGGER,  ///< ULP gpio external trigger type
  SL_M4_CT_EXT_TRIGGER     = M4_CT_EXT_TRIGGER,     ///< M4 CT external trigger type
  SL_ADC_EXT_TRIGGER_TYPE_LAST,                     ///< Last member of enum for validation
} sl_adc_ext_trigger_type_t;

/// @brief Enumeration for ADC external trigger number
typedef enum {
  SL_ADC_EXT_TRIGGER_1 = DETECTION1, ///< External trigger detection 1
  SL_ADC_EXT_TRIGGER_2 = DETECTION2, ///< External trigger detection 2
  SL_ADC_EXT_TRIGGER_3 = DETECTION3, ///< External trigger detection 3
  SL_ADC_EXT_TRIGGER_4 = DETECTION4, ///< External trigger detection 4
  SL_ADC_EXT_TRIGGER_LAST,           ///< Last member of enum for validation
} sl_adc_ext_trigger_num_t;

/// @brief Enumeration for ADC external trigger edge selection
typedef enum {
  SL_ADC_EXT_TRIGGER_POS_EDGE     = POSITIVE_EDGE, ///< External trigger positive edge
  SL_ADC_EXT_TRIGGER_NEG_EDGE     = NEGATIVE_EDGE, ///< External trigger negative edge
  SL_ADC_EXT_TRIGGER_POS_NEG_EDGE = POS_NEG_EDGE,  ///< External trigger positive and negative edge
  SL_ADC_EXT_TRIGGER_EDGE_LAST,                    ///< Last member of enum for validation
} sl_adc_ext_trigger_edge_t;

/// @brief Enumeration for ADC external trigger selection
typedef enum {
  SL_ADC_EXT_TRIGGER_SEL_1 = EXT_TRIGGER_SEL1, ///< External trigger selection 1
  SL_ADC_EXT_TRIGGER_SEL_2 = EXT_TRIGGER_SEL2, ///< External trigger selection 2
  SL_ADC_EXT_TRIGGER_SEL_3 = EXT_TRIGGER_SEL3, ///< External trigger selection 3
  SL_ADC_EXT_TRIGGER_SEL_4 = EXT_TRIGGER_SEL4, ///< External trigger selection 4
  SL_ADC_EXT_TRIGGER_SEL_LAST,                 ///< Last member of enum for validation
} sl_adc_ext_trigger_sel_t;

/// @brief Enumeration for ADC channel.
typedef enum {
  SL_ADC_CHANNEL_0,  ///< ADC channel 1
  SL_ADC_CHANNEL_1,  ///< ADC channel 2
  SL_ADC_CHANNEL_2,  ///< ADC channel 3
  SL_ADC_CHANNEL_3,  ///< ADC channel 4
  SL_ADC_CHANNEL_4,  ///< ADC channel 5
  SL_ADC_CHANNEL_5,  ///< ADC channel 6
  SL_ADC_CHANNEL_6,  ///< ADC channel 7
  SL_ADC_CHANNEL_7,  ///< ADC channel 8
  SL_ADC_CHANNEL_8,  ///< ADC channel 9
  SL_ADC_CHANNEL_9,  ///< ADC channel 10
  SL_ADC_CHANNEL_10, ///< ADC channel 11
  SL_ADC_CHANNEL_11, ///< ADC channel 12
  SL_ADC_CHANNEL_12, ///< ADC channel 13
  SL_ADC_CHANNEL_13, ///< ADC channel 14
  SL_ADC_CHANNEL_14, ///< ADC channel 15
  SL_ADC_CHANNEL_15, ///< ADC channel 16
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
 * Set the clock for the ADC peripheral, Configures the PLL clock with the 
 * value set by user in the clock configuration structure.
 *
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
 * Initialize the ADC peripheral. Based on the sampling rate, the ADC clock source 
 * will set and Pass the Vref value to set the reference voltage to ADC.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock - Only for FIFO mode on M4 state 
 *
 * @param[in]  adc_channel_config  : ADC channels configuration structure variable
 * @param[in]  adc_config    : ADC operation configuration structure variable
 * @param[in]  vref_value    : Reference voltage
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
 * Configure ADC channel parameters.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock 
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock 
 * @pre Pre-conditions:
 * -  \ref sl_si91x_adc_init
 *
 * @param[in]  adc_channel_config  : ADC channels configuration structure variable.
 * @param[in]  adc_config    : ADC operation configuration structure variable.
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
 *         - SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
 ******************************************************************************/
sl_status_t sl_si91x_adc_set_channel_configuration(sl_adc_channel_config_t adc_channel_config,
                                                   sl_adc_config_t adc_config);

/***************************************************************************/ /**
 * Register the user callback function.
 * At the time of events, the function passed in the parameter is called with the respective
 * event as the parameter.
 * Before calling this function again, it is mandatory to call the \ref sl_si91x_adc_unregister_event_callback
 * function to unregister the callback, otherwise it returns SL_STATUS_BUSY error code.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_set_channel_configuration
 *
 * @param[in] callback_event Pointer to the function which needs to be called at the time of interrupt
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 *         - SL_STATUS_BUSY (0x0004) - Driver is busy
 ******************************************************************************/
sl_status_t sl_si91x_adc_register_event_callback(sl_adc_callback_t callback_event);

/***************************************************************************/ /**
 * Un-register the user callback function.
 * It is mandatory to call this function before registering the callback again.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_register_event_callback 
 *
 * @param[in] none
 * @return none
 ******************************************************************************/
void sl_si91x_adc_unregister_event_callback(void);

/***************************************************************************/ /**
 * Configure the ADC external trigger.
 * Triggers can be used in PS4 State to collect the sample from a pre-defined channel.
 * ADC can give a sample for the selected trigger with a trigger match.
 * There are 3 external triggers, which are enabled by
 *  - ULPSS Timer interrupts
 *  - ULPSS GPIOs
 *  - M4SS Config_Timer.
 * Trigger match can be checked by reading the \ref sl_si91x_adc_get_external_trigger_status
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_set_channel_configuration
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_start
 *
 * @param[in] adc_external_trigger  :  ADC external trigger configuration structure variable.
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_configure_external_trigger(sl_adc_external_config_t adc_external_trigger);

/***************************************************************************/ /**
 * Configure the ADC sampling rate for the ADC channels.
 * It will set the channel offset value and channel frequency for each channel to set the
 * sampling rate. The minimum allowed value is 3 to satisfy the Nyquist criteria of sampling rates. 
 * Freq_value 1 and 2 are not allowed.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init
 *
 * @param[in]  adc_internal_config  :  Channel offset and frequency for each channel to set sampling rate.
 * @param[in]  channel_num   :  Channel number
 * @return    sl_status_t    :  Returns 'SL_STATUS_OK' on successful execution.
 ******************************************************************************/
sl_status_t sl_si91x_adc_configure_channel_sampling_rate(sl_adc_internal_config_t adc_internal_config,
                                                         uint8_t channel_num);

/***************************************************************************/ /**
 * ADC can give a sample for the selected trigger with a trigger match.
 * Read the ADC external trigger match.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_set_channel_configuration
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_start
 *
 * @param[in]  adc_external_trigger  :  ADC external trigger configuration structure variable.
 * @param[in]  ext_trigger           :  The status of external trigger will be store in this.
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_get_external_trigger_status(sl_adc_external_config_t adc_external_trigger,
                                                     uint8_t *ext_trigger);

/***************************************************************************/ /**
 * Clear the ADC external trigger.
 * After reading the trigger match can clear specific trigger using 
 * \ref sl_si91x_adc_clear_external_trigger API.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_set_channel_configuration
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_start
 *
 * @param[in]  adc_external_trigger  :  ADC external trigger configuration structure variable.
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_clear_external_trigger(sl_adc_external_config_t adc_external_trigger);

/***************************************************************************/ /**
 * Configure the ADC ping and pong memory location and length
 * To configure the Ping and pong memory location along with length of
 * ping memory and pong memory. It is an applicable only for FIFO mode of ADC operation.
 * FIFO mode supports dual buffer cyclic mode to avoid loss of data when buffer is full. 
 * In dual buffer cyclic mode, if Ping buffer is full for particular channel, incoming 
 * sampled data is written into Pong buffer such that, samples from Ping buffer are read back 
 * by controller during this time. That’s why there are two start addresses, 
 * two buffer lengths and two valid signals for each channel.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock 
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init
 *
 * @param[in]  adc_internal_config  :  ADC internal trigger configuration structure variable.
 * @param[in]  channel_num   :  Channel number
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_configure_ping_pong_memory_address(sl_adc_internal_config_t adc_internal_config,
                                                            uint8_t channel_num);

/***************************************************************************/ /**
 * Enable ping pong for the corresponding ADC channels
 * It is an applicable only for FIFO mode of ADC operation.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_ping_pong_memory_address
 *
 * @param[in]  channel_num   :  Channel number
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_enable_ping_pong(uint8_t channel_num);

/***************************************************************************/ /**
 * Disable ping pong for the corresponding ADC channels
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_ping_pong_memory_address
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_enable_ping_pong
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_start
 *
 * @param[in]  channel_num   :  Channel number
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_disable_ping_pong(uint8_t channel_num);

/***************************************************************************/ /**
 * Enable DMA for the corresponding ADC channels.
 * It is an applicable only for FIFO mode of ADC operation.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_channel_enable
 *
 * @param[in]  channel_num   :  Channel number
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_internal_per_channel_dma_enable(uint8_t channel_num);

/***************************************************************************/ /**
 * Disable DMA for the corresponding ADC channels.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_channel_enable
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_internal_per_channel_dma_enable
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_start
 *
 * @param[in]  channel_num   :  Channel number
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_internal_per_channel_dma_disable(uint8_t channel_num);

/***************************************************************************/ /**
 * Configure the ADC in Static Mode
 * it can be programmed for setting input type, positive and negative input channel selection.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_ping_pong_memory_address
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_enable_ping_pong
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_channel_enable
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_internal_per_channel_dma_enable
 *
 * @param[in]  adc_channel_config   :  ADC channels configuration structure variable.
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
 *         - SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
 ******************************************************************************/
sl_status_t sl_si91x_adc_configure_static_mode(sl_adc_channel_config_t adc_channel_config, uint8_t channel_num);

/***************************************************************************/ /**
 * Configure the ADC in FIFO Mode.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_ping_pong_memory_address
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_enable_ping_pong
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_channel_enable
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_internal_per_channel_dma_enable
 *
 * @param[in]  adc_channel_config   :  ADC channels configuration structure variable.
 * @param[in]  channel_num   :  Channel number
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
 *         - SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
 ******************************************************************************/
sl_status_t sl_si91x_adc_configure_fifo_mode(sl_adc_channel_config_t adc_channel_config, uint8_t channel_num);

/***************************************************************************/ /**
 * Enable the ADC channel
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock 
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init
 *
 * @param[in]  channel_num   :  Channel number
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_channel_enable(uint8_t channel_num);

/***************************************************************************/ /**
 * Disable the ADC channel.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_channel_enable
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_start
 *
 * @param[in]  channel_num   :  Channel number
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_channel_disable(uint8_t channel_num);

/***************************************************************************/ /**
 * Set the Power On and off for ADC.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock 
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init 
 *
 * @param[in]  state       :  \b ADC_POWER_ON - To powerup adc powergates,
 *                            \b ADC_POWER_OFF - To powerdown adc powergates
 *                           ( \ref POWER_STATE )
 * @return status 0 if successful,
 *         - SL_STATUS_OK (0x0000) - Success
 ******************************************************************************/
sl_status_t sl_si91x_adc_set_power_mode(POWER_STATE state);

/***************************************************************************/ /**
 * Enable or Disable the Noise averaging mode to ADC.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_set_power_mode
 *
 * @param[in]  state       :  1 - To enable noise averaging mode,
 *                            0 - To disable noise averaging mode
 * @return Returns 'SL_STATUS_OK' on successful execution.
 ******************************************************************************/
sl_status_t sl_si91x_adc_set_noise_average_mode(boolean_t state);

/***************************************************************************/ /**
 * Enable BJT based Temperature sensor as an input to ADC.
 * Operation mode of ADC should be Static.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock 
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init
 *
 * @param[in]  None
 * @return Returns 'SL_STATUS_OK' on successful execution.
 ******************************************************************************/
sl_status_t sl_si91x_adc_temperature_sensor_enable(void);

/***************************************************************************/ /**
 * Configuring ADC fifo threshold. Maximum FIFO depth is 16 can configure the threshold value.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_set_channel_configuration
 *
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
 * Configure the ADC threshold to compare threshold value with ADC data.
 * This is valid in ADC Static mode only.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_set_channel_configuration
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_start
 *
 * @param[in]  adc_threshold  :  ADC threshold configuration structure variables.
 * @return Returns 'SL_STATUS_OK' on successful execution.
 ******************************************************************************/
sl_status_t sl_si91x_adc_threshold_configuration(sl_adc_threshold_config_t adc_threshold);

/***************************************************************************/ /**
 * Read the ADC samples when FIFO mode is enabled.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_set_channel_configuration
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_start
 *
 * @param[in]  adc_channel_config   :  ADC channels configuration structure variable.
 * @param[in]  uint8_t channel_num  :  Channel number
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
 *         - SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
 ******************************************************************************/
sl_status_t sl_si91x_adc_read_data(sl_adc_channel_config_t adcchconfig, uint8_t channel_num);

/***************************************************************************/ /**
 * Read the ADC samples when static mode is enabled.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init 
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_set_channel_configuration
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_start
 *
 * @param[in]  adc_channel_config   :  ADC channels configuration structure variable.
 * @param[in]  adc_config    : ADC operation configuration structure variable.
 * @param[in]  adc_value     :  Store the reading data on adc_value.
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
 *         - SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
 ******************************************************************************/
sl_status_t sl_si91x_adc_read_data_static(sl_adc_channel_config_t adc_channel_config,
                                          sl_adc_config_t adc_config,
                                          uint16_t *adc_value);

/***************************************************************************/ /**
 * To get the channel sampling rate value which is configured to ADC.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_set_channel_configuration
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_start
 *
 * @param[in]  channel_num   :  Channel number
 * @return Returns sampling rate what it get from register
 ******************************************************************************/
uint32_t sl_si91x_adc_get_sampling_rate(uint8_t channel_num);

/***************************************************************************/ /**
 * Uninitialize the ADC. If the DMA is enabled, it also uninitializes the
 * DMA.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock 
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init 
 *
 * @param[in]  adc_config    : ADC operation configuration structure variable.
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_deinit(sl_adc_config_t adc_config);

/***************************************************************************/ /**
 * Start the ADC operation.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_set_channel_configuration
 *
 * @param[in]  adc_config    : ADC operation configuration structure variable.
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_start(sl_adc_config_t adc_config);

/***************************************************************************/ /**
 * Stop the ADC operation.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_configure_clock
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_init
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_set_channel_configuration
 * @pre Pre-conditions:
 * - \ref sl_si91x_adc_start
 *
 * @param[in]  adc_config    : ADC operation configuration structure variable.
 * @return status 0 if successful, else error code as follow
 *         - SL_STATUS_OK (0x0000) - Success 
 *         - SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_stop(sl_adc_config_t adc_config);

/***************************************************************************/ /**
 * Get the ADC version.
 * It returns the API version of ADC.
 *
 * @param[in] none
 * @return (sl_adc_version_t) type structure
 ******************************************************************************/
sl_adc_version_t sl_si91x_adc_get_version(void);

/** @} (end addtogroup ADC) */

#ifdef __cplusplus
}
#endif

#endif /* SL_SI91X_ADC_H_ */
