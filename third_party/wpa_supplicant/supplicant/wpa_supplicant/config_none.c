/*
 * WPA Supplicant / Configuration backend: empty starting point
 * Copyright (c) 2003-2005, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 *
 * This file implements dummy example of a configuration backend. None of the
 * functions are actually implemented so this can be used as a simple
 * compilation test or a starting point for a new configuration backend.
 */

#include "includes.h"
#include "common.h"
#include "config.h"
#include "base64.h"

#if 0 
const uint8 cfg_type[] = {0x00, 0x0a, 0x00, 0x50, 0xF2, 0x04, 0x00, 0x04}; 
#else /*FIXME: enable below after 5GHz support added */
//the below configuration represents a handset device
//const uint8 cfg_type_201[] = {0x00, 0x0a, 0x00, 0x50, 0xF2, 0x04, 0x00, 0x04};
//const uint8 cfg_type_301[] = {0x00, 0x0a, 0x00, 0x50, 0xF2, 0x04, 0x00, 0x05};
//the below connfiguration represents an Ap or an infrastuct network.
const uint8 cfg_type_201[] = {0x00, 0x06, 0x00, 0x50, 0xF2, 0x04, 0x00, 0x01};
const uint8 cfg_type_301[] = {0x00, 0x06, 0x00, 0x50, 0xF2, 0x04, 0x00, 0x01};
#endif

struct wpa_config * wpa_config_read(const char *name, struct wpa_config *cfgp)
{
	struct wpa_config *config;

	if (name == NULL)
		return NULL;
	if (cfgp)
		config = cfgp;
	else
  config = wpa_config_alloc_empty(NULL, NULL);
  if (config == NULL)
    return NULL;
  /* TODO: fill in configuration data */
#ifdef K60_PORTING
  config->device_name =  os_strdup((char*)supp_mgmt_if_ctx.wpa_config_read_data.device_name);
  config->manufacturer = os_strdup("Silicon labs");
  config->serial_number = os_strdup("01");
  config->model_number = os_strdup("9116");
  config->model_name = os_strdup("WiSeConnect");

  if(!supp_mgmt_if_ctx.wpa_config_read_data.module_type)
    memcpy(config->device_type, cfg_type_201, 8);
  else
    memcpy(config->device_type, cfg_type_301, 8);

  config->p2p_go_intent = *(uint16 *) supp_mgmt_if_ctx.wpa_config_read_data.go_intent;
  if (supp_mgmt_if_ctx.wpa_config_read_data.switch_band == SL_BAND_5GHZ)
  {
    config->p2p_oper_reg_class = 115;
    /* no need to map already mapped*/
    config->p2p_oper_channel = *(uint16 *)supp_mgmt_if_ctx.wpa_config_read_data.operating_channel ;//BAND_5GHZ_CHANNEL_MAP[rsi_wsc_cb.operating_channel];

    if (config->p2p_oper_channel > 140)
    {
      config->p2p_oper_reg_class = 124;
    }
    else if((supp_mgmt_if_ctx.wpa_config_read_data.dual_band == SL_BAND_DUAL) && (config->p2p_oper_channel < 36))
    {
      config->p2p_oper_reg_class = 81;
    }
  }
  else
  {
    config->p2p_oper_reg_class = 81;
    config->p2p_oper_channel = *(uint16 *)supp_mgmt_if_ctx.wpa_config_read_data.operating_channel;
  }

  config->p2p_listen_channel = 11;
  config->p2p_listen_reg_class = 81;

  config->p2p_ssid_postfix = os_strdup((char*)supp_mgmt_if_ctx.wpa_config_read_data.ssid_postfix); 
  /* WPS */
  *(u32*)config->os_version = (u32)0x01020300UL;
  /* WPS methods */
  config->config_methods = os_strdup("push_button");
  //config->wps_cred_processing = 1;
#endif
  return config;
}

int wpa_config_write(const char *name, struct wpa_config *config)
{
	struct wpa_ssid *ssid;
	struct wpa_config_blob *blob;

	wpa_printf(MSG_DEBUG, "Writing configuration file '%s'", name);

	/* TODO: write global config parameters */


	for (ssid = config->ssid; ssid; ssid = ssid->next) {
		/* TODO: write networks */
	}

	for (blob = config->blobs; blob; blob = blob->next) {
		/* TODO: write blobs */
	}

	return 0;
}



/**
*$Log: config_none.c,v $
*Revision 1.1.1.1  2012/02/28 07:14:12  kiran.gunda
*WiSe Connet initial import
*
**/ 
