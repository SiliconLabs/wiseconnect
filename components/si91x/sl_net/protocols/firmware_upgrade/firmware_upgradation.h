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

/**
 * @addtogroup SERVICE_FIRMWARE_UPGRADE_FUNCTIONS Functions
 * @{
 */

/***************************************************************************/ /**
 * @brief      
 *   Post the HTTP data for the requested URL to HTTP server.This is a non-blocking API. 
 * @param[in] type
 *   Valid values are: 0 - HTTPGET, 1 - HTTPPOST
 * @param[in] flags 
 *   Select version and security: \n 
 *   |Flags  |     Macro       |          Description |
 *   |:------|:----------------|:-----------------------------------------------------------------------|
 *   |BIT(0) | IPV6       	   |    Set this bit to enable IPv6, by default it is configured to IPv4. |
 *   |BIT(1) | SSL_ENABLE	   |    Set this bit to enable SSL feature. |
 *   |BIT(2) | SI91X_TLS_V_1_0 |    Set this bit to support SSL TLS Version 1.0 if HTTPS is enabled. |
 *   |BIT(3) | SI91X_TLS_V_1_2 |    Set this bit to support SSL_TLS Version 1.2 if HTTPS is enabled. |
 *   |BIT(4) | SI91X_TLS_V_1_1 |    Set this bit to support SSL_TLS Version 1.1 if HTTPS is enabled. |
 *   |BIT(6) | HTTP_V_1_1      |    Set this bit  to use HTTP version 1.1 |
 * @param[in] ip_address 
 *   Server IP address.
 * @param[in] port 
 *   Port number. Default : 80 - HTTP, 443 - HTTPS
 * @param[in] resource 
 *   Requested resource URL in string format.
 * @param[in] host_name       
 *   Host name.
 * @param[in] extended_header 
 *   Extender header if present, after each header member append \r\c \n
 * @param[in] username        
 *   Username for server authentication.
 * @param[in] password         
 *   Password for server authentication.
 * @param[in] post_data        
 *   HTTP data to be posted to server.
 * @param[in] post_data_length 
 *   HTTP data length to be posted to server.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_http_otaf(uint8_t type,
                               uint8_t flags,
                               uint8_t *ip_address,
                               uint16_t port,
                               uint8_t *resource,
                               uint8_t *host_name,
                               uint8_t *extended_header,
                               uint8_t *user_name,
                               uint8_t *password,
                               uint8_t *post_data,
                               uint32_t post_data_length);

/** @} */
