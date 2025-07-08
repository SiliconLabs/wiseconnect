/*******************************************************************************
* @file  module_intf.c
* @brief
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

#include "mgmt_platform_if.h"
#include "mgmt_if_utils.h"
#include "mgmt_if_header.h"
#include "mgmt_if_core.h"
#include "mgmt_data_flow.h"
#include "mgmt_if_tx.h"
#include "rsi_controller_apme.h"
#include "sli_supp_event_loop_thread.h"

#include "sli_wlan_command_interface.h"
#include "wlan_user_command_utils.h"
#include "sl_pkt_desc_utils.h"
#include "wlan_scan.h"
#include "ieee_80211_def.h"
#include "sl_wlan_global.h"
#include "rsi_store_config.h"
#ifdef DATA_PATH_UMAC_ENABLE
#include "rsi_wsc_non_rom.h"
#endif /* DATA_PATH_UMAC_ENABLE */

//extern struct ap_cb_s ap_cb;
struct ap_cb_s ap_cb; // TODO : Originally defined in supplicant_rom_image.c

#ifdef PS_DEPENDENT_VARIABLES
#include "powersave.h"
#endif /*PS_DEPENDENT_VARIABLES*/

#include "common.h"
#include "wpa_supplicant_i.h" // TODO: Needs dependency removal. This is temporary
#include "mgmt_if_rsi_wsc.h"

#define WPS_SSID     "WPS_SSID"
#define WPS_SSID_LEN (sizeof(WPS_SSID) - 1)
#ifdef FIPS_CODE_ENABLE
#define IS_AUTH_FIPS_SEC_MODE(m) (m == SEC_MODE_WPA2)
#endif
extern uint8 puf_state;

#define STATION_RSN_IE_PREAUTH_BIT_ENABLE 1

#define WLAN_ERROR_AUTH_FAILED 14
#define RSI_WLAN_EID_RSN       0x30
#define RSI_WLAN_EID_WPA       0xdd //-----------define in driver.h

#define MAX_IE_PKT_LEN     256
#define MAX_ACTION_PKT_LEN 256
#define FRAME_DESC_SZ      16
#define MGMT_QUEUE_NUM     0
#define RSI_SSID_MAX_LEN   32
#define WLAN_ACTION_TYPE   0x00D0

#ifdef DISABLE_CH144
#define WISE_MAX_CHANNEL(x) (((x) == SL_BAND_2P4GHZ) ? 14 : 24)
#else
#define WISE_MAX_CHANNEL(x) (((x) == SL_BAND_2P4GHZ) ? 14 : 25)
#endif
#define CHECK_JOIN_DATARATE(x) ((x) > 28)

int16 eapol_pkt_processing(uint8 *rx_pkt, uint8 vap_id);
extern int wpa_scan_result_compar(const void *a, const void *b);
extern int wpa_driver_rsi_scan(void *priv, struct wpa_driver_scan_params *params);
//extern rsi_uScanList_t *scan_list;
rsi_uScanList_t *scan_list; // TODO : originally defined in connection cmd parse.c

int16 start_wps_push_button();
void wsc_supp_join_done(uint8 client);
void rsi_wfd_join_timed_out(void *eloop_ctx, void *timeout_ctx);
static int rsi_client_disassoc(void *priv, const uint8 *addr, int reason);
static void *supplicant_interface_driver_init(void *ctx, const char *ifname);

struct hi_ie_s {
  uint8 *buf;
  uint16 len;
};
struct host_intf_ies_s {
  struct hi_ie_s ie[4];
};

#ifdef SUPPLICANT_NON_ROM

void rsi_sta_connected_notification_to_host(uint8 *mac_addr, uint8 con_event_type)
{
  uint8 *rxPkt = NULL;
  uint32 type  = (con_event_type == 1) ? RSI_STATION_CONN_IND : RSI_STATION_DISCON_IND;
  rxPkt        = sl_alloc_rx_cmd_desc(type, 0, 0, 6);
  if (rxPkt == NULL) {
    SLI_WLAN_MGMT_ASSERT(UMAC_ASSERT_SME_NO_BUFF_FOR_ASYC_MSG);
    return;
  }

  SL_PKT_SET_RXPKT_SCATTER_BUFF_LEN(rxPkt, 0, 6);
  SL_PKT_SET_RXPKT_SCATTER_BUFF_ADDR(rxPkt,
                                     0,
                                     ((uint32)rxPkt + SL_PKT_GET_RXPKT_HOST_DESC_OFFSET(rxPkt) + (HOST_DESC_LENGTH)));
  sl_memcpy((uint8 *)SL_PKT_GET_RXPKT_SCATTER_BUFF_ADDR(rxPkt, 0), mac_addr, 6);
  SL_PKT_SET_RXPKT_NEXT_PTR(rxPkt, NULL);
  sli_send_host_rx_pkt(rxPkt);
}

#if 1 //ndef ROM_IMAGE_20
const uint8 *wpa_scan_get_ie(const struct wpa_scan_res *res, uint8 ie)
{
  const uint8 *end, *pos;

  pos = (const uint8 *)(res + 1);
  end = pos + res->ie_len;

  while (pos + 1 < end) {
    if (pos + 2 + pos[1] > end)
      break;
    if (pos[0] == ie)
      return pos;
    pos += 2 + pos[1];
  }

  return NULL;
}
#endif

int16 sli_process_on_air_mgmt(uint8 *buf, uint16 len, int rcv_freq, int rcv_rssi, int rcv_channel)
{
  uint8 subtype;
  uint8 *wpa_s = (uint8 *)mgmt_if_adapter.supplicant_priv;
  uint16 ies_len;
  uint8 *dst, *src, *bssid;

  uint8 direct[7];
  direct[0] = 'D';
  direct[1] = 'I';
  direct[2] = 'R';
  direct[3] = 'E';
  direct[4] = 'C';
  direct[5] = 'T';
  direct[6] = '-';

  if ((len == 0) || (wpa_s == NULL)) {
    return WLAN_STATUS_RSI_FAIL;
  }

  dst   = &buf[4];
  src   = &buf[10];
  bssid = &buf[16];

  subtype = buf[0] & IEEE80211_FC0_SUBTYPE_MASK;

  switch (subtype) {
    case IEEE80211_FC0_SUBTYPE_BEACON:
    case IEEE80211_FC0_SUBTYPE_PROBE_RESP: {
      struct wpa_scan_results *scan_res = mgmt_if_adapter.scan_res;
      if (scan_res == NULL)
        break;

      if (len <= 36)
        return WLAN_STATUS_RSI_FAIL;

      ies_len = len - BEACON_IES_START_OFFSET;
      /* In P2P mode need to process only beacon/probe response with ssid
     * "DIRECT-" */
      // if ((wsc_cb->operating_mode == WISE_MODE_P2P) && sl_memcmp( &buf[38],
      // "DIRECT-", 7))
      if ((mgmt_if_adapter.operating_mode == WISE_MODE_P2P) && sl_memcmp(&buf[38], direct, 7)) {
        break;
      }
#ifdef CONFIG_PRESCAN
      if (wsc_cb->p2p_scanning) {
        if (buf[37] > 7) {
          wsc_cb->p2p_device_found = 1;
          sl_memcpy(wsc_cb->p2p_bssid, bssid, 6);
        }
        break;
      }
#endif
      /* if memory is not allocated to hold scan results break */
      struct wpa_scan_res *meta_data = NULL;
      meta_data                      = (struct wpa_scan_res *)sl_zalloc(sizeof(*meta_data));
      if (meta_data == NULL)
        break;
      sl_memzero((uint8 *)meta_data, sizeof(*meta_data));

      /* Filling common params into temporary meta_data*/
      /* copy bssid */
      sl_memcpy((uint8 *)&meta_data->bssid, bssid, ETH_ALEN);
      if (!(mgmt_if_adapter.feature_bit_map & FEAT_DISABLE_11AX)) {
        meta_data->mbssid_params.tx_bssid_B5 = meta_data->bssid[5];
      }

      sl_memcpy((uint8 *)&meta_data->tsf, &buf[24], 8);
      meta_data->freq       = rcv_freq;
      meta_data->channel_no = rcv_channel;
      meta_data->beacon_int = *(uint16 *)&buf[32];
      meta_data->level      = rcv_rssi;
      meta_data->rssiVal    = -rcv_rssi;
      /* If user configured ssid exists then allow only probe responses with
     * matching ssid*/
      if (check_ssid_match(&buf[37])) {
        /* process the scan result of the received probe response BSSID */
        if (process_tx_bss_scan_result(buf, ies_len, (uint8 *)meta_data) == -1)
          goto out;
      }
      /* to get scan results from MBSSID element*/
      if (!(mgmt_if_adapter.feature_bit_map & FEAT_DISABLE_11AX)) {
        if (process_mbss_scan_results(buf, ies_len, (uint8 *)meta_data) == -1) {
          goto out;
        }
      }
out:
      if (meta_data != NULL)
        sl_free(meta_data);
      break;
    }
    case IEEE80211_FC0_SUBTYPE_ACTION: {
      if (len <= 25)
        return WLAN_STATUS_RSI_FAIL;

      ieee80211_macframe_t *mac_header = (ieee80211_macframe_t *)buf;
      uint8 *payload                   = buf + MAC_HEADER_LENGTH;
      uint8 category;
      if (mac_header->fc[1] & IEEE80211_FC1_PROTECTED) {
        payload += CCMP_HEADER_SIZE;
      }
      category = payload[0];

      if (category == IEEE80211_ACTION_CATEGORY_WNM || category == IEEE80211_ACTION_CATEGORY_RADIO_MEASUREMENT) {
#ifdef CHIP_9117
        union wpa_event_data data;
        sl_memset((uint8 *)&data, 0, sizeof(union wpa_event_data));
        data.rx_mgmt.frame      = buf;
        data.rx_mgmt.frame_len  = len;
        data.rx_mgmt.freq       = rcv_freq;
        data.rx_mgmt.ssi_signal = rcv_rssi;
        wpa_supplicant_event_wrapper((void *)wpa_s, EVENT_RX_MGMT, &data);
#endif
      }
#if CONFIG_P2P
      else {
        umac_rom_bss_ptr->wpas_p2p_rx_action(wpa_s, dst, src, bssid, buf[24], &buf[25], len - 25, rcv_freq);
      }
#endif
      break;
    }
    case IEEE80211_FC0_SUBTYPE_PROBE_REQ: {
      if (len <= 24)
        return WLAN_STATUS_RSI_FAIL;
      ies_len = len - 24;
      break;
    }
    case IEEE80211_FC0_SUBTYPE_DEAUTH: {
      mgmt_if_adapter.supplicant_triggered_bg_scan = 0;
      mgmt_if_adapter.is_11w_enabled               = 0;
      trigger_rom2_event_deauth_wrapper((void *)wpa_s);
    } break;
    case IEEE80211_FC0_SUBTYPE_DISASSOC: {
      mgmt_if_adapter.supplicant_triggered_bg_scan = 0;
      mgmt_if_adapter.is_11w_enabled               = 0;
      trigger_rom2_event_disassoc_wrapper((void *)wpa_s);
    } break;
    default: {
      break;
    }
  }
  return WLAN_STATUS_SUCCESS;
}

#ifdef ENABLE_11R
void parse_beacon(struct scanConfirm_s *scanCon,
                  uint8 *beacon,
                  int16 pktlen,
                  uint16 scan_token_rcvd,
                  scanConfirm_nonrom_t *scanCon_nonrom)
#else
void parse_beacon(struct scanConfirm_s *scanCon, uint8 *beacon, int16 pktlen, uint16 scan_token_rcvd)
#endif
{
  uint8 RSI_WLAN_WIFI_OUI_RSN[] = { 0x0, 0x0F, 0xAC };
  uint8 RSI_WLAN_WIFI_OUI[]     = { 0x0, 0x50, 0xF2 };
  uint8 RSI_WLAN_WIFI_OUI_TMP[] = { 0x00, 0x90, 0x4c };
  uint16 ii                     = 0; /*FIXME 0 or 16*/
  uint8 eId = 0, eIdLength = 0;
  uint8 *response = NULL;
#ifdef ENABLE_11AX
  uint8 ext_id;
#endif
  response = beacon;
  if (scanCon->CapabilityInfo & BIT(4))
    scanCon->sec_mode = SEC_MODE_WEP;
  else
    scanCon->sec_mode = SEC_MODE_OPEN;

  if (((scanCon->CapabilityInfo & BIT(0)) == 1) && ((scanCon->CapabilityInfo & BIT(1)) == 0)) {
    scanCon->uNetworkType = STA_INFRASTRUCTURE;
  } else {
    scanCon->uNetworkType = IBSS;
  }

  scanCon->SupportedRatesLen    = 0; /* Initialising to zero incase it this Eid is not present. */
  scanCon->ExtSupportedRatesLen = 0; /* Initialising to zero incase it this Eid is not present. */
  scanCon->ht_capable           = 0;
  scanCon->ht_capable1          = 0;
  scanCon->vht_capable          = 0;
#ifdef ENABLE_11AX
  scanCon->he_capable = 0;
#endif
  while ((ii + 1) < pktlen) {
    eId       = response[ii];
    eIdLength = response[ii + 1];

    switch (eId) {
      case IEEE80211_ELEMID_SSID: {
        scanCon->SSIDLen = eIdLength;
        sl_memcpy((uint8 *)scanCon->SSID, (uint8 *)(response + (ii + 2)), eIdLength);
        scanCon->SSID[eIdLength] = '\0';
      } break;

      case IEEE80211_ELEMID_RATES:
        scanCon->SupportedRatesEID = IEEE80211_ELEMID_RATES;
        scanCon->SupportedRatesLen = eIdLength;
        sl_memcpy((uint8 *)scanCon->SupportedRates, (uint8 *)(response + (ii + 2)), eIdLength);
        break;
      case IEEE80211_ELEMID_XRATES:
        scanCon->ExtSupportedRatesLen = eIdLength;
        sl_memcpy((uint8 *)scanCon->ExtSupportedRates, (uint8 *)(response + (ii + 2)), eIdLength);
        break;
#ifdef CHIP_9117
      case IEEE80211_ELEMID_RM_ENABLED_CAP:
        if (is_ap_rrm_capable(scanCon)) {
          scanCon_nonrom->ap_rrm_capable = 1;
        }
        break;
#endif
      case IEEE80211_ELEMID_DSPARMS:
        /*
       * We got phy param with EID 3. Copy this to phy param array.
       */
        scanCon->PhyParamLen = eIdLength;

        sl_memcpy((uint8 *)&scanCon->PhyParam[0], (uint8 *)(response + (ii + 2)), eIdLength);

        scanCon->channelNum = scanCon->PhyParam[0];

        /*FIXME check channel number */
        break;
      case IEEE80211_ELEMID_VENDOR: {
        if (((!memcmp(&response[ii + 2], RSI_WLAN_WIFI_OUI, 3)) && response[ii + 2 + 3] == 0x01)
            && ((scanCon->sec_mode == SEC_MODE_OPEN) || (scanCon->sec_mode == SEC_MODE_WEP))) {
          uint16 pcnt           = 0;
          uint16 acnt           = 0;
          scanCon->sec_mode     = SEC_MODE_WPA;
          scanCon->group_cipher = response[ii + 11];

          /* Pairwise cipher count */
          pcnt = *(uint16 *)&response[ii + 12];

          /* Auth count */
          acnt = *(uint16 *)&response[ii + 12 + 2 + pcnt * 4];

          scanCon->enc_mode = response[ii + 17];
          if (pcnt == 2) {
            scanCon->enc_mode |= response[ii + 17 + 4];
          }

          if (response[ii + 12 + 2 + pcnt * 4 + 2 + acnt * 4 - 1] == 1) /* WPA Enterprise */
          {
            scanCon->sec_mode = SEC_MODE_WPA_ENTERPRISE;
          }
        }

        if (!memcmp(&response[ii + 2], RSI_WLAN_WIFI_OUI_TMP, 3) && response[ii + 2 + 3] == 0x33) {
          sl_memcpy((uint8 *)&scanCon->ie_11n, (uint8 *)(response + ii), eIdLength + 2);
          scanCon->ie_11n.ele_11n.temp[0] = 0x00;
          scanCon->ie_11n.ele_11n.temp[1] = 0x00;
          scanCon->ie_11n.ele_11n.temp[2] = 0x18;
          scanCon->ie_11n.ele_11n.temp[3] = 0xFF;
          scanCon->ie_11n.ele_11n.temp[4] = 0x00;
          scanCon->capable_11n            = 1;
        }
        if (!memcmp(&response[ii + 2], RSI_WLAN_WIFI_OUI, 3) && (response[ii + 2 + 3] == 0x02)) {
          if (response[ii + 8] & 0x80) {
            scanCon->wmm_ps_enable = 1;
          } else {
            scanCon->wmm_ps_enable = 0;
          }
          scanCon->qos_enable = 1;
        }
#ifdef RSI_ENABLE_CCX
        if (isccxoui(&response[ii])) {
          scanCon->ccx_capable        = SL_TRUE;
          mgmt_if_adapter.ccx_capable = SL_TRUE;

          if (response[ii + 5] == CCX_VERSION_OUI_IE) // VERSION_OUI_IE
            scanCon->ccx_ver_ie = 1;                  // CCX version Present
          else if (response[ii + 5] == CCX_RADIO_CAP_OUI_IE)
            // Copying aironet ie from probe response/beacons
            sl_memcpy(&scanCon->radio_cap_ie, &response[ii], sizeof(struct ccx_radio_cap_ie));
        }
#endif
      } break;
      case IEEE80211_ELEMID_RSN:
        scanCon->sec_mode = SEC_MODE_WPA2_ENTERPRISE;
        if (!memcmp(&response[ii + 4], RSI_WLAN_WIFI_OUI_RSN, 3)) {
          uint16 pcnt           = 0;
          uint16 akmcnt         = 0;
          scanCon->sec_mode     = SEC_MODE_WPA2;
          scanCon->group_cipher = response[ii + 7];

          /* Pairwise cipher count */
          pcnt              = *(uint16 *)&response[ii + 8];
          akmcnt            = *(uint16 *)&response[ii + 8 + 2 + 4 * pcnt];
          scanCon->enc_mode = response[ii + 13];

          if (pcnt == 2) {
            scanCon->enc_mode |= response[ii + 13 + 4];
          }

          if ((response[ii + 15 + pcnt * 4]) == 1) /* WPA2 Enterprise */
          {
            scanCon->sec_mode = SEC_MODE_WPA2_ENTERPRISE;
          }
          if (mgmt_if_adapter.csec_mode == SEC_MODE_WPA3 || mgmt_if_adapter.csec_mode == SEC_MODE_WPA3_TRANSITION) {
            wpa3_scan_result_sec_update_wrapper(&response[ii], pcnt, (void *)scanCon, 1);
          }
        }
        break;
        /* Included for 11n support */
      case IEEE80211_ELEMID_HTINFO: {
        scanCon->ht_capable = 1;
        sl_memcpy(scanCon->htCap_ie, (uint8 *)(response + ii), (eIdLength + 2));
      } break;

      case IEEE80211_ELEMID_HTCAP: {
        scanCon->ht_capable1 = 1;
        sl_memcpy(scanCon->htCap1_ie, (uint8 *)(response + ii), (eIdLength + 2));

        // scanCon->htCap1_ie[2] = 0x00;
        // scanCon->htCap1_ie[3] = 0x00;
        scanCon->htCap1_ie[4] = 0x19;
        scanCon->htCap1_ie[6] = 0x00;
      } break;

#ifdef RSI_ENABLE_CCX
      case IEEE80211_ELEMID_AIRONET: {
        // Copying aironet ie from probe response/beacons
        sl_memcpy(&scanCon->aironet_ie, &response[ii], sizeof(struct ieee80211_aironet_ie));
      } break;
      case IEEE80211_ELEMID_TPC: {
        // Copying power control ie from probe response/beacons
        sl_memcpy(&scanCon->aironet_client_pwr_ie, &response[ii], sizeof(struct ieee80211_aironet_client_pwr_ie));
      } break;
      case IEEE80211_ELEMID_PWRCNSTR: {
        scanCon->pwr_cnstraint = response[ii + 2];
      } break;
#endif
#ifdef ENABLE_11R
      case IEEE80211_ELEMID_MOB_DOMAIN: {
        sl_memcpy(&scanCon_nonrom->mdie, &response[ii], sizeof(mobility_dom_ie_t));
      } break;
#endif
#ifdef REGION_CONFIG_DISABLE_FOR_ACX
      case IEEE80211_ELEMID_COUNTRY: {
        if (is_hw_acx_module()) {
          scanCon_nonrom->Country_info.found_countryie = 1;
          sl_memcpy((uint8 *)&scanCon_nonrom->Country_info.eId, &response[ii], response[ii + 1] + 2);
        }
      } break;
#endif /* REGION_CONFIG_DISABLE_FOR_ACX */

      case IEEE80211_ELEMID_VHT_CAP: {
        scanCon->vht_capable = 1;
      } break;

#ifdef ENABLE_11AX
      case IEEE80211_ELEMID_EXTENSION: {
        ext_id = response[ii + 2];
        switch (ext_id) {
          case IEEE80211_ELEMID_EXT_HE_CAPABILITY:
            scanCon->he_capable = 1;
            break;
          default:
            break;
        }
      } break;
#endif
      default:
        break;
    }
    ii += (eIdLength + 2);
  }

  mgmt_if_adapter.sta_connection_secMode = scanCon->sec_mode;
#ifdef DATA_PATH_UMAC_ENABLE
  WSC_SAVE_STA_SEC_MODE(scanCon->sec_mode);
#endif /* DATA_PATH_UMAC_ENABLE */

  if (scanCon->PhyParam[0] == 0) {
    scanCon->PhyParam[0] = scan_token_rcvd;
    scanCon->channelNum  = scanCon->PhyParam[0];
  }
}

