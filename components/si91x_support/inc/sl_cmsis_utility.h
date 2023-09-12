/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#include "cmsis_os2.h"
#include "sl_status.h"

/******************************************************************************
 * @brief
 *   A utility function that converts osStatus_t to sl_status_t
 * @param[in] status
 *   status of type osStatus_t that needs to be converted to equivalent sl_status_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 *****************************************************************************/
static inline sl_status_t convert_cmsis_os_status(osStatus_t status) {
	switch (status) {
		case osOK :
			return SL_STATUS_OK;
		case osErrorTimeout:
			return SL_STATUS_TIMEOUT;
		case osErrorParameter:
			return SL_STATUS_INVALID_PARAMETER;
		case osErrorISR:
			return SL_STATUS_ISR;
		case osErrorResource:
		case osErrorNoMemory:
			return SL_STATUS_NO_MORE_RESOURCE;
		default:
			return SL_STATUS_FAIL;
	}
}

