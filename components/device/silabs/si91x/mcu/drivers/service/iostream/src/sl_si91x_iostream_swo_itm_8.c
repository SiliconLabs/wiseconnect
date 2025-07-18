/***************************************************************************/ /**
 * @file
 * @brief SI91x IO Stream SWO ITM Functions
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
#include <stdbool.h>
#include "sli_si91x_iostream_swo_itm_8.h"
#include "sl_iostream.h"
#include "sl_status.h"
#include "sl_assert.h"
#include "sl_si91x_debug_swo.h"

/*******************************************************************************
 ******************************   DEFINES   **********************************
 ******************************************************************************/

#define LOW_BYTE(n)  ((uint8_t)((n)&0xFF))
#define HIGH_BYTE(n) ((uint8_t)(LOW_BYTE((n) >> 8)))

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************/ /**
 * Initialize swo stimulus 8
 ******************************************************************************/
void *sli_si91x_iostream_swo_itm_8_init(void)
{
  // Configure SWO module
  sl_si91x_debug_swo_enable_itm(8U);
  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 * write data on vuart interface (WSTK-port 4900)
 ******************************************************************************/
sl_status_t sli_si91x_iostream_swo_itm_8_write(void *context,
                                               const void *buffer,
                                               size_t buffer_length,
                                               sl_si91x_iostream_swo_itm_8_msg_type_t type,
                                               uint8_t seq_nbr)
{
  uint8_t *buf           = (uint8_t *)buffer;
  uint32_t packet_length = 0;
  uint32_t i             = 0;
  uint8_t output_byte    = 0;
  sl_status_t status     = SL_STATUS_OK;
  (void)context;

  // Full length is 2 square braces, 1 byte length and 2 byte crc
  packet_length = ((uint32_t)buffer_length) + 9;

  for (i = 0; i < packet_length; ++i) {
    if (i == 0) {
      // Frame start
      output_byte = '[';
    } else if (i == 1) {
      // Including special byte, type and sequence number
      output_byte = buffer_length + 4;
    } else if (i == 2) {
      // Special EDM byte
      output_byte = 0xD1;
    } else if (i == 3) {
      // VUART TX Type byte 1
      output_byte = LOW_BYTE((uint16_t)type);
    } else if (i == 4) {
      // VUART TX Type byte 2
      output_byte = HIGH_BYTE((uint16_t)type);
    } else if (i == 5) {
      // Sequence number
      output_byte = seq_nbr;
    } else if (i == packet_length - 3) {
      // CRC first byte
      // Ignored by FW - so we also skip it
      output_byte = 0x5A;
    } else if (i == packet_length - 2) {
      // CRC second byte
      // Ignored by FW - so we also skip it
      output_byte = 0x5A;
    } else if (i == packet_length - 1) {
      // Frame end
      output_byte = ']';
    } else {
      // Data
      output_byte = buf[i - 6];
    }

    status = sl_si91x_debug_swo_write_u8(8U, output_byte);
  }
  return status;
}
