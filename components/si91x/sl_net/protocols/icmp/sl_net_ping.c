#include "sl_status.h"
#include "sl_constants.h"
#include "sl_net_ping.h"
#include "sl_si91x_driver.h"
#include <string.h>

/******************************************************
 *                      Macros
 ******************************************************/
// This macro must be used while sending ping timeout in sl_si91x_ping_request_t
#define CONVERT_TO_SI91X_PING_TIMEOUT(timeout) (timeout / 100)

/******************************************************
 *                    Constants
 ******************************************************/
#define PING_RESPONSE_TIMEOUT_MS 1000 // timeout in ms

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Extern Variable
 ******************************************************/
extern bool device_initialized;

/******************************************************
 *               Global Variable
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

sl_status_t sl_si91x_send_ping(sl_ip_address_t ip_address, uint16_t ping_size)
{
  sl_status_t status              = SL_STATUS_OK;
  sl_si91x_ping_request_t request = { 0 };

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if ((ip_address.type != SL_IPV4) && (ip_address.type != SL_IPV6)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (ip_address.type == SL_IPV6) {
    request.ip_version = SL_IPV6_VERSION;
    memcpy(&request.ping_address.ipv6_address, &ip_address.ip.v6, SL_IPV6_ADDRESS_LENGTH); // Copy IPv6 address
  } else {
    request.ip_version = SL_IPV4_VERSION;
    memcpy(&request.ping_address.ipv4_address, &ip_address.ip.v4, SL_IPV4_ADDRESS_LENGTH); // Copy IPv4 address
  }

  request.ping_size = ping_size;                                               // Copy Ping size
  request.timeout   = CONVERT_TO_SI91X_PING_TIMEOUT(PING_RESPONSE_TIMEOUT_MS); // Copy Ping timeout

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_PING_PACKET,
                                        SI91X_NETWORK_CMD_QUEUE,
                                        &request,
                                        sizeof(sl_si91x_ping_request_t),
                                        SL_SI91X_RETURN_IMMEDIATELY,
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);

  return status;
}
