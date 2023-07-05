/*******************************************************************************
* @file  rsi_cts.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

// Include Files

#include "rsi_cts.h"
#include "rsi_board.h"

#if defined(__CC_ARM)
uint32_t Threshold __attribute__((at(0x24063FD8)));
uint32_t sample_count __attribute__((at(0x240637DC)));
uint32_t avg_count __attribute__((at(0x24063FE0)));

uint32_t default_cal __attribute__((at(0x24063FE4)));
uint32_t calib __attribute__((at(0x24063FE8)));
uint32_t Ref_voltage __attribute__((at(0x24063FEC)));
#endif

// IAR support
#if defined(__ICCARM__)
#pragma location = 0x24063FD8
uint32_t Threshold;

#pragma location = 0x240637DC
uint32_t sample_count;

#pragma location = 0x24063FE0
uint32_t avg_count;

#pragma location = 0x24063FE4
uint32_t default_cal;

#pragma location = 0x24063FE8
uint32_t calib;

#pragma location = 0x24063FEC
uint32_t Ref_voltage;
#endif

#if defined(__GNUC__)
uint32_t Threshold;    // __attribute__((at(0x24063FD8)));
uint32_t sample_count; // __attribute__((at(0x240637DC)));
uint32_t avg_count;    // __attribute__((at(0x24063FE0)));

uint32_t default_cal; // __attribute__((at(0x24063FE4)));
uint32_t calib;       // __attribute__((at(0x24063FE8)));
uint32_t Ref_voltage; // __attribute__((at(0x24063FEC)));
#endif                /* defined (__GNUC__) */

// cts pin configuration structure
typedef struct CTS_PINS {
  uint8_t sensor;
  uint8_t pin;
  uint8_t sensor_enable;
} CTS_PIN_Config;

static const CTS_PIN_Config cts_pins[] = {
  { SENSOR1, AGPIO_PIN8, SENSOR1_ENABLE },  { SENSOR2, AGPIO_PIN9, SENSOR2_ENABLE },
  { SENSOR3, AGPIO_PIN10, SENSOR3_ENABLE }, { SENSOR4, AGPIO_PIN7, SENSOR4_ENABLE },
  { SENSOR5, AGPIO_PIN6, SENSOR5_ENABLE },  { SENSOR6, AGPIO_PIN3, SENSOR6_ENABLE },
  { SENSOR7, AGPIO_PIN0, SENSOR7_ENABLE },  { SENSOR8, AGPIO_PIN11, SENSOR8_ENABLE },
};

/** @addtogroup SOC21
* @{
*/

/*==============================================*/
/**   
 * @fn          RSI_CTS_PinMuxing()
 * @brief       This API is used for Configuring the gpio pins to be used in CTS               
 * @return      none             
 */
void RSI_CTS_PinMuxing()
{
  const CTS_PIN_Config *io;
  for (io = cts_pins; io != &cts_pins[sizeof(cts_pins) / sizeof(CTS_PIN_Config)]; io++) {
    if (io->sensor_enable) {
      RSI_EGPIO_SetPinMux(EGPIO1, 0, io->pin, AGPIO_MODE);
      RSI_EGPIO_SetDir(EGPIO1, 0, io->pin, 1);
    }
  }
  RSI_EGPIO_SetPinMux(EGPIO1, 0, AGPIO_PIN4, AGPIO_MODE);
  RSI_EGPIO_SetDir(EGPIO1, 0, AGPIO_PIN4, EGPIO_CONFIG_DIR_OUTPUT);
  RSI_EGPIO_SetPin(EGPIO1, 0, AGPIO_PIN4, EGPIO_CONFIG_DIR_INPUT);

  RSI_EGPIO_SetPinMux(EGPIO1, 0, AGPIO_PIN5, AGPIO_MODE);
  RSI_EGPIO_SetDir(EGPIO1, 0, AGPIO_PIN5, EGPIO_CONFIG_DIR_OUTPUT);
  RSI_EGPIO_SetPin(EGPIO1, 0, AGPIO_PIN5, EGPIO_CONFIG_DIR_INPUT);
}

