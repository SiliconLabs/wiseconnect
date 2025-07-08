/*******************************************************************************
* @file  sl_supplicant.h
* @brief This file contains declaration for the functions that are defined
*        in proprietary files created and placed in supplicant folder.
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
#ifndef SL_SUPPLICANT_H
#define SL_SUPPLICANT_H

void wps_mem_init(uint32 size);

void rsi_update_roam_params(void *wpa_s, uint8 roam_threshold, uint8 roam_hysterisis);

void generate_pmk(uint8 *passphrase, uint8 *ssid, uint16 ssid_len, uint8 *pmk);

int wise_p2p_join(struct wpa_supplicant *wpa_s, void *dev);
#endif /* SL_SUPPLICANT_H */
