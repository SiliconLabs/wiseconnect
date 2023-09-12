/*******************************************************************************
 * @file  blinky.c
 * @brief
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
/**============================================================================
 * @section Description
 * This files contains example code to demonstrate the GPIO/LED toggle functionality.
 ============================================================================**/

// Include Files
#include "rsi_ccp_user_config.h"
#include "rsi_chip.h"
#include "rsi_board.h"

#define RSI_BLINK_RATE (10) // 10 ticks per second

/*==============================================*/
/**
 * @fn      void SysTick_Handler(void)
 * @brief	Handle interrupt from SysTick timer
 * @return	None
 */
void SysTick_Handler(void)
{
  //  Toggles the current state of a board '0' number LED.
  RSI_Board_LED_Toggle(0);
}

/*==============================================*/
/**
 * @fn     int main(void)
 * @brief  Main Application Function
 * @return None
 */
int blinky_init(void)
{
  int forever = 1;
  //Configures the system default clock and power configurations
  SystemCoreClockUpdate();

  /* Enable the DEBUG UART port for debug prints and  Set up and initialize all required
        blocks and functions  related to the board hardware. */
  RSI_Board_Init();

  // Sets the state of a board '0'number LED to off(false).
  RSI_Board_LED_Set(0, false);

  // Sets the state of a board '1'number LED to on(true).
  RSI_Board_LED_Set(1, true);

  // Sets the state of a board '2'number LED to on(true).
  RSI_Board_LED_Set(2, true);

  // Enable SysTick Timer
  SysTick_Config(SystemCoreClock / RSI_BLINK_RATE);

  // LED toggling and debug prints for ever
  while (forever) {
    //Sleep till interrupt occurs
#ifdef DEBUG_UART
    // Prints on hyper-terminal as "Core is Sleeping...\n"
    DEBUGOUT("Core is Sleeping ...\n");
#endif
    // Sleep untill next interrupt occurres
    __WFI();
  }
  // Statement will never reach here , just to satisfy the standard main
  return 0;
}