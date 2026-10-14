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
#include <gtest/gtest.h>
#include <cstdint> // For u_int32_t
#include <cstring>
#include <cstdlib>
#include <string>
extern "C" {
#include "sli_async_socket_fake_functions.h"
#include "sl_si91x_socket_constants.h"
#include "sl_si91x_socket.h"
#include "sl_si91x_socket_utility.h"
#include "sl_constants.h"
#include <errno.h>
#include <stdint.h>
}

static int sli_configure_tls_extension_enomem_fake(sli_si91x_tls_extensions_t *socket_tls_extensions,
                                                   const sl_si91x_socket_type_length_value_t *tls_extension,
                                                   socklen_t option_length)
{
  (void)socket_tls_extensions;
  (void)tls_extension;
  (void)option_length;
  errno = ENOMEM;
  return -1;
}

static int sli_configure_tls_extension_einval_fake(sli_si91x_tls_extensions_t *socket_tls_extensions,
                                                   const sl_si91x_socket_type_length_value_t *tls_extension,
                                                   socklen_t option_length)
{
  (void)socket_tls_extensions;
  (void)tls_extension;
  (void)option_length;
  errno = EINVAL;
  return -1;
}

#define IPADDR_LOOPBACK ((uint32_t)0x7f000001UL)

uint16_t custom_htons(uint16_t value)
{
  return ((value & 0x00FF) << 8) | ((value & 0xFF00) >> 8);
}

uint32_t custom_htonl(uint32_t value)
{
  return ((value & 0x000000FF) << 24) | ((value & 0x0000FF00) << 8) | ((value & 0x00FF0000) >> 8)
         | ((value & 0xFF000000) >> 24);
}

// Custom fake function for sl_redirect_log
void sl_redirect_log(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  vprintf(format, args); // Simulate logging
  va_end(args);
}

// Helper function to prepare test data for sendto_async tests
struct SendToAsyncTestParams {
  uint8_t buffer[32];
  size_t buffer_length;
  int32_t flags;
  struct sockaddr_in addr;
  socklen_t addr_len;
  sl_si91x_socket_data_transfer_complete_handler_t callback = nullptr;

  SendToAsyncTestParams() : buffer_length(sizeof(buffer)), flags(0), addr_len(sizeof(addr)), callback(nullptr)
  {
    memcpy(buffer, "Test data", 9);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_port        = custom_htons(8080);
    addr.sin_addr.s_addr = custom_htonl(IPADDR_LOOPBACK);
  }
};

// Test Case : Test SL_SI91X_SO_DTLS_ENABLE option
TEST(sli_async_socket_unit_tests, enable_DTLS)
{
  int32_t sockID       = 1;
  int level            = 0;
  int option_name      = SL_SI91X_SO_DTLS_ENABLE;
  uint16_t enable_dtls = SL_SI91X_ENABLE_DTLS;
  socklen_t option_len = sizeof(enable_dtls);

  sli_si91x_socket_t mock_socket_instance;
  sli_get_si91x_socket_fake.return_val = &mock_socket_instance;

  int result = sl_si91x_setsockopt(sockID, level, option_name, &enable_dtls, option_len);

  EXPECT_TRUE(result == SLI_SI91X_NO_ERROR);
}

// Test Case : Test SL_SI91X_SO_DTLS_ENABLE option with invalid configuration
TEST(sli_async_socket_unit_tests, enable_DTLS_invalid)
{
  int32_t sockID  = 1;
  int level       = 0;
  int option_name = SL_SI91X_SO_DTLS_ENABLE;

  uint16_t enable_dtls_invalid = SL_SI91X_ENABLE_DTLS | 0xFFFF;

  socklen_t option_len = sizeof(enable_dtls_invalid);

  sli_si91x_socket_t mock_socket_instance;
  sli_get_si91x_socket_fake.return_val = &mock_socket_instance;

  int result = sl_si91x_setsockopt(sockID, level, option_name, &enable_dtls_invalid, option_len);
  EXPECT_TRUE(result == -1);
}