/*==============================================*/
/**   
 * @fn          void Aux_VddPowerOn()
 * @brief       This API is used for AUX_VDD POWER ON              
 * @return      none             
 */
void Aux_VddPowerOn()
{
  *(volatile uint32_t *)(0x240414A0) |= (BIT(11) | BIT(12) | BIT(13) | BIT(17) | BIT(18));

  *(volatile uint32_t *)(0x24041400) = 0x3007FFE0;
  *(volatile uint32_t *)(0x24041414) = 0x1;

  *(volatile uint32_t *)(0x2404C040) = 0x1D; //GPIO 4 IN MOd7
  *(volatile uint32_t *)(0x24041434) = 0x1;

  *(volatile uint32_t *)(0x24043A10) = 0x7b;

  *(volatile uint32_t *)(0x41300610) = 0x3FFFFF;
}

/*==============================================*/

/**   
 * @fn          void RSI_CTS_ClkSelection(CTS_Type *cts,uint8_t clk_sel_1,uint8_t clk_sel_2,uint8_t clk_divider_1,uint8_t clk_divider_2)
 * @brief       This API is used for clk selection 
 * @param[in]   cts           : Ppointer to cts config structure
 * @param[in]   clk_Sel_1     : mux select value for clk select 1
 * @param[in]   clk_Sel_2     : mux select value for clk select 2              
 * @param[in]   clk_divider_1 : Division factor for clk_1
 * @param[in]   clk_divider_2 : Division factor for clk_2               
 * @return      none              
 */
void RSI_CTS_ClkSelection(CTS_Type *cts,
                          uint8_t clk_sel_1,
                          uint8_t clk_sel_2,
                          uint8_t clk_divider_1,
                          uint8_t clk_divider_2)
{
  cts->CTS_CONFIG_REG_0_0_b.CLK_SEL1     = clk_sel_1;
  cts->CTS_CONFIG_REG_0_0_b.CLK_SEL2     = clk_sel_2;     //select the musx for clk_Sel1 and clk sel2
  cts->CTS_CONFIG_REG_0_0_b.PRE_SCALAR_1 = clk_divider_1; //select the clk divider for both clk
  cts->CTS_CONFIG_REG_0_0_b.PRE_SCALAR_2 = clk_divider_2;
}

/*==============================================*/
/**   
 * @fn          uint32_t RSI_CTS_GetStatus(CTS_Type *cts)
 * @brief       This API is used to get the status of cts 
 * @param[in]   cts           : Pointer to cts config structure
 * @return      status of an cts              
 */
uint32_t RSI_CTS_GetStatus(CTS_Type *cts)
{
  uint32_t cts_status = 0;
  cts_status          = (cts->CTS_CONFIG_REG_0_0 & BIT(28)); //GET THE STATUS
  cts_status |= ((cts->CTS_CONFIG_REG_1_1) & BIT(14));
  return (cts_status);
}

/*==============================================*/
/**   
 * @fn          void RSI_CTS_ConfigPolynomial(CTS_Type *cts,boolean_t enable,uint8_t poly_length,uint32_t seed,uint32_t polynomial)
 * @brief       This API is used config polynomial parameters
 * @param[in]   cts        : Pointer to cts config structure
 * @param[in]   enable     : 1 -enable seed value feed
 *                           0 -disable seed value feed
 * @param[in]   poly_length: polynomial length 
 * @param[in]    seed       : seed value to be loaded
 * @param[in]   polynomial  : polynomial for PRS 
 * @return      none 
 *              
 */
void RSI_CTS_ConfigPolynomial(CTS_Type *cts, boolean_t enable, uint8_t poly_length, uint32_t seed, uint32_t polynomial)
{
  cts->CTS_CONFIG_REG_1_1_b.SEED_LOAD      = enable; //load the seed value
  cts->CTS_CONFIG_REG_1_1_b.POLYNOMIAL_LEN = poly_length;
  cts->CTS_CONFIG_REG_1_3_b.PRS_SEED       = seed;       //LOAD SEED VALUE
  cts->CTS_CONFIG_REG_1_4_b.PRS_POLY       = polynomial; //load the polynomial for PRS
}

