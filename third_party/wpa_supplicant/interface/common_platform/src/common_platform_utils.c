/*******************************************************************************
* @file  common_platform_utils.c
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
#include "common_platform_utils.h"
#include "mgmt_if_core.h"

/* TODO: Needs EEPROM_MODULE_TYPE and MANF_DATA_BASE_ADDR */
uint8 cpf_get_module_band_type()
{
  return 0; // Returning 0 for 2.4GHz
}

/* TODO: EAP related fields needs flash base address to return */
uint32 cpf_get_cert_flash_addr()
{
  SL_MGMT_ASSERT(0);
  return 0;
}

uint32 cpf_get_eap_pac_file_addr()
{
  SL_MGMT_ASSERT(0);
  return 0;
}

uint32 cpf_get_max_eap_cert_len()
{
  SL_MGMT_ASSERT(0);
  return 0;
}

uint32 cpf_get_eap_pac_file_len_addr()
{
  SL_MGMT_ASSERT(0);
  return 0;
}

uint32 cpf_get_max_pac_file_len()
{
  SL_MGMT_ASSERT(0);
  return 0;
}

uint32 cpf_get_eap_priv_key_len_addr()
{
  SL_MGMT_ASSERT(0);
  return 1;
}

uint32 cpf_get_priv_key_pattern()
{
  SL_MGMT_ASSERT(0);
  return 1;
}

uint32 cpf_get_pem_file_pattern()
{
  SL_MGMT_ASSERT(0);
  return 1;
}

uint32 cpf_get_eap_ca_cert_len_addr()
{
  SL_MGMT_ASSERT(0);
  return 1;
}

uint32 cpf_get_ca_key_pattern()
{
  SL_MGMT_ASSERT(0);
  return 1;
}

uint32 cpf_get_eap_public_key_len_addr()
{
  SL_MGMT_ASSERT(0);
  return 1;
}

uint32 cpf_get_public_key_pattern()
{
  SL_MGMT_ASSERT(0);
  return 1;
}

uint32 cpf_get_eap_cert_len_addr()
{
  SL_MGMT_ASSERT(0);
  return 1;
}

uint32 cpf_get_eap_cert_addr()
{
  SL_MGMT_ASSERT(0);
  return 1;
}

uint32 cpf_get_eap_public_key_addr()
{
  SL_MGMT_ASSERT(0);
  return 1;
}

uint32 cpf_get_eap_private_key_addr()
{
  SL_MGMT_ASSERT(0);
  return 1;
}

uint32 cpf_get_eap_ca_cert_addr()
{
  SL_MGMT_ASSERT(0);
  return 1;
}

/* TODO: define these function as these uses flash base address to write and erase memory*/
void write_on_flash_memory(uint8 *dst, uint8 *src, uint32 len)
{
  SL_MGMT_ASSERT(0);
  return;
}

void erase_flash_memory(uint32 erase_start_addr, uint32 erase_len)
{
  SL_MGMT_ASSERT(0);
  return;
}

void *sli_supplicant_interface_driver_init(void *ctx, const char *ifname)
{
  return (void *)&mgmt_if_adapter;
}
