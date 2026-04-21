/*******************************************************************************
* @file  scan_results.c
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

#include "mgmt_supplicant_if.h"
#include "sl_wlan_global.h"
#include "mgmt_if_core.h"
#include "mgmt_if_utils.h"
#include "ieee_80211_utils.h"

/*==============================================*/
/* @fn        derive_nt_bssid
 * @brief     This function derives Non Transmitted(NT) BSSID for a given
 * Transmitted(Tx) BSSID
 * @param[in] t_bssid, Pointer to Tx BSSID
 * @param[in] max_bssid, max_bssid indicator.
 * @param[in] mbssid_inx, mbssid index of an NT profile.
 * @param[out] nt_bssid, pointer to the derived NT BSSID.
 * @return    void
 */
void derive_nt_bssid(uint8 *t_bssid, uint8 max_bssid, uint8 mbssid_inx, uint8 *nt_bssid)
{
  uint8 B  = 0;
  uint16 N = 1 << max_bssid;
  if (t_bssid == NULL)
    return;
  B = (t_bssid[5] % N);
  sl_memcpy(nt_bssid, t_bssid, 6);
  nt_bssid[5] = t_bssid[5] - B + ((B + mbssid_inx) % N);
}

/*==============================================*/
/* @fn         get_non_tx_profile_continuation
 * @brief      Extracts continuation of an NT profile if it exists.
 * @param[in]  ie, pointer to the start of the IEs
 * @param[in]  ie_len, length of all the IEs.
 * @param[in]  mb_ie, pointer to the start of mbssid element
 * @param[in]  sub_ie, pointer to the start of the NT profile whose continuation
 * is to be found.
 * @return     pointer to the start of the continuation of the given NT profile.
 */
static const struct element_s *get_non_tx_profile_continuation(const uint8 *ie,
                                                               uint16 ie_len,
                                                               const struct element_s *mb_ie,
                                                               const struct element_s *sub_ie)
{
  const uint8 *mb_ie_end            = mb_ie->data + mb_ie->datalen;
  const struct element_s *next_mbie = NULL, *next_nt_profile = NULL;

  next_mbie =
    (const struct element_s *)get_ie_wrapper(mb_ie_end, ie_len - (mb_ie_end - ie), IEEE80211_ELEMID_MULTIPLE_BSSID);

  // return NULL if profile is complete or if there is no valid next mbssid
  // element.
  if ((sub_ie->data + sub_ie->datalen < mb_ie_end - 1) || (next_mbie == NULL) || (next_mbie->datalen < 4)) {
    return NULL;
  }
  next_nt_profile = (struct element_s *)&next_mbie->data[1];

  // check if the next_nt_profile is valid and return NULL if nt profile length
  // exceeds mbssid element length.
  if ((next_mbie->data + next_mbie->datalen < next_nt_profile->data + next_nt_profile->datalen)
      || (next_nt_profile->id != 0) || (next_nt_profile->datalen < 2)) {
    return NULL;
  }
  // NT capabilities is always the first element in an NT profile.
  // If NT caps are not present then it is a continuation of previous NT
  // profile.
  return next_nt_profile->data[0] == IEEE80211_ELEMID_NONTX_BSSID_CAP ? NULL : next_mbie;
}

/*==============================================*/
/* @fn         merge_nt_profile
 * @brief      merges a NT profile if it is split across two mbssid elements.
 * @param[in]  ies_list, pointer to the start of the IEs
 * @param[in]  ies_len, length of all the IEs.
 * @param[in]  mb_ie, pointer to the start of mbssid element
 * @param[in]  sub_elem, pointer to the start of the NT profile whose
 * continuation is to be found.
 * @param[out] complete_profile, pointer to the merged NT profile
 * @param[in]  copy_max_len, the maximum allowed copy length
 * @return     length of the merged profile.
 */
uint16 merge_nt_profile(const uint8 *ies_list,
                        uint16 ies_len,
                        const struct element_s *mb_ie,
                        const struct element_s *sub_ie,
                        uint8 *complete_profile)
{
  const struct element_s *next_mbie   = mb_ie;
  const struct element_s *next_sub_ie = sub_ie;
  uint16 nt_profile_len               = 0;

  // This loop iterates and merges all the continuations of an NT subprofile
  // element if such continuations exist.
  do {
    if (nt_profile_len + next_sub_ie->datalen > ies_len) {
      break;
    }
    sl_memcpy(complete_profile + nt_profile_len, (uint8 *)next_sub_ie->data, next_sub_ie->datalen);
    nt_profile_len += next_sub_ie->datalen;
  } while ((next_mbie = get_non_tx_profile_continuation(ies_list, ies_len, next_mbie, next_sub_ie))
           && (next_sub_ie = (struct element_s *)&next_mbie->data[1]));
  return nt_profile_len;
}

