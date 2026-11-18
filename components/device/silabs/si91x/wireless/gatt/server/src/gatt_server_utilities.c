/*******************************************************************************
* @file  gatt_server_utilities.c
* @brief GATT Server Component - Utility Functions
*******************************************************************************
* # License
* <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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

/**
 * @brief GATT Server utility functions
 * EXTRACTED FROM: ble_unified_app.c, gap_utilities.c
 */

#include <stdio.h>
#include <string.h>
#include <gatt_server_config.h> // resolved via -I: config/ble_config/ (user override) first, then component inc/
#include "gatt_server.h"
#include "gap.h"
#include "rsi_ble.h"
#include "rsi_ble_apis.h"
#include "rsi_ble_common_config.h"
#include "rsi_common_utils.h"
#include "rsi_utils.h"
#include "rsi_bt_common_apis.h" // For RSI_SUCCESS, RSI_FAILURE (matches GAP and SMP pattern)

extern rsi_parsed_conf_t rsi_parsed_conf;

/* GATT Server-owned variables */
rsi_ble_t att_list                      = { 0 };
volatile uint16_t rsi_ble_att1_val_hndl = 0;
volatile uint16_t rsi_ble_att2_val_hndl = 0;
volatile uint16_t rsi_ble_att3_val_hndl = 0;

/*=======================================================================*/
//! Category 1: Connection State Initialization
/*=======================================================================*/

void rsi_ble_gatt_server_default_init(void)
{
  uint8_t iter;
  for (iter = 0; iter < TOTAL_CONNECTIONS; iter++) {
    rsi_ble_conn_info[iter].temp_prepare_write_value_len = 0;
    memset(rsi_ble_conn_info[iter].temp_prepare_write_value,
           0,
           sizeof(rsi_ble_conn_info[iter].temp_prepare_write_value));
    rsi_ble_conn_info[iter].prep_write_err = 0;
  }
}

/*=======================================================================*/
//! Category 2: Connection Buffer Configuration
/*=======================================================================*/

int8_t rsi_ble_gatt_server_initialize_conn_config(rsi_ble_conn_config_t *ble_conn_spec_conf)
{
  if (ble_conn_spec_conf == NULL) {
    return RSI_FAILURE;
  }

  if (RSI_BLE_MAX_NBR_PERIPHERALS > 0) {
    ble_conn_spec_conf[PERIPHERAL1].tx_notifications = TX_NOTIFICATIONS_TO_P1;
    ble_conn_spec_conf[PERIPHERAL1].tx_indications   = TX_INDICATIONS_TO_P1;
  }
  if (RSI_BLE_MAX_NBR_PERIPHERALS > 1) {
    ble_conn_spec_conf[PERIPHERAL2].tx_notifications = TX_NOTIFICATIONS_TO_P2;
    ble_conn_spec_conf[PERIPHERAL2].tx_indications   = TX_INDICATIONS_TO_P2;
  }
  if (RSI_BLE_MAX_NBR_PERIPHERALS > 2) {
    ble_conn_spec_conf[PERIPHERAL3].tx_notifications = TX_NOTIFICATIONS_TO_P3;
    ble_conn_spec_conf[PERIPHERAL3].tx_indications   = TX_INDICATIONS_TO_P3;
  }
  if (RSI_BLE_MAX_NBR_PERIPHERALS > 3) {
    ble_conn_spec_conf[PERIPHERAL4].tx_notifications = TX_NOTIFICATIONS_TO_P4;
    ble_conn_spec_conf[PERIPHERAL4].tx_indications   = TX_INDICATIONS_TO_P4;
  }
  if (RSI_BLE_MAX_NBR_PERIPHERALS > 4) {
    ble_conn_spec_conf[PERIPHERAL5].tx_notifications = TX_NOTIFICATIONS_TO_P5;
    ble_conn_spec_conf[PERIPHERAL5].tx_indications   = TX_INDICATIONS_TO_P5;
  }
  if (RSI_BLE_MAX_NBR_PERIPHERALS > 5) {
    ble_conn_spec_conf[PERIPHERAL6].tx_notifications = TX_NOTIFICATIONS_TO_P6;
    ble_conn_spec_conf[PERIPHERAL6].tx_indications   = TX_INDICATIONS_TO_P6;
  }
  if (RSI_BLE_MAX_NBR_PERIPHERALS > 6) {
    ble_conn_spec_conf[PERIPHERAL7].tx_notifications = TX_NOTIFICATIONS_TO_P7;
    ble_conn_spec_conf[PERIPHERAL7].tx_indications   = TX_INDICATIONS_TO_P7;
  }
  if (RSI_BLE_MAX_NBR_PERIPHERALS > 7) {
    ble_conn_spec_conf[PERIPHERAL8].tx_notifications = TX_NOTIFICATIONS_TO_P8;
    ble_conn_spec_conf[PERIPHERAL8].tx_indications   = TX_INDICATIONS_TO_P8;
  }

  if (RSI_BLE_MAX_NBR_CENTRALS > 0) {
    ble_conn_spec_conf[CENTRAL1].tx_notifications = TX_NOTIFICATIONS_TO_C1;
    ble_conn_spec_conf[CENTRAL1].tx_indications   = TX_INDICATIONS_TO_C1;
  }
  if (RSI_BLE_MAX_NBR_CENTRALS > 1) {
    ble_conn_spec_conf[CENTRAL2].tx_notifications = TX_NOTIFICATIONS_TO_C2;
    ble_conn_spec_conf[CENTRAL2].tx_indications   = TX_INDICATIONS_TO_C2;
  }

  return RSI_SUCCESS;
}