/*==============================================*/
/**   
 * @fn          void RSI_CTS_ConfigOnOffTime(CTS_Type *cts,uint16_t on_time ,uint16_t off_time)
 * @brief       This API is used set on and off time
 * @param[in]   cts        : Pointer to cts config structure
 * @param[in]   on_time    : required on time 
 * @param[in]   off_time   : required off time 
 * @return      none 
 *              
 */
void RSI_CTS_ConfigOnOffTime(CTS_Type *cts, uint16_t on_time, uint16_t off_time)
{
  cts->CTS_CONFIG_REG_1_2_b.PWM_ON_PERIOD  = on_time; //sets on and off time for sensor
  cts->CTS_CONFIG_REG_1_2_b.PWM_OFF_PERIOD = off_time;
}

/*==============================================*/
/**   
 * @fn          void RSI_CTS_ConfigSampling(CTS_Type *cts,uint16_t delay,uint16_t repetitions)
 * @brief       This API is used for configue the sampling delay
 * @param[in]   cts        : Pointer to cts config structure
 * @param[in]   delay      : inter sensor delay time
 * @param[in]   repetitions: no of times scan 
 * @return      none              
 */
void RSI_CTS_ConfigSampling(CTS_Type *cts, uint16_t delay, uint16_t repetitions)
{
  cts->CTS_CONFIG_REG_1_5_b.INTER_SENSOR_DELAY = delay;       //sets the inter senser dealy
  cts->CTS_CONFIG_REG_1_5_b.N_SAMPLE_COUNT     = repetitions; //sets the no time scan the sensor
}

/*==============================================*/
/**   
 * @fn          void RSI_CTS_ConfigSamplingPattern(CTS_Type *cts,uint32_t pattern,uint32_t valid_sensor)
 * @brief       This API is used for configue the scanning pattern and valid sensor
 * @param[in]   cts         : Pointer to cts config structure
 * @param[in]   pattern     : Sensor pattern to scan(0x12345678)
 * @param[in]   valid_sensor: no of valid sensors(1 to 8)
 * @return      none              
 */
void RSI_CTS_ConfigSamplingPattern(CTS_Type *cts, uint32_t pattern, uint32_t valid_sensor)
{
  cts->CTS_CONFIG_REG_1_6_b.SENSOR_CFG    = pattern;      //set the pattern to sense sensor
  cts->CTS_CONFIG_REG_1_7_b.VALID_SENSORS = valid_sensor; //sets the valid sensors to sense
}

/*==============================================*/
/**   
 * @fn          void RSI_CTS_RefVoltageConfig(CTS_Type *cts,uint16_t ref_voltage,boolean_t enable)
 * @brief       This API is used to configure reference voltage
 * @param[in]   cts     : Pointer to cts config structure
 * @param[in]   enable  : Enable reference voltage select  
 * @param[in]   ref_voltage : reference voltage to be set
 *                  0  -0.5V
 *                  1  -0.6V
 *                  2  -0.7V
 *                  3  -0.8V
 *                  4  -0.9V
 *                  5  -1.0V
 *                  6  -1.1V
 *                  7  -1.2V
 * @param[in]   *samples     : pointer to array to store samples 
 * @param[in]   *avg_samples : pointer to array to store avg_samples
 * @return      none              
 */
