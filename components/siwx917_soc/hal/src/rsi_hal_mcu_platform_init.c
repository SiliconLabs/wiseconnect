/*******************************************************************************
* @file  rsi_hal_mcu_platform_init.c
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

/**
 * Includes
 */
#include "rsi_board.h"
#include "system_si91x.h"

/** @addtogroup SOC4
* @{
*/
/*==============================================*/
/**
 * @fn           void rsi_hal_board_init()
 * @brief        This function Initializes the platform
 * @param[in]    none 
 * @param[out]   none
 * @return       none
 * @section description
 * This function initializes the platform
 *
 */

void rsi_hal_board_init(void)
{
  SystemCoreClockUpdate();
#ifdef COMMON_FLASH_EN
  /* Before TA going to power save mode ,set m4ss_ref_clk_mux_ctrl ,tass_ref_clk_mux_ctrl, 
  AON domain power supply controls form TA to M4 */
  RSI_Set_Cntrls_To_M4();
#endif
#ifdef DEBUG_UART
  DEBUGINIT();
#endif
}
/** @} */