/*=======================================================================*/
//! Category 3: Attribute List Management & Service Registration
/*=======================================================================*/

rsi_ble_att_list_t *rsi_gatt_get_attribute_from_list(rsi_ble_t *p_val, uint16_t handle)
{
  uint16_t i;
  for (i = 0; i < p_val->att_rec_list_count; i++) {
    if (p_val->att_rec_list[i].handle == handle) {
      return &(p_val->att_rec_list[i]);
    }
  }
  return NULL;
}

void rsi_gatt_add_attribute_to_list(rsi_ble_t *p_val,
                                    uint16_t handle,
                                    uint16_t data_len,
                                    uint8_t *data,
                                    uuid_t uuid,
                                    uint8_t char_prop)
{
  if ((p_val->DATA_ix + data_len) >= BLE_ATT_REC_SIZE) {
    printf("\r\n no data memory for att rec values \r\n");
    return;
  }

  p_val->att_rec_list[p_val->att_rec_list_count].char_uuid     = uuid;
  p_val->att_rec_list[p_val->att_rec_list_count].handle        = handle;
  p_val->att_rec_list[p_val->att_rec_list_count].value_len     = data_len;
  p_val->att_rec_list[p_val->att_rec_list_count].max_value_len = data_len;
  p_val->att_rec_list[p_val->att_rec_list_count].char_val_prop = char_prop;
  memcpy(p_val->DATA + p_val->DATA_ix, data, data_len);
  p_val->att_rec_list[p_val->att_rec_list_count].value = p_val->DATA + p_val->DATA_ix;
  p_val->att_rec_list_count++;
  p_val->DATA_ix += data_len;
}

static void rsi_ble_add_char_serv_att(void *serv_handler,
                                      uint16_t handle,
                                      uint8_t val_prop,
                                      uint16_t att_val_handle,
                                      uuid_t att_val_uuid,
                                      uint16_t auth_read)
{
  rsi_ble_req_add_att_t new_att = { 0 };

  new_att.serv_handler       = serv_handler;
  new_att.handle             = handle;
  new_att.att_uuid.size      = 2;
  new_att.att_uuid.val.val16 = RSI_BLE_CHAR_SERV_UUID;
  new_att.property           = RSI_BLE_ATT_PROPERTY_READ;
  new_att.config_bitmap      = auth_read;

  new_att.data_len = att_val_uuid.size + 4;
  new_att.data[0]  = val_prop;
  rsi_uint16_to_2bytes(&new_att.data[2], att_val_handle);
  if (new_att.data_len == 6) {
    rsi_uint16_to_2bytes(&new_att.data[4], att_val_uuid.val.val16);
  } else if (new_att.data_len == 8) {
    rsi_uint32_to_4bytes(&new_att.data[4], att_val_uuid.val.val32);
  } else if (new_att.data_len == 20) {
    memcpy(&new_att.data[4], &att_val_uuid.val.val128, att_val_uuid.size);
  }
  rsi_ble_add_attribute(&new_att);
}

