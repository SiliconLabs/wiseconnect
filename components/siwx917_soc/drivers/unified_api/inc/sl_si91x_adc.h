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
 * @addtogroup ADC ADC
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
 * Typedef for the function pointer of the callback function
 ******************************************************************************/
typedef void (*sl_adc_callback_t)(uint8_t channel, uint8_t event);

// -----------------------------------------------------------------------------
//Macros for event
#define SL_INTERNAL_DMA          INTERNAL_DMA             // Internal dma event
#define SL_ADC_STATIC_MODE_EVENT ADC_STATIC_MODE_CALLBACK // Static mode event

#ifdef M4_PS2_STATE
#define ADC_PING_BUFFER 0x24060800
#define ADC_PONG_BUFFER (0x24060800 + 1023)
#else
#define ADC_PING_BUFFER 0x0000F000
#define ADC_PONG_BUFFER (0x0000F000 + 1023)
#endif

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
  SL_ADC_SINGLE_CHNL = DYNAMIC_MODE_DI, ///< Internal DMA type
  SL_ADC_MULTI_CHNL  = DYNAMIC_MODE_EN, ///< External DMA type
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

typedef struct {
  uint16_t threshold1;      ///< Threshold_1
  uint16_t threshold2;      ///< Threshold_2
  uint8_t *threshold1_cond; ///< Threshold_1 condition like Equal, Grater, Lesser
  uint8_t *threshold2_cond; ///< Threshold_2 condition like Equal, Grater, Lesser
  uint8_t range;
} sl_adc_threshold_config_t;

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
 * sl_status_t sl_si91x_adc_configure_clock(sl_adc_clock_config_t clock_configuration)
 * Configure the ADC clock.
 *
 * @param[in] clock_configuration : clock structure variables
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid \n
 *         \ref SL_STATUS_FAIL (0x0001) - The function is failed \n
 *         \ref SL_STATUS_NOT_INITIALIZED (0x0011) - Clock is not initialized
 ******************************************************************************/
sl_status_t sl_si91x_adc_configure_clock(sl_adc_clock_config_t *clock_configuration);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_init(sl_adc_channel_config_t adc_channel_config, sl_adc_config_t adc_config, sl_adc_callback_t event)
 * Initialize the ADC peripheral.
 *
 * @pre \ref sl_si91x_adc_configure_clock - Only for FIFO mode on M4 state \n
 *
 * @param[in]  adc_channel_config  : ADC channels configuration structure variable.
 * @param[in]  adc_config    : ADC operation configuration structure variable.
 * @param[in]  event        : Callback function pointer.
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid \n
 *         \ref SL_STATUS_BUSY (0x0004) - The function is already active \n
 *         \ref SL_STATUS_INVALID_COUNT (0x002B) - Mismatch count \n
 *         \ref SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
 ******************************************************************************/
sl_status_t sl_si91x_adc_init(sl_adc_channel_config_t adc_channel_config, sl_adc_config_t adc_config);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_channel_set_configuration(sl_adc_channel_config_t adc_channel_config, sl_adc_config_t adc_config)
 * Configure ADC channel parameters.
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *
 * @param[in]  adc_channel_config  : ADC channels configuration structure variable.
 * @param[in]  adc_config    : ADC operation configuration structure variable.
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid \n
 *         \ref SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
 ******************************************************************************/
sl_status_t sl_si91x_adc_channel_set_configuration(sl_adc_channel_config_t adc_channel_config,
                                                   sl_adc_config_t adc_config);

/***************************************************************************/ /**
 * Register the user callback function.
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *      \ref sl_si91x_adc_channel_set_configuration \n
 *
 * @param[in] callback_event Pointer to the function which needs to be called at the time of interrupt
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *         \ref SL_STATUS_BUSY (0x0004) - Driver is busy
 ******************************************************************************/
sl_status_t sl_si91x_adc_register_event_callback(sl_adc_callback_t callback_event);

/***************************************************************************/ /**
 * Un-register the user callback function.
 *
 * @pre \ref sl_si91x_adc_register_event_callback \n
 *
 * @param[in] none
 * @return none
 ******************************************************************************/
void sl_si91x_adc_unregister_event_callback(void);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_configure_reference_voltage(float vref_value, float chip_voltage)
 * Configure reference voltage for ADC operation.
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *
 * @param[in] float vref_value     : Reference voltage for ADC.
 * @param[in] float chip_voltage   : Chip voltage 3.2V or 1.8V, If chip voltage to chip is 1.8V then.
 *              \n chip voltage(bypass LDO) act as reference voltage to analog peripheral.
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid \n
 *         \ref SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
 ******************************************************************************/
