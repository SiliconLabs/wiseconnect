/***************************************************************************/ /**
 * @file sl_si91x_pwm.h
 * @brief PWM API header file
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

#ifndef SL_SI91X_PWM_H
#define SL_SI91X_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

//// Includes
#include "sl_status.h"
#include "rsi_rom_pwm.h"
#include "rsi_rom_egpio.h"
#include "sl_pwm_board.h"

/***************************************************************************/ /**
 * @addtogroup PWM
 * @{
 * @brief Motor Control PWM (MCPWM) controller is used to generate a periodic pulse waveform, which is useful in motor
 * control and power control applications. The MCPWM controller acts as a timer to count up to a period count value.
 * The time period and the duty cycle of the pulses are both programmable.
 *
 * @details
 * ##Overview
 * - It generates four different duty cycles on different channels using programmable duty cycle and time period and
 *    different interrupts are supported.
 *
 * ## Features
 * - Supports up to eight PWM outputs with four duty cycle generators
 * - Complementary and Independent output modes are supported
 * - Dead time insertion in Complementary mode
 * - Manual override option for PWM output pins. Output pin polarity is programmable
 * - Supports generation of interrupt for different events
 * - Supports two hardware fault input pins
 * - Special event trigger for synchronizing analog-to-digital conversions
 *
 * ## Initialization
 * - Initialize PWM using \ref sl_si91x_pwm_init.
 * - Set PWM configuration using \ref sl_si91x_pwm_set_configuration.
 *******************************************************************************/

/*******************************************************************************
 ***************************  DEFINES / MACROS  ********************************
 ******************************************************************************/
typedef RSI_MCPWM_T sl_si91x_mcpwm_t; ///< PWM structure
typedef RSI_MCPWM_SVT_CONFIG_T
  sl_si91x_pwm_svt_config_t; ///< PWM Special Event trigger configuration parameters structure
typedef RSI_MCPWM_DT_CONFIG_T sl_si91x_pwm_dt_config_t; ///< PWM DeadTime configuration parameters structure
typedef RSI_MCPWM_CALLBACK_T sl_si91x_pwm_callback_t;   ///< PWM Callback structure
/*******************************************************************************
 ********************************   Local Variables   **************************
 ******************************************************************************/

/*******************************************************************************
 ***************************  Global  VARIABLES ********************************
 ******************************************************************************/

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/
///@brief Enum to control fault a and fault b
typedef enum {
  SL_FAULTA,     ///< PWM fault A
  SL_FAULTB,     ///< PWM fault B
  SL_FAULT_LAST, ///< Last member of enum for validation
} sl_pwm_fault_t;

///@brief Enum to control dead time
typedef enum {
  SL_DEAD_TIME_DISABLE, ///< PWM dead time disable
  SL_DEAD_TIME_ENABLE,  ///< PWM dead time enable
  SL_DEAD_TIME_LAST,    ///< Last member of enum for validation
} sl_pwm_dead_time_t;

///@brief Enum to control duty cycle
typedef enum {
  SL_DUTY_CYCLE_DISABLE, ///< PWM duty cycle disable
  SL_DUTY_CYCLE_ENABLE,  ///< PWM duty cycle enable
  SL_DUTY_CYCLE_LAST,    ///< Last member of enum for validation
} sl_pwm_duty_cycle_t;

///@brief Enum to control override
typedef enum {
  SL_OVERRIDE_RESET, ///< PWM override reset
  SL_OVERRIDE_SET,   ///< PWM override set
  SL_OVERRIDE_LAST,  ///< Last member of enum for validation
} sl_pwm_override_t;

///@brief Enum to control output fault
typedef enum {
  SL_OUTPUT_FAULT_RESET, ///< PWM output fault reset
  SL_OUTPUT_FAULT_SET,   ///< PWM output fault set
  SL_OUTPUT_FAULT_LAST,  ///< Last member of enum for validation
} sl_pwm_output_fault_t;

