/*******************************************************************************
* @file  rsi_rom_efuse.h
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

#ifndef __RSI_ROM_EFUSE_H__
#define __RSI_ROM_EFUSE_H__

/**
 * \ingroup   RSI_SPECIFIC_DRIVERS
 * \defgroup RSI_EFUSE_DRIVER  RSI:RS1xxxx EFUSE
 *  @{
 *
 */
#include "rsi_ccp_user_config.h"
#include "rsi_packing.h"
#if defined(A11_ROM)
#include "rsi_rom_table_si91x.h"
#else
#include "rsi_rom_table_RS1xxxx.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @fn           STATIC INLINE void  RSI_EFUSE_Enable(EFUSE_Type* pstcEfuse)
 * @brief        This API Is Used To Enable The EFUSE
 * @param[in]    pstcEfuse  :Pointer to the EFUSE register instance
 * @return       None
 */
STATIC INLINE void RSI_EFUSE_Enable(EFUSE_Type *pstcEfuse)
{
#if defined(ROMDRIVER_PRESENT) && !defined(CHIP_917B0)
  ROMAPI_EFUSE_API->efuse_enable(pstcEfuse);
#else
  efuse_enable(pstcEfuse);
#endif
}

/**
 * @fn           STATIC INLINE void  RSI_EFUSE_Disable(EFUSE_Type* pstcEfuse)
 * @brief        This API is used to Disable the EFUSE
 * @param[in]    pstcEfuse  :Pointer to the EFUSE register instance
 * @return       None
 */
STATIC INLINE void RSI_EFUSE_Disable(EFUSE_Type *pstcEfuse)
{
#if defined(ROMDRIVER_PRESENT) && !defined(CHIP_917B0)
  ROMAPI_EFUSE_API->efuse_Disable(pstcEfuse);
#else
  efuse_Disable(pstcEfuse);
#endif
}

/**
 * @fn           STATIC INLINE uint8_t  RSI_EFUSE_ReadData(EFUSE_Type* pstcEfuse)
 * @brief        This API is used to read the eFUSE data
 * @param[in]    pstcEfuse   : Pointer to the EFUSE register instance
 * @return       eFUSE data
 */
STATIC INLINE uint8_t RSI_EFUSE_ReadData(EFUSE_Type *pstcEfuse)
{
#if defined(ROMDRIVER_PRESENT) && !defined(CHIP_917B0)
  return ROMAPI_EFUSE_API->efuse_read_data(pstcEfuse);
#else
  return efuse_read_data(pstcEfuse);
#endif
}

/**
 * @fn           STATIC INLINE void  RSI_EFUSE_WriteAddr(EFUSE_Type* pstcEfuse , uint16_t u16Addr)
 * @brief        This API is used to set the eFUSE address for read and write operations
 * @param[in]    pstcEfuse   : Pointer to the EFUSE register instance
 * @param[in]    u16Addr     : EFUSE address
 * @return       None
 */
STATIC INLINE void RSI_EFUSE_WriteAddr(EFUSE_Type *pstcEfuse, uint16_t u16Addr)
{
#if defined(ROMDRIVER_PRESENT) && !defined(CHIP_917B0)
  ROMAPI_EFUSE_API->efuse_write_addr(pstcEfuse, u16Addr);
#else
  efuse_write_addr(pstcEfuse, u16Addr);
#endif
}

/**
 * @fn           STATIC INLINE error_t RSI_EFUSE_WriteBit(EFUSE_Type *pstcEfuse , uint16_t u16Addr ,
 *                                                         uint8_t u8BitPos,uint32_t hold_time)
 * @brief        Any Bit in this macro can be programmed in any order by raising STROBE high for aperiod specified
 *               by TPGM with a proper address selected.There is only one programming scheme,which is
 *               single bit programming
 * @param[in]    pstcEfuse   : Pointer to the EFUSE register instance
 * @param[in]    u16Addr     : Address of EFUSE to the data
 * @param[in]    u8BitPos    : position of bit to be written in 8 bit location in address
 * @param[in]    hold_time   : count value depends on clock frequency of EFUSE controller
 * @return       Nonzero     : If fails
 *               \n 0        : If success
 */
