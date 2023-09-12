#pragma once
#include "stdint.h"

/******************************************************
 * *                      Macros
 * ******************************************************/
//SNTP client feature
#define SI91X_SNTP_CLIENT_CREATE            1
#define SI91X_SNTP_CLIENT_GETTIME           2
#define SI91X_SNTP_CLIENT_GETTIME_DATE      3
#define SI91X_SNTP_CLIENT_GETSERVER_ADDRESS 4
#define SI91X_SNTP_CLIENT_DELETE            5
#define SI91X_SNTP_CLIENT_GET_SERVER_INFO   6
#define SI91X_SNTP_CLIENT_SERVER_ASYNC_RSP  7

/******************************************************
 * *                 Type Definitions
 * ******************************************************/

// Define for SNTP client initialization
typedef struct {
  uint8_t command_type;
  uint8_t ip_version;
  union {
    uint8_t ipv4_address[4];
    uint8_t ipv6_address[16];
  } server_ip_address;
  uint8_t sntp_method;
  uint8_t sntp_timeout[2];
} si91x_sntp_client_t;

typedef struct {
  uint8_t command_type;
  uint8_t ip_version;
  union {
    uint8_t ipv4_address[4];
    uint8_t ipv6_address[16];
  } server_ip_address;
  uint8_t sntp_method;
} si91x_sntp_server_info_rsp_t;

typedef struct {
  uint8_t ip_version;
  union {
    uint8_t ipv4_address[4];
    uint8_t ipv6_address[16];
  } server_ip_address;
  uint8_t sntp_method;
} si91x_sntp_server_rsp_t;