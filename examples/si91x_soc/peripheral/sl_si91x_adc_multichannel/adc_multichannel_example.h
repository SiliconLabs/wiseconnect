/***************************************************************************/ /**
 * @file adc_multichannel_example.h
 * @brief ADC multichannel example
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

#ifndef ADC_MULTICHANNEL_EXAMPLE_H_
#define ADC_MULTICHANNEL_EXAMPLE_H_

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
 * ADC example initialization function
 * ADC clock is configured followed by ADC initialize,
 * set configure, ADC channel configure for four channel and ADC start.
 *
 * @param none
 * @return none
 ******************************************************************************/
void adc_multichannel_example_init(void);

/***************************************************************************/ /**
 * Function will run continuously
 * ADC FIFO mode it will execute for four channel to read the sample from 
 * ping and pong memory and given equivalent output of 12 bit ADC data.
 *
 * @param none
 * @return none
 ******************************************************************************/
void adc_multichannel_example_process_action(void);

#endif /* ADC_MULTICHANNEL_EXAMPLE_H_ */