/**
 * Global Variables
 */

/*===========================================================================
 *
 * @fn          int16 rsi_scan(rsi_uScan *uScanFrame,uint16 band)
 * @brief       Sends the Scan command to the Wi-Fi module
 * @param[in]   rsi_uScan *uScanFrame,Pointer to scan structure
 * @param[out]  none
 * @return      errCode
 *              -1 = SPI busy / Timeout
 *              -2 = SPI Failure
 *              0  = WLAN_STATUS_SUCCESS
 * @section description 
 * This API is used to scan for Access Points. This API should be called only 
 * after the rsi_init API.
 *
 * @section prerequisite 
 * rsi_init should be done successfully  
 */

/* Init Code=0x10, Data Frame bytes to transfer=0x04, Frame Type, 0x02=Data, 0x04=Management */
int16 rsi_scan(rsi_uScan *uScanFrame, uint16 band)
{
  int16 retval;

  if ((((uint16)uScanFrame->scanFrameSnd.channel_bit_mask[0] & 0xFFFF) == 0xFFFF)
      && (*(uint32 *)&uScanFrame->scanFrameSnd.channel_bit_mask[1] & 0xFFFFFFFF) == 0xFFFFFFFF) {
    if (mgmt_if_adapter.scan_channel_bitmap_2_4_GHz || mgmt_if_adapter.scan_channel_bitmap_5_GHz) {
      if (mgmt_if_adapter.dual_band == SL_BAND_DUAL) {
        uScanFrame->scanFrameSnd.channel_bit_mask[0] = mgmt_if_adapter.scan_channel_bitmap_2_4_GHz & (0x3fff);
#ifdef DISABLE_CH144
        *(uint32 *)&uScanFrame->scanFrameSnd.channel_bit_mask[1] = mgmt_if_adapter.scan_channel_bitmap_5_GHz
                                                                   & (0xffffff);
#else
        *(uint32 *)&uScanFrame->scanFrameSnd.channel_bit_mask[1] = mgmt_if_adapter.scan_channel_bitmap_5_GHz
                                                                   & (0x1ffffff);
#endif
      } else if (band == SL_BAND_5GHZ) {
#ifdef DISABLE_CH144
        *(uint32 *)&uScanFrame->scanFrameSnd.channel_bit_mask[1] = mgmt_if_adapter.scan_channel_bitmap_5_GHz
                                                                   & (0xffffff);
#else
        *(uint32 *)&uScanFrame->scanFrameSnd.channel_bit_mask[1] = mgmt_if_adapter.scan_channel_bitmap_5_GHz
                                                                   & (0x1ffffff);
#endif
      } else {
        uScanFrame->scanFrameSnd.channel_bit_mask[0] = mgmt_if_adapter.scan_channel_bitmap_2_4_GHz & (0x3fff);
      }
    } else {

      uScanFrame->scanFrameSnd.channel_bit_mask[0] = 0x3FFF;
#ifdef DISABLE_CH144
      *(uint32 *)&uScanFrame->scanFrameSnd.channel_bit_mask[1] = 0xFFFFFF;
#else
      *(uint32 *)&uScanFrame->scanFrameSnd.channel_bit_mask[1] = 0x1FFFFFF;
#endif
    }
  }
  if (!(mgmt_if_adapter.custom_feature_bit_map & FEAT_SEL_DFS_CHANNEL_SUPPORT)
      && !(mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_IEEE_80211J)) {
#ifdef DISABLE_CH144
    *(uint32 *)&uScanFrame->scanFrameSnd.channel_bit_mask[1] &= 0xF8000F;
#else
    *(uint32 *)&uScanFrame->scanFrameSnd.channel_bit_mask[1] &= 0x1F0000F;
#endif
  }

#ifdef CHIP_9117
  if (mgmt_if_adapter.supplicant_triggered_bg_scan) {
    return trigger_bg_scan(uScanFrame->scanFrameSnd.channel_bit_mask[0],
                           *(uint32 *)&uScanFrame->scanFrameSnd.channel_bit_mask[1],
                           START);
  }
#endif
  retval = mgmt_hm_scan((uint8 *)uScanFrame);
  return retval;
}
#endif // SUPPLICANT_NON_ROM

int16 is_client_mode(void)
{
  if (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) {
    return ((mgmt_if_adapter.con_operating_mode == WISE_MODE_EAP)
            || (mgmt_if_adapter.con_operating_mode == WISE_MODE_CONNECTION));
  } else
    return ((mgmt_if_adapter.operating_mode == WISE_MODE_CONNECTION)
            || (mgmt_if_adapter.operating_mode == WISE_MODE_EAP));
}

#ifdef SUPPLICANT_NON_ROM
uint32 rsi_pin_checksum(unsigned int pin)
{
  uint32 accum = 0;
  while (pin) {
    accum += 3 * (pin % 10);
    pin /= 10;
    accum += pin % 10;
    pin /= 10;
  }

  return (10 - accum % 10) % 10;
}
unsigned int rsi_generate_pin(void)
{
  uint32 val;

  /* Generate seven random digits for the PIN */
  if (random_get_bytes_wrapper((uint8 *)&val, sizeof(val)) < 0) {
    struct os_time now;
    os_get_time_wrapper(&now);
    val = os_random_wrapper() ^ now.sec ^ now.usec;
  }
  val %= 10000000;
  if (val < 1000000) {
    val += 1111111;
  }

  /* Append checksum digit */
  return val * 10 + rsi_pin_checksum(val);
}

uint16 set_wps_key(uint8 *buf)
{
  wep_key_ds *wep_key = (wep_key_ds *)buf;
  uint16 error_code;
  uint16 key_index;

  if (wep_key->index >= 4) {
    error_code = WISE_ERROR_WRONG_LEN_WEP_KEY;
  } else {
    /* Other key may be zero but not other length allowed */
    for (key_index = 0; key_index < 4; key_index++) {
      if (!((sl_strlen((char *)wep_key->key[key_index]) == 10) || (sl_strlen((char *)wep_key->key[key_index]) == 26))) {
        if (key_index == wep_key->index) {
          // error code 46 for incorrect key length, valid lengths are 0, 10 and 26 (Note 0 means no key).
          error_code = WISE_ERROR_WRONG_LEN_WEP_KEY;
          break;
        } else {
          sl_memset(wep_key->key[key_index], 0, 32);
        }
      }
    }
  }

  if (!error_code) {
    sl_memcpy(&mgmt_if_adapter.wep_key, wep_key, sizeof(wep_key_ds));
  } else {
    SL_PRINTF(WLAN_SUPP_SET_WEP_KEY, WLAN_UMAC, LOG_ERROR, "error_code: %d", error_code);
  }

  return error_code;
}

uint16 configure_wps_method(uint8 *txPkt)
{
  rsi_uwps_t *wps_method;
  uint16 pin_length;
  uint32 pin_int           = 0;
  uint8 extended_desc_size = SL_PKT_TX_HDESC_GET_DW1_EXT_DESC_SIZE(txPkt);
  uint8 *scatter_buf0_addr = (((uint8 *)SL_PKT_GET_TXPKT_HDESC_ADDR(txPkt)) + HOST_DESC_LENGTH + extended_desc_size);

  wps_method = (rsi_uwps_t *)scatter_buf0_addr;
  /*add null in the end we are going to find the length by sl_strlen*/
  wps_method->wps_pin[8] = '\0';

  supp_mgmt_if_ctx.wps_method = wps_method->wps_method;
  /*PIN METHOD*/
  if (wps_method->wps_method == 1) {
    /*Generate Pin*/
    if (wps_method->generate_pin) {
      pin_int = rsi_generate_pin();
      /*convert the generated pin in ASCII form*/
      sl_itoa(pin_int, mgmt_if_adapter.wps_pin);
    }
    /*validate Pin*/
    else {
      /*compute the length of the Pin came from host*/
      pin_length = sl_strlen((char *)wps_method->wps_pin);
      /*clear the already stored pin*/
      sl_memset(mgmt_if_adapter.wps_pin, 0, SL_WPS_PIN_LEN);
      /*Pin is of 8 Digit*/
      if (pin_length == SL_WPS_PIN_LEN) {
        /*convert the pin in integer form to calculate the last digit*/
        pin_int = sl_atoi(wps_method->wps_pin);
        /*Calculate the last digit and compare with the last digit came from
         * host*/
        if ((pin_int % 10) != (rsi_pin_checksum(pin_int / 10))) {
          sl_mgmt_indicate_to_host(SLI_WLAN_RSP_WPS_METHOD, 0, WISE_ERROR_WRONG_PIN, NULL);
          return 0;
        }
        /*store the pin came form host*/
        sl_strcpy(mgmt_if_adapter.wps_pin, wps_method->wps_pin);
      }
      /*Pin Length is less than 8 digits incorrect pin*/
      else {
        sl_mgmt_indicate_to_host(SLI_WLAN_RSP_WPS_METHOD, 0, WISE_ERROR_WRONG_PIN_LEN, NULL);
        return 0;
      }
    }
    /*send pin to host with ok response*/
    sl_mgmt_indicate_to_host(SLI_WLAN_RSP_WPS_METHOD, SL_WPS_PIN_LEN, 0, (uint8 *)mgmt_if_adapter.wps_pin);
    return 0;
  }
  return 1;
}

/*==============================================*/
/**
 *
 * @fn         check_valid_fixed_rate_ap
 * @brief      This function check whether selected fixed rate is valid or not.
 * @param[in]  selected fixed rate.
 * @return  Success, 0
 *          Failure, 1
 *
 * @section description
 * This function check selected fixed rate is valid for creating AP or not.
 *
 *
 */
int8 check_valid_fixed_rate_ap(uint8 uRate)
{
  if ((uRate > 12) && !mgmt_if_adapter.is_11n_ap) {
    return 1;
  }
  return 0;
}

/*==============================================*/
/**
 *
 * @fn         trigger_bg_scan
 * @brief      This function executes/disables background scan
 * @param[in]  2.4GHz channel bitmask
 * @param[in]  5GHz channel bitmask
 * @param[in]  Flag to start/stop background scan
 * @return     0               - Success
 *             Non-Zero  Value - Failure
 *
 * @section description
 * This function is used to fill start/stop background scan internally.
 */
int16 trigger_bg_scan(uint16 channel_bitmask_24ghz, uint32 channel_bitmask_5ghz, uint8 cmd_enable)
{
  uint8 *scan_res = NULL;
  rsi_uBgscan_t bg_scan;

  sl_memzero(&bg_scan, sizeof(bg_scan));
  if (cmd_enable) {
    if (mgmt_if_adapter.bgscan_enabled == SL_TRUE) {
      mgmt_if_adapter.instant_bg_flag  = SL_TRUE;
      bg_scan.bgscan_enable            = mgmt_if_adapter.bgscan_enable;
      bg_scan.instant_bg_enable        = 1;
      bg_scan.active_scan_duration     = mgmt_if_adapter.active_scan_duration;
      bg_scan.passive_scan_duration    = mgmt_if_adapter.passive_scan_duration;
      bg_scan.bgscan_threshold         = mgmt_if_adapter.bgscan_threshold;
      bg_scan.rssi_tolerance_threshold = mgmt_if_adapter.rssi_tolerance_threshold;
      bg_scan.bgscan_periodicity       = mgmt_if_adapter.bgscan_periodicity;
      bg_scan.multi_probe              = mgmt_if_adapter.multi_probe;
    } else {
      //! Enable instant bg scan as scan should be fresh and result should go to
      //! host
      mgmt_if_adapter.instant_bg_flag      = SL_TRUE;
      bg_scan.bgscan_enable                = 1;
      bg_scan.instant_bg_enable            = 1;
      bg_scan.active_scan_duration         = 100;
      bg_scan.passive_scan_duration        = 50;
      bg_scan.bgscan_threshold             = 0xff;
      bg_scan.rssi_tolerance_threshold     = 0;
      bg_scan.bgscan_periodicity           = 0xff;
      bg_scan.multi_probe                  = 0;
      mgmt_if_adapter.execute_bg_scan_once = 1;
    }
  }
  sl_memset(bg_scan.ssid, 0, sizeof(bg_scan.ssid));

  //! Fill scan ssid from scan command
  bg_scan.ssid_len = mgmt_if_adapter.scan_ssid_len;
  if (bg_scan.ssid_len > 0) {
    sl_memcpy(bg_scan.ssid, mgmt_if_adapter.scan_ssid, bg_scan.ssid_len);
  }

  if (!channel_bitmask_24ghz) {
    channel_bitmask_24ghz = 0x3FFF;
  }
  if (!channel_bitmask_5ghz) {
#ifdef DISABLE_CH144
    channel_bitmask_5ghz = 0xFFFFFF;
#else
    channel_bitmask_5ghz = 0x1FFFFFF;
#endif
  }

  if (mgmt_if_adapter.current_freq_band == SL_BAND_5GHZ) {
    bg_scan.channel_bitmap = channel_bitmask_5ghz;
  } else {
    bg_scan.channel_bitmap = channel_bitmask_24ghz;
  }

  if (cmd_enable) {
    //! Clear the scan list
    scan_res = sl_get_scan_results(mgmt_if_adapter.supplicant_priv);
    if (scan_res)
      wpa_scan_results_free_wrapper((void *)scan_res);
  }

  return mgmt_hm_send_bgscan_cmd(&bg_scan);
}

