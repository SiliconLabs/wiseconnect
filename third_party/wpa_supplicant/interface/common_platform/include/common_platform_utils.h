/*******************************************************************************
* @file  common_platform_utils.h
* @brief This file contains functions and global variables that are common and 
*        platform specific.
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
#ifndef COMMON_PLATFORM_UTILS_H
#define COMMON_PLATFORM_UTILS_H

#include "sl_wlan_data_types.h"
#include "wlan_user_command.h"
#include "sl_core.h"
#include "sl_mgmt_if_thread.h"

#ifdef PS_DEPENDENT_VARIABLES
#define SYSTEM_CLOCK _tx_timer_system_clock
#elif SUPP_RTOS_PORTING
#define SYSTEM_CLOCK osKernelGetTickCount();
#else
#define SYSTEM_CLOCK 0
#endif
#define RCV_BUF_IND 1

/* TODO: redefine this function if support is added at NHCP level*/
#define cpf_sl_is_buffers_available() 1
#define cpf_get_rx_head_room()        (sizeof(sl_wlan_mgmt_if_rx_pkt_t))
#define cpf_get_rx_frame_desc_size()  (HOST_DESC_LENGTH)
#define cpf_sl_pkb_alloc(pkt_type)    sli_nhcp_allocate_pkt(pkt_type, WLAN_MGMT_IF_HIGH_PRIORITY)

#define CPF_CORE_ENTER_ATOMIC() \
  CORE_DECLARE_IRQ_STATE;       \
  CORE_ENTER_ATOMIC();

#define CPF_CORE_EXIT_ATOMIC() CORE_EXIT_ATOMIC();

uint32 cpf_get_cert_flash_addr();
uint8 cpf_get_module_band_type();
uint32 cpf_get_eap_pac_file_addr();
uint32 cpf_get_max_eap_cert_len();
uint32 cpf_get_eap_pac_file_len_addr();
uint32 cpf_get_max_pac_file_len();
uint32 cpf_get_eap_priv_key_len_addr();
uint32 cpf_get_priv_key_pattern();
uint32 cpf_get_pem_file_pattern();
uint32 cpf_get_eap_ca_cert_len_addr();
uint32 cpf_get_ca_key_pattern();
uint32 cpf_get_eap_public_key_len_addr();
uint32 cpf_get_public_key_pattern();
uint32 cpf_get_eap_cert_len_addr();
uint32 cpf_get_eap_cert_addr();
uint32 cpf_get_eap_public_key_addr();
uint32 cpf_get_eap_private_key_addr();
uint32 cpf_get_eap_ca_cert_addr();
void write_on_flash_memory(uint8 *dst, uint8 *src, uint32 len);
void erase_flash_memory(uint32 erase_start_addr, uint32 erase_len);
void *sli_supplicant_interface_driver_init(void *ctx, const char *ifname);
#endif /* COMMON_PLATFORM_UTILS_H */
