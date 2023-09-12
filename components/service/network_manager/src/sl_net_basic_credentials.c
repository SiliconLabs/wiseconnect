#include "sl_net.h"
#include "sl_net_default_values.h"
#include <string.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct {
  sl_net_credential_type_t type;
  uint16_t data_length;
  uint8_t data[];
} sl_net_basic_credential_entry_t;

#define MAX_CREDENTIAL_COUNT 10

#ifndef DEFAULT_WIFI_CLIENT_CREDENTIAL
#define DEFAULT_WIFI_CLIENT_CREDENTIAL "YOUR_AP_PASSPHRASE"
#endif

static sl_net_basic_credential_entry_t *credentials[MAX_CREDENTIAL_COUNT] = {
  [SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID] = (sl_net_basic_credential_entry_t *)&default_wifi_client_credential,
  [SL_NET_DEFAULT_WIFI_AP_CREDENTIAL_ID]     = (sl_net_basic_credential_entry_t *)&default_wifi_ap_credential,
};

sl_status_t sl_net_set_credential(sl_net_credential_id_t id,
                                  sl_net_credential_type_t type,
                                  const void *credential,
                                  uint32_t credential_length)
{
  if (id >= MAX_CREDENTIAL_COUNT) {
    return SL_STATUS_NOT_SUPPORTED;
  }
  if (credentials[id] == NULL) {
    credentials[id] = malloc(sizeof(sl_net_basic_credential_entry_t) + credential_length);
    if (credentials[id] == NULL) {
      return SL_STATUS_ALLOCATION_FAILED;
    }
    credentials[id]->data_length = credential_length;
  }
  sl_net_basic_credential_entry_t *entry = credentials[id];

  switch (id) {
    case SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID: {
      if (credential_length > sizeof(default_wifi_client_credential.data)) {
        return SL_STATUS_NOT_SUPPORTED;
      }
      entry->type        = type;
      entry->data_length = MIN(credential_length, sizeof(default_wifi_client_credential.data));
      memcpy(entry->data, credential, entry->data_length);

      break;
    }

    case SL_NET_DEFAULT_WIFI_AP_CREDENTIAL_ID: {
      if (credential_length > sizeof(default_wifi_ap_credential.data)) {
        return SL_STATUS_NOT_SUPPORTED;
      }
      entry->type        = type;
      entry->data_length = MIN(credential_length, sizeof(default_wifi_ap_credential.data));
      memcpy(entry->data, credential, entry->data_length);

      break;
    }

    case SL_NET_MQTT_CLIENT_CREDENTIALS_ID: {
      entry->type        = type;
      entry->data_length = credential_length;
      memcpy(entry->data, credential, entry->data_length);

      break;
    }

    case SL_NET_HTTP_CLIENT_CREDENTIAL_ID: {
      entry->type        = type;
      entry->data_length = credential_length;
      memcpy(entry->data, credential, entry->data_length);
      break;
    }

    case SL_NET_DEFAULT_PMK_CREDENTIAL_ID: {
      entry->type        = type;
      entry->data_length = credential_length;
      memcpy(entry->data, credential, entry->data_length);
      break;
    }

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
  sl_net_basic_credential_entry_t *entry = credentials[id];

  *type = entry->type;

  if (credential != NULL) {
    switch (id) {
      case SL_NET_DEFAULT_WIFI_AP_CREDENTIAL_ID:
      case SL_NET_MQTT_CLIENT_CREDENTIALS_ID:
      case SL_NET_HTTP_CLIENT_CREDENTIAL_ID:
      case SL_NET_DEFAULT_PMK_CREDENTIAL_ID:
      case SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID:
        if (*credential_length < entry->data_length) {
          return SL_STATUS_FAIL;
        }

        *type              = entry->type;
        *credential_length = entry->data_length;
        memcpy(credential, entry->data, entry->data_length);
        return SL_STATUS_OK;
        break;

      default:
        return SL_STATUS_FAIL;
    }
  }
  return SL_STATUS_OK;
}

sl_status_t sl_net_delete_credential(sl_net_credential_id_t id)
{
  UNUSED_PARAMETER(id);
  return SL_STATUS_OK;
}