// venkat FIXME :  is this function is expecting linear buffer instead of
// scatters ??
void send_supplicant_command(uint8 *txPkt)
{
  int *freqs;
  rsi_uScan_non_rom_t *scan_cb;
  rsi_uJoin *join_cb;
  uint8 tmp_ssid[64];
  uint8 pmk_null[32];
  uint8 ssid_len;
  uint8 pmf                    = 0;
  int16 ret_val                = 0;
  sl_status_t status           = SL_STATUS_OK;
  struct wpa_supplicant *wpa_s = (struct wpa_supplicant *)mgmt_if_adapter.supplicant_priv;
  uint8 null_bssid[6]          = { 0 };
  uint16 tmp_psk_len           = 0;
#if (defined(CONFIG_P2P) || (!defined(EXCLUDE_AP) && !defined(EXCLUDE_WPS)))
  uint8 go;
#endif
  uint8 extended_desc_size = SL_PKT_TX_HDESC_GET_DW1_EXT_DESC_SIZE(txPkt);
  uint8 *scatter_buf0_addr = (((uint8 *)SL_PKT_GET_TXPKT_HDESC_ADDR(txPkt)) + HOST_DESC_LENGTH + extended_desc_size);
  uint8 *scan_res          = NULL;

  if (mgmt_if_adapter.supplicant_priv == NULL) {

    /* P2P/EAP config not done ERROR_CODE*/
    sl_mgmt_indicate_to_host(SLI_WLAN_RSP_SCAN, 0, P2P_EAP_CONFIG_NOT_DONE, NULL);
    return;
  }
#ifdef CHIP_9117
  else if (SL_PKT_TX_HDESC_GET_DW0_FRAME_TYPE(txPkt) == SLI_WLAN_REQ_SET_NON_PREF_CHAN) {
    rsi_set_non_pref_chan_info_t *non_pref_chan_config = (rsi_set_non_pref_chan_info_t *)scatter_buf0_addr;
    int status = wpas_mbo_update_non_pref_chan_wrapper(wpa_s, non_pref_chan_config);
    if (status == 0)
      sl_mgmt_indicate_to_host(SLI_WLAN_REQ_SET_NON_PREF_CHAN, 0, WLAN_STATUS_SUCCESS, NULL);
    else
      sl_mgmt_indicate_to_host(SLI_WLAN_REQ_SET_NON_PREF_CHAN, 0, NON_PREF_CHAN_CONFIG_FAILED, NULL);
    return;

  }
#endif
  else if (SL_PKT_TX_HDESC_GET_DW0_FRAME_TYPE(txPkt) == SLI_MGMT_REQ_DISCONNECT) {
    rsi_disassoc_t *tmp_disassoc = (rsi_disassoc_t *)scatter_buf0_addr;
#ifdef RSI_GLOBAL_VARIABLES
    set_rejoin_going_on(0);
    set_okc(0);
#endif /* RSI_GLOBAL_VARIABLES */
#ifdef PS_DEPENDENT_VARIABLES
    set_roam_ind(0);
#endif /* PS_DEPENDENT_VARIABLES */
    status = free_tx_host_q();
    if (status != SL_STATUS_OK) {
      SL_DEBUG_LOG("[ERROR] Failed to free tx queue: 0x%lX\r\n", status);
      SL_MGMT_ASSERT(0);
    }
#ifdef ENABLE_DRAEGER_CUSTOMIZATION
    if (!tmp_disassoc->remove_client) {
      // This is the case where host issue diconnect in station mode
      mgmt_if_adapter.rejoin_going_on   = SL_FALSE;
      mgmt_if_adapter.roam_ind          = 0;
      mgmt_if_adapter.okc               = 0;
      mgmt_if_adapter.async_state_code  = RSI_MODULE_STATE_UNASSOCIATED;
      mgmt_if_adapter.async_reason_code = RSI_MODULE_STATE_DEAUTH_FRM_HOST;
      wise_module_state(RSI_MODULE_STATE_FINAL_CONNECTON);
      // Clean up the updated value
      mgmt_if_adapter.async_state_code  = RSI_MODULE_STATE_START_UP_OR_NOT_AVAVILABLE;
      mgmt_if_adapter.async_reason_code = RSI_MODULE_STATE_START_UP_OR_NOT_AVAVILABLE;
      mgmt_if_adapter.async_rssi        = RSI_MODULE_STATE_RSSI_NOT_AVAILABLE;
      mgmt_if_adapter.async_channel     = RSI_MODULE_STATE_START_UP_OR_NOT_AVAVILABLE;
      sl_memzero(mgmt_if_adapter.async_bssid, MAC_ADDR_LEN);
    }
#endif
    mgmt_if_adapter.host_cmd = SL_PKT_TX_HDESC_GET_DW0_FRAME_TYPE(txPkt);
    /* If it is AP mode remove client should be 1 */
    if (tmp_disassoc->remove_client) {
      //rsi_client_disassoc(&rsi_wsc_cb, eth_src_hw_addr, tmp_disassoc->mac_addr, 0);
      rsi_client_disassoc(NULL, tmp_disassoc->mac_addr, 0);
    } else {
      //! Setting 1 to differentiate disconnect is from host or from supplicant
      SL_PKT_TX_HDESC_SET_DW2_B0(txPkt, 1);
      if (!mgmt_if_adapter.dyn_sta_ap_switch_enable) {
#ifdef DATA_PATH_UMAC_ENABLE
        //! memsetting the ip address
        sl_memzero(&client_ip.nx_ip_interface[!(tcp_ip_cb.netx_ap_intf_id)].nx_interface_ip_address,
                   sizeof(client_ip.nx_ip_interface[!(tcp_ip_cb.netx_ap_intf_id)].nx_interface_ip_address));
        //! memsetting the network mask
        sl_memzero(&client_ip.nx_ip_interface[!(tcp_ip_cb.netx_ap_intf_id)].nx_interface_ip_network_mask,
                   sizeof(client_ip.nx_ip_interface[!(tcp_ip_cb.netx_ap_intf_id)].nx_interface_ip_network_mask));
#endif /*DATA_PATH_UMAC_ENABLE*/
      }
    }
  } else if (SL_PKT_TX_HDESC_GET_DW0_FRAME_TYPE(txPkt) == SLI_WLAN_BGSCAN) {
    scan_res = sl_get_scan_results(mgmt_if_adapter.supplicant_priv);
    wpa_scan_results_free_wrapper(scan_res);
  } else if (SL_PKT_TX_HDESC_GET_DW0_FRAME_TYPE(txPkt) == SLI_WLAN_REQ_SCAN) /* SCAN */
  {
    scan_cb = (rsi_uScan_non_rom_t *)scatter_buf0_addr;
#ifdef ENABLE_AP_MODE_SCAN
    if (mgmt_if_adapter.operating_mode == WISE_MODE_AP) {
      rsi_uBgscan_t bg_scan;
      sl_memzero(&bg_scan, sizeof(rsi_uBgscan_t));

      //! Enable instant bg scan as scan should be fresh and result should go to
      //! host
      mgmt_if_adapter.instant_bg_flag = SL_TRUE;
      bg_scan.bgscan_enable           = 1;
      bg_scan.instant_bg_enable       = 1;

      sl_memset(bg_scan.ssid, 0, sizeof(bg_scan.ssid));

      //! Fill scan ssid from scan command
      bg_scan.ssid_len = sl_strlen((char *)scan_cb->scanFrameSnd.ssid);
      if (bg_scan.ssid_len > 0) {
        sl_memcpy(bg_scan.ssid, scan_cb->scanFrameSnd.ssid, bg_scan.ssid_len);

        // copy scan ssid into bgscan
        mgmt_if_adapter.bgscan_ssid_len = bg_scan.ssid_len;
        sl_memcpy((char *)mgmt_if_adapter.bgscan_ssid,
                  (char *)scan_cb->scanFrameSnd.ssid,
                  mgmt_if_adapter.bgscan_ssid_len);
      }

      //! Fill channel bitmap
      if (scan_cb->scanFrameSnd.scan_channel) {
        /* Validate the channel */
        if ((mgmt_if_adapter.current_freq_band == SL_BAND_5GHZ
               ? reverse_translate_channel(scan_cb->scanFrameSnd.scan_channel)
               : scan_cb->scanFrameSnd.scan_channel)
            > WISE_MAX_CHANNEL(mgmt_if_adapter.current_freq_band)) {
          sl_mgmt_indicate_to_host(SLI_WLAN_RSP_SCAN, 0, WISE_ERROR_WRONG_SCAN_INFO, NULL);
          return;
        }
        if ((mgmt_if_adapter.current_freq_band == SL_BAND_5GHZ)
            && (!(mgmt_if_adapter.custom_feature_bit_map & FEAT_SEL_DFS_CHANNEL_SUPPORT))) {
          if ((scan_cb->scanFrameSnd.scan_channel > 48 && scan_cb->scanFrameSnd.scan_channel < 149)
              || (scan_cb->scanFrameSnd.scan_channel < 36 || scan_cb->scanFrameSnd.scan_channel > 165)) {
            sl_mgmt_indicate_to_host(SLI_WLAN_RSP_SCAN, 0, FREQUENCY_NOT_SUPPORTED, NULL);
            return;
          }
        }
        if (mgmt_if_adapter.current_freq_band == SL_BAND_5GHZ) {
          bg_scan.channel_bitmap = BIT(reverse_translate_channel(scan_cb->scanFrameSnd.scan_channel) - 1);
        } else {
          bg_scan.channel_bitmap = BIT(scan_cb->scanFrameSnd.scan_channel - 1);
        }
      } else {
        if (!scan_cb->scanFrameSnd.channel_bit_mask[0]) {
          (*(uint16 *)&scan_cb->scanFrameSnd.channel_bit_mask[0]) = 0x3FFF;
        }
        if (!(*(uint32 *)&scan_cb->scanFrameSnd.channel_bit_mask[1])) {
#ifdef DISABLE_CH144
          (*(uint32 *)&scan_cb->scanFrameSnd.channel_bit_mask[1]) = 0xFFFFFF;
#else
          (*(uint32 *)&scan_cb->scanFrameSnd.channel_bit_mask[1]) = 0x1FFFFFF;
#endif
        }
        if (mgmt_if_adapter.current_freq_band == SL_BAND_5GHZ) {
          bg_scan.channel_bitmap = (*(uint32 *)&scan_cb->scanFrameSnd.channel_bit_mask[1]);
        } else {
          bg_scan.channel_bitmap = (*(uint16 *)&scan_cb->scanFrameSnd.channel_bit_mask[0]);
        }
      }

      //! Clear the scan list
      scan_res = sl_get_scan_results(mgmt_if_adapter.supplicant_priv);
      if (scan_res == NULL) {
        // asm("debug");
      }
      wpa_scan_results_free_wrapper((void *)scan_res);

      mgmt_hm_send_bgscan_cmd(&bg_scan);

      return;
    }
#endif
#ifdef ENABLE_DRAEGER_CUSTOMIZATION
    /*This is the case where configured AP is not found*/
    mgmt_if_adapter.async_state_code  = RSI_MODULE_STATE_START_UP_OR_NOT_AVAVILABLE;
    mgmt_if_adapter.async_reason_code = RSI_MODULE_STATE_START_UP_OR_NOT_AVAVILABLE;
    mgmt_if_adapter.async_rssi        = RSI_MODULE_STATE_RSSI_NOT_AVAILABLE;
    mgmt_if_adapter.async_channel     = RSI_MODULE_STATE_START_UP_OR_NOT_AVAVILABLE;
    sl_memzero(mgmt_if_adapter.async_bssid, MAC_ADDR_LEN);

    if (mgmt_if_adapter.custom_feature_bit_map & FEAT_SEL_ENABLE_ASYNC_MESSAGE) {
      wise_module_state(RSI_MODULE_STATE_SCANNING);
    }
#endif
    /*Dual band support handling*/
    if (mgmt_if_adapter.dual_band == SL_BAND_DUAL) {
      if (scan_cb->scanFrameSnd.scan_channel > 14 && mgmt_if_adapter.current_freq_band != SL_BAND_5GHZ) {
        mgmt_if_adapter.current_freq_band = SL_BAND_5GHZ;
      } else if (scan_cb->scanFrameSnd.scan_channel <= 14 && mgmt_if_adapter.current_freq_band != SL_BAND_2P4GHZ) {
        mgmt_if_adapter.current_freq_band = SL_BAND_2P4GHZ;
      }

      //! To sink SME and UMAC band, need to send switch band each time
      mgmt_hm_switch_band(&mgmt_if_adapter.current_freq_band);
    }
    if ((scan_cb->scanFrameSnd.scan_channel != 0)) {
#ifdef CONFIG_IEEE_80211J
      if ((!(mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_IEEE_80211J))
          || ((mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_IEEE_80211J)
              && ((mgmt_if_adapter.region_code != REGION_JAP_NUM)))) {
#endif
        /* Validate the channel */
        if ((mgmt_if_adapter.current_freq_band == SL_BAND_5GHZ
               ? reverse_translate_channel(scan_cb->scanFrameSnd.scan_channel)
               : scan_cb->scanFrameSnd.scan_channel)
            > WISE_MAX_CHANNEL(mgmt_if_adapter.current_freq_band)) {
          sl_mgmt_indicate_to_host(SLI_WLAN_RSP_SCAN, 0, WISE_ERROR_WRONG_SCAN_INFO, NULL);
          return;
        }
        if ((mgmt_if_adapter.current_freq_band == SL_BAND_5GHZ)
            && (!(mgmt_if_adapter.custom_feature_bit_map & FEAT_SEL_DFS_CHANNEL_SUPPORT))) {
          if ((scan_cb->scanFrameSnd.scan_channel > 48 && scan_cb->scanFrameSnd.scan_channel < 149)
              || (scan_cb->scanFrameSnd.scan_channel < 36 || scan_cb->scanFrameSnd.scan_channel > 165)) {
            sl_mgmt_indicate_to_host(SLI_WLAN_RSP_SCAN, 0, FREQUENCY_NOT_SUPPORTED, NULL);
            return;
          }
        }
#ifdef CONFIG_IEEE_80211J
      } else {
        //! FIXME
        if (!(valid_80211J_channel(scan_cb->scanFrameSnd.scan_channel))) {
          sl_mgmt_indicate_to_host(SLI_WLAN_RSP_SCAN, 0, FREQUENCY_NOT_SUPPORTED, NULL);
          return;
        }
      }
#endif

      freqs = (int *)sl_zalloc(sizeof(int) * (1 + 1));
      if (freqs == NULL) {
        /* Send scan response from here */
        sl_mgmt_indicate_to_host(SLI_WLAN_RSP_SCAN, 0, MEM_ALLOC_FAILED, NULL);
        return;
      } else {
        freqs[0]               = get_frequency(mgmt_if_adapter.current_freq_band,
                                 scan_cb->scanFrameSnd.scan_channel,
                                 ((mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_IEEE_80211J)
                                  && (mgmt_if_adapter.region_code == REGION_JAP_NUM)));
        freqs[1]               = 0;
        wpa_s->next_scan_freqs = freqs;
      }
      if (mgmt_if_adapter.current_freq_band == SL_BAND_5GHZ) {
        mgmt_if_adapter.scan_channel_bitmap_5_GHz =
          BIT(reverse_translate_channel(scan_cb->scanFrameSnd.scan_channel) - 1);
        mgmt_if_adapter.scan_channel_bitmap_2_4_GHz = 0;
      }
#ifdef CONFIG_IEEE_80211J
      else if ((mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_IEEE_80211J)
               && (mgmt_if_adapter.region_code == REGION_JAP_NUM)) {
        mgmt_if_adapter.scan_channel_bitmap_5_GHz =
          BIT(reverse_translate_80211j_channel(scan_cb->scanFrameSnd.scan_channel) - 1);
      }
#endif
      else {
        mgmt_if_adapter.scan_channel_bitmap_2_4_GHz = BIT(scan_cb->scanFrameSnd.scan_channel - 1);
        mgmt_if_adapter.scan_channel_bitmap_5_GHz   = 0;
      }
    } else {
      /*update scan bitmap every time if scan channel is zero*/
      // if (*(uint32 *)scan_cb->scanFrameSnd.channel_bit_mask /*&& (host_if !=
      // UART) && (host_if != USB_CDC)*/)
      {
        if (!scan_cb->scanFrameSnd.channel_bit_mask[0]) {
          scan_cb->scanFrameSnd.set_scan_channel_bitmask_from_user = 0;
          if (mgmt_if_adapter.rsi_region_code_from_host == 1) {
            /* For US region, upto 11-channels */
            if (mgmt_if_adapter.region_code == 1) {
              (*(uint16 *)&scan_cb->scanFrameSnd.channel_bit_mask[0]) = 0x7FF;
            }
            // For EU, KR and CN(SRRC) region, upto 13-channels
            else if ((mgmt_if_adapter.region_code == 2) || (mgmt_if_adapter.region_code == 5)
                     || (mgmt_if_adapter.region_code == REGION_CN_NUM)) {
              (*(uint16 *)&scan_cb->scanFrameSnd.channel_bit_mask[0]) = 0x1FFF;
            }
            /* For World mode and JAPAN mode upto 14-channels */
            else {
              (*(uint16 *)&scan_cb->scanFrameSnd.channel_bit_mask[0]) = 0x3FFF;
            }
          } else /* default, all channels */
          {
            (*(uint16 *)&scan_cb->scanFrameSnd.channel_bit_mask[0]) = 0x3FFF;
          }
        } else {
          scan_cb->scanFrameSnd.set_scan_channel_bitmask_from_user = 1;
        }
        if (!(*(uint32 *)&scan_cb->scanFrameSnd.channel_bit_mask[1])) {
#ifdef DISABLE_CH144
          (*(uint32 *)&scan_cb->scanFrameSnd.channel_bit_mask[1]) = 0xFFFFFF;
#else
          (*(uint32 *)&scan_cb->scanFrameSnd.channel_bit_mask[1]) = 0x1FFFFFF;
#endif
#ifdef CONFIG_IEEE_80211J
          if ((mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_IEEE_80211J)
              && (mgmt_if_adapter.region_code == REGION_JAP_NUM)) {
            (*(uint32 *)&scan_cb->scanFrameSnd.channel_bit_mask[1]) = 0x7F;
          }
#endif
        }
        if (mgmt_if_adapter.dual_band == SL_BAND_DUAL) {
          mgmt_if_adapter.scan_channel_bitmap_2_4_GHz = (*(uint16 *)&scan_cb->scanFrameSnd.channel_bit_mask[0]);
          mgmt_if_adapter.scan_channel_bitmap_5_GHz   = (*(uint32 *)&scan_cb->scanFrameSnd.channel_bit_mask[1]);
        } else if (mgmt_if_adapter.current_freq_band == SL_BAND_5GHZ) {
          mgmt_if_adapter.scan_channel_bitmap_5_GHz   = (*(uint32 *)&scan_cb->scanFrameSnd.channel_bit_mask[1]);
          mgmt_if_adapter.scan_channel_bitmap_2_4_GHz = 0;
        }

        else {
          mgmt_if_adapter.scan_channel_bitmap_2_4_GHz = (*(uint16 *)&scan_cb->scanFrameSnd.channel_bit_mask[0]);
          mgmt_if_adapter.scan_channel_bitmap_5_GHz   = 0;
        }
      }
    }

    if (scan_cb->scanFrameSnd.pscan_bitmap != 0) {
      mgmt_if_adapter.passive_scan_bitmap = scan_cb->scanFrameSnd.pscan_bitmap;
    }

    ssid_len = sl_strlen((char *)scan_cb->scanFrameSnd.ssid);
    if (!scan_cb->scanFrameSnd.scan_channel || !ssid_len) {
      scan_cb->scanFrameSnd.scan_feature_bitmap &= ~FEATURE_SELECT_QUICK_SCAN;
    }
    mgmt_if_adapter.scan_feature_bitmap = scan_cb->scanFrameSnd.scan_feature_bitmap;
    mgmt_if_adapter.scan_ssid_len       = ssid_len;

    mgmt_if_adapter.ssid_len = 0;
    if (ssid_len > 0) {
      sl_memset(mgmt_if_adapter.bssid, 0xFF, 6);
      if (scan_cb->scanFrameSnd.scan_feature_bitmap & FEATURE_BSSID_BASED_SCAN /*FEATURE_SCAN_SUPPLICANT_BYPASS*/) {
        sl_memcpy((char *)mgmt_if_adapter.bssid, (char *)scan_cb->scanFrameSnd.bssid, 6);
      }
      sl_memset(mgmt_if_adapter.ssid, 0, 33);
      sl_memcpy((char *)mgmt_if_adapter.ssid, (char *)scan_cb->scanFrameSnd.ssid, ssid_len);
      mgmt_if_adapter.ssid_len = ssid_len;
      sl_memset(mgmt_if_adapter.scan_ssid, 0, SL_SSID_LEN);
      sl_memcpy(mgmt_if_adapter.scan_ssid, scan_cb->scanFrameSnd.ssid, SL_SSID_LEN);
    }

    wpa_s->scan_req                    = 1;
    mgmt_if_adapter.scan_req_from_host = 1;
    //! Added check for clearing scan results in concurrent mode if ap connected
    //! first
    if (!mgmt_if_adapter.connected || (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE)) {
      scan_res = rsi_get_scan_results(wpa_s->drv_priv);
      if (scan_res == NULL) {
        // asm("debug");
      }
      wpa_scan_results_free_wrapper((void *)scan_res);
    }
    wpa_supplicant_req_scan_wrapper(wpa_s, 0, 0);
  } else if (SL_PKT_TX_HDESC_GET_DW0_FRAME_TYPE(txPkt) == SLI_WLAN_REQ_JOIN) {
    join_cb = (rsi_uJoin *)scatter_buf0_addr;
    if ((join_cb == NULL) || CHECK_JOIN_DATARATE(join_cb->joinFrameSnd.dataRate)
        || (!(mgmt_if_adapter.set_region_given
              && (join_cb->joinFrameSnd.join_feature_bitmap & JOIN_FEAT_KEY_MGMT_WPA_CCXV2))
            && CHECK_JOIN_POWERLEVEL(join_cb->joinFrameSnd.powerLevel)) /*||(rsi_strlen(join_cb->joinFrameSnd.ssid)
                                                                        == 0)*/
        || ((mgmt_if_adapter.current_freq_band == SL_BAND_5GHZ) && join_cb->joinFrameSnd.dataRate
            && (join_cb->joinFrameSnd.dataRate < 5))) {
      /* Join information is not sent ERROR_CODE */
      /* Send join response from here ERROR message */
      sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, JOIN_INFO_NOT_SENT, NULL);
      return;
    }

    uint8 current_opermode;
    if (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) {
      if (join_cb->joinFrameSnd.vap_id != 0) {
        if ((mgmt_if_adapter.state >= WISE_STATE_CONNECTED)
            && (mgmt_if_adapter.join_channel != mgmt_if_adapter.apconfig.channel_no)) {
          sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, CONCURRENT_AP_CHANNEL_MISMATCH, NULL);
          return;
        }
        current_opermode                   = WISE_MODE_AP;
        mgmt_if_adapter.con_operating_mode = WISE_MODE_AP;
        wpa_s                              = (struct wpa_supplicant *)mgmt_if_adapter.ap_supplicant_priv;
      } else {
        current_opermode                   = WISE_MODE_CONNECTION;
        mgmt_if_adapter.con_operating_mode = WISE_MODE_CONNECTION;
        wpa_s                              = (struct wpa_supplicant *)mgmt_if_adapter.supplicant_priv;
      }
    } else {
      current_opermode = mgmt_if_adapter.operating_mode;
      wpa_s            = (struct wpa_supplicant *)mgmt_if_adapter.supplicant_priv;
    }

    if (current_opermode == WISE_MODE_CONNECTION || current_opermode == WISE_MODE_EAP) {
      //! copy join ssid into bgscan
      mgmt_if_adapter.bgscan_ssid_len = sl_strlen((char *)join_cb->joinFrameSnd.ssid);
      sl_memcpy((char *)mgmt_if_adapter.bgscan_ssid,
                (char *)join_cb->joinFrameSnd.ssid,
                mgmt_if_adapter.bgscan_ssid_len);
    } else {
      if ((mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) && current_opermode != WISE_MODE_AP) {
        mgmt_if_adapter.bgscan_ssid_len = 0;
        sl_memset((char *)mgmt_if_adapter.bgscan_ssid, 0, sizeof(mgmt_if_adapter.bgscan_ssid));
      }
    }

    if (current_opermode == WISE_MODE_CONNECTION) {
      switch (join_cb->joinFrameSnd.securityType) {
        case SEC_MODE_WEP: {

          if (!(sl_strlen((char *)mgmt_if_adapter.wep_key.key[0]) || sl_strlen((char *)mgmt_if_adapter.wep_key.key[1])
                || sl_strlen((char *)mgmt_if_adapter.wep_key.key[2])
                || sl_strlen((char *)mgmt_if_adapter.wep_key.key[3]))) /* For WEP */
          {
            sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, WEP_KEY_NOT_GIVEN, NULL);
            return;
          }
        } break;
        case SEC_MODE_WPA:
        case SEC_MODE_WPA2:
        case SEC_MODE_WPA3:
        case SEC_MODE_MIXED_MODE:
        case SEC_MODE_WPA3_TRANSITION: {
          tmp_psk_len = sl_strlen((char *)join_cb->joinFrameSnd.psk);
          if (tmp_psk_len) {
            if ((tmp_psk_len < 8) || (tmp_psk_len > 63)) {
              sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, WISE_ERROR_WRONG_LEN_PSK, NULL);
              return;
            }
            mgmt_if_adapter.pmk_stored = SL_FALSE;
            sl_strcpy((uint8 *)mgmt_if_adapter.psk, join_cb->joinFrameSnd.psk);
            sl_memcpy(mgmt_if_adapter.psk, join_cb->joinFrameSnd.psk, SL_PSK_LEN);
          } else {
            sl_memset(pmk_null, 0, SL_PMK_LEN);
            if (((mgmt_if_adapter.operating_mode == WISE_MODE_CONNECTION)
                 || ((mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE)
                     && (mgmt_if_adapter.con_operating_mode == WISE_MODE_CONNECTION)))
                && (sl_memcmp(mgmt_if_adapter.pmk, pmk_null, SL_PMK_LEN))) {
              mgmt_if_adapter.pmk_stored = SL_TRUE;
            }
          }
          if ((!mgmt_if_adapter.pmk_stored) && (!sl_strlen((char *)mgmt_if_adapter.psk))) {
            sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, PSK_OR_PMK_NOT_GIVEN, NULL);
            return;
          }
        } break;
        case SEC_MODE_OPEN:
          break;
        default: {
          sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, INVALID_SECURITY_MODE, NULL);
          return;
        }
      }
    }

    mgmt_if_adapter.join_data_rate   = join_cb->joinFrameSnd.dataRate;
    mgmt_if_adapter.join_power_level = join_cb->joinFrameSnd.powerLevel;

    /* JOIN */
    //! Check for AP mode
    if (join_cb->joinFrameSnd.vap_id == 1) {
      mgmt_if_adapter.join_feature_bitmap_concurrent_ap = join_cb->joinFrameSnd.join_feature_bitmap;
    } else {
      sl_memset(mgmt_if_adapter.join_ssid, 0, SL_SSID_LEN);
      sl_strcpy(mgmt_if_adapter.join_ssid, join_cb->joinFrameSnd.ssid);
      mgmt_if_adapter.csec_mode = join_cb->joinFrameSnd.securityType;

      mgmt_if_adapter.listen_interval     = join_cb->joinFrameSnd.listen_interval;
      mgmt_if_adapter.join_feature_bitmap = join_cb->joinFrameSnd.join_feature_bitmap;
    }

    if (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) {
      if ((current_opermode == WISE_MODE_CONNECTION)
          && (mgmt_if_adapter.join_feature_bitmap & JOIN_FEAT_MAC_BASED_JOIN)) {
        if (sl_memcmp(join_cb->joinFrameSnd.join_bssid, null_bssid, 6) == 0) {
          sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, MAC_BASED_JOIN_MAC_NOT_PRESENT, NULL);
          return;
        }
        sl_memcpy(mgmt_if_adapter.join_bssid, join_cb->joinFrameSnd.join_bssid, 6);
      }
    } else {
      if (mgmt_if_adapter.join_feature_bitmap & JOIN_FEAT_MAC_BASED_JOIN) {
        if (sl_memcmp(join_cb->joinFrameSnd.join_bssid, null_bssid, 6) == 0) {
          sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, MAC_BASED_JOIN_MAC_NOT_PRESENT, NULL);
          return;
        }

        sl_memcpy(mgmt_if_adapter.join_bssid, join_cb->joinFrameSnd.join_bssid, 6);
      } else {
        sl_memzero(mgmt_if_adapter.join_bssid, 6);
      }
    }

    if (!(mgmt_if_adapter.join_feature_bitmap & JOIN_FEAT_LISTEN_INTERVAL_VALID))
      join_cb->joinFrameSnd.listen_interval = 0;
    /*sl_memset the array to compare with join ssid to differentiate between WPS
     * and normal station*/
    sl_memset(tmp_ssid, 0, 34);

    if (memcmp(join_cb->joinFrameSnd.ssid, tmp_ssid, 34) != 0) {
      if (current_opermode == WISE_MODE_CONNECTION || current_opermode == WISE_MODE_EAP) {
        /*follow this if not in WPS mode*/
        set_wpa_auto_reconnect_disabled(wpa_s, 1); /*Default retry is disabled*/
        if (get_first_time_retry_enable())
          set_wpa_auto_reconnect_disabled(wpa_s, 0); /*Only retry if host intended to do so*/
      }
      mgmt_if_adapter.wps_session = SL_FALSE;
    } else {
      if (current_opermode == WISE_MODE_CONNECTION) {
        mgmt_if_adapter.wps_session = SL_TRUE;
      }
    }

    if (current_opermode == WISE_MODE_CONNECTION) {
      mgmt_if_adapter.join_from_host = 1;
#ifdef CONFIG_IEEE_80211W
      if (mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_IEEE_80211W)
        pmf = ((join_cb->joinFrameSnd.join_feature_bitmap & 0x60)
                 ? ((join_cb->joinFrameSnd.join_feature_bitmap & 0x40) ? (MGMT_FRAME_PROTECTION_REQUIRED)
                                                                       : (MGMT_FRAME_PROTECTION_OPTIONAL))
                 : 0);
      wpa_update_conf_pmf(wpa_s, pmf);
#endif
      if (memcmp(join_cb->joinFrameSnd.ssid, tmp_ssid, 34) == 0) {
#ifdef EXCLUDE_WPS
        sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, NOT_SUPPORTED, NULL);
        return;
#else
        if (mgmt_if_adapter.feature_bit_map & FEAT_WPS_DISABLE) {
          sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, NOT_SUPPORTED, NULL);
          return;
        }
        if (eloop_is_timeout_registered_wrapper(wpas_wps_timeout_wrapper, wpa_s, NULL)) {

          sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, PBC_OVERLAP, NULL);
          return;
        }
        start_wps_sta(wpa_s);
        sl_set_wps_client(SL_TRUE);
