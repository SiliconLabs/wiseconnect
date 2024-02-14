/***************************************************************************/ /**
 * @file sl_si91x_i2s.h
 * @brief I2S API implementation
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

#ifndef SL_SI91X_I2S_H
#define SL_SI91X_I2S_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_status.h"
#include "SAI.h"

/***************************************************************************/ /**
 * @addtogroup I2S I2S
 * @ingroup SI91X_PERIPHERAL_APIS
 * @{
 *
 ******************************************************************************/
// -----------------------------------------------------------------------------
// Data Types

typedef ARM_SAI_SignalEvent_t sl_i2s_signal_event_t; ///< Renamed signal event structure
typedef ARM_SAI_STATUS sl_i2s_status_t;              ///< Renamed status structure
typedef ARM_DRIVER_SAI sl_i2s_driver_t;              ///< Renamed I2S driver structure
typedef const void *sl_i2s_handle_t;                 ///< Created I2S handle type

/// @brief Enumeration for different I2S callback events
typedef enum {
  SL_I2S_SEND_COMPLETE    = ARM_SAI_EVENT_SEND_COMPLETE,    ///< Send complete event
  SL_I2S_RECEIVE_COMPLETE = ARM_SAI_EVENT_RECEIVE_COMPLETE, ///< Receive lost event
  SL_I2S_TX_UNDERFLOW     = ARM_SAI_EVENT_TX_UNDERFLOW,     ///< Tx underflow event
  SL_I2S_RX_OVERFLOW      = ARM_SAI_EVENT_RX_OVERFLOW,      ///< Rx Overflow event
  SL_I2S_FRAME_ERROR      = ARM_SAI_EVENT_FRAME_ERROR       ///< Frame error event
} i2s_event_typedef_t;

/// @brief Enumeration for I2S power state
typedef enum {
  SL_I2S_POWER_OFF  = ARM_POWER_OFF, ///< Power mode OFF
  SL_I2S_LOW_POWER  = ARM_POWER_LOW, ///< Low power mode
  SL_I2S_FULL_POWER = ARM_POWER_FULL ///< Full power mode
} sl_i2s_power_state_t;

/// @brief Enumeration for I2S Primary and Secondary modes
typedef enum {
  SL_I2S_MASTER = ARM_SAI_MODE_MASTER, ///< I2S primary mode
  SL_I2S_SLAVE  = ARM_SAI_MODE_SLAVE   ///< I2S secondary mode
} sl_i2s_mode_t;

/// @brief Enumeration for SAI protocol type
typedef enum {
  SL_I2S_PROTOCOL = ARM_SAI_PROTOCOL_I2S, ///< I2S protocol
  SL_PCM_PROTOCOL = 0                     ///< PCM protocol, currently not supported in the driver
} sl_sai_protocol_t;

/// @brief Enumeration for I2S SYNC/ASYNC modes
typedef enum {
  SL_I2S_SYNC  = ARM_SAI_SYNCHRONOUS, ///< I2S synchronous mode
  SL_I2S_ASYNC = ARM_SAI_ASYNCHRONOUS ///< I2S asynchronous mode
} sl_i2s_sync_t;

/// @brief Enumeration for I2S transfer type
typedef enum {
  SL_I2S_TRANSMIT         = ARM_SAI_CONFIGURE_TX,  ///< I2S transmit
  SL_I2S_RECEIVE          = ARM_SAI_CONFIGURE_RX,  ///< I2S receive
  SL_I2S_TRANSMIT_CONTROL = ARM_SAI_CONTROL_TX,    ///< I2S receive
  SL_I2S_RECEIVE_CONTROL  = ARM_SAI_CONTROL_RX,    ///< I2S receive
  SL_I2S_SEND_ABORT       = ARM_SAI_ABORT_SEND,    ///< I2S abort transmit
  SL_I2S_RECEIVE_ABORT    = ARM_SAI_ABORT_RECEIVE, ///< I2S abort receive
} sl_i2s_xfer_type_t;

