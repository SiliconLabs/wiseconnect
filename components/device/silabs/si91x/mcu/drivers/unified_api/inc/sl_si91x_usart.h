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
 * @brief Initilize the USART/UART module.
 * This function will configure the clocks for USART/UART module and also
 * initialization of the DMA for UART/USART, if DMA is enabled for data transfers.
 * @details This function will configure the clocks for USART/UART module and also
 * initialize the DMA for UART/USART if DMA is enabled for data transfers.
 * @param[in] usart_handle Pointer to the USART/UART driver
 * @param[in] callback_event Callback function which needs to be called on data transfer
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 *         - \ref SL_STATUS_FAIL (0x0001) - Fail, UART/USART initialization failed 
 *         - \ref SL_STATUS_OK (0x0000) - Success, UART/USART initialization done properly
******************************************************************************/
sl_status_t sl_si91x_usart_init(usart_peripheral_t usart_instance, sl_usart_handle_t *usart_handle);

/***************************************************************************/ /**
 * @brief Uninitialize the USART/UART module.
 * @details This function will disable the clocks for USART/UART module and also
 * Deinit the DMA for UART/USART if DMA is enabled for data transfers.
 *
 * @param[in] usart_handle Pointer to the USART/UART driver
 *
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_FAIL (0x0001) - Fail, UART/USART Deinitialization failed 
 *         - \ref SL_STATUS_OK (0x0000) - Success, UART/USART Deinitialization done properly 
 * @note
 *   When the USART/UART module is used in combination with other peripherals, while de-initializing in the application, refer to the notes below:
 *   1. Whenever sl_si91x_usart_deinit() gets called, it will power down the domain (PERI_EFUSE), which contains different peripherals as mentioned below:
 *      USART, UART, I2C, SSI Master, SSI Slave, Generic-SPI Master, I2S Master, I2S Slave, Micro-DMA Controller, Config Timer,
 *      Random-Number Generator, CRC Accelerator, SIO, QEI, MCPWM and EFUSE.
 *      Since deinit power downs the PERI_EFUSE domain, it's recommended to call the sl_si91x_usart_deinit() API at the end of the application.
 *   2. Few peripherlas(ULP Peripherls, UULP Peripherlas, GPDMA and SDIO-SPI) have seperate domains those can be powered down indepedently. For additional details, refer to the Power architecture section in the Hardware Reference Manual
 *      e.g., To power down ULP UART, use the API below:
 *      RSI_PS_M4ssPeriPowerDown(ULPSS_PWRGATE_ULP_UART); 
 *      Here, ULP_UART has seperate power domain ULPSS_PWRGATE_ULP_UART, which can be powered down indepedently. Refer to the rsi_power_save.h file for all power gates definitions.
******************************************************************************/
sl_status_t sl_si91x_usart_deinit(sl_usart_handle_t usart_handle);

/***************************************************************************/ /**
 * @brief Register the user callback function.
 * @details If another callback is registered without unregistering previous callback then, 
 * it returns an error code, so it is mandatory 
 * to unregister the callback before registering to another callback.
 * @pre Pre-condition:
 *      - \ref sl_si91x_usart_set_configuration
 *
 * @param[in] callback_event Pointer to the function which needs to be called at the time of interrupt
 *
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_OK (0x0000) - Success 
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 *         - \ref SL_STATUS_BUSY (0x0004) - Driver is busy 
******************************************************************************/
sl_status_t sl_si91x_usart_register_event_callback(sl_usart_signal_event_t callback_event);

/***************************************************************************/ /**
* @brief Unregister the user callback function.
* Unregister the user callback function before regsitering new callback function.
*
* @param[in] none
*
* @return none
******************************************************************************/
void sl_si91x_usart_unregister_event_callback(void);

