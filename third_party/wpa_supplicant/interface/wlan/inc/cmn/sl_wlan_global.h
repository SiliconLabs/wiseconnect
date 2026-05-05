/*******************************************************************************
* @file  sl_wlan_global.h
* @brief This file contains wlan related global macros
*******************************************************************************
* # License
* <b>Copyright 2020-2024 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef SL_WLAN_GLOBAL_H
#define SL_WLAN_GLOBAL_H

/* True and False Definitions */
#define SL_FALSE (0)
#define SL_TRUE  (1)

/* WLAN Status */
#define WLAN_STATUS_SUCCESS  0
#define WLAN_STATUS_FAILURE  1
#define WLAN_STATUS_RSI_FAIL -2 /* Definition corresponding to RSI_FAIL */

#define SME_FT_MALLOC_MEMORY 400 // Current requirement doesnot exceeded it.

#define IP_ADDRESS_SZ 4
#endif /* SL_WLAN_GLOBAL_H */
