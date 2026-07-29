/***************************************************************************/ /**
 * @file ulp_ssi_freertos.h
 * @brief ULP SSI master FreeRTOS example functions
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
#ifndef ULP_SSI_FREERTOS_H_
#define ULP_SSI_FREERTOS_H_

/** Full-duplex DMA transfer (loopback compare when RX enabled in same run). */
#define ULP_SSI_MASTER_TRANSFER ENABLE
/** Half-duplex: TX only (no `compare` unless receive phase also runs). */
#define ULP_SSI_MASTER_SEND DISABLE
/** Half-duplex: RX only (expects prior slave stimulus; uses `compare`). */
#define ULP_SSI_MASTER_RECEIVE DISABLE

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
 * @brief Spawn the ULP SSI master FreeRTOS worker (`ulp_ssi_freertos.c`).
 *
 * @details Default thread attributes: stack 8192 bytes, `osPriorityLow1`.
 ******************************************************************************/
void ulp_ssi_master_example_init(void);

#ifdef __cplusplus
}
#endif

#endif /* ULP_SSI_FREERTOS_H_ */