void RSI_CTS_ConfigRefVoltage(CTS_Type *cts,
                              int16_t ref_voltage,
                              boolean_t enable,
                              uint16_t *samples,
                              uint16_t *avg_samples)
{
  uint32_t ref_vlg_change;
  if (((default_cal == 3) || (default_cal == 4)) && (sample_count == 0)) {
    ref_vlg_change = Ref_voltage;
    if ((Threshold / (float)AVG_MULTIPLIER) < ((float)LOWER_LIMIT * ON_TIME)) {
      if (ref_voltage > MIN_REF) {
        Threshold    = 0;
        avg_count    = 0;
        sample_count = 0;

        if (default_cal != 4) {
          default_cal = 2;
        }
        Ref_voltage -= 1;
      }
    } else if (((Threshold / (float)AVG_MULTIPLIER) > ((float)UPPER_LIMIT * ON_TIME))
               && (Threshold < ON_TIME * AVG_MULTIPLIER)) {
      if (ref_voltage < MAX_REF) {
        Threshold    = 0;
        avg_count    = 0;
        sample_count = 0;
        if (default_cal != 4) {
          default_cal = 2;
        }

        Ref_voltage += 1;
      }
    }

    if ((Threshold != 0) && (default_cal == 3)) {
      if (Threshold < WAKEUP_THRESHOLD) {
        calib = 0;
      }
      avg_count    = 0;
      sample_count = 0;
      default_cal  = 2; //to move to slow calibration
      memset(avg_samples, 0, (NO_OF_SLOW_SAMPLES * 2));
      memset(samples, 0, (NO_OF_SAMPLES * 2));
    } else if (default_cal == 4 && ref_vlg_change != Ref_voltage) {
      //default_cal=4 switches to slow calibration
      calib        = 1;
      avg_count    = 0;
      sample_count = 0; //ref_voltage changes switches to fast calibration
      default_cal  = 2;
      memset(avg_samples, 0, (NO_OF_SLOW_SAMPLES * 2));
      memset(samples, 0, (NO_OF_SAMPLES * 2));
    }
  }
#ifdef CHIP_917
  cts->CTS_CONFIG_REG_1_7_b.REF_VOLT_CONFIG |= (Ref_voltage << 6);
  if (enable)
    cts->CTS_CONFIG_REG_1_7_b.REF_VOLT_CONFIG |= (BIT(0));
  else
    cts->CTS_CONFIG_REG_1_7_b.REF_VOLT_CONFIG &= (~BIT(0));
#else
  cts->CTS_CONFIG_REG_1_7_b.REF_VOLT_CONFIG = Ref_voltage; //set the reference voltage
  cts->CTS_CONFIG_REG_1_7_b.VREF_SEL        = enable;      //enables ref vlg select
#endif
}

/*==============================================*/
/**   
 * @fn          void RSI_CTS_ConfigWakeUp(CTS_Type *cts,uint8_t mode,uint16_t threshold)
 * @brief       This API is used to configure the wakeup mode
 * @param[in]   cts      :pointer to cts config structure
 * @param[in]   mode     : 1 -wakeup if count is greater than threshold  
 *                         0 -wakeup if count is less than threshold 
 * @param[in]   threshold : wakeup threshold
 * @return      none 
 *              
 */
void RSI_CTS_ConfigWakeUp(CTS_Type *cts, uint8_t mode, uint16_t threshold)
{
  cts->CTS_CONFIG_REG_1_7_b.WAKEUP_MODE = mode; //configure the wakeup mode

  if (threshold) {
    cts->CTS_CONFIG_REG_1_7_b.WAKE_UP_THRESHOLD = threshold; //write threshold value
  } else {
    cts->CTS_CONFIG_REG_1_7_b.WAKE_UP_THRESHOLD = WAKEUP_THRESHOLD;
    Threshold                                   = WAKEUP_THRESHOLD;
  }
}

/*==============================================*/
/**   
 * @fn          uint32_t RSI_CTS_ReadRandomData(CTS_Type *cts)
 * @brief       This API is used read the random data
 * @param[in]   cts      :pointer to cts config structure                  
 * @return      none 
 *              
 */
uint32_t RSI_CTS_ReadRandomData(CTS_Type *cts)
{
  uint32_t random_data;
  random_data = cts->CTS_CONFIG_REG_1_8_b.PRS_STATE; //read the random data
  return random_data;
}

/*==============================================*/
/**   
 * @fn          uint32_t RSI_CTS_ReadFifo(CTS_Type *cts)
 * @brief       This API is used read the data from fifo
 * @param[in]   cts      :pointer to cts config structure                      
 * @return      none 
 *              
 */
uint32_t RSI_CTS_ReadFifo(CTS_Type *cts)
{
  uint32_t read_data;
  read_data = cts->CTS_FIFO_ADDRESS_b.FIFO;
  return read_data;
}