/// @brief Enumeration for I2S transfer data size
typedef enum {
  SL_I2S_DATA_SIZE16 = 16, ///< 16 bits
  SL_I2S_DATA_SIZE32 = 32  ///< 32 bits
} sl_i2s_xfer_size_t;

/// @brief Enumeration for I2S audio data resolutions
typedef enum {
  SL_I2S_RESOLUTION_12 = 12, ///< 12 bit resolution
  SL_I2S_RESOLUTION_16 = 16, ///< 16 bit resolution
  SL_I2S_RESOLUTION_20 = 20, ///< 20 bit resolution
  SL_I2S_RESOLUTION_24 = 24, ///< 24 bit resolution
  SL_I2S_RESOLUTION_32 = 32  ///< 32 bit resolution
} sl_i2s_data_resolution_t;

/// @brief Enumeration for I2S sample rates
typedef enum {
  SL_I2S_SAMPLING_RATE_8000   = 8000,  ///< 8kHz
  SL_I2S_SAMPLING_RATE_11025  = 11025, ///< 11.025kHz
  SL_I2S_SAMPLING_RATE_16000  = 16000, ///< 16kHz
  SL_I2S_SAMPLING_RATE_22050  = 22050, ///< 22.05kHz
  SL_I2S_SAMPLING_RATE_24000  = 24000, ///< 24kHz
  SL_I2S_SAMPLING_RATE_32000  = 32000, ///< 32kHz
  SL_I2S_SAMPLING_RATE_44100  = 44100, ///< 44.1kHz
  SL_I2S_SAMPLING_RATE_48000  = 48000, ///< 48kHz
  SL_I2S_SAMPLING_RATE_88200  = 88200, ///< 88.2kHz
  SL_I2S_SAMPLING_RATE_96000  = 96000, ///< 96kHz
  SL_I2S_SAMPLING_RATE_192000 = 192000 ///< 192kHz
} sl_i2s_sampling_rate_t;

/// @brief Structure to hold the version numbers of peripheral API
typedef struct {
  uint8_t release; ///< Release version number
  uint8_t major;   ///< sqa version number
  uint8_t minor;   ///< dev version number
} sl_i2s_version_t;

/// @brief Structure to hold transmit and receive config parameters
typedef struct {
  uint16_t mode;          ///< Primary/Secondary mode
  uint16_t sync;          ///< SYNC/ASYNC mode
  uint16_t protocol;      ///< I2S/PCM (currently only I2S is supported)
  uint16_t resolution;    ///< Audio data resolutions
  uint32_t data_size;     ///< data size
  uint32_t sampling_rate; ///< Audio sampling rate
  uint32_t transfer_type; ///< Tx/Rx
} sl_i2s_xfer_config_t;

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/
/**
 * Initialize the I2S. Pass the address of the pointer for storing the I2S handle,
 * which can be used in future for other function calls.
 * 
 * @pre Pre-conditions:
 *      - \ref none 
 *
 * @param[in] I2S instance , 0 - I2S0
                            1 - I2S1
 * @param[in] i2s_handle Double Pointer to the I2S driver handle
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_STATUS_OK (0x0000) - Success 
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - Invalid null pointer received as argument 
 *         - \ref SL_STATUS_BUSY (0x0004) - Driver is busy
 ******************************************************************************/
sl_status_t sl_si91x_i2s_init(uint32_t i2s_instance, sl_i2s_handle_t *i2s_handle);

/***************************************************************************/
/**
 * Uninitialize I2S peripheral. This will also disable the DMA instance used for I2S transfer.
 * This API needs to be called only if the I2S peripheral is initialized before using sl_si91x_i2s_init(). 
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_i2s_init 
 * 
 * @param[in] i2s_handle Pointer to the I2S driver handle
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_STATUS_OK (0x0000) - Success 
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - Invalid null pointer received as argument
 ******************************************************************************/
