/***************************************************************************/ /**
 * @file sl_si91x_usart.h
 * @brief USART API implementation
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

#ifndef SL_SI91X_USART_H
#define SL_SI91X_USART_H

#ifdef __cplusplus
extern "C" {
#endif

// Includes
#include "sl_status.h"
#include "USART.h"
#include "rsi_usart.h"

/***************************************************************************/ /**
* @addtogroup USART USART
* @ingroup SI91X_PERIPHERAL_APIS
* @{
*
******************************************************************************/

/*******************************************************************************
***************************  DEFINES / MACROS   ********************************
******************************************************************************/
typedef ARM_USART_SignalEvent_t sl_usart_signal_event_t;
typedef ARM_USART_STATUS sl_usart_status_t;
typedef ARM_POWER_STATE sl_usart_power_state_t;
typedef ARM_USART_MODEM_CONTROL sl_usart_modem_control_t;
typedef ARM_USART_MODEM_STATUS sl_usart_modem_status_t;
typedef ARM_USART_CAPABILITIES sl_usart_capabilities_t;
typedef ARM_DRIVER_USART sl_usart_driver_t;
typedef USART_RESOURCES usart_resources_t;
typedef const void *sl_usart_handle_t;

/*******************************************************************************
********************************   ENUMS   ************************************
******************************************************************************/
/// @brief USART/UART Events
typedef enum {
  SL_USART_EVENT_SEND_COMPLETE     = ARM_USART_EVENT_SEND_COMPLETE,     ///< Send complete event
  SL_USART_EVENT_RECEIVE_COMPLETE  = ARM_USART_EVENT_RECEIVE_COMPLETE,  ///< Receive complete event
  SL_USART_EVENT_TRANSFER_COMPLETE = ARM_USART_EVENT_TRANSFER_COMPLETE, ///< Transfer complete event
  SL_USART_EVENT_TX_COMPLETE       = ARM_USART_EVENT_TX_COMPLETE,       ///< Tx complete event event
  SL_USART_EVENT_TX_UNDERFLOW      = ARM_USART_EVENT_TX_UNDERFLOW,      ///< TX underflow event
  SL_USART_EVENT_RX_OVERFLOW       = ARM_USART_EVENT_RX_OVERFLOW,       ///< RX overflow event
  SL_USART_EVENT_RX_TIMEOUT        = ARM_USART_EVENT_RX_TIMEOUT,        ///< RX timeout event
  SL_USART_EVENT_RX_BREAK          = ARM_USART_EVENT_RX_BREAK,          ///< RX break event
  SL_USART_EVENT_RX_FRAMING_ERROR  = ARM_USART_EVENT_RX_FRAMING_ERROR,  ///< RX Framing error event
  SL_USART_EVENT_RX_PARITY_ERROR   = ARM_USART_EVENT_RX_PARITY_ERROR,   ///< RX parity error event
  SL_USART_EVENT_CTS               = ARM_USART_EVENT_CTS,               ///< CTS event
  SL_USART_EVENT_DSR               = ARM_USART_EVENT_DSR,               ///< DSR EVENT
  SL_USART_EVENT_DCD               = ARM_USART_EVENT_DCD,               ///< DCD event
  SL_USART_EVENT_RI                = ARM_USART_EVENT_RI                 ///< RI Event
} usart_event_typedef_t;

/// @brief General power states
typedef enum {
  SL_POWER_OFF  = ARM_POWER_OFF,  ///< Power Off
  SL_POWER_LOW  = ARM_POWER_LOW,  ///< Power low
  SL_POWER_FULL = ARM_POWER_FULL, ///< Power Full
  SL_POWER_MODE_LAST,
} power_mode_typedef_t;

/// @brief Databit selection.
typedef enum {
  SL_USART_DATA_BITS_5 = ARM_USART_DATA_BITS_5, ///< 5 data bits
  SL_USART_DATA_BITS_6 = ARM_USART_DATA_BITS_6, ///< 6 data bits
  SL_USART_DATA_BITS_7 = ARM_USART_DATA_BITS_7, ///< 7 data bits
  SL_USART_DATA_BITS_8 = ARM_USART_DATA_BITS_8, ///< 8 data bits
  SL_USART_DATA_BITS_9 = ARM_USART_DATA_BITS_9, ///< 9 data bits
} usart_databits_typedef_t;