#endif
      } else {

        if (((join_cb->joinFrameSnd.securityType == SEC_MODE_WEP)
             && !sl_strlen((char *)mgmt_if_adapter.wep_key.key[mgmt_if_adapter.wep_key.index]))) {
          sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, WEP_KEY_NOT_GIVEN, NULL);
          return;
        }

        ret_val = start_sta(wpa_s,
                            (uint8 *)mgmt_if_adapter.psk,
                            join_cb->joinFrameSnd.ssid,
                            join_cb->joinFrameSnd.securityType);

        if (ret_val != 0) {
          SL_PRINTF(WLAN_SUPP_JOIN_FAIL_1, WLAN_UMAC, LOG_TRACE, "ret_val : %d", ret_val);
          sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, ret_val, NULL);
        } else {
          /* NULL has to be reviewed - when to indicate the new results */
          wpa_supplicant_event_wrapper(wpa_s, EVENT_SCAN_RESULTS, NULL);
        }
      }
    }
#ifdef CONFIG_P2P
    else if (current_opermode == WISE_MODE_P2P) {
      mgmt_if_adapter.join_from_host = 1;
      /* registering a timer to check the scan results for required device */
      eloop_cancel_timeout_wrapper(rsi_wfd_join_timed_out, NULL, NULL);
      eloop_register_timeout_wrapper(120, 0, rsi_wfd_join_timed_out, NULL, NULL);

      /* If it is GO */
      if ((wsc->connected == 1) && (wsc->client == 0)) {
        /* Handle return type here and convert to appropriate error ERROR_CODE
         */
        /* -1 - Not AP interface; -2 - PBC overlap */
        ret_val = start_wps_push_button();
        if (ret_val != 0) {
          ret_val = (ret_val == -1) ? NOT_AP_INTERFACE : PBC_OVERLAP;
          SL_PRINTF(WLAN_SUPP_JOIN_FAIL_2, WLAN_UMAC, LOG_TRACE, "ret_val : %d", ret_val);
          sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, ret_val, NULL);
        } else {
          go = wsc->client ? 'C' : 'G';
          sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 1, WLAN_STATUS_SUCCESS, &go);
        }
      } else if ((*(uint16 *)(wsc->sc_params.go_intent)) <= 15) {
        if ((ret_val = wise_p2p_connect(wpa_s, join_cb->joinFrameSnd.ssid)) != 0) {
          if (ret_val != P2P_DEV_NOT_FOUND) {
            /* 25 - P2P device not found; -3 - frequency not supportd; any other
             * - unable start GO neg ERROR_CODE */
            ret_val = (ret_val == -3) ? FREQUENCY_NOT_SUPPORTED : UNABLE_START_GO_NEG;
          }
          /* Send join response from here ERROR message */
          SL_PRINTF(WLAN_SUPP_JOIN_FAIL_3, WLAN_UMAC, LOG_TRACE, "ret_val : %d", ret_val);
          sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, ret_val, NULL);
        }
      } else if ((*(uint16 *)wsc->sc_params.go_intent) == WISE_AUTONOMOUS_GO) {
        if (mgmt_if_adapter.state != WISE_STATE_CONNECTED) {
          /**
           * wpas_p2p_group_add - Add a new P2P group with local end as Group
           * Owner
           * @wpa_s: Pointer to wpa_supplicant data from
           * wpa_supplicant_add_iface()
           * @persistent_group: Whether to create a persistent group
           * @freq: Frequency for the group or 0 to indicate no hardcoding
           * Returns: 0 on success, -1 on failure
           *
           * This function creates a new P2P group with the local end as the
           * Group Owner, i.e., without using Group Owner Negotiation.
           */
          if (sl_strcmp((char *)join_cb->joinFrameSnd.ssid, (char *)wsc->sc_params.device_name)
              || (wpas_p2p_group_add(mgmt_if_adapter.supplicant_priv, 0, 0, 0, 0, 0, 0, 0) != 0)) {
            sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, WISE_ERROR_UNABLE_TO_FORM_AUTOGO, NULL);
          }
        }
      }
      return;
    }
