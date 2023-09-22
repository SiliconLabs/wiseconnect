/*******************************************************************************
 * @file
 * @brief Calibration Application
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_iostream.h"
#include "sl_iostream_init_instances.h"
#include "sl_iostream_handles.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "sl_board_configuration.h"
#include "cmsis_os2.h"
#include "sl_wifi.h"
#include "sl_net.h"
#include "sl_utility.h"
#include <string.h>
#include "sl_si91x_driver.h"

/******************************************************
 *                      Macros
 ******************************************************/
/******************************************************
 *                    Constants
 ******************************************************/
#define BURST_MODE      0
#define CONTINUOUS_MODE 1
#define TX_TEST_POWER   18
#define TX_TEST_RATE    0
#define TX_TEST_LENGTH  30
#define TX_TEST_MODE    BURST_MODE
#define TEST_CHANNEL    1

#define MAX_CALIB_COMMAND_LENGTH 200
#define MIN_CALIB_COMMAND_LENGTH (strlen("rsi_evm_write=") + 2 /*atleast one char of input data and CR/LF */)
#define NO_OF_CALIB_COMMANDS     4

#define PARSE_1_BYTE  1
#define PARSE_2_BYTES 2
#define PARSE_4_BYTES 4

typedef struct calib_commands_t {
  uint8_t cmd[MAX_CALIB_COMMAND_LENGTH];
} calib_commands_t;

enum calib_cmd_types { FREQ_OFFSET, CALIB_WRITE, EVM_OFFSET, EVM_WRITE };

calib_commands_t calib_commands[NO_OF_CALIB_COMMANDS] = { { "sl_freq_offset=" },
                                                          { "sl_calib_write=" },
                                                          { "sl_evm_offset=" },
                                                          { "sl_evm_write=" } };

/******************************************************
 *               Global Variable
 ******************************************************/

const osThreadAttr_t thread_attributes = {
  .name       = "app",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 3072,
  .priority   = osPriorityLow,
  .tz_module  = 0,
  .reserved   = 0,
};

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#ifndef BUFSIZE
#define BUFFER_SIZE 80 //Input buffer size
#endif

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

/* Input buffer */
static char buffer[BUFFER_SIZE];
uint32_t status   = 0;
bool end_of_cmd   = false;
bool cmd_valid    = true;
uint8_t cmd_index = -1;

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************/ /**
 * Initialize example.
 ******************************************************************************/

static const sl_wifi_device_configuration_t sl_wifi_calibration_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode              = SL_SI91X_TRANSMIT_TEST_MODE,
                   .coex_mode              = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map        = (SL_SI91X_FEAT_SECURITY_PSK | SL_SI91X_FEAT_AGGREGATION),
                   .tcp_ip_feature_bit_map = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT),
                   .custom_feature_bit_map = (SL_SI91X_FEAT_CUSTOM_FEAT_EXTENTION_VALID),
                   .ext_custom_feature_bit_map =
                     (SL_SI91X_EXT_FEAT_XTAL_CLK | SL_SI91X_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS
#ifdef CHIP_917B0
                      | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
#ifndef RSI_M4_INTERFACE
                      | RAM_LEVEL_NWP_ALL_MCU_ZERO
#else
                      | RAM_LEVEL_NWP_ADV_MCU_BASIC
#endif
                      ),
                   .bt_feature_bit_map         = 0,
                   .ext_tcp_ip_feature_bit_map = SL_SI91X_CONFIG_FEAT_EXTENTION_VALID,
                   .ble_feature_bit_map        = 0,
                   .ble_ext_feature_bit_map    = 0,
                   .config_feature_bit_map     = 0 }
};

/******************************************************
 *               Function Declarations
 ******************************************************/
static void application_start(void *argument);
sl_status_t calibration_app(void);
#ifdef RSI_M4_INTERFACE
void iostream_usart_init(void);
void iostream_rx(void);
#endif
void display_calib_cmd_usage(void);
void validate_input_cmd(void);

/******************************************************
 *               Function Definitions
 ******************************************************/

