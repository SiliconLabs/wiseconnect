/***************************************************************************/ /**
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
#include "sli_net_common_utility.h"
#include "sl_net.h"
#include "sl_net_constants.h"
#include "cmsis_os2.h"
#include "sl_net_types.h"
#include "sl_net_wifi_types.h"
#include "sl_net_si91x.h"
#include "sl_wifi.h"
#include "string.h"
#ifdef SLI_SI91X_LWIP_HOSTED_NETWORK_STACK
#include "sl_net_for_lwip.h"
#endif
#include "sli_wifi_constants.h"

sl_net_event_handler_t net_event_handler = NULL;
static osEventFlagsId_t auto_join_event_flag;
static osThreadId_t network_manager_id          = NULL;
const osThreadAttr_t network_manager_attributes = {
  .name       = "network_manager",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 2304,
  .priority   = osPriorityNormal,
  .tz_module  = 0,
  .reserved   = 0,
};
osMessageQueueId_t network_manager_queue;
const osMessageQueueAttr_t network_manager_queue_attributes = { .name = "network_manager_queue" };
extern sl_net_event_handler_t net_event_handler;
#ifdef SLI_SI91X_LWIP_HOSTED_NETWORK_STACK
extern sl_net_wifi_lwip_context_t *wifi_client_context;
#endif

sl_status_t sli_net_register_event_handler(sl_net_event_handler_t function)
{
  net_event_handler = function;
  return SL_STATUS_OK;
}

sl_status_t sli_handle_auto_join(sl_net_interface_t interface, sl_net_wifi_client_profile_t *profile)
{
  sl_status_t status;
  sl_wifi_interface_t client_interface = 0;

  // Create the event flag for auto-join
  auto_join_event_flag = osEventFlagsNew(NULL);
  if (auto_join_event_flag == NULL) {
    SL_DEBUG_LOG("Failed to create auto-join event flag.\n");
    return SL_STATUS_FAIL;
  }

  // Create a message queue for network manager communication
  network_manager_queue =
    osMessageQueueNew(10, sizeof(sl_network_manager_message_t), &network_manager_queue_attributes);
  if (network_manager_queue == NULL) {
    SL_DEBUG_LOG("Failed to create network manager message queue.\n");
    return SL_STATUS_FAIL;
  }

  // Create the network manager thread
  network_manager_id =
    osThreadNew((osThreadFunc_t)sli_network_manager_event_handler, NULL, &network_manager_attributes);
  if (network_manager_id == NULL) {
    SL_DEBUG_LOG("Failed to create network manager thread.\n");
    return SL_STATUS_FAIL;
  }

  // Get the client profile using the default profile ID
  status = sl_net_get_profile(interface, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID, profile);
  VERIFY_STATUS_AND_RETURN(status);

#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
  //Configure the client interface based on the band
  if (profile->config.channel.band == SL_WIFI_BAND_2_4GHZ || profile->config.channel.band == SL_WIFI_AUTO_BAND) {
    client_interface = SL_WIFI_CLIENT_2_4GHZ_INTERFACE;
  } else if (profile->config.channel.band == SL_WIFI_BAND_5GHZ) {
    client_interface = SL_WIFI_CLIENT_5GHZ_INTERFACE;
  } else if (profile->config.channel.band == SL_WIFI_BAND_DUAL) {
    client_interface = SL_WIFI_CLIENT_DUAL_INTERFACE;
  }
#endif

  status = sl_wifi_connect(client_interface, &profile->config, SLI_WIFI_CONNECT_TIMEOUT);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG("Failed to connect to Wi-Fi network.\n");
    sl_network_manager_message_t message;
    message.interface   = interface;
    message.event_flags = NETWORK_MANAGER_CONNECT_FAILURE_CMD;
    osMessageQueuePut(network_manager_queue, &message, 0, 0);

    // Wait for the auto-join process to complete
    uint32_t event_result = osEventFlagsWait(auto_join_event_flag, 0x01, osFlagsWaitAny, osWaitForever);
    if (event_result & AUTO_JOIN_SUCCESS_FLAG) {
      SL_DEBUG_LOG("Auto-join process completed.\n");
      return SL_STATUS_OK;
    } else if (event_result & AUTO_JOIN_FAILURE_FLAG) {
      SL_DEBUG_LOG("Auto-join process failed.\n");
      return SL_STATUS_FAIL;
    }
  }

#ifdef SLI_SI91X_LWIP_HOSTED_NETWORK_STACK
#if LWIP_IPV4 && LWIP_IPV6
  sl_net_wifi_client_profile_t *lwip_profile = (sl_net_wifi_client_profile_t *)profile;
  if ((lwip_profile->ip.type & SL_IPV4) == SL_IPV4) {
    ip_addr_t *addr;
    // Set the IP address of v4 interface
    addr = &wifi_client_context->netif.ip_addr;
    memcpy(lwip_profile->ip.ip.v4.ip_address.bytes, &addr->u_addr.ip4.addr, sizeof(addr->u_addr.ip4.addr));

    addr = &wifi_client_context->netif.gw;
    memcpy(lwip_profile->ip.ip.v4.gateway.bytes, &addr->u_addr.ip4.addr, sizeof(addr->u_addr.ip4.addr));

    addr = &wifi_client_context->netif.netmask;
    memcpy(lwip_profile->ip.ip.v4.netmask.bytes, &addr->u_addr.ip4.addr, sizeof(addr->u_addr.ip4.addr));
  }

  if ((lwip_profile->ip.type & SL_IPV6) == SL_IPV6) {
    // Set the IP address of v6 interface
    // Loop through the first 4 elements of the IPv6 address arrays to convert and assign them to the lwip_profile structure
    for (int i = 0; i < 4; i++) {
      lwip_profile->ip.ip.v6.link_local_address.value[i] =
        ntohl(wifi_client_context->netif.ip6_addr[0].u_addr.ip6.addr[i]);
      lwip_profile->ip.ip.v6.global_address.value[i] = ntohl(wifi_client_context->netif.ip6_addr[1].u_addr.ip6.addr[i]);
      lwip_profile->ip.ip.v6.gateway.value[i]        = ntohl(wifi_client_context->netif.ip6_addr[2].u_addr.ip6.addr[i]);
    }
  }
#else /* LWIP_IPV4 && LWIP_IPV6 */
#if LWIP_IPV4
  u32_t *addr;

  addr = &wifi_client_context->netif.ip_addr.addr;
  memcpy(profile->ip.ip.v4.ip_address.bytes, addr, sizeof(*addr));

  addr = &wifi_client_context->netif.gw.addr;
  memcpy(profile->ip.ip.v4.gateway.bytes, addr, sizeof(*addr));

  addr = &wifi_client_context->netif.netmask.addr;
  memcpy(profile->ip.ip.v4.netmask.bytes, addr, sizeof(*addr));
