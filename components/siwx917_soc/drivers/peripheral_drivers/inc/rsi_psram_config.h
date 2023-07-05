/*******************************************************************************
 * @file  rsi_psram_config.h
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
#ifndef __RSI_PSRAM_CONFIG_H_
#define __RSI_PSRAM_CONFIG_H_

/*PSRAM Mode of Operation*/
#define SINGLE_MODE 0
#define QUAD_MODE   2

/******************************User Configuration*****************************/
/*PSRAM GPIO Pin Selection, Select one of the below as per HW configuration*/

//#define PSRAM_GPIO_PIN_SET_0_TO_5

#define PSRAM_GPIO_PIN_SET_52_TO_57

//#define PSRAM_GPIO_PIN_SET_46_TO_51_CS_0

//#define PSRAM_GPIO_PIN_SET_46_TO_51_CS_1

//#define PSRAM_GPIO_PIN_SET_46_TO_57_CS_0

//#define PSRAM_GPIO_PIN_SET_46_TO_57_CS_1

/*Select the mode of operation of SPI*/
#define PSRAM_SPI_MODE   QUAD_MODE
#define PSRAM_MODEL_WRAP 0

/*AES Security Configuration*/
#define PSRAM_AES_KEY_SIZE   128
#define PSRAM_AES_USE_KEY_KH 1

#endif //__RSI_PSRAM_CONFIG_H_
