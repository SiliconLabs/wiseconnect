/***************************************************************************/ /**
 * @file app_config.h
 * @brief User configuration for the WLAN BLE power save example
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

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

//! Load certificate to device flash for SSL client
//! Certificate could be loaded once and need not be loaded for every boot up
#define SSL_CLIENT 1

//! NWP credential slot for TLS CA (use with SL_NET_TLS_SERVER_CREDENTIAL_ID)
#define CERTIFICATE_INDEX 0

#endif // APP_CONFIG_H