#elif LWIP_IPV6
  // Loop through the first 4 elements of the IPv6 address arrays to convert and assign them to the profile structure
  for (int i = 0; i < 4; i++) {
    profile->ip.ip.v6.link_local_address.value[i] = ntohl(wifi_client_context->netif.ip6_addr[0].addr[i]);
    profile->ip.ip.v6.global_address.value[i]     = ntohl(wifi_client_context->netif.ip6_addr[1].addr[i]);
    profile->ip.ip.v6.gateway.value[i]            = ntohl(wifi_client_context->netif.ip6_addr[2].addr[i]);
  }
#endif /* LWIP_IPV6 */
#endif /* LWIP_IPV4 && LWIP_IPV6 */
#endif /* SLI_SI91X_LWIP_HOSTED_NETWORK_STACK */
#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
  status = sl_si91x_configure_ip_address(&profile->ip, SL_SI91X_WIFI_CLIENT_VAP_ID_1);
  VERIFY_STATUS_AND_RETURN(status);
#endif

  // Set the client profile
  status = sl_net_set_profile(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID, profile);

  return status;
}

// Partition function for QuickSort
static int sli_partition_profile_ids(uint8_t profile_ids[], uint8_t priorities[], int low, int high)
{
  uint8_t pivot = priorities[high]; // Choose the last element as the pivot
  int i         = low - 1;

  for (int j = low; j < high; j++) {
    if (priorities[j] < pivot) { // Compare priorities
      i++;
      // Swap profile IDs
      uint8_t temp_id = profile_ids[i];
      profile_ids[i]  = profile_ids[j];
      profile_ids[j]  = temp_id;

      // Swap priorities
      uint8_t temp_priority = priorities[i];
      priorities[i]         = priorities[j];
      priorities[j]         = temp_priority;
    }
  }

  // Swap profile IDs and priorities for the pivot
  uint8_t temp_id    = profile_ids[i + 1];
  profile_ids[i + 1] = profile_ids[high];
  profile_ids[high]  = temp_id;

  uint8_t temp_priority = priorities[i + 1];
  priorities[i + 1]     = priorities[high];
  priorities[high]      = temp_priority;

  return i + 1; // Return the pivot index
}

