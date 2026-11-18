/***************************************************************************/ /**
 * @file hrng_example.c
 * @brief HRNG example
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <inttypes.h>
#include "sl_si91x_hrng.h"
#include "sl_si91x_clock_manager.h"
#include "rsi_debug.h"
#include "hrng_example.h"

/*******************************************************************************
  ***************************  Defines / Macros  ********************************
  ******************************************************************************/

#define HRNG_TRANSFER_SIZE 10 // HRNG transfer size
/*******************************************************************************
  **********************  Local variables   ***************************
  ******************************************************************************/

/*******************************************************************************
  **********************  Local Function prototypes   ***************************
  ******************************************************************************/

static sl_status_t hrng_read_and_process(sl_si91x_hrng_mode_t mode, const char *mode_name);

/*******************************************************************************
  **************************   GLOBAL FUNCTIONS   *******************************
  ******************************************************************************/

/***************************************************************************/ /**
  * @brief Initializes the HRNG (Hardware Random Number Generator) example.
  *
  * @details This function sets up the default system clock and power configurations
  * and enables the HRNG module. If the HRNG module is successfully enabled, a success
  * message is printed. If the enablement fails, an error message is printed.
   ******************************************************************************/
void hrng_example_init(void)
{
  sl_status_t status = SL_STATUS_OK;

  do {
    /* Enable the HRNG module */
    status = sl_si91x_hrng_init();
    if (status != SL_STATUS_OK) {
      /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
      SL_PRINT_STRING_ERROR("Failed to initialize HRNG\r\n");
      break;
    } else {
      SL_PRINT_STRING_ERROR("Successfully initialized HRNG\r\n");
    }
  } while (false);
}

/***************************************************************************/ /**
  * @brief This function checks for repeated random numbers in the provided buffer.
  *
  * @param[in] random_buff  Pointer to the buffer containing random numbers.
  * @param[in] len        The length of the random number buffer.
  *
  * @return SL_STATUS_OK if no repetition found, SL_STATUS_ABORT if repetition is found.
  ******************************************************************************/
static sl_status_t checkRandomNumRepeat(uint32_t *random_buff, uint32_t len)
{
  sl_status_t status = SL_STATUS_ABORT;
  uint32_t i = 0, j = 0;

  for (i = 0; i < len - 1; i++) {
    for (j = i + 1; j < len; j++) {
      if (random_buff[i] == random_buff[j]) {
        SL_PRINT_STRING_ERROR("Repeated numbers found: %lu at index %ld and %ld\r\n", random_buff[i], i, j);
        return status;
      }
    }
  }
  status = SL_STATUS_OK;
  return status;
}

/***************************************************************************/
/**
  * @brief Helper function to start HRNG, read data, and process results.
  *
  * @param[in] mode HRNG mode (TRUE_RANDOM or PSEUDO_RANDOM)
  * @param[in] mode_name String description of the mode
  *
  * @return SL_STATUS_OK on success, error code on failure
  ***************************************************************************/
static sl_status_t hrng_read_and_process(sl_si91x_hrng_mode_t mode, const char *mode_name)
{
  sl_status_t status;
  uint32_t random_bytes[HRNG_TRANSFER_SIZE];

  status = sl_si91x_hrng_start(mode);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Failed to start HRNG\r\n");
    return status;
  } else {
    SL_PRINT_STRING_ERROR("Successfully started HRNG with %s mode\r\n", (uintptr_t)mode_name);
  }

#ifndef LFSR_MODE_EN
  status = sl_si91x_hrng_get_bytes(random_bytes, HRNG_TRANSFER_SIZE);
#else
  status = sl_si91x_hrng_read_lfsr_input(HRNG_TRANSFER_SIZE, random_bytes);
#endif

  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Failed to read HRNG data\r\n");
    return status;
  } else {
    SL_PRINT_STRING_ERROR("Successfully read HRNG data\r\n");
  }

  status = sl_si91x_hrng_stop();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Failed to stop HRNG\r\n");
    return status;
  } else {
    SL_PRINT_STRING_ERROR("Successfully stopped HRNG\r\n");
  }

  for (int i = 0; i < HRNG_TRANSFER_SIZE; i++) {
    SL_PRINT_STRING_ERROR("Random Byte [%d]: %lu\r\n", i, random_bytes[i]);
  }

  status = checkRandomNumRepeat(random_bytes, HRNG_TRANSFER_SIZE);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Random numbers are repeated\r\n");
  } else {
    SL_PRINT_STRING_ERROR("Random numbers are not repeated\r\n");
  }

  return status;
}

/***************************************************************************/ /**
  * @brief HRNG example process action to generate random numbers and verify them.
  *
  * This function demonstrates HRNG operation in three states: SOFT_RESET enabled,
  * PSEUDO_RANDOM mode, and TRUE_RANDOM mode. Uses a state machine to execute
  * one phase per function call.
  ******************************************************************************/
void hrng_example_process_action(void)
{
  sl_status_t status = SL_STATUS_OK;

  /* Soft reset enabled state */
  sl_si91x_hrng_soft_reset_set();

  hrng_read_and_process(SL_SI91X_HRNG_PSEUDO_RANDOM, "PSEUDO_RANDOM (soft reset enabled)");

  /* PSEUDO_RANDOM mode */
  sl_si91x_hrng_soft_reset_clear();

  hrng_read_and_process(SL_SI91X_HRNG_PSEUDO_RANDOM, "PSEUDO_RANDOM");

  /* TRUE_RANDOM mode */

  hrng_read_and_process(SL_SI91X_HRNG_TRUE_RANDOM, "TRUE_RANDOM");

  /* Disable the HRNG */
  status = sl_si91x_hrng_deinit();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Failed to de-initialize HRNG\r\n");
  } else {
    SL_PRINT_STRING_ERROR("Successfully de-initialized HRNG\r\n");
  }
  while (1)
    ;
}
