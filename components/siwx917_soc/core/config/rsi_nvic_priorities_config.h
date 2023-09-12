/*
 * nvic_priorities_config.h
 *
 *  Created on: Aug 1, 2023
 *      Author: surondla
 */

#ifndef NVIC_PRIORITIES_CONFIG_H_
#define NVIC_PRIORITIES_CONFIG_H_

#include "FreeRTOSConfig.h"

#define DEFAULT_PRIORITY configMAX_SYSCALL_INTERRUPT_PRIORITY >> (8 - configPRIO_BITS)

void sl_si91x_device_init_nvic();

#endif /* NVIC_PRIORITIES_CONFIG_H_ */
