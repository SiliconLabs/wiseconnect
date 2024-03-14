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
#include "sl_status.h"
#include "sl_net.h"
#include "sl_si91x_driver.h"

/// Enumerations of TLS certificate types
typedef enum {
  SL_SI91X_EAP_CLIENT             = 1,  ///< SL_SI91X_EAP_CLIENT
  SL_SI91X_FAST_PAC_FILE          = 2,  ///< SL_SI91X_FAST_PAC_FILE
  SL_SI91X_TLS_CLIENT             = 3,  ///< SL_SI91X_TLS_CLIENT
  SL_SI91X_TLS_CLIENT_PRIVATE_KEY = 4,  ///< SL_SI91X_TLS_CLIENT_PRIVATE_KEY
  SL_SI91X_TLS_CA_CERTIFICATE     = 5,  ///< SL_SI91X_TLS_CA_CERTIFICATE
  SL_SI91X_TLS_SERVER_CERTIFICATE = 6,  ///< SL_SI91X_TLS_SERVER_CERTIFICATE
  SL_SI91X_TLS_SERVER_PRIVATE_KEY = 7,  ///< SL_SI91X_TLS_SERVER_PRIVATE_KEY
  SL_SI91X_EAP_PRIVATE_KEY        = 17, ///< SL_SI91X_EAP_PRIVATE_KEY
  SL_SI91X_EAP_PUBLIC_KEY         = 33, ///< SL_SI91X_EAP_PUBLIC_KEY
  SL_SI91X_EAP_CA_CERTIFICATE     = 49, ///< SL_SI91X_EAP_CA_CERTIFICATE
} sl_si91x_cert_type_t;

/******************************************************
 *               Function Declarations
 ******************************************************/
sl_status_t sl_si91x_set_credential(sl_net_credential_id_t id,
                                    sl_net_credential_type_t type,
                                    const void *credential,
                                    uint32_t credential_length);
sl_status_t sl_si91x_get_credential(sl_net_credential_id_t id,
                                    sl_net_credential_type_t *type,
                                    void *credential,
                                    uint32_t *credential_length);
sl_status_t sl_si91x_delete_credential(sl_net_credential_id_t id, sl_net_credential_type_t type);

static sl_si91x_cert_type_t convert_to_si91x_cert_type(sl_net_credential_id_t id, sl_net_credential_type_t type)
{
  switch (type) {
    case SL_NET_SIGNING_CERTIFICATE:
      if ((id == SL_NET_WIFI_EAP_SERVER_CREDENTIAL_ID) || (id == SL_NET_WIFI_EAP_CLIENT_CREDENTIAL_ID)) {
        return SL_SI91X_EAP_CA_CERTIFICATE;
      }
      switch (id & SL_NET_CREDENTIAL_GROUP_MASK) {
        case SL_NET_TLS_SERVER_CREDENTIAL_START:
          return SL_SI91X_TLS_CA_CERTIFICATE;
        default:
          break;
      }
      break;

    case SL_NET_CERTIFICATE:
      if (id == SL_NET_WIFI_EAP_CLIENT_CREDENTIAL_ID) {
        return SL_SI91X_EAP_CLIENT;
      }

      switch (id & SL_NET_CREDENTIAL_GROUP_MASK) {
        case SL_NET_TLS_CLIENT_CREDENTIAL_START:
          return SL_SI91X_TLS_CLIENT;
        case SL_NET_TLS_SERVER_CREDENTIAL_START:
          return SL_SI91X_TLS_SERVER_CERTIFICATE;
        default:
          break;
      }
      break;

    case SL_NET_PUBLIC_KEY:
      switch (id) {
        case SL_NET_WIFI_EAP_CLIENT_CREDENTIAL_ID:
          return SL_SI91X_EAP_PUBLIC_KEY;
        default:
          break;
      }
      break;

    case SL_NET_PRIVATE_KEY:
      if (id == SL_NET_WIFI_EAP_CLIENT_CREDENTIAL_ID) {
        return SL_SI91X_EAP_PRIVATE_KEY;
      }
      switch (id & SL_NET_CREDENTIAL_GROUP_MASK) {
        case SL_NET_TLS_CLIENT_CREDENTIAL_START:
          return SL_SI91X_TLS_CLIENT_PRIVATE_KEY;
        case SL_NET_TLS_SERVER_CREDENTIAL_START:
          return SL_SI91X_TLS_SERVER_PRIVATE_KEY;
        default:
          break;
      }
      break;
    case SL_NET_PACK_FILE:
      if (id == SL_NET_WIFI_EAP_CLIENT_CREDENTIAL_ID) {
        return SL_SI91X_FAST_PAC_FILE;
      }
      break;
    default:
      return 0;
  }

  return 0;
}