void app_init(const void *unused)
{
  UNUSED_PARAMETER(unused);
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

#ifdef RSI_M4_INTERFACE
void iostream_usart_init()
{
  /* Prevent buffering of output/input.*/
#if !defined(__CROSSWORKS_ARM) && defined(__GNUC__)
  setvbuf(stdout, NULL, _IONBF, 0); /*Set unbuffered mode for stdout (newlib)*/
  setvbuf(stdin, NULL, _IONBF, 0);  /*Set unbuffered mode for stdin (newlib)*/
#endif
}
#endif

static void application_start(void *argument)
{
  UNUSED_PARAMETER(argument);
  sl_status_t status;

#ifdef RSI_M4_INTERFACE
  iostream_usart_init();
#endif

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &sl_wifi_calibration_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("Failed to start Wi-Fi client interface: 0x%lx\r\n", status);
    return;
  } else {
    printf("Wi-Fi initialization successful\r\n");
  }

  status = sl_si91x_transmit_test_start(TX_TEST_POWER, TX_TEST_RATE, TX_TEST_LENGTH, TX_TEST_MODE, TEST_CHANNEL);
  if (status != SL_STATUS_OK) {
    printf("Transmit test start failed: 0x%lx\r\n", status);
  } else {
    printf("Transmit test started\r\n");
  }

  status = calibration_app();

  if (status != SL_STATUS_OK) {
    printf("Calibration test failed with status 0x%lx\r\n", status);
    return;
  }
}

#ifdef RSI_M4_INTERFACE
void iostream_rx()
{
  char c               = 0;
  static uint8_t index = 0;
  sl_iostream_getchar(SL_IOSTREAM_STDIN, &c);
  if (c > 0) {
    if ((c == '\n')) {
      if (buffer[index - 1] == '\r') {
        buffer[index - 1] = '\0';
        index             = 0;
        end_of_cmd        = true;
      }
    } else {
      if (index < BUFFER_SIZE - 1) {
        buffer[index] = (char)c;
        index++;
      }
    }
  }
}
#endif

void display_calib_cmd_usage()
{
  printf("Calibration commands usage:\r\n");
  printf("*************************************************************************************************************"
         "**************************************************************\r\n");
  printf("sl_freq_offset=<freq_offset_in_KHz>\r\n");
  printf("sl_calib_write=<target>,<flags>,<gain_offset_low>,<gain_offset_mid>,<gain_offset_high>\r\n");
  printf("OR\r\n");
  printf("sl_calib_write=<target>,<flags>,<gain_offset_low>,<gain_offset_mid>,<gain_offset_high>,<xo_ctune>\r\n");
  printf("sl_evm_offset=<index>,<evm_offset>\r\n");
  printf("sl_evm_write=<target>,<flags>,<evm_offset_11B>,<evm_offset_11G_36M_54M_11N_MCS3_MCS7>,<evm_offset_11G_6M_24M_"
         "11N_MCS0_MCS2>,<evm_offset_11N_MCS0>,<evm_offset_11N_MCS7>\r\n");
  printf("*************************************************************************************************************"
         "**************************************************************\r\n");
}

void validate_input_cmd()
{
  if ((strncasecmp((const char *)buffer, (const char *)"sl_", strlen("sl_") != 0))
      || (strlen(buffer) < MIN_CALIB_COMMAND_LENGTH) || (strlen(buffer) > MAX_CALIB_COMMAND_LENGTH)) {
    cmd_valid = false;
  }
}

int32_t validate_and_set_cmd_index()
{
  uint8_t i       = 0;
  int32_t cmd_len = 0;

  validate_input_cmd();

  if (!cmd_valid)
    return -1;

  for (i = 0; i < NO_OF_CALIB_COMMANDS; i++) {
    cmd_len = strlen((const unsigned char *)calib_commands[i].cmd);
    if (!strncasecmp((const char *)&buffer, (char *)calib_commands[i].cmd, cmd_len)) {
      cmd_index = i;
      cmd_valid = true;
      return cmd_len;
    }
  }

  cmd_valid = false;
  return -1;
}