///@brief Enum to control events
typedef enum {
  SL_EVENT_DISABLE, ///< PWM event disable
  SL_EVENT_ENABLE,  ///< PWM event enable
  SL_EVENT_LAST,    ///< Last member of enum for validation
} sl_pwm_event_t;

///@brief Enumeration for PWM channels(1-4)
typedef enum {
  SL_CHANNEL_1,    ///< PWM channel 1
  SL_CHANNEL_2,    ///< PWM channel 2
  SL_CHANNEL_3,    ///< PWM channel 3
  SL_CHANNEL_4,    ///< PWM channel 4
  SL_CHANNEL_LAST, ///< Last member of enum for validation
} sl_pwm_channel_t;

///@brief Enumeration for PWM timer
typedef enum {
  SL_BASE_TIMER_EACH_CHANNEL, ///< PWM timer for each channel
  SL_BASE_TIMER_ALL_CHANNEL,  ///< PWM timer for all channels
  SL_BASE_TIMER_LAST,         ///< Last member of enum for validation
} sl_pwm_timer_t;

///@brief Enumeration for PWM polarity low
typedef enum {
  SL_POLARITYL_LOW,  ///< PWM output polarity for low side(L0-L3)-low
  SL_POLARITYL_HIGH, ///< PWM output polarity for low side(H0-H3)-high
  SL_POLARITYL_LAST, ///< Last member of enum for validation
} sl_pwm_polarity_low_t;

///@brief Enumeration for PWM polarity high
typedef enum {
  SL_POLARITYH_LOW,  ///< PWM output polarity for high side(L0-L3)-low
  SL_POLARITYH_HIGH, ///< PWM output polarity for high side(H0-H3)-high
  SL_POLARITYH_LAST, ///< Last member of enum for validation
} sl_pwm_polarity_high_t;

///@brief Enumeration for PWM mode
typedef enum {
  SL_MODE_INDEPENDENT,   ///< PWM independent mode
  SL_MODE_COMPLEMENTARY, ///< PWM complementary mode
  SL_MODE_LAST,          ///< Last member of enum for validation
} sl_pwm_mode_t;

///@brief Enumeration for PWM base timer modes
typedef enum {
  SL_FREE_RUN_MODE         = 0, ///< PWM free run mode
  SL_SINGLE_EVENT_MODE     = 1, ///< PWM single event mode
  SL_DOWN_COUNT_MODE       = 2, ///< PWM down count mode
  SL_UP_DOWN_MODE          = 4, ///< PWM up/down mode
  SL_UP_DOWN_DOUBLE_UPDATE = 5, ///< PWM up/down double update mode
  SL_BASE_TIMER_MODE_LAST  = 6, ///< Last member of enum for validation
} sl_pwm_base_timer_mode_t;

///@brief Enumeration for PWM output complementary pairs
typedef enum {
  SL_OUTPUT_LOW0,  ///< PWM output L0
  SL_OUTPUT_LOW1,  ///< PWM output L1
  SL_OUTPUT_LOW2,  ///< PWM output L2
  SL_OUTPUT_LOW3,  ///< PWM output L3
  SL_OUTPUT_HIGH0, ///< PWM output H0
  SL_OUTPUT_HIGH1, ///< PWM output H1
  SL_OUTPUT_HIGH2, ///< PWM output H2
  SL_OUTPUT_HIGH3, ///< PWM output H3
  SL_OUTPUT_LAST,  ///< Last member of enum for validation
} sl_pwm_output_t;

///@brief Enumeration for PWM special event trigger time base
typedef enum {
  SL_SVT_COUNT_UP,   ///< Special event trigger occurs when time base is counting up
  SL_SVT_COUNT_DOWN, ///< Special event trigger occurs when time base is counting down
  SL_SVT_COUNT_LAST, ///< Last member of enum for validation
} sl_pwm_svt_t;

