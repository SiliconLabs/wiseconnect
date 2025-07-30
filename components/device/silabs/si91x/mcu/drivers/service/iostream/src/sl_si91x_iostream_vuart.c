/***************************************************************************/ /**
 * @file
 * @brief SI91x IO Stream VUART Component.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
// Define module name for Power Manager debug feature
#define CURRENT_MODULE_NAME "IOSTREAM_VUART_SI91X"

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#include "sli_si91x_iostream_swo_itm_8.h"
#include "sl_si91x_iostream_vuart_config.h"
#include "sl_si91x_iostream_vuart.h"
#include "sl_iostream.h"
#include "sl_si91x_iostream_swo.h"
#include "sl_status.h"
#include "sl_assert.h"
#include "SEGGER_RTT.h"
#include "sl_rtt_buffer_index.h"
#include "sl_si91x_debug_swo.h"

/*******************************************************************************
 ******************************   VARIABLES   **********************************
 ******************************************************************************/

// Public stream variable for VUART io component
static sl_iostream_t sl_si91x_iostream_vuart;

sl_iostream_t *sl_si91x_iostream_vuart_handle = &sl_si91x_iostream_vuart;

sl_iostream_instance_info_t sl_si91x_iostream_instance_vuart_info = {
  .handle    = &sl_si91x_iostream_vuart,
  .name      = "vuart",
  .type      = SL_SI91X_IOSTREAM_TYPE_VUART,
  .periph_id = 0,
  .init      = sl_si91x_iostream_vuart_init,
};

// RTT down channel ring buffer
static char rtt_buffer[SL_SI91X_IOSTREAM_VUART_RX_BUFFER_SIZE];

// Packet TX sequence number
static uint8_t seq_number = 0;

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static sl_status_t vuart_write(void *context, const void *buffer, size_t buffer_length);

static sl_status_t vuart_read(void *context, void *buffer, size_t buffer_length, size_t *bytes_read);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************/ /**
 * Initialize vuart stream component.
 ******************************************************************************/
sl_status_t sl_si91x_iostream_vuart_init(void)
{
  sl_status_t status = SL_STATUS_OK;

  status = sl_si91x_debug_swo_init();
  if (status != SL_STATUS_OK) {
    return status;
  }

  void *context;
  // Configure SWO stimulus 8
  context                         = sli_si91x_iostream_swo_itm_8_init();
  sl_si91x_iostream_vuart.context = context;

  // Configure RTT
  // The "silabsvuartdown" RTT channel name is used by the debugger to find the
  // location of the RTT buffer in RAM, and can therefore not be changed without
  // updating the board controller firmware.

  SEGGER_RTT_ConfigDownBuffer(SL_IOSTREAM_VUART_RTT_BUFFER_INDEX,
                              "silabsvuartdown",
                              rtt_buffer,
                              SL_SI91X_IOSTREAM_VUART_RX_BUFFER_SIZE,
                              SEGGER_RTT_MODE_NO_BLOCK_TRIM);

  sl_si91x_iostream_vuart.write = vuart_write;
  sl_si91x_iostream_vuart.read  = vuart_read;

  // Set default IO Stream to VUART
  status = sl_iostream_set_system_default(&sl_si91x_iostream_vuart);
  return status;
}

/*******************************************************************************
 *********************   LOCAL FUNCTIONS   ***************************
 ******************************************************************************/

/***************************************************************************/ /**
 * Internal stream write implementation
 ******************************************************************************/
static sl_status_t vuart_write(void *context, const void *buffer, size_t buffer_length)
{
  sl_status_t status =
    sli_si91x_iostream_swo_itm_8_write(context, buffer, buffer_length, SI91X_DEBUG_VIRTUAL_UART_TX, seq_number);
  if (status == SL_STATUS_OK) {
    seq_number++; // Wrap arround/overflow on the usigned int is expected to reset the sequence number
  }

  return status;
}

/***************************************************************************/ /**
 * Internal stream read implementation
 ******************************************************************************/
static sl_status_t vuart_read(void *context, void *buffer, size_t buffer_length, size_t *bytes_read)
{
  (void)context;

  *bytes_read = SEGGER_RTT_Read(SL_IOSTREAM_VUART_RTT_BUFFER_INDEX, buffer, buffer_length);
  if (*bytes_read == 0) {
    return SL_STATUS_EMPTY;
  }

  return SL_STATUS_OK;
}