/***************************************************************************/ /**
* @brief This function registers the user callback function for multiple USART instances i.e., to use different 
* instance (USART_O, UART_1, ULP_UART) this API need to use
* @pre Pre-condition:
*         - \ref sl_si91x_usart_set_configuration();
* @param[in] usart_instance USART Instance
* @param[in] callback_event Pointer to the function which needs to be called at the time of interrupt
* @return status 0 if successful, else error code
*         \ref SL_STATUS_OK (0x0000) - Success \n
*         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
*         \ref SL_STATUS_BUSY (0x0004) - Driver is busy \n
******************************************************************************/
sl_status_t sl_si91x_usart_multiple_instance_register_event_callback(usart_peripheral_t usart_instance,
                                                                     sl_usart_signal_event_t callback_event);

/***************************************************************************/ /**
* @brief Unregister the user callback function in case of multiple USART instances.
* @param[in] usart_instance USART Instance
* @return none
******************************************************************************/
void sl_si91x_usart_multiple_instance_unregister_event_callback(usart_peripheral_t usart_instance);

/***************************************************************************/ /**
* @brief Send the USART data when USART/UART is configured.
* @details If DMA mode is set this function will configure the DMA channel and enables the DMA channel ,
* then transfers the data pointed to it, else it fill the data to the transfer FIFO and transfer the data
* @pre Pre-conditions:
*      - \ref sl_si91x_usart_init();
*      - \ref sl_si91x_usart_set_configuration();
* @param[in] usart_handle Pointer to the USART/UART driver
* @param[in] data Pointer to the variable which contains transfer data
* @param[in] data_length Data_length to Send
* @return status 0 if successful, else error code
*        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument
*        \ref SL_STATUS_FAIL (0x0001)   - Fail , Data transfer failed
*        \ref SL_STATUS_BUSY (0x0004)   -  Busy ,already data transfer is going on
*        \ref SL_STATUS _OK (0X000)     - Success ,UART/USART initialization done properly
********************************************************************************/
sl_status_t sl_si91x_usart_send_data(sl_usart_handle_t usart_handle, const void *data, uint32_t data_length);

/***************************************************************************/ /**
* @brief To Send the data in async mode when USART/UART is configured.
* @details This function returns immediately(non blocking) and data transfer happens asyncronously. Once the data transfer compeletes,
* registered user callback get invoked
* @pre  Pre-conditions:
*       - \ref sl_si91x_usart_init();
*       - \ref sl_si91x_usart_set_configuration();
* @param[in] usart_handle Pointer to the USART/UART driver
* @param[in] data Pointer to the variable which will store the transferred data
* @param[in] data_length Data_length to transfer
* @return status 0 if successful, else error code
*        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument
*        \ref SL_STATUS_FAIL (0x0001)   - Fail , Data transfer failed
*        \ref SL_STATUS_BUSY (0x0004)   -  Busy ,already data transfer is going on
*        \ref SL_STATUS _OK (0X000)     - Success ,UART/USART initialization done properly
*
* *******************************************************************************/
sl_status_t sl_si91x_usart_async_send_data(sl_usart_handle_t usart_handle, const void *data, uint32_t data_length);

/***************************************************************************/ /**
 * @brief To receive the data when USART/UART is configured.
 * @details If DMA mode is set, it configures the DMA channel, enables the DMA channel,
 * and receives data via DMA. If DMA mode is not set, it receives the data from FIFO.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_usart_init
 *      - \ref sl_si91x_usart_set_configuration
 * @param[in] usart_handle Pointer to the USART/UART driver
 * @param[in] data Pointer to the variable which will store the received data
 * @param[in] data_length Data_length to receive
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 *         - \ref SL_STATUS_FAIL (0x0001) - Fail, Data transfer failed 
 *         - \ref SL_STATUS_BUSY (0x0004) - Busy, already data transfer is going on 
 *         - \ref SL_STATUS_OK (0x0000) - Success, UART/USART initialization done properly 
******************************************************************************/
sl_status_t sl_si91x_usart_receive_data(sl_usart_handle_t usart_handle, void *data, uint32_t data_length);