#endif
    else if ((current_opermode == WISE_MODE_AP)) {
      /*Check for Fixed rate*/
      if (join_cb->joinFrameSnd.dataRate && check_valid_fixed_rate_ap(join_cb->joinFrameSnd.dataRate)) {
        sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, 0x33 /*INVALID_FIXED_RATE*/, NULL);
        return;
      }
      if (mgmt_if_adapter.apconfig.security_type == SEC_MODE_WPA3) {
        /*To use H2E for PWE derivation by default
         0- Hunting and pecking only(H&P).
         1- Hash to element only(H2E).
         2- both H2E and H&P.
         */
        wpa_update_conf_sae_pwe(wpa_s, 1);
      }

      if (mgmt_if_adapter.apconfig.security_type == SEC_MODE_WPA3
          || mgmt_if_adapter.apconfig.security_type == SEC_MODE_WPA3_TRANSITION) {
        // Override encryption type for WPA3 modes since only CCMP is allowed
        mgmt_if_adapter.apconfig.encryp_mode = (mgmt_if_adapter.apconfig.encryp_mode & 0xf0);
        mgmt_if_adapter.apconfig.encryp_mode |= ENCRYPTION_TYPE_CCMP;
      }

      /* If it is GO */
      if (((!(mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE)
            && ((mgmt_if_adapter.connected == SL_TRUE) && (mgmt_if_adapter.client == SL_FALSE)))
           || ((mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) && (wpa_s->wpa_state == WPA_COMPLETED)))
          && !sl_strcmp(join_cb->joinFrameSnd.ssid, mgmt_if_adapter.apconfig.ssid)) {
        /* Handle return type here and convert to appropriate error ERROR_CODE
         */
        /* -1 - Not AP interface; -2 - PBC overlap */
#ifdef EXCLUDE_WPS
        sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, NOT_SUPPORTED, NULL);
        return;
#else
        if (mgmt_if_adapter.feature_bit_map & FEAT_WPS_DISABLE) {
          sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, SLI_WLAN_WPS_NOT_SUPPORTED, NULL);
          return;
        }
#ifndef EXCLUDE_AP
        ret_val = start_wps_push_button();
        if (ret_val != 0) {
          ret_val = (ret_val == -1) ? NOT_AP_INTERFACE : PBC_OVERLAP;
          SL_PRINTF(WLAN_SUPP_JOIN_FAIL_4, WLAN_UMAC, LOG_TRACE, "ret_val : %d", ret_val);
          sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, ret_val, NULL);
        } else {
          go = mgmt_if_adapter.client ? 'C' : 'G';
          sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 1, WLAN_STATUS_SUCCESS, &go);
        }
#endif
#endif
      } else {
        if (!sl_strcmp(join_cb->joinFrameSnd.ssid, mgmt_if_adapter.apconfig.ssid)) {
          wpas_start_wps_apconf(wpa_s);
        } else {
          sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, WISE_ERROR_UNABLE_TO_FORM_AUTOGO, NULL);
        }
      }
    }
#ifndef EXCLUDE_EAP
    else if (current_opermode == WISE_MODE_EAP) {
      mgmt_if_adapter.join_from_host = 1;
      if (mgmt_if_adapter.okc & BIT(0)) {
        wpa_update_conf_okc(wpa_s, 1);
      }
      tmp_ssid[0] = '\"';
      ssid_len    = sl_strlen((char *)join_cb->joinFrameSnd.ssid);
      sl_memcpy(&tmp_ssid[1], join_cb->joinFrameSnd.ssid, ssid_len);
      tmp_ssid[ssid_len + 1] = '\"';
      tmp_ssid[ssid_len + 2] = '\0';

#ifdef CONFIG_IEEE_80211W
      if (mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_IEEE_80211W)
        pmf = ((join_cb->joinFrameSnd.join_feature_bitmap & 0x60)
                 ? ((join_cb->joinFrameSnd.join_feature_bitmap & 0x40) ? (MGMT_FRAME_PROTECTION_REQUIRED)
                                                                       : (MGMT_FRAME_PROTECTION_OPTIONAL))
                 : 0);
      wpa_update_conf_pmf(wpa_s, pmf);
#endif
      /* Handle join error code here ERROR_CODE */
      ret_val = start_enterprise_security(wpa_s, NULL, tmp_ssid, join_cb->joinFrameSnd.securityType);
      if (ret_val != 0) {
        SL_PRINTF(WLAN_SUPP_JOIN_FAIL_5, WLAN_UMAC, LOG_TRACE, "ret_val : %d", ret_val);
        sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, ret_val, NULL);
      } else {
        /* NULL has to be reviewed - when to indicate the new results */
        wpa_supplicant_event_wrapper(wpa_s, EVENT_SCAN_RESULTS, NULL);
      }
    }
#endif
    else {
      /* INVALID oper mode ERROR_CODE */
      sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, INVALID_OPER_MODE, NULL);
    }
  }
}

#ifdef CONFIG_P2P
/* Copies the user given PSK for GO & Autonomous GO */
void rsi_copy_psk(void *go_params)
{
  struct p2p_go_neg_results *params = (struct p2p_go_neg_results *)go_params;
  if (sl_strlen((char *)rsi_wsc_cb.psk) > 0) {
    sl_strcpy(params->passphrase, (char *)rsi_wsc_cb.psk);
  } else {
    sl_strcpy((char *)rsi_wsc_cb.psk, params->passphrase);
  }
}
#endif

void driver_listen_done(void *eloop_ctx, void *timeout_ctx)
{
  union wpa_event_data data;

  sl_memset((uint8 *)&data, 0, sizeof(union wpa_event_data));
  // data.remain_on_channel.freq = listen_freq; //TODO freq should be filled
  // here
  wpa_supplicant_event_wrapper(mgmt_if_adapter.supplicant_priv, EVENT_CANCEL_REMAIN_ON_CHANNEL, &data);
}

int rsi_set_ListenMode(void *priv, int report)
{
  listen_mode_t listen_mode;

  /* Don't send if it is not P2P mode */
  if (mgmt_if_adapter.operating_mode != WISE_MODE_P2P)
    return -1;

  listen_mode.probe_report = report;
  return (int)mgmt_hm_set_ListenMode(&listen_mode);
}

int rsi_set_channel(void *priv, unsigned int freq, unsigned int duration)
{
  int retval;
  set_channel_t set_channel;
  unsigned int secs  = 0;
  unsigned int usecs = 0;
  /* Don't send if it is not P2P mode */
  if (mgmt_if_adapter.operating_mode != WISE_MODE_P2P)
    return -1;
  if (duration) {
    secs  = duration / 1000; /* duration will be in milli seconds */
    usecs = (duration % 1000) * 1000;
  }
  if ((freq >= 5180) && (mgmt_if_adapter.band == SL_BAND_DUAL)) {
    mgmt_if_adapter.current_freq_band = SL_BAND_5GHZ;
    mgmt_hm_switch_band(&mgmt_if_adapter.current_freq_band);
  }
  set_channel.channel_number                    = get_channel_num((uint16)freq);
  mgmt_if_adapter.dbg_host_intf->channel_number = set_channel.channel_number;
  retval                                        = (int)mgmt_hm_set_channel(&set_channel);

  if (duration) {
    /* registering a timer in order to imitate the listen done that should come
     * from driver */
    eloop_cancel_timeout_wrapper(driver_listen_done, priv, NULL);
    eloop_register_timeout_wrapper(secs, usecs, driver_listen_done, priv, NULL);
  }

  return retval;
}

void wise_report_fail_response(void *eloop_ctx, void *timeout_ctx)
{
  sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, JOIN_TIMED_OUT, NULL);
}

void wise_report_fail_response_group(void *eloop_ctx, void *timeout_ctx)
{
  sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, SLI_WLAN_FINITE_CYCLIC_GROUP_NOT_SUPPORTED, NULL);
}

void wise_report_fail_response_auth(void *eloop_ctx, void *timeout_ctx)
{
  sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, WPA3_SAE_CONFIRM_RECEIVED_AFTER_AUTH_COMPLETE, NULL);
}
#endif // SUPPLICANT_NON_ROM

STATIC int rsi_disconnect_wlan(void *priv, const uint8 *mac_addr, int reason_code)
{
  int16 retval;
  disconnect_wlan_t disconnect_wlan;
  struct wpa_supplicant *wpa_s;
  mgmt_if_adapter.dbg_host_intf->disconnect_wlan_count++;
  wpa_s = priv ? mgmt_if_adapter.supplicant_priv : NULL;

  if (mgmt_if_adapter.host_cmd == SLI_WLAN_REQ_AP_STOP) {
    // Disconnection for AP Stop is handled in SME. No need to execute this
    // function.
    return -1;
  }
  if (wpa_s == NULL) {
    return -1;
  }

  sl_memset(&disconnect_wlan, 0, sizeof(disconnect_wlan_t));

  retval = mgmt_hm_send_disconnect(&disconnect_wlan, ((is_wps_client()) && (!wpa_s->wps_success)), 0);

  if ((mgmt_if_adapter.host_cmd == SLI_WLAN_REQ_JOIN) && !get_first_time_retry_enable()) {
    mgmt_if_adapter.host_cmd = 0;
    switch (reason_code) {
      case WLAN_STATUS_FINITE_CYCLIC_GROUP_NOT_SUPPORTED:
        eloop_register_timeout_wrapper(0, 0, wise_report_fail_response_group, priv, NULL);
        break;
      case WLAN_STATUS_UNKNOWN_AUTH_TRANSACTION:
        eloop_register_timeout_wrapper(0, 0, wise_report_fail_response_auth, priv, NULL);
        break;
      default:
        if (!((wpa_s) && wpa_s->key_mgmt == WPA_KEY_MGMT_WPS && wpa_s->wps_success)) {
          /* Send join response from here ERROR message */
          /* Defer sending FAIL rsp since it is de-init'ing the supp and after
         * this control is going back and accessing the supplicant related
         * structures/points, which is leading to reset of the module */
          if (mgmt_if_adapter.operating_mode == WISE_MODE_EAP && (wpa_s->eap_expected_failure == 1)) {
            //! This is expected failure like WPS and P2P, so donot through error
            //! to host
          } else {
            eloop_register_timeout_wrapper(0, 0, wise_report_fail_response, priv, NULL);
          }
        }
    }
  }
  return retval;
}

#ifdef SUPPLICANT_NON_ROM
int rsi_sta_disassoc(void *priv, const uint8 *own_addr, const uint8 *addr, uint16 reason)
{
  int16 retval;
  uint8 i;
  disconnect_wlan_t disconnect_wlan;
  struct wpa_supplicant *wpa_s = (struct wpa_supplicant *)(mgmt_if_adapter.supplicant_priv);
  mgmt_if_adapter.dbg_host_intf->sta_disassoc_count++;
  if (is_broadcast_ether_addr(addr)) {
    return 0;
  }

  for (i = 0; i < 6 && addr; i++) {
    disconnect_wlan.station_mac[i] = addr[i];
  }

  retval = mgmt_hm_send_disconnect(&disconnect_wlan, ((is_wps_client()) && (!wpa_s->wps_success)), 0);

  return retval;
}

/* @fn         sl_ap_remove_sta
 * @brief      This function triggers DISCONNET to SME.
 * @param[in]  priv, Pointer to AP configuration
 * @param[out] addr, pointer to STA MAC address
 * @return     0, success
 *
 * @section description
 * This function triggers DISCONNECT to SME layer thereby clearing the STA information before removing it
 */
int sl_ap_remove_sta(void *priv, const u8 *addr)
{
  return rsi_sta_disassoc(priv, addr, addr, 0);
}

static int rsi_client_disassoc(void *priv, const uint8 *addr, int reason)
{
  int16 retval = 0;
  uint8 i;
  disconnect_wlan_t disconnect_wlan;
  mgmt_if_adapter.dbg_host_intf->sta_disassoc_count++;
  if (is_broadcast_ether_addr(addr)) {
    return 0;
  }

  for (i = 0; i < 6 && addr; i++) {
    disconnect_wlan.station_mac[i] = addr[i];
  }

  mgmt_hm_send_disconnect(&disconnect_wlan, 0, 1);

  return retval;
}

int is_rejoin_happenig(void)
{
  sl_status_t status = SL_STATUS_OK;
#ifdef ENABLE_DRAEGER_CUSTOMIZATION
  uint8 *wpa_s = (uint8 *)mgmt_if_adapter.supplicant_priv;
  if (wpa_s && !get_wpa_auto_reconnect_disabled(wpa_s) && (mgmt_if_adapter.state < WISE_STATE_CONNECTED))
#else
  if (rsi_wsc_cb.rejoin_going_on == 1)
#endif
  {
    mgmt_if_adapter.rejoin_count++;
#ifdef ENABLE_DRAEGER_CUSTOMIZATION
    if (mgmt_if_adapter.custom_feature_bit_map & FEAT_SEL_ENABLE_ASYNC_MESSAGE) {
      wise_module_state(RSI_MODULE_STATE_SCANNING);
    }
    if ((mgmt_if_adapter.rejoin_magic_code == 0x4321 && mgmt_if_adapter.max_rejoin_retry
         && mgmt_if_adapter.rejoin_count >= mgmt_if_adapter.max_rejoin_retry)
        || (mgmt_if_adapter.rejoin_magic_code != 0x4321 && mgmt_if_adapter.rejoin_count >= MAX_REJOIN_RETRIES))
#else
    if (mgmt_if_adapter.rejoin_count >= MAX_REJOIN_RETRIES)
#endif
    {
      set_wpa_auto_reconnect_disabled(wpa_s, 1);
      mgmt_if_adapter.rejoin_count = 0;
#ifdef RSI_GLOBAL_VARIABLES
      set_rejoin_going_on(0);
#endif /* RSI_GLOBAL_VARIABLES */
      mgmt_if_adapter.rejoin_going_on = SL_FALSE;
      status                          = free_tx_host_q();
      if (status != SL_STATUS_OK) {
        SL_DEBUG_LOG("[ERROR] Failed to free tx queue: 0x%lX\r\n", status);
        SL_MGMT_ASSERT(0);
      }
      rsi_rejoin_fail_to_lmac();
    }
  }
#ifdef RSI_GLOBAL_VARIABLES
  set_rejoin_going_on(!!mgmt_if_adapter.rejoin_count);
#endif /* RSI_GLOBAL_VARIABLES */
  mgmt_if_adapter.rejoin_going_on = !!mgmt_if_adapter.rejoin_count;

  return mgmt_if_adapter.rejoin_going_on;
}

