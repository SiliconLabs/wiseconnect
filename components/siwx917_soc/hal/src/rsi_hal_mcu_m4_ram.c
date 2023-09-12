/*******************************************************************************
* @file  rsi_hal_mcu_m4_ram.c
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

#ifdef CHIP_917
//! This file should be in RAM
#include "rsi_driver.h"
#include "rsi_m4.h"
#include "rsi_board.h"

/*==================================================*/
/**
 * @fn          void sl_mv_m4_app_from_flash_to_ram(int option)
 * @brief       Raise interrupt to TA and poll for task done 
 * @param[in]   option
 * @param[out]  none  
 */

void sl_mv_m4_app_from_flash_to_ram(int option)
{

  //! Disable all interrupts
  __disable_irq();

  if (option == UPGRADE_M4_IMAGE_OTA) {
    //! Raise interrupt to TA
    ROM_WL_raise_m4_to_ta_interrupt(UPGRADE_M4_IMAGE);

    //! Poll for bit to clear
    while ((M4SS_P2P_INTR_CLR_REG & UPGRADE_M4_IMAGE))
      ;
  } else if (option == TA_WRITES_ON_COMM_FLASH) {
    //! Raise interrupt to TA
    ROM_WL_raise_m4_to_ta_interrupt(M4_WAITING_FOR_TA_TO_WR_ON_FLASH);

    //! Poll for bit to clear
    while (M4SS_P2P_INTR_CLR_REG & M4_WAITING_FOR_TA_TO_WR_ON_FLASH)
      ;
  }

  //! Enable all interrupts
  __enable_irq();
}

#endif
