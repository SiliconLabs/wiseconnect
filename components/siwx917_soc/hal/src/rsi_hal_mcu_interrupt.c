/*******************************************************************************
* @file  rsi_hal_mcu_interrupt.c
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

#include "rsi_driver.h"

/** @addtogroup SOC4
* @{
*/
/*===================================================*/
/**
 * @fn          void rsi_hal_intr_config(void (*rsi_interrupt_handler)(void))
 * @brief       HAL API should contain the code to initialize the register/pins related to interrupts and enable the interrupts.
 * @param[in]   rsi_interrupt_handler() - call back function to handle interrupt
 * @return      void 
 */

void rsi_hal_intr_config(void (*rsi_interrupt_handler)(void))
{

  // Configure interrupt pin/register in input mode and register the interrupt handler

  return;
}
/*===================================================*/
/** 
 * @fn           void rsi_hal_intr_mask(void)
 * @brief        Disable the SPI Interrupt.This HAL API should contain the code to mask/disable interrupts.
 * @param[in]    void
 * @return       void 
 */

void rsi_hal_intr_mask(void)
{

  // Mask/Disable the interrupt

  return;
}

/*===================================================*/
/**
 * @fn           void rsi_hal_intr_unmask(void)
 * @brief        This HAL API should contain the code to enable interrupts.
 * @pre 		 None
 * @param[in]    none  
 * @param[out]   none
 * @return       none
 */

void rsi_hal_intr_unmask(void)
{

  // Unmask/Enable the interrupt

  return;
}

/*===================================================*/
/**
 * @fn           void rsi_hal_intr_clear(void)
 * @brief        HAL API should contain the code to clear the handled interrupts.
 * @param[in]    void 
 * @return       void 
 */

void rsi_hal_intr_clear(void)
{
  // Clear the interrupt

  return;
}

/*===================================================*/
/**
 * @fn          void rsi_hal_intr_pin_status(void)
 * @brief       Check interrupt pin status(pin level whether it is high/low).
 * @param[in]   void  
 * @param[out]  uint8_t interrupt status 
 * @return      void 
 * 
 */

uint8_t rsi_hal_intr_pin_status(void)
{

  volatile uint8_t status = 0;

  // Return interrupt pin  status(high(1) /low (0))

  return status;
}
/** @} */