sl_status_t sl_si91x_i2s_deinit(sl_i2s_handle_t *i2s_handle);

/***************************************************************************/
/**
 * Change the power mode of I2S, the supported modes are
 *  POWER_OFF - I2S peripheral clocks and I2S DMA channel is disabled.
 *  FULL_POWER - Enable I2S peripheral clocks and configure I2S DMA channel.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_i2s_init 
 * 
 * @param[in] i2s_handle Pointer to the I2S driver handle
 * @param[in] power state, SL_I2S_POWER_OFF/SL_I2S_FULL_POWER
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_STATUS_OK (0x0000) - Success 
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - Invalid null pointer received as argument
 ******************************************************************************/
sl_status_t sl_si91x_i2s_configure_power_mode(sl_i2s_handle_t i2s_handle, sl_i2s_power_state_t state);

/***************************************************************************/
/**
 * Configure transmitter/Receiver parameters for I2S transfer.
 * DMA transmit and receive configurations. 
 * Configuring transfer_type to SL_I2S_TRANSMIT will configure Tx channel and SL_I2S_RECEIVE
 * will configure Rx channel. 
 * receiving data.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_i2s_init 
 *      - \ref sl_si91x_i2s_configure_power_mode 
 * 
 * @param[in] i2s_handle Pointer to the I2S driver handle
 * @param[in] Address of structure which stores transfer parameters
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_STATUS_OK (0x0000) - Success 
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - Invalid null pointer received as argument
 ******************************************************************************/
sl_status_t sl_si91x_i2s_config_transmit_receive(sl_i2s_handle_t i2s_handle, sl_i2s_xfer_config_t *xfer_config);

/***************************************************************************/
/**
 * Configure I2S tx DMA channel descriptors and trigger dma transfer. sl_si91x_i2s_config_transmit_receive()
 * should be called with transfer_type set to SL_I2S_TRANSMIT before sending data.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_i2s_init 
 *      - \ref sl_si91x_i2s_configure_power_mode 
 *      - \ref sl_si91x_i2s_config_transmit_receive 
 * 
 * @param[in] i2s_handle Pointer to the I2S driver handle
 * @param[in] Address of transmit data
 * @param[in] data size
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_STATUS_OK (0x0000) - Success 
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - Invalid null pointer received as argument
 ******************************************************************************/
sl_status_t sl_si91x_i2s_transmit_data(sl_i2s_handle_t i2s_handle, const void *data, uint32_t size);

/***************************************************************************/
/**
 * Configure I2S rx DMA channel descriptors and enable dma channel to recieve data. sl_si91x_i2s_config_transmit_receive()
 * should be called with transfer_type set to SL_I2S_RECEIVE before receiving data.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_i2s_init 
 *      - \ref sl_si91x_i2s_configure_power_mode 
 *      - \ref sl_si91x_i2s_config_transmit_receive 
 * 
 * @param[in] i2s_handle Pointer to the I2S driver handle
 * @param[in] Address of receive data
 * @param[in] data size
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_STATUS_OK (0x0000) - Success 
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - Invalid null pointer received as argument
 ******************************************************************************/
sl_status_t sl_si91x_i2s_receive_data(sl_i2s_handle_t i2s_handle, const void *data, uint32_t size);

/***************************************************************************/
/**
 * Register the user callback function. Transfer complete event can be notified using this callback.
 * Callbacks should be registered before starting I2S transfer.
 * 
 * @pre Pre-conditions:
 *      - \ref none 
 * 
 * @param[in] i2s_handle Pointer to the I2S driver handle
 * @param[in] callback_event Pointer to the function which needs to be called at the time of interrupt
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_STATUS_OK (0x0000) - Success 
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 *         - \ref SL_STATUS_BUSY (0x0004) - Driver is busy
 ******************************************************************************/