///@brief Enumeration for PWM Time base output post scale bits
typedef enum {
  SL_TIME_PERIOD_POSTSCALE_1_1,    ///< 1:1 post scale
  SL_TIME_PERIOD_POSTSCALE_1_2,    ///< 1:2 post scale
  SL_TIME_PERIOD_POSTSCALE_1_3,    ///< 1:3 post scale
  SL_TIME_PERIOD_POSTSCALE_1_4,    ///< 1:4 post scale
  SL_TIME_PERIOD_POSTSCALE_1_5,    ///< 1:5 post scale
  SL_TIME_PERIOD_POSTSCALE_1_6,    ///< 1:6 post scale
  SL_TIME_PERIOD_POSTSCALE_1_7,    ///< 1:7 post scale
  SL_TIME_PERIOD_POSTSCALE_1_8,    ///< 1:8 post scale
  SL_TIME_PERIOD_POSTSCALE_1_9,    ///< 1:9 post scale
  SL_TIME_PERIOD_POSTSCALE_1_10,   ///< 1:10 post scale
  SL_TIME_PERIOD_POSTSCALE_1_11,   ///< 1:11 post scale
  SL_TIME_PERIOD_POSTSCALE_1_12,   ///< 1:12 post scale
  SL_TIME_PERIOD_POSTSCALE_1_13,   ///< 1:13 post scale
  SL_TIME_PERIOD_POSTSCALE_1_14,   ///< 1:14 post scale
  SL_TIME_PERIOD_POSTSCALE_1_15,   ///< 1:15 post scale
  SL_TIME_PERIOD_POSTSCALE_1_16,   ///< 1:16 post scale
  SL_TIME_PERIOD_POSTSCALE_1_LAST, ///< Last member of enum for validation
} sl_pwm_post_t;

///@brief Enumeration for PWM input clock pre scale select value
typedef enum {
  SL_TIME_PERIOD_PRESCALE_1,    ///< 1x input clock period
  SL_TIME_PERIOD_PRESCALE_2,    ///< 2x input clock period
  SL_TIME_PERIOD_PRESCALE_4,    ///< 4x input clock period
  SL_TIME_PERIOD_PRESCALE_8,    ///< 8x input clock period
  SL_TIME_PERIOD_PRESCALE_16,   ///< 16x input clock period
  SL_TIME_PERIOD_PRESCALE_32,   ///< 32x input clock period
  SL_TIME_PERIOD_PRESCALE_64,   ///< 64x input clock period
  SL_TIME_PERIOD_PRESCALE_LAST, ///< Last member of enum for validation
} sl_pwm_pre_t;

///@brief Enumeration for fault input override value
typedef enum {
  SL_VALUE_INACTIVE, ///< PWM output pin is driven inactive on an external fault input A/B event
  SL_VALUE_ACTIVE,   ///< PWM output pin is driven active on an external fault input A/B event
  SL_VALUE_LAST,     ///< Last member of enum for validation
} sl_pwm_fault_input_t;

///@brief Enumeration for output override control
typedef enum {
  SL_OP_OVERRIDE_UNSYNC, ///< no effect.
  SL_OP_OVERRIDE_SYNC,   ///< Output override is in sync with pwm time period depending on operating mode
  SL_OP_OVERRIDE_LAST,   ///< Last member of enum for validation
} sl_pwm_output_override_t;

///@brief Enumeration for override value
typedef enum {
  SL_OVERRIDE_VALUE0,     ///< Override value 0
  SL_OVERRIDE_VALUE1,     ///< Override value 1
  SL_OVERRIDE_VALUE_LAST, ///< Last member of enum for validation
} sl_pwm_override_value_t;

///@brief PWM enable external triggering
typedef enum {
  SL_TRIGGER_DISABLE, ///< Disabling external triggering
  SL_TRIGGER_ENABLE,  ///< Enabling external triggering
  SL_TRIGGER_LAST,    ///< Last member of enum for validation
} sl_pwm_trigger_t;

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/
/// @brief Structure to hold the different versions of peripheral API
typedef struct {
  uint8_t release; ///< Release version number
  uint8_t major;   ///< major version number
  uint8_t minor;   ///< minor version number
} sl_pwm_version_t;

