/*******************************************************************************
* @file  sl_net_basic_credentials.c
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

#include "sl_net.h"
#include "sl_net_default_values.h"
#include "sl_common.h"
#ifdef SLI_SI917
#include "sl_net_si91x.h"
#endif
#include <string.h>

typedef struct {
  sl_net_credential_type_t type;
  uint16_t data_length;
  uint8_t data[];
} sl_net_basic_credential_entry_t;

#define MAX_CREDENTIAL_COUNT 12
#define CRED_TYPE_CERT       0
#define CRED_TYPE_CRED       1

#ifndef DEFAULT_WIFI_CLIENT_CREDENTIAL
#define DEFAULT_WIFI_CLIENT_CREDENTIAL "YOUR_AP_PASSPHRASE"
#endif

sl_status_t sl_si91x_delete_credential(sl_net_credential_id_t id, sl_net_credential_type_t type);

static sl_net_basic_credential_entry_t *credentials[MAX_CREDENTIAL_COUNT] = {
  [SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID] = (sl_net_basic_credential_entry_t *)&default_wifi_client_credential,
  [SL_NET_DEFAULT_WIFI_AP_CREDENTIAL_ID]     = (sl_net_basic_credential_entry_t *)&default_wifi_ap_credential,
};

static int sli_si91x_check_cred_type(sl_net_credential_type_t type)
{
  if ((SL_NET_CERTIFICATE == type) || (SL_NET_PUBLIC_KEY == type) || (SL_NET_PRIVATE_KEY == type)
      || (SL_NET_SIGNING_CERTIFICATE == type) || (SL_NET_PACK_FILE == type)) {
    return CRED_TYPE_CERT;
  }

  return CRED_TYPE_CRED;
}

sl_status_t sl_net_set_credential(sl_net_credential_id_t id,
                                  sl_net_credential_type_t type,
                                  const void *credential,
                                  uint32_t credential_length)
{
  sl_net_basic_credential_entry_t *entry = NULL;
  int group_id                           = 0;
  int cred_id                            = 0;

  if (CRED_TYPE_CERT == sli_si91x_check_cred_type(type)) {
#ifdef SLI_SI917
    return sl_si91x_set_credential(id, type, credential, credential_length);
#else
    return SL_STATUS_FAIL;
#endif
  }

  group_id = (id & SL_NET_CREDENTIAL_GROUP_MASK);

  if (group_id > 0) {
    cred_id = (SL_NET_USER_CREDENTIAL_ID + (group_id >> 8));
  } else {
    group_id = id;
    cred_id  = id;
  }

  if (cred_id < MAX_CREDENTIAL_COUNT) {
    if (credentials[cred_id] == NULL) {
      credentials[cred_id] = malloc(sizeof(sl_net_basic_credential_entry_t) + credential_length);
      if (credentials[cred_id] == NULL) {
        return SL_STATUS_ALLOCATION_FAILED;
      }
      memset(credentials[cred_id], 0, sizeof(sl_net_basic_credential_entry_t) + credential_length);
      credentials[cred_id]->data_length = credential_length;
    }
    entry = credentials[cred_id];
  }

  switch (group_id) {
    case SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID:
      if (credential_length > sizeof(default_wifi_client_credential.data)) {
        return SL_STATUS_NOT_SUPPORTED;
      }
      entry->type        = type;
      entry->data_length = SL_MIN(credential_length, sizeof(default_wifi_client_credential.data));
      memcpy(entry->data, credential, entry->data_length);
      break;

    case SL_NET_DEFAULT_WIFI_AP_CREDENTIAL_ID:
      if (credential_length > sizeof(default_wifi_ap_credential.data)) {
        return SL_STATUS_NOT_SUPPORTED;
      }
      entry->type        = type;
      entry->data_length = SL_MIN(credential_length, sizeof(default_wifi_ap_credential.data));
      memcpy(entry->data, credential, entry->data_length);
      break;
    case SL_NET_WIFI_EAP_CLIENT_CREDENTIAL_ID:
    case SL_NET_WIFI_EAP_SERVER_CREDENTIAL_ID:
    case SL_NET_USER_CREDENTIAL_ID:
    case SL_NET_TLS_CLIENT_CREDENTIAL_START:
    case SL_NET_TLS_SERVER_CREDENTIAL_START:
    case SL_NET_MQTT_SERVER_CREDENTIAL_START:
    case SL_NET_MQTT_CLIENT_CREDENTIAL_START:
    case SL_NET_HTTP_SERVER_CREDENTIAL_START:
    case SL_NET_HTTP_CLIENT_CREDENTIAL_START:
      entry->type        = type;
      entry->data_length = credential_length;
      memcpy(entry->data, credential, entry->data_length);
      break;

    default:
      return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

sl_status_t sl_net_get_credential(sl_net_credential_id_t id,
                                  sl_net_credential_type_t *type,
                                  void *credential,
                                  uint32_t *credential_length)
{
  sl_net_basic_credential_entry_t *entry = NULL;
  int group_id                           = 0;
  int cred_id                            = 0;

  group_id = (id & SL_NET_CREDENTIAL_GROUP_MASK);

  if (group_id > 0) {
    cred_id = (SL_NET_USER_CREDENTIAL_ID + (group_id >> 8));
  } else {
    group_id = id;
    cred_id  = id;
  }
  entry = credentials[cred_id];
  *type = entry->type;

  if (credential != NULL) {
    switch (group_id) {
      case SL_NET_DEFAULT_WIFI_AP_CREDENTIAL_ID:
      case SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID:
      case SL_NET_WIFI_EAP_CLIENT_CREDENTIAL_ID:
      case SL_NET_WIFI_EAP_SERVER_CREDENTIAL_ID:
      case SL_NET_USER_CREDENTIAL_ID:
      case SL_NET_TLS_CLIENT_CREDENTIAL_START:
      case SL_NET_TLS_SERVER_CREDENTIAL_START:
      case SL_NET_MQTT_SERVER_CREDENTIAL_START:
      case SL_NET_MQTT_CLIENT_CREDENTIAL_START:
      case SL_NET_HTTP_SERVER_CREDENTIAL_START:
      case SL_NET_HTTP_CLIENT_CREDENTIAL_START:
        if (*credential_length < entry->data_length) {
          return SL_STATUS_FAIL;
        }

        *type              = entry->type;
        *credential_length = entry->data_length;
        memcpy(credential, entry->data, entry->data_length);
        return SL_STATUS_OK;

      default:
        return SL_STATUS_FAIL;
    }
  }
  return SL_STATUS_OK;
}

sl_status_t sl_net_delete_credential(sl_net_credential_id_t id, sl_net_credential_type_t type)
{
  if (CRED_TYPE_CERT == sli_si91x_check_cred_type(type)) {
#ifdef SLI_SI917
    return sl_si91x_delete_credential(id, type);
#else
    return SL_STATUS_FAIL;
#endif
  }

  return SL_STATUS_OK;
}
