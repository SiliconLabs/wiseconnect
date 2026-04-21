/***************************************************************************/ /**
 * # License
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is Third Party Software licensed by Silicon Labs from a third party
 * and is governed by the sections of the MSLA applicable to Third Party
 * Software and the additional terms set forth below.
 *
 ******************************************************************************/
/**************************************************************************/  /**
 * @file unity_conf.c
 * @brief Empty implementations of Unity's setUp and tearDown functions
 * @version 1.0.0
 ******************************************************************************
 * # License
 * <b>(C) Copyright 2011 Energy Micro AS, http://www.energymicro.com</b>
 ******************************************************************************
 *
 * This source code is the property of Energy Micro AS. The source and compiled
 * code may only be used on Energy Micro "EFM32" microcontrollers.
 *
 * This copyright notice may not be removed from the source code nor changed.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
 * obligation to support this Software. Energy Micro AS is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Energy Micro AS will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 *****************************************************************************/

#include "unity.h"

#if defined(ENABLE_TEST_COVERAGE)
extern void __gcov_flush(void);
#endif

/**************************************************************************/ /**
 * @brief Unity helper function - called before each test
 *****************************************************************************/
__weak void setUp(void)
{
}

/**************************************************************************/ /**
 * @brief Unity helper function - called after each test
 *****************************************************************************/
__weak void tearDown(void)
{
}

/**************************************************************************/ /**
 * @brief Sets up Unity and begins test group
 *****************************************************************************/
void sl_unity_start_test(const char *test_file)
{
  UNITY_Init();
  UnityBeginGroup(test_file);
}

/**************************************************************************/ /**
 * @brief Ends Unity group and waits for ever
 *****************************************************************************/
void sl_unity_stop_test(void)
{
  UnityEnd();

#if defined(ENABLE_TEST_COVERAGE)
  __gcov_flush();
#endif

  UnityPrint("ENDSWO");

  /* Do not exit main() when the tests are finished */
  while (1)
    ;
}
