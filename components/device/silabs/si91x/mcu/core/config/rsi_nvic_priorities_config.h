/*******************************************************************************
* @file  rsi_nvic_priorities_config.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef NVIC_PRIORITIES_CONFIG_H_
#define NVIC_PRIORITIES_CONFIG_H_

/*
 * NOTE:
 * Keep this file OS-agnostic. Any OS-specific definitions (e.g. FreeRTOS
 * interrupt priority macros) must be provided by the OS porting layer.
 */

#if defined(SLI_SI91X_ENABLE_OS)
#include "sl_si91x_os.h"
#endif

#ifndef SL_SI91X_OS_DEFAULT_IRQ_PRIORITY
#define SL_SI91X_OS_DEFAULT_IRQ_PRIORITY (0U)
#endif

#define DEFAULT_PRIORITY SL_SI91X_OS_DEFAULT_IRQ_PRIORITY

void sl_si91x_device_init_nvic();

#endif /* NVIC_PRIORITIES_CONFIG_H_ */