/// @brief Parity selection
typedef enum {
  SL_USART_NO_PARITY   = ARM_USART_PARITY_NONE, ///< No parity
  SL_USART_EVEN_PARITY = ARM_USART_PARITY_EVEN, ///< Even parity
  SL_USART_ODD_PARITY  = ARM_USART_PARITY_ODD   ///< Odd parity.
} usart_parity_typedef_t;

/// @brief USART Modem control selection
typedef enum {
  SL_USART_RTS_CLEAR = ARM_USART_RTS_CLEAR, ///< RTS clear
  SL_USART_RTS_SET   = ARM_USART_RTS_SET,   ///< RTS Set
  SL_USART_DTR_CLEAR = ARM_USART_DTR_CLEAR, ///< DTR Clear
  SL_USART_DTR_SET   = ARM_USART_DTR_SET,   ///< Activate DTR
  SL_USART_MODEM_CONTROL_LAST
} usart_modem_control_typedef_t;

/// @brief Stop bits selection, used for asynchronous operation
typedef enum {
  SL_USART_STOP_BITS_0_5 = ARM_USART_STOP_BITS_0_5, ///< 0.5 stop bits.
  SL_USART_STOP_BITS_1   = ARM_USART_STOP_BITS_1,   ///< 1 stop bits.
  SL_USART_STOP_BITS_1_5 = ARM_USART_STOP_BITS_1_5, ///< 1.5 stop bits.
  SL_USART_STOP_BITS_2   = ARM_USART_STOP_BITS_2    ///< 2 stop bits
} usart_stopbit_typedef_t;

/// @brief Hardware Flow Control Selection
typedef enum {
  SL_USART_FLOW_CONTROL_NONE = ARM_USART_FLOW_CONTROL_NONE, ///< No hardware flow control
  SL_USART_FLOW_CONTROL_CTS  = ARM_USART_FLOW_CONTROL_CTS,  ///< CTS signal is enabled for TX flow control.
  SL_USART_FLOW_CONTROL_RTS  = ARM_USART_FLOW_CONTROL_RTS,  ///< RTS signal is enabled for RX flow control.
  SL_USART_FLOW_CONTROL_RTS_CTS =
    ARM_USART_FLOW_CONTROL_RTS_CTS ///< CTS and RTS signals are enabled for TX and RX flow control.
} usart_hwflowcontol_typedef_t;

/// @brief USART Mode selection
typedef enum {
  SL_USART_MODE_ASYNCHRONOUS       = ARM_USART_MODE_ASYNCHRONOUS,       ///< Asynchronous mode
  SL_USART_MODE_SYNCHRONOUS_MASTER = ARM_USART_MODE_SYNCHRONOUS_MASTER, ///< Synchronous mode master
  SL_USART_MODE_SYNCHRONOUS_SLAVE  = ARM_USART_MODE_SYNCHRONOUS_SLAVE,  ///< Synchronous mode slave
  SL_USART_MODE_SINGLE_WIRE        = ARM_USART_MODE_SINGLE_WIRE, ///< UART Single-wire (half-duplex); arg = Baudrate
  SL_USART_MODE_IRDA               = ARM_USART_MODE_IRDA         ///< UART IrDA; arg = Baudrate
} usart_mode_typedef_t;

/// @brief USART misc control selection
typedef enum {
  SL_USART_MISC_CONTROL_NONE,
  SL_USART_SET_DEFAULT_TX_VALUE = ARM_USART_SET_DEFAULT_TX_VALUE, ///< Set default TX value
  SL_USART_CONTROL_TX           = ARM_USART_CONTROL_TX,           ///< Set transfer line
  SL_USART_CONTROL_RX           = ARM_USART_CONTROL_RX,           ///< Set receive line
  SL_USART_CONTROL_BREAK        = ARM_USART_CONTROL_BREAK,        ///< Set USART control break
  SL_USART_ABORT_SEND           = ARM_USART_ABORT_SEND,           ///< Abort send
  SL_USART_ABORT_RECEIVE        = ARM_USART_ABORT_RECEIVE,        ///< Abort receive
  SL_USART_ABORT_TRANSFER       = ARM_USART_ABORT_TRANSFER        ///< Abort transfer
} usart_misc_control_typedef_t;