static void rsi_ble_add_char_val_att(void *serv_handler,
                                     uint16_t handle,
                                     uuid_t att_type_uuid,
                                     uint8_t val_prop,
                                     uint8_t *data,
                                     uint8_t data_len,
                                     uint8_t auth_read)
{
  rsi_ble_req_add_att_t new_att = { 0 };

  new_att.serv_handler  = serv_handler;
  new_att.handle        = handle;
  new_att.config_bitmap = auth_read;
  memcpy(&new_att.att_uuid, &att_type_uuid, sizeof(uuid_t));
  new_att.property = val_prop;

  if (data != NULL)
    memcpy(new_att.data, data, RSI_MIN(sizeof(new_att.data), data_len));

  new_att.data_len = data_len;
  rsi_ble_add_attribute(&new_att);

  if ((auth_read == ATT_REC_MAINTAIN_IN_HOST) || (data_len > 20)) {
    if (data != NULL) {
      rsi_gatt_add_attribute_to_list(&att_list, handle, data_len, data, att_type_uuid, val_prop);
    }
  }

  if ((val_prop & RSI_BLE_ATT_PROPERTY_NOTIFY) || (val_prop & RSI_BLE_ATT_PROPERTY_INDICATE)) {
    memset(&new_att, 0, sizeof(rsi_ble_req_add_att_t));
    new_att.serv_handler       = serv_handler;
    new_att.handle             = handle + 1;
    new_att.att_uuid.size      = 2;
    new_att.att_uuid.val.val16 = RSI_BLE_CLIENT_CHAR_UUID;
    new_att.property           = RSI_BLE_ATT_PROPERTY_READ | RSI_BLE_ATT_PROPERTY_WRITE;
    new_att.data_len           = 2;
    rsi_ble_add_attribute(&new_att);
  }
}

uint32_t rsi_ble_add_simple_chat_serv(void)
{
  uuid_t new_uuid                       = { 0 };
  rsi_ble_resp_add_serv_t new_serv_resp = { 0 };
  uint8_t data[230]                     = { 1, 0 };

  new_uuid.size      = 2;
  new_uuid.val.val16 = RSI_BLE_NEW_SERVICE_UUID;
  rsi_ble_add_service(new_uuid, &new_serv_resp);

  new_uuid.size      = 2;
  new_uuid.val.val16 = RSI_BLE_ATTRIBUTE_1_UUID;
  rsi_ble_add_char_serv_att(new_serv_resp.serv_handler,
                            new_serv_resp.start_handle + 1,
                            RSI_BLE_ATT_PROPERTY_READ | RSI_BLE_ATT_PROPERTY_NOTIFY | RSI_BLE_ATT_PROPERTY_WRITE,
                            new_serv_resp.start_handle + 2,
                            new_uuid,
                            0);

  rsi_ble_att1_val_hndl = new_serv_resp.start_handle + 2;
  new_uuid.size         = 2;
  new_uuid.val.val16    = RSI_BLE_ATTRIBUTE_1_UUID;
  rsi_ble_add_char_val_att(new_serv_resp.serv_handler,
                           new_serv_resp.start_handle + 2,
                           new_uuid,
                           RSI_BLE_ATT_PROPERTY_READ | RSI_BLE_ATT_PROPERTY_NOTIFY | RSI_BLE_ATT_PROPERTY_WRITE,
                           data,
                           sizeof(data),
                           1);

  return 0;
}