int16_t parse_cmd(void *address, uint16_t length, uint8_t *value)
{
  uint8_t temp_buff[15];
  uint16_t temp_offset = 0;
  uint16_t offset;

  for (offset = 0; offset < length; offset++) {
    uint16_t index = 0;
    /* Copy the bytes until '.' ',' '\0'  is reached */
    while ((value[temp_offset] != '.') && (value[temp_offset] != ',') && (value[temp_offset] != '\0')) {
      if (index >= (sizeof(temp_buff) - 1))
        break;
      temp_buff[index++] = value[temp_offset++];
    }
    temp_buff[index] = '\0';
    temp_offset++;
    if (length == PARSE_2_BYTES) {
      *((uint16_t *)address) = atoi((int8_t *)temp_buff);
      break;
    } else if (length == PARSE_4_BYTES) {
      *((uint32_t *)address) = atoi((int8_t *)temp_buff);
      break;
    } else
      *((uint8_t *)address + offset) = atoi((int8_t *)temp_buff);
  }

  return temp_offset;
}

sl_status_t calibration_app()
{
  sl_status_t status;
  int32_t cmd_len                            = 0;
  int8_t temp1B                              = 0;
  int16_t temp2B                             = 0;
  int32_t temp4B                             = 0;
  int32_t offset                             = 0;
  sl_si91x_calibration_read_t target         = { 0 };
  sl_si91x_calibration_read_t calib_read_pkt = { 0 };
  sl_si91x_calibration_write_t calib_pkt     = { 0 };
  sl_si91x_freq_offset_t freq_calib_pkt      = { 0 };
  sl_si91x_evm_offset_t evm_offset_pkt       = { 0 };
  sl_si91x_evm_write_t evm_write_pkt         = { 0 };
  sl_si91x_efuse_read_t efuse_read_pkt       = { 0 };
  uint8_t *efuse_read_buf                    = NULL;

  target.target      = 1;
  calib_pkt.target   = 1;
  calib_pkt.flags    = 6;
  calib_pkt.xo_ctune = 80;

  freq_calib_pkt.frequency_offset_in_khz = 30;

  evm_offset_pkt.evm_offset_val = 12;
  evm_offset_pkt.evm_index      = 1;

  evm_write_pkt.target                              = 1;
  evm_write_pkt.flags                               = 2;
  evm_write_pkt.evm_offset_11G_6M_24M_11N_MCS0_MCS2 = 10;

  efuse_read_pkt.efuse_read_addr_offset = 602;
  efuse_read_pkt.efuse_read_data_len    = 20;

  while (1) {
    display_calib_cmd_usage();

    printf("Enter the calibration command:\r\n");

#ifdef RSI_M4_INTERFACE
    while (!end_of_cmd) {
      iostream_rx();
    }
#endif

    end_of_cmd = false;

    printf("Command read complete\r\n");

    cmd_len = validate_and_set_cmd_index();
    offset  = cmd_len;

    if (cmd_valid) {
      switch (cmd_index) {
        case FREQ_OFFSET:
          offset += parse_cmd(&temp4B, PARSE_4_BYTES, &buffer[offset]);
          freq_calib_pkt.frequency_offset_in_khz = temp4B;
          status                                 = sl_si91x_frequency_offset(&freq_calib_pkt);
          if (status != SL_STATUS_OK) {
            printf("Frequency offset correction failed: 0x%lx\r\n", status);
            //return status;
          } else {
            printf("Frequency offset correction successful\r\n");
          }
          offset = 0;
          temp4B = 0;
          break;
        case CALIB_WRITE:
          if (buffer[offset] != '\0') {
            offset += parse_cmd(&temp1B, PARSE_1_BYTE, &buffer[offset]);
            calib_pkt.target = temp1B;
          }
          if (buffer[offset] != '\0') {
            offset += parse_cmd(&temp4B, PARSE_4_BYTES, &buffer[offset]);
            calib_pkt.flags = temp4B;
          }
          if (buffer[offset] != '\0') {
            offset += parse_cmd(&temp1B, PARSE_1_BYTE, &buffer[offset]);
            calib_pkt.gain_offset[0] = temp1B;
          }
          if (buffer[offset] != '\0') {
            offset += parse_cmd(&temp1B, PARSE_1_BYTE, &buffer[offset]);
            calib_pkt.gain_offset[1] = temp1B;
          }
          if (buffer[offset] != '\0') {
            offset += parse_cmd(&temp1B, PARSE_1_BYTE, &buffer[offset]);
            calib_pkt.gain_offset[2] = temp1B;
          }
          if (buffer[offset] != '\0') {
            offset += parse_cmd(&temp1B, PARSE_1_BYTE, &buffer[offset]);
            calib_pkt.xo_ctune = temp1B;
          }
          status = sl_si91x_calibration_write(calib_pkt);
          if (status != SL_STATUS_OK) {
            printf("Calibration data write failed: 0x%lx\r\n", status);
            return status;
          } else {
            printf("Calibration data write successful\r\n");
          }
          status = sl_si91x_calibration_read(target, &calib_read_pkt);
          if (status != SL_STATUS_OK) {
            printf("Calibration data read failed: 0x%lx\r\n", status);
            return status;
          } else {
            printf("Calibration data read successful\r\n");
            printf("target %d, gain_offset_low:%d, gain_offset_2:%d, gain_offset_3:%d,xo_tune:%d\r\n",
                   calib_read_pkt.target,
                   calib_read_pkt.gain_offset[0],
                   calib_read_pkt.gain_offset[1],
                   calib_read_pkt.gain_offset[2],
                   calib_read_pkt.xo_ctune);
          }
          offset = 0;
          temp1B = 0;
          temp4B = 0;
          break;
        case EVM_OFFSET:
          if (buffer[offset] != '\0') {
            offset += parse_cmd(&temp1B, PARSE_1_BYTE, &buffer[offset]);
            evm_offset_pkt.evm_index = temp1B;
          }
          if (buffer[offset] != '\0') {
            offset += parse_cmd(&temp1B, PARSE_1_BYTE, &buffer[offset]);
            evm_offset_pkt.evm_offset_val = temp1B;
          }
          status = sl_si91x_evm_offset(&evm_offset_pkt);
          if (status != SL_STATUS_OK) {
            printf("EVM offset correction failed: 0x%lx\r\n", status);
            return status;
          } else {
            printf("EVM offset correction successful\r\n");
          }
          offset = 0;
          temp1B = 0;
          break;
        case EVM_WRITE:
          if (buffer[offset] != '\0') {
            offset += parse_cmd(&temp1B, PARSE_1_BYTE, &buffer[offset]);
            evm_write_pkt.target = temp1B;
          }
          if (buffer[offset] != '\0') {
            offset += parse_cmd(&temp4B, PARSE_4_BYTES, &buffer[offset]);
            evm_write_pkt.flags = temp4B;
          }
          if (buffer[offset] != '\0') {
            offset += parse_cmd(&temp1B, PARSE_1_BYTE, &buffer[offset]);
            evm_write_pkt.evm_offset_11B = temp1B;
          }
          if (buffer[offset] != '\0') {
            offset += parse_cmd(&temp1B, PARSE_1_BYTE, &buffer[offset]);
            evm_write_pkt.evm_offset_11G_36M_54M_11N_MCS3_MCS7 = temp1B;
          }
          if (buffer[offset] != '\0') {
            offset += parse_cmd(&temp1B, PARSE_1_BYTE, &buffer[offset]);
            evm_write_pkt.evm_offset_11G_6M_24M_11N_MCS0_MCS2 = temp1B;
          }
          if (buffer[offset] != '\0') {
            offset += parse_cmd(&temp1B, PARSE_1_BYTE, &buffer[offset]);
            evm_write_pkt.evm_offset_11N_MCS0 = temp1B;
          }
          if (buffer[offset] != '\0') {
            offset += parse_cmd(&temp1B, PARSE_1_BYTE, &buffer[offset]);
            evm_write_pkt.evm_offset_11N_MCS7 = temp1B;
          }
          status = sl_si91x_evm_write(&evm_write_pkt);
          if (status != SL_STATUS_OK) {
            printf("EVM offset correction failed: 0x%lx\r\n", status);
            return status;
          } else {
            printf("EVM offset correction successful\r\n");
          }
          break;
        default:
          printf("Invalid command\r\n");
          break;
      }
    } else {
      printf("Invalid command\r\n");
    }

    memset(buffer, 0, sizeof(buffer));
  }
}
