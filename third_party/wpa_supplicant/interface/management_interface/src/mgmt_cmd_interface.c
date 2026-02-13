/*******************************************************************************
* @file  mgmt_cmd_interface.c
* @brief This file acts as interface between command interface and
*        management interface
*******************************************************************************
* # License
* <b>Copyright 2020-2024 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "mgmt_if_header.h"
#include "mgmt_platform_if.h"
#include "sli_wlan_command_interface.h"
#include "wlan_user_command_utils.h"
#include "mgmt_if_core.h"
#include "mgmt_if_tx.h"
#include "mgmt_if_rsi_wsc.h"
#include "sli_nhcp_pkt_struct.h"
#include "sli_nhcp_driver_thread.h"

/*********** Command internal function declarations ***********/
static uint16 parse_gain_table_command(WLAN_USR_GAIN_TABLE_Req *gain_table_info);
static uint16 parse_eap_cfg_command(WLAN_SET_EAP_CFG_Req *eap_config, uint16 pkt_length);
static int32 transform_apconf_txpkt(uint8 *txPkt);
static int32 validate_command_input_params(uint16 command_id, uint8 *scatter_buf, mgmt_command_status_t *status);

/*********** Command internal function definitions ***********/

static uint16 parse_eap_cfg_command(WLAN_SET_EAP_CFG_Req *eap_config, uint16 pkt_length)
{
  if ((mgmt_if_adapter.feature_bit_map & FEAT_EAP_LEAP) && (strcmp((const char *)eap_config->eapMethod, "LEAP"))) {
    return NOT_SUPPORTED;
  }

#ifdef RSI_GLOBAL_VARIABLES
  set_okc(*(int32 *)eap_config->okc);
  sc_save_eap_method(eap_config->eapMethod);
#endif

  mgmt_if_adapter.okc = *(int32 *)eap_config->okc;
  sl_strcpy(mgmt_if_adapter.eap_method, eap_config->eapMethod);
  sl_strcpy(mgmt_if_adapter.inner_method, eap_config->innerMethod);
  sl_strcpy(mgmt_if_adapter.user_identity, eap_config->UserIdentity);
  sl_strcpy(mgmt_if_adapter.passwd, eap_config->password);

  if ((pkt_length > 260)
      && (sl_strlen((char *)eap_config->private_key_passwd) <= sizeof(mgmt_if_adapter.private_key_passwd))) {
#ifdef RSI_GLOBAL_VARIABLES
    sc_save_private_key_passwd(eap_config->private_key_passwd);
#endif /* RSI_GLOBAL_VARIABLES */
    sl_strcpy(mgmt_if_adapter.private_key_passwd, eap_config->private_key_passwd);
  } else {
#ifdef RSI_GLOBAL_VARIABLES
    clear_private_key_passwd();
#endif /* RSI_GLOBAL_VARIABLES */
    sl_memzero(mgmt_if_adapter.private_key_passwd, sizeof(mgmt_if_adapter.private_key_passwd));
  }

#ifdef FIPS_CODE_ENABLE
  mgmt_if_adapter.fips_key_status_flag = PASSWORD_ENTERED;
#endif

  /* wsc supplicant initialization */
  if (mgmt_if_adapter.supplicant_priv == NULL) {
    wsc_supp_start();
  }

#ifdef FIPS_CODE_ENABLE
  //! If EAP password is NULL
  if (sl_strlen((const char *)rsi_wsc_cb.sc_params.passwd) == 0) {
    //! Get flash configuration pointer
    tmp_sc = (struct sc_params_s *)CONFIG_SAVE_FLASH_LOCATION;

    if ((tmp_sc->FIPS_key_restore == FIPS_KEY_STORE_MAGIC_NO) && (tmp_sc->FIPS_key_type_stored & FIPS_KEY_EAP)) {
      //! Copy EAP password from flash
      sl_strcpy(rsi_wsc_cb.sc_params.passwd, tmp_sc->passwd);

      //! Set flag to PASSWORD_VERIFIED since re-check is not needed
      mgmt_if_adapter.fips_key_status_flag = PASSWORD_VERIFIED;
    } else {
      status->command = SLI_WLAN_RSP_HOST_PSK;
      status->status  = FIPS_KEY_NOT_STORED;
      break;
    }
  }
#endif

  wise_update_state(WISE_STATE_EAP_CFG_DONE);

  return WLAN_STATUS_SUCCESS;
}

static uint16 parse_gain_table_command(WLAN_USR_GAIN_TABLE_Req *gain_table_info)
{
  uint32 offset = 0;
  uint8 ii      = 0;
  uint16 first_channel;
  uint32 number_of_regions, region_code, number_of_channels;

  if ((gain_table_info->size <= RSI_MIN_GAIN_TABLE_PAYLOAD_LEN)
      || (gain_table_info->size > RSI_MAX_GAIN_TABLE_PAYLOAD_LEN)) {
    return ERROR_IN_LENGTH_OF_THE_COMMAND;
  }

  number_of_regions = gain_table_info->gain_table[offset++];

  for (ii = 0; ii < number_of_regions; ii++) {
    region_code = gain_table_info->gain_table[offset++];

    if ((region_code != REGION_FCC) && (region_code != REGION_ETSI) && (region_code != REGION_TELEC)
        && (region_code != REGION_WORLD_WIDE) && (region_code != REGION_KCC) && (region_code != REGION_SRRC)) {
      return REGION_CODE_NOT_SUPPORTED;
    }

    number_of_channels = (gain_table_info->gain_table[offset++] & 0xf);
    first_channel      = gain_table_info->gain_table[offset];

    if (gain_table_info->band == RSI_GAIN_TABLE_BAND_2P4_GHZ) {
      //! 5 --> (channel_no, b,g,n,ax)
      //! 4 --> (channel_no, b,g,n)
      if (first_channel == 255) {
        offset += 5;
      } else {
        offset += 5 * number_of_channels;
      }
    } else if (gain_table_info->band == RSI_GAIN_TABLE_BAND_5GHZ) {
      if (first_channel == 255) {
        offset += 4;
      } else {
        offset += 4 * number_of_channels;
      }
    } else {
      return INVALID_BAND_GIVEN;
    }
  }

  if (offset != gain_table_info->size) {
    return ERROR_IN_LENGTH_OF_THE_COMMAND; //! check_for_user given and payload_len mismatch
  }

  return WLAN_STATUS_SUCCESS;
}

static int32 transform_apconf_txpkt(uint8 *txPkt)
{
  uint8 *scatter_buf = NULL;

  if (SL_PKT_GET_TXPKT_HDESC_ADDR(txPkt)) {
    scatter_buf =
      (((uint8 *)SL_PKT_GET_TXPKT_HDESC_ADDR(txPkt)) + HOST_DESC_LENGTH + SL_PKT_TX_HDESC_GET_DW1_EXT_DESC_SIZE(txPkt));
  }

  ap_conf_ta_t at;
  apconfig_st_t *ap_conf = (apconfig_st_t *)scatter_buf;

  /* K60 apconfig structure format is different from TA ap config structure that is why all this stuff :) */
  at.beacon_interval    = ap_conf->beacon_interval;
  at.dtim_period        = ap_conf->dtim_period;
  at.channel            = ap_conf->channel_no;
  at.max_no_sta         = ap_conf->max_no_sta;
  at.encryp_mode        = ap_conf->encryp_mode;
  at.keep_alive_type    = ap_conf->keep_alive_type;
  at.keep_alive_counter = ap_conf->keep_alive_counter;

  sl_memcpy(scatter_buf, &at, sizeof(ap_conf_ta_t));
  SL_PKT_SET_TXPKT_SCATTER_BUFF_LEN(txPkt, 0, sizeof(ap_conf_ta_t) + HOST_DESC_LENGTH);
  SL_PKT_TX_HDESC_SET_DW0_LENGTH(txPkt, (0xFFF) & (SL_PKT_GET_TXPKT_SCATTER_BUFF_LEN(txPkt, 0) - HOST_DESC_LENGTH));

  return 0;
}

/**
 * @brief Validates the current state of the system for the given command and operating mode.
 *
 * This function checks whether the system's current state allows the execution 
 * of a specified command. It evaluates conditions specific to each command 
 * and returns an error if the state is not valid. If the state is invalid, an 
 * error indication is sent to the host.
 *
 * @param[in] cmd            The command to be validated.
 * @param[in] operating_mode The current operating mode of the system.
 * 
 * @return 
 *         - 0 if the command is valid in the current state.
 *         - 1 if the command is invalid in the current state or any other error occurs.
 *
 * @note If the system is in the process of rejoining, certain commands are restricted.
 */