static void sli_quicksort_profile_ids(uint8_t profile_ids[], uint8_t priorities[], int low, int high)
{
  if (low < high) {
    // Partition the array and get the pivot index
    int pivot_index = sli_partition_profile_ids(profile_ids, priorities, low, high);

    // Recursively sort elements before and after the partition
    sli_quicksort_profile_ids(profile_ids, priorities, low, pivot_index - 1);
    sli_quicksort_profile_ids(profile_ids, priorities, pivot_index + 1, high);
  }
}

static void sli_notify_net_event_handler(sl_net_event_t event, sl_status_t status, void *data, size_t data_size)
{
  if (net_event_handler) {
    net_event_handler(event, status, data, data_size);
  }
}

static int sli_get_iterate_profiles_count()
{
  sl_wifi_performance_profile_t performance_profile;
  sl_status_t status = sl_wifi_get_performance_profile(&performance_profile);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG("\r\nFailed to get performance profile: 0x%lx\r\n", status);
    return 1;
  }
  if (performance_profile.profile == HIGH_PERFORMANCE) {
    return AUTO_JOIN_RETRY_COUNT;
  } else {
    return 1;
  }
}

static sl_status_t sli_fetch_and_sort_profiles(sl_net_interface_t interface,
                                               uint8_t sorted_profile_ids[],
                                               uint8_t priorities[])
{
  sl_status_t status;

  // Fetch all profile IDs and their priorities
  for (uint8_t profile_id = 0; profile_id < MAX_WIFI_CLIENT_PROFILES; profile_id++) {
    sl_net_wifi_client_profile_t profile;
    status = sl_net_get_profile(interface, profile_id, &profile);
    if (status == SL_STATUS_OK) {
      sorted_profile_ids[profile_id] = profile_id;       // Store the profile ID
      priorities[profile_id]         = profile.priority; // Store the priority
    } else {
      // If fetching a profile fails, set priority to a high value to deprioritize it
      sorted_profile_ids[profile_id] = profile_id;
      priorities[profile_id]         = UINT8_MAX;
    }
  }

  // Sort profile IDs based on their priorities using QuickSort
  sli_quicksort_profile_ids(sorted_profile_ids, priorities, 0, MAX_WIFI_CLIENT_PROFILES - 1);

  return SL_STATUS_OK;
}

static bool sli_attempt_connection_to_profiles(const uint8_t sorted_profile_ids[],
                                               sl_net_event_t event,
                                               sl_status_t *status)
{
  for (int i = 0; i < MAX_WIFI_CLIENT_PROFILES; i++) {
    *status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, sorted_profile_ids[i]); // Use profile ID
    if (*status == SL_STATUS_OK) {
      SL_DEBUG_LOG("\r\nSuccess to set up Wi-Fi for Profile ID %d\r\n", sorted_profile_ids[i]);
      sl_net_auto_join_status_t join_status = SL_NET_AUTO_JOIN_CONNECTED;
      sli_notify_net_event_handler(event, *status, &join_status, sizeof(int));
      return true;
    } else {
      SL_DEBUG_LOG("\r\nFailed to set up Wi-Fi for Profile ID %d: 0x%lx\r\n", sorted_profile_ids[i], *status);
    }
  }
  return false;
}