// Test Case : Test SL_SI91X_SO_DTLS_V_1_0_ENABLE option
TEST(sli_async_socket_unit_tests, enable_DTLS_V_1_0)
{
  int32_t sockID       = 1;
  int level            = 0;
  int option_name      = SL_SI91X_SO_DTLS_V_1_0_ENABLE;
  uint16_t enable_dtls = SL_SI91X_ENABLE_DTLS | SL_SI91X_DTLS_V_1_0;
  socklen_t option_len = sizeof(enable_dtls);

  sli_si91x_socket_t mock_socket_instance;
  sli_get_si91x_socket_fake.return_val = &mock_socket_instance;

  int result = sl_si91x_setsockopt(sockID, level, option_name, &enable_dtls, option_len);

  EXPECT_TRUE(result == SLI_SI91X_NO_ERROR);
}

// Test Case: Test SL_SI91X_SO_DTLS_V_1_0_ENABLE with invalid configuration
TEST(sli_async_socket_unit_tests, enable_DTLS_V_1_0_invalid_version)
{
  int32_t sockID  = 1;
  int level       = 0;
  int option_name = SL_SI91X_SO_DTLS_V_1_0_ENABLE;

  uint16_t enable_dtls_invalid = SL_SI91X_ENABLE_DTLS | SL_SI91X_DTLS_V_1_0 | 0xFFFF;

  socklen_t option_len = sizeof(enable_dtls_invalid);

  sli_si91x_socket_t mock_socket_instance;
  sli_get_si91x_socket_fake.return_val = &mock_socket_instance;

  int result = sl_si91x_setsockopt(sockID, level, option_name, &enable_dtls_invalid, option_len);

  EXPECT_TRUE(result == -1);
}

// Test Case : Test SL_SI91X_SO_DTLS_V_1_2_ENABLE option
TEST(sli_async_socket_unit_tests, enable_DTLS_V_1_2)
{
  int32_t sockID       = 1;
  int level            = 0;
  int option_name      = SL_SI91X_SO_DTLS_V_1_2_ENABLE;
  uint16_t enable_dtls = SL_SI91X_ENABLE_DTLS | SL_SI91X_DTLS_V_1_2;
  socklen_t option_len = sizeof(enable_dtls);

  sli_si91x_socket_t mock_socket_instance;
  sli_get_si91x_socket_fake.return_val = &mock_socket_instance;

  int result = sl_si91x_setsockopt(sockID, level, option_name, &enable_dtls, option_len);

  EXPECT_TRUE(result == SLI_SI91X_NO_ERROR);
}

// Test Case: Test SL_SI91X_SO_DTLS_V_1_2_ENABLE with invalid configuration
TEST(sli_async_socket_unit_tests, enable_DTLS_V_1_2_invalid_version)
{
  int32_t sockID  = 1;
  int level       = 0;
  int option_name = SL_SI91X_SO_DTLS_V_1_2_ENABLE;

  uint16_t enable_dtls_invalid = SL_SI91X_ENABLE_DTLS | SL_SI91X_DTLS_V_1_2 | 0xFFFF;

  socklen_t option_len = sizeof(enable_dtls_invalid);

  sli_si91x_socket_t mock_socket_instance;
  sli_get_si91x_socket_fake.return_val = &mock_socket_instance;

  int result = sl_si91x_setsockopt(sockID, level, option_name, &enable_dtls_invalid, option_len);

  EXPECT_TRUE(result == -1);
}

// Test Case: Test SL_SI91X_SO_DTLS_V_1_2_ENABLE with missing or wrong DTLS version flag
TEST(sli_async_socket_unit_tests, enable_DTLS_V_1_2_invalid_flag)
{
  int32_t sockID  = 1;
  int level       = 0;
  int option_name = SL_SI91X_SO_DTLS_V_1_2_ENABLE;

  uint16_t enable_dtls_invalid = SL_SI91X_ENABLE_DTLS | SL_SI91X_DTLS_V_1_0;

  socklen_t option_len = sizeof(enable_dtls_invalid);

  sli_si91x_socket_t mock_socket_instance;
  sli_get_si91x_socket_fake.return_val = &mock_socket_instance;

  int result = sl_si91x_setsockopt(sockID, level, option_name, &enable_dtls_invalid, option_len);

  EXPECT_TRUE(result == -1);
}