/***************************************************************************/ /**
* @brief To receive the data in async mode when USART/UART is configured.
* @details This function returns immediately(Non blocking) and data reception happens asyncronously. Once the data reception compeletes,
* registered user callback get invoked
* @pre Pre-conditions:
*       - \ref sl_si91x_usart_init();
*       - \ref sl_si91x_usart_set_configuration();
* @param[in] usart_handle Pointer to the USART/UART driver
* @param[in] data Pointer to the variable which will store the received data
* @param[in] data_length Data_length to receive
* @return status 0 if successful, else error code
*        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument
*        \ref SL_STATUS_FAIL (0x0001)   - Fail , Data transfer failed
*        \ref SL_STATUS_BUSY (0x0004)   -  Busy ,already data transfer is going on
*        \ref SL_STATUS _OK (0X000)     - Success ,UART/USART initialization done properly
*
* *******************************************************************************/
sl_status_t sl_si91x_usart_async_receive_data(sl_usart_handle_t usart_handle, void *data, uint32_t data_length);

/***************************************************************************/ /**
* @brief To send and receive the data when USART in Synchronous mode is enabled. 
 * @details This function sends and receives the data to and from usart transmitter and
 * receiver in synchronous mode of operation.
 * Configure the DMA channel and enables the DMA channel, if DMA mode is set and 
 * transfer's the data pointed to it else transfer the data from fifo.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_usart_init
 *      - \ref sl_si91x_usart_set_configuration
 * @param[in] usart_handle Pointer to the USART/UART driver
 * @param[in] data_out Pointer to the variable which will store the data to be transferred
 * @param[in] data_in Pointer to the variable which will store the received data
 * @param[in] data_length Data_length to receive
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 *         - \ref SL_STATUS_FAIL (0x0001) - Fail, Data transfer failed 
 *         - \ref SL_STATUS_BUSY (0x0004) - Busy, already data transfer is going on 
 *         - \ref SL_STATUS_OK (0x0000) - Success, UART/USART initialization done properly 
 *
 * @note This function to be used in USART mode only, i.e., synchronous mode only in asynchronous
 * mode use sl_si91x_usart_receive_data() and sl_si91x_usart_send_data()
******************************************************************************/
sl_status_t sl_si91x_usart_transfer_data(sl_usart_handle_t usart_handle,
                                         const void *data_out,
                                         void *data_in,
                                         uint32_t data_length);

/***************************************************************************/ /**
 * @brief To get the TX data count of USART/UART
 * @details This function will return the USART data transferred count.
 *
 * @param[in] usart_handle Pointer to the USART/UART driver
 *
 * @return return the no of bytes transferred
******************************************************************************/
uint32_t sl_si91x_usart_get_tx_data_count(sl_usart_handle_t usart_handle);

/***************************************************************************/ /**
* @brief To get the RX data count of USART/UART
* @details This function will return the USART/UART data received count
*
* @param[in] usart_handle Pointer to the USART/UART driver
*
* @return return the no of bytes received
******************************************************************************/
uint32_t sl_si91x_usart_get_rx_data_count(sl_usart_handle_t usart_handle);

/***************************************************************************/ /**
* @brief To control and configure the USART/UART module in different configurations.
* @details This function configure the USART in different configurations such as USART mode,
* Data Bits, Parity , stop bits, flow control and baud rate.
*
* @param[in] usart_handle Pointer to the USART/UART driver
*
* @param[in] control_configuration pointer to the USART configurations
*
* @return status 0 if successful, else error code
*        - \ref SL_STATUS_BUSY (0x0004)         -  Busy ,already data transfer is going on 
*        - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument 
*        - \ref SL_STATUS_INVALID_MODE (0x0024) -  USART Invalid mode of operation 
*        - \ref SL_STATUS_NOT_SUPPORTED(0x000F) -  Feature not supported 
*        - \ref SL_STATUS _OK (0X000)     - Success ,UART/USART initialization done properly 
******************************************************************************/
sl_status_t sl_si91x_usart_set_configuration(sl_usart_handle_t usart_handle,
                                             sl_si91x_usart_control_config_t *control_configuration);

