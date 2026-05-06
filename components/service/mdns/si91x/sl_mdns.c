/*******************************************************************************
* @file  sl_mdns.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_mdns.h"
#include "sl_slist.h"
#include "sl_si91x_driver.h"

#define MAX_MDNS_SERVICE_COUNT 1
#define MDNSD_BUFFER_SIZE      1000

#define SI91X_MDNSD_INIT             1 //! MDNSD Initialize
#define SI91X_MDNSD_REGISTER_SERVICE 3 //! MDNSD register service
#define SI91X_MDNSD_DEINIT           6 //! MDNSD deinitialize

// mdnsd init structure
typedef struct {
  uint8_t ip_version; // ip version
  uint8_t ttl[2];     // time to live
} sl_si91x_mdns_init_t;

// mdnsd register-service structure
typedef struct {
  uint8_t port[2]; // port number
  uint8_t ttl[2];  // time to live
  uint8_t more;    // reset if it is last service in the list
} sl_si91x_mdns_reg_srv_t;

// mdnsd structure
typedef struct {
  uint8_t command_type; // command type 1-MDNSD init, 3- Register service, 6- Deinit
  union {
    sl_si91x_mdns_init_t init;       // mdns init
    sl_si91x_mdns_reg_srv_t reg_srv; // mdns register
  } req_conf;
  uint8_t buffer[MDNSD_BUFFER_SIZE]; //Data buffer
} sl_si91x_mdns_req_t;

/**********************************************************************************************************************
 *                                                Static Functions                                                    *
**********************************************************************************************************************/
static void sli_si91x_clean_service_handle(sl_wifi_buffer_t *service_handle)
{
  sl_mdns_service_t *service = NULL;
  uint16_t buffer_length     = 0;

  service = (sl_mdns_service_t *)sl_si91x_host_get_buffer_data(service_handle, 0, &buffer_length);
  free((char *)service->instance_name);
  free((char *)service->service_type);
  free((char *)service->service_message);
  sl_si91x_host_free_buffer(service_handle);

  return;
}

static void sli_si91x_clean_service_list(sl_mdns_interface_t *interface)
{
  sl_wifi_buffer_t *service = NULL;
  sl_wifi_buffer_t *block   = NULL;

  service = interface->service_list;

  while (service != NULL) {
    block   = service;
    service = (sl_wifi_buffer_t *)service->node.node;
    sli_si91x_clean_service_handle(block);
  }

  return;
}

static void sli_si91x_clean_mdns_handle(sl_mdns_t *mdns)
{
  sl_wifi_buffer_t *interface = NULL;
  sl_wifi_buffer_t *block     = NULL;
  sl_mdns_interface_t *in     = NULL;
  uint16_t buffer_length;

  interface = mdns->interface_list;

  while (interface != NULL) {
    block     = interface;
    interface = (sl_wifi_buffer_t *)interface->node.node;
    in        = (sl_mdns_interface_t *)sl_si91x_host_get_buffer_data(block, 0, &buffer_length);
    sli_si91x_clean_service_list(in);
    sl_si91x_host_free_buffer(block);
  }

  return;
}
/**********************************************************************************************************************
 *                                                Public Functions                                                    *
**********************************************************************************************************************/
sl_status_t sl_mdns_init(sl_mdns_t *mdns, const sl_mdns_configuration_t *config, sl_mdns_event_handler_t event_handler)
{
  sl_si91x_mdns_req_t req = { 0 };
  sl_status_t status      = SL_STATUS_FAIL;
  uint32_t length         = 0;
  uint16_t ttl            = 0;

  memset(mdns, 0, sizeof(sl_mdns_t));
  memcpy(&(mdns->configuration), config, sizeof(sl_mdns_configuration_t));
  mdns->event_handler = event_handler;

  req.command_type         = SI91X_MDNSD_INIT;
  req.req_conf.init.ttl[0] = ttl & 0x00ff;
  req.req_conf.init.ttl[1] = (ttl >> 8) & 0x00ff;

  if (SL_IPV4_VERSION == config->type) {
    req.req_conf.init.ip_version = 4;
  } else if (SL_IPV6_VERSION == config->type) {
    req.req_conf.init.ip_version = 6;
  } else {
    return SL_STATUS_INVALID_TYPE;
  }

  memcpy(req.buffer, config->host_name, strlen(config->host_name));
  length = sizeof(sl_si91x_mdns_req_t) - MDNSD_BUFFER_SIZE + strlen(config->host_name) + 1;

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_MDNSD,
                                        SI91X_NETWORK_CMD,
                                        &req,
                                        length,
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);

  return status;
}

sl_status_t sl_mdns_deinit(sl_mdns_t *mdns)
{
  sl_si91x_mdns_req_t req = { 0 };
  sl_status_t status      = SL_STATUS_FAIL;
  uint32_t length         = 0;

  req.command_type = SI91X_MDNSD_DEINIT;
  length           = sizeof(sl_si91x_mdns_req_t) - MDNSD_BUFFER_SIZE;

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_MDNSD,
                                        SI91X_NETWORK_CMD,
                                        &req,
                                        length,
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);

  sli_si91x_clean_mdns_handle(mdns);
  memset(mdns, 0, sizeof(sl_mdns_t));
  return status;
}

