/***************************************************************************/ /**
 * @file ulp_adc_freertos.h
 * @brief ULP ADC FreeRTOS example functions
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef ULP_ADC_FREERTOS_H_
#define ULP_ADC_FREERTOS_H_

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
 * @brief Creates the ULP ADC FreeRTOS task (`ulp_adc_freertos.c`).
 *
 * @details Default thread attributes: stack 3072 bytes, `osPriorityLow1`.
 ******************************************************************************/
void adc_example_init(void);

#ifdef __cplusplus
}
#endif

#endif /* ULP_ADC_FREERTOS_H_ */