/*******************************************************************************
* @brief This is internal function used to configure the different configurations of USART Interface. 
* @details This API will not pick the configurations from USART UC.
* Configure the USART in different configurations such as USART mode,
* Data Bits, Parity , stop bits, flow control and baud rate.
*
* @param[in] usart_handle Pointer to the USART/UART driver
*
* @param[in] control_configuration pointer to the USART configurations
*
* @return status 0 if successful, else error code
*        - \ref SL_STATUS_BUSY (0x0004)         -  Busy ,already data transfer is going on 
*        - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument 
*        - \ref SL_STATUS_INVALID_MODE (0x0024) -  USART Invalid mode of operation 
*        - \ref SL_STATUS_NOT_SUPPORTED(0x000F) -  Feature not supported 
*        - \ref SL_STATUS _OK (0X000)     - Success ,UART/USART initialization done properly 
******************************************************************************/
sl_status_t sli_si91x_usart_set_non_uc_configuration(sl_usart_handle_t usart_handle,
                                                     sl_si91x_usart_control_config_t *control_configuration);

/***************************************************************************/ /**
* @brief To get the USART/UART status.
* @details This function will return USART/UART transfer and receive status
* @param[in] usart_handle Pointer to the USART/UART driver
* @return usart line status
*       tx busy ,rx_busy, tx_underflow ,rx overflow, rx break, rx framing error
*       rx parity error
******************************************************************************/
sl_usart_status_t sl_si91x_usart_get_status(sl_usart_handle_t usart_handle);

/***************************************************************************/ /**
* @brief This function will set the USART modem control line
* @param[in] usart_handle Pointer to the USART/UART driver
* @param[in] control  usart modem control
* @return status 0 if successful, else error code
*       - \ref SL_STATUS_NOT_SUPPORTED(0x000F) -  Feature not supported 
*       - \ref SL_STATUS _OK (0x000)           - Success ,UART/USART initialization done properly
******************************************************************************/
sl_status_t sl_si91x_usart_set_modem_control(sl_usart_handle_t usart_handle, sl_usart_modem_control_t control);

/***************************************************************************/ /**
* @brief This function return USART modem control status
* @param[in] usart_handle Pointer to the USART/UART driver
* @return \ref usart modem status states are active or not
******************************************************************************/
sl_usart_modem_status_t sl_si91x_usart_get_modem_status(sl_usart_handle_t usart_handle);

/***************************************************************************/ /**
* @brief This function is used to know the USART version
* @param[in] none
* @return \ref sl_usart_version_t type version
******************************************************************************/
sl_usart_version_t sl_si91x_usart_get_version(void);

/***************************************************************************/ /**
* @brief Get the USART configurations set in the module such as baudrate, parity bit
* stop bits etc.
* @param[in] usart_module USART peripheral type
*   -  0 - USART0
*   -  1 - UART1
*   -  2 - ULP_UART
* @param[in] usart_config Pointer to the USART configurations structure
* @return status 0 if successful, else error code
*      - \ref SL_STATUS_NULL_POINTER(0x0022) -  Invalid NULL pointer received as argument \
*      - \ref SL_STATUS _OK (0x000)           - Success ,UART/USART configurations retrieved
*                                          successfully \
******************************************************************************/
sl_status_t sl_si91x_usart_get_configurations(uint8_t usart_module, sl_si91x_usart_control_config_t *usart_config);

