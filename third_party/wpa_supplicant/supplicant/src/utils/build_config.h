/*
 * wpa_supplicant/hostapd - Build time configuration defines
 * Copyright (c) 2005-2006, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 *
 * This header file can be used to define configuration defines that were
 * originally defined in Makefile. This is mainly meant for IDE use or for
 * systems that do not have suitable 'make' tool. In these cases, it may be
 * easier to have a single place for defining all the needed C pre-processor
 * defines.
 */

#ifndef BUILD_CONFIG_H
#define BUILD_CONFIG_H

#define ENABLE_ENTERPRISE_SECURITY
#define __KINETIS_K60__ 
#define K60_PORTING 1
#ifndef EXCLUDE_AP
#define CONFIG_AP 1
#define CONFIG_NO_ACCOUNTING
#define CONFIG_NO_VLAN
#define CONFIG_NO_RADIUS
#define EAP_SERVER_IDENTITY
#endif
#define CONFIG_DRIVER_NONE 
#ifndef EXCLUDE_WPS
#define CONFIG_WPS
#define EAP_WSC 1
#endif
#define IEEE8021X_EAPOL
#if UNUSED_FEAT_IN_SUPP_29
#define CONFIG_P2P 1
#endif
#if !defined(EXCLUDE_AP) && !defined(EXCLUDE_WPS)
#define EAP_SERVER_WSC 
#endif
#ifndef EXCLUDE_EAP
#define EAP_MSCHAPv2
#define EAP_TTLS
#define EAP_FAST
#define EAP_PEAP
#define EAP_LEAP
#define EAP_TLS
#endif
#define TKIP_COUNTER_MEASURES_ENABLE
#define USE_INTERNAL_CRYPTO
#define CONFIG_ECC 
#define CONFIG_SAE 

#define CONFIG_NO_RANDOM_POOL
#define CONFIG_NO_STDOUT_DEBUG
#define CONFIG_NO_WPA_MSG
#define OS_NO_C_LIB_DEFINES
#define CONFIG_NO_CONFIG_WRITE
#define CONFIG_NO_CONFIG_BLOBS
#define CONFIG_NO_HOSTAPD_LOGGER
#define PROBE_REQ_RESP_SUPPORT
#ifdef CHIP_9117
#define CONFIG_MBO_STA
#define CONFIG_WNM
#endif

/* Insert configuration defines, e.g., #define EAP_MD5, here, if needed. */

#ifdef CONFIG_WIN32_DEFAULTS
#define CONFIG_NATIVE_WINDOWS
#define CONFIG_ANSI_C_EXTRA
#define CONFIG_WINPCAP
#define IEEE8021X_EAPOL
#define PKCS12_FUNCS
#define PCSC_FUNCS
#define CONFIG_CTRL_IFACE
#define CONFIG_CTRL_IFACE_NAMED_PIPE
#define CONFIG_DRIVER_NDIS
#define CONFIG_NDIS_EVENTS_INTEGRATED
#define CONFIG_DEBUG_FILE
#define EAP_MD5
#define EAP_TLS
#define EAP_MSCHAPv2
#define EAP_PEAP
#define EAP_TTLS
#define EAP_GTC
#define EAP_OTP
#define EAP_LEAP
#define EAP_TNC
#define _CRT_SECURE_NO_DEPRECATE

#ifdef USE_INTERNAL_CRYPTO
#define CONFIG_TLS_INTERNAL_CLIENT
#define CONFIG_INTERNAL_LIBTOMMATH
#define CONFIG_CRYPTO_INTERNAL
#endif /* USE_INTERNAL_CRYPTO */
#endif /* CONFIG_WIN32_DEFAULTS */
#ifdef USE_INTERNAL_CRYPTO
#define CONFIG_TLS_INTERNAL_CLIENT
#define CONFIG_INTERNAL_LIBTOMMATH
#define CONFIG_CRYPTO_INTERNAL
#endif /* USE_INTERNAL_CRYPTO */
#if 1
//#define SERVICE_DISCOVERY_SUPPORT
//#define CONFIG_P2P_MANAGER 1
//#define PERSISTENT_SUPPORT
//#define INVITATION_SUPPORT
#define CONFIG_OFFCHANNEL
#define PRESENCE_REQ_SUPPORT
#define NOA_SUPPORT
#define EXT_LISTEN_SUPPORT
#define PEER_FILTERING_SUPPORT
#define DEV_DISCOVERY_SUPPORT
#define CROSS_CONNECT_SUPPORT
#endif

#define CONFIG_OPTIMIZE_CODE
#endif /* BUILD_CONFIG_H */
