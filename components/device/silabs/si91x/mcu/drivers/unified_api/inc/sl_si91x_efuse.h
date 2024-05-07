/***************************************************************************/ /**
* @file sl_si91x_efuse.h
* @brief EFUSE API implementation
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* SPDX-License-Identifier: Zlib
*
* The licensor of this software is Silicon Laboratories Inc.
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgment in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
******************************************************************************/

#ifndef SL_SI91X_EFUSE_H
#define SL_SI91X_EFUSE_H

#ifdef __cplusplus
extern "C" {
#endif

//// Includes
#include "rsi_rom_clks.h"
#include "rsi_rom_efuse.h"
#include "sl_status.h"
/***************************************************************************/ /**
 * @addtogroup EFUSE E-Fuse
 * @ingroup SI91X_PERIPHERAL_APIS
 * @{
 *
 ******************************************************************************/
/*******************************************************************************
 ***************************  DEFINES / MACROS  ********************************
 ******************************************************************************/

/*******************************************************************************
 ********************************   Local Variables   **************************
 ******************************************************************************/

/*******************************************************************************
 ***************************  Global  VARIABLES ********************************
 ******************************************************************************/

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/

/// @brief Structure to hold the versions number of peripheral API
typedef struct {
  uint8_t release; // Release version number
  uint8_t major;   // sqa version number
  uint8_t minor;   // dev version number
} sl_efuse_version_t;

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************/ /**
* @brief This API is used to get the release, sqa and dev version of EFUSE
* @param none
* @return \ref sl_efuse_version_t type structure
******************************************************************************/
sl_efuse_version_t sl_si91x_efuse_get_version(void);

/***************************************************************************/ /**
* @brief This API is used to enable peripheral clock for efuse.
* @param none
* @return returns status 0 if successful,
*                 else error code.
*        - \ref SL_STATUS_INVALID_PARAMETER - The parameter is an invalid argument \n
*        - \ref SL_STATUS_OK - Success \n
******************************************************************************/
sl_status_t sl_si91x_efuse_enable_clock(void);

/***************************************************************************/ /**
* @brief This API is used to disable peripheral clock for efuse.
* @pre   \ref sl_si91x_efuse_init() 
*
* @pre   \ref sl_si91x_efuse_set_address() 
*
* @pre   \ref sl_si91x_efuse_write_bit() 
*
* @pre   \ref sl_si91x_efuse_memory_mapped_read_byte() 
*
* @param none
* @return returns status 0 if successful,
*                 else error code.
*        - \ref SL_STATUS_INVALID_PARAMETER - The parameter is an invalid argument \n
*        - \ref SL_STATUS_OK - Success \n
******************************************************************************/
sl_status_t sl_si91x_efuse_disable_clock(void);

/***************************************************************************/ /**
* @brief This API is used to Initialize the eFUSE. This API first enables the efuse clock and then efuse.
*
* @param none
*
* @return returns status 0 if successful,else error code.
*        - \ref SL_STATUS_INVALID_PARAMETER - The parameter is an invalid argument \n
*        - \ref SL_STATUS_OK - Success \n
******************************************************************************/
sl_status_t sl_si91x_efuse_init(void);

/***************************************************************************/ /**
* @brief This API is used to un-initialize the eFUSE. This API first disables the efuse and then efuse clock.
* 
* @pre Pre-conditions:
* @pre   \ref sl_si91x_efuse_init() 
*
* @pre   \ref sl_si91x_efuse_set_address() 
*
* @pre   \ref sl_si91x_efuse_write_bit() 
*
* @pre   \ref sl_si91x_efuse_memory_mapped_read_byte() 
*
* @param none
* @return returns status 0 if successful,else error code.
*        - \ref SL_STATUS_INVALID_PARAMETER - The parameter is an invalid argument \n
*        - \ref SL_STATUS_OK - Success \n
******************************************************************************/
sl_status_t sl_si91x_efuse_deinit(void);

/***************************************************************************/ /**
* @brief This API is used to set the eFUSE address for read and write
*        operations.
*  @pre Pre-condition:
*      - \ref sl_si91x_efuse_init() 
* @param[in] address - Holds the address at which the data has to be written
*                      in the efuse Max is 0x31.
* @return returns status 0 if successful,
*                 else error code.
*      - SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid
*                                                   argument 
*      - SL_STATUS_OK - Success
*
******************************************************************************/
sl_status_t sl_si91x_efuse_set_address(uint16_t address);

/***************************************************************************/ /**
* @brief This API is used to get the eFUSE address for read and write
*        operations.
* @pre Pre-conditions:
*     - \ref sl_si91x_efuse_init() 
*     - \ref sl_si91x_efuse_set_address() 
* @param[out] read_address - Pointer holds the address at which the data has
*                           to be written in the efuse
* @return returns status 0 if successful,
*                 else error code as follow:
*       - SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid
*                                                   argument 
*       - SL_STATUS_OK - Success
*
******************************************************************************/
sl_status_t sl_si91x_efuse_get_address(uint16_t *read_address);

/***************************************************************************/ /**
 * @brief This API is used to write the bit in the position of an 8-bit location
 *        in eFUSE specified address. There is only one programming scheme,
 *        which is single bit programming.
 * @pre Pre-conditions:
 *     - \ref sl_si91x_efuse_init() 
 *     - \ref sl_si91x_efuse_set_address() 
 * @param[in] address - Holds the address at which the data has to be written in
 *                      the efuse Max is 0x31.
 * @param[in] bit_pos - Variable that holds the position of bit on which the
 *                      data will be placed (1 - 7).
 * @param[in] hold_time - eFUSE strobe hold time, depends on clock frequency of eFUSE controller
 * @return returns status 0 if successful,
 *                 else error code as follow.
 *       - SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 *       - SL_STATUS_OK - Success
*
 ******************************************************************************/
sl_status_t sl_si91x_efuse_write_bit(uint16_t address, uint8_t bit_pos, uint32_t hold_time);

/***************************************************************************/ /**
 * @brief This API is used to read the 1 word (16 bits) of data from 32x8 byte eFUSE memory (OTP) in
 *        memory mapped mode.
 * @pre   \ref sl_si91x_efuse_init() 
*
 * @pre   \ref sl_si91x_efuse_set_address() 
*
 * @pre   \ref sl_si91x_efuse_write_bit() 
*
 * @param[in] address - Holds the address from where we are reading the 1 word (max value is 0x31).
 *                      of data (max value is 0x31).
 * @param[out] read_word - Pointer that points to the data which is stored in the
 *                        efuse
 * @param[in] soc_clk  - It is input clock frequency of eFUSE controller in range between (1000000 -180000000).
 * @return returns status 0 if successful,
 *                 else error code as follow.
 *       - SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid
 *                                                   argument 
 *       - SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 *       - SL_STATUS_OK - Success
*
 ******************************************************************************/
sl_status_t sl_si91x_efuse_memory_mapped_read_word(uint16_t address, uint16_t *read_word, uint32_t soc_clk);

/***************************************************************************/ /**
 * @brief This API is used to read a 1 byte of data from 32x8 byte eFUSE memory(OTP) in
 *        memory mapped mode.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_efuse_init() 
 *      - \ref sl_si91x_efuse_set_address() 
 *      - \ref sl_si91x_efuse_write_bit() 
 * @param[in] address - Holds the address from where we are reading the 1 byte
 *                      of data (max value is 0x31).
 * @param[out] read_byte - Pointer that points to the 8 bit data which is stored
 *                        in the efuse
 * @param[in] soc_clk  - It is input clock frequency in range between (1000000 -180000000).
 * @return returns status 0 if successful,
 *                 else error code as follow.
 *       - SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid
 *                                                   argument 
 *       - SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 *       - SL_STATUS_OK - Success
 *
 ******************************************************************************/
sl_status_t sl_si91x_efuse_memory_mapped_read_byte(uint16_t address, uint8_t *read_byte, uint32_t soc_clk);

/***************************************************************************/ /**
 * @brief This API is used to read a 1 byte of data from 32x8 byte eFUSE memory (OTP) in
 *        fsm mode.
 * @pre Pre-conditions:
 *     - \ref sl_si91x_efuse_init() 
 *     - \ref sl_si91x_efuse_set_address() 
 *     - \ref sl_si91x_efuse_write_bit() 
 * @param[in] address - Holds the address from where we are reading the 1 byte
 *                      of data (max value is 0x31).
 * @param[out] read_byte - Pointer that points to the 8 bit data which is stored
 *                        in the efuse
 * @param[in] soc_clk  - It is input clock frequency in range between (1000000 -180000000).
 * @return returns status 0 if successful,
 *                 else error code as follow.
 *  - SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid
 *                                                   argument 
 *  - SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 *  - SL_STATUS_OK - Success
 *
 ******************************************************************************/
sl_status_t sl_si91x_efuse_fsm_read_byte(uint16_t address, uint8_t *read_byte, uint32_t soc_clk);

/***************************************************************************/ /**
* @brief This API is used to enable the EFUSE.
* @pre   \ref sl_si91x_efuse_enable_clock() 
*
* @param none
*
* @return    none
******************************************************************************/
__STATIC_INLINE void sl_si91x_efuse_enable(void)
{
  RSI_EFUSE_Enable(EFUSE);
}

/***************************************************************************/ /**
* @brief This API is used to Disable the EFUSE.
* @pre   \ref sl_si91x_efuse_init() 
*
* @pre   \ref sl_si91x_efuse_set_address() 
*
* @pre   \ref sl_si91x_efuse_write_bit() 
*
* @pre   \ref sl_si91x_efuse_memory_mapped_read_byte() 
*
* @param none
* @return    none
******************************************************************************/
__STATIC_INLINE void sl_si91x_efuse_disable(void)
{
  RSI_EFUSE_Disable(EFUSE);
}

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY !***********************
/// @addtogroup EFUSE
/// @{
///
///   @details
///
///   @n @section EFUSE_Intro Introduction
///
///  Electrically programmable fuses or eFUSEs contains one-time programmable memory
///  that are used to store configuration or data in integrated circuits (ICs).
///  E-fuse use 32-bit addressing with each address containing 8 bits.
///  The eFuse controller is used to program and read these bits.
///
///  @li Once information is programmed into an eFUSE, it is typically irreversible.
///  Due to their irreversible nature, eFUSEs are suitable for storing critical data that cannot be altered.
///
///  @li eFUSEs find application in various areas, including
///
///   1. Supports eFuse programming and read operations
///
///   2. Supports memory mapped and FSM based read operation
///
///   @n @section EFUSE_Config Configuration
///
///   The following apis allows users to read and write into the E-fuse 256-bit address.
///
///   1. @ref  sl_si91x_efuse_fsm_read_byte or @ref sl_si91x_efuse_memory_mapped_read_byte or @ref sl_si91x_efuse_memory_mapped_read_word
///
///   2. @ref  sl_si91x_efuse_write_bit
///
///   @n @section EFUSE_Use Usage
///
///   The Efuse application provides an example of how to use various write and read
///   operations to write to and read from OTP (one-time programmable) memory.
///   The WRITE_ENABLE macro must first be enabled before an application may write to memory.
///   By default, only read operations are carried out at the designated memory address. The efuse
///   can be initialized and configured using the following apis in default mode. And it also has
///   other apis where one can use them as per requirements which are discussed in functions.
///
///   1. @ref sl_si91x_efuse_init
///
///   2. @ref sl_si91x_efuse_set_address
///
///   3. @ref sl_si91x_efuse_get_address
///
///   4. @ref sl_si91x_efuse_write_bit
///
///   5. @ref sl_si91x_efuse_fsm_read_byte can be used to read byte when the mode of read is FSM.
///
///   6. @ref sl_si91x_efuse_memory_mapped_read_byte can be used to read byte when the mode of read is Memory mapped mode.
///
///   7. @ref sl_si91x_efuse_memory_mapped_read_word can be used to read word when the mode of read is Memory mapped mode.
///
///   8. @ref sl_si91x_efuse_deinit
///
/** @} (end addtogroup EFUSE) */

#ifdef __cplusplus
}
#endif

#endif // SL_SI91X_EFUSE_H
/**************************************************************************************************/