static bool sli_connect_to_sorted_wifi_profiles(const uint8_t sorted_profile_ids[],
                                                int iterate_profiles_count,
                                                sl_net_event_t event)
{
  sl_status_t status;
  bool ap_connected = false;

  while (iterate_profiles_count > 0) {
    ap_connected = sli_attempt_connection_to_profiles(sorted_profile_ids, event, &status);
    if (ap_connected) {
      break;
    }

    SL_DEBUG_LOG("\r\nRetrying to set up Wi-Fi...\r\n");
    osDelay(30000); // Wait for 30 seconds before retrying
    iterate_profiles_count--;
  }

  return ap_connected;
}

static bool sli_handle_disconnect_or_failure_event(const sl_network_manager_message_t *message,
                                                   uint8_t sorted_profile_ids[],
                                                   uint8_t priorities[],
                                                   sl_net_event_t event)
{
  sl_status_t status = SL_STATUS_OK;
  if (!(message->event_flags & (NETWORK_MANAGER_DISCONNECT_CMD | NETWORK_MANAGER_CONNECT_FAILURE_CMD))) {
    return false; // Not a disconnect or failure event
  }

  // Notify user that auto-join is in progress
  if (net_event_handler) {
    sl_net_auto_join_status_t join_status = SL_NET_AUTO_JOIN_IN_PROGRESS;
    sli_notify_net_event_handler(event, status, &join_status, sizeof(int));
  }

  status = sli_fetch_and_sort_profiles(message->interface, sorted_profile_ids, priorities);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG("\r\nFailed to fetch and sort profiles: 0x%lx\r\n", status);
    return true;
  }

  int iterate_profiles_count = sli_get_iterate_profiles_count();
  bool ap_connected          = sli_connect_to_sorted_wifi_profiles(sorted_profile_ids, iterate_profiles_count, event);

  if (ap_connected) {
    osEventFlagsSet(auto_join_event_flag, AUTO_JOIN_SUCCESS_FLAG);
  } else {
    osEventFlagsSet(auto_join_event_flag, AUTO_JOIN_FAILURE_FLAG); // No AP available, unblock anyway
    // Notify user that the auto-join process failed
    if (net_event_handler) {
      sl_net_auto_join_status_t join_status = SL_NET_AUTO_JOIN_FAILED;
      sli_notify_net_event_handler(event, status, &join_status, sizeof(int));
    }
  }
  return true;
}

/**
* @brief Attempts to connect to Wi-Fi profiles in the sorted list.
*
* @param sorted_profile_ids Array of sorted profile IDs.
* @param event The network event to notify.
* @param status Pointer to store the connection status.
* @return true if a connection is successful, false otherwise.
*/
void sli_network_manager_event_handler(const void *arg)
{
  UNUSED_PARAMETER(arg);
  sl_network_manager_message_t message;
  uint8_t sorted_profile_ids[MAX_WIFI_CLIENT_PROFILES];
  uint8_t priorities[MAX_WIFI_CLIENT_PROFILES];
  sl_net_event_t event = SL_NET_AUTO_JOIN_EVENT;

  while (1) {
    if (osMessageQueueGet(network_manager_queue, &message, NULL, osWaitForever) != osOK) {
      continue; // Skip if message retrieval fails
    }

    if (sli_handle_disconnect_or_failure_event(&message, sorted_profile_ids, priorities, event) == true) {
      continue; // Handle disconnect or failure event
    }

    if (message.event_flags & NETWORK_MANAGER_CONNECT_CMD) {
      SL_DEBUG_LOG("\r\n Connected asynchronously\r\n");
    }
  }
}