/*==============================================*/
/* @fn         generate_nt_bss
 * @brief      generate NT IEs list from T and NT subelements.
 * @param[in]  ie, pointer to the start of the IEs
 * @param[in]  ie_len, length of all the IEs.
 * @param[in]  sub_elem, pointer to the start of the NT profile whose
 * continuation is to be found.
 * @param[in]  subie_len, length of the NT subelement.
 * @param[out] new_bss, the generated NT IEs.
 * @return     length of the generated NT IEs.
 *
 * @section description
 * pos is a pointer to a location in new_bss to copy the element at.
 * ie_ptr points to an information element
 * tx_ie_ptr points to an information element in the Tx BSS
 * ntx_ie_ptr points to an information element in the NTx Profile
 * sub_dup is copy of NT profile
 */
uint16 generate_nt_bss(const uint8 *ie, uint16 ie_len, const uint8 *sub_elem, uint16 subie_len, uint8 *new_bss)
{
  uint8 *pos = NULL, *ie_ptr = NULL;
  const uint8 *tx_ie_ptr = NULL, *ntx_ie_ptr = NULL;
  const struct element_s *non_inherit_ie = NULL;

  pos = &new_bss[0];
  // copy elements from NT profile to new_bss except NT caps ie, ssid ie and
  // mbssid index ie.
  ntx_ie_ptr = sub_elem;
  while (ntx_ie_ptr + ntx_ie_ptr[1] + 2 - sub_elem <= subie_len) {
    if (!((ntx_ie_ptr[0] == IEEE80211_ELEMID_NONTX_BSSID_CAP)
          || (ntx_ie_ptr[0] == IEEE80211_ELEMID_MULTIPLE_BSSID_INDEX)
          || (ntx_ie_ptr[0] == IEEE80211_ELEMID_EXTENSION && ntx_ie_ptr[2] == IEEE80211_ELEMID_EXT_NON_INHERITANCE))) {
      sl_memcpy(pos, (uint8 *)ntx_ie_ptr, ntx_ie_ptr[1] + 2);
      pos += ntx_ie_ptr[1] + 2;
    }
    if (ntx_ie_ptr + ntx_ie_ptr[1] + 2 - sub_elem == subie_len) {
      break;
    }
    ntx_ie_ptr += ntx_ie_ptr[1] + 2;
  }
  // getting non inheritance element.
  non_inherit_ie =
    (const struct element_s *)get_ie_ext_wrapper(sub_elem, subie_len, IEEE80211_ELEMID_EXT_NON_INHERITANCE);

  tx_ie_ptr = get_ie_wrapper(ie, ie_len, IEEE80211_ELEMID_SSID);
  tx_ie_ptr = (tx_ie_ptr) ? tx_ie_ptr + tx_ie_ptr[1] + 2 : ie;

  while (tx_ie_ptr + tx_ie_ptr[1] + 2 - ie <= ie_len) {
    if (tx_ie_ptr[0] == IEEE80211_ELEMID_SSID) {
      tx_ie_ptr += tx_ie_ptr[1] + 2;
      continue;
    }
    if (tx_ie_ptr[0] == IEEE80211_ELEMID_EXTENSION)
      ie_ptr = (uint8 *)get_ie_ext_wrapper(sub_elem, subie_len, tx_ie_ptr[2]);
    else
      ie_ptr = (uint8 *)get_ie_wrapper(sub_elem, subie_len, tx_ie_ptr[0]);

    if (ie_ptr == NULL) {
      const struct element_s *old_ie = (struct element_s *)tx_ie_ptr;
      if (check_inheritance(old_ie, non_inherit_ie)) {
        // ie is in T profile but not in NT profile and it is inherited.
        sl_memcpy(pos, (uint8 *)tx_ie_ptr, tx_ie_ptr[1] + 2);
        pos += tx_ie_ptr[1] + 2;
      }
    } else {
      // IE is present in both T and NT copy from NT.
      // If vendor specific IE compare T and NT IE. If same copy from NT. If
      // different, copy from T.
      if (tx_ie_ptr[0] == IEEE80211_ELEMID_VENDOR) {
        if (sl_memcmp((uint8 *)tx_ie_ptr + 2, ie_ptr + 2, 5) != 0) {
          sl_memcpy(pos, (uint8 *)tx_ie_ptr, tx_ie_ptr[1] + 2);
          pos += tx_ie_ptr[1] + 2;
        }
      }
    }
    if (tx_ie_ptr + tx_ie_ptr[1] + 2 - ie == ie_len)
      break;
    tx_ie_ptr += tx_ie_ptr[1] + 2;
  }

  return pos - new_bss;
}