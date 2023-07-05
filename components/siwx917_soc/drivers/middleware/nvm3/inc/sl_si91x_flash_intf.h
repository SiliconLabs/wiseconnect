/*******************************************************************************
 * @file  sl_si91x_flash_intf.h
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

#ifndef SL_SI91x_FLASH_INTF_H_
#define SL_SI91x_FLASH_INTF_H_

/***************************************************************************/ /**
* @addtogroup NVM3
* @{
* @brief The NVM3 driver provides a means to write and read data objects (key/value pairs) stored in flash

* ##Overview
* refer to https://docs.silabs.com/gecko-platform/3.1/driver/api/group-nvm3

* ## Initialization
* refer to https://docs.silabs.com/gecko-platform/3.1/driver/api/group-nvm3
   ******************************************************************************/
// Includes
#ifdef EXECUTION_FROM_RAM
#include "sl_si91x_common_flash_intf.h"
#else
#include "sl_si91x_dual_flash_intf.h"
#endif

/** @} (end addtogroup NVM3) */
#endif /* SL_SI91x_FLASH_INTF_H_*/
