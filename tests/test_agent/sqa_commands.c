#include "console.h"
#include "socket.h"
#include "sl_constants.h"
#include "cmsis_os2.h"
#include "printf.h"
#include "sl_si91x_socket_support.h"
#include <stdio.h>
#include "sl_si91x_socket_support.h"
#ifdef SLI_SI91X_MCU_INTERFACE
#include "rsi_rom_clks.h"
#endif
#include "sl_si91x_socket.h"
#include "sl_si91x_socket_utility.h"
#include "sl_si91x_socket_constants.h"
#include "sl_si91x_core_utilities.h"

static void measure_throughput(uint32_t total_num_of_bytes, uint32_t test_timeout);
static uint32_t str_to_ipv4(const char *string);
sl_status_t udp_tx_test(int socket_id, struct sockaddr_in *remote_address, void *buffer, uint32_t buffer_length);
sl_status_t tcp_tx_test(int socket_id, struct sockaddr_in *remote_address, void *buffer, uint32_t buffer_length);
sl_status_t tcp_rx_test(int socket_id,
                        struct sockaddr_in *remote_address,
                        void *buffer,
                        uint32_t buffer_length,
                        uint8_t async_socket);
sl_status_t udp_rx_test(int socket_id,
                        struct sockaddr_in *remote_address,
                        void *buffer,
                        uint32_t buffer_length,
                        uint8_t async_socket);

#ifdef SLI_SI91X_MCU_INTERFACE
#define SOC_PLL_REF_FREQUENCY 40000000  /*<! PLL input REFERENCE clock 40MHZ */
#define PS4_SOC_FREQ          119000000 /*<! PLL out clock 100MHz            */
#endif

#ifdef SLI_SI91X_MCU_INTERFACE
uint32_t tick_count_s = 1;
#else
uint32_t tick_count_s = 1;
#endif

#define SL_HIGH_PERFORMANCE_SOCKET 128
#define IPV4_FORMAT                "%u.%u.%u.%u"

uint8_t remote_terminate;

/*****************************************************
 *                      Socket configuration
*****************************************************/
#define TOTAL_SOCKETS                   1  //@ Total number of sockets. TCP TX + TCP RX + UDP TX + UDP RX
#define TOTAL_TCP_SOCKETS               1  //@ Total TCP sockets. TCP TX + TCP RX
#define TOTAL_UDP_SOCKETS               0  //@ Total UDP sockets. UDP TX + UDP RX
#define TCP_TX_ONLY_SOCKETS             0  //@ Total TCP TX only sockets. TCP TX
#define TCP_RX_ONLY_SOCKETS             1  //@ Total TCP RX only sockets. TCP RX
#define UDP_TX_ONLY_SOCKETS             0  //@ Total UDP TX only sockets. UDP TX
#define UDP_RX_ONLY_SOCKETS             0  //@ Total UDP RX only sockets. UDP RX
#define TCP_RX_HIGH_PERFORMANCE_SOCKETS 1  //@ Total TCP RX High Performance sockets
#define TCP_RX_WINDOW_SIZE_CAP          10 //@ TCP RX Window size
#define TCP_RX_WINDOW_DIV_FACTOR        10 //@ TCP RX Window division factor

static sl_si91x_socket_config_t socket_config = {
  TOTAL_SOCKETS,                   // Total sockets
  TOTAL_TCP_SOCKETS,               // Total TCP sockets
  TOTAL_UDP_SOCKETS,               // Total UDP sockets
  TCP_TX_ONLY_SOCKETS,             // TCP TX only sockets
  TCP_RX_ONLY_SOCKETS,             // TCP RX only sockets
  UDP_TX_ONLY_SOCKETS,             // UDP TX only sockets
  UDP_RX_ONLY_SOCKETS,             // UDP RX only sockets
  TCP_RX_HIGH_PERFORMANCE_SOCKETS, // TCP RX high performance sockets
  TCP_RX_WINDOW_SIZE_CAP,          // TCP RX window size
  TCP_RX_WINDOW_DIV_FACTOR         // TCP RX window division factor
};

