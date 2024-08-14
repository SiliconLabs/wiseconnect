/***************************************************************************/ /**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#pragma once

#include "sl_status.h"

/** \addtogroup SI91X_FIRMWARE_UPDATE_FROM_MODULE_FUNCTIONS Firmware Update From Module
 * \ingroup SI91X_FIRMWARE_UPDATE_FUNCTIONS
 * @{ */

/***************************************************************************/
/**
 * @brief      
 *   Sends an HTTP request to a specified server URL with optional data and headers,
 *   and facilitates firmware downloads from the server. This API is non-blocking API.
 * @param[in] type
 *   Valid values are: 2 - HTTP_OTAF
 * @param[in] flags 
 *   Configuration flags. See the table below for details.
 * @param[in] ip_address 
 *   Server IP address.
 * @param[in] port 
 *   Port number. Default: 80 - HTTP, 443 - HTTPS
 * @param[in] resource 
 *   Requested resource URL in string format.
 * @param[in] host_name       
 *   Host name.
 * @param[in] extended_header 
 *   Extended header if present, each header member should end with \r\n.
 * @param[in] username        
 *   Username for server authentication.
 * @param[in] password         
 *   Password for server authentication.
 * @param[in] post_data        
 *   HTTP data to be posted to the server.
 * @param[in] post_data_length 
 *   HTTP data length to be posted to the server.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * 
 * @details
 *   The following table lists the flags that can be used with this function:
 * 
 *   | Flags  |     Macro                           | Description                                                            |
 *   |:-------|:------------------------------------|:-----------------------------------------------------------------------|
 *   | BIT(0) | HTTPS_SUPPORT                       | Set this bit to enable HTTPS_SUPPORT to use HTTPS feature.             |
 *   | BIT(1) | IP_VERSION_6                        | Set this bit to enable IPv6. By default, it is configured to IPv4.      |
 *   | BIT(2) | SI91X_TLS_V_1_0                     | Set this bit to support SSL TLS Version 1.0 if HTTPS is enabled.       |
 *   | BIT(3) | SI91X_TLS_V_1_2                     | Set this bit to support SSL TLS Version 1.2 if HTTPS is enabled.       |
 *   | BIT(4) | SI91X_TLS_V_1_1                     | Set this bit to support SSL_TLS Version 1.1 if HTTPS is enabled.       |
 *   | BIT(6) | HTTP_V_1_1                          | Set this bit to use HTTP version 1.1                                   |
 *   | BIT(9) | SL_SI91X_HTTPS_CERTIFICATE_INDEX_1  | Set this bit to specify index of SSL cert to be used for HTTPS.        |
 *   | BIT(10) | SL_SI91X_HTTPS_CERTIFICATE_INDEX_2 | Set this bit to specify index of SSL cert to be used for HTTPS.     
 ******************************************************************************/
sl_status_t sl_si91x_http_otaf(uint8_t type,
                               uint16_t flags,
                               uint8_t *ip_address,
                               uint16_t port,
                               uint8_t *resource,
                               uint8_t *host_name,
                               uint8_t *extended_header,
                               uint8_t *user_name,
                               uint8_t *password,
                               const uint8_t *post_data,
                               uint32_t post_data_length);

/** @} */
