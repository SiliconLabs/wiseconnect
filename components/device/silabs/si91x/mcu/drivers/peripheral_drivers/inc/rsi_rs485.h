/******************************************************************************
* @file  rsi_rs485.h
*******************************************************************************
* # License
* <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "rsi_ccp_common.h"
/**
 * @brief Structure to hold the rs485 multidrop configuration for USART RS485.
 *
 * This structure defines the configuration parameters for the USART RS485 multidrop including the data bit 9, address match,
 * send address, and transmit mode.
 */
typedef struct {
  uint8_t addr_match;    // Enable Address match with RAR register during receive
  uint8_t send_addr;     // Used to determine when to send the address during transmit mode
  uint8_t transmit_mode; // This bit is used to set transmit mode during 9-bit data transfers
} usart_rs485_multidrop_t;

/**
 * @brief Structure to set TurnAround Timing.
 *
 * This structure hold the DE and RE enable Turn around time 
 */
typedef struct {
  uint16_t de_re_tat_set; // Driver Enable to Receiver Enable TurnAround time
  uint16_t re_de_tat_set; // Receiver Enable to Driver Enable TurnAround time.
} usart_rs485_tat_t;

/**
 * @brief Structure to hold the usart rs485 configs.
 */
typedef struct {
  uint8_t
    transfer_mode; // Set the transfer mode 0 =  Full Duplex Mode, 1 = Software-Enabled Half Duplex Mode, 2 = Hardware enabled Half Duplex mode.
  uint8_t de_assert_time;            // Driver enable assertion time
  uint8_t de_deassert_time;          // Driver enable de-assertion time
  bool de_pol;                       // Driver Enable Polarity
  bool re_pol;                       // Receiver Enable Polarity
  bool multidrop_enable;             // Multidrop enable to confiure the multidrop parameterts
  usart_rs485_tat_t tat_set;         // Structure to set TurnAround Timing
  usart_rs485_multidrop_t multidrop; // Structure to multidrop parameters
} usart_rs485_config_t;

/*****************************************************************************************************************/ /**
 * This function initializes the RS485 required gpio's and enables RS485 communicationn.
 *
 * @param[in]usart         Pointer to the USART/UART peripheral register block.
 *
 * @return                 None.
********************************************************************************************************************/
void rsi_usart_rs485_enable(USART0_Type *usart);

/*****************************************************************************************************************/ /**
 * This function configures rs485 communication.
 *
 * @param[in]usart         Pointer to the USART/UART peripheral register block.
 * @param[in]usart_rs485   RS485 config parameters 
 *
 * @return                 None
********************************************************************************************************************/
void rsi_usart_rs485_configure(USART0_Type *usart, usart_rs485_config_t usart_rs485);

/*****************************************************************************************************************/ /**
 * This function Enable/Disable the Driver Enable.
 *
 * @param[in]usart         Pointer to the USART/UART peripheral register block.
 * @param[in]enable        Driver Enable signal enable or Disable 
 *
 * @return                 None.
********************************************************************************************************************/
void rsi_usart_rs485_de_enable(USART0_Type *usart, bool enable);

/*****************************************************************************************************************/ /**
 * This function Enable/Disable the Receiver output.
 *
 * @param[in]usart         Pointer to the USART/UART peripheral register block.
 * @param[in]enable        Receiver output enable or Disable
 *
 * @return                 RSI_OK if success
********************************************************************************************************************/
void rsi_usart_rs485_re_enable(USART0_Type *usart, bool enable);

/*****************************************************************************************************************/ /**
 * This function set address to the slave on which master can send data.
 *
 * @param[in]usart         Pointer to the USART/UART peripheral register block.
 * @param[in]rx_addr       address to be set
 *
 * @return                 None
********************************************************************************************************************/
void rsi_usart_rs485_rx_address_set(USART0_Type *usart, uint8_t rx_addr);

/*****************************************************************************************************************/ /**
 * This function check the address received or not..
 *
 * @param[in]usart         Pointer to the USART/UART peripheral register block.
 *
 * @return                 None
********************************************************************************************************************/
void rsi_usart_rs485_transfer_address(USART0_Type *usart, usart_rs485_multidrop_t usart_rs485, uint8_t tx_addr);

/*****************************************************************************************************************/ /**
 * This function used to halt or resume the transfer.
 *
 * @details This api is used when application is required to fill the data bytes in the TxFIFO before sending the address on 
 * the uart line.Then this api need to call with enable bit set to 1 so that uart does not start sending out the data in the 
 * TxFIFO as data byte, Once the TxFIFO is filled, then call the rsi_rs485_transfer_address() api  and call this api with 
 * enable bit set as 0.
 * @param[in]usart         Pointer to the USART/UART peripheral register block.
 * @param[in]enable       resume or halt the data transmission
 *
 * @return                 None
********************************************************************************************************************/
void rsi_usart_rs485_resume_tx(USART0_Type *usart, bool enable);

/*****************************************************************************************************************/ /**
 * This function deinitialize the rs485.
 *
 * @param[in]usart        Pointer to the USART/UART peripheral register block.
 *
 * @return                 None
********************************************************************************************************************/
void rsi_usart_rs485_disable(USART0_Type *usart);
