/*******************************************************************************
* @file  sl_adc_board.h
* @brief sl adc board specific configuration
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/

#ifndef __SL_ADC_BOARD_H__
#define __SL_ADC_BOARD_H__
/**
 * @defgroup  Board_Specific_Driver   Board specific drivers and support functions
 * @{
 */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @defgroup  Board_Drivers BOARD: Common board components used with board drivers
 * @{
 */

/* GPIO selection */
#define SL_ADC_GPIO_0      0
#define SL_ADC_GPIO_1      1
#define SL_ADC_GPIO_2      2
#define SL_ADC_GPIO_3      3
#define SL_ADC_GPIO_4      4
#define SL_ADC_GPIO_5      5
#define SL_ADC_GPIO_6      6
#define SL_ADC_GPIO_7      7
#define SL_ADC_GPIO_8      8
#define SL_ADC_GPIO_9      9
#define SL_ADC_GPIO_10     10
#define SL_ADC_GPIO_11     11
#define SL_ADC_GPIO_12     12
#define SL_ADC_GPIO_13     13
#define SL_ADC_GPIO_14     14
#define SL_ADC_GPIO_15     15
#define SL_ADC_TOP_GPIO_23 23
#define SL_ADC_TOP_GPIO_24 24
#define SL_ADC_TOP_GPIO_25 25
#define SL_ADC_TOP_GPIO_26 26
#define SL_ADC_TOP_GPIO_27 27
#define SL_ADC_TOP_GPIO_28 28
#define SL_ADC_TOP_GPIO_29 29
#define SL_ADC_TOP_GPIO_30 30

/* Pin Mode selection */
#define SL_ANALOG_MODE          7
#define SL_TOP_GPIO_ANALOG_MODE 14

#ifdef __cplusplus
}
#endif

#endif /*__SL_ADC_BOARD_H__*/

/* @} end of Board_Drivers */
/* @} end of Board_Specific_Driver */