// Test case for successful data send with TCP and ACK indication
TEST(sli_async_socket_unit_tests, SendToAsync_Success)
{
  RESET_FAKE(sli_get_si91x_socket);
  RESET_FAKE(sli_si91x_send_socket_data);

  // Mock behavior for sli_get_si91x_socket
  sli_si91x_socket_t mock_socket;
  memset(&mock_socket, 0, sizeof(mock_socket));
  mock_socket.id                        = 0;           // Default socket ID
  mock_socket.type                      = SOCK_STREAM; // Set type to SOCK_STREAM for TCP
  mock_socket.state                     = CONNECTED;
  mock_socket.local_address.sin6_family = AF_INET;                          // Set family to AF_INET
  mock_socket.mss                       = SLI_DEFAULT_STREAM_MSS_SIZE_IPV4; // Set default MSS for IPv4
  mock_socket.is_waiting_on_ack         = false;
  sli_get_si91x_socket_fake.return_val  = &mock_socket;

  // Mock behavior for sli_si91x_send_socket_data
  sli_si91x_send_socket_data_fake.return_val = SL_STATUS_IN_PROGRESS;

  int socket = 1;
  SendToAsyncTestParams params;

  int result = sl_si91x_sendto_async(socket,
                                     params.buffer,
                                     params.buffer_length,
                                     params.flags,
                                     (struct sockaddr *)&params.addr,
                                     params.addr_len,
                                     params.callback);

  EXPECT_EQ(result, params.buffer_length);
  EXPECT_FALSE(mock_socket.is_waiting_on_ack);
}

// Test Case: Valid UDP Socket
TEST(sli_async_socket_unit_tests, valid_udp_Socket_request)
{
  RESET_FAKE(sli_get_si91x_socket);
  RESET_FAKE(sli_si91x_send_socket_data);

  // Mock behavior for sli_get_si91x_socket
  sli_si91x_socket_t mock_socket;
  memset(&mock_socket, 0, sizeof(mock_socket));
  mock_socket.id                       = 0; // Default socket ID
  mock_socket.type                     = SOCK_DGRAM;
  mock_socket.state                    = BOUND;
  sli_get_si91x_socket_fake.return_val = &mock_socket;

  // Mock behavior for sli_si91x_send_socket_data
  sli_si91x_send_socket_data_fake.return_val = SL_STATUS_IN_PROGRESS;

  int socket = 1;
  SendToAsyncTestParams params;

  // Call the function under test
  int result = sl_si91x_sendto_async(socket,
                                     params.buffer,
                                     params.buffer_length,
                                     params.flags,
                                     (struct sockaddr *)&params.addr,
                                     params.addr_len,
                                     params.callback);

  // Validate the result
  EXPECT_EQ(result, params.buffer_length);
  EXPECT_EQ(sli_si91x_send_socket_data_fake.call_count, 1);
}

// Test case: Invalid socket
TEST(sli_async_socket_unit_tests, send_to_async_invalid_socket)
{
  // Mock behavior for `sli_get_si91x_socket`
  sli_get_si91x_socket_fake.return_val = nullptr; // Simulate invalid socket

  int socket = -1; // Invalid socket
  SendToAsyncTestParams params;

  int result = sl_si91x_sendto_async(socket,
                                     params.buffer,
                                     params.buffer_length,
                                     params.flags,
                                     (struct sockaddr *)&params.addr,
                                     params.addr_len,
                                     params.callback);

  EXPECT_EQ(result, -1);   // Ensure the function returns -1 for invalid socket
  EXPECT_EQ(errno, EBADF); // Ensure errno is set to EBADF
}