void wise_reset_to_band(uint8 vap_id)
{
  sl_status_t status = SL_STATUS_OK;
  uint8 *wpa_s       = (uint8 *)(vap_id ? mgmt_if_adapter.ap_supplicant_priv : mgmt_if_adapter.supplicant_priv);

  if (!wpa_s) {
    return;
  }

#ifdef TRANS_MODE_ENABLE
#ifdef DATA_PATH_UMAC_ENABLE
  if ((tcp_ip_cb.trans_mode_enable == TRANS_MODE_ENABLE_WORD) && (tcp_ip_cb.transmode_state < TRANS_MODE_END)
      && (tcp_ip_cb.transmode_state >= TRANS_MODE_START)) {
    exit_trans_mode(TRANS_MODE_EXIT_WIFI_DISC, 1);
  }
#endif /*DATA_PATH_UMAC_ENABLE*/
#endif /*TRANS_MODE_ENABLE*/

  mgmt_if_adapter.host_cmd    = 0;
  mgmt_if_adapter.connected   = SL_FALSE;
  mgmt_if_adapter.wps_session = SL_FALSE;

  if (!vap_id) {
    // Fix for not joining wpa2 security mode once it connect with wep mode
    sl_memset(mgmt_if_adapter.bssid, 0, ETH_ALEN);
    sl_memset(mgmt_if_adapter.ssid, 0, 33);
    mgmt_if_adapter.ssid_len        = 0;
    mgmt_if_adapter.keys_configured = 0;
    mgmt_if_adapter.pmk_stored      = SL_FALSE;
    mgmt_if_adapter.roam_ind        = 0;
    /*resetting the scan bitmap in init for WPS case this should be cleared*/
    mgmt_if_adapter.scan_channel_bitmap_2_4_GHz = 0;
    mgmt_if_adapter.scan_channel_bitmap_5_GHz   = 0;
    sl_memset(mgmt_if_adapter.pmk, 0, SL_PMK_LEN);
    sl_memzero(mgmt_if_adapter.passwd, sizeof(mgmt_if_adapter.passwd));
    sl_memset((void *)&mgmt_if_adapter.wep_key, 0, sizeof(wep_key_ds));
  }

  sl_memset(mgmt_if_adapter.psk, 0, 64);
  /* To free the scan entries hold in supplicant layer */
  if ((mgmt_if_adapter.scan_res != NULL) && !vap_id) {
    wpa_scan_results_free_wrapper(mgmt_if_adapter.scan_res);
    mgmt_if_adapter.scan_res = NULL;
  }

  if (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) {
    //! If station mode then send deauth to supplicant to reset station intf
    //! For AP mode this is not required as if not connected then it will be
    //! in init. In connected state of AP intferface, it suppose not to come
    //! here.
    if (vap_id == 0) {
      union wpa_event_data data;
      struct wpa_ssid *ssid                        = NULL;
      mgmt_if_adapter.supplicant_triggered_bg_scan = 0;
      mgmt_if_adapter.is_11w_enabled               = 0;
      wpa_notify_remove_all_networks(wpa_s);
      //! Reset supplicant state to disconnected
      wpa_supplicant_event_wrapper(wpa_s, EVENT_DEAUTH, &data);
      //! Remove configured netwok
      ssid = get_wpa_supp_conf_ssid(wpa_s);
      wpa_config_remove_network_wrapper(wpa_s, ssid->id);
      //! Need to reset following as it need to do fresh restart
      set_wpa_disconnected(wpa_s, 0);
    }
  } else if (mgmt_if_adapter.operating_mode != WISE_CONCURRENT_MODE) {
    wpa_supplicant_deinit_wrapper(wpa_s);
    if (mgmt_if_adapter.supplicant_thread_started) {
      eloop_terminate_wrapper();
      // Call supplicant event loop thread deinit
      status = sli_supp_eloop_thread_deinit();
      if (status != SL_STATUS_OK) {
        SL_DEBUG_LOG("[ERROR] Failed to deinit supp thread: 0x%lX\r\n", status);
        return;
      }
      mgmt_if_adapter.supplicant_thread_started = SL_FALSE;
    }
    mgmt_if_adapter.supplicant_priv = NULL;
  }

#ifdef RSI_GLOBAL_VARIABLES
  set_rejoin_going_on(0);
#endif /* RSI_GLOBAL_VARIABLES */
  mgmt_if_adapter.rejoin_going_on = SL_FALSE;
  mgmt_if_adapter.rejoin_count    = 0;

  status = free_tx_host_q();
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG("[ERROR] Failed to free tx queue: 0x%lX\r\n", status);
    SL_MGMT_ASSERT(0);
  }

  sl_memset(mgmt_if_adapter.psk, 0, sizeof(mgmt_if_adapter.psk));

  // Free allocated memory for dbg_host_intf
  if (mgmt_if_adapter.dbg_host_intf) {
    sl_free(mgmt_if_adapter.dbg_host_intf);
  }

  //! Set state based on the VAP ID in concurrent mode
  if ((mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) && (vap_id == 0)) {
    wise_update_state(WISE_STATE_BAND_DONE);
  } else if ((mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) && (vap_id == 1)) {
    wise_update_concurrent_state(WISE_CON_STATE_INIT_DONE);
  } else {
    wise_update_state(WISE_STATE_BAND_DONE);
  }
}

void wise_reset_ap(uint8 vap_id)
{
  sl_status_t status = SL_STATUS_OK;
  uint8 *wpa_s       = (uint8 *)(vap_id ? mgmt_if_adapter.ap_supplicant_priv : mgmt_if_adapter.supplicant_priv);

  if (!wpa_s) {
    return;
  }

  wpa_supplicant_remove_iface_wrapper(wpa_s, 1);
  if (vap_id) {
    mgmt_if_adapter.ap_supplicant_priv = NULL;
  } else {
    mgmt_if_adapter.supplicant_priv = NULL;
  }

  if (mgmt_if_adapter.client == SL_FALSE) {
    mgmt_if_adapter.host_cmd    = 0;
    mgmt_if_adapter.connected   = SL_FALSE;
    mgmt_if_adapter.ap_started  = SL_FALSE;
    mgmt_if_adapter.wps_session = SL_FALSE;
    sl_memset(mgmt_if_adapter.psk, 0, SL_PSK_LEN);
  }

  // Restart supplicant soon after destroying it so as to avoid any memory
  // hogging by other entities.
  ap_supp_start(wpa_s);

  status = free_tx_host_q();
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG("[ERROR] Failed to free tx queue: 0x%lX\r\n", status);
    SL_MGMT_ASSERT(0);
  }
#ifdef NETX_ENABLE
  //! Free allocated memory for NETX
  sl_free_netx_malloc_mem(vap_id);
#endif

  //! Set state based on the VAP ID in concurrent mode
  if (vap_id) {
    wise_update_concurrent_state(WISE_CON_STATE_INIT_DONE);
  } else {
    wise_update_state(WISE_STATE_INIT_DONE);
  }
  sl_mgmt_indicate_to_host(SLI_WLAN_RSP_AP_STOP, 0, 0, NULL);
}

int8 get_scan_results(uint8 scan_flag)
{
  struct wpa_scan_results *scan_res = mgmt_if_adapter.scan_res;
  uint8 *rxPkt                      = NULL;
  uint16 SCAN_RSP_SZ                = sizeof(struct rsi_scan_resp_s);
  rx_scatter_t *pb;
  struct wpa_scan_res *result = NULL;
  uint8 *sc_buf;
  uint8 *rsp;
  int (*compar)(const void *, const void *) = wpa_scan_result_compar;
  uint16 temp_sz, i, scan_rsp_cnt = 0;
  uint8 bssid_found                 = scan_res->num;
  struct rsi_host_ScanRsp *scan_rsp = NULL;
  uint32 rx_head_room = 0, rx_frame_desc_size = 0;

  if (scan_res->num == 0) {
    if (scan_flag == 1)
      sl_mgmt_indicate_to_host(SLI_WLAN_RSP_BGSCAN, 0, APS_NOT_FOUND, NULL);
    else {

      if (mgmt_if_adapter.scan_feature_bitmap & FEATURE_SCAN_SUPPLICANT_BYPASS)
        sl_mgmt_indicate_to_host(SLI_WLAN_RSP_SCAN, 0, 0, NULL);
      else
        sl_mgmt_indicate_to_host(SLI_WLAN_RSP_SCAN, 0, APS_NOT_FOUND, NULL);
    }
    return -1;
  }
  qsort(scan_res->res, scan_res->num, sizeof(struct wpa_scan_res *), compar);
  if (scan_flag)
    rxPkt = sl_alloc_rx_cmd_desc(SLI_WLAN_RSP_BGSCAN, 0, WLAN_STATUS_SUCCESS, SCAN_RSP_SZ * bssid_found + 8);
  else
    rxPkt = sl_alloc_rx_cmd_desc(SLI_WLAN_RSP_SCAN, 0, WLAN_STATUS_SUCCESS, SCAN_RSP_SZ * bssid_found + 8);
  if (rxPkt == NULL) {
    return 0;
  }

  rx_head_room         = cpf_get_rx_head_room();
  rx_frame_desc_size   = cpf_get_rx_frame_desc_size();
  pb                   = &(SL_PKT_GET_RXPKT_SCATTER_BUFF(rxPkt, 0));
  rsp                  = ((uint8 *)SL_PKT_GET_RXPKT_SCATTER_BUFF_ADDR(rxPkt, 0)) + rx_head_room + rx_frame_desc_size;
  scan_rsp             = (struct rsi_host_ScanRsp *)rsp;
  temp_sz              = SL_PKT_CHUNK_SIZE - (rx_head_room + rx_frame_desc_size) - 8;
  scan_rsp->uScanCount = 0;
  scan_rsp->uErrorcode = 0;
  rsp                  = rsp + 8;
  pb->len              = 8;
  scan_rsp_cnt         = 0;

  for (i = 0; i < scan_res->num; i++) {
    result = scan_res->res[i];
    sc_buf = (uint8 *)&result->channel_no;
    if (temp_sz < SCAN_RSP_SZ) {
      /* Check if the scatter count of the received packet exceeds the allowable limit */
      if (SL_PKT_GET_RXPKT_SCATTER_COUNT(rxPkt) >= SLI_MGMT_IF_RX_SCATTER_NUM) {
        SL_DEBUG_LOG("[ERROR] GET_SCAN_RESULT_FAILED: file %s, line %d\n", __FILE__, __LINE__);
        /* Free the allocated buffers and return */
        sl_free_rx_pkt(rxPkt);
        return -1;
      }
      uint16 rem_len = SCAN_RSP_SZ - temp_sz;
      sl_memcpy(rsp, (uint8 *)sc_buf, temp_sz);
      sc_buf += temp_sz;
      pb->len += temp_sz;
      pb++;
      pb->addr = (uint32)cpf_sl_pkb_alloc(SL_WLAN_MGMT_IF_PKT_TYPE_RX);

      if (pb->addr == 0) {
        SL_DEBUG_LOG("[ERROR] GET_SCAN_RESULT_FAILED: file %s, line %d\n", __FILE__, __LINE__);
        /* Free the allocated buffers and return */
        sl_free_rx_pkt(rxPkt);
        return -1;
      }

      pb->len = 0;
      SL_PKT_SET_RXPKT_SCATTER_COUNT(rxPkt, SL_PKT_GET_RXPKT_SCATTER_COUNT(rxPkt) + 1);
      rsp = (uint8 *)pb->addr;
      /* Copy remaining length */
      sl_memcpy(rsp, sc_buf, rem_len);
      rsp += rem_len;
      pb->len += rem_len;
      temp_sz = SL_PKT_CHUNK_SIZE - rem_len;
    } else {
      sl_memcpy(rsp, sc_buf, SCAN_RSP_SZ);
      rsp += SCAN_RSP_SZ;
      temp_sz -= SCAN_RSP_SZ;
      pb->len += SCAN_RSP_SZ;
    }
    scan_rsp_cnt++;
  }

  SL_PKT_SET_RXPKT_SCATTER_BUFF_ADDR(rxPkt,
                                     0,
                                     ((uint32)rxPkt + SL_PKT_GET_RXPKT_HOST_DESC_OFFSET(rxPkt) + (HOST_DESC_LENGTH)));
  scan_rsp->uScanCount = scan_rsp_cnt;
  SL_PKT_RX_HDESC_SET_DW0_QNUM(rxPkt, SL_WLAN_MGMT_Q);
  SL_PKT_RX_HDESC_SET_DW0_LENGTH(rxPkt, (SCAN_RSP_SZ * bssid_found + 8));
#ifdef AP_JSON_SCAN_LIST
  //! Check FEAT_WAC_PROVISIONING bit to avoid scan response to host
  if ((mgmt_if_adapter.custom_feature_bit_map & FEAT_WIRELESS_PROVISIONING)
      && (mgmt_if_adapter.operating_mode == WISE_MODE_AP) && (mgmt_if_adapter.scan_req_from_host)) {
    rsi_update_ap_json_scan_list("provisioning.html");

    mgmt_if_adapter.scan_req_from_host = 0;
  }

  else
#endif
  {
    sli_send_host_rx_pkt((uint8 *)rxPkt);
  }
  return 0;
}

static void set_module_mac(uint8 *mac)
{
#ifdef DATA_PATH_UMAC_ENABLE
  struct app_info_s *app_info = &glbl_app_info;
  sl_memcpy(app_info->PermanentAddress, mac, MAC_ADDR_LEN);
#endif /* DATA_PATH_UMAC_ENABLE */
  sl_memcpy(mgmt_if_adapter.mac_first_if, mac, MAC_ADDR_LEN);
  if (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) {
    sl_memcpy(mgmt_if_adapter.mac_second_if, mac + MAC_ADDR_LEN, MAC_ADDR_LEN);
  }
}

int16 mgmtif_process_init_rsp(uint8 *rxPkt, uint8 *send_to_host)
{
  uint8 *wpa_s  = (uint8 *)mgmt_if_adapter.supplicant_priv;
  uint32 status = 0;

  /*Set MAC for supplicant  */
  set_module_mac((uint8 *)SL_PKT_GET_RXPKT_SCATTER_BUFF_ADDR(rxPkt, 0));

  if (mgmt_if_adapter.operating_mode == WISE_MODE_CONNECTION || mgmt_if_adapter.operating_mode == WISE_MODE_PER
      || mgmt_if_adapter.operating_mode == WISE_MODE_EAP || mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) {
    if (!(mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) && mgmt_if_adapter.supplicant_priv) {
      if (wpa_s) {
        wpa_supplicant_deinit_wrapper(wpa_s);
        if (mgmt_if_adapter.supplicant_thread_started == SL_TRUE) {
          eloop_terminate_wrapper();
          // Call supplicant event loop thread deinit
          status = sli_supp_eloop_thread_deinit();
          if (status != SL_STATUS_OK) {
            SL_DEBUG_LOG("[ERROR] Failed to deinit supp thread: 0x%lX\r\n", status);
            return;
          }
          mgmt_if_adapter.supplicant_thread_started = SL_FALSE;
        }
        mgmt_if_adapter.supplicant_priv = NULL;
      }
    }
    /* WSC supplicant initialization */
    if ((mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE && !mgmt_if_adapter.supplicant_priv
         && !mgmt_if_adapter.ap_supplicant_priv)
        || !(mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE)) {
      //! bypass supplicant when wlan_btr_mode is enabled.
      if (!mgmt_if_adapter.wlan_btr_mode_en) {
        wsc_supp_start();
      }
    }
  }

  *send_to_host = 1;
  //! Dont change STA state when AP creation failed, if it is in concurrent
  //! mode Change state if it is not concurrent mode
  if (((mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) && (mgmt_if_adapter.state < WISE_STATE_INIT_DONE)
       && !mgmt_if_adapter.rejoin_going_on)
      || !(mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE)) {
    wise_update_state(WISE_STATE_INIT_DONE);
  }

  if ((mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) && !mgmt_if_adapter.ap_started) {
    wise_update_concurrent_state(WISE_CON_STATE_INIT_DONE);
  }

  return WLAN_STATUS_SUCCESS;
}

int rsi_get_ssid(void *priv, uint8 *ssid)
{
  if (!priv || !ssid) {
#ifdef WISE_DEBUG_ENABLE
    SLI_WLAN_MGMT_ASSERT(RSI_GET_SSID_ERROR);
#endif
    return -1;
  }
  if (mgmt_if_adapter.ssid_len > 0) {
    sl_memcpy(ssid, mgmt_if_adapter.ssid, mgmt_if_adapter.ssid_len);
    return mgmt_if_adapter.ssid_len;
  } else {
    return 0;
  }
}
#endif // SUPPLICANT_NON_ROM

#ifdef SUPPLICANT_NON_ROM
/**
 * set_generic_elem - Add IEs into Beacon/Probe Response frames (AP)
 * @priv: Private driver interface data
 * @elem: Information elements
 * @elem_len: Length of the elem buffer in octets
 * Returns: 0 on success, -1 on failure
 *
 * This is an optional function to add information elements in the
 * kernel driver for Beacon and Probe Response frames. This can be left
 * undefined (set to %NULL) if the driver uses the Beacon template from
 * set_beacon().
 */
