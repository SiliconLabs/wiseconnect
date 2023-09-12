/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

/** @file
 *
 * File Description
 *
 */
#include "sl_ethernet_constants.h"
#include "sl_ethernet.h"
#include "sl_net.h"
//#include "sl_thread.h"
//#include "sl_memory.h"
#include "sl_board_configuration.h"
//#include "sl_network_interface.h"

#include <stdio.h>
#include <string.h>

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

static void ethernet_driver_thread_main(uint32_t arg);

/******************************************************
 *               Variable Definitions
 ******************************************************/

//const console_descriptive_command_t ethernet_init_command    = { "", "", ethernet_init_command_handler, { COMMAND_OPTIONAL_ARG('t', COMMAND_ARG_UINT8), COMMAND_OPTIONAL_ARG('r', COMMAND_ARG_UINT8), COMMAND_ARG_END } };

static const sl_ethernet_config_t ethernet_config = { .mac_address       = { { 0xD6, 0x15, 0x2C, 0x98, 0x1E, 0x29 } },
                                                      .phy_bus_mode      = SL_ETHERNET_PHY_BUS_MODE_RMII,
                                                      .duplex_mode       = SL_ETHERNET_DUPLEX_MODE_FULL,
                                                      .speed             = SL_ETHERNET_SPEED_AUTO,
                                                      .phy_address       = SL_ETHERNET_PHY_AUTO_ADDRESS,
                                                      .pin_configuration = &ethernet_pin_configuration };
//sl_thread_t ethernet_driver_thread;

/******************************************************
 *               Function Definitions
 ******************************************************/

sl_status_t ethernet_init_command_handler(console_args_t *arguments)
{
  sl_status_t status;
  void *stack = NULL;
  //    uint8_t tx_buffer_count = IS_COMMAND_ARG_VALID(0) ? arguments->arg[0] : DEFAULT_ETHERNET_TX_BUFFER_COUNT;
  //    uint8_t rx_buffer_count = IS_COMMAND_ARG_VALID(1) ? arguments->arg[1] : DEFAULT_ETHERNET_RX_BUFFER_COUNT;

  //    sli_net_ethernet_init();

  return SL_STATUS_OK;

exit:
  //    SL_CLEANUP_MALLOC(tx_buffers);
  //    SL_CLEANUP_MALLOC(rx_buffers);
  //    SL_CLEANUP_MALLOC(stack);
  return status;
}