// Test case: Buffer overflow
TEST(sli_async_socket_unit_tests, send_to_async_buffer_overflow)
{
  // Create a mock socket variable
  sli_si91x_socket_t mock_socket;
  memset(&mock_socket, 0, sizeof(mock_socket));

  // Set up mock socket properties
  mock_socket.id                        = 1;
  mock_socket.type                      = SOCK_STREAM;
  mock_socket.state                     = CONNECTED;
  mock_socket.local_address.sin6_family = AF_INET;
  mock_socket.mss                       = 1500;

  // Mock behavior for `sli_get_si91x_socket`
  sli_get_si91x_socket_fake.return_val = &mock_socket;

  int socket = 1;
  SendToAsyncTestParams params;
  // Overwrite buffer_length to simulate overflow
  params.buffer_length = 2000;

  int result = sl_si91x_sendto_async(socket,
                                     params.buffer,
                                     params.buffer_length,
                                     params.flags,
                                     (struct sockaddr *)&params.addr,
                                     params.addr_len,
                                     params.callback);

  EXPECT_EQ(result, -1);      // Ensure the function returns -1 for buffer overflow
  EXPECT_EQ(errno, EMSGSIZE); // Ensure errno is set to EMSGSIZE
}

// Test Case: Message Size Exceeding Limit
TEST(sli_async_socket_unit_tests, message_size_exceeds_limit)
{
  RESET_FAKE(sli_get_si91x_socket);

  // Mock behavior for sli_get_si91x_socket
  sli_si91x_socket_t mock_socket;
  memset(&mock_socket, 0, sizeof(mock_socket));
  mock_socket.id                        = 0; // Default socket ID
  mock_socket.type                      = SOCK_DGRAM;
  mock_socket.state                     = CONNECTED;
  mock_socket.local_address.sin6_family = AF_INET;
  sli_get_si91x_socket_fake.return_val  = &mock_socket;

  int socket = 1;
  SendToAsyncTestParams params;
  // Overwrite buffer_length to exceed max datagram size
  params.buffer_length = SLI_DEFAULT_DATAGRAM_MSS_SIZE_IPV4 + 1;

  int result = sl_si91x_sendto_async(socket,
                                     params.buffer,
                                     params.buffer_length,
                                     params.flags,
                                     (struct sockaddr *)&params.addr,
                                     params.addr_len,
                                     params.callback);

  EXPECT_EQ(result, -1);
  EXPECT_EQ(errno, EMSGSIZE);
}

// Test Case: Invalid Buffer Length
TEST(sli_async_socket_unit_tests, invalid_buffer_length)
{
  RESET_FAKE(sli_get_si91x_socket);

  // Mock behavior for sli_get_si91x_socket
  sli_si91x_socket_t mock_socket;
  memset(&mock_socket, 0, sizeof(mock_socket));
  mock_socket.id                       = 0; // Default socket ID
  mock_socket.type                     = SOCK_DGRAM;
  mock_socket.state                    = CONNECTED;
  sli_get_si91x_socket_fake.return_val = &mock_socket;

  int socket = 1;
  SendToAsyncTestParams params;
  mock_socket.local_address.sin6_family = AF_INET6;

  // Set an invalid buffer_length
  params.buffer_length = 1453;

  int result = sl_si91x_sendto_async(socket,
                                     params.buffer,
                                     params.buffer_length,
                                     params.flags,
                                     (struct sockaddr *)&params.addr,
                                     params.addr_len,
                                     params.callback);

  EXPECT_EQ(result, -1);
  EXPECT_EQ(errno, EMSGSIZE);
}

namespace {

sl_si91x_socket_type_length_value_t *make_tls_alpn_tlv(socklen_t *option_length)
{
  const char *protocol      = "mqtt";
  const size_t value_length = strlen(protocol);
  const size_t total_size   = sizeof(sl_si91x_socket_type_length_value_t) + value_length;
  auto *tlv                 = (sl_si91x_socket_type_length_value_t *)malloc(total_size);

  tlv->type   = SL_SI91X_TLS_EXTENSION_ALPN_TYPE;
  tlv->length = (uint16_t)value_length;
  memcpy(tlv->value, protocol, value_length);
  *option_length = (socklen_t)total_size;
  return tlv;
}

} // namespace