uint32_t rsi_ble_add_simple_chat_serv2(void)
{
  uint8_t data1[231]                 = { 1, 0 };
  static const uuid_t custom_service = { .size             = 16,
                                         .reserved         = { 0x00, 0x00, 0x00 },
                                         .val.val128.data1 = 0x6A4E3300,
                                         .val.val128.data2 = 0x667B,
                                         .val.val128.data3 = 0x11E3,
                                         .val.val128.data4 = { 0x9A, 0x94, 0x00, 0x08, 0x66, 0x9A, 0x0C, 0x20 } };

  static const uuid_t custom_characteristic = {
    .size             = 16,
    .reserved         = { 0x00, 0x00, 0x00 },
    .val.val128.data1 = 0x6A4E3304,
    .val.val128.data2 = 0x667B,
    .val.val128.data3 = 0x11E3,
    .val.val128.data4 = { 0x9A, 0x94, 0x00, 0x08, 0x66, 0x9A, 0x0C, 0x20 }
  };

  rsi_ble_resp_add_serv_t new_serv_resp = { 0 };
  rsi_ble_add_service(custom_service, &new_serv_resp);

  rsi_ble_add_char_serv_att(new_serv_resp.serv_handler,
                            new_serv_resp.start_handle + 1,
                            RSI_BLE_ATT_PROPERTY_WRITE_NO_RESPONSE,
                            new_serv_resp.start_handle + 2,
                            custom_characteristic,
                            0);

  rsi_ble_att2_val_hndl = new_serv_resp.start_handle + 2;
  rsi_ble_add_char_val_att(new_serv_resp.serv_handler,
                           new_serv_resp.start_handle + 2,
                           custom_characteristic,
                           RSI_BLE_ATT_PROPERTY_WRITE_NO_RESPONSE,
                           data1,
                           sizeof(data1),
                           1);
  return 0;
}

uint32_t rsi_ble_add_custom_service_serv(void)
{
  uuid_t new_uuid                       = { 0 };
  rsi_ble_resp_add_serv_t new_serv_resp = { 0 };
  uint8_t data[1]                       = { 90 };
  rsi_ble_pesentation_format_t presentation_format;
  uint8_t format_data[7];

  format_data[0] = presentation_format.format = RSI_BLE_UINT8_FORMAT;
  format_data[1] = presentation_format.exponent = RSI_BLE_EXPONENT;
  presentation_format.unit                      = RSI_BLE_PERCENTAGE_UNITS_UUID;
  memcpy(&format_data[2], &presentation_format.unit, sizeof(presentation_format.unit));
  format_data[4] = presentation_format.name_space = RSI_BLE_NAME_SPACE;
  presentation_format.description                 = RSI_BLE_DESCRIPTION;
  memcpy(&format_data[5], &presentation_format.description, sizeof(presentation_format.description));

  new_uuid.size      = 2;
  new_uuid.val.val16 = RSI_BLE_CUSTOM_SERVICE_UUID;
  rsi_ble_add_service(new_uuid, &new_serv_resp);

  new_uuid.size      = 2;
  new_uuid.val.val16 = RSI_BLE_CUSTOM_LEVEL_UUID;
  rsi_ble_add_char_serv_att(new_serv_resp.serv_handler,
                            new_serv_resp.start_handle + 1,
                            RSI_BLE_ATT_PROPERTY_READ | RSI_BLE_ATT_PROPERTY_INDICATE,
                            new_serv_resp.start_handle + 2,
                            new_uuid,
                            SEC_MODE_1_LEVEL_1);

  rsi_ble_att3_val_hndl = new_serv_resp.start_handle + 2;
  new_uuid.size         = 2;
  new_uuid.val.val16    = RSI_BLE_CUSTOM_LEVEL_UUID;
  rsi_ble_add_char_val_att(new_serv_resp.serv_handler,
                           new_serv_resp.start_handle + 2,
                           new_uuid,
                           RSI_BLE_ATT_PROPERTY_READ | RSI_BLE_ATT_PROPERTY_INDICATE,
                           data,
                           sizeof(data),
                           1);

  new_uuid.size      = 2;
  new_uuid.val.val16 = RSI_BLE_CHAR_PRESENTATION_FORMATE_UUID;
  rsi_ble_add_char_val_att(new_serv_resp.serv_handler,
                           new_serv_resp.start_handle + 4,
                           new_uuid,
                           RSI_BLE_ATT_PROPERTY_READ,
                           format_data,
                           sizeof(format_data),
                           1);

  return 0;
}
