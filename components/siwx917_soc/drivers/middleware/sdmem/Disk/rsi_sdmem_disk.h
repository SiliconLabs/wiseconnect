/*******************************************************************************
* @file  rsi_sdmem_disk.h
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

// Includes

#ifndef __RSI_SDMEM_DISK_H__
#define __RSI_SDMEM_DISK_H__

#include <stdint.h>
#include "diskio.h"

#if defined(__cplusplus)
extern "C" {
#endif

DSTATUS sdmem_disk_initialize(uint8_t Drive);

DSTATUS sdmem_disk_status(uint8_t Drive);

DRESULT sdmem_disk_read(uint8_t Drive, uint8_t *buffer, uint32_t sector, uint8_t count);

DRESULT sdmem_disk_write(uint8_t Drive, const uint8_t *buffer, uint32_t sector, uint8_t count);

DRESULT sdmem_disk_ioctl(uint8_t Drive, uint8_t command, void *buffer);

/* @} */
#if defined(__cplusplus)
}
#endif

#endif /* __RSI_SDMEM_DISK_H__ */
