/*******************************************************************************
* @file  rsi_efuse.c
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

// Include Files

#include "rsi_ccp_user_config.h"
#if !defined(ROMDRIVER_PRESENT)
#include "rsi_rom_efuse.h"

/*==============================================*/
/**
 * @fn        void efuse_enable(EFUSE_Type *pstcEfuse)
 * @brief     This API Is Used To Enable The EFUSE
 * @param[in] pstcEfuse - Pointer to the EFUSE_TYPE
 * @return    None
 */
void efuse_enable(EFUSE_Type *pstcEfuse)
{
  pstcEfuse->EFUSE_CTRL_REG_b.EFUSE_ENABLE = 1U;
}

/*==============================================*/
/**
 * @fn        void efuse_disable(EFUSE_Type *pstcEfuse)
 * @brief     This API is used to Disable the EFUSE
 * @param[in] pstcEfuse - Pointer to the EFUSE_TYPE
 * @return    None
 */
void efuse_Disable(EFUSE_Type *pstcEfuse)
{
  pstcEfuse->EFUSE_CTRL_REG_b.EFUSE_ENABLE = 0U;
}

/*==============================================*/
/**
 * @fn        uint8_t efuse_read_data(EFUSE_Type *pstcEfuse)
 * @brief     This API is used to read the eFUSE data
 * @param[in] pstcEfuse - Pointer to the EFUSE_TYPE
 * @return    eEfuse data
 */
uint8_t efuse_read_data(EFUSE_Type *pstcEfuse)
{
  return (pstcEfuse->EFUSE_READ_DATA_REG & 0xFF);
}

/*==============================================*/
/**
 * @fn        void efuse_write_addr(EFUSE_Type *pstcEfuse, uint16_t u16Addr)
 * @brief     This API is used to set the eFUSE address for read and write operations
 * @param[in] pstcEfuse - Pointer to the EFUSE_TYPE ( EFUSE register instance)
 * @param[in] u16Addr   - Parameter refers to the address 
 * @return    None
 */
void efuse_write_addr(EFUSE_Type *pstcEfuse, uint16_t u16Addr)
{
  // Write address into address register
  pstcEfuse->EFUSE_DA_ADDR_REG |= (u16Addr & 0XFFFF);
}

/* Any Bit in this macro can be programmed in any order by raising STROBE high for aperiod specified  by TPGM with a proper address selected.
   There is only one programming scheme,which is single bit programming */
error_t efuse_write_bit(EFUSE_Type *pstcEfuse, uint16_t u16Addr, uint8_t u8BitPos, uint32_t hold_time)
{
  // Check for valid parameters
  if (pstcEfuse == NULL) {
    return ERROR_EFUSE_INVALID_PARAMETERS;
  }
  if (u16Addr > ADDRESS_MAX) {
    return ERROR_EFUSE_INVALID_WRITE_ADDRESS;
  }
  if (u8BitPos > BIT_POS_MAX) {
    return ERROR_EFUSE_INVALID_WRITE_BIT_POSITION;
  }

  // Direct access enable
  pstcEfuse->EFUSE_CTRL_REG = 0x06;

  // Lowering PGENB,CSB and LOAD with direct access
  pstcEfuse->EFUSE_DA_CTRL_CLEAR_REG = 0x0B;

  // Address with bit position
  u16Addr |= (u8BitPos << 5);

  // Fill the address register
  pstcEfuse->EFUSE_DA_ADDR_REG = u16Addr;

  // Setting STROBE with direct access
  pstcEfuse->EFUSE_DA_CTRL_SET_REG = 0x04;

  // Fill the eFUSE strobe hold count register
  // 32Mhz as base clock 2us hold time
  pstcEfuse->EFUSE_DA_CLR_STROBE_REG = (BIT(9) | hold_time);

  while (!(pstcEfuse->EFUSE_STATUS_REG & 0x0400))
    ;

  // Setting CSB and LOAD  with direct access
  pstcEfuse->EFUSE_DA_CTRL_SET_REG = 0x0A;

  // waiting for THP_A  (A7~A0 to STROBE Set time into Program mode)
  // Setting  PGENB with direct access
  pstcEfuse->EFUSE_DA_CTRL_SET_REG = 0x01;

  pstcEfuse->EFUSE_CTRL_REG &= 0x02;

  return RSI_OK;
}

/*==============================================*/
/**
 * @fn        error_t efuse_fsm_read_byte(EFUSE_Type *pstcEfuse, uint16_t u16Addr, uint8_t *pu8Byte, uint32_t SocClk)
 * @brief     This API is used to read the data from 32x8 byte eFUSE memory(OTP) in fsm mode
 * @param[in] pstcEfuse - Pointer to the EFUSE register instance
 * @param[in] u16Addr   - byte address to be read
 * @param[in] pu8Byte   - pointer to hold the read byte
 * @param[in] SocClk    - clock frequency of EFUSE controller
 * @return    Non zero  : If fails
 *               \n 0   : If success
 */