/*==============================================*/
/**   
 * @fn          void RSI_CTS_IntrClear(CTS_Type *cts)
 * @brief       This API is used clear the interrupt for cts
 * @param[in]   cts      :Pointer to cts config structure                     
 * @return      none               
 */
void RSI_CTS_IntrClear(CTS_Type *cts)
{
  CTS->CTS_CONFIG_REG_1_1_b.WAKE_UP_ACK = 1;
  CTS->CTS_CONFIG_REG_1_1_b.WAKE_UP_ACK = 0;
}

/*==============================================*/
/**   
 * @fn          void RSI_CTS_InputSelection(CTS_Type *cts)
 * @brief       This API is used to selection of an capacitance and resistance input
 * @param[in]   cts       : pointer to cts config structure
 * @param[in]   cap_input : 1 to  select Int cap input(AGPIO8) for cap sensor
 * @param[in]   res_input : 1 to select resistance input select(AGPIO10) for cap sensor                 
 * @return      none 
 * @NOTE:Default inpus on AGPIO4 and AGPIO5 to select AGPIO8 and AGPIO10 pass 0             
 */
void RSI_CTS_InputSelection(CTS_Type *cts, uint32_t cap_input, uint32_t res_input)
{
  if (cap_input) {
    ULP_SPI_MEM_MAP(0x141) &= ~BIT(13);
  } else if (res_input) {
    ULP_SPI_MEM_MAP(0x141) &= ~BIT(14);
  }
}

/*==============================================*/
/**   
 * @fn          void RSI_CTS_Calibration(uint16_t avg)
 * @brief       This API is used to do CTS fast calibration
 * @param[in]   avg         : average of the samples
 * @param[in]   samples     : pointer to array to store samples
 * @param[in]   avg_samples : pointer to array to store avg_samples 
 * @return      none 
 *              
 */
void RSI_CTS_Calibration(uint16_t avg, uint16_t *samples, uint16_t *avg_samples)
{
  uint8_t i        = 0;
  uint32_t average = 0;

  if ((sample_count == 0) && (avg_count == 0) && (default_cal == 1)) {
    Threshold             = WAKEUP_THRESHOLD;
    samples[sample_count] = avg;

    if (default_cal == 1) {
      sample_count = 0;
    } else {
      sample_count += 1;
    }
    default_cal = 2;
  } else {
    if ((sample_count < NO_OF_SAMPLES) && (avg_count < NO_OF_AVG_SAMPLES)) {
      samples[sample_count] = avg;
      sample_count += 1;
    } else {
      average = 0;
      if ((avg_count < NO_OF_AVG_SAMPLES) && (sample_count == NO_OF_SAMPLES)) {
        for (i = 0; i < NO_OF_SAMPLES; i++) {
          average += samples[i];
        }
        avg_samples[avg_count] = average / NO_OF_SAMPLES;

        avg_count += 1;
        average = 0;

        if ((default_cal == 2) && (avg_count == NO_OF_AVG_SAMPLES)) //wait upto all the average sample location filled
        {
          default_cal = 3;
        }

        if (default_cal == 3 && avg_count <= NO_OF_AVG_SAMPLES) //when default_cal=3 update threshold value
        {
          for (i = 0; i < NO_OF_AVG_SAMPLES; i++) {
            average += avg_samples[i];
          }
          Threshold = (AVG_MULTIPLIER * (float)(average / NO_OF_AVG_SAMPLES));
        }

        sample_count = 0;

        if (avg_count == (NO_OF_AVG_SAMPLES)) {
          avg_count = 0;
        }
      }
    }
  }
}

/*==============================================*/
/**   
 * @fn          void RSI_CTS_SlowCalibration(uint16_t avg)
 * @brief       This API is used to do CTS calibration
 * @param[in]   avg         : average of the samples
 * @param[in]   samples    : pointer to array to store samples
 * @param[in]   avg_samples: pointer to array to store avg_sampless                     
 * @return      none             
 */