static int16 check_state(uint16 cmd, uint8 operating_mode)
{
  int16 ret_val    = 0;
  int16 curr_state = mgmt_if_adapter.state;

  /* TODO: Need to add commands here while adding new commands */
  if (CHECK_REJOIN_STATE_AND_CMD(cmd)) {
    return 1;
  }

  switch (cmd) {
    case SLI_WLAN_REQ_P2P_ENABLE: {
      ret_val = !(curr_state < WISE_STATE_OPERMODE_SET);
    } break;

    case SLI_WLAN_REQ_BAND: {
      ret_val = !((curr_state > WISE_STATE_INIT) && (curr_state < WISE_STATE_BAND_DONE));
    } break;

    case SLI_WLAN_REQ_INIT: {
      ret_val = !((curr_state < WISE_STATE_INIT_DONE) && (curr_state == WISE_STATE_BAND_DONE));
    } break;

    case SLI_WLAN_REQ_HOST_PSK: {
      ret_val = (curr_state >= WISE_STATE_CONNECTED);
    } break;

    case SLI_WLAN_REQ_TIMEOUTS: {
      ret_val = (curr_state == WISE_STATE_INIT);
    } break;

    case SLI_WLAN_REQ_CONFIG: {
      ret_val = !(curr_state >= WISE_STATE_INIT);
    } break;

    case SLI_WLAN_REQ_SET_REGION: {
      ret_val = !(operating_mode != WISE_MODE_AP && curr_state == WISE_STATE_INIT_DONE);
    } break;

    case SLI_WLAN_REQ_SET_FEATURE: {
      ret_val = ((curr_state > WISE_STATE_BAND_DONE) || (curr_state < WISE_STATE_OPERMODE_SET));
    } break;

    case SLI_WLAN_REQ_SCAN: {
      switch (curr_state) {
        case WISE_STATE_INIT_DONE: {
          ret_val = !((operating_mode == WISE_MODE_CONNECTION) || (operating_mode == WISE_MODE_EAP));
        } break;

        case WISE_STATE_EAP_CFG_DONE: {
          ret_val = (operating_mode != WISE_MODE_EAP);
        } break;

        case WISE_STATE_SCAN_DONE: {
          ret_val = (operating_mode == WISE_MODE_P2P);
        } break;

        case WISE_STATE_CONNECTED: {
          ret_val = 0;
        } break;

        default: {
          ret_val = 1;
        } break;
      }
    } break;

    case SLI_WLAN_REQ_JOIN: {
      switch (curr_state) {
        case WISE_STATE_INIT_DONE: {
          ret_val = (operating_mode != WISE_MODE_CONNECTION);
        } break;

        case WISE_STATE_P2P_CFG_DONE: {
          ret_val = (operating_mode != WISE_MODE_P2P);
        } break;

        case WISE_STATE_EAP_CFG_DONE: {
          ret_val = (operating_mode != WISE_MODE_EAP);
        } break;

        case WISE_STATE_SCAN_DONE: {
          ret_val = (operating_mode == WISE_MODE_P2P);
        } break;

        case WISE_STATE_CONNECTED: {
          ret_val = 1;
          if ((operating_mode == WISE_MODE_P2P) || (operating_mode == WISE_MODE_AP)) {
            if (!mgmt_if_adapter.client && mgmt_if_adapter.connected) {
              ret_val = 0;
            }
          }
        } break;

        case WISE_STATE_APCONF_DONE: {
          ret_val = (operating_mode != WISE_MODE_AP);
        } break;

        default: {
          ret_val = 1;
        } break;
      }
    } break;

    case SLI_WLAN_BGSCAN: {
      ret_val = mgmt_if_adapter.state < WISE_STATE_CONNECTED;
    } break;

    case SLI_WLAN_REQ_ROAM_PARAMS: {
      //! Allow only after init is given
      ret_val = (mgmt_if_adapter.state < WISE_STATE_INIT_DONE);
    } break;

    case SLI_MGMT_REQ_DISCONNECT: {
      if (!mgmt_if_adapter.rejoin_going_on) {
        if ((operating_mode == WISE_CONCURRENT_MODE)
            && (!(mgmt_if_adapter.concurrent_mode_state < WISE_CON_STATE_CONNECTED)
                || !(mgmt_if_adapter.state < WISE_STATE_CONNECTED))) {
          ret_val = 0;
        } else {
          ret_val = (mgmt_if_adapter.state < WISE_STATE_CONNECTED);
        }
      }
    } break;

    case SLI_WLAN_REQ_REJOIN_PARAMS: {
      ret_val = (curr_state < WISE_STATE_INIT_DONE);
    } break;

    case SLI_WLAN_REQ_SNR:
      break;

    case SLI_WLAN_REQ_RSSI: {
      ret_val = (mgmt_if_adapter.state < WISE_STATE_CONNECTED);
    } break;

    case SLI_WLAN_REQ_PER_PARAMS: {
      ret_val = ((curr_state < WISE_STATE_INIT_DONE) || (operating_mode != WISE_MODE_PER));
    } break;

    case SLI_WLAN_REQ_WLAN_EXT_STATS:
    case SLI_WLAN_REQ_GET_WLAN_STATS:
    // The above cases ment to fall through
    case SLI_WLAN_REQ_PER_STATS: {
      // This command is allowed after Opermode is set.
      ret_val = (curr_state == WISE_STATE_INIT);
    } break;

    case SLI_WLAN_REQ_GET_MAC_ADDR: {
      ret_val = (curr_state < WISE_STATE_INIT_DONE);
    } break;

    case SLI_WLAN_REQ_SET_MAC: {
      ret_val = !((curr_state == WISE_STATE_OPERMODE_SET) || (curr_state == WISE_STATE_MAC_DONE));
    } break;

    case SLI_WLAN_REQ_MULTICAST_FILTER: {
      ret_val = (curr_state < WISE_STATE_INIT);
    } break;

    case SLI_WLAN_REQ_FILTER_BROADCAST: {
      ret_val = (curr_state < WISE_STATE_OPERMODE_SET);
    } break;

    default: {
      ret_val = 1;
    } break;
  }

  if (ret_val) {
    sl_mgmt_indicate_to_host(cmd, 0, WISE_ERROR_WRONG_STATE, NULL);
  }
  return ret_val;
}

