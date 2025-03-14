/*******************************************************************************
* @file  si91x_sntp_client_callback_framework.h
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
#pragma once

#include "sl_si91x_types.h"

sl_status_t sli_si91x_sntp_event_handler(sli_si91x_queue_packet_t *data);