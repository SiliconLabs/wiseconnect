/*******************************************************************************
* @file  rsi_bt_common_apis.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef RSI_BT_COMMON_APIS_H
#define RSI_BT_COMMON_APIS_H

#include "rsi_bt_common.h"
#include "rsi_utils.h"

/******************************************************
 * *                      Macros
 * ******************************************************/
// success return value
#define RSI_SUCCESS 0

// failure return value
#define RSI_FAILURE -1

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/

/******************************************************
 * *                 Type Definitions
 * ******************************************************/

/******************************************************
 * *                    Structures
 * ******************************************************/

/******************************************************
 * *                 Global Variables
 * ******************************************************/

/******************************************************
 * *              BT Common API's Declarations
 * ******************************************************/
/** @addtogroup BT-BLE
* @{
*/
#ifdef __cplusplus
extern "C" {
#endif
/*==============================================*/
/**
 * @fn          int32_t rsi_bt_set_bd_addr(uint8_t *dev_addr)
 * @brief       Set the device BD address. This is a blocking API.
 * @pre         needs to be called immediately after device initialization.
 * @param[in]   dev_addr - public address of the device to be set \n
 * @return      0 		 - Success \n
 *              Non-Zero Value - Failure \n
 *              -3       - Command is given in wrong state(i.e not immediate after opermode)
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 */
int32_t rsi_bt_set_bd_addr(uint8_t *dev_addr);

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_ber_enable_or_disable(rsi_bt_ber_cmd_t *ber_cmd)
 * @brief      Enable and disable BER. This is a blocking API.
 * @param[in]  ber_cmd - pointer address which contains the command structure. Please refer rsi_bt_ber_cmd_s structure for more info \n
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 * @note       Refer Error Codes section for above error codes \ref error-codes.
 */
int32_t rsi_bt_ber_enable_or_disable(rsi_bt_ber_cmd_t *ber_cmd);

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_set_local_name(uint8_t *local_name)
 * @brief      Set the given name to local device. This is a blocking API.
 * @pre        Device should be initialized before calling this API.
 * @param[in]  local_name - Name to be set to the local device.
 * @note       For BLE alone Opermode : When the name of the local device is set to a value with length more than 16 bytes then error is returned with an error code 0x4E66.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 * @note       Refer Error Codes section for above error codes \ref error-codes.
 *
 */
int32_t rsi_bt_set_local_name(uint8_t *local_name);

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_cmd_update_gain_table_offset_or_max_pwr(uint8_t node_id, uint8_t payload_len, uint8_t *payload, uint8_t req_type)
 * @brief      Update gain table offset/max power. This is blocking API.
 * @pre        Device should be initialized before calling this API.
 * @param[in]  node_id     - Node ID (0 - BLE, 1 - BT).
 * @param[in]  payload_len - Length of the payload.
 * @param[in]  payload     - Payload containing table data of gain table offset/max power
 * @param[in]  req_type    - update gain table request type (0 - max power update, 1 - offset update)
 * @return     0		-	Success \n
 *             0x4F01		-	Invalid gain table payload length \n
 *             0x4F02		-	Invalid region. \n
 *             0x4F03		-	Invalid gain table offset request type \n
 *             0x4F04           -       Invalid node id. \n
 * @note       Refer Error Codes section for above error codes \ref error-codes.
 *
 */
int32_t rsi_bt_cmd_update_gain_table_offset_or_max_pwr(uint8_t node_id,
                                                       uint8_t payload_len,
                                                       uint8_t *payload,
                                                       uint8_t req_type);

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_get_local_name(rsi_bt_resp_get_local_name_t *bt_resp_get_local_name)
 * @brief      Get the local device name. This is a blocking API.
 * @pre        Device should be initialized before calling this API.
 * @param[out] bt_resp_get_local_name - This parameter is the response buffer to hold the response of this API. Please refer rsi_bt_resp_get_local_name_s structure for more info.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 * @note       Refer Error Codes section for above error codes \ref error-codes.
 */
int32_t rsi_bt_get_local_name(rsi_bt_resp_get_local_name_t *bt_resp_get_local_name);

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_get_rssi(uint8_t *dev_addr, int8_t *resp)
 * @brief      Get the RSSI of the remote device. This is a blocking API.
 * @pre        \ref rsi_bt_connect() API need to be called before this API.
 * @param[in]  dev_addr -  Remote device address.
 * @param[out] resp - Parameter to hold the response of this API, RSSI is filled in this resp parameter.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 * @note       Refer Error Codes section for above error codes \ref error-codes.
 */
int32_t rsi_bt_get_rssi(uint8_t *dev_addr, int8_t *resp);

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_get_local_device_address(uint8_t *resp)
 * @brief      Get the local device address. This is a blocking API.
 * @pre        Device should be initialized before calling this API.
 * @param[out] resp - Parameter to hold the response of this API, local bd_addr is filled in this resp parameter.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 * @note       Refer Error Codes section for above error codes \ref error-codes.
 */
int32_t rsi_bt_get_local_device_address(uint8_t *resp);

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_get_bt_stack_version(rsi_bt_resp_get_bt_stack_version_t *bt_resp_get_bt_stack_version)
 * @brief      Get the BT stack version. This is a blocking API.
 * @param[out] bt_resp_get_bt_stack_version -  Response buffer to hold the response of this API. Please refer rsi_bt_resp_get_bt_stack_version_s structure for more info
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 * @note       Refer Error Codes section for above error codes \ref error-codes.
 */
int32_t rsi_bt_get_bt_stack_version(rsi_bt_resp_get_bt_stack_version_t *bt_resp_get_bt_stack_version);

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_init(void)
 * @brief      Initialize the BT device. This is a blocking API.
 * @pre        Device should be initialized before calling this API.  \n
 *             If the device is in powersave, get back the device to ACTIVE MODE by using \ref rsi_bt_power_save_profile()
 * @param      None
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 * @note       Refer Error Codes section for above error codes \ref error-codes.
 */
int32_t rsi_bt_init(void);

/*==============================================*/
/**
 * @fn         int32_t 	rsi_bt_deinit(void)
 * @brief      Deinitialize the BT device. This is a blocking API.
 * @pre        \ref Device should be initialized before this API. \n
 * @param      None
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 * @note       Refer Error Codes section for above error codes \ref error-codes.
 */
int32_t rsi_bt_deinit(void);

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_set_antenna(uint8_t antenna_value)
 * @brief      Select either internal / external antenna on the chip. This is a blocking API.
 * @pre        Device should be initialized before calling this API.
 * @param[in]  antenna_value - Parameter is used to select either internal or external antenna. Possible values: \n
 *                             0x00 RSI_SEL_INTERNAL_ANTENNA \n
 *                             0x01 RSI_SEL_EXTERNAL_ANTENNA
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 * @note       Refer Error Codes section for above error codes \ref error-codes.
 */
int32_t rsi_bt_set_antenna(uint8_t antenna_value);

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_set_antenna_tx_power_level(uint8_t protocol_mode, int8_t tx_power)
 * @brief      Enable/Disable the mentioned features. This is a blocking API.
 * @pre        Device should be initialized before calling this API
 * @param[in]  protocol_mode - protocol mode \n
 *             1 - BT classic \n
 *			   2 - BT Low Energy
 * @param[in] tx-power - power value \n
 *             Antenna transmit power level \n
 *             Default Value for BLE TX Power Index is 31, The range for the BLE TX Power Index is 1 to 75 (0, 32 indexes are invalid) \n
 *             Default TX power index for BT classic is 14 \n
 *                      1 - 31     0DBM Mode.  \n
 *                     33 - 63     10DBM Mode. \n
 *                     64 - 75     HP Mode.    \n
 * @note       The default value will vary based on country region and board \n
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 * @note       Refer Error Codes section for above error codes \ref error-codes.
 */
int32_t rsi_bt_set_antenna_tx_power_level(uint8_t protocol_mode, int8_t tx_power);

/*==============================================*/
/**
 * @fn          int32_t rsi_bt_power_save_profile(uint8_t psp_mode, uint8_t psp_type)
 * @brief       Select the power save profile mode for BT / BLE. This is a blocking API.
 * @pre         Device should be initialized before calling this API
 * @param[in]   psp_mode Following psp_mode is defined. \n
 *              0 - RSI_ACTIVE. In this mode module is active and power save is disabled. \n
 *              1 - RSI_SLEEP_MODE_1. On mode. In this sleep mode, SoC will never turn off, therefore no \n
 *                  handshake is required before sending data to the module. BT/BLE does not support this mode. \n
 *              2 - RSI_SLEEP_MODE_2. Connected sleep mode. In this sleep mode, SoC will go to sleep based \n
 *                  on GPIO or Message, therefore handshake is required before sending data to the module. \n
 *              8 - RSI_SLEEP_MODE_8 :Deep sleep mode with RAM RETENTION. \n
 *              10- RSI_SLEEP_MODE_10 : Deep sleep mode without RAM RETENTION. \n
 *                  In this sleep mode, module will turn off the \n
 *                  SoC. Since SoC is turn off, therefore handshake is required before sending data to the module. \n
 * @param[in]   psp_type Following psp_type is defined. \n
 *              0 - RSI_MAX_PSP. This psp_type will be used for max power saving \n
 *              1 - Fast PSP \n
 *              2 - UAPSD
 * @return      0		-	Success \n
 *              Non-Zero Value	-	Failure \n
 * @note        If the user wants to enable power save in CoEx mode (WLAN + BT LE) mode - It is mandatory to enable WLAN power save along with BT LE power save. \n
 * @note        The device will enter into power save if and only if both protocol (WLAN, BLE) power save modes are enabled. \n
 * @note       Refer Error Codes section for above error codes \ref error-codes.
 * @note        psp_type is only valid in psp_mode 2. \n
 *              BT/BLE does not support in RSI_SLEEP_MODE_1. \n
 *              BT/BLE supports only RSI_MAX_PSP mode. Remaining modes are not supported.
 *
 */
int32_t rsi_bt_power_save_profile(uint8_t psp_mode, uint8_t psp_type);

/*==============================================*/
/**
 * @fn          int32_t rsi_bt_per_stats(uint8_t cmd_type, struct rsi_bt_per_stats_s *rsi_bt_per_stats)
 * @brief       Request the local device for BT PER operation.
 * @pre         Call \ref rsi_bt_per_tx() or rsi_bt_per_rx() before calling this API.
 * @param[in]   cmd_type - Parameter to define the command id type for PER operation. \n
 *              BT_PER_STATS_CMD_ID (0x08) - Command id enables PER statistics \n
 *              BT_TRANSMIT_CMD_ID (0x15) - Command id enables PER transmit \n
 *              BT_RECEIVE_CMD_ID (0x16) - Command id enables PER receive
 * @param[in]   rsi_bt_per_stats - reference to the response structure. Please refer to rsi_bt_per_stats_s structure for more info.
 * @return      0			-	Success \n
 *              Non-Zero Value	-	Failure
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 */
int32_t rsi_bt_per_stats(uint8_t cmd_type, struct rsi_bt_per_stats_s *per_stats);

/*==============================================*/
/**
 * @fn          int32_t rsi_bt_per_cw_mode(struct rsi_bt_per_cw_mode_s *bt_cw_mode)
 * @brief       Keep the device in continuous wave mode. \n
 *              Not necessary to be called if transmit_mode is set to 2 - CONTINUOUS_WAVE_MODE in \ref rsi_bt_tx_per_params_s structure.
 * @pre         Call \ref rsi_bt_per_tx() before calling this API.
 * @param[in]   bt_cw_mode - continuous wave mode information needs to filled in this structure. Please refer \ref rsi_bt_per_cw_mode_s for more info.
 * @return      0			-	Success \n
 *              Non-Zero Value	-	Failure
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 */
int32_t rsi_bt_per_cw_mode(struct rsi_bt_per_cw_mode_s *bt_cw_mode);

/** @} */

/*==============================================*/
/**
 * @fn         rsi_bt_set_feature_bitmap
 *
 */
int32_t rsi_bt_set_feature_bitmap(uint32_t feature_bit_map);

#ifdef __cplusplus
}
#endif

#endif
