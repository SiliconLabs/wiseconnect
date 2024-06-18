/***************************************************************************/ /**
 * @file sl_si91x_ro_temperature_sensor.c
 * @brief RO temperature sensor API implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_si91x_ro_temperature_sensor.h"
#include "rsi_ipmu.h"

/*******************************************************************************
 ***************************  DEFINES / MACROS   *******************************
 ******************************************************************************/
#define SIGN_BIT_POSITION 10      // Temperature read in signed format bit
#define NOMINAL_TEMP      25      // Nominal temperature value
#define ARRAY_SIZE        31      // Array size
#define MSB_BIT_POSITION  31      // MSB bit for sign check
#define MAX_NOMINAL_VALUE 128     // Maximum nominal value
#define KNOWN_TEMP        255     // Known temperature
#define MAX_COUNT_VALUE   1024    // Maximum count value
#define NEG_VALUE         1.3437f // value multiplied for conversion to negative
/*******************************************************************************
 ***************************  Global  VARIABLES ********************************
 ******************************************************************************/

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/
static int16_t temp_decimal_value;

/*******************************************************************************
 **************************   Global function Definitions   ********************
 ******************************************************************************/

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/
static void temp_conversion(uint32_t temp);

/******************************************************************************
 * Sets the count of reference clock on which ptat clock counts.
 *****************************************************************************/
sl_status_t sl_si91x_ro_temperature_set_count(uint32_t count)
{
  sl_status_t status;
  // Returns invalid parameter if count of reference count is greater than MAX_COUNT_VALUE
  if (count > MAX_COUNT_VALUE) {
    status = SL_STATUS_INVALID_PARAMETER;
  } else {
    // set the count of reference clock on which ptat clock counts
    RSI_TS_SetCntFreez(MCU_TEMP, count);
    status = SL_STATUS_OK;
  }
  return status;
}

/******************************************************************************
 * Selects the reference clock (use RO kHz clock (or) MCU FSM clock) to the temperature sensor.
 *****************************************************************************/
sl_status_t sl_si91x_ro_temperature_select_reference_clock(sl_ro_reference_clock_t ref_clk)
{
  sl_status_t status;
  // Returns invalid parameter if reference clock selection is not proper
  if (ref_clk >= SL_RO_CLK_LAST) {
    status = SL_STATUS_INVALID_PARAMETER;
  } else {
    // Selects the reference clock based on sl_ro_reference_clock_t selection
    RSI_TS_RefClkSel(MCU_TEMP, ref_clk);
    status = SL_STATUS_OK;
  }
  return status;
}

/******************************************************************************
 * Used to enable/disable the temperature sensor. It is a self clearing register.
 *****************************************************************************/
sl_status_t sl_si91x_ro_temperature_enable(sl_ro_temperature_state_t state)
{
  sl_status_t status;
  // Returns invalid parameter if other than enabling (or) disabling is choosen
  if (state >= SL_RO_TEMPERATURE_LAST) {
    status = SL_STATUS_INVALID_PARAMETER;
  } else {
    // Enable/Disable the sensor based on sl_ro_temperature_t selection
    RSI_TS_Enable(MCU_TEMP, state);
    status = SL_STATUS_OK;
  }
  return status;
}

/******************************************************************************
 * Used to set the calibrated temperature value of the sensor. By default it is set to 25.
 *****************************************************************************/
sl_status_t sl_si91x_ro_temperature_nominal(uint32_t value)
{
  sl_status_t status;
  // Returns invalid parameter if calibrated temperature value > MAX_NOMINAL_VALUE
  if (value > MAX_NOMINAL_VALUE) {
    status = SL_STATUS_INVALID_PARAMETER;
  } else {
    // Calibrates temperature value of sensor
    RSI_TS_Config(MCU_TEMP, value);
    status = SL_STATUS_OK;
  }
  return status;
}

/******************************************************************************
 * Reads the temperature value in signed format.
 *****************************************************************************/
sl_status_t sl_si91x_ro_temperature_read(int32_t *temperature)
{
  sl_status_t status;
  uint32_t temp_read, f2_count;
  int32_t temp;
  // Validates the null pointer, if true returns error code
  if (temperature == NULL) {
    status = SL_STATUS_NULL_POINTER;
  } else {
    // Reads the temperature from sensor
    temp = (int32_t)RSI_TS_ReadTemp(MCU_TEMP);
    if (temp & BIT(SIGN_BIT_POSITION)) {
      // read the f2 count
      f2_count = RSI_TS_GetPtatClkCnt(MCU_TEMP);
      // subtract f2_nominal count from f2_count NOTE:configure the f2_nominal
      // temp_read=(f2-f2_nominal)
      temp_read = (f2_count - RSI_IPMU_RO_TsEfuse());
      if (temp_read & BIT(MSB_BIT_POSITION)) {
        temp_conversion(temp_read);
        // calculate the negative temperature
        *temperature = (int32_t)(NOMINAL_TEMP - (temp_decimal_value * NEG_VALUE));
      }
    } else {
      *temperature = temp;
    }
    status = SL_STATUS_OK;
  }
  return status;
}

