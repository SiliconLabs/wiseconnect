#include <string.h>
#include "network_interface.h"
#include "socket.h"
#include "netinet6_in6.h"
#include "sl_net_dns.h"
#include "sl_constants.h"
#include "sl_si91x_constants.h"
#include "sl_si91x_core_utilities.h"
#include "sl_si91x_protocol_types.h"
#include "errno.h"
#include "aws_iot_error.h"

/******************************************************
*                    Macros
******************************************************/
#define IPV6              (1 << 0)
#define CLIENT_PORT       4002
#define MAX_DNS_REQ_COUNT 5
#define TLS_SOCKET        2
#define SL_CERT_INDEX_0   0

/******************************************************
*               Variable Declarations
******************************************************/
static int32_t get_aws_error(int32_t status);
static int32_t ConnecttoNetwork(Network *n, uint8_t flags, sl_ip_address_t *addr, int dst_port, int src_port);

/******************************************************
*               Function Definitions
******************************************************/
static int32_t get_aws_error(int32_t status)
{
  switch (status) {
    case EBADF:
    case EINVAL:
    case EISCONN:
    case EFAULT:
    case ENOBUFS:
    case EPROTOTYPE:
    case EAFNOSUPPORT:
    case EADDRINUSE:
    case ENOPROTOOPT:
      return TCP_CONNECTION_ERROR;
    // TODO: Handle a mapping to convert Si91x firmware errors to AWS errors.
    case SL_STATUS_SI91X_SOCKET_READ_TIMEOUT:
      return NETWORK_SSL_READ_TIMEOUT_ERROR;
    default:
      return FAILURE;
  }
}

int32_t _iot_tls_verify_cert(void *data, int *crt, int depth, uint32_t *flags)
{
  UNUSED_PARAMETER(data);
  UNUSED_PARAMETER(crt);
  UNUSED_PARAMETER(depth);
  UNUSED_PARAMETER(flags);
  // TODO: Implement this function if external stack is used.
  // NOTE: Verification of certificate takes place in firmware when internal stack feature is used. So API returns SUCCESS.
  return SUCCESS;
}

IoT_Error_t iot_tls_init(Network *pNetwork,
                         char *pRootCALocation,
                         char *pDeviceCertLocation,
                         char *pDevicePrivateKeyLocation,
                         char *pDestinationURL,
                         uint16_t DestinationPort,
                         uint32_t timeout_ms,
                         bool ServerVerificationFlag)
{
  pNetwork->tlsConnectParams.DestinationPort           = DestinationPort;
  pNetwork->tlsConnectParams.pDestinationURL           = pDestinationURL;
  pNetwork->tlsConnectParams.pDeviceCertLocation       = pDeviceCertLocation;
  pNetwork->tlsConnectParams.pDevicePrivateKeyLocation = pDevicePrivateKeyLocation;
  pNetwork->tlsConnectParams.pRootCALocation           = pRootCALocation;
  pNetwork->tlsConnectParams.timeout_ms                = timeout_ms;
  pNetwork->tlsConnectParams.ServerVerificationFlag    = ServerVerificationFlag;

  pNetwork->connect     = iot_tls_connect;
  pNetwork->read        = iot_tls_read;
  pNetwork->write       = iot_tls_write;
  pNetwork->disconnect  = iot_tls_disconnect;
  pNetwork->isConnected = iot_tls_is_connected;
  pNetwork->destroy     = iot_tls_destroy;

  return SUCCESS;
}

IoT_Error_t iot_tls_is_connected(Network *pNetwork)
{
  UNUSED_PARAMETER(pNetwork);
  /* Use this to add implementation which can check for physical layer disconnect */
  return NETWORK_PHYSICAL_LAYER_CONNECTED;
}

