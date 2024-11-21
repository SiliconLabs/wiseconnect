/**
 * @file sl_wifi_credentials.h
 * @brief This file defines the Credential Management for Wi-Fi operations.
 *
 * This file contains the definitions and structures used for managing Credentials.
 * It includes enumerations for buffer types and structures for Credential management.
 *
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 *
 * (c) Copyright 2024, Silicon Laboratories Inc. All rights reserved.
 */

#ifndef SL_WIFI_CREDENTIALS_H
#define SL_WIFI_CREDENTIALS_H

#include "sl_wifi_types.h"

/** 
 * \addtogroup WIFI_CREDENTIAL_FUNCTIONS Wi-Fi Credential
 * \ingroup SL_WIFI_FUNCTIONS
 * @{ */

/***************************************************************************/ /**
 * @brief
 *   Store a credential in specified credential identifier.
 * 
 * @details
 *   This function stores the credential type and data for the specified credential ID.
 *   The credential data can include client credentials, access point credentials and user credentials.
 * 
 *   Repeatedly calling this API with the same ID will overwrite the existing credential type and data.
 *  
 * @pre Pre-conditions:
 * - @ref sl_wifi_init should be called before this API.
 * 
 * @param[in] id
 *   Credential identifier as identified by @ref sl_wifi_credential_id_t.
 * 
 * @param[in] type
 *   Credential type as identified by @ref sl_wifi_credential_type_t.
 * 
 * @param[in] credential
 *   Pointer to the credential data object.
 * 
 * @param[in] credential_length
 *   Length of the credential data object.
 * 
 * @return
 *   sl_status_t. See [Status Codes](https://docs.silabs.com/gecko-platform/latest/platform-common/status) and [Additional Status Codes](../wiseconnect-api-reference-guide-err-codes/sl-additional-status-errors) for details.
 ******************************************************************************/
sl_status_t sl_wifi_set_credential(sl_wifi_credential_id_t id,
                                   sl_wifi_credential_type_t type,
                                   const void *credential,
                                   uint32_t credential_length);

/***************************************************************************/ /**
 * @brief
 *   Retrieve a stored credential.
 * 
 * @details
 *   This function retrieves the credential data for the specified credential ID.
 *   The retrieved credential data is stored in the provided credential object.
 * 
 * @pre Pre-conditions:
 * - @ref sl_wifi_init should be called before this API.
 * 
 * @param[in] id
 *   Credential identifier as identified by @ref sl_wifi_credential_id_t.
 * 
 * @param[out] type
 *   Credential type as identified by @ref sl_wifi_credential_type_t.
 * 
 * @param[out] credential
 *   Pointer to location where credential data is stored.
 * 
 * @param[in,out] credential_length
 *   in: Number of bytes available at credential,
 *   out: Number of bytes written.
 * 
 * @return
 *   sl_status_t. See [Status Codes](https://docs.silabs.com/gecko-platform/latest/platform-common/status) and [Additional Status Codes](../wiseconnect-api-reference-guide-err-codes/sl-additional-status-errors) for details.
 ******************************************************************************/
sl_status_t sl_wifi_get_credential(sl_wifi_credential_id_t id,
                                   sl_wifi_credential_type_t *type,
                                   void *credential,
                                   uint32_t *credential_length);

/***************************************************************************/ /**
 * @brief
 *   Delete a stored credential.
 * 
 * @details
 *   This function deletes the credential data for the specified credential ID and type.
 *   Once deleted, the credential cannot be used for any operations.
 * 
 * @pre Pre-conditions:
 * -
 *   @ref sl_wifi_init should be called before this API.
 * 
 * @param[in] id
 *   Credential identifier as identified by @ref sl_wifi_credential_id_t.
 * 
 * @return
 *   sl_status_t. See [Status Codes](https://docs.silabs.com/gecko-platform/latest/platform-common/status) and [Additional Status Codes](../wiseconnect-api-reference-guide-err-codes/sl-additional-status-errors) for details.
 ******************************************************************************/
sl_status_t sl_wifi_delete_credential(sl_wifi_credential_id_t id);

/** @} */

#endif //SL_WIFI_CREDENTIALS_H