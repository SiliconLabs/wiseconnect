/*******************************************************************************
* @file  sl_net_nvm_credentials.c
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
#include "sl_net_wifi_types.h"
#include "nvm3.h"
#include "sl_common.h"
#ifdef SLI_SI917
#include "sl_net_si91x.h"
#endif
#include <string.h>

#ifndef SL_NET_CREDENTIAL_NVM3_BASE_KEY
#define SL_NET_CREDENTIAL_NVM3_BASE_KEY 0x3000
#endif

#define SL_NET_CREDENTIAL_TABLE_KEY SL_NET_CREDENTIAL_NVM3_BASE_KEY
#define CRED_TYPE_CERT              0
#define CRED_TYPE_CRED              1

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

static int check_cred_type(sl_net_credential_type_t type)
{
  if ((SL_NET_CERTIFICATE == type) || (SL_NET_PUBLIC_KEY == type) || (SL_NET_PRIVATE_KEY == type)
      || (SL_NET_SIGNING_CERTIFICATE == type)) {
    return CRED_TYPE_CERT;
  }

  return CRED_TYPE_CRED;
}

sl_status_t sl_net_set_credential(sl_net_credential_id_t id,
                                  sl_net_credential_type_t type,
                                  const void *credential,
                                  uint32_t credential_length)
{
  Ecode_t ecode;
  sl_net_credential_entry_t *entry;
  int group_id = 0;
  int cred_id  = 0;

  if (id == SL_NET_INVALID_CREDENTIAL_ID) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (CRED_TYPE_CERT == check_cred_type(type)) {
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

  switch (group_id) {
    case SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID:
    case SL_NET_DEFAULT_WIFI_AP_CREDENTIAL_ID:
    case SL_NET_WIFI_EAP_CLIENT_CREDENTIAL_ID:
    case SL_NET_WIFI_EAP_SERVER_CREDENTIAL_ID:
    case SL_NET_USER_CREDENTIAL_ID:
    case SL_NET_TLS_CLIENT_CREDENTIAL_START:
    case SL_NET_TLS_SERVER_CREDENTIAL_START:
    case SL_NET_MQTT_SERVER_CREDENTIAL_START:
    case SL_NET_MQTT_CLIENT_CREDENTIAL_START:
    case SL_NET_HTTP_SERVER_CREDENTIAL_START:
    case SL_NET_HTTP_CLIENT_CREDENTIAL_START:
      find_entry(cred_id, &entry);

      if (entry->type != type) {
        entry->type = type;
        save_credential_table();
      }
      ecode =
        nvm3_writeData(nvm3_defaultHandle, SL_NET_CREDENTIAL_NVM3_BASE_KEY + cred_id, credential, credential_length);
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
  int group_id = 0;
  int cred_id  = 0;

  group_id = (id & SL_NET_CREDENTIAL_GROUP_MASK);

  if (group_id > 0) {
    cred_id = (SL_NET_USER_CREDENTIAL_ID + (group_id >> 8));
  } else {
    group_id = id;
    cred_id  = id;
  }

  find_entry(cred_id, &entry);

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
      ecode =
        nvm3_getObjectInfo(nvm3_defaultHandle, SL_NET_CREDENTIAL_NVM3_BASE_KEY + cred_id, &object_type, &data_length);
      if (credential_length != NULL) {
        if (credential != NULL) {
          data_length = SL_MIN(data_length, *credential_length);
          ecode = nvm3_readData(nvm3_defaultHandle, SL_NET_CREDENTIAL_NVM3_BASE_KEY + cred_id, credential, data_length);
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

sl_status_t sl_net_delete_credential(sl_net_credential_id_t id, sl_net_credential_type_t type)
{
  int group_id = 0;
  int cred_id  = 0;

  if (CRED_TYPE_CERT == check_cred_type(type)) {
#ifdef SLI_SI917
    return sl_si91x_delete_credential(id, type);
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
      nvm3_deleteObject(nvm3_defaultHandle, SL_NET_CREDENTIAL_NVM3_BASE_KEY + cred_id);
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
