/***************************************************************************/ /**
 * @file app.h
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2026 Silicon Laboratories Inc. www.silabs.com</b>
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

/***************************************************************************/ /**
 * app_init: Initialize and start the application.
 * Called once from main(). Starts the secure-storage example by creating the
 * application thread (see app.c). No other work is done here.
 ******************************************************************************/
void app_init(void);

/***************************************************************************/ /**
 * app_process_action: Periodic tick callback.
 * Called from main() loop each iteration. This app does all work in a single
 * thread; nothing is done here (empty implementation).
 ******************************************************************************/
void app_process_action(void);

#endif // APP_H
