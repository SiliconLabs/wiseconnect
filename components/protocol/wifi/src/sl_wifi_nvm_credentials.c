/*******************************************************************************
* @file  sl_wifi_nvm_credentials.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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
#include <string.h>
#include "nvm3.h"
#include "sl_wifi.h"
#include "sl_wifi_types.h"
#include "sl_common.h"

#ifndef SL_WIFI_MAX_CREDENTIAL_COUNT
#define SL_WIFI_MAX_CREDENTIAL_COUNT 16
#endif

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
  sl_wifi_credential_id_t id;
  sl_wifi_credential_type_t type;
} sl_wifi_nvm_credential_entry_t;

static sl_wifi_nvm_credential_entry_t *credential_table = NULL;
static size_t credential_table_size                     = 0;

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
    credential_table_size = SL_WIFI_MAX_CREDENTIAL_COUNT;
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

static sl_status_t find_entry(sl_wifi_credential_id_t id, sl_wifi_nvm_credential_entry_t **entry)
{
  sl_wifi_nvm_credential_entry_t *first_empty_slot = NULL;

  if (credential_table == NULL) {
    load_credential_table();
  }

  for (int a = 0; a < credential_table_size / sizeof(sl_wifi_nvm_credential_entry_t); ++a) {
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

sl_status_t sl_wifi_set_credential(sl_wifi_credential_id_t id,
                                   sl_wifi_credential_type_t type,
                                   const void *credential,
                                   uint32_t credential_length)
{
  Ecode_t ecode;
  sl_wifi_nvm_credential_entry_t *entry;

  if (id >= SL_WIFI_MAX_CREDENTIAL_COUNT) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Check if the credential is invalid parameter
  if ((NULL == credential) || (0 == credential_length)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  find_entry(id, &entry);

  if (entry->type != type) {
    entry->type = type;
    save_credential_table();
  }
  ecode = nvm3_writeData(nvm3_defaultHandle, SL_NET_CREDENTIAL_NVM3_BASE_KEY + id, credential, credential_length);
  VERIFY_ECODE_AND_RETURN(ecode);

  return SL_STATUS_OK;
}

sl_status_t sl_wifi_get_credential(sl_wifi_credential_id_t id,
                                   sl_wifi_credential_type_t *type,
                                   void *credential,
                                   uint32_t *credential_length)
{
  Ecode_t ecode;
  size_t data_length;
  sl_wifi_nvm_credential_entry_t *entry;
  uint32_t object_type;

  // Check if the credential ID is invalid parameter
  if (id >= SL_WIFI_MAX_CREDENTIAL_COUNT) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Check if the credential is invalid parameter
  if ((NULL == credential) || (0 == *credential_length)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  find_entry(id, &entry);
  ecode = nvm3_getObjectInfo(nvm3_defaultHandle, SL_NET_CREDENTIAL_NVM3_BASE_KEY + id, &object_type, &data_length);

  if (credential_length != NULL) {
    if (credential != NULL) {
      data_length = SL_MIN(data_length, *credential_length);
      ecode       = nvm3_readData(nvm3_defaultHandle, SL_NET_CREDENTIAL_NVM3_BASE_KEY + id, credential, data_length);
      if (ecode != ECODE_OK) {
        return SL_STATUS_FAIL;
      }
      *type = entry->type;
    }

    *credential_length = data_length;
  }

  return SL_STATUS_OK;
}

sl_status_t sl_wifi_delete_credential(sl_wifi_credential_id_t id)
{
  // Check if the credential ID is invalid parameter
  if (id >= SL_WIFI_MAX_CREDENTIAL_COUNT) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  nvm3_deleteObject(nvm3_defaultHandle, SL_NET_CREDENTIAL_NVM3_BASE_KEY + id);
  return SL_STATUS_OK;
}
