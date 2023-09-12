/***************************************************************************/ /**
 * @file i2c_leader_example.h
 * @brief I2C Leader examples functions
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
#ifndef I2C_LEADER_EXAMPLE_H_
#define I2C_LEADER_EXAMPLE_H_

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
 * It is an initialization function, it initializes the clock, pin configuration
 * init_para for I2C communication.
 * 
 * @param none
 * @return none
 ******************************************************************************/
void i2c_leader_example_init(void);

/***************************************************************************/ /**
 * The state machine code for send and receive is implemented here.
 * This function is called in while loop.
 * 
 * @param none
 * @return none
 ******************************************************************************/
void i2c_leader_example_process_action(void);

extern float sensor_data;

#endif /* I2C_LEADER_EXAMPLE_H_ */
