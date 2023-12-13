/*******************************************************************************
* @file  rsi_user.h
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

#ifndef RSI_USER_H
#define RSI_USER_H
/******************************************************
 * *                      Macros
 * ******************************************************/

// Silicon Version 1.3
#define CHIP_VER_1P3 0

// Silicon Version 1.4 and above
#define CHIP_VER_1P4_AND_ABOVE 1

// To selcet Silicon version, select either of one from among two macros
#define RS9116_SILICON_CHIP_VER \
  CHIP_VER_1P4_AND_ABOVE // Set 1 for Silicon Chip Version 1p4 and Above, 0 for Silicon Chip Version 1p3

#define TX_WAIT_TIME 0

#define BT_WAIT_TIMEOUT_SF        1
#define BLE_WAIT_TIMEOUT_SF       1
#define BT_COMMON_WAIT_TIMEOUT_SF 1

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/
/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
#endif