sl_status_t sl_si91x_adc_configure_reference_voltage(float vref_value, float chip_voltage);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_configure_external_trigger(sl_adc_external_config_t adc_external_trigger)
 * Configure the ADC external trigger.
 * This API is used to mux select to choose between ulp_timer, ulp_gpio, M4_timer based on this
 * detection edge and channel trigger will interrupt.
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *      \ref sl_si91x_adc_channel_set_configuration \n
 *      \ref sl_si91x_adc_start \n
 *
 * @param[in] adc_external_trigger  :  ADC external trigger configuration structure variable.
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_configure_external_trigger(sl_adc_external_config_t adc_external_trigger);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_configure_channel_sampling_rate(sl_adc_internal_config_t adc_internal_config, uint8_t channel_num)
 * Configure the ADC sampling rate for ADC channels.
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *
 * @param[in]  adc_internal_config  :  Channel offset and frequency for each channel to set sampling rate.
 * @param[in]  uint8_t channel_num   :  Channel number
 * @return    sl_status_t    :  Returns 'SL_STATUS_OK' on successful execution.
 ******************************************************************************/
sl_status_t sl_si91x_adc_configure_channel_sampling_rate(sl_adc_internal_config_t adc_internal_config,
                                                         uint8_t channel_num);

/***************************************************************************/ /**
 * uint8_t sl_si91x_adc_get_external_trigger_status(sl_adc_external_config_t adc_external_trigger)
 * Read the ADC external trigger status.
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *      \ref sl_si91x_adc_channel_set_configuration \n
 *      \ref sl_si91x_adc_start \n
 *
 * @param[in]  adc_external_trigger  :  ADC external trigger configuration structure variable.
 * @param[in]  uint8_t ext_trigger   :  The status of external trigger will be store in this.
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_get_external_trigger_status(sl_adc_external_config_t adc_external_trigger,
                                                     uint8_t *ext_trigger);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_clear_external_trigger(sl_adc_external_config_t adc_external_trigger)
 * Clear the ADC external trigger status.
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *      \ref sl_si91x_adc_channel_set_configuration \n
 *      \ref sl_si91x_adc_start \n
 *
 * @param[in]  adc_external_trigger  :  ADC external trigger configuration structure variable.
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_clear_external_trigger(sl_adc_external_config_t adc_external_trigger);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_configure_ping_pong_memory_address(sl_adc_internal_config_t adc_internal_config)
 * Configure the ADC Ping and pong memory location along with length of ping memory and pong memory.
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *
 * @param[in]  adc_internal_config  :  ADC internal trigger configuration structure variable.
 * @param[in]  uint8_t channel_num   :  Channel number
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_configure_ping_pong_memory_address(sl_adc_internal_config_t adc_internal_config,
                                                            uint8_t channel_num);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_enable_ping_pong(void)
 * Enable ping pong for corresponding ADC channels
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *      \ref sl_si91x_adc_configure_ping_pong_memory_address \n
 *
 * @param[in]  uint8_t channel_num   :  Channel number
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_enable_ping_pong(uint8_t channel_num);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_disable_ping_pong(void)
 * Disable ping pong for corresponding ADC channels
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *      \ref sl_si91x_adc_configure_ping_pong_memory_address \n
 *      \ref sl_si91x_adc_enable_ping_pong \n
 *      \ref sl_si91x_adc_start \n
 *
 * @param[in]  uint8_t channel_num   :  Channel number
  * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_disable_ping_pong(uint8_t channel_num);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_internal_per_channel_dma_enable(void)
 * Enable internal DMA for corresponding ADC channels.
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *      \ref sl_si91x_adc_channel_enable \n
 *
 * @param[in]  uint8_t channel_num   :  Channel number
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_internal_per_channel_dma_enable(uint8_t channel_num);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_internal_per_channel_dma_disable(void)
 * Disable internal dma channel for corresponding ADC channels.
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *      \ref sl_si91x_adc_channel_enable \n
 *      \ref sl_si91x_adc_internal_per_channel_dma_enable \n
 *      \ref sl_si91x_adc_start \n
 *
 * @param[in]  uint8_t channel_num   :  Channel number
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_internal_per_channel_dma_disable(uint8_t channel_num);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_configure_static_mode(sl_adc_channel_config_t adc_channel_config)
 * Configure the ADC in Static Mode
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *      \ref sl_si91x_adc_configure_ping_pong_memory_address \n
 *      \ref sl_si91x_adc_enable_ping_pong \n
 *      \ref sl_si91x_adc_channel_enable \n
 *      \ref sl_si91x_adc_internal_per_channel_dma_enable \n
 *
 * @param[in]  adc_channel_config   :  ADC channels configuration structure variable.
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid \n
 *         \ref SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
 ******************************************************************************/