/*******************************************************************************
   *******************************   STRUCTS   ***********************************
   ******************************************************************************/
/// @brief USART configuration control structure
typedef struct {
  uint32_t baudrate;                          ///< baud rate
  usart_mode_typedef_t mode;                  ///< USART mode of operation
  usart_parity_typedef_t parity;              ///< USART parity bit
  usart_stopbit_typedef_t stopbits;           ///< USART stop bits
  usart_hwflowcontol_typedef_t hwflowcontrol; ///< USART h/w flow control
  usart_databits_typedef_t databits;          ///< USART databits
  usart_misc_control_typedef_t misc_control;  ///< USART MISC_CONTROL
  uint32_t usart_module;                      ///< USART module
  boolean_t config_enable;                    ///< USART TX and RX Config enable
  boolean_t synch_mode;                       ///< Synchronous mode
} sl_si91x_usart_control_config_t;

/// @brief Structure to hold the different versions of peripheral API
typedef struct {
  uint8_t release; ///< Release version number
  uint8_t major;   ///< sqa version number
  uint8_t minor;   ///< dev version number
} sl_usart_version_t;
/*******************************************************************************
*****************************   PROTOTYPES   **********************************
******************************************************************************/

/***************************************************************************/ /**
* @brief
* Initialize USART/UART interface.
*
* @details
* This function will configure the clocks for USART/UART module and also
* initialization the DMA for UART/USART  if DMA is enabled for data transfers.
*
* @param[in] usart_handle Pointer to the USART/UART driver
* @param[in] callback_event Callback function which need to be called on data transfer
* @return status 0 if successful, else error code
*       \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
*       \ref SL_STATUS_FAIL (0x0001)         - Fail , UART/USART initialization failed \n
*       \ref SL_STATUS _OK (0X000)           - Success ,UART/USART initialization done properly \n
*
******************************************************************************/
sl_status_t sl_si91x_usart_init(usart_peripheral_t usart_instance, sl_usart_handle_t *usart_handle);

/***************************************************************************/ /**
* @brief
* Deinit USART/UART interface.
*
* @details
* This function will disable the clocks for USART/UART module and also
* Deinit the DMA for UART/USART if DMA is enabled for data transfers.
*
* @param[in] usart_handle Pointer to the USART/UART driver
* @return status 0 if successful, else error code
*       \ref SL_STATUS_FAIL (0x0001)   - Fail , UART/USART Deinitialization failed \n
*       \ref SL_STATUS _OK (0X000)     - Success ,UART/USART Deinitialization done properly \n
*
***************************************rsi***************************************/
sl_status_t sl_si91x_usart_deinit(sl_usart_handle_t usart_handle);

/***************************************************************************/ /**
* @brief
* Set the  USART/UART power mode.
*
* @details
* This function will set UART/USART power mode of operation .
* In poweroff power state will disable the interrupt , disable DMA channel if DMA mode
* disable the power to the USART block.So no usart operations are possible when set
* this mode
* In powerFull power state will enable the intterupt and set the flags initialization
* and power on flag so that USART data transfer can takes place
*
* @pre \ref sl_si91x_usart_init();
*
* @param[in] usart_handle Pointer to the USART/UART driver
* @param[in] state  power state
*                 @li powerOff
*                 @li powerLow
*                 @li powerFull
*
* @return status 0 if successful, else error code
*       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
*       \ref SL_STATUS_FAIL (0x0001)   - Fail , UART/USART initialization failed \n
*       \ref SL_STATUS _OK (0X000)     - Success ,UART/USART initialization done properly \n
*
* @note
* powerLow power state is not supported
******************************************************************************/
sl_status_t sl_si91x_usart_set_power_mode(sl_usart_handle_t usart_handle, power_mode_typedef_t state);

/***************************************************************************/ /**
* @brief
* Register the user callback function.
*
* @pre sl_si91x_usart_set_configuration();
*
* @param[in] callback_event Pointer to the function which needs to be called at the time of interrupt
* @return status 0 if successful, else error code
*         \ref SL_STATUS_OK (0x0000) - Success \n
*         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
*         \ref SL_STATUS_BUSY (0x0004) - Driver is busy \n
******************************************************************************/
sl_status_t sl_si91x_usart_register_event_callback(sl_usart_signal_event_t callback_event);

