/*******************************************************************************
 * @file  sl_si91x_sdio_slave_drv_config.h
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#ifndef __SL_SI91X_SDIO_SLAVE_DRV_CONFIG_H_
#define __SL_SI91X_SDIO_SLAVE_DRV_CONFIG_H_

#include "sl_si91x_sdio_slave.h"

// <o TX_TRANSFER_BLOCK_SIZE> SDIO Slave to Master transfer block size
// <i> Default: 256
#define TX_TRANSFER_BLOCK_SIZE 256

#endif //__SL_SI91X_SDIO_SLAVE_DRV_CONFIG_H_