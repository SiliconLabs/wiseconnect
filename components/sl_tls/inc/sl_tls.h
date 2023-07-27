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
#ifndef SL_TLS_H
#define SL_TLS_H

#include <stdbool.h>

#include "sl_status.h"
#include "sl_net_constants.h"

typedef uint8_t sl_cert_identifier_t;

// Note: The following enums and the corresponding elements in sl_tls_store_configuration_t structure must be removed once the firmware is fixed.

/**
 * @addtogroup SL_TLS_CONSTANTS
 * @{ 
 */

/// Enumerations of TLS Certificate types
typedef enum {
  SL_TLS_EAP_CLIENT             = 1,  ///< TLS Enterprise Client Certificate
  SL_TLS_FAST_PAC_FILE          = 2,  ///< TLS PAC Certificate file
  SL_TLS_SSL_CLIENT             = 3,  ///< TLS SSL Client Certificate
  SL_TLS_SSL_CLIENT_PRIVATE_KEY = 4,  ///< TLS SSL Client Private Key Certificate
  SL_TLS_SSL_CA_CERTIFICATE     = 5,  ///< TLS SSL CA Certificate
  SL_TLS_SSL_SERVER_CERTIFICATE = 6,  ///< TLS SSL Server Certificate
  SL_TLS_SSL_SERVER_PRIVATE_KEY = 7,  ///< TLS SSL Server Private Key
  SL_TLS_EAP_PRIVATE_KEY        = 17, ///< TLS Enterprise Client Private key
  SL_TLS_EAP_PUBLIC_KEY         = 33, ///< TLS Enterprise Client Public Key
  SL_TLS_EAP_CA_CERTIFICATE     = 49  ///< TLS Enterprise Client CA Certificate
} sl_tls_certificate_type;

/// Enumerations of application protocols
typedef enum {
  SL_NET_APPLICATION_PROTOCOL_HTTP_HTTPS = 1,        ///< HTTP/HTTPS Application Protocol
  SL_NET_APPLICATION_PROTOCOL_MQTT       = (1 << 1), ///< MQTT Application protocol
  SL_NET_APPLICATION_PROTOCOL_FTP        = (1 << 2), ///< FTP Application protocol
  SL_NET_APPLICATION_PROTOCOL_SSL_SOCKET = (1 << 3), ///< SSL Socket Application protocol
  SL_NET_APPLICATION_PROTOCOL_ALL        = 0xFF      ///< All Application Protocols
} sl_net_application_protocol_t;                     // This enum should be defined in sl_net.h

/** @} */

/**
 * @addtogroup SL_TLS_TYPES
 * @{
 */

/// TLS store configuration structure
typedef struct {
  uint8_t *cacert;            ///< Pointer to certificate authority's certificate buffer
  unsigned int cacert_length; ///< Size of certificate authority certificate pointed by cacert
  sl_tls_certificate_type cacert_type;
  unsigned char *clientcert;      ///< Pointer to client certificate buffer
  unsigned int clientcert_length; ///< Size of client certificate pointed to by clientcert
  sl_tls_certificate_type clientcert_type;
  unsigned char *clientkey;      ///< Pointer to client key buffer
  unsigned int clientkey_length; ///< Size of client key pointed to by clientkey
  sl_tls_certificate_type clientkey_type;
  sl_cert_identifier_t cert_identifier; ///< Identifier for certificate store
  bool use_secure_element;              ///< Enable this option to use secure element or atecc608a chip
} sl_tls_store_configuration_t;

/** @} */

/** 
 * @addtogroup GLOBAL_TLS_STORE_API Global Store
 * @ingroup SL_TLS_FUNCTIONS
 * @{ 
 */

// Global TLS store APIs

/***************************************************************************/ /**
 * @brief
 *   Initialize the Global TLS store configuration.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] void
 *   This API does not require any arguments.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_tls_store_init(void);

/***************************************************************************/ /**
 * @brief
 *   Set the Global TLS store configuration.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] configuration
 *   @ref sl_tls_store_configuration_t object that contains TLS certificate configuration.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_tls_set_global_ca_store(const sl_tls_store_configuration_t configuration);

/***************************************************************************/ /**
 * @brief
 *   Get the Global TLS store configuration.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] configuration
 *   Pointer to @ref sl_tls_store_configuration_t object that can store TLS certificate configuration.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_tls_get_global_ca_store(sl_tls_store_configuration_t *configuration);

/***************************************************************************/ /**
 * @brief
 *   Clear the Global TLS store configuration.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] void
 *   This API does not require any arguments.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_tls_clear_global_ca_store(void);

/** @} */

/** 
 * @addtogroup INTERFACE_PROTOCOL_TLS_STORE_API Interface and Protocol Store
 * @ingroup SL_TLS_FUNCTIONS
 * @{ 
 */

// Interface/Protocol specific TLS store functions

/***************************************************************************/ /**
 * @brief
 *   Set the TLS store configuration specific to Interface/Protocol.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   SL-NET interface as identified by @ref sl_net_interface_t.
 * @param[in] sl_net_application_protocol_t
 *   Application protocol as identified by @ref sl_net_application_protocol_t.
 * @param[in] configuration
 *   @ref sl_tls_store_configuration_t object that contains TLS certificate configuration.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_tls_set_ca_store(sl_net_interface_t interface,
                                sl_net_application_protocol_t protocol,
                                const sl_tls_store_configuration_t configuration);

/***************************************************************************/ /**
 * @brief
 *   Get the TLS store configuration specific to Interface/Protocol.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   SL-NET interface as identified by @ref sl_net_interface_t.
 * @param[in] sl_net_application_protocol_t
 *   Application protocol as identified by @ref sl_net_application_protocol_t.
 * @param[in] configuration
 *   Pointer to @ref sl_tls_store_configuration_t object that can store TLS certificate configuration.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_tls_get_ca_store(sl_net_interface_t interface,
                                sl_net_application_protocol_t protocol,
                                sl_tls_store_configuration_t *configuration);

/***************************************************************************/ /**
 * @brief
 *   Clear the TLS store configuration specific to Interface/Protocol.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   SL-NET interface as identified by @ref sl_net_interface_t.
 * @param[in] sl_net_application_protocol_t
 *   Application protocol as identified by @ref sl_net_application_protocol_t.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_tls_clear_ca_store(sl_net_interface_t interface,
                                  sl_net_application_protocol_t protocol,
                                  sl_cert_identifier_t cert_identifier);

/** @} */

#endif //SL_TLS_H
