/***************************************************************************/ /**
 * @file
 * @brief Flash Read Write Example Application
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_net.h"
#include "sl_utility.h"
#include "cmsis_os2.h"
#include "sl_wifi_callback_framework.h"
#include "sl_si91x_driver.h"
/******************************************************
 *               Variable Definitions
 ******************************************************/

#define M4_FLASH_ADDRESS  0x8300000
#define NWP_FLASH_ADDRESS 0

#define LENGTH_OF_BUFFER          (1024 * 8)
#define FLASH_SECTOR_ERASE_ENABLE 0

unsigned char data_read_write_buffer[LENGTH_OF_BUFFER];

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

/******************************************************
 *               Function Declarations
 ******************************************************/

static void application_start(void *argument);

/******************************************************
 *               Function Definitions
 ******************************************************/

void app_init(const void *unused)
{
  UNUSED_PARAMETER(unused);
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

static void application_start(void *argument)
{
  UNUSED_PARAMETER(argument);
  sl_status_t status;

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, NULL, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to start Wi-Fi client interface: 0x%lx\r\n", status);
    return;
  }

  printf("\r\nApplication started.\r\n");

  // Fill data_write_buffer with test data
  memset(data_read_write_buffer, 0x01, LENGTH_OF_BUFFER);

  // M4 flash memory
  status = (int)sl_si91x_command_to_write_common_flash(M4_FLASH_ADDRESS,
                                                       data_read_write_buffer,
                                                       LENGTH_OF_BUFFER,
                                                       FLASH_SECTOR_ERASE_ENABLE);
  if (status == SL_STATUS_OK) {
    printf("\r\nData successfully written to M4 flash memory.\r\n");
  } else {
    printf("\r\nError writing data to M4 flash memory.\r\n");
  }

  memset(data_read_write_buffer, 0x00, LENGTH_OF_BUFFER);

  unsigned char *result = memcpy((uint8_t *)data_read_write_buffer, (uint8_t *)M4_FLASH_ADDRESS, LENGTH_OF_BUFFER);

  // Check if memcpy was successful
  if (result == data_read_write_buffer) {
    printf("\r\nData successfully read from M4 flash memory.\r\n");
  } else {
    printf("\r\nError reading data from M4 flash memory.\r\n");
  }

  memset(data_read_write_buffer, 0x01, LENGTH_OF_BUFFER);

  // NWP flash memory
  status = (int)sl_si91x_command_to_write_common_flash(NWP_FLASH_ADDRESS,
                                                       data_read_write_buffer,
                                                       LENGTH_OF_BUFFER,
                                                       FLASH_SECTOR_ERASE_ENABLE);
  if (status == SL_STATUS_OK) {
    printf("\r\nData successfully written to NWP flash memory.\r\n");
  } else {
    printf("\r\nError writing data to NWP flash memory.\r\n");
  }

  memset(data_read_write_buffer, 0x00, LENGTH_OF_BUFFER);

  // Read from NWP flash memory
  status = (int)sl_si91x_command_to_read_common_flash(NWP_FLASH_ADDRESS, LENGTH_OF_BUFFER, data_read_write_buffer);

  if (status == SL_STATUS_OK) {
    printf("\r\nData successfully read from NWP flash memory.\r\n");
  } else {
    printf("\r\nError reading data from NWP flash memory.\r\n");
  }

  while (1) {
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
    // Let the CPU go to sleep if the system allows it.
    sl_power_manager_sleep();
#else
    osDelay(osWaitForever);
#endif
  }
}