static int32 validate_command_input_params(uint16 command_id, uint8 *scatter_buf, mgmt_command_status_t *status)
{
  switch (command_id) {

    case SLI_WLAN_REQ_P2P_ENABLE: {
      P2PmodeFrameSnd *op = (P2PmodeFrameSnd *)scatter_buf;
      if (!((op->modeVal == HOST_OP_CLIENT_MODE) || (op->modeVal == HOST_OP_P2P_MODE)
            || (op->modeVal == HOST_OP_ENTERPRISE_CLIENT_MODE) || (op->modeVal == HOST_OP_ACCESS_POINT_MODE)
            || (op->modeVal == HOST_OP_BTR_MODE) || (op->modeVal == HOST_OP_PER_MODE)
            || (op->modeVal == HOST_OP_CONCURRENT_MODE))) {
        status->command = SLI_WLAN_RSP_P2P_ENABLE;
        status->status  = INVALID_OPERATING_MODE;
        break;
      }

#ifdef EXCLUDE_AP
      /*Raise the feature not supported error if operating mode is set to AP or concurrent mode in sta_alone image*/
      if (op->modeVal == HOST_OP_ACCESS_POINT_MODE || op->modeVal == HOST_OP_CONCURRENT_MODE) {
        status->command = SLI_WLAN_RSP_P2P_ENABLE;
        status->status  = NOT_SUPPORTED;
        SL_PRINTF(WLAN_CMD_PARSE_P2P_ENABLE_FEATURE_NOT_SUPPORTED, WLAN_UMAC, LOG_ERROR);
        break;
      }
#endif

#ifdef EXCLUDE_EAP
      /*Raise the feature not supported error if operating mode is set to EAP in sta_alone image*/
      if (op->modeVal == HOST_OP_ENTERPRISE_CLIENT_MODE) {
        status->command = SLI_WLAN_RSP_P2P_ENABLE;
        status->status  = NOT_SUPPORTED;
        SL_PRINTF(WLAN_CMD_PARSE_P2P_ENABLE_FEATURE_NOT_SUPPORTED, WLAN_UMAC, LOG_ERROR);
        break;
      }
#endif

#ifdef REGION_CONFIG_DISABLE_FOR_ACX
#ifdef CONFIG_IEEE_80211J
      if ((is_hw_acx_module() && op->ext_custom_feature_bit_map & EXT_FEAT_IEEE_80211J)) {
        status->command = SLI_WLAN_RSP_P2P_ENABLE;
        status->status  = NOT_SUPPORTED;
        SL_PRINTF(WLAN_CMD_PARSE_P2P_ENABLE_FEATURE_NOT_SUPPORTED, WLAN_UMAC, LOG_ERROR);
        break;
      }
#endif
#endif /* REGION_CONFIG_DISABLE_FOR_ACX */

      //! 40MHz bandwidth is not supported in STA and AP mode
      if ((op->custom_feature_bit_map & (FEAT_CUSTOM_FEAT_EXTENTION_VALID))
          && (op->ext_custom_feature_bit_map & (EXT_FEAT_40MHZ_SUPPORT_ENABLE))) {
        status->command = SLI_WLAN_RSP_P2P_ENABLE;
        status->status  = FREQUENCY_NOT_SUPPORTED;
        SL_PRINTF(WLAN_CMD_PARSE_FREQUENCY_NOT_SUPPORTED, WLAN_UMAC, LOG_ERROR);
        break;
      }

      if (op->modeVal == HOST_OP_ENTERPRISE_CLIENT_MODE) {
        //! If mode is EAP,then ignore OPEN/PSK bits
        op->feature_bit_map &= ~(FEAT_SECURITY_OPEN | FEAT_SECURITY_PSK);
      }
    } break;

    case SLI_WLAN_REQ_APCONF: {
      apconfig_st_t *ap_conf = (apconfig_st_t *)scatter_buf;

      if (sl_strlen((char *)ap_conf->ssid) > 32) {
        status->command = SLI_WLAN_RSP_APCONF;
        status->status  = WISE_ERROR_WRONG_PARAMETER;
        SL_PRINTF(WLAN_CMD_PARSE_SSID_TOO_LONG, WLAN_UMAC, LOG_ERROR);
        return 1;
      }

      if ((ap_conf->keep_alive_type & BIT(0)) && (!ap_conf->keep_alive_counter)) {
        status->command = SLI_WLAN_RSP_APCONF;
        status->status  = WISE_ERROR_WRONG_PARAMETER;
        SL_PRINTF(WLAN_CMD_PARSE_KEEP_ALIVE_WRONG_PARAMETER, WLAN_UMAC, LOG_ERROR);
        return 1;
      }

      if (ap_conf->max_no_sta > mgmt_if_adapter.max_station_supported) {
        status->command = SLI_WLAN_RSP_APCONF;
        status->status  = GIVEN_STATIONS_NOT_SUPPORTED;
        SL_PRINTF(WLAN_CMD_PARSE_GIVEN_STATIONS_NOT_SUPPORTED, WLAN_UMAC, LOG_ERROR);
        return 1;
      }

      if ((ap_conf->beacon_interval < 100) || (ap_conf->beacon_interval > 1000) || (ap_conf->beacon_interval % 100)) {
        status->command = SLI_WLAN_RSP_APCONF;
        status->status  = INVALID_BEACON_INTERVAL;
        SL_PRINTF(WLAN_CMD_PARSE_INVALID_BEACON_INTERVAL, WLAN_UMAC, LOG_ERROR);
        return 1;
      }

      if ((ap_conf->dtim_period < 1) || (ap_conf->dtim_period > 255)) {
        status->command = SLI_WLAN_RSP_APCONF;
        status->status  = INVALID_BEACON_INTERVAL;
        SL_PRINTF(WLAN_CMD_PARSE_INVALID_DTIM_PERIOD, WLAN_UMAC, LOG_ERROR);
        return 1;
      }

      if ((ap_conf->security_type == SEC_MODE_WPA_ENTERPRISE) || (ap_conf->security_type == SEC_MODE_WPA2_ENTERPRISE)) {
        status->command = SLI_WLAN_RSP_APCONF;
        status->status  = WISE_ERROR_WRONG_PARAMETER;
        return 1;
      }

      if (ap_conf->security_type && ((sl_strlen((char *)ap_conf->psk) < 8) || (sl_strlen((char *)ap_conf->psk) > 63))) {
        status->command = SLI_WLAN_RSP_APCONF;
        status->status  = WISE_ERROR_WRONG_LEN_PSK;
        SL_PRINTF(WLAN_CMD_PARSE_PSK_TOO_LONG, WLAN_UMAC, LOG_ERROR);
        return 1;
      }

      if ((ap_conf->encryp_mode & 0xF) > 2) {
        status->command = SLI_WLAN_RSP_APCONF;
        status->status  = WISE_ERROR_WRONG_PARAMETER;
        SL_PRINTF(WLAN_CMD_PARSE_INVALID_ENCRYPTION_MODE, WLAN_UMAC, LOG_ERROR);
        return 1;
      }

#ifndef ENABLE_ACS
      if ((mgmt_if_adapter.operating_mode != WISE_CONCURRENT_MODE)
          && (get_frequency(mgmt_if_adapter.current_freq_band,
                            ap_conf->channel_no,
                            ((rsi_wsc_cb.sc_params.ext_custom_feature_bit_map & EXT_FEAT_IEEE_80211J)
                             && (rsi_wsc_cb.sc_params.region_code == REGION_JAP_NUM)))
              == 0)) {
        status->command = SLI_WLAN_RSP_APCONF;
        status->status  = FREQUENCY_NOT_SUPPORTED;
        SL_PRINTF(WLAN_CMD_PARSE_FREQUENCY_NOT_SUPPORTED, WLAN_UMAC, LOG_ERROR);
        return 1;
      }
#endif

      /* If host gives '0' as Max no of stations, configure it to 4. */
      if (!ap_conf->max_no_sta) {
        ap_conf->max_no_sta = 4;
      }

#ifdef CONFIG_IEEE_80211J
      if ((mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_IEEE_80211J) && !mgmt_if_adapter.set_region_given) {
        status->command = SLI_WLAN_RSP_SCAN;
        status->status  = SET_REGION_NOT_GIVEN_IN_11J;
        SL_PRINTF(WLAN_CMD_PARSE_SET_REGION_NOT_GIVEN_IN_11J, WLAN_UMAC, LOG_ERROR);
        return 1;
      }
#endif
    } break;

    case SLI_WLAN_REQ_SET_REGION: {
      rsi_setregion_t *ptr_setregion = (rsi_setregion_t *)scatter_buf;
      if ((ptr_setregion->setregion_code_from_user_cmd > 1) || (ptr_setregion->region_code > 6)
          || ((!ptr_setregion->setregion_code_from_user_cmd) && (mgmt_if_adapter.operating_mode == WISE_MODE_P2P))) {
        status->command = SLI_WLAN_RSP_SET_REGION;
        status->status  = NOT_SUPPORTED;
        return 1;
      }
      break;
    }

#ifdef ENABLE_11AX
    case SLI_WLAN_REQ_AX_PARAMS: {
      if (mgmt_if_adapter.feature_bit_map & FEAT_DISABLE_11AX) {
        status->command = SLI_WLAN_REQ_AX_PARAMS;
        status->status  = NOT_SUPPORTED;
        SL_PRINTF(WLAN_CMD_PARSE_11AX_MODE_NOT_SUPPORTED_FEATURE_DISABLED, WLAN_UMAC, LOG_ERROR);
        return 1;
      }
    } break;
#endif /* ENABLE_11AX */

    case SLI_WLAN_REQ_JOIN: {
      rsi_uJoin *req_join = (rsi_uJoin *)scatter_buf;

      if ((mgmt_if_adapter.operating_mode == WISE_MODE_AP)
          || ((mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) && (req_join->joinFrameSnd.vap_id == 1))) {
        if ((req_join->joinFrameSnd.securityType == SEC_MODE_WPA_ENTERPRISE)
            || (req_join->joinFrameSnd.securityType == SEC_MODE_WPA2_ENTERPRISE)) {
          status->command = SLI_WLAN_RSP_JOIN;
          status->status  = WISE_ERROR_WRONG_PARAMETER;
          return 1;
        }
      }

#ifdef ENABLE_CONCURRENT_MODE
      if (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) {
        if ((req_join->joinFrameSnd.vap_id == 0) && (mgmt_if_adapter.state >= WISE_STATE_CONNECTED)) {
          status->command = SLI_WLAN_RSP_JOIN;
          status->status  = WISE_ERROR_WRONG_STATE;
          SL_PRINTF(WLAN_CMD_PARSE_CONCURRENT_MODE_INCORRECT_STATE, WLAN_UMAC, LOG_ERROR);
          return 1;
        }
      }
#endif
      /* In WPA3-SAE mode, it is mandatory to enable PMF-required bit*/
      if ((req_join->joinFrameSnd.securityType == SEC_MODE_WPA3)
          && !((req_join->joinFrameSnd.join_feature_bitmap & 0x40)
               && (req_join->joinFrameSnd.join_feature_bitmap & 0x20))) {
        status->command = SLI_WLAN_RSP_JOIN;
        status->status  = WISE_ERROR_WRONG_PARAMETER;
        SL_PRINTF(WLAN_CMD_PARSE_CONCURRENT_MODE_WRONG_PARAMETER, WLAN_UMAC, LOG_ERROR);
        return 1;
      }
      /* In WPA3-transition mode, it is mandatory to enable PMF-capable bit*/
      if ((mgmt_if_adapter.operating_mode == WISE_MODE_AP)
          && (req_join->joinFrameSnd.securityType == SEC_MODE_WPA3_TRANSITION)
          && ((req_join->joinFrameSnd.join_feature_bitmap & 0x40)
              || !(req_join->joinFrameSnd.join_feature_bitmap & 0x20))) {
        status->command = SLI_WLAN_RSP_JOIN;
        status->status  = WISE_ERROR_WRONG_PARAMETER;
        SL_PRINTF(WLAN_CMD_PARSE_CONCURRENT_MODE_WRONG_PARAMETER, WLAN_UMAC, LOG_ERROR);
        return 1;
      } else {
        /* In WPA3-transition mode, it is mandatory to enable PMF-capable bit*/
        if ((req_join->joinFrameSnd.securityType == SEC_MODE_WPA3_TRANSITION)
            && ((req_join->joinFrameSnd.join_feature_bitmap & 0x40)
                || !(req_join->joinFrameSnd.join_feature_bitmap & 0x20))) {
          status->command = SLI_WLAN_RSP_JOIN;
          status->status  = WISE_ERROR_WRONG_PARAMETER;
          SL_PRINTF(WLAN_CMD_PARSE_CONCURRENT_MODE_WRONG_PARAMETER, WLAN_UMAC, LOG_ERROR);
          return 1;
        }
      }

#ifdef FIPS_CODE_ENABLE
      if (rsi_wsc_cb.fips.fips_mode_enable == 1) {
        /*Check whether rekey is completed or not*/
        if ((mgmt_if_adapter.fips_key_status_flag != PASSWORD_VERIFIED)) {
          /*Send error Rekey not done*/
          status->command = SLI_WLAN_RSP_JOIN;
          status->status  = FIPS_REKEY_NOT_DONE;
          SL_PRINTF(WLAN_CMD_PARSE_FIPS_REKEY_NOT_DONE, WLAN_UMAC, LOG_ERROR);
          return 1;
        }
      }
#endif
    } break;

    case SLI_WLAN_REQ_SCAN: {
#ifdef CONFIG_IEEE_80211J
      if ((mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_IEEE_80211J) && !mgmt_if_adapter.set_region_given) {
        status->command = SLI_WLAN_RSP_SCAN;
        status->status  = SET_REGION_NOT_GIVEN_IN_11J;
        SL_PRINTF(WLAN_CMD_PARSE_SET_REGION_NOT_GIVEN_IN_11J, WLAN_UMAC, LOG_ERROR);
        return 1;
      }
#endif /* CONFIG_IEEE_80211J */
    } break;

    default:
      break;
  }
  return 0;
}