static int32_t ConnecttoNetwork(Network *n, uint8_t flags, sl_ip_address_t *addr, int dst_port, int src_port)
{
  int32_t status  =0;
  struct sockaddr_in client_addr   = { 0 };
  struct sockaddr_in server_addr   = { 0 };
//  struct sockaddr_in6 client_addr6 = { 0 };
  struct sockaddr_in6 server_addr6 = { 0 };

  n->socket_id = socket(((flags & IPV6) ? AF_INET6 : AF_INET), SOCK_STREAM, IPPROTO_TCP);
  if (n->socket_id < 0) {
    return get_aws_error(errno);
  }

  if (flags & TLS_SOCKET) {
    if (setsockopt(n->socket_id, SOL_TCP, TCP_ULP, TLS, sizeof(TLS))) {
      return get_aws_error(errno);
    }

    uint8_t certificate_index = SL_CERT_INDEX_0;
    if (setsockopt(n->socket_id, SOL_SOCKET, SO_CERT_INDEX, &certificate_index, sizeof(certificate_index))) {
      return get_aws_error(errno);
    }
  }

  if (flags & IPV6) {
//    client_addr6.sin6_family = AF_INET6;
//    client_addr6.sin6_port   = src_port;
    server_addr6.sin6_family = AF_INET6;
    server_addr6.sin6_port   = dst_port;
    memcpy(&server_addr6.sin6_addr.__u6_addr.__u6_addr8, &addr->ip.v6.value, SL_IPV6_ADDRESS_LENGTH);
  } else {
    client_addr.sin_family = AF_INET;
    client_addr.sin_port   = src_port;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = dst_port;
    memcpy(&server_addr.sin_addr.s_addr, &addr->ip.v4.value, SL_IPV4_ADDRESS_LENGTH);
  }

  status = bind( n->socket_id, (struct sockaddr *)&client_addr, sizeof(client_addr));
  if (status < 0) {
    close( n->socket_id);
    return get_aws_error(errno);
  }

  status = connect( n->socket_id, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (status < 0) {
    close( n->socket_id);
    return get_aws_error(errno);
  }

  return SUCCESS;
}

IoT_Error_t iot_tls_connect(Network *pNetwork, TLSConnectParams *params)
{
  UNUSED_PARAMETER(params);
  int32_t status                     = 0;
  int8_t count                       = MAX_DNS_REQ_COUNT;
  sl_ip_address_t dns_query_response = { 0 };

  do {
    status = sl_dns_host_get_by_name((char *)pNetwork->tlsConnectParams.pDestinationURL,
                                     SL_SI91X_WAIT_FOR_DNS_RESOLUTION,
                                     SL_NET_DNS_TYPE_IPV4, // TODO: handle ipv6
                                     &dns_query_response);
    if (status == SL_STATUS_OK) {
      break;
    }
    count--;
  } while (count > 0);

  if (status) {
    return NETWORK_ERR_NET_UNKNOWN_HOST;
  }

  status = ConnecttoNetwork(pNetwork,
                            TLS_SOCKET,
                            &dns_query_response,
                            pNetwork->tlsConnectParams.DestinationPort,
                            CLIENT_PORT);
  return status;
}

IoT_Error_t iot_tls_write(Network *pNetwork, unsigned char *pMsg, size_t len, Timer *timer, size_t *written_len)
{
  size_t bytes_written = 0;
  size_t temp_len      = len;

  if (len <= 0) {
    return MQTT_TX_BUFFER_TOO_SHORT_ERROR;
  }

  do {
    bytes_written = send(pNetwork->socket_id, pMsg, temp_len, 0);
    if (bytes_written <= 0) {
      return NETWORK_SSL_WRITE_ERROR;
    }
    pMsg += bytes_written;
    temp_len -= bytes_written;
  } while ((temp_len > 0) && !has_timer_expired(timer));

  *written_len = bytes_written;
  return ((bytes_written >= len) ? SUCCESS : NETWORK_SSL_WRITE_TIMEOUT_ERROR);
}

IoT_Error_t iot_tls_read(Network *pNetwork, unsigned char *pMsg, size_t len, Timer *timer, size_t *read_len)
{
  int32_t status          = 0;
  int32_t bytes_read      = 0;
  size_t total_bytes_read = 0;
  size_t temp_len         = len;
  sl_si91x_time_value timeout = { 0 };

  if (len <= 0) {
    return MQTT_RX_BUFFER_TOO_SHORT_ERROR;
  }

  timeout.tv_usec = timer->timeout * 1000;

  status = setsockopt(pNetwork->socket_id, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
  if (status) {
    return get_aws_error(errno);
  }

  do {
    bytes_read = recv(pNetwork->socket_id, pMsg, temp_len, 0);
    if (bytes_read == 0) {
      return NETWORK_SSL_READ_ERROR;
    } else if (bytes_read < 0) {
      return get_aws_error(get_saved_firmware_status());
    }

    total_bytes_read += bytes_read;
    pMsg += bytes_read;
    temp_len -= bytes_read;

  } while ((temp_len > 0) && !has_timer_expired(timer));

  *read_len = total_bytes_read;
  return (total_bytes_read >= len) ? SUCCESS : NETWORK_SSL_READ_TIMEOUT_ERROR;
}

IoT_Error_t iot_tls_disconnect(Network *pNetwork)
{
  int32_t status = close(pNetwork->socket_id);
  if (status) {
    return get_aws_error(status);
  }
  return SUCCESS;
}

IoT_Error_t iot_tls_destroy(Network *pNetwork)
{
  UNUSED_PARAMETER(pNetwork);
  // TODO: Implement this function when external TLS stack is used.
  // NOTE: TLS functionality takes place in firmware when internal stack feature is used. So based on the previous errno value from iot_tls_disconnect() status we return SUCCESS/FAILURE.
  if (errno)
    return FAILURE;
  else
    return SUCCESS;
}
