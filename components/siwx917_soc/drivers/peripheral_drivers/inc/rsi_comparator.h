/*******************************************************************************
* @file  rsi_comparator.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

// Includes Files

#include "rsi_chip.h"

#ifndef RSI_COMPARATOR_H
#define RSI_COMPARATOR_H

#ifdef __cplusplus
extern "C" {
#endif

//cond HIDDEN_SYMBOLS
#define MAX_NUM_OF_COMPARATORS 2
#define MIN_NUM_OF_COMPARATORS 1
#define MAX_P_SEL_MUX          15
#define MAX_N_SEL_MUX          15
#define MAX_NO_OF_HYST         3
#define MAX_THRSHOLD_VALUE     31
#define MAX_SCALAR_FACTOR      15
#define COMP1                  1
#define COMP2                  2

#define AGPIO_PIN0  0
#define AGPIO_PIN1  1
#define AGPIO_PIN2  2
#define AGPIO_PIN3  3
#define AGPIO_PIN4  4
#define AGPIO_PIN5  5
#define AGPIO_PIN6  6
#define AGPIO_PIN7  7
#define AGPIO_PIN8  8
#define AGPIO_PIN9  9
#define AGPIO_PIN10 10
#define AGPIO_PIN11 11
#define AGPIO_PIN12 12
#define AGPIO_PIN13 13
#define AGPIO_PIN14 14
#define AGPIO_PIN15 15

#define TGPIO_PIN0 25
#define TGPIO_PIN1 26
#define TGPIO_PIN2 27
#define TGPIO_PIN3 28
#define TGPIO_PIN4 29
#define TGPIO_PIN5 30
#define TGPIO_PIN6 23
#define TGPIO_PIN7 24

#define AGPIO_MODE 7
#define TGPIO_MODE 14

#define AGPIO_PORT        7
#define BG_SCDC_PROG_REG1 0x127

#define COMP1        1
#define COMP2        2
#define METAL_REV1P4 0x14

error_t RSI_COMP_Enable(AUX_ADC_DAC_COMP_Type *comp, uint8_t comp_number, uint8_t enable);

error_t RSI_COMP_Config(AUX_ADC_DAC_COMP_Type *comp,
                        uint8_t comp_number,
                        uint8_t sel_p_mux,
                        uint8_t sel_n_mux,
                        uint8_t hyst_en,
                        uint8_t filter_en);

error_t RSI_COMP_ResBank(AUX_ADC_DAC_COMP_Type *comp, uint16_t value_thrsh);

error_t RSI_COMP_ReferenceScaler(AUX_ADC_DAC_COMP_Type *comp, uint16_t scalar_factor_value);

error_t RSI_COMP_PinMux(uint8_t comp_num, uint8_t pos_pin, uint8_t neg_pin);

error_t RSI_COMP_IntrEnableDisable(uint8_t comp_number, uint8_t enable);

error_t RSI_COMP_OutputMaskConfig(uint8_t comp_number, uint8_t MaskConfig);
#ifdef __cplusplus
}
#endif

#endif // RSI_COMPARATOR_H