/** @} (end addtogroup USART) */

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY !***********************
/// @addtogroup USART USART
/// @{
///
///   @details
///
///
///   @n @section USART_Intro Introduction
///
///   The USART/UART driver offers a robust set of functionalities to enable
///   seamless communication over the USART/UART interface. This driver supports various features, including DMA (Direct Memory Access)
///   for efficient data transfer, event callback mechanisms for interrupt handling, etc.
///   The driver simplifies the implementation of serial communication protocols and enhances the overall performance and reliability of the system.
///
///   @n @section USART_Config Configuration
///
///   To configure the USART/UART driver, user can utilize a comprehensive set of parameters and functions
///   tailored to their specific requirements. This configuration process involves setting up various aspects of the USART/UART module,
///   such as baud rate, data bits, parity, stop bits, flow control, and DMA settings if DMA is enabled.
///   Additionally, register callback functions can be used to handle events and interrupts, ensuring timely and efficient communication.
///   The driver also supports multiple USART/UART instances, allowing for versatile usage across different peripherals.
///
///   @li For more information on configuring available parameters refer to the respective peripheral example readme document.
///
///   @n @section USART_Usage Usage
///
///   USART has to be initialized and configured first by calling the @ref sl_si91x_usart_init. The DMA (Direct Memory Access) controller in the USART driver
///   will enable efficient data transfer, thus reducing CPU overhead. Initialization of the DMA for UART/USART will also be done with the same USART init function,
///   if DMA is enabled for data transfers. The other common functions include the following:
///
///   @li @ref sl_si91x_usart_init
///   @li @ref sl_si91x_usart_set_configuration
///   @li @ref sl_si91x_usart_register_event_callback
///   @li @ref sl_si91x_usart_multiple_instance_register_event_callback
///   @li @ref sl_si91x_usart_async_send_data
///   @li @ref sl_si91x_usart_async_receive_data
///   @li @ref sl_si91x_usart_transfer_data
///   @li @ref sl_si91x_usart_get_status
///   @li @ref sl_si91x_usart_get_tx_data_count
///   @li @ref sl_si91x_usart_get_rx_data_count
///   @li @ref sl_si91x_usart_set_modem_control
///   @li @ref sl_si91x_usart_deinit
///
///   @li Once initialised, you can configure the module using @ref sl_si91x_usart_set_configuration. Here you can set appropriate configurations such as USART module
///   Recieve-transfer configuration (flow control), data bits, parity and baud rate.
///
///   @li Once configured, you can register callback functions using @ref sl_si91x_usart_register_event_callback
///   and @ref sl_si91x_usart_unregister_event_callback for event handling.
///   For multiple USART instances, you can use @ref sl_si91x_usart_multiple_instance_register_event_callback and
///   @ref sl_si91x_usart_multiple_instance_unregister_event_callback for the same.
///
///   There are a total of three instances of USART - USART, UART2, ULP UART. USART can be used synchronously as well as asynchronously.
///   For USART in synchronous mode use the function @ref sl_si91x_usart_transfer_data. Here, data transfer is done with a shared clock signal
///   between the sender and receiver. On the other hand, to use USART in asynchronous mode, for UART2, ULP UART use @ref sl_si91x_usart_async_send_data and
///   @ref sl_si91x_usart_async_receive_data to send and receive data asynchronously. Here, data transfer will happen asynchronously i.e., in non-blocking.
///   Once the data transfer is complete, user event callback function will be called.
///
///   User can also use @ref sl_si91x_usart_get_tx_data_count and
///   @ref sl_si91x_usart_get_rx_data_count to get the number of bytes transferred and received, respectively.
///
///   @li Additionally, You can also check the status of the module with @ref sl_si91x_usart_get_status.
///   you can control the modem lines using @ref sl_si91x_usart_set_modem_control.
///
///   @li @ref sl_si91x_usart_deinit can be used to deinitialize the module. Similar to initialization, de-initialization of the DMA for
///   UART/USART will also be done with the USART de-init function, if DMA is enabled for data transfers.
///
/// @} end group SYSRTC ********************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SL_SI91X_USART_H */