error_t efuse_fsm_read_byte(EFUSE_Type *pstcEfuse, uint16_t u16Addr, uint8_t *pu8Byte, uint32_t SocClk)
{
  float read_time;
  uint8_t read_parameter;
  read_time      = ((1 / SocClk) * (1000));
  read_parameter = 42 / read_time;
  /* Check for valid parameters */
  if (pstcEfuse == NULL) {
    return ERROR_EFUSE_INVALID_PARAMETERS;
  }
  if (u16Addr > ADDRESS_MAX) {
    return ERROR_EFUSE_INVALID_WRITE_ADDRESS;
  }
  // Set eFUSE time
  pstcEfuse->EFUSE_RD_TMNG_PARAM_REG = ((0x5 << 8) | (read_parameter << 4) | (0x1 << 0));

  // 10 bit address with msb made 1 to enable read state machine
  pstcEfuse->EFUSE_READ_ADDR_REG = (0x8000 | u16Addr);

  // Enable eFUSE
  pstcEfuse->EFUSE_CTRL_REG = 0x1;

  // Wait for FSM data read done
  while (!(((pstcEfuse->EFUSE_READ_DATA_REG)) & READ_FSM_DONE))
    ;

  // Disable eFUSE
  pstcEfuse->EFUSE_CTRL_REG = 0x0;

  // Read eFUSE Data and copy to the application pointer
  *pu8Byte = efuse_read_data(pstcEfuse);

  return RSI_OK;
}

/*==============================================*/
/**
 * @fn        error_t efuse_mem_map_read_byte(EFUSE_Type *pstcEfuse, uint16_t u16Addr, uint8_t *pu8Byte, uint32_t SocClk)
 * @brief     This API is used to read the data from 32x8 byte eFUSE memory(OTP) in memory mapped mode
 * @param[in] pstcEfuse - Pointer to the EFUSE register instance
 * @param[in] u16Addr   - byte address to be read
 * @param[in] pu8Byte   - pointer to hold the read byte
 * @param[in] SocClk    - clock frequency of EFUSE controller
 * @return    Non zero  : If fails
 *               \n 0   : If success
 */
error_t efuse_mem_map_read_byte(EFUSE_Type *pstcEfuse, uint16_t u16Addr, uint8_t *pu8Byte, uint32_t SocClk)
{
  float read_time;
  uint8_t read_parameter;
  read_time      = ((1 / SocClk) * (1000));
  read_parameter = 42 / read_time;
  // Check for valid parameters
  if (pstcEfuse == NULL) {
    return ERROR_EFUSE_INVALID_PARAMETERS;
  }
  if (u16Addr > ADDRESS_MAX) {
    return ERROR_EFUSE_INVALID_WRITE_ADDRESS;
  }
  pstcEfuse->EFUSE_RD_TMNG_PARAM_REG = ((0x5 << 8) | (read_parameter << 4) | (0x1 << 0));

  // enable
  pstcEfuse->EFUSE_CTRL_REG = 0x1;

  // Configure memory MAP read in byte mode
  pstcEfuse->EFUSE_MEM_MAP_LENGTH_REG = 0x0000;

  // Read byte from memory
  *pu8Byte = *((uint8_t volatile *)(EFUSE_BASE + 0x2000 + u16Addr));

  return RSI_OK;
}

/*==============================================*/
/**
 * @fn        error_t efuse_mem_map_read_word(EFUSE_Type *pstcEfuse, uint16_t u16Addr, uint16_t *pu16Word, uint32_t SocClk)
 * @brief     This API is used to Read the 1 word(16 bits) of data to EFUSE macro
 * @param[in] pstcEfuse - Pointer to the EFUSE register instance
 * @param[in] u16Addr   - byte address to be read
 * @param[in] pu8Byte   - pointer to hold the read byte
 * @param[in] SocClk    - clock frequency of EFUSE controller
 * @return    Non zero  : If fails
 *               \n 0   : If success  
 */

error_t efuse_mem_map_read_word(EFUSE_Type *pstcEfuse, uint16_t u16Addr, uint16_t *pu16Word, uint32_t SocClk)
{
  float read_time;
  uint8_t read_parameter;
  read_time      = ((1 / SocClk) * (1000));
  read_parameter = 42 / read_time;
  // Check for valid parameters
  if (pstcEfuse == NULL) {
    return ERROR_EFUSE_INVALID_PARAMETERS;
  }
  if (u16Addr > ADDRESS_MAX) {
    return ERROR_EFUSE_INVALID_WRITE_ADDRESS;
  }
  pstcEfuse->EFUSE_RD_TMNG_PARAM_REG = ((0x5 << 8) | (read_parameter << 4) | (0x1 << 0));

  // eFUSE enable
  pstcEfuse->EFUSE_CTRL_REG = 0x1;

  // Select 16 bit read mode
  pstcEfuse->EFUSE_MEM_MAP_LENGTH_REG = 0x0001;

  // Read from address
  *pu16Word = *((uint16_t volatile *)(EFUSE_BASE + 0x2000 + u16Addr));
  return RSI_OK;
}

// This creates a table with the addresses of all the functions
// This table of function pointers is the API interface.
const ROM_EFUSE_API_T efuse_api = {

  &efuse_enable,    &efuse_Disable,       &efuse_read_data,         &efuse_write_addr,
  &efuse_write_bit, &efuse_fsm_read_byte, &efuse_mem_map_read_byte, &efuse_mem_map_read_word
};
#endif