#ifdef SLI_SI91X_MCU_INTERFACE
void switch_m4_frequency(void)
{
  /*Switch M4 SOC clock to Reference clock*/
  /*Default keep M4 in reference clock*/
  RSI_CLK_M4SocClkConfig(M4CLK, M4_ULPREFCLK, 0);
  /*Configure the PLL frequency*/
  RSI_CLK_SetSocPllFreq(M4CLK, PS4_SOC_FREQ, SOC_PLL_REF_FREQUENCY);
  /*Switch M4 clock to PLL clock for speed operations*/
  RSI_CLK_M4SocClkConfig(M4CLK, M4_SOCPLLCLK, 0);
}
#endif

uint16_t ttl_sec; // Hack to get things working
uint32_t total_bytes_received = 0;

void data_callback(uint32_t sock_no, uint8_t *buffer, uint32_t length)
{
  UNUSED_PARAMETER(sock_no);
  UNUSED_PARAMETER(buffer);
  total_bytes_received += length;
}

sl_status_t set_error()
{
  if (errno == 0) {
    return sl_si91x_get_saved_firmware_status();
  } else {
    return errno;
  }
}

sl_status_t iperf_command(console_args_t *arguments)
{
  int32_t protocol_family         = sl_cli_get_argument_int32(arguments, 0);
  int32_t type                    = sl_cli_get_argument_int32(arguments, 1); /* socket type: TCP is 1, UDP is 2 */
  int32_t protocol                = sl_cli_get_argument_int32(arguments, 2);
  uint16_t local_port             = sl_cli_get_argument_int16(arguments, 3);
  char *local_ip_string           = sl_cli_get_argument_string(arguments, 4);
  char *remote_ip_string          = sl_cli_get_argument_string(arguments, 5);
  uint16_t remote_port            = sl_cli_get_argument_uint16(arguments, 6);
  int32_t data_len                = sl_cli_get_argument_uint32(arguments, 7);
  ttl_sec                         = sl_cli_get_argument_uint16(arguments, 8);
  uint16_t throughput_type        = sl_cli_get_argument_uint16(arguments, 9);
  uint16_t qos                    = sl_cli_get_argument_uint16(arguments, 10);
  uint8_t async_socket            = sl_cli_get_argument_uint8(arguments, 11);
  uint8_t high_performance_socket = SL_HIGH_PERFORMANCE_SOCKET;

  total_bytes_received = 0;

  // yet to be implemented
  //  uint16_t is_async = sl_cli_get_argument_uint16(arguments, 11);
  //  char *tmp         = sl_cli_get_argument_string(arguments, 12);
  //
  //  uint8_t disable_bt_data_transfer;
  //  uint8_t disable_ble_data_transfer;
  //  uint8_t version;
  //  sscanf(tmp, "%03d", &disable_bt_data_transfer);
  //  sscanf(tmp + 4, "%03d", &disable_ble_data_transfer);
  //  sscanf(tmp + 8, "%03d", &version);

  //  UNUSED_PARAMETER(is_async);
  //  int8_t receive_completed = 0;
  sl_status_t status = SL_STATUS_OK;
  uint32_t local_ip  = str_to_ipv4(local_ip_string);
  uint32_t remote_ip = str_to_ipv4(remote_ip_string);
  int socket_id      = 0;

  if (local_ip == 0 || remote_ip == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (data_len == 0) {
    if (type == 1)
      data_len = 1460;
    else
      data_len = 1470;
  }
  if (ttl_sec == 0) {
    ttl_sec = 30;
  }

  //  int32_t total_bytes_tx = 0;
  //  int32_t tt_start       = 0;
  //  int32_t tt_end         = 0;
  //  int32_t packet_count   = 0;

  void *buffer = malloc(data_len);
  if (buffer == NULL) {
    printf("\r\n Unable to allocate the required buffer for the requested data size, %ld bytes", data_len);
    return SL_STATUS_ALLOCATION_FAILED;
  }
  memset(buffer, 0, data_len);
  struct sockaddr_in remote_address = {
    .sin_family = AF_INET,
    .sin_port   = remote_port,
    .sin_addr   = { .s_addr = remote_ip },
  };
  struct sockaddr_in local_address = {
    .sin_family = AF_INET,
    .sin_port   = local_port,
    .sin_addr   = { .s_addr = local_ip },
  };
  // generate random buffer
  //  set_random_bytes(buffer, data_len);

  // Create socket
  if (async_socket) {
    if (throughput_type == 3) {
      status = sl_si91x_config_socket(socket_config);
      if (status != SL_STATUS_OK) {
        free(buffer);
        printf("Socket config failed: %ld\r\n", status);
        return status;
      }
    }
    socket_id = sl_si91x_socket_async(protocol_family, type, protocol, &data_callback);
    if (socket_id < 0) {
      free(buffer);
      return set_error();
    }
    if (throughput_type == 3) {
      int ret = sl_si91x_setsockopt_async(socket_id,
                                          SOL_SOCKET,
                                          SL_SI91X_SO_HIGH_PERFORMANCE_SOCKET,
                                          &high_performance_socket,
                                          sizeof(high_performance_socket));
      if (ret < 0) {
        free(buffer);
        return set_error();
      }
    }
    int ret = sl_si91x_bind(socket_id, (struct sockaddr *)&local_address, sizeof(local_address));
    if (ret < 0) {
      close(socket_id);
      free(buffer);
      return set_error();
    }
  } else {
    socket_id = socket(protocol_family, type, protocol);
    if (socket_id < 0) {
      free(buffer);
      return set_error();
    }
    int sockopt_ret = setsockopt(socket_id, IPPROTO_IP, IP_TOS, &qos, sizeof(qos));
    if (sockopt_ret < 0) {
      close(socket_id);
      free(buffer);
      return set_error();
    }

    if (throughput_type == 3) {
      int ret = sl_si91x_set_custom_sync_sockopt(socket_id,
                                                 SOL_SOCKET,
                                                 SO_HIGH_PERFORMANCE_SOCKET,
                                                 &high_performance_socket,
                                                 sizeof(high_performance_socket));
      if (ret < 0) {
        free(buffer);
        return set_error();
      }
    }
    // Bind socket
    int ret = bind(socket_id, (struct sockaddr *)&local_address, sizeof(local_address));
    if (ret < 0) {
      close(socket_id);
      free(buffer);
      return set_error();
    }
  }

  switch (throughput_type) {
    case 1:
      status = tcp_tx_test(socket_id, &remote_address, buffer, data_len);
      break;

    case 2:
      status = udp_tx_test(socket_id, &remote_address, buffer, data_len);
      break;

    case 3:
      status = tcp_rx_test(socket_id, &remote_address, buffer, data_len, async_socket);
      break;

    case 4:
      status = udp_rx_test(socket_id, &remote_address, buffer, data_len, async_socket);
      break;
  }
  free(buffer);
  if (status != 0) {
    close(socket_id);
    return status;
  }
  int close_ret = close(socket_id);
  if (close_ret < 0) {
    return set_error();
  }
  return SL_STATUS_OK;
}

sl_status_t udp_tx_test(int socket_id, struct sockaddr_in *remote_address, void *buffer, uint32_t buffer_length)
{
  int receive_completed = 0;

  uint32_t total_bytes_tx = 0;
  uint32_t tt_end         = 0;
  uint32_t packet_count   = 0;
  uint32_t tt_start       = osKernelGetTickCount();

  while (1) {
    // Send data on socket
    ssize_t ret =
      sendto(socket_id, buffer, buffer_length, 0, (struct sockaddr *)remote_address, sizeof(*remote_address));
    if (ret < 0) {
      tt_end = osKernelGetTickCount();
      measure_throughput(total_bytes_tx, tt_end - tt_start);
      return set_error();
    }

    total_bytes_tx = total_bytes_tx + ret;
    tt_end         = osKernelGetTickCount();
    packet_count++;
    if (((tt_end - tt_start) / 1000 > ttl_sec) || receive_completed == 1)
      break;
  }

  measure_throughput(total_bytes_tx, tt_end - tt_start);

  return SL_STATUS_OK;
}

sl_status_t tcp_tx_test(int socket_id, struct sockaddr_in *remote_address, void *buffer, uint32_t buffer_length)
{
  int ret;
  uint32_t total_bytes_tx    = 0;
  uint32_t tt_start          = 0;
  uint32_t tt_end            = 0;
  uint32_t packet_count      = 0;
  uint32_t receive_completed = 0;

  ret = connect(socket_id, (struct sockaddr *)remote_address, sizeof(*remote_address));
  if (ret < 0) {
    return set_error();
  }

  // take the current time
  tt_start = osKernelGetTickCount();
  while (1) {
    if (remote_terminate) {
      break;
    }
    // Send data on socket
    ssize_t size = send(socket_id, buffer, buffer_length, 0);
    if (size < 0) {
      tt_end = osKernelGetTickCount();
      measure_throughput(total_bytes_tx, tt_end - tt_start);
      return set_error();
    }
    total_bytes_tx = total_bytes_tx + size;
    tt_end         = osKernelGetTickCount();
    packet_count++;
    if (((tt_end - tt_start) / 1000 > ttl_sec) || receive_completed == 1)
      break;
  }

  measure_throughput(total_bytes_tx, tt_end - tt_start);
  return SL_STATUS_OK;
}

sl_status_t tcp_rx_test(int socket_id,
                        struct sockaddr_in *remote_address,
                        void *buffer,
                        uint32_t buffer_length,
                        uint8_t async_socket)
{
  UNUSED_PARAMETER(remote_address);
  int ret;
  int client_socket          = -1;
  uint32_t tt_start          = 0;
  uint32_t tt_end            = 0;
  uint32_t receive_completed = 0;
  int read_bytes             = 1;
  sl_status_t status         = SL_STATUS_OK;

  if (async_socket) {
    ret = sl_si91x_listen(socket_id, 1);
    if (ret < 0) {
      return set_error();
    }
    client_socket = sl_si91x_accept(socket_id, NULL, 0);
    if (client_socket < 0) {
      return set_error();
    }
  } else {
    ret = listen(socket_id, 1);
    if (ret < 0) {
      return set_error();
    }
    client_socket = accept(socket_id, NULL, NULL);
    if (client_socket < 0) {
      return set_error();
    }
  }
  // take the current time
  tt_start = osKernelGetTickCount();
  while (1) {
    if (!async_socket) {
      read_bytes = recv(client_socket, buffer, buffer_length, 0);
      if (read_bytes < 0) {
        tt_end = osKernelGetTickCount();
        measure_throughput(total_bytes_received, tt_end - tt_start);
        return set_error();
      } else {
        total_bytes_received += read_bytes;
      }
      tt_end = osKernelGetTickCount();
    } else {
      tt_end = osKernelGetTickCount();
    }
    if (((tt_end - tt_start) / 1000 > ttl_sec) || receive_completed == 1)
      break;
  }
  measure_throughput(total_bytes_received, tt_end - tt_start);
  int close_ret = close(client_socket);
  if (close_ret < -1)
    return set_error();
  return status;
}

sl_status_t udp_rx_test(int socket_id,
                        struct sockaddr_in *remote_address,
                        void *buffer,
                        uint32_t buffer_length,
                        uint8_t async_socket)
{
  UNUSED_PARAMETER(remote_address);
  uint32_t tt_start          = 0;
  uint32_t tt_end            = 0;
  uint32_t receive_completed = 0;
  int read_bytes             = 1;

  // take the current time
  tt_start = osKernelGetTickCount();
  while (1) {
    if (!async_socket) {
      read_bytes = recvfrom(socket_id, buffer, buffer_length, 0, NULL, NULL);
      if (read_bytes < 0) {
        tt_end = osKernelGetTickCount();
        measure_throughput(total_bytes_received, tt_end - tt_start);
        return set_error();
      } else {
        total_bytes_received += read_bytes;
      }
      tt_end = osKernelGetTickCount();
    } else {
      tt_end = osKernelGetTickCount();
    }
    if (((tt_end - tt_start) / 1000 > ttl_sec) || receive_completed == 1)
      break;
  }
  measure_throughput(total_bytes_received, tt_end - tt_start);
  return SL_STATUS_OK;
}

static void measure_throughput(uint32_t total_num_of_bytes, uint32_t test_timeout)
{
  if (total_num_of_bytes > 0) {
    uint32_t duration = (test_timeout) / 1000 / tick_count_s; // milli sec to sec
    uint32_t result   = (total_num_of_bytes * 8) / duration;  // number of bytes per second
    result            = (result / 1000);                      // bps to Kbps
    printf("Throughput is @ %lu Kbps in %lu sec\n", result, duration);
  }
}

static uint32_t str_to_ipv4(const char *string)
{
  uint32_t temp[4];
  int nchars                        = -1;
  static const char str_to_ip_fmt[] = IPV4_FORMAT "%n";

  if (sscanf(string,
             str_to_ip_fmt,
             (unsigned int *)&temp[0],
             (unsigned int *)&temp[1],
             (unsigned int *)&temp[2],
             (unsigned int *)&temp[3],
             &nchars)
      != 4) {
    return 0;
  }
  // Check for extra characters not read in
  else if (*(string + nchars) != '\0') {
    return 0;
  } else if (temp[0] > 255 || temp[1] > 255 || temp[2] > 255 || temp[3] > 255) {
    return 0;
  }

  return (uint32_t)temp[3] << 24 | temp[2] << 16 | temp[1] << 8 | temp[0];
}

sl_status_t sl_send_data_config_pkts(console_args_t *arguments)
{
  struct sockaddr_in server_addr;
  int32_t socketID     = sl_cli_get_argument_uint32(arguments, 0);
  int32_t data_len     = sl_cli_get_argument_uint32(arguments, 1);
  int32_t no_of_pkts   = sl_cli_get_argument_uint32(arguments, 2);
  int32_t ttl_sec      = sl_cli_get_argument_uint32(arguments, 3);
  char *server_ip_str  = sl_cli_get_argument_string(arguments, 4);
  uint32_t server_port = sl_cli_get_argument_uint32(arguments, 5);

  int32_t status   = 0;
  uint32_t count   = 0;
  int32_t tt_start = 0, tt_end = 0;
  unsigned int sent_bytes = 0;

  uint32_t server_ip = str_to_ipv4(server_ip_str);
  if (data_len == 0) {
    data_len = 1460;
  }
  if (server_port) {
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = server_port;
    server_addr.sin_addr.s_addr = server_ip;
  }

  int8_t *buffer = malloc(data_len);
  if (buffer == NULL) {
    printf("\r\n Unable to allocate the required buffer for the requested data size, %ld bytes", data_len);
    return SL_STATUS_FAIL;
  }

  memset(buffer, 'a', data_len);
  tt_start = osKernelGetTickCount();
  for (count = 0; count < (uint32_t)no_of_pkts; count++) {
    if (((tt_end - tt_start) / 1000) > ttl_sec) {
      break;
    }

    if (server_port) {
      status = sendto(socketID, buffer, data_len, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    } else {
      status = send(socketID, buffer, data_len, 0);
    }
    if (status < 0) {
      free(buffer);
      printf("Bytes sent:%u\n", sent_bytes);
      return set_error();
    }
    sent_bytes += status;
    tt_end = osKernelGetTickCount();
  }

  if (status) {
    printf("Bytes sent:%u\n", sent_bytes);
    return SL_STATUS_OK;
  }

  free(buffer);
  return SL_STATUS_OK;
}

/* Function to get no of set bits in binary
representation of positive integer n */
unsigned int countSetBits(unsigned int number)
{
  unsigned int count = 0;
  while (number) {
    count += number & 1;
    number >>= 1;
  }
  return count;
}

sl_status_t send_data_on_multiple_sockets(console_args_t *arguments)
{
  uint32_t sock_list  = sl_cli_get_argument_uint32(arguments, 0);
  uint32_t data_len   = sl_cli_get_argument_uint32(arguments, 1);
  uint16_t ttl_sec    = sl_cli_get_argument_uint16(arguments, 2);
  uint32_t no_of_pkts = sl_cli_get_argument_uint32(arguments, 3);

  int32_t tt_start = 0, tt_end = 0;
  uint8_t sock_no          = 0;
  uint32_t status          = 0;
  uint32_t tx_stats[10][5] = { 0 };
  uint8_t max_sock         = 0;
  uint8_t flag             = 0;

  max_sock = countSetBits(sock_list);
  if (data_len == 0) {
    data_len = 1460;
  }

  int8_t *buffer = malloc(data_len);
  if (buffer == NULL) {
    printf("\r\n Unable to allocate the required buffer for the requested data size, %ld bytes", data_len);
    return SL_STATUS_FAIL;
  }
  memset(buffer, 'a', data_len);
  tt_start = osKernelGetTickCount();
  while (1) {
    if ((((tt_end - tt_start) / 1000) > ttl_sec) || (flag == max_sock)) {
      break;
    }
    if ((sock_list & (1 << sock_no)) && (!(tx_stats[sock_no][4]))) {
      status = send(sock_no, buffer, data_len, 0);
      if (status) {
        tx_stats[sock_no][0] += status;
        tx_stats[sock_no][1] = tt_end - tt_start;
        tx_stats[sock_no][3]++;
        if (tx_stats[sock_no][3] == (uint32_t)no_of_pkts) {
          tx_stats[sock_no][4] = 1;
          flag++;
        }
      } else {
        tx_stats[sock_no][1] = tt_end - tt_start;
        tx_stats[sock_no][2] = status;
        tx_stats[sock_no][4] = 1;
        flag++;
      }
    }
    sock_no++;
    if (sock_no == max_sock) {
      sock_no = 0;
    }
    tt_end = osKernelGetTickCount();
  }
  memset(buffer, '\0', data_len);
  free(buffer);
  printf("\ntx_stats:");

  for (sock_no = 0; sock_no < max_sock; sock_no++) {
    printf("%ld,%ld,%ld-", tx_stats[sock_no][0], tx_stats[sock_no][1], tx_stats[sock_no][2]);
  }
  printf("\n");
  return SL_STATUS_OK;
}

sl_status_t socket_data_receive_handler(console_args_t *arguments)
{
  uint32_t sock_id           = sl_cli_get_argument_uint32(arguments, 0);
  uint32_t buffer_length     = sl_cli_get_argument_uint32(arguments, 1);
  uint16_t ttl_sec           = sl_cli_get_argument_uint16(arguments, 2);
  uint32_t no_of_pkts        = sl_cli_get_argument_uint32(arguments, 3);
  uint8_t sock_sync          = sl_cli_get_argument_uint8(arguments, 4);
  uint32_t pkt_count         = 0;
  uint32_t total_data_length = 0;
  uint32_t tt_start          = 0;
  uint32_t tt_end            = 0;
  int read_bytes             = 1;
  void *buffer               = malloc(buffer_length);
  if (buffer == NULL) {
    printf("\r\n Unable to allocate the required buffer for the requested data size, %ld bytes", buffer_length);
    return SL_STATUS_ALLOCATION_FAILED;
  }
  memset(buffer, 0, buffer_length);
  if (sock_sync) {
    tt_start = osKernelGetTickCount();
    while (1) {
      read_bytes = recv(sock_id, buffer, buffer_length, 0);
      if (read_bytes < 0) {
        free(buffer);
        return set_error();
      }
      pkt_count++;
      tt_end = osKernelGetTickCount();
      total_data_length += read_bytes;

      if ((pkt_count >= no_of_pkts) || ((tt_end - tt_start) / 1000 > ttl_sec)) {
        break;
      }
    }
    printf("socket rx stats : %ld\n", total_data_length);
  } else {
    ; //will add as per the requirement
  }
  free(buffer);
  return SL_STATUS_OK;
}

sl_status_t switch_host_frequency_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
#ifdef SLI_SI91X_MCU_INTERFACE
  switch_m4_frequency();
  SysTick_Config(SystemCoreClock / (1000 * tick_count_s));
#endif
  return SL_STATUS_OK;
}