STATIC INLINE error_t RSI_EFUSE_WriteBit(EFUSE_Type *pstcEfuse, uint16_t u16Addr, uint8_t u8BitPos, uint32_t hold_time)
{
#if defined(ROMDRIVER_PRESENT) && !defined(CHIP_917B0)
  return ROMAPI_EFUSE_API->efuse_write_bit(pstcEfuse, u16Addr, u8BitPos, hold_time);
#else
  return efuse_write_bit(pstcEfuse, u16Addr, u8BitPos, hold_time);
#endif
}

/**
 * @fn           STATIC INLINE error_t RSI_EFUSE_FsmReadByte(EFUSE_Type *pstcEfuse , uint16_t u16Addr ,
 *                                                            uint8_t *pu8Byte ,uint32_t SocClk)
 * @brief        This API is used to read the data from 32x8 byte eFUSE memory(OTP) in fsm mode
 * @param[in]    pstcEfuse  : Pointer to the EFUSE register instance
 * @param[in]    u16Addr    : byte address to be read
 * @param[out]   pu8Byte    : pointer to hold the read byte
 * @param[in]    SocClk     : clock frequency of EFUSE controller
 * @return       Non zero   : If fails
 *               \n 0       : If success
 */
STATIC INLINE error_t RSI_EFUSE_FsmReadByte(EFUSE_Type *pstcEfuse, uint16_t u16Addr, uint8_t *pu8Byte, uint32_t SocClk)
{
#if defined(ROMDRIVER_PRESENT) && !defined(CHIP_917B0)
  return ROMAPI_EFUSE_API->efuse_fsm_read_byte(pstcEfuse, u16Addr, pu8Byte, SocClk);
#else
  return efuse_fsm_read_byte(pstcEfuse, u16Addr, pu8Byte, SocClk);
#endif
}

/**
 * @fn           STATIC INLINE error_t RSI_EFUSE_MemMapReadByte(EFUSE_Type *pstcEfuse , uint16_t u16Addr ,
 *                                                             uint8_t *pu8Byte ,uint32_t SocClk)
 * @brief        This API is used to read the data from 32x8 byte eFUSE memory(OTP) in memory mapped mode
 * @param[in]    pstcEfuse : Pointer to the EFUSE register instance
 * @param[in]    u16Addr   : byte address to be read
 * @param[out]   pu8Byte   : pointer to hold the read byte
 * @param[in]    SocClk    : clock frequency of EFUSE controller
 * @return       Non zero  : If fails
 *               \n 0      : If success
 */
STATIC INLINE error_t RSI_EFUSE_MemMapReadByte(EFUSE_Type *pstcEfuse,
                                               uint16_t u16Addr,
                                               uint8_t *pu8Byte,
                                               uint32_t SocClk)
{
#if defined(ROMDRIVER_PRESENT) && !defined(CHIP_917B0)
  return ROMAPI_EFUSE_API->efuse_mem_map_read_byte(pstcEfuse, u16Addr, pu8Byte, SocClk);
#else
  return efuse_mem_map_read_byte(pstcEfuse, u16Addr, pu8Byte, SocClk);
#endif
}

/**
 * @fn           STATIC INLINE error_t RSI_EFUSE_MemMapReadWord(EFUSE_Type *pstcEfuse , uint16_t u16Addr ,
 *                                                            uint16_t *pu16Word ,uint32_t SocClk)
 * @brief        This API is used to Read the 1 word(16 bits) of data to EFUSE macro
 * @param[in]    pstcEfuse : Pointer to the EFUSE register instance
 * @param[in]    u16Addr   : Address of EFUSE to the data
 * @param[out]   pu16Word  : pointer to the buffer to hold the read byte
 * @param[in]    SocClk    : clock frequency of EFUSE controller
 * @return       Non zero  : If fails
 *               \n 0      : If success
 */
STATIC INLINE error_t RSI_EFUSE_MemMapReadWord(EFUSE_Type *pstcEfuse,
                                               uint16_t u16Addr,
                                               uint16_t *pu16Word,
                                               uint32_t SocClk)
{
#if defined(ROMDRIVER_PRESENT) && !defined(CHIP_917B0)
  return ROMAPI_EFUSE_API->efuse_mem_map_read_word(pstcEfuse, u16Addr, pu16Word, SocClk);
#else
  return efuse_mem_map_read_word(pstcEfuse, u16Addr, pu16Word, SocClk);
#endif
}

#ifdef __cplusplus
}
#endif

#endif /*__RSI_ROM_EFUSE_H__*/

/* @} end of  RSI_EFUSE_DRIVER */
