/***************************************************************************/ /**
* @file app.c
* @brief Top level application functions
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
#include "ps2_wireless_event_monitor.h"
#include "app.h"

/*******************************************************************************
 * Initialize application.
 ******************************************************************************/
void app_init(void)
{
  ps2_wireless_event_monitor_init();
}

/*******************************************************************************
 * App ticking function.
 ******************************************************************************/
void app_process_action(void)
{
  ps2_wireless_event_monitor_process_action();
}