/***************************************************************************/ /**
* @brief
* Un-register the user callback function.
*
* @param[in] none
* @return none
******************************************************************************/
void sl_si91x_usart_unregister_event_callback(void);

/***************************************************************************/ /**
* @brief
* Start sending data to USART transmitter.
*
* @details
* if DMA mode is set this function will configure the DMA channel and enables the DMA channel ,
* then transfer's the data pointed to it else it fill the data to the transfer FIFO and transfer the data
*
* @pre \ref sl_si91x_usart_init(); \ref sl_si91x_usart_set_power_mode();
*      \ref sl_si91x_usart_set_configuration();
*
* @param[in] usart_handle Pointer to the USART/UART driver
* @param[in] data Pointer to the variable which contains transfer data
* @param[in] data_length Data_length to Send
*
* @return status 0 if successful, else error code
*        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
*        \ref SL_STATUS_FAIL (0x0001)   - Fail , Data transfer failed \n
*        \ref SL_STATUS_BUSY (0x0004)   -  Busy ,already data transfer is going on \n
*        \ref SL_STATUS _OK (0X000)     - Success ,UART/USART initialization done properly \n
*
********************************************************************************/
sl_status_t sl_si91x_usart_send_data(sl_usart_handle_t usart_handle, const void *data, uint32_t data_length);

/***************************************************************************/ /**
* @brief
* Start receiving data from USART receiver.
*
* @details
* If DMA mode is set, it configures the DMA channel, enables the DMA channel,
*  and receives data via DMA. If DMA mode is not set, it receives the data from FIFO.
*
* @pre \ref sl_si91x_usart_init(); \ref sl_si91x_usart_set_power_mode();
*       \ref sl_si91x_usart_set_configuration();
*
* @param[in] usart_handle Pointer to the USART/UART driver
* @param[in] data Pointer to the variable which will store the received data
* @param[in] data_length Data_length to receive
*
* @return status 0 if successful, else error code
*        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
*        \ref SL_STATUS_FAIL (0x0001)   - Fail , Data transfer failed \n
*        \ref SL_STATUS_BUSY (0x0004)   -  Busy ,already data transfer is going on \n
*        \ref SL_STATUS _OK (0X000)     - Success ,UART/USART initialization done properly \n
*
* *******************************************************************************/
sl_status_t sl_si91x_usart_receive_data(sl_usart_handle_t usart_handle, void *data, uint32_t data_length);

/***************************************************************************/ /**
* @brief
* Start sending/receiving data to/from USART transmitter/receiver.
*
* @details
* This function will configure the DMA channel and enables the DMA channel , DMA
* if DMA mode is set and transfer's the data pointed to it
*
* @pre \ref sl_si91x_usart_init(); \ref sl_si91x_usart_set_power_mode();
*      \ref sl_si91x_usart_set_configuration();
*
* @param[in] usart_handle Pointer to the USART/UART driver
* @param[in] data_out Pointer to the variable which will store the data to be transferred
* @param[in] data_in Pointer to the variable which will store the received data
* @param[in] data_length Data_length to receive
*
* @return status 0 if successful, else error code
*        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
*        \ref SL_STATUS_FAIL (0x0001)   - Fail , Data transfer failed \n
*        \ref SL_STATUS_BUSY (0x0004)   -  Busy ,already data transfer is going on \n
*        \ref SL_STATUS _OK (0X000)     - Success ,UART/USART initialization done properly \n
*
* @note
* This function to be use in USART mode only i.e synchronous mode only in asynchronous
* mode use sl_si91x_usart_receive_data() and sl_si91x_usart_send_data()
* *******************************************************************************/
sl_status_t sl_si91x_usart_transfer_data(sl_usart_handle_t usart_handle,
                                         const void *data_out,
                                         void *data_in,
                                         uint32_t data_length);

/***************************************************************************/ /**
* @brief
* Get the USART/UART transfer data count.
*
* @details
* This function will return the USART data transferred count.
*
* @param[in] usart_handle Pointer to the USART/UART driver
* @return return the no of bytes transferred
******************************************************************************/
uint32_t sl_si91x_usart_get_tx_data_count(sl_usart_handle_t usart_handle);

