/*******************************************************************************
 * @file  sl_si91x_dual_flash_intf.h
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
#ifndef SL_SI91x_DUAL_FLASH_INTF_H_
#define SL_SI91x_DUAL_FLASH_INTF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rsi_qspi.h"
#include "rsi_rom_egpio.h"

#define ECODE_QSPI_OK    (0)
#define ECODE_QSPI_ERROR (1)

/* M4 clock control configurations register */
#define M4SS_CLK_PWR_CTRL_BASE_ADDR 0x46000000

/* M4 clock enable configurations register */
#define M4SS_CLK_ENABLE_SET_REG1 *(volatile uint32_t *)(M4SS_CLK_PWR_CTRL_BASE_ADDR + 0x00)
/* M4 clock clear configurations register */
#define M4SS_CLK_ENABLE_CLEAR_REG1 *(volatile uint32_t *)(M4SS_CLK_PWR_CTRL_BASE_ADDR + 0x04)
#define M4SS_STATIC_CLK_SWITCH     BIT(19)

/* Enable QSPI  clock and hclock */
#define CLK_ENABLE_SET_2_REG_QSPI (*((volatile uint32_t *)(M4SS_CLK_PWR_CTRL_BASE_ADDR + 0x08)))
/*Disable the flash based retention sleep */
/*This macro needs to be enabled when flash based retention sleep is needed*/
#define FLASH_BASED_RET_SLEEP 1
/* Stack pointer on retention wake up */
#define INTERMEDIATE_SP 0x1F800
/* GPIO port number */
#define QSPI_PINS_PORT 0
/* TA core reset address */
#define TA_RESET_ADDR 0x22000004
/* Default flash page size */
#ifndef FLASH_PAGE_SIZE
#define FLASH_PAGE_SIZE 256U
#endif
/* Macros to set or clear bit positions */
#define VAL_12 12
#define VAL_11 11
#define VAL_15 15
#define VAL_6  6

/******************************************
 *              FLASH CMDS
 ******************************************/
//! Write enable cmd
#define WREN  0x06
#define WREN2 0xF9
//! Write disable cmd
#define WRDI  0x04
#define WRDI2 0xFB
//! Read status reg cmd
#define RDSR  0x05
#define RDSR2 0xFA
//! chip erase cmd
#define CHIP_ERASE 0xC7
//! block erase cmd
#define BLOCK_ERASE 0xD8
//! sector erase cmd
#define SECTOR_ERASE 0x20
//! high speed rd cmd
#define HISPEED_READ 0x0B
//! rd cmd
#define READ 0x03
//! write config2
#define WCFG2 0x72

#define _1BYTE 0
#define _2BYTE 1
#define _4BYTE 3

/* GPIO register bit position related defines */
#define GPIO_0_TO_5   0
#define GPIO_6_TO_11  1
#define GPIO_46_TO_51 2
#define GPIO_52_TO_57 3
#define GPIO_58_TO_63 4
//! TA OCTA/DUAL combinations
#define GPIO_0_AND_58  5
#define GPIO_6_AND_58  6
#define GPIO_46_AND_58 7
//! M4 OCTA/DUAL combinations
#define GPIO_0_AND_52  8
#define GPIO_6_AND_52  9
#define GPIO_46_AND_52 10
#define NOT_USING      11 //! Not applicable
#define GPIO_58_AND_52 12
#define GPIO_DDR_PADS  13

/***************************************************************************/ /**
 *  @fn          bool rsi_flash_init(void)
 *  @pre         None
 *  @brief       This function initializes GPIO, QSPI and flash
 *  @param       none
 *  @return      return is zero then successfully initialized if non-zero then fail.
 ******************************************************************************/
bool rsi_flash_init(void);

/***************************************************************************/ /**
 *  @fn          bool rsi_flash_erase_sector(uint32_t sector_address)
 *  @pre         This function should be called after rsi_flash_init()
 *  @brief       This function erases the flash.Erase each sector from the given 
 *               sector address to the configured page size.
 *               it will wait for to complete sector erase
 *  @param[in]   sector_address     start address of the flash sector
 *  @return      return is zero then successfully erased if non-zero then fail.
 ******************************************************************************/
bool rsi_flash_erase_sector(uint32_t *sector_address);

/***************************************************************************/ /**
 *  @fn          bool rsi_flash_write(uint32_t address, unsigned char *data, uint32_t length)
 *  @pre         This function should be called after rsi_flash_init()
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
 *  @pre         This function should be called after rsi_flash_init()
 *  @brief       Uninitialize QSPI module
 *  @return      return is zero then successfully Uninitialized if non-zero then fail.
 ******************************************************************************/
bool rsi_flash_uninitialize(void);

/***************************************************************************/ /**
 *  @fn          void get_qspi_config(spi_config_t *spi_config)
 *  @brief       Configurations for QSPI module
 *  @param[in]   spi_config     A pointer to the qspi Configurations
 ******************************************************************************/
void get_qspi_config(spi_config_t *spi_config);

#ifdef __cplusplus
}
#endif

#endif /* SL_SI91x_DUAL_FLASH_INTF_H_*/