/// @brief Structure to hold the parameters of timer configurations
typedef struct {
  sl_pwm_channel_t channel;                 ///< PWM channel selection(ch0 to ch4)
  uint32_t frequency;                       ///< PWM frequency
  boolean_t is_polarity_low;                ///< PWM output polarity for low side
  boolean_t is_polarity_high;               ///< PWM output polarity for high side
  boolean_t is_mode;                        ///< PWM mode(independent/complementary)
  uint32_t base_time_counter_initial_value; ///< PWM base time counter initial value
  uint8_t duty_cycle;                       ///< PWM duty cycle
  uint8_t base_timer_mode;                  ///< PWM base timer mode
  uint8_t channel_timer_selection;          ///< PWM channel timer selection
} sl_pwm_config_t;

///@brief Structure to hold portl and pinh for channels
typedef struct {
  uint8_t port_l; ///< PWM GPIO output low side port
  uint8_t pin_l;  ///< PWM GPIO output low side pin
  uint8_t port_h; ///< PWM GPIO output high side port
  uint8_t pin_h;  ///< PWM GPIO output high side pin
  uint8_t mux_l;  ///< PWM GPIO output low side mux
  uint8_t mux_h;  ///< PWM GPIO output high side mux
  uint8_t pad_l;  ///< PWM GPIO output low side pad
  uint8_t pad_h;  ///< PWM GPIO output high side pad
} sl_pwm_init_t;

///@brief Structure to hold port and pin of different events
typedef struct {
  uint8_t port; ///< PWM GPIO port
  uint8_t pin;  ///< PWM GPIO pin
  uint8_t mux;  ///< PWM GPIO mux
  uint8_t pad;  ///< PWM GPIO pad
} sl_pwm_fault_init_t;
/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************/ /**
 * @brief  This API is used to de-initialize the PWM peripheral
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 * @param[in] none
 * @return none
 ******************************************************************************/
void sl_si91x_pwm_deinit(void);

/***************************************************************************/ /**
 * @brief  This API is used to get the PWM version
 * @param[in] none
 * @return returns structure of type \ref sl_pwm_version_t
 ******************************************************************************/
sl_pwm_version_t sl_si91x_pwm_get_version(void);