sl_status_t sl_si91x_adc_configure_static_mode(sl_adc_channel_config_t adc_channel_config, uint8_t channel_num);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_configure_fifo_mode(sl_adc_channel_config_t adc_channel_config)
 * Configure the ADC in FIFO Mode.
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *      \ref sl_si91x_adc_configure_ping_pong_memory_address \n
 *      \ref sl_si91x_adc_enable_ping_pong \n
 *      \ref sl_si91x_adc_channel_enable \n
 *      \ref sl_si91x_adc_internal_per_channel_dma_enable \n
 *
 * @param[in]  adc_channel_config   :  ADC channels configuration structure variable.
 * @param[in]  uint8_t channel_num   :  Channel number
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid \n
 *         \ref SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
 ******************************************************************************/
sl_status_t sl_si91x_adc_configure_fifo_mode(sl_adc_channel_config_t adcchconfig, uint8_t channel_num);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_channel_enable(void)
 * Enable the ADC channel
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *
 * @param[in]  uint8_t channel_num   :  Channel number
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_channel_enable(uint8_t channel_num);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_channel_disable(void)
 * Disable the ADC channel.
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *      \ref sl_si91x_adc_channel_enable \n
 *      \ref sl_si91x_adc_start \n
 *
 * @param[in]  uint8_t channel_num   :  Channel number
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_channel_disable(uint8_t channel_num);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_set_power_mode(sl_adc_power_state_t state)
 * Set to Power On and off for ADC.
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *
 * @param[in]  state       :  \b ADC_POWER_ON - To powerup adc powergates
 *                         \b ADC_POWER_OFF - To powerdown adc powergates
  * @return status 0 if successful,
 *         \ref SL_STATUS_OK (0x0000) - Success
 ******************************************************************************/
sl_status_t sl_si91x_adc_set_power_mode(POWER_STATE state);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_set_noise_average_mode(boolean_t state)
 * Enable or Disable Noise averaging mode
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_power_control \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *
 * @param[in]  state       :  1 - To enable noise averaging mode
 *                            0 - To disable noise averaging mode
 * @return    sl_status_t  :  Returns 'SL_STATUS_OK' on successful execution.
 ******************************************************************************/
sl_status_t sl_si91x_adc_set_noise_average_mode(boolean_t state);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_temperature_sensor_enable(void)
 * Enable temp-sensor for ADC.
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *
 * @param[in]  None
 * @return    sl_status_t    :  Returns 'SL_STATUS_OK' on successful execution.
 ******************************************************************************/
sl_status_t sl_si91x_adc_temperature_sensor_enable(void);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_fifo_threshold_configuration(sl_adc_config_t adc_config, sl_adc_fifo_thrld_config_t adc_fifo_threshold)
 * Configuring ADC fifo threshold.
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *      \ref sl_si91x_adc_channel_set_configuration \n
 *
 * @param[in]  adc_config    : ADC operation configuration structure variable.
 * @param[in]  adc_fifo_threshold : ADC fifo structure variable like aempty fifo, afull fifo threshold level.
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid \n
 *         \ref SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
 ******************************************************************************/
sl_status_t sl_si91x_adc_fifo_threshold_configuration(sl_adc_config_t adc_config,
                                                      sl_adc_fifo_thrld_config_t adc_fifo_threshold);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_threshold_configuration(sl_adc_threshold_config_t adc_threshold)
 * Configure the ADC threshold to compare threshold value with ADC data.
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *      \ref sl_si91x_adc_channel_set_configuration \n
 *      \ref sl_si91x_adc_start \n
 *
 * @param[in]  adc_threshold  :  ADC threshold configuration structure variables.
 * @return    sl_status_t     :  Returns 'SL_STATUS_OK' on successful execution.
 ******************************************************************************/