int rsi_set_generic_elem(void *priv, const uint8 *elem, size_t elem_len)
{
  if (!priv || !elem) {
#ifdef WISE_DEBUG_ENABLE
    SLI_WLAN_MGMT_ASSERT(SET_GENERIC_ELEMENT_FAILED);
#endif
    return -1;
  }
  rsi_load_ie((uint8 *)elem, elem_len, IE_PROBE_RES);
  rsi_load_ie((uint8 *)elem, elem_len, IE_BEACON);
  return 0;
}

/**
 * set_ap_wps_ie - Add WPS IE(s) into Beacon/Probe Response frames (AP)
 * @priv: Private driver interface data
 * @beacon: WPS IE(s) for Beacon frames or %NULL to remove extra IE(s)
 * @proberesp: WPS IE(s) for Probe Response frames or %NULL to remove
 *	extra IE(s)
 * @assocresp: WPS IE(s) for (Re)Association Response frames or %NULL
 *	to remove extra IE(s)
 * Returns: 0 on success, -1 on failure
 *
 * This is an optional function to add WPS IE in the kernel driver for
 * Beacon and Probe Response frames. This can be left undefined (set
 * to %NULL) if the driver uses the Beacon template from set_beacon()
 * and does not process Probe Request frames. If the driver takes care
 * of (Re)Association frame processing, the assocresp buffer includes
 * WPS IE(s) that need to be added to (Re)Association Response frames
 * whenever a (Re)Association Request frame indicated use of WPS.
 *
 * This will also be used to add P2P IE(s) into Beacon/Probe Response
 * frames when operating as a GO. The driver is responsible for adding
 * timing related attributes (e.g., NoA) in addition to the IEs
 * included here by appending them after these buffers. This call is
 * also used to provide Probe Response IEs for P2P Listen state
 * operations for drivers that generate the Probe Response frames
 * internally.
 */
int rsi_set_ap_wps_ie(void *priv,
                      const struct wpabuf *beacon,
                      const struct wpabuf *proberesp,
                      const struct wpabuf *assocresp)
{
  if (!priv) {
#ifdef WISE_DEBUG_ENABLE
    SLI_WLAN_MGMT_ASSERT(SET_AP_WPS_IE_FAILED);
#endif
    return -1;
  }

  if (beacon) {
    if (rsi_load_ie(wpabuf_mhead_u8((struct wpabuf *)beacon), wpabuf_len((struct wpabuf *)beacon), IE_BEACON)) {
      return WLAN_STATUS_RSI_FAIL;
    }
  }
  if (proberesp) {
    if (rsi_load_ie(wpabuf_mhead_u8((struct wpabuf *)proberesp),
                    wpabuf_len((struct wpabuf *)proberesp),
                    IE_PROBE_RES)) {
      return WLAN_STATUS_RSI_FAIL;
    }
  }
  if (assocresp) {
    if (rsi_load_ie(wpabuf_mhead_u8((struct wpabuf *)assocresp),
                    wpabuf_len((struct wpabuf *)assocresp),
                    IE_ASSOC_RES)) {
      return WLAN_STATUS_RSI_FAIL;
    }
  }

  return 0;
}
#endif // SUPPLICANT_NON_ROM

int rsi_get_bssid(void *priv, uint8 *bssid)
{
  if (!priv || !bssid) {

#ifdef WISE_DEBUG_ENABLE
    SLI_WLAN_MGMT_ASSERT(RSI_GET_BSSID_FAILED);
#endif
    return -1;
  }
  sl_memcpy(bssid, get_bssid_from_mgmt_intf_adapter(), ETH_ALEN);
  return 0;
}

#ifdef SUPPLICANT_NON_ROM
const uint8 *driver_get_mac_addr()
{
  if (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) {
    if ((uint8)mgmt_if_adapter.own_mac_type == (uint8)1)
      return ((const uint8 *)mgmt_if_adapter.mac_second_if);
  }
  return ((const uint8 *)mgmt_if_adapter.mac_first_if);
}
#endif // SUPPLICANT_NON_ROM

uint8 *rsi_get_scan_results(void *priv)
{
  struct wpa_scan_results *ret_val;

  if (!priv) {
    return NULL;
  }

  /* results are forward to supplicant */
  ret_val = mgmt_if_adapter.scan_res;
  /*reintialize the scan_res to hold next scan responses */
  mgmt_if_adapter.scan_res = (struct wpa_scan_results *)sl_zalloc(sizeof(struct wpa_scan_results));
  if (mgmt_if_adapter.scan_res == NULL) {
#ifdef WISE_DEBUG_ENABLE
    SLI_WLAN_MGMT_ASSERT(NO_SCAN_RESULT);
#endif
  }

  return (uint8 *)ret_val;
}

#ifdef SUPPLICANT_NON_ROM
/*** Commands to supplicant ***/

void rsi_p2p_start_full_scan()
{
#if CONFIG_P2P
  struct wpa_supplicant *wpa_s = (struct wpa_supplicant *)mgmt_if_adapter.supplicant_priv;
  /*
     int wpas_p2p_find(struct wpa_supplicant *wpa_s, unsigned int timeout,
     enum p2p_discovery_type type,
     unsigned int num_req_dev_types, const uint8 *req_dev_types,
     const uint8 *dev_id, unsigned int search_delay)
     */

  wpas_p2p_find(wpa_s, 0, P2P_FIND_START_WITH_FULL, 0, NULL, NULL, 0, 0, NULL,
                0); // For now 0 timeout - 2nd parameter
#endif
}

void rsi_p2p_start_find()
{
#if CONFIG_P2P
  struct wpa_supplicant *wpa_s = (struct wpa_supplicant *)mgmt_if_adapter.supplicant_priv;
  wpas_p2p_find(wpa_s, 0, P2P_FIND_ONLY_SOCIAL, 0, NULL, NULL, 0, 0, NULL,
                0); // For now 0 timeout - 2nd parameter
#endif
}

#if !defined(EXCLUDE_AP) && !defined(EXCLUDE_WPS)
int16 start_wps_push_button()
{
  if (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) {
    return (wpa_supplicant_ap_wps_pbc_wrapper((struct wpa_supplicant *)mgmt_if_adapter.ap_supplicant_priv, NULL, NULL));
  } else {
    return (wpa_supplicant_ap_wps_pbc_wrapper((struct wpa_supplicant *)mgmt_if_adapter.supplicant_priv, NULL, NULL));
  }
}
#endif

void wise_update_psk_in_nwparams(uint8 *key, uint16 key_len)
{
  if (key_len <= 64) {
    sl_memcpy(mgmt_if_adapter.psk, key, key_len);
  }
}
#endif // SUPPLICANT_NON_ROM

int16 is_eapol_pkt(uint8 *rx_pkt, uint8 vap_id)
{
  uint8 *buf = (uint8 *)(SL_PKT_GET_RXPKT_SCATTER_BUFF_ADDR(rx_pkt, 0)); /* Skip the descriptor */

  if (((htons(*(uint16 *)&buf[12]) == ETH_P_RSN_PREAUTH)
       && (mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_PREAUTH_SUPPORT_ENABLE))
      || (htons(*(uint16 *)&buf[12]) == ETH_P_EAPOL)) {
    eapol_pkt_processing(rx_pkt, vap_id);
  }

  return SL_FALSE;
}

uint32 dbg_avail_rx_mem_index = 0;
extern struct mem_info_s glbl_mem_info;
#ifdef SUPPLICANT_NON_ROM
int16 eapol_pkt_processing(uint8 *rx_pkt, uint8 vap_id)
{
  uint8 *buf     = (uint8 *)(SL_PKT_GET_RXPKT_SCATTER_BUFF_ADDR(rx_pkt, 0)); /* Skip the descriptor */
  uint32 pkt_len = SL_PKT_RX_HDESC_GET_DW0_LENGTH(rx_pkt) - SL_PKT_RX_HDESC_GET_DW1_EXT_DESC_SIZE(rx_pkt);
#ifdef RS9113_PREAUTH_PORTING
  if ((htons(*(uint16 *)&buf[12]) == ETH_P_RSN_PREAUTH)
      && (mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_PREAUTH_SUPPORT_ENABLE)) {
    //wpa_printf(1, "Recvd preauth packet");
    buf = sli_form_linear_buffer(rx_pkt);
    if (buf == NULL) {
      sl_free_rx_pkt((uint8 *)rx_pkt);
      return SL_TRUE;
    }
    sl_free_rx_pkt((uint8 *)rx_pkt);
    rsn_preauth_receive_wrapper(mgmt_if_adapter.supplicant_priv, &buf[6], &buf[14], (pkt_len - SL_ETH_HDR_LEN));

    sl_free(buf);
    return SL_TRUE;
  }
#endif

  if (htons(*(uint16 *)&buf[12]) == ETH_P_EAPOL) {
    buf = sli_form_linear_buffer(rx_pkt);
    if (buf == NULL) {
      sl_free_rx_pkt((uint8 *)rx_pkt);
      return SL_TRUE;
    }
    sl_free_rx_pkt((uint8 *)rx_pkt);
    mgmt_if_adapter.dbg_host_intf->eap_rx_pkt_cnt++;

    if (vap_id == 0) {
      drv_event_eapol_rx(mgmt_if_adapter.supplicant_priv, &buf[6], &buf[14], pkt_len - SL_ETH_HDR_LEN);
    } else {
      drv_event_eapol_rx(mgmt_if_adapter.ap_supplicant_priv, &buf[6], &buf[14], pkt_len - SL_ETH_HDR_LEN);
    }

    sl_free(buf);

    return SL_TRUE;
  }
  return SL_FALSE;
}

/*Start timer for reseting the WFD connection if it is not succeeded or failed*/
void rsi_wfd_join_timed_out(void *eloop_ctx, void *timeout_ctx)
{
  /*Reset supplicant state*/
  wise_reset_to_band(0);
  /*Give error to host*/
  sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, JOIN_TIMED_OUT, NULL);
}

int16 wise_send_go_params(void)
{
  rsi_GOParams go_params;
  uint16 len             = sizeof(rsi_GOParams);
  int16 status           = 0;
  struct sta_info_s *sta = &ap_cb.sta_cb[0];

  if ((!(mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) && is_client_mode())
      || ((mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE)
          && (sl_get_concurrent_mode_state() < WISE_CON_STATE_CONNECTED))) {
    len    = 0;
    status = WISE_ERROR_NOT_AP_MODE;
  } else {
    sl_memset(&go_params, 0, sizeof(rsi_GOParams));

    sl_memcpy((char *)go_params.ssid, (char *)sl_get_ssid_from_mgmt_if_adapter(), mgmt_if_adapter.ssid_len);

    if (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) {
      sl_memcpy((char *)go_params.ssid, (char *)mgmt_if_adapter.apconfig.ssid, sizeof(mgmt_if_adapter.apconfig.ssid));
    }

    sl_memcpy((char *)go_params.bssid, (char *)mgmt_if_adapter.mac_first_if, MAC_ADDR_LEN);

    if (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) {
      sl_memcpy((char *)go_params.bssid, (char *)mgmt_if_adapter.mac_second_if, MAC_ADDR_LEN);
    }

    go_params.channel_number = mgmt_if_adapter.join_channel;

    if (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) {
      sl_strcpy(go_params.psk, mgmt_if_adapter.apconfig.psk);
    } else {
      sl_strcpy(go_params.psk, mgmt_if_adapter.psk);
    }

#ifdef DATA_PATH_UMAC_ENABLE
    get_go_ip_address_info(&go_params, sta);
#endif /*DATA_PATH_UMAC_ENABLE*/

    go_params.sta_count = mgmt_if_adapter.sta_count;
    len -= ((mgmt_if_adapter.max_station_supported - mgmt_if_adapter.sta_count) * sizeof(struct go_sta_info_s));
  }

  sl_mgmt_indicate_to_host(SLI_E_WLAN_GO_PARAMS_QUERY, len, status, (uint8 *)&go_params);

  return 0;
}

void wise_notify_p2p_dev_found(uint8 new_device, uint8 *device_name, uint8 *p2p_device_addr, uint8 *pri_dev_type)
{
  uint8 dev_type[2] = { 0, 0 };
  rsi_wfdDevInfo wfdInfo;

  sl_memset((void *)&wfdInfo, 0, sizeof(rsi_wfdDevInfo));

  if (pri_dev_type) {
    dev_type[0] = pri_dev_type[1];
    dev_type[1] = pri_dev_type[7];
  }

  wfdInfo.devState = new_device;
  if (device_name)
    sl_memcpy(wfdInfo.devName, device_name, 32);
  sl_memcpy(wfdInfo.macAddress, p2p_device_addr, 6);
  *(uint16 *)wfdInfo.devtype = *(uint16 *)dev_type;
  sl_mgmt_indicate_to_host(SLI_E_WLAN_WFD_DEV_RSP, sizeof(rsi_wfdDevInfo), 0, (uint8 *)&wfdInfo);
}
#endif // SUPPLICANT_NON_ROM

void gamepad_set_dest_mac(uint8 *mac);
int rsi_wsc_send_eapol(void *priv, const uint8 *dest, uint16 proto, const uint8 *data, size_t data_len)
{
  uint8 *msg;

  if (!priv) {
#ifdef WISE_DEBUG_ENABLE
    SLI_WLAN_MGMT_ASSERT(SEND_EAPOL_FAILED);
#endif
    return -1;
  }
  mgmt_if_adapter.dbg_host_intf->send_eapol_count++;
  /* Allocate buffer */
  msg = sl_malloc(10 + SL_ETH_HDR_LEN);
  if (!msg)
    return -1;

  /* Construct Ethernet hdr */
  sl_memcpy(&msg[0], (void *)dest, ETH_ALEN);

  sl_memcpy(&msg[ETH_ALEN], (uint8 *)mgmt_if_adapter.mac_first_if, ETH_ALEN);
  if ((mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE)
      && (sl_memcmp((uint8 *)dest, (uint8 *)(get_bssid_from_mgmt_intf_adapter()), ETH_ALEN) != 0)) {
    /*Assuming dest is BSSID in station mode else packet is destined to
     * connecting station*/
    sl_memcpy(&msg[ETH_ALEN], (uint8 *)mgmt_if_adapter.mac_second_if, ETH_ALEN);
  }

  *(uint16 *)&msg[12] = htons(proto);

  /* TODO: Handle failure case here */
  rsi_send_eapol_cmd((uint8 *)data, data_len, msg, is_client_mode());

  sl_free(msg);

  return 0;
}

/**
 * set_operstate - Sets device operating state to DORMANT or UP
 * @priv: private driver interface data
 * @state: 0 = dormant, 1 = up
 * Returns: 0 on success, -1 on failure
 *
 * This is an optional function that can be used on operating systems
 * that support a concept of controlling network device state from user
 * space applications. This function, if set, gets called with
 * state = 1 when authentication has been completed and with state = 0
 * when connection is lost.
 */
int rsi_set_operstate(void *priv, int state)
{
  if (is_client_mode()) {
    if (state == 1) {
      if ((mgmt_if_adapter.csec_mode == SEC_MODE_OPEN) || (mgmt_if_adapter.csec_mode == SEC_MODE_WEP)
          || ((mgmt_if_adapter.join_feature_bitmap & JOIN_FEAT_KEY_MGMT_WPA_CCXV2) && mgmt_if_adapter.ccx_capable
              && mgmt_if_adapter.roam_ind)) {
        wsc_supp_join_done(1);
      } else {
        mgmt_if_adapter.cfm_required = SL_TRUE;
      }
    }
  }
  return 0;
}

//#endif

#ifdef SUPPLICANT_NON_ROM
/**
 * hapd_send_eapol - Send an EAPOL packet (AP only)
 * @priv: private driver interface data
 * @addr: Destination MAC address
 * @data: EAPOL packet starting with IEEE 802.1X header
 * @data_len: Length of the EAPOL packet in octets
 * @encrypt: Whether the frame should be encrypted
 * @own_addr: Source MAC address
 * @flags: WPA_STA_* flags for the destination station
 *
 * Returns: 0 on success, -1 on failure
 */
int rsi_hapd_send_eapol(void *priv,
                        const uint8 *addr,
                        const uint8 *data,
                        size_t data_len,
                        int encrypt,
                        const uint8 *own_addr,
                        uint32 flags)
{
  rsi_wsc_send_eapol(priv, addr, ETH_P_EAPOL, data, data_len);
  return 0;
}

/**
 * get_capa - Get driver capabilities
 * @priv: private driver interface data
 *
 * Returns: 0 on success, -1 on failure
 *
 * Get driver/firmware/hardware capabilities.
 */
int rsi_get_capa(void *priv, struct wpa_driver_capa *capa)
{
  sl_memset((uint8 *)capa, 0, sizeof(struct wpa_driver_capa));

  capa->flags = WPA_DRIVER_FLAGS_AP;
  if (mgmt_if_adapter.operating_mode == WISE_MODE_P2P) {
    capa->flags |= WPA_DRIVER_FLAGS_P2P_CAPABLE;
  } else if ((mgmt_if_adapter.operating_mode == WISE_MODE_CONNECTION)
             || (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE)) {
    capa->enc |= WPA_DRIVER_CAPA_ENC_WEP40 | WPA_DRIVER_CAPA_ENC_WEP104;
  }
  return 0;
}

