/*******************************************************************************
* @file  console_variable_database.c
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

#include "console_types.h"
#include "console_constants.h"
#include "nvm_ids.h"

const console_variable_node_t console_variable_table[] = {};

const uint32_t console_variable_table_size = sizeof(console_variable_table) / sizeof(console_variable_node_t);
