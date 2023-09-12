/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */
#include "sl_net_for_lwip.h"
#include "sl_status.h"
#include "lwip/tcpip.h"
#include "lwip/etharp.h"
#include "lwip/dhcp.h"
#include "cmsis_os2.h"
#include "sl_si91x_host_interface.h"
#include "sl_wifi.h"
#include "sl_utility.h"
#include "sl_wifi_callback_framework.h"
#include "sl_net_wifi_types.h"
#include "sl_net.h"
#include <string.h>

#define INTERFACE_NAME_0  's' ///< Network interface name 0
#define INTERFACE_NAME_1  'l' ///< Network interface name 1
#define MAX_TRANSFER_UNIT 1500

extern sys_thread_t lwip_thread;
static err_t sta_ethernetif_init(struct netif *netif);

err_t sl_low_level_output(struct netif *netif, struct pbuf *p);
void low_level_init(struct netif *netif);

// This should be part of sl_si91x.h
extern sl_status_t sl_si91x_driver_raw_send_command(uint8_t command,
                                                    const void *data,
                                                    uint32_t data_length,
                                                    uint32_t wait_time);

sl_net_wifi_lwip_context_t *wifi_client_context;
sl_net_wifi_lwip_context_t *wifi_ap_context;

sl_status_t sl_net_wifi_ap_init(sl_net_interface_t interface,
                                const void *configuration,
                                void *workspace,
                                sl_net_event_handler_t event_handler)
{
  UNUSED_PARAMETER(interface);
  UNUSED_PARAMETER(configuration);
  UNUSED_PARAMETER(workspace);
  UNUSED_PARAMETER(event_handler);
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_net_wifi_ap_deinit(sl_net_interface_t interface, void *wifi_ap_context)
{
  UNUSED_PARAMETER(interface);
  UNUSED_PARAMETER(wifi_ap_context);
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_net_wifi_ap_up(sl_net_interface_t interface, sl_net_profile_id_t profile_id)
{
  UNUSED_PARAMETER(interface);
  UNUSED_PARAMETER(profile_id);
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_net_wifi_ap_down(sl_net_interface_t interface)
{
  UNUSED_PARAMETER(interface);
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_net_wifi_client_init(sl_net_interface_t interface,
                                    const void *configuration,
                                    void *context,
                                    sl_net_event_handler_t event_handler)
{
  UNUSED_PARAMETER(interface);
  UNUSED_PARAMETER(event_handler);
  sl_status_t status = sl_wifi_init(configuration, default_wifi_event_handler);
  if (status != SL_STATUS_OK) {
    return status;
  }
  wifi_client_context = context;
  tcpip_init(NULL, 0);
  return SL_STATUS_OK;
}

sl_status_t sl_net_wifi_client_deinit(sl_net_interface_t interface, void *wifi_client_context)
{
  UNUSED_PARAMETER(interface);
  UNUSED_PARAMETER(wifi_client_context);
  struct sys_timeo **list_head = NULL;

  if (lwip_thread.thread_handle != NULL) {
    osStatus_t thread_status = osThreadTerminate(lwip_thread.thread_handle);
    if (thread_status != osOK) {
      SL_DEBUG_LOG("\n\r TCP-IP thread terminate status : %d \r\n", thread_status);
      return SL_STATUS_FAIL;
    }
  }

  //! Free all timers
  for (int i = 0; i < lwip_num_cyclic_timers; i++) {
    list_head = sys_timeouts_get_next_timeout();
    if (*list_head != NULL)
      sys_untimeout((*list_head)->h, (*list_head)->arg);
  }

  return sl_wifi_deinit();
}

sl_status_t sl_net_wifi_client_up(sl_net_interface_t interface, sl_net_profile_id_t profile_id)
{
  UNUSED_PARAMETER(interface);
  uint32_t status = 0;

  // Load profile and connect here
  sl_net_wifi_client_profile_t profile;

  status = sl_net_get_profile(SL_NET_WIFI_CLIENT_INTERFACE, profile_id, &profile);
  VERIFY_STATUS_AND_RETURN(status);

  status = sl_wifi_connect(SL_WIFI_CLIENT_INTERFACE, &profile.config, 5000);
  VERIFY_STATUS_AND_RETURN(status);

  // Add network interface
  netif_add(&wifi_client_context->netif,
            IP4_ADDR_ANY,
            IP4_ADDR_ANY,
            IP4_ADDR_ANY,
            NULL,
            &sta_ethernetif_init,
            &tcpip_input);

  // Register handler for netif
  wifi_client_context->netif.name[0]    = INTERFACE_NAME_0;
  wifi_client_context->netif.name[1]    = INTERFACE_NAME_1;
  wifi_client_context->netif.output     = etharp_output;
  wifi_client_context->netif.linkoutput = sl_low_level_output;

  // initialize the hardware
  //  low_level_init( &wifi_client_context->netif );
  netif_set_default(&wifi_client_context->netif);
  netif_set_up(&wifi_client_context->netif);

  // Request IP address
  /* Start DHCP  */
  status = dhcp_start(&wifi_client_context->netif);
  if (status != ERR_OK) {
    SL_DEBUG_LOG("LWIP IP Config failed %x\r\n", status);
    return SL_STATUS_FAIL;
  }
  //! Looping until IP for the module
  while (!((uint8_t)(wifi_client_context->netif.ip_addr.addr & 0xff))) {
    osDelay(100);
  }
  printf("Device IP address : %d.%d.%d.%d\r\n",
         (uint8_t)(wifi_client_context->netif.ip_addr.addr & 0xff),
         (uint8_t)(wifi_client_context->netif.ip_addr.addr >> 8),
         (uint8_t)(wifi_client_context->netif.ip_addr.addr >> 16),
         (uint8_t)(wifi_client_context->netif.ip_addr.addr >> 24));
  return SL_STATUS_OK;
}

sl_status_t sl_net_wifi_client_down(sl_net_interface_t interface)
{
  // DHCP stop and release the resources
  dhcp_release_and_stop(&wifi_client_context->netif);

  // Network interface down
  netif_set_down(&wifi_client_context->netif);
  netif_remove(&wifi_client_context->netif);

  return sl_wifi_disconnect(SL_WIFI_CLIENT_INTERFACE);
}

sl_status_t sl_si91x_host_process_data_frame(sl_wifi_interface_t interface, sl_wifi_buffer_t *buffer)
{
  struct pbuf *pbuf_packet;
  sl_si91x_packet_t *packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  SL_DEBUG_LOG("\nlen :%d\n", packet->length);

  pbuf_packet = pbuf_alloc(PBUF_RAW, sizeof(struct pbuf), PBUF_POOL);
  if (pbuf_packet == NULL) {
    return SL_STATUS_FAIL;
  }

  pbuf_packet->next = NULL;
  memcpy(pbuf_packet->payload, packet->data, packet->length);
  pbuf_packet->len     = packet->length;
  pbuf_packet->tot_len = packet->length;
  tcpip_input(pbuf_packet,
              (interface & SL_WIFI_CLIENT_INTERFACE) ? &wifi_client_context->netif : &wifi_ap_context->netif);

  return SL_STATUS_OK;
}

/*****************************************************************************
 * Sets up the station network interface.
 *
 * @param netif the lwip network interface structure
 * @returns ERR_OK if successful
 ******************************************************************************/
static err_t sta_ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip_sta";
#endif /* LWIP_NETIF_HOSTNAME */
  //! Set the netif name to identify the interface
  netif->name[0] = INTERFACE_NAME_0;
  netif->name[1] = INTERFACE_NAME_1;

  //! Assign handler/function for the interface
  netif->input      = tcpip_input;
  netif->output     = etharp_output;
  netif->linkoutput = sl_low_level_output;

  //! initialize the hardware
  low_level_init(netif);
  //  sta_netif = *netif;

  return ERR_OK;
}

/***************************************************************************
 * Initializes the hardware parameters. Called from ethernetif_init().
 *
 * @param netif The already initialized lwip network interface structure
 ******************************************************************************/
void low_level_init(struct netif *netif)
{
  uint32_t status = 0;
  sl_mac_address_t mac_addr;

  // set netif MAC hardware address length
  netif->hwaddr_len = ETH_HWADDR_LEN;

  // Request MAC address
  status = sl_wifi_get_mac_address(SL_WIFI_CLIENT_INTERFACE, &mac_addr);
  if (status != SL_STATUS_OK) {
    printf("\r\n MAC address failed \r\n");
    return;
  }

  // set netif MAC hardware address
  netif->hwaddr[0] = mac_addr.octet[0];
  netif->hwaddr[1] = mac_addr.octet[1];
  netif->hwaddr[2] = mac_addr.octet[2];
  netif->hwaddr[3] = mac_addr.octet[3];
  netif->hwaddr[4] = mac_addr.octet[4];
  netif->hwaddr[5] = mac_addr.octet[5];

  // set netif maximum transfer unit
  netif->mtu = MAX_TRANSFER_UNIT;

  // Accept broadcast address and ARP traffic
  netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

  // Set netif link flag
  netif->flags |= NETIF_FLAG_LINK_UP;
}

/******************************************************************************
 * Transmits packet(s).
 *
 * @param netif the lwip network interface structure
 * @param p the packet to send
 * @returns ERR_OK if successful
 ******************************************************************************/
err_t sl_low_level_output(struct netif *netif, struct pbuf *p)
{
  UNUSED_PARAMETER(netif);
  sl_status_t status;
  status = sl_si91x_driver_raw_send_command(RSI_SEND_RAW_DATA,
                                            (uint8_t *)p->payload,
                                            p->len,
                                            RSI_SEND_RAW_DATA_RESPONSE_WAIT_TIME);
  if (status != SL_STATUS_OK) {
    return ERR_IF;
  }
  return ERR_OK;
}
