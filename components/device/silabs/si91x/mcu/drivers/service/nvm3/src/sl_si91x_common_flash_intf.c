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
#include "sl_si91x_common_flash_intf.h"
#include "si91x_device.h"

#include "system_si91x.h"
#include "sl_si91x_driver.h"
#include "sl_core.h"
#include "cmsis_os2.h"

/*******************************************************************************
 ***************************  DEFINES / MACROS   ********************************
 ******************************************************************************/

#define SECTOR_SIZE 4096 //Sector size of Si9117 common flash
#define FLASH_ERASE 1    //flash_sector_erase_enable value for erase operation
#define FLASH_WRITE 0    //flash_sector_erase_enable value for write operation

/******************************************************
 *               Variable Definitions
 ******************************************************/
static osSemaphoreId_t nvm3_Sem;

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
  //Erase sector
  status = (int)sl_si91x_command_to_write_common_flash((uint32_t)sector_address, dummy_buff, SECTOR_SIZE, FLASH_ERASE);
  return status;
}

/***************************************************************************/ /**
 * This function writes to destination flash address location
 ******************************************************************************/
bool rsi_flash_write(uint32_t *address, unsigned char *data, uint32_t length)
{

  int status = 0;
  //Write to flash
  status = (int)sl_si91x_command_to_write_common_flash((uint32_t)address, data, (uint16_t)length, FLASH_WRITE);
  return status;
}

/***************************************************************************/ /**
 * Reads data from the address in selected mode
 * Note: Do not use this function independently. This function should only be used
 * by NVM3
 ******************************************************************************/
bool rsi_flash_read(uint32_t *address, unsigned char *data, uint32_t length, uint8_t auto_mode)
{
  (void)auto_mode;
  //Read data from flash
  memcpy((uint8_t *)data, (uint8_t *)address, length * 4);
  return 1;
}

#ifndef NVM3_LOCK_OVERRIDE
/***************************************************************************/ /**
 * @details
 * The implementation of lock-begin based on free RTOs.
 * @note
 * The default NVM3 protection functions can be substituted by the application
 * if other syncronization functions are available and disabling interrupts for
 * extended periods is not desired.
 ******************************************************************************/
void nvm3_lockBegin(void)
{

  if (nvm3_Sem == NULL) {
    nvm3_Sem = osSemaphoreNew(1, 0, NULL);
    osSemaphoreRelease(nvm3_Sem);
  }
  osSemaphoreAcquire(nvm3_Sem, osWaitForever);
}
/***************************************************************************/ /**
 * @details
 * The implementation of lock-end based on free RTOs.
 ******************************************************************************/
void nvm3_lockEnd(void)
{
  osSemaphoreRelease(nvm3_Sem);
}
#endif // NVM3_LOCK_OVERRIDE
