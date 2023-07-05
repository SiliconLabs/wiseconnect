/*******************************************************************************
* @file  rsi_wwdt.c
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

/*==============================================*/
/**
 * @fn           void RSI_WWDT_Init(MCU_WDT_Type *pstcWDT)
 * @brief        This API is used to initialize the Watch dog timer
 * @param[in]    pstcWDT : pointer to the WDT register instance
 * @return       none
 */

void RSI_WWDT_Init(MCU_WDT_Type *pstcWDT)
{
  RSI_PS_NpssPeriPowerUp(SLPSS_PWRGATE_ULP_MCUWDT | SLPSS_PWRGATE_ULP_TIMEPERIOD);
  MCU_FSM->MCU_FSM_CRTL_PDM_AND_ENABLES_b.ENABLE_WDT_IN_SLEEP_b = 1;
}

/*==============================================*/
/**
 * @fn           void RSI_WWDT_IntrClear(void)
 * @brief        This API is used to clear the WDT interrupt
 * @return       none
 */

void RSI_WWDT_IntrClear(void)
{
  NPSS_INTR_CLEAR_REG                  = NPSS_TO_MCU_WDT_INTR;
  MCU_FSM->MCU_FSM_WAKEUP_STATUS_CLEAR = WWD_INTERRUPT_STATUS_CLEAR;
  return;
}

/*==============================================*/
/**
 * @fn           uint8_t RSI_WWDT_GetIntrStatus(void)
 * @brief        This API is used to get the WDT interrupt status
 * @return       returns the WDT interrupt status.
 */

uint8_t RSI_WWDT_GetIntrStatus(void)
{
  if (NPSS_INTR_STATUS_REG & NPSS_TO_MCU_WDT_INTR) {
    return 1;
  } else {
    return 0;
  }
}

/*==============================================*/
/**
 * @fn           void RSI_WWDT_DeInit(MCU_WDT_Type *pstcWDT)
 * @brief        This API is used to disable the WDT.
 * @param[in]    pstcWDT : pointer to the WDT register instance
 * @return       none
 */

void RSI_WWDT_DeInit(MCU_WDT_Type *pstcWDT)
{
  RSI_WWDT_Disable(pstcWDT);
  return;
}

/*==============================================*/
/**
 * @fn           void RSI_WWDT_Start(MCU_WDT_Type *pstcWDT)
 * @brief        This API is used to start the WDT.
 * @param[in]    pstcWDT : pointer to the WDT register instance
 * @return       none
 */

void RSI_WWDT_Start(MCU_WDT_Type *pstcWDT)
{
  /*CLEAR MASK*/
  /*0xAA to enable the watch dog */
  pstcWDT->MCU_WWD_MODE_AND_RSTART_b.WWD_MODE_EN_STATUS = 0xAA;
  pstcWDT->MCU_WWD_MODE_AND_RSTART_b.WWD_MODE_RSTART    = 1;
  return;
}

/*End of file not truncated*/
