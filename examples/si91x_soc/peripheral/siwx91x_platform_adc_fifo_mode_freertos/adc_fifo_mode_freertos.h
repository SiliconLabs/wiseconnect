/***************************************************************************/ /**
 * @file adc_fifo_mode_freertos.h
 * @brief ADC FreeRTOS FIFO-mode example: sample channels in a task, wait for
 *        completion per buffer via semaphore, read and print voltage.
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

#ifndef ADC_FIFO_MODE_FREERTOS_H_
#define ADC_FIFO_MODE_FREERTOS_H_

/***************************************************************************/ /**
 * ADC FIFO-mode example init (FreeRTOS). Creates the ADC FIFO task; the task
 * runs ADC init then continuously samples, reads data and prints voltages.
 *
 * @param none
 * @return none
 ******************************************************************************/
void adc_fifo_mode_example_init(void);

#endif /* ADC_FIFO_MODE_FREERTOS_H_ */