/*******************************************************************************
 * This API is used to set the PWM configuration parameters
 * @pre   \ref  sl_si91x_pwm_init \n
 * @param[in] pwm_config : pointer to configuration parameters of type \ref sl_pwm_config_t
 * @return      returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_set_configuration(sl_pwm_config_t *pwm_config);

/***************************************************************************/ /**
 * @brief   This API is used to set output polarity for MCPWM
 * @pre   \ref  sl_si91x_pwm_init \n
 * @param[in] polarity_low  :  Output polarity for low side (L3, L2, L1, L0)
 * @param[in] polarity_high  :  Output polarity for high side (H3, H2, H1, H0)
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_set_output_polarity(boolean_t polarity_low, boolean_t polarity_high);

/***************************************************************************/ /**
 * @brief     This API is used to start the MCPWM operation for required channel
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 *        \ref  sl_si91x_pwm_set_base_timer_mode \n
 *        \ref  sl_si91x_pwm_control_period \n
 *        \ref  sl_si91x_pwm_register_callback \n
 * @param[in]   channel :  Channel number(1 to 4) of type \ref sl_pwm_channel_t
 * @return      returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_start(sl_pwm_channel_t channel);

/***************************************************************************/ /**
 * @brief     This API is used to stop the MCPWM operation for required channel
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 *        \ref  sl_si91x_pwm_set_base_timer_mode \n
 *        \ref  sl_si91x_pwm_control_period \n
 *        \ref  sl_si91x_pwm_register_callback \n
 *        \ref  sl_si91x_pwm_start \n
 * @param[in]   channel :  Channel number(1 to 4) of type \ref sl_pwm_channel_t
 * @return      returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_stop(sl_pwm_channel_t channel);

/***************************************************************************/ /**
 * @brief     This API is used to select number of base timers as four base timers for
 *                  four channels or one base timer for all channels of MCPWM
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 * @param[in]   base_timer :  PWM base timer of type \ref sl_pwm_timer_t
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_control_base_timer(sl_pwm_timer_t base_timer);

/***************************************************************************/ /**
 * @brief   This API is used to set time period and counter initial value for the required MCPWM channel.
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 * @param[in]   channel :  Channel number(1 to 4) of type \ref sl_pwm_channel_t
 * @param[in]   period  :  Time period value
 * @param[in]   init_val :  Update the base time counter initial value
 * @return      returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_set_time_period(sl_pwm_channel_t channel, uint32_t period, uint32_t init_val);

/***************************************************************************/ /**
 * @brief    This API is used to configure special event trigger generation for required MCPWM channel which
 *                allows the A/D converter to be synchronized to the PWM time base.
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 *        \ref  sl_si91x_pwm_set_base_timer_mode \n
 *        \ref  sl_si91x_pwm_control_period \n
 *        \ref  sl_si91x_pwm_register_callback \n
 * @param[in] direction  :  Special event trigger for time base direction of type \ref sl_pwm_svt_t
 * @param[in] pwm_config   :  Pointer to the structure of type \ref sl_si91x_pwm_svt_config_t\n
 * @return      returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_trigger_special_event(sl_pwm_svt_t direction, sl_si91x_pwm_svt_config_t *pwm_config);

/***************************************************************************/ /**
 * @brief     This API is used to configure Dead time insertion parameters for MCPWM
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 *        \ref  sl_si91x_pwm_set_base_timer_mode \n
 *        \ref  sl_si91x_pwm_control_period \n
 *        \ref  sl_si91x_pwm_register_callback \n
 * @param[in]   dead_time  :  Pointer to the structure of type \ref sl_si91x_pwm_dt_config_t\n
 * @param[in]   channel  :Channel number(1 to 4) of type \ref sl_pwm_channel_t
 * @return      returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_configure_dead_time(sl_si91x_pwm_dt_config_t *dead_time, sl_pwm_channel_t channel);

/***************************************************************************/ /**
 * @brief   This API is used to reset the required channel of MCPWM.
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 *        \ref  sl_si91x_pwm_set_base_timer_mode \n
 *        \ref  sl_si91x_pwm_control_period \n
 *        \ref  sl_si91x_pwm_register_callback \n
 * @param[in] channel :  Channel number(1 to 4) of type \ref sl_pwm_channel_t
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_reset_channel(sl_pwm_channel_t channel);

/***************************************************************************/ /**
 * @brief   This API is used to reset the counter from required channel of MCPWM
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 *        \ref  sl_si91x_pwm_set_base_timer_mode \n
 *        \ref  sl_si91x_pwm_control_period \n
 *        \ref  sl_si91x_pwm_register_callback \n
 * @param[in] channel :  Channel number(1 to 4) of type \ref sl_pwm_channel_t
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_reset_counter(sl_pwm_channel_t channel);

/***************************************************************************/ /**
 * @brief   This API is used to set base time period control for the required MCPWM channel.
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 *        \ref  sl_si91x_pwm_set_base_timer_mode \n
 * @param[in] post_scale  :  Time base output post scale bits of type \ref sl_pwm_post_t
 * @param[in] pre_scale  :   Base timer input clock pre scale select value of type \ref sl_pwm_pre_t
 * @param[in] channel  :  Channel number(1 to 4) of type \ref sl_pwm_channel_t
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_control_period(sl_pwm_post_t post_scale, sl_pwm_pre_t pre_scale, sl_pwm_channel_t channel);

/***************************************************************************/ /**
 * @brief     This API is used to control fault A/B pin output value to be overridden when fault condition occurs.
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 *        \ref  sl_si91x_pwm_set_base_timer_mode \n
 *        \ref  sl_si91x_pwm_control_period \n
 *        \ref  sl_si91x_pwm_register_callback \n
 * @param[in]   fault : enum of type \ref sl_pwm_fault_t
 * @param[in]   pwm_output : enum of type \ref sl_pwm_output_t
 * @param[in]   value : Fault input A/B PWM output override value of type \ref sl_pwm_override_value_t
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_control_fault(sl_pwm_fault_t fault, sl_pwm_output_t pwm_output, sl_pwm_override_value_t value);

/***************************************************************************/ /**
 * @brief   This API is used to set the mode of base timer for required channel
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 * @param[in] mode  :  Base timer operating mode of type \ref sl_pwm_base_timer_mode_t
 * @param[in] channel : Channel number(1 to 4) of type \ref sl_pwm_channel_t
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_set_base_timer_mode(sl_pwm_base_timer_mode_t mode, sl_pwm_channel_t channel);

/***************************************************************************/ /**
 * @brief   This API is used to set output mode for the MCPWM
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 * @param[in] mode : PWM Output mode of type \ref sl_pwm_mode_t
 * @param[in] channel : Channel number(1 to 4) of type \ref sl_pwm_channel_t
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_set_output_mode(sl_pwm_mode_t mode, sl_pwm_channel_t channel);

/***************************************************************************/ /**
 * @brief   Handles all interrupt flags of MCPWM.
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 *        \ref  sl_si91x_pwm_set_base_timer_mode \n
 *        \ref  sl_si91x_pwm_control_period \n
 * @param[in] callback_event :  structure of type \ref sl_si91x_pwm_callback_t
 * @param[in] flag : flag :  It is the logical OR of different interrupts generated on multiple channels
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_register_callback(sl_si91x_pwm_callback_t *callback_event, uint16_t flag);

/***************************************************************************/ /**
 * @brief   Unregisters the PWM event
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 *        \ref  sl_si91x_pwm_set_base_timer_mode \n
 *        \ref  sl_si91x_pwm_control_period \n
 *        \ref  sl_si91x_pwm_register_callback \n
 *        \ref  sl_si91x_pwm_start \n
 * @param[in] flag : flag :  It is the logical OR of different interrupts generated on multiple channels
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_unregister_callback(uint16_t flag);

/***************************************************************************/ /**
 * @brief     This API is used to read the counter current value
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 *        \ref  sl_si91x_pwm_set_base_timer_mode \n
 *        \ref  sl_si91x_pwm_control_period \n
 *        \ref  sl_si91x_pwm_register_callback \n
 *        \ref  sl_si91x_pwm_start \n
 * @param[in]   counter_value : counter value
 * @param[in]   channel  : Channel number(1 to 4) of type \ref sl_pwm_channel_t
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_read_counter(uint16_t *counter_value, sl_pwm_channel_t channel);

/***************************************************************************/ /**
 * @brief     This API is used to get time period counter direction status of required MCPWM channel
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 *        \ref  sl_si91x_pwm_set_base_timer_mode \n
 *        \ref  sl_si91x_pwm_control_period \n
 *        \ref  sl_si91x_pwm_register_callback \n
 *        \ref  sl_si91x_pwm_start \n
 * @param[out]  counter_direction :  Counter direction as up/down counter
 * @param[in]   channel  :  Channel number(1 to 4) of type \ref sl_pwm_channel_t
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_get_counter_direction(uint8_t *counter_direction, sl_pwm_channel_t channel);

/***************************************************************************/ /**
 * @brief     Controls dead time insertion at rise edge (or) fall edge of any four channels
 *  * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 *        \ref  sl_si91x_pwm_set_base_timer_mode \n
 * @param[in]   dead_time : Enum of type \ref sl_pwm_dead_time_t
 * @param[in]   flag   :  This can be ORing of below values
                          -  DT_EN_CH0
                          -  DT_EN_CH1
                          -  DT_EN_CH2
                          -  DT_EN_CH3
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_control_dead_time(sl_pwm_dead_time_t dead_time, uint32_t flag);

/***************************************************************************/ /**
 * @brief     This API is used to clear the interrupts of MCPWM
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 *        \ref  sl_si91x_pwm_set_base_timer_mode \n
 *        \ref  sl_si91x_pwm_control_period \n
 *        \ref  sl_si91x_pwm_register_callback \n
 *        \ref  sl_si91x_pwm_start \n
 * @param[in]   flag :  It is the logical OR of different interrupts generated on multiple channels
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_clear_interrupt(uint32_t flag);

/***************************************************************************/ /**
 * @brief     This API is used to get the interrupt status of interrupt flags of MCPWM.
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 *        \ref  sl_si91x_pwm_set_base_timer_mode \n
 *        \ref  sl_si91x_pwm_control_period \n
 *        \ref  sl_si91x_pwm_register_callback \n
 *        \ref  sl_si91x_pwm_start \n
 * @param[in]   flag  :  Flag value
 * @param[in]   intr_status : Pointer to interrupt status
 * @return    Interrupt status of required interrupt flag
 ******************************************************************************/
