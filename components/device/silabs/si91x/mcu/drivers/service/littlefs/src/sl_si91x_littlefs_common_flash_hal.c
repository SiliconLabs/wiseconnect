/***************************************************************************/ /**
 * @file sl_si91x_littlefs_common_flash_hal.c
 * @brief Little fs Common Flash HAL API's implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_si91x_driver.h"
#include "sl_si91x_littlefs_hal.h"
#include "cmsis_os2.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/

#define SECTOR_SIZE     4096 //Sector size of Si9117 common flash
#define FLASH_ERASE     1    //flash_sector_erase_enable value for erase operation
#define FLASH_WRITE     0    //flash_sector_erase_enable value for write operation
#define LITTLE_FS_OK    0    // LittleFS success code
#define LITTLE_FS_ERROR 1    // LittleFS Error code

extern char linker_littlefs_begin;
static osSemaphoreId_t littlefs_sem;

__attribute__((used)) uint8_t littlefs_default_storage[LITTLEFS_DEFAULT_MEM_SIZE] __attribute__((section(".ltfs")));
#define LITTLEFS_BASE (&linker_littlefs_begin)

/******************************************************************************
 * Initialize the qspi for littlefs
 ******************************************************************************/
void sl_si91x_littlefs_qspi_init(void)
{
  // QSPI initialization is not required because it is already intialized while
  // having sl_net init call.
}

/******************************************************************************
 * Read the data from flash
 ******************************************************************************/
int si91x_block_device_read(const struct lfs_config *cfg,
                            lfs_block_t block,
                            lfs_off_t off,
                            void *buffer,
                            lfs_size_t size)
{
  uint32_t flash_read_addr = 0;
  int status               = LITTLE_FS_ERROR;
  assert(block < cfg->block_count);

  //Calculate the flash read address based on block number and offset
  flash_read_addr = (uint32_t)LITTLEFS_BASE + (block * cfg->block_size) + off;
  if (flash_read_addr != 0) {
    memcpy((uint8_t *)buffer, (uint8_t *)flash_read_addr, size);
    status = LITTLE_FS_OK;
  }
  return status;
}

/******************************************************************************
 * Write the data to the flash
 ******************************************************************************/
int si91x_block_device_prog(const struct lfs_config *cfg,
                            lfs_block_t block,
                            lfs_off_t off,
                            const void *buffer,
                            lfs_size_t size)
{
  uint32_t flash_prog_addr = 0;
  int status               = LITTLE_FS_ERROR;
  assert(block < cfg->block_count);
  //Calculate the flash write address based on block number and offset
  flash_prog_addr = (uint32_t)LITTLEFS_BASE + (block * cfg->block_size) + off;

  if (flash_prog_addr != 0) {
    //Write to flash
    status = sl_si91x_command_to_write_common_flash(flash_prog_addr, buffer, (uint16_t)size, FLASH_WRITE);
    if (status != LITTLE_FS_OK) {
      status = LFS_ERR_IO;
    }
  }
  return status;
}

/******************************************************************************
 * Erase the data from flash
 ******************************************************************************/
int si91x_block_device_erase(const struct lfs_config *cfg, lfs_block_t block)
{
  uint32_t flash_erase_addr = 0;
  int status                = LITTLE_FS_ERROR;
  uint8_t dummy_buff[10]    = { 0 };

  assert((block < cfg->block_count));

  //Calculate the flash erase address/block start address based on block number and offset
  flash_erase_addr = (uint32_t)LITTLEFS_BASE + (block * cfg->block_size);

  if (flash_erase_addr != 0) {
    //Erase sector
    status = sl_si91x_command_to_write_common_flash(flash_erase_addr, dummy_buff, SECTOR_SIZE, FLASH_ERASE);
    if (status != LITTLE_FS_OK) {
      status = LFS_ERR_IO;
    }
  }
  return status;
}

/*****************************************************************************
 *  Sync API
 ******************************************************************************/
int si91x_block_device_sync(const struct lfs_config *c)
{
  UNUSED_PARAMETER(c);
  return LITTLE_FS_OK;
}

/*****************************************************************************
 *  Initialize and begin the lock, acquires the semaphore.
 ******************************************************************************/
int si91x_block_device_lock(const struct lfs_config *c)
{
  UNUSED_PARAMETER(c);
  if (littlefs_sem == NULL) {
    littlefs_sem = osSemaphoreNew(1, 0, NULL);
    osSemaphoreRelease(littlefs_sem);
  }
  osSemaphoreAcquire(littlefs_sem, osWaitForever);
  return LITTLE_FS_OK;
}
/*****************************************************************************
 *  Ends the lock, release the semaphore.
 ******************************************************************************/
int si91x_block_device_unlock(const struct lfs_config *c)
{
  UNUSED_PARAMETER(c);
  int status = LITTLE_FS_ERROR;
  if (littlefs_sem != NULL) {
    osSemaphoreRelease(littlefs_sem);
    status = LITTLE_FS_OK;
  }
  return status;
}