static uint8_t get_certificate_index(sl_net_credential_id_t id)
{
  if ((id & SL_NET_CREDENTIAL_GROUP_MASK) != 0) {
    return ((uint8_t)id & 0xFF);
  }
  switch (id) {
    case SL_NET_WIFI_EAP_CLIENT_CREDENTIAL_ID:
    case SL_NET_WIFI_EAP_SERVER_CREDENTIAL_ID:
      return 0;

    default:
      break;
  }
  return -1;
}

sl_status_t sl_si91x_set_credential(sl_net_credential_id_t id,
                                    sl_net_credential_type_t type,
                                    const void *credential,
                                    uint32_t credential_length)
{
  sl_status_t status;
  sl_si91x_cert_type_t cert_type = convert_to_si91x_cert_type(id, type);
  uint8_t index                  = get_certificate_index(id);

  // Clear the certificate
  status = sl_si91x_wifi_set_certificate_index((uint8_t)cert_type, index, NULL, 0);

  // Set the certificate
  status = sl_si91x_wifi_set_certificate_index((uint8_t)cert_type, index, credential, credential_length);

  return status;
}

sl_status_t sl_si91x_get_credential(sl_net_credential_id_t id,
                                    sl_net_credential_type_t *type,
                                    void *credential,
                                    uint32_t *credential_length)
{
  UNUSED_PARAMETER(id);
  UNUSED_PARAMETER(type);
  UNUSED_PARAMETER(credential);
  UNUSED_PARAMETER(credential_length);
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_si91x_delete_credential(sl_net_credential_id_t id, sl_net_credential_type_t type)
{
  sl_si91x_cert_type_t cert_type = convert_to_si91x_cert_type(id, type);
  uint8_t index                  = get_certificate_index(id);

  // Clear the certificate
  return sl_si91x_wifi_set_certificate_index((uint8_t)cert_type, index, NULL, 0);
}

sl_status_t sl_si91x_host_get_credentials(sl_wifi_credential_id_t id, uint8_t type, sl_wifi_credential_t *cred)
{
  UNUSED_PARAMETER(type);
  sl_net_credential_type_t actual_type;
  uint32_t credential_length = sizeof(sl_wifi_credential_t) - offsetof(sl_wifi_credential_t, pmk);
  sl_status_t status         = sl_net_get_credential(id, &actual_type, &cred->pmk, &credential_length);
  VERIFY_STATUS_AND_RETURN(status);

  // Map the network credential type to a simplified type
  switch (actual_type) {
    case SL_NET_WIFI_PSK:
      // Set the credential type to Pre-Shared Key (PSK)
      cred->type = SL_WIFI_PSK_CREDENTIAL;
      break;
    case SL_NET_WIFI_PMK:
      // Set the credential type to Pairwise Master Key (PMK)
      cred->type = SL_WIFI_PMK_CREDENTIAL;
      break;
    case SL_NET_WIFI_WEP:
      // Set the credential type to Wired Equivalent Privacy (WEP)
      cred->type = SL_WIFI_WEP_CREDENTIAL;
      break;
    case SL_NET_EAP_CLIENT_CREDENTIAL:
      // Set the credential type to Extensible Authentication Protocol (EAP)
      cred->type = SL_WIFI_EAP_CREDENTIAL;
      break;
    default:
      return SL_STATUS_FAIL;
  }
  return SL_STATUS_OK;
}