sl_status_t sl_si91x_i2s_register_event_callback(sl_i2s_handle_t i2s_handle, sl_i2s_signal_event_t callback_event);

/***************************************************************************/
/**
 * Un-register the user callback function. This function can be used only if callbacks are
 * registered before using sl_si91x_i2s_register_event_callback()
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_i2s_register_event_callback 
 * 
 * @param[in] i2s_handle Pointer to the I2S driver handle
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid \n
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - Invalid null pointer received as argument
 ******************************************************************************/
sl_status_t sl_si91x_i2s_unregister_event_callback(sl_i2s_handle_t i2s_handle);

/***************************************************************************/
/**
 * Get the transmit data count of I2S. If an active I2S transfer is started, this function
 * can be used to get number of data bytes transfered.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_i2s_init 
 *      - \ref sl_si91x_i2s_configure_power_mode 
 *      - \ref sl_si91x_i2s_config_transmit_receive 
 *      - \ref sl_si91x_i2s_transmit_data 
 * 
 * @param[in] i2s_handle Pointer to the I2S driver handle 
 * @return uint32_t value of the tx data count
 ******************************************************************************/
uint32_t sl_si91x_i2s_get_transmit_data_count(sl_i2s_handle_t i2s_handle);

/***************************************************************************/
/**
 * Get the receive data count of I2S. If I2S is configured for receive, this function
 * can be used to get number of data bytes received.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_i2s_init 
 *      - \ref sl_si91x_i2s_configure_power_mode 
 *      - \ref sl_si91x_i2s_config_transmit_receive 
 *      - \ref sl_si91x_i2s_receive_data 
 * 
 * @param[in] i2s_handle Pointer to the I2S driver handle 
 * @return uint32_t value of the rx data count
 ******************************************************************************/
uint32_t sl_si91x_i2s_get_receive_data_count(sl_i2s_handle_t i2s_handle);

/***************************************************************************/
/**
 * Get the release, sqa, and dev version of I2S.
 * 
 * @pre Pre-conditions:
 *      - \ref none 
 * 
 * @param[in] none
 * @return (sl_i2s_version_t) type structure
 ******************************************************************************/
sl_i2s_version_t sl_si91x_i2s_get_version(void);

/***************************************************************************/
/**
 *  Get following transfer status of I2S,
 * 1. Frame error
 * 2. Rx busy
 * 3. Rx overflow
 * 4. Tx busy
 * 5. Tx underflow
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_i2s_init 
 *      - \ref sl_si91x_i2s_configure_power_mode 
 *      - \ref sl_si91x_i2s_config_transmit_receive 
 * 
 * @param[in] i2s_handle Pointer to the I2S driver handle
 * @return (sl_i2s_status_t) type structure
 ******************************************************************************/
sl_i2s_status_t sl_si91x_i2s_get_status(sl_i2s_handle_t i2s_handle);

/***************************************************************************/
/**
 * Abort I2S Tx/Rx operations. Disable I2S clocks and DMA channel.
 * This function need to be called after completion of I2S transmit/receive.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_i2s_init 
 *      - \ref sl_si91x_i2s_configure_power_mode 
 *      - \ref sl_si91x_i2s_config_transmit_receive 
 *      - \ref sl_si91x_i2s_transmit_data/sl_si91x_i2s_receive_data 
 * 
 * @param[in] i2s_handle Pointer to the I2S driver handle
 * @param[in] abort type, ARM_SAI_ABORT_SEND/ARM_SAI_ABORT_RECEIVE 
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_STATUS_OK (0x0000) - Success 
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - Invalid null pointer received as argument
 ******************************************************************************/
sl_status_t sl_si91x_i2s_end_transfer(sl_i2s_handle_t i2s_handle, sl_i2s_xfer_type_t abort_type);

/** @} (end addtogroup I2S) */

#ifdef __cplusplus
}
#endif

#endif /* SL_SI91X_I2S_H */
