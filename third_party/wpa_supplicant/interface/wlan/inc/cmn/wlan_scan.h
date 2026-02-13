/*******************************************************************************
* @file  wlan_scan.h
* @brief
*******************************************************************************
* # License
* <b>Copyright 2020-2023 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef WLAN_SCAN_H
#define WLAN_SCAN_H

#include "ieee_80211_def.h"

/* Scan response array */
typedef struct scanConfirm_s {
  uint8 channelNum;
  uint8 sec_mode;
  uint8 rssiVal;
  uint8 uNetworkType;
  uint8 SSID[SL_SSID_LEN];
  uint8 bssid[MAC_ADDR_LEN];
  uint8 SNR_value;
  uint8 reserved;
  /* Do not interchange/move the above fields */

  uint16 TimeStamp[4];
  uint16 Beacon;         /* Beacon Interval.   */
  uint16 CapabilityInfo; /* AP capability info.*/
  uint8 SSIDLen;
  uint8 SupportedRatesLen;
  uint8 SupportedRatesEID;
  uint8 SupportedRates[16];
  uint8 ExtSupportedRatesLen;
  uint8 ExtSupportedRates[16];
  uint8 ATIMWindow;
  uint8 PhyParamLen;
  uint8 PhyParam[3];
  uint8 enc_mode;
  uint8 enc_mode_bkp;
  genIe_t ie_11n;
  uint8 htCap_ie[28];
  uint8 htCap1_ie[28];
  uint8 ht_capable;
  uint8 ht_capable1;
  uint8 capable_11n;
  uint8 qos_enable;
  uint8 wmm_ps_enable;
  uint16 group_cipher;
#ifdef RSI_ENABLE_CCX
  struct ieee80211_aironet_ie aironet_ie;
  struct ccx_radio_cap_ie radio_cap_ie;
  struct ieee80211_aironet_client_pwr_ie aironet_client_pwr_ie;
  uint64 tsf;
  uint32 rn;
  uint64 mic;
  uint8 ccx_ver_ie;
  uint8 ccx_capable;
  uint8 pwr_cnstraint;
#endif
  uint8 vht_capable;
#ifdef ENABLE_11AX
  uint8 he_capable;
#endif
} scanConfirm_t;

#endif /* WLAN_SCAN_H */