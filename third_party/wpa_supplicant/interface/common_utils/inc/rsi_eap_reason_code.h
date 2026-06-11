/*******************************************************************************
* @file  rsi_eap_reason_code.h
* @brief This file contains EAP failure reason codes
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

#ifndef RSI_EAP_REASON_CODE_H
#define RSI_EAP_REASON_CODE_H

#include "sl_wlan_data_types.h"

extern uint8 reason_code_eap_failure;

enum reason_code_eap_failure_e {
  TLS_CA_CERT_NOT_PRESENT = 40,
  TLS_PRIV_KEY_NOT_PRESENT,
  TLS_CLIENT_CERT_NOT_PRESENT,
  TLS_NO_CERT_PRESENT,
  PEAP_CA_CERT_NOT_PRESENT,
  SERVER_CERT_INVALID_KEY_TYPE,
  SERVER_INT_ROOT_CA_INVALID_KEY_TYPE,
  SERVER_ROOT_CA_INVALID_KEY_TYPE,
  CLIENT_CERT_INVALID_KEY_TYPE,
  CLIENT_ROOT_CA_INVALID_KEY_TYPE,
  FIPS_SERVER_CERT_INVALID_LENGTH,
  FIPS_SERVER_INT_ROOT_CA_INVALID_LENGTH,
  FIPS_SERVER_ROOT_CA_INVALID_LENGTH,
  FIPS_CLIENT_CERT_INVALID_LENGTH,
  FIPS_CLIENT_ROOT_CA_INVALID_LENGTH,
  SERVER_CERT_INVALID_LENGTH,
  SERVER_INT_ROOT_CA_INVALID_LENGTH,
  SERVER_ROOT_CA_INVALID_LENGTH,
  CLIENT_CERT_INVALID_LENGTH,
  CLIENT_ROOT_CA_INVALID_LENGTH,
  SERVER_CERT_INVALID_SIGN_ALG,
  SERVER_INT_ROOT_CA_INVALID_SIGN_ALG,
  SERVER_ROOT_CA_INVALID_SIGN_ALG,
  CLIENT_CERT_INVALID_SIGN_ALG,
  CLIENT_ROOT_CA_INVALID_SIGN_ALG,
  SERVER_INT_ROOT_CA_NOT_PRESENT,
  SERVER_ROOT_CA_PARSE_ERR,
  SERVER_INT_ROOT_CA_PARSE_ERR,
  SERVER_CERT_PARSE_ERR,
  CLIENT_CERT_PARSE_ERR,
  INCORRECT_PRIVATE_KEY_PASSWORD,
  EAP_FAILURE_RECEIVED,
  CLIENT_CERT_BAD_DATE_ERR,
  SERVER_CERT_BAD_DATE_ERR,
  SERVER_ROOT_CA_BAD_DATE_ERR,
  CLIENT_ROOT_CA_BAD_DATE_ERR,
  SERVER_INT_ROOT_CA_BAD_DATE_ERR,
  PEM_HEADER_ERR,
  PEM_FOOTER_ERR,
  CLIENT_INT_ROOT_CA_INVALID_SIGN_ALG,
  CLIENT_INT_ROOT_CA_INVALID_LENGTH,
  FIPS_CLIENT_INT_ROOT_CA_INVALID_LENGTH,
  CLIENT_INT_ROOT_CA_INVALID_KEY_TYPE,
  PEM_ERR,
  CERT_PATHLEN_ERR
};

#endif /* RSI_EAP_REASON_CODE_H */