TEST(sli_async_socket_unit_tests, SetSockopt_TlsAlpn_Success)
{
  RESET_FAKE(sli_get_si91x_socket);
  RESET_FAKE(sli_si91x_configure_tls_extension);

  sli_si91x_socket_t mock_socket_instance           = { 0 };
  sli_get_si91x_socket_fake.return_val              = &mock_socket_instance;
  sli_si91x_configure_tls_extension_fake.return_val = SLI_SI91X_NO_ERROR;

  socklen_t option_length                  = 0;
  sl_si91x_socket_type_length_value_t *tlv = make_tls_alpn_tlv(&option_length);

  const int result = sl_si91x_setsockopt(1, 0, SL_SI91X_SO_TLS_ALPN, tlv, option_length);

  EXPECT_EQ(result, SLI_SI91X_NO_ERROR);
  EXPECT_EQ(sli_si91x_configure_tls_extension_fake.call_count, 1u);
  free(tlv);
}

TEST(sli_async_socket_unit_tests, SetSockopt_TlsAlpn_MemoryError)
{
  RESET_FAKE(sli_get_si91x_socket);
  RESET_FAKE(sli_si91x_configure_tls_extension);

  sli_si91x_socket_t mock_socket_instance            = { 0 };
  sli_get_si91x_socket_fake.return_val               = &mock_socket_instance;
  sli_si91x_configure_tls_extension_fake.custom_fake = sli_configure_tls_extension_enomem_fake;

  socklen_t option_length                  = 0;
  sl_si91x_socket_type_length_value_t *tlv = make_tls_alpn_tlv(&option_length);

  const int result = sl_si91x_setsockopt(1, 0, SL_SI91X_SO_TLS_ALPN, tlv, option_length);

  EXPECT_EQ(result, -1);
  EXPECT_EQ(errno, ENOMEM);
  EXPECT_EQ(sli_si91x_configure_tls_extension_fake.call_count, 1u);
  free(tlv);
}

TEST(sli_async_socket_unit_tests, SetSockopt_TlsAlpn_InvalidParameter)
{
  RESET_FAKE(sli_get_si91x_socket);
  RESET_FAKE(sli_si91x_configure_tls_extension);

  sli_si91x_socket_t mock_socket_instance            = { 0 };
  sli_get_si91x_socket_fake.return_val               = &mock_socket_instance;
  sli_si91x_configure_tls_extension_fake.custom_fake = sli_configure_tls_extension_einval_fake;

  socklen_t option_length                  = 0;
  sl_si91x_socket_type_length_value_t *tlv = make_tls_alpn_tlv(&option_length);

  const int result = sl_si91x_setsockopt(1, 0, SL_SI91X_SO_TLS_ALPN, tlv, option_length);

  EXPECT_EQ(result, -1);
  EXPECT_EQ(errno, EINVAL);
  EXPECT_EQ(sli_si91x_configure_tls_extension_fake.call_count, 1u);
  free(tlv);
}

TEST(sli_async_socket_unit_tests, SetSockopt_TlsAlpn_NullPointer)
{
  RESET_FAKE(sli_get_si91x_socket);
  RESET_FAKE(sli_si91x_configure_tls_extension);

  sli_si91x_socket_t mock_socket_instance            = { 0 };
  sli_get_si91x_socket_fake.return_val               = &mock_socket_instance;
  sli_si91x_configure_tls_extension_fake.custom_fake = sli_configure_tls_extension_einval_fake;

  socklen_t option_length                  = 0;
  sl_si91x_socket_type_length_value_t *tlv = make_tls_alpn_tlv(&option_length);

  const int result = sl_si91x_setsockopt(1, 0, SL_SI91X_SO_TLS_ALPN, tlv, option_length);

  EXPECT_EQ(result, -1);
  EXPECT_EQ(errno, EINVAL);
  EXPECT_EQ(sli_si91x_configure_tls_extension_fake.call_count, 1u);
  free(tlv);
}
