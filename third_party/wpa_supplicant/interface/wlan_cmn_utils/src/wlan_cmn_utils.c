/*******************************************************************************
* @file  wlan_cmn_utils.c
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

#include "wlan_cmn_utils.h"
#include "wlan_user_command.h"
//#include "umac_platform_if.h"

#define FREQ_BITMAP_MASK_2P4GHZ(A) 1 << ((A - 2412) / 5)

#define FREQ_BITMAP_MASK_5GHZ(A)                                     \
  (((A >= 5180) && (A <= 5320))   ? (1 << ((A - 5180) / 20))         \
   : ((A >= 5500) && (A <= 5720)) ? ((1 << 8) << ((A - 5500) / 20))  \
   : ((A >= 5745) && (A <= 5825)) ? ((1 << 20) << ((A - 5745) / 20)) \
                                  : 0)

uint8 set_operating_mode(uint8 temp_modeVal)
{

  /*command is valid*/
  if (temp_modeVal == HOST_OP_PER_MODE) {
    //! PER mode
    temp_modeVal = WISE_MODE_PER;
  } else if (temp_modeVal == HOST_OP_CONCURRENT_MODE) {
    temp_modeVal = WISE_CONCURRENT_MODE;
  } else if (temp_modeVal > 5) {
    temp_modeVal = (temp_modeVal) / 2;
  } else if (temp_modeVal > 2) {
    temp_modeVal = temp_modeVal - 3;
  } else {
  }
  if (temp_modeVal > WISE_CONCURRENT_MODE) {
    temp_modeVal = WISE_MODE_CONNECTION;
  }

  return temp_modeVal;
}

/*==============================================*/
/**
 *
 * @fn          reverse_translate_channel 
 * @brief       This function translate 802.11 channel to host represented channel.
 * @param[in]   channelNum, 802.11 channel number in 5GHz mode. 
 * @return      0, if ChannelNum is 0
 *              channelNum, host represented channel number
 *
 * @section description
 * This function translate 802.11 channel to host represented channel.
 *
 *
 */
uint8 reverse_translate_channel(uint8 channelNum)
{
  if (!channelNum) {
    return 0;
  }
  if (channelNum <= 64) {
    channelNum = ((channelNum - 32) / 4);
#ifdef DISABLE_CH144
  } else if ((channelNum >= 100) && (channelNum <= 140)) {
#else
  } else if ((channelNum >= 100) && (channelNum <= 144)) {
#endif
    channelNum = ((channelNum - 100) / 4) + 9;
#ifdef DISABLE_CH144
  } else if (channelNum >= 149) {
    channelNum = ((channelNum - 149) / 4) + 20;
#else
  } else {
    channelNum = ((channelNum - 149) / 4) + 21;
#endif
  } /* End if <condition> */

  return channelNum;
}

/*==============================================*/
/**
 *
 * @fn           translate_channel
 * @brief        This function translate host represented channel number to 802.11 channel number.
 * @param[in]    channelNum,host represented channel number  
 * @return       channel number in 5GHz mode
 *
 *
 * @section description
 * This function translate user represented channel number to 802.11 channel number in 5GHz.
 *
 *
 */

uint8 translate_channel(uint8 channelNum)
{
  if (channelNum) {
    if (channelNum < 9) {
      channelNum = (channelNum * 4 + 32);
#ifdef DISABLE_CH144
    } else if (channelNum < 20) {
#else
    } else if (channelNum < 21) {
#endif
      channelNum = ((channelNum - 9) * 4 + 100);
#ifdef DISABLE_CH144
    } else if (channelNum < 25) {
      channelNum = ((channelNum - 20) * 4) + 149;
#else
    } else if (channelNum < 26) {
      channelNum = ((channelNum - 21) * 4) + 149;
#endif
    } /* End if <condition> */
  }
  return channelNum;
}

#ifdef CONFIG_IEEE_80211J
uint8 valid_80211J_channel(uint32 channel)
{
  if (channel != 8 && channel != 12 && channel != 16 && channel != 184 && channel != 188 && channel != 192
      && channel != 196)
    return SL_FALSE;
  else
    return SL_TRUE;
}
#endif

uint16 get_frequency(uint16 band, uint16 rcv_channel, uint8 enabled_11j_japan_region)
{
  uint16 rcv_freq = 0;

  /*band is different and scan response is in different band if band is 5GHz
   * then only do reverse translation*/

  if (band == SL_BAND_2P4GHZ) {
    if (rcv_channel >= 1 && rcv_channel < 14) { /* TODO Need to take care of 5GHz channels also */
      rcv_freq = 2407 + rcv_channel * 5;
    } else if (rcv_channel == 14) {
      rcv_freq = 2484;
    } else {
      rcv_freq = 0;
    }
  }
#ifdef CONFIG_IEEE_80211J
  else if (enabled_11j_japan_region) {
    if (rcv_channel >= 8 && rcv_channel <= 16)
      return 5000 + rcv_channel * 5;
    else if (rcv_channel >= 183 && rcv_channel <= 196)
      return 4000 + rcv_channel * 5;
    else
      return 0;
  }
#endif
  else {
    rcv_channel = reverse_translate_channel(rcv_channel);

    if (rcv_channel >= 1 && rcv_channel <= 8) {
      rcv_freq = 5180 + ((rcv_channel - 1) * 20);
    } else if (rcv_channel >= 9 && rcv_channel <= 20) {
      rcv_freq = 5500 + ((rcv_channel - 9) * 20);
    } else if (rcv_channel >= 21 && rcv_channel <= 25) {
      rcv_freq = 5745 + ((rcv_channel - 21) * 20);
    } else {
      rcv_freq = 0;
    }
  }

  return rcv_freq;
}

