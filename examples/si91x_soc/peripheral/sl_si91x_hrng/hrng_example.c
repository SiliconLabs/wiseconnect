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

#include "sl_si91x_hrng.h"
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
      DEBUGOUT("Failed to initialize HRNG\n");
      break;
    } else {
      DEBUGOUT("Successfully initialized HRNG\n");
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
        DEBUGOUT("Repeated numbers found: %lu at index %ld and %ld\n", random_buff[i], i, j);
        return status;
      }
    }
  }
  status = SL_STATUS_OK;
  return status;
}

/***************************************************************************/ /**
 * @brief HRNG example process action to generate random numbers and verify them.
 *
 * This function starts the HRNG, retrieves random numbers, checks for repetition,
 * and stops the HRNG. The results are printed for debugging purposes.
 ******************************************************************************/
void hrng_example_process_action(void)
{
  sl_status_t status;
  static sl_si91x_hrng_mode_t hrng_mode = SL_SI91X_HRNG_TRUE_RANDOM;
  uint32_t random_bytes[HRNG_TRANSFER_SIZE]; // Random bytes buffer

  do {
    /* Start the HRNG */
    status = sl_si91x_hrng_start(hrng_mode);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Failed to start HRNG\n");
      break;
    } else {
      if (hrng_mode == SL_SI91X_HRNG_TRUE_RANDOM) {
        DEBUGOUT("Successfully started HRNG with TRUE_RANDOM Mode\n");
        hrng_mode = SL_SI91X_HRNG_PSEUDO_RANDOM;
      } else {
        DEBUGOUT("Successfully started HRNG with PSEUDO_RANDOM mode\n");
        hrng_mode = SL_SI91X_HRNG_TRUE_RANDOM;
      }
    }

#ifndef LFSR_MODE_EN
    /* Get random bytes */
    status = sl_si91x_hrng_get_bytes(random_bytes, HRNG_TRANSFER_SIZE);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Failed to Read LFRS Input\n");
      break;
    } else {
      DEBUGOUT("Successfully read LFSR Input\n");
    }

#else
    status = sl_si91x_hrng_read_lfsr_input(HRNG_TRANSFER_SIZE, random_bytes);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Failed to Read LFRS Input\n");
      break;
    } else {
      DEBUGOUT("Successfully read LFSR Input\n");
    }
#endif

    /* Stop the HRNG */
    status = sl_si91x_hrng_stop();
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Failed to Stop HRNG\n");
      break;
    } else {
      DEBUGOUT("Successfully stopped HRNG\n");
    }

    /* Disable the HRNG */
    status = sl_si91x_hrng_deinit();
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Failed to de-initialize HRNG\n");
      break;
    } else {
      DEBUGOUT("Successfully de-initialized HRNG\n");
    }

    /* Print random bytes */
    for (int i = 0; i < HRNG_TRANSFER_SIZE; i++) {
      DEBUGOUT("Random Byte [%d]: %lu\n", i, random_bytes[i]);
    }

    /* Check for repeated random numbers */
    status = checkRandomNumRepeat(random_bytes, HRNG_TRANSFER_SIZE);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("Random numbers are repeated\n");
      break;
    } else {
      DEBUGOUT("Random numbers are not repeated\n");
    }
  } while (false);
}
