/*******************************************************************************
 * @file  Flash_Intf.c
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
#include "sl_si91x_flash_intf.h"
#include "si91x_device.h"
#include "rsi_board.h"
#include "rsi_chip.h"
#include "system_si91x.h"
#include "sl_si91x_driver.h"
#include "em_core_generic.h"

/*******************************************************************************
 ***************************  DEFINES / MACROS   ********************************
 ******************************************************************************/

#define SECTOR_SIZE 4096 //Sector size of Si9117 common flash
#define FLASH_ERASE 1    //flash_sector_erase_enable value for erase operation
#define FLASH_WRITE 0    //flash_sector_erase_enable value for write operation
#ifdef CHIP_917B0
#define TA_M4_ADDRESS_OFFSET 0x4000000 //Difference between TA and M4 flash access address
#else
#define TA_M4_ADDRESS_OFFSET 0x4032000 //Difference between TA and M4 flash access address
#endif

/*******************************************************************************
 ***************************  Local VARIABLES  ********************************
 ******************************************************************************/
extern CORE_irqState_t irqState;

/*******************************************************************************
* @brief: Flash access APIs used for NVM3 for Si911x common flash

* @details:
* These APIs are used for flash erase, flash write and flash read operations
* For erase operation supplied address should be 4k aligned. For write operation
* address should be word aligned

* @note:
* These APIs should be only used by NVM3 and should be called after device_init and wireless_init 
* functions
 *******************************************************************************/

/***************************************************************************/ /**
 *  Uninitialize Flash module
 ******************************************************************************/
bool rsi_flash_uninitialize(void)
{
  /* This function not supported so always returns zero */
  return RSI_SUCCESS;
}

/***************************************************************************/ /**
 * This function initializes flash
 ******************************************************************************/
bool rsi_flash_init(void)
{
  /* This function not supported so always returns zero */
  return RSI_SUCCESS;
}

/***************************************************************************/ /**
 * This function erases the flash
 * Note: Do not use this function independently. This function should only be used
 * by NVM3
 ******************************************************************************/
bool rsi_flash_erase_sector(uint32_t *sector_address)
{

  int status             = 0;
  uint8_t dummy_buff[10] = { 0 };
  //NVM3 disables IRQ before calling this function. sl_si91x_command_to_write_common_flash() needs
  //TA-M4 interrupt to be enabled. So, enable interrupts before erase operation
  CORE_EXIT_CRITICAL();
  //Obtain actual TA flash access address
  uint32_t address_ta = (uint32_t)sector_address - TA_M4_ADDRESS_OFFSET;
  //Erase sector
  status = sl_si91x_command_to_write_common_flash(address_ta, dummy_buff, SECTOR_SIZE, FLASH_ERASE);
  //Disable the interrupts back
  CORE_ENTER_CRITICAL();
  return status;
}

/***************************************************************************/ /**
 * This function writes to destination flash address location
 ******************************************************************************/
bool rsi_flash_write(uint32_t *address, unsigned char *data, uint32_t length)
{

  int status = 0;
  //NVM3 disables IRQ before calling this function. sl_si91x_command_to_write_common_flash() needs
  //TA-M4 interrupt to be enabled. So, enable interrupts before write operation
  CORE_EXIT_CRITICAL();
  uint32_t address_ta = (uint32_t)address - TA_M4_ADDRESS_OFFSET;
  //Write to flash
  status = sl_si91x_command_to_write_common_flash(address_ta, data, length, FLASH_WRITE);
  //Disable the interrupts back
  CORE_ENTER_CRITICAL();
  return status;
}

/***************************************************************************/ /**
 * Reads data from the address in selected mode
 * Note: Do not use this function independently. This function should only be used
 * by NVM3
 ******************************************************************************/
bool rsi_flash_read(uint32_t *address, unsigned char *data, uint32_t length, uint8_t auto_mode)
{

  uint32_t address_ta = (uint32_t)address - TA_M4_ADDRESS_OFFSET;
  //Read data from flash
  memcpy((uint8_t *)data, (uint8_t *)address_ta, length * 4);
  return 1;
}
