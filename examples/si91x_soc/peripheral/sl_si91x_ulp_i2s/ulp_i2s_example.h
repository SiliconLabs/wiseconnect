/***************************************************************************/ /**
 * @file ulp_i2s_lowpower_example.h
 * @brief ULP I2S example functions
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

#ifndef ULP_I2S_EXAMPLE_H_
#define ULP_I2S_EXAMPLE_H_

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
 * I2S example initialization function
 * Clock is configured, followed by power mode, and I2S configuration
 * 
 * @param none
 * @return none
 ******************************************************************************/
void ulp_i2s_example_init(void);

/***************************************************************************/ /**
 * Function will run continuously 
 * State machine flow is set for transfer, send and receive operations.
 * 
 * 
 * @param none
 * @return none
 ******************************************************************************/
void ulp_i2s_example_process_action(void);

/***************************************************************************/ /**
 * Function will switch power state from PS4 to PS2
 *
 *
 * @param none
 * @return none
 ******************************************************************************/
void hardware_setup(void);

#endif /* I2S_LOWPOWER_EXAMPLE_H_ */