int16 freq_to_bitmap_convert(int *freqs, uint16 *bitmap)
{
  int cnt = 0;

  while (*freqs != 0) {

    if ((*freqs >= 2412) && (*freqs <= 2462)) {
      bitmap[0] |= FREQ_BITMAP_MASK_2P4GHZ(*freqs);
    } else if ((*freqs >= 5180) && (*freqs <= 5825)) {
      *(uint32 *)&bitmap[1] |= FREQ_BITMAP_MASK_5GHZ(*freqs);
    }

    freqs++;
    cnt++;
  }

  if (cnt == 0) {
    bitmap[0] = 0xFFFF;
    bitmap[1] = 0xFFFF;
    bitmap[2] = 0xFFFF;
  }

  return cnt;
}

/*
   36 channel 1 (5180)
   40 channel 2 (5200)
   44 channel 3 (5220)
   48 channel 4 (5240)
   52 channel 5 (5260)
   56 channel 6 (5280)
   60 channel 7 (5300)
   64 channel 8 (5320)

   100 channel 9 (5500)
   104 channel 10 (5520)
   108 channel 11 (5540)
   112 channel 12 (5560)
   116 channel 13 (5580)
   120 channel 14 (5600)
   124 channel 15 (5620)
   128 channel 16 (5640)
   132 channel 17 (5660)
   136 channel 18 (5680)
   140 channel 19 (5700)

   149 channel 20 (5745)
   153 channel 21 (5765)
   157 channel 22 (5785)
   161 channel 23 (5805)
   165 channel 24 (5825
   */
uint16 get_channel_num(uint16 freq)
{
  uint16 channel = 0;

  if ((freq >= 2412) && (freq <= 2472)) {
    channel = ((freq - 2407) / 5);
  } else if (freq == 2484) {
    channel = 14;
  }
#ifdef CONFIG_IEEE_80211J
  else if ((freq >= 4915) && (freq < 5000)) {
    channel = ((freq - 4000) / 5);
    return channel;
  } else if ((freq >= 5000) && (freq < 5180)) {
    channel = ((freq - 5000) / 5);
    return channel;
  }
#endif
  else if ((freq >= 5180) && (freq <= 5320)) {
    channel = ((freq - 5160) / 20);
  } else if ((freq >= 5500) && (freq <= 5720)) {
    channel = ((freq - 5480) / 20) + 8;
  } else if ((freq >= 5745) && (freq <= 5825)) {
    channel = ((freq - 5725) / 20) + 20;
  }

  if (GET_FREQ_BAND(freq) == SL_BAND_5GHZ) {
    channel = translate_channel(channel);
  }

  return channel;
}

uint8 reverse_translate_80211j_channel(uint32 channel)
{
  switch (channel) {
    case 8:
      return 1;
    case 12:
      return 2;
    case 16:
      return 3;
    case 184:
      return 4;
    case 188:
      return 5;
    case 192:
      return 6;
    case 196:
      return 7;
    default:
      return 0;
  }
}

/*==============================================*/
/* @fn         check_inheritance
 * @brief      Checks whether element from T BSS is inherited by the NT BSS or
 * not.
 * @param[in]  ie, pointer to the start of the IEs
 * @param[in]  non_inherit_ie, pointer to the start of the non inheritence
 * element obtained from NT profile subelement.
 * @return     1 if element is inherited; 0 if not inherited.
 */
int8 check_inheritance(const struct element_s *ie, const struct element_s *non_inherit_ie)
{
  const uint8 *eid_list = NULL;
  uint8 id_list_len = 0, ext_id_list_len = 0, ii = 0;

  if (ie->id == IEEE80211_ELEMID_MULTIPLE_BSSID) {
    return 0;
  }

  if ((non_inherit_ie == NULL) || (non_inherit_ie->datalen < 3)) {
    return 1;
  }
  // Non-inheritance element has two list fields, Elements list field followed
  // by Extension elements list field.
  id_list_len     = non_inherit_ie->data[1];
  ext_id_list_len = non_inherit_ie->data[2 + id_list_len];
  // Each List field contains length of the list followed by list of element IDs
  // to be omitted in NT profile. Lengths are mandatory and lists are optional.
  if ((non_inherit_ie->datalen < 3 + id_list_len) || (non_inherit_ie->datalen < 3 + id_list_len + ext_id_list_len)) {
    return 1;
  }
  if (ie->id == IEEE80211_ELEMID_EXTENSION) {
    eid_list = &non_inherit_ie->data[3 + id_list_len];
    for (ii = 0; ii < ext_id_list_len; ii++) {
      if (eid_list[ii] == ie->data[0]) {
        return 0;
      }
    }
  } else {
    eid_list = &non_inherit_ie->data[2];
    for (ii = 0; ii < id_list_len; ii++) {
      if (eid_list[ii] == ie->id) {
        return 0;
      }
    }
  }

  return 1;
}
