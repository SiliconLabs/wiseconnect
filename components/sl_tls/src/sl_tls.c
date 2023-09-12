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
#include "sl_constants.h"
#include "sl_tls.h"
#include "sl_si91x_driver.h"
#include "sl_constants.h"

#define GLOBAL_TLS_STORE_INDEX   0
#define PROTOCOL_TLS_STORE_INDEX (GLOBAL_TLS_STORE_INDEX + 1)
#define PROTOCOL_TLS_STORE_SIZE  2

typedef struct {
  sl_net_interface_t interface;
  sl_net_application_protocol_t protocol;
  bool enabled;
} sl_tls_protocol_tls_store_status;

static sl_tls_protocol_tls_store_status protocol_store[PROTOCOL_TLS_STORE_SIZE] = { 0 };

static sl_status_t set_tls_store(uint8_t index, const sl_tls_store_configuration_t *configuration)
{
  sl_status_t status = SL_STATUS_INVALID_PARAMETER;

  if ((NULL != configuration->cacert) && (0 != configuration->cacert_length)) {
    status = sl_si91x_wifi_set_certificate_index((uint8_t)configuration->cacert_type,
                                                 index,
                                                 configuration->cacert,
                                                 configuration->cacert_length);

    if (SL_STATUS_OK != status) {
      return status;
    }
  }

  if ((NULL != configuration->clientcert) && (0 != configuration->clientcert_length)) {
    status = sl_si91x_wifi_set_certificate_index((uint8_t)configuration->clientcert_type,
                                                 index,
                                                 configuration->clientcert,
                                                 configuration->clientcert_length);

    if (SL_STATUS_OK != status) {
      return status;
    }
  }

  if ((NULL != configuration->clientkey) && (0 != configuration->clientkey_length)) {
    status = sl_si91x_wifi_set_certificate_index((uint8_t)configuration->clientkey_type,
                                                 index,
                                                 configuration->clientkey,
                                                 configuration->clientkey_length);

    if (SL_STATUS_OK != status) {
      return status;
    }
  }

  return status;
}

static sl_status_t clear_tls_store(uint8_t index)
{
  sl_status_t status = SL_STATUS_INVALID_PARAMETER;
  int i;
  int cert_type[] = { SL_TLS_EAP_CLIENT,
                      SL_TLS_FAST_PAC_FILE,
                      SL_TLS_SSL_CLIENT,
                      SL_TLS_SSL_CLIENT_PRIVATE_KEY,
                      SL_TLS_SSL_CA_CERTIFICATE,
                      SL_TLS_SSL_SERVER_CERTIFICATE,
                      SL_TLS_SSL_SERVER_PRIVATE_KEY,
                      SL_TLS_EAP_PRIVATE_KEY,
                      SL_TLS_EAP_PUBLIC_KEY,
                      SL_TLS_EAP_CA_CERTIFICATE };

  for (i = 0; i < (int)(sizeof(cert_type) / sizeof(int)); i++) {
    status = sl_si91x_wifi_set_certificate_index(cert_type[i], index, NULL, 0);

    if (SL_STATUS_OK != status) {
      return status;
    }
  }

  return status;
}

sl_status_t sl_tls_global_ca_store_init(void)
{
  return SL_STATUS_OK;
}

sl_status_t sl_tls_set_global_ca_store(const sl_tls_store_configuration_t configuration)
{
  return set_tls_store(GLOBAL_TLS_STORE_INDEX, &configuration);
}

sl_status_t sl_tls_get_global_ca_store(sl_tls_store_configuration_t *configuration)
{
  UNUSED_PARAMETER(configuration);
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_tls_clear_global_ca_store(void)
{
  return clear_tls_store(GLOBAL_TLS_STORE_INDEX);
}

sl_status_t sl_tls_set_ca_store(sl_net_interface_t interface,
                                sl_net_application_protocol_t protocol,
                                const sl_tls_store_configuration_t configuration)
{
  uint8_t cert_index = 0;

  if ((configuration.cert_identifier < PROTOCOL_TLS_STORE_INDEX)
      || (configuration.cert_identifier > PROTOCOL_TLS_STORE_SIZE)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  cert_index = configuration.cert_identifier - 1;

  if ((false == protocol_store[cert_index].enabled)
      || ((protocol_store[cert_index].interface == interface) && (protocol_store[cert_index].protocol == protocol))) {
    protocol_store[cert_index].interface = interface;
    protocol_store[cert_index].protocol  = protocol;
    protocol_store[cert_index].enabled   = true;
  } else {
    return SL_STATUS_ALREADY_INITIALIZED;
  }

  return set_tls_store(configuration.cert_identifier, &configuration);
}

sl_status_t sl_tls_get_ca_store(sl_net_interface_t interface,
                                sl_net_application_protocol_t protocol,
                                sl_tls_store_configuration_t *configuration)
{
  UNUSED_PARAMETER(interface);
  UNUSED_PARAMETER(protocol);
  UNUSED_PARAMETER(configuration);

  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_tls_clear_ca_store(sl_net_interface_t interface,
                                  sl_net_application_protocol_t protocol,
                                  sl_cert_identifier_t cert_identifier)
{
  sl_status_t status = 0;
  uint8_t index      = 0;

  if ((cert_identifier < PROTOCOL_TLS_STORE_INDEX) || (cert_identifier > PROTOCOL_TLS_STORE_SIZE)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  index = cert_identifier - 1;

  if (protocol_store[index].enabled) {
    if ((protocol_store[index].interface != interface) || (protocol_store[index].protocol != protocol)) {
      return SL_STATUS_INVALID_PARAMETER;
    }
  }

  status = clear_tls_store(cert_identifier);
  if (SL_STATUS_OK == status) {
    protocol_store[cert_identifier].enabled = false;
  }

  return status;
}