void RSI_CTS_SlowCalibration(uint16_t avg, uint16_t *samples, uint16_t *avg_samples)
{
  uint8_t i        = 0;
  uint32_t average = 0;

  if (default_cal == 2) //fill all the avg_sample locations with fast clibration sample locations
  {
    if (sample_count < NO_OF_SAMPLES && avg_count < NO_OF_SLOW_AVG_SAMPLES) {
      samples[sample_count] = avg;
      sample_count += 1;
    } else {
      average = 0;
      if ((avg_count < NO_OF_SLOW_AVG_SAMPLES) && (sample_count == NO_OF_SAMPLES)) {
        for (i = 0; i < NO_OF_SAMPLES; i++) {
          average += samples[i];
        }
        avg_samples[avg_count] = average / NO_OF_SAMPLES;

        avg_count += 1;
        average = 0;

        if ((default_cal == 2) && (avg_count == NO_OF_SLOW_AVG_SAMPLES)) {
          default_cal = 3;
        }

        if ((default_cal == 3) && (avg_count <= NO_OF_SLOW_AVG_SAMPLES)) {
          for (i = 0; i < NO_OF_SLOW_AVG_SAMPLES; i++) {
            average += avg_samples[i];
          }

          Threshold   = (AVG_MULTIPLIER * (float)(average / NO_OF_SLOW_AVG_SAMPLES));
          default_cal = 4;
        }

        sample_count = 0;

        if (avg_count == (NO_OF_SLOW_AVG_SAMPLES)) {
          avg_count = 0;
        }
      }
    }
  } else if (default_cal == 4) {
    if ((sample_count < NO_OF_SLOW_SAMPLES) && (avg_count < NO_OF_SLOW_AVG_SAMPLES)) {
      samples[sample_count] = avg;
      sample_count += 1;
    } else {
      average = 0;
      if ((avg_count < NO_OF_SLOW_AVG_SAMPLES) && (sample_count == NO_OF_SLOW_SAMPLES)) {
        for (i = 0; i < NO_OF_SLOW_SAMPLES; i++) {
          average += samples[i];
        }
        avg_samples[avg_count] = (average / NO_OF_SLOW_SAMPLES);
        avg_count += 1;
        average = 0;

        if (default_cal == 4 && avg_count <= NO_OF_SLOW_AVG_SAMPLES) {
          for (i = 0; i < NO_OF_SLOW_AVG_SAMPLES; i++) {
            average += avg_samples[i];
          }
          Threshold = (AVG_MULTIPLIER * (float)(average / NO_OF_SLOW_AVG_SAMPLES));
        }

        sample_count = 0;

        if (avg_count == (NO_OF_SLOW_AVG_SAMPLES)) {
          avg_count = 0;
        }
      }
    }
  }
}

/*==============================================*/
/**   
 * @fn          void RSI_CTS_TouchDetection(uint16_t avg)
 * @brief       This API is used to check which sensor is pressed
 * @param[in]   fifo_read    : Pointer to Sensor count value
 * @param[in]   sensor_count : Pointer to store pressed sensor no                    
 * @return      none            
 */
void RSI_CTS_TouchDetection(uint16_t *fifo_read, uint8_t *sensor_count)
{
  uint16_t value = 0, count_value = 0;
  uint8_t i = 0;

  for (i = 0; i < NO_OF_SENSORS; i++) {
    value       = (*(fifo_read + i) >> 12) & 0xf;
    count_value = (*(fifo_read + i)) & 0x0fff;

    switch (value) {
      case 1:
        if (count_value > Threshold) {
          sensor_count[0]++;
        }
        break;
      case 2:
        if (count_value > Threshold) {
          sensor_count[1]++;
        }
        break;
      case 3:
        if (count_value > Threshold) {
          sensor_count[2]++;
        }
        break;
      case 4:
        if (count_value > Threshold) {
          sensor_count[3]++;
        }
        break;
      case 5:
        if (count_value > Threshold) {
          sensor_count[4]++;
        }
        break;
      case 6:
        if (count_value > Threshold) {
          sensor_count[5]++;
        }
        break;

      case 7:
        if (count_value > Threshold) {
          sensor_count[6]++;
        }
        break;
      case 8:
        if (count_value > Threshold) {
          sensor_count[7]++;
        }
        break;

      default:
        break;
    }
  }
}
/** @} */