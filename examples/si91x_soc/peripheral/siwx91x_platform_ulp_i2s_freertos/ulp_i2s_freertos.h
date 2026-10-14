/***************************************************************************/ /**
 * @file ulp_i2s_freertos.h
 * @brief ULP I2S loopback FreeRTOS example functions
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
#ifndef ULP_I2S_FREERTOS_H_
#define ULP_I2S_FREERTOS_H_

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
 * @brief Creates the ULP I2S loopback FreeRTOS task (`ulp_i2s_freertos.c`).
 *
 * @details Default thread attributes: stack 10240 bytes, `osPriorityLow1`.
 ******************************************************************************/
void ulp_i2s_example_init(void);

#ifdef __cplusplus
}
#endif

#endif /* ULP_I2S_FREERTOS_H_ */
