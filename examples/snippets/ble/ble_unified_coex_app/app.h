/*******************************************************************************
* @file  app.h
* @brief BLE Unified Coex Application - Public API
*******************************************************************************
* # License
* <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef APP_H
#define APP_H

/******************************************************
 * Application function declarations
 ******************************************************/

// Application initialization
void app_init(const void *unused);

// Application process action
void app_process_action(void);

#endif // APP_H
