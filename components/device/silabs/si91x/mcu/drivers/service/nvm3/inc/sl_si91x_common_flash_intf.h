/*******************************************************************************
 * @file  sl_si91x_common_flash_intf.h
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
#ifndef SL_SI91x_COMMON_FLASH_INTF_H_
#define SL_SI91x_COMMON_FLASH_INTF_H_

#include "base_types.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************/ /**
 *  @fn          bool rsi_flash_init(void)
 *  @pre         none
 *  @brief       This function initializes GPIO, QSPI and flash
 *  @param       none
 *  @return      return is zero then successfully initialized if non-zero then fail.
 ******************************************************************************/
bool rsi_flash_init(void);

/***************************************************************************/ /**
 *  @fn          bool rsi_flash_erase_sector(uint32_t sector_address)
 *  @pre         None
 *  @brief       This function erases the flash.Erase each sector from the given 
 *               sector address to the configured page size.
 *               it will wait for to complete sector erase
 *  @param[in]   sector_address     start address of the flash sector
 *  @return      return is zero then successfully erased if non-zero then fail.
 ******************************************************************************/
bool rsi_flash_erase_sector(uint32_t *sector_address);

/***************************************************************************/ /**
 *  @fn          bool rsi_flash_write(uint32_t address, unsigned char *data, uint32_t length)
 *  @pre         None
 *  @brief       This function writes to destination flash address location
 *  @param[in]   address     destination address of NVM3 write
 *  @param[in]   data        A pointer to the object data to write
 *  @param[in]   length      The size of the object data in number of bytes
 *  @return      return is zero then successfully data write if non-zero then fail.
 ******************************************************************************/
bool rsi_flash_write(uint32_t *address, unsigned char *data, uint32_t length);

/***************************************************************************/ /**
 *  @fn          bool rsi_flash_read(uint32_t address, unsigned char *data, uint32_t length,uint8_t auto_mode);
 *  @pre         None
 *  @brief       Reads data from the address in selected mode
 *  @param[in]   address     source address to read the data
 *  @param[in]   length      The size of the object data in number of bytes
 *  @param[in]   auto_mode   mode of read operation from flash
 *  @param[out]  data        destination address of NVM3 read to write data
 *  @return      return is zero then successfully data read if non-zero then fail.
 ******************************************************************************/
bool rsi_flash_read(uint32_t *address, unsigned char *data, uint32_t length, uint8_t auto_mode);

/***************************************************************************/ /**
 *  @fn          bool rsi_flash_uninitialize(void)
 *  @pre         None
 *  @brief       Uninitialize QSPI module
 *  @return      return is zero then successfully Uninitialized if non-zero then fail.
 ******************************************************************************/
bool rsi_flash_uninitialize(void);

#ifdef __cplusplus
}
#endif

#endif /* SL_SI91x_COMMON_FLASH_INTF_H_*/