sl_status_t sl_mdns_add_interface(sl_mdns_t *mdns, sl_net_interface_t interface)
{
  sl_status_t status              = SL_STATUS_FAIL;
  sl_wifi_buffer_t *new_interface = NULL;
  sl_mdns_interface_t *in         = NULL;
  uint16_t buffer_length          = 0;

  status = sl_si91x_host_allocate_buffer(&new_interface, SL_WIFI_CONTROL_BUFFER, sizeof(sl_mdns_interface_t), 1000);
  VERIFY_STATUS_AND_RETURN(status);
  in               = (sl_mdns_interface_t *)sl_si91x_host_get_buffer_data(new_interface, 0, &buffer_length);
  in->interface    = interface;
  in->service_list = NULL;

  new_interface->node.node = (sl_slist_node_t *)mdns->interface_list;
  mdns->interface_list     = new_interface;

  return SL_STATUS_OK;
}

sl_status_t sl_mdns_remove_interface(sl_mdns_t *mdns, sl_net_interface_t interface)
{
  UNUSED_PARAMETER(mdns);
  UNUSED_PARAMETER(interface);
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_mdns_register_service(sl_mdns_t *mdns, sl_net_interface_t interface, sl_mdns_service_t *service)
{
  sl_status_t status            = SL_STATUS_FAIL;
  sl_wifi_buffer_t *new_service = NULL;
  sl_wifi_buffer_t *target_in   = NULL;
  sl_mdns_interface_t *in       = NULL;
  sl_mdns_service_t *srv        = NULL;
  sl_si91x_mdns_req_t req       = { 0 };
  uint32_t length               = 0;
  uint16_t buffer_length        = 0;

  if (NULL == mdns->interface_list) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (mdns->service_count >= MAX_MDNS_SERVICE_COUNT) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (NULL == service->service_type) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (NULL == service->service_type) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (NULL == service->service_message) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if ((strlen(service->service_type) + strlen(service->instance_name) + strlen(service->service_message) + 3)
      > MDNSD_BUFFER_SIZE) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  for (target_in = mdns->interface_list; (NULL != target_in); target_in = (sl_wifi_buffer_t *)target_in->node.node) {
    in = (sl_mdns_interface_t *)sl_si91x_host_get_buffer_data(target_in, 0, &buffer_length);

    if (in->interface == interface) {
      break;
    } else {
      in = NULL;
    }
  }

  if (NULL == in) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  status = sl_si91x_host_allocate_buffer(&new_service, SL_WIFI_CONTROL_BUFFER, sizeof(sl_mdns_service_t), 1000);
  VERIFY_STATUS_AND_RETURN(status);
  srv                = (sl_mdns_service_t *)sl_si91x_host_get_buffer_data(new_service, 0, &buffer_length);
  srv->instance_name = malloc(strlen(service->instance_name) + 1);
  strcpy((char *)srv->instance_name, service->instance_name);

  srv->service_type = malloc(strlen(service->service_type) + 1);
  strcpy((char *)srv->service_type, service->service_type);

  srv->service_message = malloc(strlen(service->service_message) + 1);
  strcpy((char *)srv->service_message, service->service_message);

  srv->port = service->port;
  srv->ttl  = service->ttl;

  // Prepare Firmware request
  req.command_type             = SI91X_MDNSD_REGISTER_SERVICE;
  req.req_conf.reg_srv.port[0] = service->port & 0x00ff;
  req.req_conf.reg_srv.port[1] = (service->port >> 8) & 0x00ff;
  req.req_conf.reg_srv.ttl[0]  = service->ttl & 0x00ff;
  req.req_conf.reg_srv.ttl[1]  = (service->ttl >> 8) & 0x00ff;
  req.req_conf.reg_srv.more    = 0;
  length                       = 0;

  strcpy((char *)req.buffer, service->service_type);
  length += strlen(service->service_type) + 1;

  strcpy((char *)((req.buffer) + length), service->instance_name);
  length += strlen(service->instance_name) + 1;

  strcpy((char *)((req.buffer) + length), service->service_message);
  length += strlen(service->service_message) + 1 + (sizeof(sl_si91x_mdns_req_t) - MDNSD_BUFFER_SIZE);

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_MDNSD,
                                        SI91X_NETWORK_CMD,
                                        &req,
                                        length,
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);
  if (SL_STATUS_OK != status) {
    sli_si91x_clean_service_handle(new_service);
    VERIFY_STATUS_AND_RETURN(status);
  }

  new_service->node.node = (sl_slist_node_t *)in->service_list;
  in->service_list       = new_service;
  mdns->service_count++;
  return SL_STATUS_OK;
}

sl_status_t sl_mdns_unregister_service(sl_mdns_t *mdns, sl_mdns_service_query_t *service_query)
{
  UNUSED_PARAMETER(mdns);
  UNUSED_PARAMETER(service_query);
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_mdns_update_service_message(sl_mdns_t *mdns,
                                           sl_mdns_service_query_t *service_query,
                                           const char *message,
                                           uint32_t message_length)
{
  UNUSED_PARAMETER(mdns);
  UNUSED_PARAMETER(service_query);
  UNUSED_PARAMETER(message);
  UNUSED_PARAMETER(message_length);
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_mdns_discover_service(sl_mdns_t *mdns, sl_mdns_service_query_t *service_query)
{
  UNUSED_PARAMETER(mdns);
  UNUSED_PARAMETER(service_query);
  return SL_STATUS_NOT_SUPPORTED;
}
