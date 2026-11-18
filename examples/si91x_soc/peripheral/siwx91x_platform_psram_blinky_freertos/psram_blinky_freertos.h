/***************************************************************************/ /**
 * @file psram_blinky_freertos.h
 * @brief PSRAM blinky FreeRTOS example functions
 *******************************************************************************
 * # License
 * <b>Copyright 2026 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef PSRAM_BLINKY_FREERTOS_H_
#define PSRAM_BLINKY_FREERTOS_H_

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
 * @brief Creates the PSRAM blinky FreeRTOS task (`psram_blinky_freertos.c`).
 *
 * @details Default thread attributes: stack 2048 bytes, `osPriorityLow1`.
 ******************************************************************************/
void psram_blinky_example_init(void);

#ifdef __cplusplus
}
#endif

#endif /* PSRAM_BLINKY_FREERTOS_H_ */
