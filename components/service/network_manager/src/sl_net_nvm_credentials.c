#include "sl_net.h"
#include "sl_net_wifi_types.h"
#include "nvm3.h"
#include <string.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#ifndef SL_NET_CREDENTIAL_NVM3_BASE_KEY
#define SL_NET_CREDENTIAL_NVM3_BASE_KEY 0x3000
#endif

#define SL_NET_CREDENTIAL_TABLE_KEY SL_NET_CREDENTIAL_NVM3_BASE_KEY

#define VERIFY_ECODE_AND_RETURN(ecode) \
  do {                                 \
    if (ecode != ECODE_OK) {           \
      return SL_STATUS_FAIL;           \
    }                                  \
  } while (0)

typedef struct {
  sl_net_credential_id_t id;
  sl_net_credential_type_t type;
} sl_net_credential_entry_t;

sl_net_credential_entry_t *credential_table = NULL;
size_t credential_table_size                = 0;

static sl_status_t load_credential_table(void);
static sl_status_t save_credential_table(void);
static sl_status_t find_entry(sl_net_credential_id_t id, sl_net_credential_entry_t **entry);

sl_status_t sl_net_set_credential(sl_net_credential_id_t id,
                                  sl_net_credential_type_t type,
                                  const void *credential,
                                  uint32_t credential_length)
{
  Ecode_t ecode;
  if (id == SL_NET_INVALID_CREDENTIAL_ID) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_net_credential_entry_t *entry;
  find_entry(id, &entry);

  switch (id) {
    case SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID:
    case SL_NET_DEFAULT_WIFI_AP_CREDENTIAL_ID:
      if (entry->type != type) {
        entry->type = type;
        save_credential_table();
      }
      ecode = nvm3_writeData(nvm3_defaultHandle, SL_NET_CREDENTIAL_NVM3_BASE_KEY + id, credential, credential_length);
      VERIFY_ECODE_AND_RETURN(ecode);
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
  Ecode_t ecode;
  size_t data_length;
  sl_net_credential_entry_t *entry;
  uint32_t object_type;

  find_entry(id, &entry);

  switch (id) {
    case SL_NET_DEFAULT_WIFI_AP_CREDENTIAL_ID:
    case SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID:
      ecode = nvm3_getObjectInfo(nvm3_defaultHandle, SL_NET_CREDENTIAL_NVM3_BASE_KEY + id, &object_type, &data_length);
      if (credential_length != NULL) {
        if (credential != NULL) {
          data_length = MIN(data_length, *credential_length);
          ecode = nvm3_readData(nvm3_defaultHandle, SL_NET_CREDENTIAL_NVM3_BASE_KEY + id, credential, data_length);
          if (ecode != ECODE_OK) {
            return SL_STATUS_FAIL;
          }
          *type = entry->type;
        }

        *credential_length = data_length;
      }

      return SL_STATUS_OK;
      break;

    default:
      return SL_STATUS_FAIL;
  }
}

sl_status_t sl_net_delete_credential(sl_net_credential_id_t id)
{
  switch (id) {
    case SL_NET_DEFAULT_WIFI_AP_CREDENTIAL_ID:
    case SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID:
      nvm3_deleteObject(nvm3_defaultHandle, SL_NET_CREDENTIAL_NVM3_BASE_KEY + id);
      break;

    default:
      return SL_STATUS_FAIL;
  }
  return SL_STATUS_OK;
}

static sl_status_t save_credential_table(void)
{
  Ecode_t ecode =
    nvm3_writeData(nvm3_defaultHandle, SL_NET_CREDENTIAL_TABLE_KEY, credential_table, credential_table_size);
}

static sl_status_t load_credential_table(void)
{
  if (credential_table != NULL) {
    return SL_STATUS_OK;
  }

  uint32_t type = 0xFFFFFFFFU;

  if (nvm3_getObjectInfo(nvm3_defaultHandle, SL_NET_CREDENTIAL_TABLE_KEY, &type, &credential_table_size) != 0) {
    credential_table_size = 16;
  }
  credential_table = malloc(credential_table_size);
  if (credential_table == NULL) {
    return SL_STATUS_ALLOCATION_FAILED;
  }

  if (type != 0xFFFFFFFFU) {
    Ecode_t ecode =
      nvm3_readData(nvm3_defaultHandle, SL_NET_CREDENTIAL_TABLE_KEY, credential_table, credential_table_size);
  } else {
    memset(credential_table, 0, credential_table_size);
  }

  return SL_STATUS_OK;
}

static sl_status_t find_entry(sl_net_credential_id_t id, sl_net_credential_entry_t **entry)
{
  sl_net_credential_entry_t *first_empty_slot = NULL;

  if (credential_table == NULL) {
    load_credential_table();
  }

  for (int a = 0; a < credential_table_size / sizeof(sl_net_credential_entry_t); ++a) {
    if (credential_table[a].id == id) {
      *entry = &credential_table[a];
      return SL_STATUS_OK;
    }
    if (credential_table[a].id == 0 && first_empty_slot == NULL) {
      first_empty_slot = &credential_table[a];
    }
  }

  if (first_empty_slot == NULL) {
    return SL_STATUS_FAIL;
  }

  first_empty_slot->id = id;
  *entry               = first_empty_slot;
  return SL_STATUS_OK;
}