/******************************************************************************
 * Enable/Disable bjt based temperature updation.
 *****************************************************************************/
sl_status_t sl_si91x_ro_temperature_based_update(sl_ro_temperature_update_t enable)
{
  sl_status_t status;
  // Returns invalid parameter if temperature update based on RO is >= SL_RO_TEMP_BJT_UPDATE_LAST
  if (enable >= SL_RO_TEMP_BJT_UPDATE_LAST) {
    status = SL_STATUS_INVALID_PARAMETER;
  } else {
    // Enables the temperature update based on RO calculation
    RSI_TS_RoBjtEnable(MCU_TEMP, enable);
    status = SL_STATUS_OK;
  }
  return status;
}

/******************************************************************************
 * Updation of RO temperature through SPI.
 *****************************************************************************/
sl_status_t sl_si91x_ro_temperature_load(uint8_t temperature)
{
  sl_status_t status;
  // Returns invalid parameter if known temperature >= KNOWN_TEMP
  if (temperature >= KNOWN_TEMP) {
    status = SL_STATUS_INVALID_PARAMETER;
  } else {
    // Read the temperature value for spi based
    RSI_TS_LoadBjt(MCU_TEMP, temperature);
    status = SL_STATUS_OK;
  }
  return status;
}

/******************************************************************************
 * Reads the count of f1 clock cycles.
 *****************************************************************************/
sl_status_t sl_si91x_ro_temperature_get_reference_clk_count(uint32_t *ref_count)
{
  sl_status_t status;
  // Validates the null pointer, if true returns error code
  if (ref_count == NULL) {
    status = SL_STATUS_NULL_POINTER;
  } else {
    // Reads the f1 clock cycles count
    *ref_count = RSI_TS_GetRefClkCnt(MCU_TEMP);
    status     = SL_STATUS_OK;
  }
  return status;
}

/******************************************************************************
 * Reads the count of f2 clock cycles.
 *****************************************************************************/
sl_status_t sl_si91x_ro_temperature_get_ptat_clk_count(uint32_t *ptat_count)
{
  sl_status_t status;
  // Validates the null pointer, if true returns error code
  if (ptat_count == NULL) {
    status = SL_STATUS_NULL_POINTER;
  } else {
    // Reads the f2 clock cycles count
    *ptat_count = RSI_TS_GetPtatClkCnt(MCU_TEMP);
    status      = SL_STATUS_OK;
  }
  return status;
}

/******************************************************************************
 * Updates the temperature periodically based on trigger time selected
 *****************************************************************************/
sl_status_t sl_si91x_ro_temperature_periodic_update(sl_ro_temperature_periodic_check_t periodic_check,
                                                    sl_ro_temperature_trigger_time_t trigger_time)
{
  sl_status_t status;
  // Validates periodic and trigger times and if true returns error code
  if ((periodic_check >= SL_RO_TEMPERATURE_PERIODIC_LAST) || (trigger_time >= SL_RO_TEMPERATURE_TRIGGER_LAST)) {
    status = SL_STATUS_INVALID_PARAMETER;
  } else {
    // Updates the temperature periodically
    RSI_Periodic_TempUpdate(TIME_PERIOD, periodic_check, trigger_time);
    status = SL_STATUS_OK;
  }
  return status;
}

/******************************************************************************
 * Two's complement conversion function
 *****************************************************************************/
static void temp_conversion(uint32_t temp)
{
  int16_t base = 1;
  char binary[ARRAY_SIZE], ones_comp[ARRAY_SIZE], twos_comp[ARRAY_SIZE];
  uint8_t i = 0, rem, carry = 1;
  uint8_t num        = 0;
  temp_decimal_value = 0, base = 1;
  while (temp > 0) {
    binary[i] = temp % 2;
    temp      = temp / 2;
    i++;
  }
  for (i = 0; i < MSB_BIT_POSITION; i++) {
    if (binary[i] == 0)
      ones_comp[i] = 1;
    else
      ones_comp[i] = 0;
  }
  for (i = 0; i < MSB_BIT_POSITION; i++) {
    if (ones_comp[i] == 1 && (carry == 1)) {
      twos_comp[i] = 0;
    } else if ((ones_comp[i] == 0) && (carry == 1)) {
      twos_comp[i] = 1;
      carry        = 0;
    } else {
      twos_comp[i] = ones_comp[i];
    }
  }
  for (i = 0; i < MSB_BIT_POSITION; i++) {
    num                = twos_comp[i];
    rem                = num % 10;
    temp_decimal_value = (int16_t)(temp_decimal_value + rem * base);
    base               = base * 2;
  }
}