sl_status_t sl_si91x_pwm_get_interrupt_status(uint32_t flag, uint16_t *intr_status);

/***************************************************************************/ /**
 * @brief       This API is used to control duty cycle control parameters for the required MCPWM channel
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 *        \ref  sl_si91x_pwm_set_base_timer_mode \n
 * @param[in]   duty_cycle : Enum of type \ref sl_pwm_duty_cycle_t
 * @param[in]   value   :  This can be logical OR of below parameters
                           - IMDT_DUTYCYCLE_UPDATE_EN :  Enable to update the duty cycle immediately
                           - DUTYCYCLE_UPDATE_DISABLE :  Duty cycle register updation disable
 * @param[in]   channel :  Channel number(1 to 4) of type \ref sl_pwm_channel_t
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_configure_duty_cycle(sl_pwm_duty_cycle_t duty_cycle, uint32_t value, sl_pwm_channel_t channel);

/***************************************************************************/ /**
 * @brief     This API is used to control the output override operation of MCPWM
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 *        \ref  sl_si91x_pwm_set_base_timer_mode \n
 * @param[in]   override : Enum of type \ref sl_pwm_override_t
 * @param[in]   pwm_output :  PWM output override of type \ref sl_pwm_output_t
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_output_override(sl_pwm_override_t override, sl_pwm_output_t pwm_output);

/***************************************************************************/ /**
 * @brief     This API is used to control the override control parameter,output is in sync with pwm time period
 *               depending on operating mode
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 *        \ref  sl_si91x_pwm_set_base_timer_mode \n
 * @param[in]   override : Enum of type \ref sl_pwm_override_t
 * @param[in]   value  : Output override to be in sync with pwm time period \ref sl_pwm_output_override_t
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_control_override(sl_pwm_override_t override, sl_pwm_output_override_t value);

/***************************************************************************/ /**
 * @brief     This API is used to control override value for the required output of MCPWM.
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 *        \ref  sl_si91x_pwm_set_base_timer_mode \n
 * @param[in]   override : Enum of type \ref sl_pwm_override_t
 * @param[in]   pwm_output : PWM output override of type \ref sl_pwm_output_t
 * @param[in]   value  :  override value of type \ref sl_pwm_override_value_t
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_control_override_value(sl_pwm_override_t override,
                                                sl_pwm_output_t pwm_output,
                                                sl_pwm_override_value_t value);

/***************************************************************************/ /**
 * @brief     This API is used to control output fault override control parameters for required PWM output
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 *        \ref  sl_si91x_pwm_set_base_timer_mode \n
 *        \ref  sl_si91x_pwm_control_period \n
 *        \ref  sl_si91x_pwm_register_callback \n
 * @param[in]   output_fault : Enum of type \ref sl_pwm_output_fault_t
 * @param[in]   value   :  This can be logical OR of below parameters
                           -  FLT_A_MODE      : if bit one then cycle by cycle by mode and zero then latched mode
                           -  FLT_B_MODE      : if bit one then cycle by cycle by mode and zero then latched mode
                           -  OP_POLARITY_H Ouput polarity for high (H3, H2, H1, H0) side signals.
                           \n if bit 0 then in active low mode and 1 then active high mode.
                           -  OP_POLARITY_L Ouput polarity for low (L3, L2, L1, L0) side signals.
                           \n if bit 0 then in active low mode and 1 then active high mode.
                           -  FLT_A_ENABLE    :  enable fault A
                           -  FLT_B_ENABLE    :  enable fault B
                           -  COMPLEMENT_MODE :  PWM I/O pair mode
                           \n if bit is 1 then PWM I/O pin pair is in the complementary output mode
                           \n if bit is 0 then PWM I/O pin pair is in the independent output mode
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_control_output_fault(sl_pwm_output_fault_t output_fault, uint32_t value);

/***************************************************************************/ /**
 * @brief     This API is used to control generation of special event trigger for required channel of MCPWM
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 *        \ref  sl_si91x_pwm_set_base_timer_mode \n
 * @param[in] event : Enum of type \ref sl_pwm_event_t
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_control_special_event_trigger(sl_pwm_event_t event);

/***************************************************************************/ /**
 * @brief     This API is used to control dead time control parameters for the required channel.
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 *        \ref  sl_si91x_pwm_set_base_timer_mode \n
 * @param[in]   dead_time : Enum of type \ref sl_pwm_dead_time_t
 * @param[in]   value   :  This can be logical OR of below parameters
               - DEADTIME_SELECT_ACTIVE  : Deadtime select bits for PWM going active
                             Possible values are as below if bit zero then use counter A , if one then use counter B
               - DEADTIME_SELECT_INACTIVE : Deadtime select bits for PWM going inactive
                             Possible values are as below if bit zero then use counter A , if one then use counter B
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_select_dead_time(sl_pwm_dead_time_t dead_time, uint32_t value);

/***************************************************************************/ /**
 * @brief   This API is used to select duty cycle for the required MCPWM channel.
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 * @param[in] duty_cycle :  Duty cycle value
 * @param[in] channel  :  channel number(1 to 4) of type \ref sl_pwm_channel_t
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_set_duty_cycle(uint32_t duty_cycle, sl_pwm_channel_t channel);

/***************************************************************************/ /**
 * @brief   This API is used to get duty cycle for the required MCPWM channel.
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 * @param[in] channel  :  channel number(1 to 4) of type \ref sl_pwm_channel_t
 * @param[in] duty_cycle :  Duty cycle value
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_get_duty_cycle(sl_pwm_channel_t channel, uint32_t *duty_cycle);

/***************************************************************************/ /**
 * @brief     This API is used to enable to use external trigger for base time counter
 *                increment or decrement of MCPWM
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 *        \ref  sl_si91x_pwm_set_base_timer_mode \n
 * @param[in]   enable  :  Controlling external trigger of type \ref sl_pwm_trigger_t
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_enable_external_trigger(sl_pwm_trigger_t enable);

/***************************************************************************/ /**
 * @brief    Get time period for required channel.
 * @pre   \ref  sl_si91x_pwm_init \n
 *        \ref  sl_si91x_pwm_set_configuration \n
 * @param[in]   channel  : channel number(1-4) of type \ref sl_pwm_channel_t
 * @param[out]  time_period  :  pointer to read time period.
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_INVALID_PARAMETER - The parameter is invalid argument \n
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_get_time_period(sl_pwm_channel_t channel, uint16_t *time_period);

/***************************************************************************/ /**
 * @brief    This API is used for initialize PWM pins and clock
 * @param[in] pwm_init : Pointer to structure of type \ref sl_pwm_init_t
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_init(sl_pwm_init_t *pwm_init);

/*******************************************************************************
 * This API is used for initialize PWM event pins
 * @param[in] pwm_fault : Pointer to structure of type \ref sl_pwm_fault_init_t
 * @return    returns status 0 if successful,
 *                 else error code.
 *                \ref SL_STATUS_OK - Success \n
 *                \ref SL_STATUS_NULL_POINTER - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_pwm_fault_init(sl_pwm_fault_init_t *pwm_fault);

/** @} (end addtogroup PWM) */

#ifdef __cplusplus
}
#endif

#endif ///< SL_SI91X_PWM_H
/**************************************************************************************************/
