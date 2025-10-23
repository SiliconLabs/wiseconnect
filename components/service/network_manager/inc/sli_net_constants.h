/***************************************************************************/ /**
 * @file
 * @brief Networking constants
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

/// Enumerate IP configuration modes
typedef enum {
  SLI_NET_STATIC_IP = 0, ///< Static
  SLI_NET_DHCP,          ///< DHCP
  SLI_NET_DHCP_RESERVED, ///< DHCP Rseserved
  SLI_NET_DHCP_HOSTNAME, ///< DHCP HostName
  SLI_NET_DHCP_OPTION81, ///< DHCP option 81
  SLI_NET_DHCP_OPTION77  ///< DHCP option 77
} sli_net_ip_config_mode_t;
