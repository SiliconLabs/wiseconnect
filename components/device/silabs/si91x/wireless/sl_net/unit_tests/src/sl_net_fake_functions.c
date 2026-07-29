/*******************************************************************************
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "sl_net_fake_functions.h"

/* Headers for link-time stubs used by sl_net_si91x_integration_handler.c */
#include "cmsis_os2.h"
#include "sli_cmsis_os2_ext_task_register.h"
#include "sli_wifi_device_core_utilities.h"
#include "sl_si91x_types.h"
#include "sl_si91x_socket_callback_framework.h"
/* Do not include sli_wifi_utility.h here: it redeclares sli_wifi_host_get_buffer_data with a
 * void* buffer type and conflicts with the fff fake declared in sl_net_fake_functions.h. */

DEFINE_FFF_GLOBALS;

DEFINE_FAKE_VALUE_FUNC4(sl_status_t, sl_si91x_wifi_set_certificate_index, uint8_t, uint8_t, const void *, uint32_t);
DEFINE_FAKE_VALUE_FUNC1(uint8_t, sli_get_certificate_index, sl_net_credential_id_t);
DEFINE_FAKE_VALUE_FUNC1(int, sli_net_check_cred_type, sl_net_credential_type_t);
DEFINE_FAKE_VALUE_FUNC2(sli_si91x_cert_type_t,
                        convert_to_si91x_cert_type,
                        sl_net_credential_id_t,
                        sl_net_credential_type_t);
DEFINE_FAKE_VALUE_FUNC0(sl_wifi_operation_mode_t, sli_wifi_get_opermode);
DEFINE_FAKE_VALUE_FUNC2(sl_status_t, sli_convert_si91x_event_to_sl_net_event, const uint16_t, sl_net_event_t);
DEFINE_FAKE_VALUE_FUNC2(sl_status_t, sli_si91x_vap_shutdown, uint8_t, sli_si91x_bsd_disconnect_reason_t);
DEFINE_FAKE_VALUE_FUNC1(bool, sli_wifi_is_ip_address_zero, const sl_ip_address_t *);
DEFINE_FAKE_VALUE_FUNC2(sl_status_t, sl_si91x_default_handler, sl_net_event_t, sl_wifi_buffer_t *);
DEFINE_FAKE_VALUE_FUNC3(void *, sli_wifi_host_get_buffer_data, sl_wifi_buffer_t *, uint16_t, uint16_t *);
DEFINE_FAKE_VALUE_FUNC1(sl_wifi_buffer_t *, sli_wifi_get_response_buffer, sli_command_engine_response_t *);
DEFINE_FAKE_VALUE_FUNC1(sli_command_engine_metadata_t *,
                        sli_wifi_get_response_metadata,
                        sli_command_engine_response_t *);
DEFINE_FAKE_VALUE_FUNC1(bool, sli_wifi_is_interface_up, sl_wifi_interface_t);
DEFINE_FAKE_VALUE_FUNC7(sl_status_t,
                        sli_wifi_send_command,
                        uint32_t,
                        sli_wifi_command_type_t,
                        const void *,
                        uint32_t,
                        sli_wifi_wait_period_t,
                        void *,
                        void **);
DEFINE_FAKE_VALUE_FUNC1(sl_status_t, sli_buffer_manager_free_buffer, sli_buffer_t);

/***************************************************************************/ /**
 * Stubs for symbols referenced by sl_net_si91x_integration_handler.c that are
 * normally provided by sli_wifi / OS layers but are not linked in this target.
 ******************************************************************************/
#ifndef __ZEPHYR__
sli_task_register_id_t sli_fw_status_storage_index = SLI_FW_STATUS_STORAGE_INVALID_INDEX;

sl_status_t sli_osTaskRegisterSetValue(const osThreadId_t thread_id,
                                       const sli_task_register_id_t reg_id,
                                       const uint32_t value)
{
  (void)thread_id;
  (void)reg_id;
  (void)value;
  return SL_STATUS_OK;
}
#endif

uint16_t sli_wifi_get_wifi_frame_status(const sl_wifi_system_packet_t *packet)
{
  (void)packet;
  return (uint16_t)SL_STATUS_OK;
}

sl_status_t sli_si91x_socket_event_handler(sl_status_t status,
                                           sli_si91x_socket_context_t *sdk_context,
                                           sl_wifi_system_packet_t *rx_packet)
{
  (void)status;
  (void)sdk_context;
  (void)rx_packet;
  return SL_STATUS_OK;
}