/*******************************************************
 * @ Description: This function shall process wlan management related user commands
 * @ Input: uint8 *txPkt
 * @ Called from : Command Interface - connection_cmd_parse()
 * @ Output : Command status : 1/0    0 : The caller frees the packet
 */
void wlan_mgmt_if_cmd_handler(uint8 *txPkt)
{
  uint8 *scatter_buf           = NULL;
  uint8 *scatter_buf1          = NULL;
  uint16 command_id            = 0xFFFF;
  uint32 cmd_status            = MGMT_IF_FREE_CMD_PKT;
  uint16 pkt_length            = 0;
  uint16 scatter_len           = 0;
  uint16 scatter_len1          = 0;
  uint16 state_validity_status = 0;

  /* TODO: array may not be needed for this */
  uint8 connection_status[2]    = { 0 };
  mgmt_command_status_t *status = &(mgmt_command_status_t){ 0 };

  if (SL_PKT_GET_TXPKT_HDESC_ADDR(txPkt)) {
    command_id = SL_PKT_TX_HDESC_GET_DW0_FRAME_TYPE(txPkt);
    scatter_buf =
      (((uint8 *)SL_PKT_GET_TXPKT_HDESC_ADDR(txPkt)) + HOST_DESC_LENGTH + SL_PKT_TX_HDESC_GET_DW1_EXT_DESC_SIZE(txPkt));
    scatter_len = SL_PKT_GET_TXPKT_SCATTER_BUFF_LEN(txPkt, 0) - HOST_DESC_LENGTH;

    /* TODO: Need to check for EAP, as it is used only in EAP */
    scatter_buf1 = (uint8 *)(((sli_nhcp_tx_pkt_t *)(txPkt))->scatter_info[1].addr);
    scatter_len1 = SL_PKT_GET_TXPKT_SCATTER_BUFF_LEN(txPkt, 1);
    pkt_length   = SL_PKT_TX_HDESC_GET_DW0_LENGTH(txPkt);
  }

  sl_memzero(status, sizeof(mgmt_command_status_t));
  switch (command_id) {
    case SLI_WLAN_REQ_DYNAMIC_POOL:
    case SLI_WLAN_REQ_CONFIG:
    case SLI_WLAN_REQ_TIMEOUTS: {
      sl_mgmt_indicate_to_host(command_id, 0, CMD_STATUS_NO_ERROR, NULL);
      goto STATE_FAILURE;
    }
    default:
      break;
  }

  state_validity_status = check_state(command_id, mgmt_if_adapter.operating_mode);
  if (state_validity_status == INVALID_STATE)
    goto STATE_FAILURE;
  validate_command_input_params(command_id, scatter_buf, status);
  if ((status->status) == CMD_STATUS_NO_ERROR) {
    switch (command_id) {
      case SLI_WLAN_REQ_P2P_ENABLE: {
        P2PmodeFrameSnd *op = (P2PmodeFrameSnd *)scatter_buf;
        if (parse_dyn_sta_ap_switch_selection(op)) {
          mgmt_if_adapter.dyn_sta_ap_switch_enable = 1;
        }

        if (op->modeVal == HOST_OP_BTR_MODE) {
          op->modeVal                      = WISE_MODE_CONNECTION;
          mgmt_if_adapter.wlan_btr_mode_en = 1;
        }

        mgmt_if_adapter.client  = SL_TRUE;
        supp_mgmt_if_ctx.is_eap = (op->modeVal == HOST_OP_ENTERPRISE_CLIENT_MODE);

        mgmt_if_adapter.custom_feature_bit_map     = op->custom_feature_bit_map;
        mgmt_if_adapter.ext_custom_feature_bit_map = 0;
        if (op->custom_feature_bit_map & FEAT_CUSTOM_FEAT_EXTENTION_VALID) {
          mgmt_if_adapter.ext_custom_feature_bit_map = op->ext_custom_feature_bit_map;
        }

        mgmt_if_adapter.config_feature_bit_map = 0;
        if ((op->tcp_ip_feature_bit_map & TCP_IP_FEAT_EXTENTION_VALID)
            && (op->ext_tcp_ip_feature_bit_map & CONFIG_FEAT_EXTENTION_VALID)) {
          mgmt_if_adapter.config_feature_bit_map = op->config_feature_bit_map;
        } else {
          op->config_feature_bit_map = 0;
        }
        supp_mgmt_if_ctx.tls_version = 0;
        if (mgmt_if_adapter.config_feature_bit_map & FEAT_TLS_V1P0) {
          supp_mgmt_if_ctx.tls_version = 1;
        } else if (mgmt_if_adapter.config_feature_bit_map & FEAT_TLS_V1P2) {
          supp_mgmt_if_ctx.tls_version = 2;
        }

#ifdef ENABLE_11R
        //! If both roaming protocol is enabled then select only OTA and remove ODS roaming
        if ((op->ext_custom_feature_bit_map & EXT_FEAT_ENABLE_11R_ODS)
            && (op->ext_custom_feature_bit_map & EXT_FEAT_ENABLE_11R_OTA)) {
          op->ext_custom_feature_bit_map &= ~(EXT_FEAT_ENABLE_11R_ODS);
        }
#endif

        uint8 max_stations_supported, max_clients_supported;
        int max_sta_validation = 0;

        max_sta_validation = parse_max_stations_supported_from_opermode((rsi_uP2Pmode_ram *)op,
                                                                        &max_stations_supported,
                                                                        &max_clients_supported);
        if (max_sta_validation == -1) {
          status->command = SLI_WLAN_RSP_P2P_ENABLE;
          status->status  = STATIONS_COUNT_EXCEEDED_MAX_STA_SUPPORTED;
          cmd_status      = MGMT_IF_FREE_CMD_PKT;
          break;
        }

        mgmt_if_adapter.max_station_supported = max_stations_supported;

        mgmt_if_adapter.opermode        = op->modeVal;
        mgmt_if_adapter.feature_bit_map = op->feature_bit_map;

        uint8 temp_modeVal = set_operating_mode(op->modeVal);

        mgmt_if_adapter.operating_mode = temp_modeVal;

        if (temp_modeVal == WISE_MODE_AP) {
          op->modeVal            = LMAC_OP_AP;
          mgmt_if_adapter.client = SL_FALSE;
        } else if (temp_modeVal == WISE_MODE_PER) {
          op->modeVal = LMAC_OP_PER;
        } else if (temp_modeVal == WISE_CONCURRENT_MODE) {
          op->modeVal = LMAC_OP_CON_CUR;
        } else {
          op->modeVal = (temp_modeVal == WISE_MODE_P2P) ? LMAC_OP_P2P_CLIENT : LMAC_OP_STA;
        }

        if (mgmt_if_adapter.wlan_btr_mode_en == 1) {
          op->modeVal = WLAN_BTR_MODE;
        }

        cmd_status = MGMT_IF_FWD_CMD_TO_UMAC;
      } break;

#ifdef ENABLE_WLAN_RADIO_REQ
      case SLI_WLAN_RADIO_REQ_DEINIT: {
        rsi_wlan_req_radio_t *wlan_radio = (rsi_wlan_req_radio_t *)scatter_buf;
        /*Here LMAC is expecting 5th byte to one to register the wlan radio
          and 0 to deregister the wlan radio*/
        SL_PKT_TX_HDESC_SET_DW1_B1(txPkt, wlan_radio->radio_req);

        if (wlan_radio->radio_req) {
          mgmt_if_adapter.wlan_radio_deinit_from_host = 1;
        } else {
          mgmt_if_adapter.wlan_radio_deinit_from_host = 2;
        }

        /*Here LMAC is expecting frame type as 0x48. As it is already
          being used for wlan query host is sending 0x81 */
        cmd_status = MGMT_IF_FWD_CMD_TO_UMAC;
      } break;
#endif

      case SLI_WLAN_REQ_INIT: {
        if (mgmt_if_adapter.dual_band && mgmt_if_adapter.current_freq_band != SL_BAND_2P4GHZ) {
          mgmt_if_adapter.current_freq_band = SL_BAND_2P4GHZ;
        }

        cmd_status = MGMT_IF_FWD_CMD_TO_UMAC;
      } break;

      case SLI_WLAN_REQ_SET_NON_PREF_CHAN: {
        send_supplicant_command(txPkt);
        cmd_status = MGMT_IF_FREE_CMD_PKT;
        break;
      }

      case SLI_WLAN_REQ_GO_PARAMS_QUERY:
        wise_send_go_params();
        break;

      case SLI_WLAN_REQ_WPS_METHOD: {
#ifdef EXCLUDE_WPS
        status->command = SLI_WLAN_RSP_WPS_METHOD;
        status->status  = NOT_SUPPORTED;
        SL_PRINTF(WLAN_CMD_PARSE_WPS_CLIENT_FEATURE_NOT_SUPPORTED, WLAN_UMAC, LOG_ERROR);
        break;
#endif
#ifdef FIPS_CODE_ENABLE
        /* Dont allow WPS client mode in FIPS mode */
        if (rsi_wsc_cb.fips.fips_mode_enable != 0) {
          status->command = SLI_WLAN_RSP_WPS_METHOD;
          status->status  = FIPS_UNALLOWED_SEC_MODE;
          SL_PRINTF(WLAN_CMD_PARSE_WPS_CLIENT_IN_FIPS_MODE_NOT_ALLOWED, WLAN_UMAC, LOG_ERROR);
          break;
        }
#endif
        if (configure_wps_method(txPkt)) {
          status->command = SLI_WLAN_RSP_WPS_METHOD;
          status->status  = WLAN_STATUS_SUCCESS;
        }
        cmd_status = MGMT_IF_FREE_CMD_PKT;
        break;
      }

      case SLI_WLAN_REQ_GET_WLAN_STATS: {
        uint32 opermode = mgmt_if_adapter.opermode & 0xFF;
        if (!((opermode == 0) || (opermode == 2) || (opermode == 6))) {
          status->command = SLI_WLAN_RSP_GET_WLAN_STATS;
          status->status  = NOT_SUPPORTED;
          SL_PRINTF(WLAN_CMD_PARSE_GET_WLAN_STATS_NOT_SUPPORTED, WLAN_UMAC, LOG_ERROR);
          cmd_status = MGMT_IF_FREE_CMD_PKT;
        } else {
          cmd_status = MGMT_IF_FWD_CMD_TO_UMAC;
        }
        break;
      }

      case SLI_WLAN_REQ_WLAN_EXT_STATS: {
        uint32 opermode = mgmt_if_adapter.opermode & 0xFF;
        if (!((opermode == 0) || (opermode == 2) || (opermode == 6))) {
          status->command = SLI_WLAN_RSP_WLAN_EXT_STATS;
          status->status  = NOT_SUPPORTED;
          SL_PRINTF(WLAN_CMD_PARSE_WLAN_EXT_STATS_NOT_SUPPORTED, WLAN_UMAC, LOG_ERROR);
          cmd_status = MGMT_IF_FREE_CMD_PKT;
        } else {
          cmd_status = MGMT_IF_FWD_CMD_TO_UMAC;
        }
        break;
      }

      case SLI_WLAN_REQ_CONSTATUS_QUERY: {
        if (mgmt_if_adapter.state >= WISE_STATE_CONNECTED) {
          connection_status[0] = 1;
        }
        // TODO
        // status->command = SLI_WLAN_RSP_CONSTATUS_QUERY;
        // status->status  = WLAN_STATUS_SUCCESS;
        sl_mgmt_indicate_to_host(SLI_WLAN_RSP_CONSTATUS_QUERY, 2, WLAN_STATUS_SUCCESS, (uint8 *)connection_status);
        break;
      }

      case SLI_WLAN_REQ_GET_MAC_ADDR: {
        uint8_t size = MAC_ADDR_LEN;
        if (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) {
          size += MAC_ADDR_LEN;
        }
        status->command = SLI_WLAN_RSP_GET_MAC_ADDR;
        status->status  = CMD_STATUS_NO_ERROR;
        status->length  = size;
        status->message = sl_malloc(size);
        if (status->message != NULL) {
          sl_memcpy(status->message, mgmt_if_adapter.mac_first_if, MAC_ADDR_LEN);
          if (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) {
            sl_memcpy(status->message + MAC_ADDR_LEN, mgmt_if_adapter.mac_second_if, MAC_ADDR_LEN);
          }
        } else {
          status->status = MEM_ALLOC_FAILED;
          status->length = 0;
        }
        cmd_status = MGMT_IF_FREE_CMD_PKT;
      } break;

      case SLI_WLAN_REQ_SET_MAC: {
        cmd_status = MGMT_IF_FWD_CMD_TO_UMAC;
      } break;

      case SLI_WLAN_REQ_BAND: {
        uint8 band_val = ((WiFi_CNFG_Band_t *)scatter_buf)->band_val;
#ifdef CONFIG_IEEE_80211J
        if ((mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_IEEE_80211J) && (band_val != SL_BAND_5GHZ)) {
          cmd_status      = MGMT_IF_FREE_CMD_PKT;
          status->command = SLI_WLAN_RSP_BAND;
          status->status  = WISE_ERROR_BAND_NOT_SUPPORTED;
          SL_PRINTF(WLAN_CMD_PARSE_BAND_NOT_SUPPORTED, WLAN_UMAC, LOG_ERROR);
          break;
        }
#endif
        uint8 module_type = cpf_get_module_band_type();

#ifdef CONFIG_IEEE_80211J
        if (!(mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_IEEE_80211J)) {
#endif
          if ((mgmt_if_adapter.operating_mode == WISE_MODE_P2P)
              && (band_val == SL_BAND_5GHZ || band_val == SL_BAND_DUAL)) {
            if (band_val == SL_BAND_5GHZ) {
              status->command = SLI_WLAN_RSP_BAND;
              status->status  = WISE_ERROR_BAND_NOT_SUPPORTED;
              SL_PRINTF(WLAN_CMD_PARSE_BAND_NOT_SUPPORTED, WLAN_UMAC, LOG_ERROR);
              break;
            }
            mgmt_if_adapter.switch_band = SL_BAND_5GHZ;
            update_wpa_config_read_data(WPA_CONFIG_READ_SWITCH_BAND, &mgmt_if_adapter.switch_band, 1);

            if ((module_type == 1) && (band_val == SL_BAND_DUAL)) {
              mgmt_if_adapter.dual_band = SL_BAND_DUAL;
            }
            band_val = SL_BAND_2P4GHZ;
          }

          if ((band_val == SL_BAND_DUAL)) {
            if ((mgmt_if_adapter.operating_mode == WISE_MODE_CONNECTION
                 || mgmt_if_adapter.operating_mode == WISE_MODE_EAP)
                && module_type == 1) {
              mgmt_if_adapter.dual_band = SL_BAND_DUAL;
              band_val                  = SL_BAND_2P4GHZ;
            } else {
              cmd_status      = MGMT_IF_FREE_CMD_PKT;
              status->command = SLI_WLAN_RSP_BAND;
              status->status  = WISE_ERROR_BAND_NOT_SUPPORTED;
              SL_PRINTF(WLAN_CMD_PARSE_BAND_NOT_SUPPORTED, WLAN_UMAC, LOG_ERROR);
              break;
            }
          }
#ifdef CONFIG_IEEE_80211J
        }
#endif
        if ((band_val == SL_BAND_5GHZ) && (module_type != 1)) {
          status->command = SLI_WLAN_RSP_BAND;
          status->status  = WISE_ERROR_BAND_NOT_SUPPORTED;
          SL_PRINTF(WLAN_CMD_PARSE_BAND_NOT_SUPPORTED, WLAN_UMAC, LOG_ERROR);
          break;
        }

        if (mgmt_if_adapter.dual_band == SL_BAND_DUAL) {
          mgmt_if_adapter.band = SL_BAND_DUAL;
        } else {
          mgmt_if_adapter.band = band_val;
        }

        mgmt_if_adapter.current_freq_band = band_val;

        update_wpa_config_read_data(WPA_CONFIG_READ_MODULE_TYPE, &module_type, 1);
        update_wpa_config_read_data(WPA_CONFIG_READ_DUAL_BAND, &mgmt_if_adapter.dual_band, 1);
        cmd_status = MGMT_IF_FWD_CMD_TO_UMAC;
      } break;

      case SLI_WLAN_REQ_JOIN: {
        rsi_uJoin *req_join = (rsi_uJoin *)scatter_buf;

        mgmt_if_adapter.host_cmd = command_id;

        if (req_join->joinFrameSnd.securityType & ENCRYPT_MODE_CCMP_ONLY) {
          req_join->joinFrameSnd.securityType &= ~ENCRYPT_MODE_CCMP_ONLY;

          if (mgmt_if_adapter.operating_mode == WISE_MODE_CONNECTION || mgmt_if_adapter.operating_mode == WISE_MODE_EAP
              || (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE && req_join->joinFrameSnd.vap_id == 0)) {
            mgmt_if_adapter.sta_encryption_mode = CCMP_ONLY;
          } else {
            status->command = SLI_WLAN_RSP_JOIN;
            status->status  = WISE_ERROR_WRONG_PARAMETER;
            SL_PRINTF(WLAN_CMD_PARSE_CCMP_ONLY_IN_NON_STA_MODE, WLAN_UMAC, LOG_ERROR);
            break;
          }
        }
        send_supplicant_command(txPkt);
      } break;

      case SLI_WLAN_REQ_HOST_PSK: {
        uint16 ssid_len = 0, psk_len = 0;
        sl_set_psk_t *psk          = (sl_set_psk_t *)scatter_buf;
        mgmt_if_adapter.pmk_stored = SL_FALSE;
        sl_memset(mgmt_if_adapter.pmk, 0, SL_PMK_LEN);
        sl_memset(mgmt_if_adapter.psk, 0, SL_PSK_LEN);

        if ((psk->TYPE == PSK_FRM_HOST) || (psk->TYPE == LENGTH_BASED_PSK)) {
          /* PSK present */
          if ((sl_strlen((char *)psk->psk_or_pmk) > 7) && (sl_strlen((char *)psk->psk_or_pmk) < SL_PSK_LEN)) {
            sl_strcpy((uint8 *)mgmt_if_adapter.psk, psk->psk_or_pmk);
            sl_memcpy(mgmt_if_adapter.psk, psk->psk_or_pmk, SL_PSK_LEN);
            status->command = SLI_WLAN_RSP_HOST_PSK;
            status->status  = WLAN_STATUS_SUCCESS;
          } else {
            status->command = SLI_WLAN_RSP_HOST_PSK;
            status->status  = WISE_ERROR_WRONG_LEN_PSK;
            SL_DEBUG_LOG("[ERROR] WLAN_CMD_PARSE_INVALID_PSK_LENGTH: 0x%lX\n", status->status);
          }
        } else if (psk->TYPE == PMK_FRM_HOST) {
          mgmt_if_adapter.pmk_stored = SL_TRUE;
          sl_memcpy((uint8 *)mgmt_if_adapter.pmk, psk->psk_or_pmk, SL_PMK_LEN);
          status->command = SLI_WLAN_RSP_HOST_PSK;
          status->status  = WLAN_STATUS_SUCCESS;
        } else if ((psk->TYPE == GENERATE_PMK) || (psk->TYPE == GEN_PMK_FRM_LENGTH_BASED_PSK)) {
          /* PSK and SSID from host generate PMK */
          ssid_len = sl_strlen((char *)psk->ap_ssid);
          psk_len  = sl_strlen((char *)psk->psk_or_pmk);
          /* PSK present */
          if ((psk_len > 7) && (psk_len < SL_PSK_LEN) && (ssid_len)) {
            generate_pmk_wrapper(psk->psk_or_pmk, psk->ap_ssid, ssid_len, mgmt_if_adapter.pmk);
            mgmt_if_adapter.pmk_stored = SL_TRUE;
            status->command            = SLI_WLAN_RSP_HOST_PSK;
            status->status             = WLAN_STATUS_SUCCESS;
            status->length             = SL_PMK_LEN;
            status->message            = sl_malloc(SL_PMK_LEN);
            sl_memcpy(status->message, mgmt_if_adapter.pmk, SL_PMK_LEN);
          } else {
            if (ssid_len) {
              status->command = SLI_WLAN_RSP_HOST_PSK;
              status->status  = WISE_ERROR_WRONG_LEN_PSK;
              SL_DEBUG_LOG("[ERROR] WLAN_CMD_PARSE_WRONG_LEN_PSK: 0x%lX\n", status->status);
            } else {
              status->command = SLI_WLAN_RSP_HOST_PSK;
              status->status  = WISE_ERROR_SSID_NOT_PRESENT;
              SL_DEBUG_LOG("[ERROR] WLAN_CMD_PARSE_SSID_NOT_PRESENT: 0x%lX\n", status->status);
            }
          }
        } else {
          status->command = SLI_WLAN_RSP_HOST_PSK;
          status->status  = WISE_ERROR_WRONG_PARAMETER;
          SL_DEBUG_LOG("[ERROR] WLAN_CMD_PARSE_WRONG_PARAMETER: 0x%lX\n", status->status);
        }
        cmd_status = MGMT_IF_FREE_CMD_PKT;
      } break;

      case SLI_WLAN_REQ_SCAN: {
        send_supplicant_command(txPkt);
      } break;

      case SLI_WLAN_REQ_APCONF: {
        apconfig_st_t *ap_conf = (apconfig_st_t *)scatter_buf;

        sl_memcpy(&mgmt_if_adapter.apconfig, (uint8 *)ap_conf, sizeof(apconfig_st_t));

        /* Start the supplicant if it is not yet started */

        if ((mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE && mgmt_if_adapter.ap_supplicant_priv == NULL)
            || (mgmt_if_adapter.operating_mode == WISE_MODE_AP && mgmt_if_adapter.supplicant_priv == NULL)) {
          wsc_supp_start();
        }

        if (ap_conf->security_type) {
          sl_memcpy(mgmt_if_adapter.psk, ap_conf->psk, SL_PSK_LEN);
        }

        mgmt_if_adapter.sec_type_enable = ap_conf->security_type;

        transform_apconf_txpkt(txPkt);

        cmd_status = MGMT_IF_FWD_CMD_TO_UMAC;
        break;
      }

      case SLI_WLAN_REQ_SET_REGION_AP: {
        rsi_setregion_ap_t *tmp_set_region_ap = (rsi_setregion_ap_t *)scatter_buf;

#ifdef REGION_CONFIG_DISABLE_FOR_ACX
        if (is_hw_acx_module()) {
          status->command = SLI_WLAN_RSP_SET_REGION_AP;
          status->status  = NOT_SUPPORTED;
          SL_PRINTF(WLAN_CMD_PARSE_SET_REGION_AP_NOT_SUPPORTED, WLAN_UMAC, LOG_ERROR);
          break;
        }
#endif /* REGION_CONFIG_DISABLE_FOR_ACX */
        if ((mgmt_if_adapter.operating_mode == WISE_MODE_P2P) && (tmp_set_region_ap->setregion_code_from_user_cmd)) {
          status->command = SLI_WLAN_RSP_SET_REGION_AP;
          status->status  = NOT_SUPPORTED;
          SL_PRINTF(WLAN_CMD_PARSE_SET_REGION_AP_NOT_SUPPORTED, WLAN_UMAC, LOG_ERROR);
          break;
        }
        if (tmp_set_region_ap->no_of_rules > 24) {
          status->command = SLI_WLAN_RSP_SET_REGION_AP;
          status->status  = SET_REGION_AP_ERROR_IN_PARAMETERS;
          SL_PRINTF(WLAN_CMD_PARSE_SET_REGION_AP_ERROR_IN_PARAMETERS, WLAN_UMAC, LOG_ERROR);
          break;
        }
        mgmt_if_adapter.rsi_region_code_from_host = tmp_set_region_ap->setregion_code_from_user_cmd;

        if (!(sl_strncmp(tmp_set_region_ap->country_code, (uint8 *)("US"), 2))) {
          mgmt_if_adapter.region_code = 1;
        } else if (!(sl_strncmp(tmp_set_region_ap->country_code, (uint8 *)("EU"), 2))) {
          mgmt_if_adapter.region_code = 2;
        } else if (!(sl_strncmp(tmp_set_region_ap->country_code, (uint8 *)("JP"), 2))) {
          mgmt_if_adapter.region_code = 3;
        } else if (!(sl_strncmp(tmp_set_region_ap->country_code, (uint8 *)("KR"), 2))) {
          mgmt_if_adapter.region_code = 5;
        } else if (!(sl_strncmp(tmp_set_region_ap->country_code, (uint8 *)("CN"), 2))) {
          mgmt_if_adapter.region_code = REGION_CN_NUM;
        }
        mgmt_if_adapter.set_region_given = 1;
        cmd_status                       = MGMT_IF_FWD_CMD_TO_UMAC;
      } break;

      case SLI_WLAN_REQ_SNR: {
        status->command = SLI_WLAN_RSP_SNR;
        status->status  = NOT_SUPPORTED;
        cmd_status      = MGMT_IF_FREE_CMD_PKT;
      } break;

      case SLI_WLAN_REQ_MULTICAST_FILTER: {
        cmd_status = MGMT_IF_FWD_CMD_TO_UMAC;
        sl_mgmt_indicate_to_host(SLI_WLAN_RSP_MULTICAST_FILTER, 0, WLAN_STATUS_SUCCESS, NULL);
      } break;

      case SLI_WLAN_REQ_GAIN_TABLE: {
        WLAN_USR_GAIN_TABLE_Req *gain_table_info = (WLAN_USR_GAIN_TABLE_Req *)scatter_buf;
        uint16 error_code                        = parse_gain_table_command(gain_table_info);
        status->command                          = SLI_WLAN_RSP_GAIN_TABLE;
        status->status                           = error_code;
      } break;

#ifdef ENABLE_11AX
      case SLI_WLAN_REQ_AX_PARAMS: {
        wlan_11ax_config_params_t *config = NULL;
        config                            = (wlan_11ax_config_params_t *)scatter_buf;
        if ((config->guard_interval > 3) || (config->nominal_pe > 2) || (config->ng_cb_values & 0xEE)
            || (config->tx_only_on_ap_trig > 2) || (config->config_er_su > 2)) {
          status->command = SLI_WLAN_RSP_AX_PARAMS;
          status->status  = NOT_SUPPORTED;
          SL_PRINTF(WLAN_CMD_PARSE_11AX_MODE_NOT_SUPPORTED, WLAN_UMAC, LOG_ERROR);
          break;
        }
        cmd_status = MGMT_IF_FWD_CMD_TO_UMAC;
      } break;

#endif /* ENABLE_11AX */

      case SLI_WLAN_REQ_AP_HT_CAPS: {
        ap_ht_caps_cmd_t *ht_caps = (ap_ht_caps_cmd_t *)scatter_buf;
        //! This command should be avoided if encryption mode is TKIP
        apconfig_st_t *ap_conf = &mgmt_if_adapter.apconfig;
        if ((ap_conf->security_type == 1 || ap_conf->security_type == 2) && ((ap_conf->encryp_mode & 0xF) == 1)) {
          mgmt_if_adapter.is_11n_ap = 0;
          status->command           = SLI_WLAN_RSP_AP_HT_CAPS;
          status->status            = GIVEN_HT_CAPABILITIES_ARE_NOT_SUPPORTED;
          SL_PRINTF(WLAN_CMD_PARSE_HT_CAPABAPILITIES_NOT_SUPPORTED, WLAN_UMAC, LOG_ERROR);
          cmd_status = MGMT_IF_FREE_CMD_PKT;
        } else {
          mgmt_if_adapter.is_11n_ap = ht_caps->is_11n_ap;
          cmd_status                = MGMT_IF_FWD_CMD_TO_UMAC;
        }
      } break;

      case SLI_WLAN_REQ_SET_REGION: {
        rsi_setregion_t *ptr_setregion            = (rsi_setregion_t *)scatter_buf;
        mgmt_if_adapter.region_code               = ptr_setregion->region_code;
        mgmt_if_adapter.rsi_region_code_from_host = ptr_setregion->setregion_code_from_user_cmd;
        mgmt_if_adapter.set_region_given          = 1;
        cmd_status                                = MGMT_IF_FWD_CMD_TO_UMAC;
      } break;

      case SLI_WLAN_REQ_SET_EAP_CFG: {
        uint8 *cmd_buffer                = NULL;
        WLAN_SET_EAP_CFG_Req *eap_config = NULL;

        //! Need to take care of multiple chunk if command comming with new structure change instead of old format
        if (pkt_length > 260) {
          //! Multiple chunks are present so copy content to the linear buffer from scatter
          cmd_buffer = sl_malloc(sizeof(WLAN_SET_EAP_CFG_Req));
          if (cmd_buffer == NULL) {
            status->command = SLI_WLAN_RSP_SET_EAP_CFG;
            status->status  = MEM_ALLOC_FAILED;
            break;
          }

          //! Copy from first scatter
          sl_memcpy(cmd_buffer, scatter_buf, scatter_len);

          //! Copy from second scatter
          sl_memcpy(cmd_buffer + scatter_len, (void *)scatter_buf1, scatter_len1);

          //! Point to the linear buffer
          eap_config = (WLAN_SET_EAP_CFG_Req *)cmd_buffer;
        }

        uint16 error_code = parse_eap_cfg_command(eap_config, pkt_length);

        sl_free(eap_config);

        status->command = SLI_WLAN_RSP_SET_EAP_CFG;
        status->status  = error_code;
      } break;

      case SLI_WLAN_BGSCAN: {
        rsi_uBgscan_t *bg_scan = (rsi_uBgscan_t *)scatter_buf;

        if (bg_scan->bgscan_enable == SL_TRUE) {
          mgmt_if_adapter.bgscan_enabled  = SL_TRUE; //TODO: check cmd is for enable or disable
          mgmt_if_adapter.instant_bg_flag = !!(bg_scan->instant_bg_enable);
        } else if (bg_scan->bgscan_enable == SL_FALSE) {
          mgmt_if_adapter.bgscan_enabled  = SL_FALSE;
          mgmt_if_adapter.instant_bg_flag = SL_FALSE;
        }
        sl_memzero(bg_scan->ssid, sizeof(bg_scan->ssid));

        if (mgmt_if_adapter.bgscan_ssid_len > 0) {
          sl_memcpy(bg_scan->ssid, mgmt_if_adapter.bgscan_ssid, mgmt_if_adapter.ssid_len);
        }

        mgmt_if_adapter.bgscan_enable            = bg_scan->bgscan_enable;
        mgmt_if_adapter.bgscan_threshold         = bg_scan->bgscan_threshold;
        mgmt_if_adapter.rssi_tolerance_threshold = bg_scan->rssi_tolerance_threshold;
        mgmt_if_adapter.bgscan_periodicity       = bg_scan->bgscan_periodicity;
        mgmt_if_adapter.active_scan_duration     = bg_scan->active_scan_duration;
        mgmt_if_adapter.passive_scan_duration    = bg_scan->passive_scan_duration;
        mgmt_if_adapter.multi_probe              = bg_scan->multi_probe;

        /* Send confirmation only if instant_bg is disabled or if bgscan is disabled */
        if ((!mgmt_if_adapter.instant_bg_flag) || (mgmt_if_adapter.bgscan_enabled == SL_FALSE)) {
          status->command = SLI_WLAN_BGSCAN;
          status->status  = WLAN_STATUS_SUCCESS;
        }

        if (status->command) {
          sl_mgmt_indicate_to_host(status->command, status->length, status->status, status->message);
        }
        send_supplicant_command(txPkt);
        cmd_status = MGMT_IF_FWD_CMD_TO_UMAC;
      } break;

#ifdef FIPS_CODE_ENABLE
      case SLI_WLAN_REQ_RE_KEY: {
        uint16 error_code    = WLAN_STATUS_SUCCESS;
        rsi_re_key_t *re_key = (rsi_re_key_t *)scatter_buf;
        /*PMK validation request*/
        if (re_key->type == 0) {
          /*compare with the existing PMK*/
          if (sl_memcmp((uint8 *)mgmt_if_adapter.pmk, re_key->key, SL_PMK_LEN) == 0) {
            error_code = WLAN_STATUS_SUCCESS;

            //! Set state as key verified
            mgmt_if_adapter.fips_key_status_flag = PASSWORD_VERIFIED;
          }
          /*PMK mismatch*/
          else {
            if (mgmt_if_adapter.fips_key_status_flag == PASSWORD_ENTERED) {
              /*PMK didn't match with the existing PMK*/
              error_code = WISE_ERROR_WRONG_PASSWORD;
            } else {
              /*PMK is not present to compare the PMK*/
              error_code = WISE_ERROR_PASSWORD_NOT_PRESENT;
            }
          }
        }
        /*EAP PASSWORD validate*/
        else if (re_key->type == 1) {
          /*Compare the password with the stored Password*/
          if (sl_strcmp((uint8 *)mgmt_if_adapter.passwd, (uint8 *)re_key->key) == 0) {
            error_code = WLAN_STATUS_SUCCESS;

            //! Set state as key verified
            mgmt_if_adapter.fips_key_status_flag = PASSWORD_VERIFIED;
          }
          /*Password Mismatch*/
          else {
            if (mgmt_if_adapter.fips_key_status_flag == PASSWORD_ENTERED) {
              /*Password Mismatch*/
              error_code = WISE_ERROR_WRONG_PASSWORD;
            } else {
              /*Password is not present to compare*/
              error_code = WISE_ERROR_PASSWORD_NOT_PRESENT;
            }
          }
        } else {
          /*send error*/
          error_code = WISE_ERROR_WRONG_PARAMETER;
        }

        status->command = SLI_WLAN_REQ_RE_KEY;
        status->status  = error_code;
        /* The above command has been analyzed and the sc_params members updated here are being used outside of store config.
       * TODO: Refactor sc_params usage.
       */
      } break;
#endif /* FIPS_CODE_ENABLE */

      case SLI_WLAN_REQ_SET_WEPKEY: {
        uint16 error_code = set_wps_key(scatter_buf);

        if (!error_code) {
          cmd_status = MGMT_IF_FWD_CMD_TO_UMAC;
        }

        status->command = SLI_WLAN_RSP_SET_WEPKEY;
        status->status  = error_code;
        SL_PRINTF(WLAN_CMD_PARSE_SET_WEP_KEY, WLAN_UMAC, LOG_INFO, "error_code: %d", error_code);
      } break;

      case SLI_WLAN_REQ_ROAM_PARAMS: {
        rsi_roam_params_t *roam_params = (rsi_roam_params_t *)scatter_buf;
        uint16 error_code              = WISE_ERROR_WRONG_PARAMETER;
        if ((roam_params->roam_threshold <= 0xFF) && (roam_params->roam_hysteresis <= 0xFF)) {
          mgmt_if_adapter.roam_params.roam_enable     = roam_params->roam_enable;
          mgmt_if_adapter.roam_params.roam_hysteresis = roam_params->roam_hysteresis;
          mgmt_if_adapter.roam_params.roam_threshold  = roam_params->roam_threshold;
          /*This is provided to change the value of roam_params and threshold on the fly at any time*/
          rsi_update_roam_params_wrapper(mgmt_if_adapter.supplicant_priv,
                                         roam_params->roam_threshold,
                                         roam_params->roam_hysteresis);
          error_code = 0; /*Success*/
        }
        status->command = SLI_WLAN_RSP_ROAM_PARAMS;
        status->status  = error_code;
        SL_PRINTF(WLAN_CMD_PARSE_ROAM_PARAMS, WLAN_UMAC, LOG_ERROR, "error_code: %d", error_code);
      } break;

      case SLI_WLAN_REQ_REJOIN_PARAMS: {
        rsi_rejoin_params_t *rejoin_retry_params = (rsi_rejoin_params_t *)scatter_buf;
        mgmt_if_adapter.max_rejoin_retry         = rejoin_retry_params->rsi_max_try;
        mgmt_if_adapter.rejoin_scan_interval     = rejoin_retry_params->rsi_scan_interval;
        mgmt_if_adapter.first_time_retry_enable  = rejoin_retry_params->rsi_first_time_retry_enable;
        mgmt_if_adapter.rejoin_magic_code        = 0x4321;
        cmd_status                               = MGMT_IF_FWD_CMD_TO_UMAC;
        sl_mgmt_indicate_to_host(SLI_WLAN_RSP_REJOIN_PARAMS, 0, WLAN_STATUS_SUCCESS, NULL);
      } break;

      case SLI_WLAN_REQ_PER_PARAMS:
      case SLI_WLAN_REQ_PER_STATS: {
#ifdef CONFIG_IEEE_80211J
        if ((mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_IEEE_80211J) && !mgmt_if_adapter.set_region_given) {
          status->command = command_id;
          status->status  = SET_REGION_NOT_GIVEN_IN_11J;
          break;
        }
#endif /* CONFIG_IEEE_80211J */
        cmd_status = MGMT_IF_FWD_CMD_TO_UMAC;
      } break;

      case SLI_MGMT_REQ_DISCONNECT: {
        rsi_disassoc_t *tmp_disassoc = (rsi_disassoc_t *)scatter_buf;

        if (!tmp_disassoc->remove_client) {
          cmd_status = MGMT_IF_FWD_CMD_TO_UMAC;
        }

        send_supplicant_command(txPkt);
      } break;

      case SLI_WLAN_REQ_SELECT_ANTENNA: {
        rsi_antenna_t *antenna = (rsi_antenna_t *)scatter_buf;

        if (pkt_length == 1) {
          /*Adding this for backward compatibality*/
          antenna->gain_2g = 0;
          antenna->gain_5g = 0;
        }
        cmd_status      = MGMT_IF_FWD_CMD_TO_UMAC;
        status->command = SLI_WLAN_RSP_SELECT_ANTENNA;
        status->status  = WLAN_STATUS_SUCCESS;
      } break;

      case SLI_WLAN_REQ_WMM_PS: {
        cmd_status = MGMT_IF_FWD_CMD_TO_UMAC;
#ifdef ENABLE_UAPSD
        rsi_uwmm_ps_t *wmm_ps = (rsi_uwmm_ps_t *)scatter_buf;
        if (wmm_ps->wmm_ps_uapsd_bitmap)
#endif
        {
          status->command = SLI_WLAN_RSP_WMM_PS;
          status->status  = WLAN_STATUS_SUCCESS;
        }
        break;
      }

#ifdef CONFIG_P2P
      case SLI_WLAN_REQ_CFG_P2P: {
        WLAN_CFG_P2P_Req *p2p_cfg = (WLAN_CFG_P2P_Req *)scatter_buf;
        /* GO intent should not be more than 16 */
        sl_memcpy(mgmt_if_adapter.go_intent, p2p_cfg->GOIntent, 2);
        update_wpa_config_read_data(WPA_CONFIG_READ_GO_INTENT, (uint8 *)p2p_cfg->GOIntent, 2);
        update_wpa_config_read_data(WPA_CONFIG_READ_OPERATING_CHANNEL, (uint8 *)p2p_cfg->operChannel, 2);
        /* Device name should not be more than 32 bytes */
        update_wpa_config_read_data(WPA_CONFIG_READ_DEVICE_NAME,
                                    (uint8 *)p2p_cfg->DeviceName,
                                    strlen((const char *)p2p_cfg->DeviceName));
        /* SSID post fix should not be more than 23 */
        update_wpa_config_read_data(WPA_CONFIG_READ_SSID_POSTFIX,
                                    (uint8 *)p2p_cfg->SSIDPostfix,
                                    strlen((const char *)p2p_cfg->SSIDPostfix));
        sl_strcpy((uint8 *)mgmt_if_adapter.psk, p2p_cfg->psk);

        uint16 error_code = 0;

        if ((*(uint16 *)mgmt_if_adapter.go_intent > WISE_AUTONOMOUS_GO)
            || ((strlen((const char *)p2p_cfg->DeviceName) > 32))
            || ((strlen((const char *)p2p_cfg->SSIDPostfix) > 23))) {
          error_code = P2P_CONFIG_FAIL;
        }

        if (*(uint16 *)mgmt_if_adapter.go_intent == WISE_AUTONOMOUS_GO) {
#ifdef RSI_GLOBAL_VARIABLES
          set_client_mode(0);
#endif /* RSI_GLOBAL_VARIABLES */
          mgmt_if_adapter.client = SL_FALSE;
        }

        if (error_code == 0) {
          mgmt_if_adapter.sec_type_enable = 2; /* RSN mode */
          /* wsc supplicant initialization */
          wsc_supp_start();
          wsc_connect();
          wise_update_state(WISE_STATE_P2P_CFG_DONE);
        }
        sl_mgmt_indicate_to_host(SLI_WLAN_RSP_CFG_P2P, 0, error_code, NULL);
      } break;
#endif /*CONFIG_P2P*/

      case SLI_WLAN_REQ_SET_CHANNEL:
      case SLI_BTR_REQ_FLUSH_DATA_Q:
      case SLI_BTR_REQ_SET_MCAST_FILTER:
      case SLI_BTR_REQ_SET_CONFIG_PARAMS: {
        if (mgmt_if_adapter.wlan_btr_mode_en) {
          cmd_status = MGMT_IF_FWD_CMD_TO_UMAC;
        } else {
          status->command = command_id;
          status->status  = CMD_NOT_ALLOWED_IN_THIS_OPERMODE;
        }
      } break;

      case SLI_BTR_REQ_PEER_LIST_UPDATE: {
        if (mgmt_if_adapter.wlan_btr_mode_en && (mgmt_if_adapter.feature_bit_map & FEAT_BTR_MAC_PEER_DS_SUPPORT)) {
          cmd_status = MGMT_IF_FWD_CMD_TO_UMAC;
        } else {
          status->command = SLI_BTR_REQ_PEER_LIST_UPDATE;
          status->status  = CMD_NOT_ALLOWED_IN_THIS_OPERMODE;
        }
      } break;

      case SLI_WLAN_REQ_AP_STOP: {
        mgmt_if_adapter.host_cmd = SLI_WLAN_REQ_AP_STOP;
        cmd_status               = MGMT_IF_FWD_CMD_TO_UMAC;
      } break;

      case SLI_WLAN_REQ_FILTER_BROADCAST:
#ifdef ENABLE_11AX
      case SLI_WLAN_REQ_TWT_AUTO_CONFIG:
      case SLI_WLAN_REQ_TWT_PARAMS:
      case SLI_WIFI_REQ_RESCHEDULE_TWT:
#endif /* ENABLE_11AX */
#ifdef RF_PWR_LEVEL_ENABLE
      case SLI_WLAN_REQ_RF_PWR_LEVEL:
#endif /* RF_PWR_LEVEL_ENABLE */
#ifdef TSF_SYNC
      case SLI_WLAN_REQ_SET_AUTO_RATE:
      case SLI_WLAN_REQ_HOST_TSF_QUERY:
      case SLI_WLAN_REQ_HOST_TSF_SYNC_PKT:
#endif /* TSF_SYNC */
      case SLI_WLAN_REQ_SET_LOW_PWR_SCAN:
      case SLI_WLAN_REQ_NETWORK_PARAMS:
      case SLI_WLAN_REQ_RSSI:
      case SLI_WLAN_REQ_CONFIG:
      case SLI_WLAN_REQ_SET_CONFIG:
      case SLI_WLAN_REQ_FEATSEL:
      case SLI_WLAN_REQ_WMM_PARAMS:
      case SLI_WLAN_REQ_LOAD_MFI_IE:
      case SLI_WLAN_REQ_TIMEOUTS:
      case SLI_WLAN_REQ_SET_FEATURE:
        cmd_status = MGMT_IF_FWD_CMD_TO_UMAC;
        break;

      default:
        break;
    }
  }

  if (cmd_status == MGMT_IF_FWD_CMD_TO_UMAC) {
    uint32_t pkt_sent_status =
      sli_hal_nhcp_send_pkt_without_copy((uint8_t *)(&((sli_nhcp_tx_pkt_t *)txPkt)->host_desc));
    if (pkt_sent_status != SL_STATUS_OK) {
      SL_DEBUG_LOG("\r\nFailed to send TX packet to NWP: 0x%lX\r\n", status);
      SL_MGMT_ASSERT(0);
    }
  } else if (cmd_status == MGMT_IF_FREE_CMD_PKT) {
    if (status->command) {
      sl_mgmt_indicate_to_host(status->command, status->length, status->status, status->message);
    }
    if (status->length != 0 && status->message != NULL) {
      sl_free(status->message);
    }
STATE_FAILURE:
    sl_free_tx_pkt(txPkt);
  }
}

void mgmt_if_update_max_sta_supported(uint8 max_station_supported)
{
  mgmt_if_adapter.max_station_supported = max_station_supported;
}