sl_status_t sl_si91x_adc_threshold_configuration(sl_adc_threshold_config_t adc_threshold);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_read_data(sl_adc_channel_config_t adc_channel_config)
 * Read the ADC samples when ulp memories are used.
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *      \ref sl_si91x_adc_channel_set_configuration \n
 *      \ref sl_si91x_adc_start \n
 *
 * @param[in]  adc_channel_config   :  ADC channels configuration structure variable.
 * @param[in]  uint8_t channel_num  :  Channel number
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid \n
 *         \ref SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
 ******************************************************************************/
sl_status_t sl_si91x_adc_read_data(sl_adc_channel_config_t adcchconfig, uint8_t channel_num);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_read_data_static(sl_adc_channel_config_t adc_channel_config, uint16_t *adc_value)
 * Read the ADC samples when static mode is enabled.
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *      \ref sl_si91x_adc_channel_set_configuration \n
 *      \ref sl_si91x_adc_start \n
 *
 * @param[in]  adc_channel_config   :  ADC channels configuration structure variable.
 * @param[in]  adc_value     :  Store the reading data on adc_value.
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid \n
 *         \ref SL_STATUS_INVALID_RANGE (0x0028) - Mismatch Range
 ******************************************************************************/
sl_status_t sl_si91x_adc_read_data_static(sl_adc_channel_config_t adc_channel_config,
                                          sl_adc_config_t adc_config,
                                          uint16_t *adc_value);

/***************************************************************************/ /**
 * float sl_si91x_adc_get_chip_voltage(void)
 * Get the battery/chip voltage level status.
 *
 * @param[in]  None.
 * @return    float  :  Returns "chip_volt" chip voltage level.
 ******************************************************************************/
float sl_si91x_adc_get_chip_voltage(void);

/***************************************************************************/ /**
 * uint32_t sl_si91x_adc_get_sampling_rate(void)
 * Read the ADC sampling rate when static mode is enabled.
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *      \ref sl_si91x_adc_channel_set_configuration \n
 *      \ref sl_si91x_adc_start \n
 *
 * @param[in]  uint8_t channel_num   :  Channel number
 * @return    uint32_t  :  Returns sampling rate what it get from register
 ******************************************************************************/
uint32_t sl_si91x_adc_get_sampling_rate(uint8_t channel_num);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_deinit(sl_adc_config_t adc_config)
 * De-initialize the ADC.
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *
 * @param[in]  adc_config    : ADC operation configuration structure variable.
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_deinit(sl_adc_config_t adc_config);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_start(sl_adc_config_t adc_config)
 * Start the ADC operation.
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *      \ref sl_si91x_adc_channel_set_configuration \n
 *
 * @param[in]  adc_config    : ADC operation configuration structure variable.
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_start(sl_adc_config_t adc_config);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_stop(sl_adc_config_t adc_config)
 * Stop the ADC operation.
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *      \ref sl_si91x_adc_channel_set_configuration \n
 *      \ref sl_si91x_adc_start \n
 *
 * @param[in]  adc_config    : ADC operation configuration structure variable.
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_stop(sl_adc_config_t adc_config);

/***************************************************************************/ /**
 * sl_status_t sl_si91x_adc_channel_interrupt_clear(sl_adc_config_t adc_config)
 * Unmask the ADC channel
 *
 * @pre \ref sl_si91x_adc_configure_clock \n
 *      \ref sl_si91x_adc_init \n
 *      \ref sl_si91x_adc_configure_reference_voltage \n
 *      \ref sl_si91x_adc_channel_set_configuration \n
 *      \ref sl_si91x_adc_start \n
 *      \ref sl_si91x_adc_read_data_static \n
 *
 * @param[in]  adc_config    : ADC operation configuration structure variable.
 * @param[in]  uint8_t channel_num   :  Channel number
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 ******************************************************************************/
sl_status_t sl_si91x_adc_channel_interrupt_clear(sl_adc_config_t adc_config, uint8_t channel_num);

/***************************************************************************/ /**
 * Get the release, sqa and dev version of ADC.
 *
 * @param[in] none
 * @return (sl_adc_version_t) type structure
 *
 ******************************************************************************/
sl_adc_version_t sl_si91x_adc_get_version(void);

/** @} (end addtogroup ADC) */

#ifdef __cplusplus
}
#endif

#endif /* SL_SI91X_ADC_H_ */
