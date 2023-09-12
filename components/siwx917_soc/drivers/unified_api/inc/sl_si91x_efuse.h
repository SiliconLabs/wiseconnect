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
* @addtogroup EFUSE
* @{
* @brief EFUSE peripheral
*
* ##Overview
*
* The chipset provides 256 eFuse bits as a one-time programmable memory
* location. These bits use 32-bit addressing with each address containing 8
* bits. The eFuse controller is used to program and read these bits. The 255th
* eFuse bit is programmed to 1'b1 and tested as part of manufacturing tests.
* Hence this bit has to be marked as Reserved with a default value to '1'
*
* @details
* ##Features
* - Supports eFuse programming and read operations
* - Supports memory mapped and FSM based read operation
*
* ##Initialization
* - Enable The EFUSE Clock  using @ref sl_si91x_efuse_enable_clock.
* - Enable the EFUSE using \ref sl_si91x_efuse_enable.
* - Set the eFUSE address for read and write operations using
*   \ref sl_si91x_efuse_set_address.
*******************************************************************************/

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
* To get the release, sqa and dev version of GSPI
* @param[in] none
* @return (sl_gspi_version_t) type structure
*
******************************************************************************/
sl_efuse_version_t sl_si91x_efuse_get_version(void);

/***************************************************************************/ /**
* @brief This API Is Used to enable efuse clock.
* @param[in] none
* @return returns status 0 if successful,
*                 else error code.
******************************************************************************/
sl_status_t sl_si91x_efuse_enable_clock(void);

/***************************************************************************/ /**
* @brief This API Is Used to disable efuse clock.
* @pre   \ref sl_si91x_efuse_init() \n
*        \ref sl_si91x_efuse_set_address() \n
*        \ref sl_si91x_efuse_write_bit() \n
*        \ref sl_si91x_efuse_memory_mapped_read_byte() \n
* @param[in] none
* @return returns status 0 if successful,
*                 else error code.
******************************************************************************/
sl_status_t sl_si91x_efuse_disable_clock(void);

/***************************************************************************/ /**
* @brief This API is used to Initialize the EFUSE. This API first enables the
*        efuse clock and then efuse.
* @param[in] none
* @return returns status 0 if successful,
*                 else error code.
******************************************************************************/
sl_status_t sl_si91x_efuse_init(void);

/***************************************************************************/ /**
* @brief This API is used to Un-initialize the EFUSE. This API first disables
*        the efuse and then efuse clock.
* @pre   \ref sl_si91x_efuse_init() \n
*        \ref sl_si91x_efuse_set_address() \n
*        \ref sl_si91x_efuse_write_bit() \n
*        \ref sl_si91x_efuse_memory_mapped_read_byte() \n
* @param[in] none
* @return returns status 0 if successful,
*                 else error code.
******************************************************************************/
sl_status_t sl_si91x_efuse_deinit(void);

/***************************************************************************/ /**
* @brief This API is used to set the eFUSE address for read and write
*        operations.
* @pre   \ref sl_si91x_efuse_init() \n
* @param[in] address - Holds the address at which the data has to be written
*                      in the efuse
* @return returns status 0 if successful,
*                 else error code.
*       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid
*                                                   argument \n
******************************************************************************/
sl_status_t sl_si91x_efuse_set_address(uint16_t address);

/***************************************************************************/ /**
* @brief This API is used to get the eFUSE address for read and write
*        operations.
* @pre   \ref sl_si91x_efuse_init() \n
*        \ref sl_si91x_efuse_set_address() \n
* @param[in] read_address - Pointer holds the address at which the data has
*                           to be written in the efuse
* @return returns status 0 if successful,
*                 else error code.
*       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid
*                                                   argument \n
******************************************************************************/
sl_status_t sl_si91x_efuse_get_address(uint16_t *read_address);

/***************************************************************************/ /**
 * @brief This API is used to write the eFUSE data in the specified address.
 * @pre   \ref sl_si91x_efuse_init() \n
 *        \ref sl_si91x_efuse_set_address() \n
 * @param[in] address - Holds the address at which the data has to be written in
 *                      the efuse
 * @param[in] bit_pos - Variable that holds the position of bit on which the
 *                      data will be placed hold_time - hold time in write bit
 * @return returns status 0 if successful,
 *                 else error code.
 *       \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_efuse_write_bit(uint16_t address, uint8_t bit_pos, uint32_t hold_time);

/***************************************************************************/ /**
 * @brief This API is used to Read the 1 word(16 bits) of data to EFUSE macro in
 *        memory mapped mode.
 * @pre   \ref sl_si91x_efuse_init() \n
 *        \ref sl_si91x_efuse_set_address() \n
 *        \ref sl_si91x_efuse_write_bit() \n
 * @param[in] address - Holds the address from where we are reading the 1 word
 *                      of data
 * @param[in] read_word - Pointer that points to the data which is stored in the
 *                        efuse
 * @param[in] soc_clk  - It is input clock frequency
 * @return returns status 0 if successful,
 *                 else error code.
 *       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid
 *                                                   argument \n
 *       \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_efuse_memory_mapped_read_word(uint16_t address, uint16_t *read_word, uint32_t soc_clk);

/***************************************************************************/ /**
 * @brief This API is used to read the data from 32x8 byte eFUSE memory(OTP) in
 *        memory mapped mode.
 * @pre   \ref sl_si91x_efuse_init() \n
 *        \ref sl_si91x_efuse_set_address() \n
 *        \ref sl_si91x_efuse_write_bit() \n
 * @param[in] address - Holds the address from where we are reading the 1 byte
 *                      of data
 * @param[in] read_byte - Pointer that points to the 8 bit data which is stored
 *                        in the efuse
 * @param[in] soc_clk  - It is input clock frequency
 * @return returns status 0 if successful,
 *                 else error code.
 *       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid
 *                                                   argument \n
 *       \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_efuse_memory_mapped_read_byte(uint16_t address, uint8_t *read_byte, uint32_t soc_clk);

/***************************************************************************/ /**
 * @brief This API is used to read the data from 32x8 byte eFUSE memory(OTP) in
 *        fsm mode.
 * @pre   \ref sl_si91x_efuse_init() \n
 *        \ref sl_si91x_efuse_set_address() \n
 *        \ref sl_si91x_efuse_write_bit() \n
 * @param[in] address - Holds the address from where we are reading the 1 byte
 *                      of data
 * @param[in] read_byte - Pointer that points to the 8 bit data which is stored
 *                        in the efuse
 * @param[in] soc_clk  - It is input clock frequency
 * @return returns status 0 if successful,
 *                 else error code.
 *       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid
 *                                                   argument \n
 *       \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 ******************************************************************************/
sl_status_t sl_si91x_efuse_fsm_read_byte(uint16_t address, uint8_t *read_byte, uint32_t soc_clk);

/***************************************************************************/ /**
* @brief This API is used to enable the EFUSE.
* @pre   \ref sl_si91x_efuse_enable_clock() \n
* @param[in] none
* @return    none
******************************************************************************/
__STATIC_INLINE void sl_si91x_efuse_enable(void)
{
  RSI_EFUSE_Enable(EFUSE);
}

/***************************************************************************/ /**
* @brief This API is used to Disable the EFUSE.
* @pre   \ref sl_si91x_efuse_init() \n
*        \ref sl_si91x_efuse_set_address() \n
*        \ref sl_si91x_efuse_write_bit() \n
*        \ref sl_si91x_efuse_memory_mapped_read_byte() \n
* @param[in] none
* @return    none
******************************************************************************/
__STATIC_INLINE void sl_si91x_efuse_disable(void)
{
  RSI_EFUSE_Disable(EFUSE);
}

/** @} (end addtogroup EFUSE) */

#ifdef __cplusplus
}
#endif

#endif // SL_SI91X_EFUSE_H
/**************************************************************************************************/