/***************************************************************************/ /**
* @brief
* Get the USART/UART received data count.
*
* @details
* This function will return the USART/UART data received count
*
* @param[in] usart_handle Pointer to the USART/UART driver
* @return return the no of bytes received
******************************************************************************/
uint32_t sl_si91x_usart_get_rx_data_count(sl_usart_handle_t usart_handle);

/***************************************************************************/ /**
* @brief
* Configure the different configurations of USART Interface.
*
* @details
* This function configure the USART in different configurations such as USART mode,
* Data Bits, Parity , stop bits, flow control and baud rate.
*
* @param[in] usart_handle Pointer to the USART/UART driver
* @param[in] control_configuration pointer to the USART configurations
* @param[in] baud_rate baud rate to data transfer
* @return status 0 if successful, else error code
*        \ref SL_STATUS_BUSY (0x0004)         -  Busy ,already data transfer is going on \n
*        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
*        \ref SL_STATUS_INVALID_MODE (0x0024) -  USART Invalid mode of operation \n
*        \ref SL_STATUS_NOT_SUPPORTED(0x000F) -  Feature not supported \n
*        \ref SL_STATUS _OK (0X000)     - Success ,UART/USART initialization done properly \n
******************************************************************************/
sl_status_t sl_si91x_usart_set_configuration(sl_usart_handle_t usart_handle,
                                             sl_si91x_usart_control_config_t *control_configuration,
                                             uint32_t baud_rate);

/***************************************************************************/ /**
* @brief
* Get USART status.
*
* @details
* This function will return USART/UART transfer and receive status
*
* @param[in] usart_handle Pointer to the USART/UART driver
* @return usart line status *
*       tx busy ,rx_busy, tx_underflow ,rx overflow, rx break, rx framing error
*       rx parity error
******************************************************************************/
sl_usart_status_t sl_si91x_usart_get_status(sl_usart_handle_t usart_handle);

/***************************************************************************/ /**
* @brief
* Set USART Modem Control line state.
*
* @details
* This function will set the USART modem control line
*
* @param[in] usart_handle Pointer to the USART/UART driver
* @param[in] control  usart modem control
* @return status 0 if successful, else error code
*       \ref SL_STATUS_NOT_SUPPORTED(0x000F) -  Feature not supported \n
*       \ref SL_STATUS _OK (0x000)           - Success ,UART/USART initialization done properly \n
*
******************************************************************************/
sl_status_t sl_si91x_usart_set_modem_control(sl_usart_handle_t usart_handle, sl_usart_modem_control_t control);

/***************************************************************************/ /**
* @brief
* Get USART Modem Control line state.
*
* @details
* This function return USART modem control status
*
* @param[in] usart_handle Pointer to the USART/UART driver
* @return \ref usart modem status states are active or not
******************************************************************************/
sl_usart_modem_status_t sl_si91x_usart_get_modem_status(sl_usart_handle_t usart_handle);

/***************************************************************************/ /**
* @brief
* Get the USART version.
*
* @details
* This function is used to know the USART version
*
* @param[in] none
* @return \ref sl_usart_version_t type version
******************************************************************************/
sl_usart_version_t sl_si91x_usart_get_version(void);

/***************************************************************************/ /**
* @brief
* Get the USART configurations set.
*
* @details
* Get the USART configurations set in the module such as baudrate ,parity bit
* stop bits etc
*
* @param[in] usart_module USART peripheral type
*   -  0 - USART0
*   -  1 - UART1
*   -  2 - ULP_UART
* @param[in] usart_config Pointer to the USART configurations structure
* @return status 0 if successful, else error code
*      \ref SL_STATUS_NULL_POINTER(0x0022) -  Invalid NULL pointer received as argument \n
*      \ref SL_STATUS _OK (0x000)           - Success ,UART/USART configurations retrieved
*                                          successfully \n
******************************************************************************/
sl_status_t sl_si91x_usart_get_configurations(uint8_t usart_module, sl_si91x_usart_control_config_t *usart_config);

/** @} (end addtogroup USART) */

#ifdef __cplusplus
}
#endif

#endif /* SL_SI91X_USART_H */