/**
 * get_hw_feature_data - Get hardware support data (channels and rates)
 * @priv: Private driver interface data
 * @num_modes: Variable for returning the number of returned modes
 * flags: Variable for returning hardware feature flags
 * Returns: Pointer to allocated hardware data on success or %NULL on
 * failure. Caller is responsible for freeing this.
 */
#ifndef SUPPLICANT_OPTIMIZATION
struct hostapd_hw_modes *rsi_get_hw_feature_data(void *priv, uint16 *num_modes, uint16 *flags, uint8 *dfs)
{
  return NULL;
}
#endif

#ifdef ENABLE_11R
int rsi_update_ft_ies_to_sme(void *wpa_s, const uint8 *md, const uint8 *ies, size_t ies_len)
{
  int16 retval = 0;

  mgmt_hm_update_ft_ies(ies, ies_len);
  return retval;
}
#endif /* ENABLE_11R */

#ifdef GTK_OFFLOAD
void rsi_set_rekey_info(void *priv,
                        const uint8 *kek,
                        size_t kek_len,
                        const uint8 *kck,
                        size_t kck_len,
                        const uint8 *replay_ctr)
{
  gtk_rekey_info_t gtk_rekey;

  sl_memzero(&gtk_rekey, sizeof(gtk_rekey));

  //! Copy kek,kck,replay ctr from supplicant to locally initialized structure
  sl_memcpy(gtk_rekey.kek, (uint8 *)kek, KEK_LENGTH);
  sl_memcpy(gtk_rekey.kck, (uint8 *)kck, KCK_LENGTH);
  sl_memcpy(gtk_rekey.replay_cntr, (uint8 *)replay_ctr, REPLAY_COUNTER_LENGTH);

  //wpa_printf(1, "OBE: gtk rekey info received from supplicant\n");

  mgmt_hm_set_rekey_info(&gtk_rekey);

  return;
}
#endif

/**
 * set_key - Configure encryption key
 * @ifname: Interface name (for multi-SSID/VLAN support)
 * @priv: private driver interface data
 * @alg: encryption algorithm (%WPA_ALG_NONE, %WPA_ALG_WEP,
 *	%WPA_ALG_TKIP, %WPA_ALG_CCMP, %WPA_ALG_IGTK, %WPA_ALG_PMK);
 *	%WPA_ALG_NONE clears the key.
 * @addr: Address of the peer STA (BSSID of the current AP when setting
 *	pairwise key in station mode), ff:ff:ff:ff:ff:ff for
 *	broadcast keys, %NULL for default keys that are used both for
 *	broadcast and unicast; when clearing keys, %NULL is used to
 *	indicate that both the broadcast-only and default key of the
 *	specified key index is to be cleared
 * @key_idx: key index (0..3), usually 0 for unicast keys; 0..4095 for
 *	IGTK
 * @set_tx: configure this key as the default Tx key (only used when
 *	driver does not support separate unicast/individual key
 * @seq: sequence number/packet number, seq_len octets, the next
 *	packet number to be used for in replay protection; configured
 *	for Rx keys (in most cases, this is only used with broadcast
 *	keys and set to zero for unicast keys); %NULL if not set
 * @seq_len: length of the seq, depends on the algorithm:
 *	TKIP: 6 octets, CCMP: 6 octets, IGTK: 6 octets
 * @key: key buffer; TKIP: 16-byte temporal key, 8-byte Tx Mic key,
 *	8-byte Rx Mic Key
 * @key_len: length of the key buffer in octets (WEP: 5 or 13,
 *	TKIP: 32, CCMP: 16, IGTK: 16)
 *
 * Returns: 0 on success, -1 on failure
 *
 * Configure the given key for the kernel driver. If the driver
 * supports separate individual keys (4 default keys + 1 individual),
 * addr can be used to determine whether the key is default or
 * individual. If only 4 keys are supported, the default key with key
 * index 0 is used as the individual key. STA must be configured to use
 * it as the default Tx key (set_tx is set) and accept Rx for all the
 * key indexes. In most cases, WPA uses only key indexes 1 and 2 for
 * broadcast keys, so key index 0 is available for this kind of
 * configuration.
 *
 * Please note that TKIP keys include separate TX and RX MIC keys and
 * some drivers may expect them in different order than wpa_supplicant
 * is using. If the TX/RX keys are swapped, all TKIP encrypted packets
 * will trigger Michael MIC errors. This can be fixed by changing the
 * order of MIC keys by swapping te bytes 16..23 and 24..31 of the key
 * in driver_*.c set_key() implementation, see driver_ndis.c for an
 * example on how this can be done.
 */
int rsi_set_key(const char *ifname,
                void *priv,
                enum wpa_alg alg,
                const uint8 *addr,
                int key_idx,
                int set_tx,
                const uint8 *seq,
                size_t seq_len,
                const uint8 *key,
                size_t key_len)
{
  set_keys_t tmp_setkey;
  uint8 clear_keys = 0;
  uint8 ap_iface   = 0;

  if (!priv) {
    return -1;
  }
  if ((alg == WPA_ALG_NONE) || (key_len == 0)) {
    clear_keys = 1;
    /*No need of sending clear keys*/
    return 0;
  }
  if (clear_keys && !mgmt_if_adapter.keys_configured) {
    return 0;
  }
  mgmt_if_adapter.dbg_host_intf->set_key_count++;
  sl_memset((uint8 *)&tmp_setkey, 0, sizeof(set_keys_t));

  tmp_setkey.type   = KEY_TYPE_PTK;
  tmp_setkey.key_id = (uint8)(key_idx & 3);
  if ((addr == NULL) || is_broadcast_ether_addr(addr)) {
    tmp_setkey.type = KEY_TYPE_GTK;
    //wpa_printf(1, "Set Key GTK and Key ID %d", tmp_setkey.key_id);
  } else {
    //wpa_printf(1, "Set Key PTK and Key ID %d", tmp_setkey.key_id);
  }
  if (alg == WPA_ALG_IGTK) {
    mgmt_if_adapter.is_11w_enabled = 1;
  }

  tmp_setkey.type |= (uint8)(alg << 4);
  if (mgmt_if_adapter.ap_mode_set && addr) {
    sl_memcpy(tmp_setkey.mac_address, (void *)addr, 6);
  }

  if (key_len <= 32) {
    sl_memcpy(tmp_setkey.key, (void *)key, key_len);
    if ((alg == WPA_ALG_TKIP) && (tmp_setkey.type & KEY_TYPE_PTK)) {
      // sl_memcpy(dbg_tx_mic_key, key + 16, 8);
    }
  } else {
    //wpa_printf(1, "Unable to load keys due to lack of Tx buffers");
    SLI_WLAN_MGMT_ASSERT(UMAC_ASSERT_UNABLE_TO_LOAD_KEYS);
    return -1;
  }

  tmp_setkey.key_len = key_len;

  if (!clear_keys) {
    tmp_setkey.key_id |= INSTALL_KEY;
  }

  if ((mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE)
      && ((sl_memcmp((void *)ifname, (void *)("ap0"), 3) == 0))) {
    ap_iface = 1;
  }

  /* TODO: Handle failure case here */
  rsi_set_keys_frame_cmd(&tmp_setkey, ap_iface, &mgmt_if_adapter.keys_configured);

  return WLAN_STATUS_SUCCESS;
}

#ifdef ENABLE_11R
static int rsi_send_ft_action(struct wpa_sm *sm, uint8 action, const uint8 *target_ap, const uint8 *ies, size_t ies_len)
{
  int16 retval = 0;
  ft_action_req_frm_supp_t ft_params;

  sl_memzero(&ft_params, sizeof(ft_params));
  ft_params.action_code = action;
  sl_memcpy(ft_params.target_mac, (uint8 *)target_ap, 6);
  ft_params.ft_ies_len = ies_len;
  mgmt_hm_send_ft_action(ies, ies_len, &ft_params);

  return retval;
}
#endif /* ENABLE_11R */

static struct wpa_scan_results *rsi_get_scan_results_wrapper(void *priv)
{
  return (struct wpa_scan_results *)rsi_get_scan_results(priv);
}

static int wpa_driver_rsi_join_wrapper(void *priv, struct wpa_driver_associate_params *params_p)
{
  return wpa_driver_rsi_join(priv, (void *)params_p);
}

static void *supplicant_interface_driver_init(void *ctx, const char *ifname)
{
  return (void *)&mgmt_if_adapter;
}

struct wpa_driver_ops wpa_driver_none_ops;
#ifdef CONFIG_IEEE80211R
uint8 driver_name[] = "none";
#endif
void init_wsc_supp_interface()
{
  mgmt_if_adapter.dbg_host_intf = (dbg_host_intf_t *)sl_zalloc(sizeof(dbg_host_intf_t));
  sl_memset(&wpa_driver_none_ops, 0, sizeof(struct wpa_driver_ops));

  if (mgmt_if_adapter.scan_res != NULL) {
    wpa_scan_results_free_wrapper(mgmt_if_adapter.scan_res);
  }

  mgmt_if_adapter.scan_res = (struct wpa_scan_results *)sl_zalloc(sizeof(struct wpa_scan_results));

  /* Driver interface init */
  wpa_driver_none_ops.send_eapol = rsi_wsc_send_eapol;

  wpa_driver_none_ops.deauthenticate = (int (*)(void *, const uint8 *, uint16))rsi_disconnect_wlan;
  // wpa_driver_none_ops.disassociate = rsi_disconnect_wlan;

  wpa_driver_none_ops.sta_disassoc      = rsi_sta_disassoc;
  wpa_driver_none_ops.sta_remove        = sl_ap_remove_sta;
  wpa_driver_none_ops.sta_deauth        = rsi_sta_disassoc;
  wpa_driver_none_ops.get_ssid          = rsi_get_ssid;
  wpa_driver_none_ops.set_key           = rsi_set_key;
  wpa_driver_none_ops.set_operstate     = rsi_set_operstate;
  wpa_driver_none_ops.init              = supplicant_interface_driver_init;
  wpa_driver_none_ops.get_mac_addr      = driver_get_mac_addr;
  wpa_driver_none_ops.get_bssid         = rsi_get_bssid;
  wpa_driver_none_ops.get_scan_results2 = rsi_get_scan_results_wrapper;
  wpa_driver_none_ops.remain_on_channel = rsi_set_channel;
  wpa_driver_none_ops.probe_req_report  = rsi_set_ListenMode;
  wpa_driver_none_ops.associate         = wpa_driver_rsi_join_wrapper;
  wpa_driver_none_ops.scan2             = wpa_driver_rsi_scan;
  wpa_driver_none_ops.send_action       = rsi_drv_send_action_wrapper;
  wpa_driver_none_ops.send_mlme         = wpa3_send_sae_handshake_packet_wrapper;
  wpa_driver_none_ops.sl_wpa_driver     = sl_wpa_driver;
#ifdef ENABLE_11R
  //! 802.11R specific supplicant to UMAC driver callback
  wpa_driver_none_ops.update_ft_ies  = (int (*)(void *, const uint8 *, const uint8 *, size_t))rsi_update_ft_ies_to_sme;
  wpa_driver_none_ops.send_ft_action = (int (*)(void *, uint8, const uint8 *, const uint8 *, size_t))rsi_send_ft_action;
#endif
#ifndef SUPPLICANT_OPTIMIZATION
  wpa_driver_none_ops.get_hw_feature_data = rsi_get_hw_feature_data;
#endif

  /* Hostapd */
  wpa_driver_none_ops.set_generic_elem = rsi_set_generic_elem;
  wpa_driver_none_ops.hapd_send_eapol  = rsi_hapd_send_eapol;
  wpa_driver_none_ops.set_ap_wps_ie    = rsi_set_ap_wps_ie;
  wpa_driver_none_ops.get_capa         = rsi_get_capa;

  wpa_driver_none_ops.name = (const char *)driver_name;
#ifdef GTK_OFFLOAD
  // Note: By default GTK rekey data not allowing to LMAC
  // We are allowing only if we enable BIT(17) in Ext_feature_bit_map
  if (mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_ENABLE_GTK_REKEY_DATA) {
    wpa_driver_none_ops.set_rekey_info = rsi_set_rekey_info;
  }
#endif
}

/**
 *$Log: module_intf.c,v $
 *Revision 1.1.1.1  2012/02/28 07:14:12  kiran.gunda
 *WiSe Connet initial import
 *
 **/

#ifdef ENABLE_DRAEGER_CUSTOMIZATION
uint8 rsi_find_reason_code(uint8 failure_casue)
{
  uint8 reason_code = 0;
  switch (failure_casue) {
    case PSK_NOT_CONFIGURED:
      reason_code = RSI_MODULE_STATE_PSK_NOT_CONF;
      break;
    case DEAUTH_FROM_AP:
    case UMAC_DEAUTH_RECEIVED_AFTER_CHANNEL_SWITCH:
      reason_code = RSI_MODULE_STATE_DEAUTH_FROM_AP;
      break;
    case SLI_WLAN_ASSOCIATION_FAILED:
    case UMAC_ASSOCIATION_TIMEOUT:
      reason_code = RSI_MODULE_STATE_ASSOC_FAILED;
      break;
    case SLI_WLAN_AUTHENTICATION_FAILED:
    case UMAC_SYNCHRONISATION_MISSED:
    case UMAC_AUTHENTICATION_TIMEOUT:
      reason_code = RSI_MODULE_STATE_AUTH_FAILED;
      break;
    case DEAUTH_DUE_TO_BEACON_MISSED:
      reason_code = RSI_MODULE_STATE_BEACON_LOSS;
      break;
    case SUPP_DEAUTH:
      reason_code = RSI_MODULE_STATE_SUPP_DEAUTH;
      break;
    case EAPOL_4_WAY_HANDSHAKE_FAIL:
      reason_code = RSI_MODULE_STATE_FOUR_WAY_HANDSHAKE_FAIL;
      break;
  }
  return reason_code;
}

void wise_module_state(uint8 currentState)
{
  uint8 *rxPkt;
  rsi_con_state_notificaton_t notification;
  extern uint8 reason_code_eap_failure;

  /*Add feature select bit to enable or disable the messaging*/
  if (!(mgmt_if_adapter.custom_feature_bit_map & FEAT_SEL_ENABLE_ASYNC_MESSAGE)) {
    return; /*Do not give message to host*/
  }

  notification.TimeStamp = SYSTEM_CLOCK;
  notification.stateCode = mgmt_if_adapter.async_state_code;
  if (reason_code_eap_failure) {
    mgmt_if_adapter.async_reason_code = reason_code_eap_failure;
  }
  notification.reason_code = mgmt_if_adapter.async_reason_code;
  notification.rsi_channel = mgmt_if_adapter.async_channel;
  reason_code_eap_failure  = 0;
  notification.rsi_rssi    = mgmt_if_adapter.async_rssi; /*100 means rssi not available*/
  sl_memcpy(notification.rsi_bssid, mgmt_if_adapter.async_bssid, MAC_ADDR_LEN);
  if (cpf_sl_is_buffers_available()) {
    rxPkt = sl_alloc_rx_cmd_desc(RSI_RSP_MODULE_STATE, 0, 0, sizeof(rsi_con_state_notificaton_t));
    if (rxPkt == NULL) {
      SLI_WLAN_MGMT_ASSERT(UMAC_ASSERT_SME_NO_BUFF_FOR_ASYC_MSG);
      return;
    }

    SL_PKT_SET_RXPKT_SCATTER_BUFF_LEN(rxPkt, 0, sizeof(rsi_con_state_notificaton_t));
    SL_PKT_SET_RXPKT_SCATTER_BUFF_ADDR(rxPkt,
                                       0,
                                       ((uint32)rxPkt + SL_PKT_GET_RXPKT_HOST_DESC_OFFSET(rxPkt) + (HOST_DESC_LENGTH)));
    sl_memcpy((uint8 *)SL_PKT_GET_RXPKT_SCATTER_BUFF_ADDR(rxPkt, 0),
              &notification,
              SL_PKT_GET_RXPKT_SCATTER_BUFF_LEN(rxPkt, 0));
    SL_PKT_SET_RXPKT_NEXT_PTR(rxPkt, NULL);
    sli_send_host_rx_pkt(rxPkt);
  } else {
    uint16 assertion_val = UMAC_ASSERT_NO_BUFFER_FOR_ASYNC_MSG_IN_STATE_I;
    switch (currentState) {
      case RSI_MODULE_STATE_SCANNING:
        assertion_val = UMAC_ASSERT_NO_BUFFER_FOR_ASYNC_MSG_IN_STATE_I;
        break;
      case RSI_MODULE_STATE_SCAN_RESULT:
        assertion_val = UMAC_ASSERT_NO_BUFFER_FOR_ASYNC_MSG_IN_STATE_II;
        break;
      case RSI_MODULE_STATE_FINAL_CONNECTON:
        assertion_val = UMAC_ASSERT_NO_BUFFER_FOR_ASYNC_MSG_IN_STATE_III;
        break;
    }
    SLI_WLAN_MGMT_ASSERT(assertion_val);
  }
}
#endif

#endif // SUPPLICANT_NON_ROM
