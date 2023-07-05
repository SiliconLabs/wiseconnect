/*******************************************************************************
* @file  rsi_ir.c
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

// Include files

#include "rsi_chip.h"
#include "rsi_ir.h"

/*==============================================*/
/**
 * @fn           uint16_t RSI_IR_ReadData(IR_Type* pIr,uint16_t memory_address)
 * @brief        This API is used read IR address.
 * @param[in]    pIr  : IR type pointer
 * @param[in]    memory_address      : memory address value (0 .. 128).
 * @return       16-Bit IR data received (BIT[15] in received data will indicate the polarity of pulse)
 *               \n remaining bit will contain the incremented counter value of the pulse.
 */
uint16_t RSI_IR_ReadData(IR_Type *pIr, uint16_t memory_address)
{
  if (memory_address > MAX_MEMORY_ADDRESS) {
    return INVALID_PARAMETERS;
  }
  pIr->IR_MEM_ADDR_ACCESS_b.IR_MEM_ADDR  = memory_address;
  pIr->IR_MEM_ADDR_ACCESS_b.IR_MEM_RD_EN = 1U;
  return pIr->IR_MEM_READ_b.IR_MEM_DATA_OUT;
}

/*==============================================*/
/**
 * @fn           void RSI_IR_SoftwareRestart(IR_Type* pIr)
 * @brief        This API restart the IR operation. 
 * @param[in]    pIr  : IR type pointer
 */
void RSI_IR_SoftwareRestart(IR_Type *pIr)
{
  uint8_t i;
  pIr->IR_CONFIG_b.SREST_IR_CORE = 1U;
  for (i = 0; i < 10; i++) {
    __ASM("nop");
  }
  pIr->IR_CONFIG_b.SREST_IR_CORE = 0U;
}
