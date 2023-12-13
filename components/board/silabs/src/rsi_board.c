/*******************************************************************************
* @file  rsi_board.c
* @brief 
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

#include "rsi_chip.h"
#include "rsi_board.h"
#include "rsi_ccp_common.h"
#include <stdio.h>

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
typedef struct {
  uint8_t port;
  uint8_t pin;
} PORT_PIN_T;

#if ((defined(SLI_SI91X_MCU_CONFIG_RADIO_BOARD_BASE_VER)) && (defined(SLI_SI91X_MCU_CONFIG_RADIO_BOARD_VER2)))
static const PORT_PIN_T ledBits[] = { { 0, 2 }, { 0, 10 } };
#elif SLI_SI91X_MCU_CONFIG_RADIO_BOARD_BASE_VER
static const PORT_PIN_T ledBits[] = { { 0, 10 }, { 0, 8 } };
#else
#ifdef REV_1P2_CHIP
static const PORT_PIN_T ledBits[] = { { 0, 0 }, { 0, 1 }, { 0, 2 } };
#else
static const PORT_PIN_T ledBits[] = { { 0, 0 }, { 0, 2 }, { 0, 12 } };
#endif
#endif
static const uint32_t ledBitsCnt = sizeof(ledBits) / sizeof(PORT_PIN_T);

/**
 * @fn        void RSI_Board_Init(void)
 * @brief     Set up and initialize all required blocks and functions related to the board hardware.
 * @return    none
 */
void RSI_Board_Init(void)
{
  uint32_t i;
#if ((defined(SLI_SI91X_MCU_CONFIG_RADIO_BOARD_BASE_VER) && (SLI_SI91X_MCU_CONFIG_RADIO_BOARD_BASE_VER == 1)) \
     && (defined(SLI_SI91X_MCU_CONFIG_RADIO_BOARD_VER2) && (SLI_SI91X_MCU_CONFIG_RADIO_BOARD_VER2 == 1)))
  for (i = 0; i < ledBitsCnt; i++) {
    if (i == 0) {
      /*Set the GPIO pin MUX */
      RSI_EGPIO_SetPinMux(EGPIO1, ledBits[i].port, ledBits[i].pin, 0);
      /*Set GPIO direction*/
      RSI_EGPIO_SetDir(EGPIO1, ledBits[i].port, ledBits[i].pin, 0);
    } else {
      RSI_EGPIO_PadSelectionEnable(5);
      /*Set the GPIO pin MUX */
      RSI_EGPIO_SetPinMux(EGPIO, ledBits[i].port, ledBits[i].pin, 0);
      /*Set GPIO direction*/
      RSI_EGPIO_SetDir(EGPIO, ledBits[i].port, ledBits[i].pin, 0);
    }
  }
#elif SLI_SI91X_MCU_CONFIG_RADIO_BOARD_BASE_VER
  for (i = 0; i < ledBitsCnt; i++) {
    if (i == 0) {
      RSI_EGPIO_PadSelectionEnable(5);
      /*Set the GPIO pin MUX */
      RSI_EGPIO_SetPinMux(EGPIO, ledBits[i].port, ledBits[i].pin, 0);
      /*Set GPIO direction*/
      RSI_EGPIO_SetDir(EGPIO, ledBits[i].port, ledBits[i].pin, 0);
    } else {
      /*Set the GPIO pin MUX */
      RSI_EGPIO_SetPinMux(EGPIO1, ledBits[i].port, ledBits[i].pin, 0);
      /*Set GPIO direction*/
      RSI_EGPIO_SetDir(EGPIO1, ledBits[i].port, ledBits[i].pin, 0);
    }
  }
#else
  for (i = 0; i < ledBitsCnt; i++) {
    /*Set the GPIO pin MUX */
    RSI_EGPIO_SetPinMux(EGPIO1, ledBits[i].port, ledBits[i].pin, 0);
    /*Set GPIO direction*/
    RSI_EGPIO_SetDir(EGPIO1, ledBits[i].port, ledBits[i].pin, 0);
  }
#endif
  return;
}
