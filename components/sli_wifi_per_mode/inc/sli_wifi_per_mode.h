/***************************************************************************/ /**
 * @file sli_wifi_per_mode.h
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef SLI_WIFI_PER_MODE_H
#define SLI_WIFI_PER_MODE_H
#include "sl_status.h"
#include "sl_wifi_constants.h"
#include "sl_wifi_types.h"
#include "sli_wifi.h"

#ifndef SLI_WIFI_PER_TRANSMIT_LOOPBACK_RESPONSE_SIZE
#define SLI_WIFI_PER_TRANSMIT_LOOPBACK_RESPONSE_SIZE 1624
#endif

#ifndef SLI_WIFI_PER_TRANSMIT_LOOPBACK_REQUEST_SIZE
#define SLI_WIFI_PER_TRANSMIT_LOOPBACK_REQUEST_SIZE 1608
#endif

#ifndef SLI_WIFI_PER_TX_DAC_REQUEST_DATA_SIZE
#define SLI_WIFI_PER_TX_DAC_REQUEST_DATA_SIZE 403
#endif

#define SLI_WIFI_TRANSMIT_LOOPBACK_DATA_RESPONSE_SIZE SLI_WIFI_PER_TRANSMIT_LOOPBACK_RESPONSE_SIZE
#define SLI_WIFI_TRANSMIT_LOOPBACK_REQUEST_SIZE       SLI_WIFI_PER_TRANSMIT_LOOPBACK_REQUEST_SIZE
#define SLI_WIFI_TX_DAC_DATA_SIZE                     SLI_WIFI_PER_TX_DAC_REQUEST_DATA_SIZE
#define SLI_WIFI_LUT_GAIN                             96   ///< Default LUT gain value for DPD
#define SLI_FRAME_DESC_SIZE                           16   ///< Frame descriptor size
#define SLI_WIFI_FREQ_PLANNING_PARAMS_COUNT           10   ///< Frequency planning parameters array size
#define SLI_WIFI_FREQ_PLANNING_CONFIG_COUNT           4    ///< Frequency planning configuration array size
#define SLI_WIFI_DIG_CLK_PARAMS_COUNT                 4    ///< Digital clock parameters array size
#define SLI_WIFI_DIG_FREQ_PARAMS_COUNT                4    ///< Digital frequency parameters array size
#define SLI_WIFI_CALIBRATION_DATA_COUNT               4    ///< TX/RX calibration data array size
#define SLI_WIFI_SET_SYTH_CONFIG_COUNT                4    ///< Set syth config configs array size
#define SLI_WIFI_READ_HMATRIX_CONFIG_COUNT            4    ///< Read H-matrix configs array size
#define SLI_WIFI_QUERY_COMMAND_CONFIG_COUNT           4    ///< Query command configs array size
#define SLI_WIFI_STATICS_COMMAND_CONFIG_COUNT         64   ///< Statics command configs array size
#define SLI_WIFI_READ_HMATRIX_RESPONSE_DATA_SIZE      1024 ///< Read H-matrix response data buffer size
#define SLI_WIFI_QUERY_COMMAND_RESPONSE_DATA_SIZE     1024 ///< Query command response data buffer size
#define SLI_WIFI_STATICS_COMMAND_RESPONSE_DATA_SIZE   1024 ///< Statics command response data buffer size
#define SLI_WIFI_GET_TX_POWER_LOG_RESERVED_COUNT      128 ///< Reserved array size in sli_wifi_response_get_tx_power_log_t

/* MLO config */
#define MLO_CONFIG_CMD   87
#define MLO_SET_LINK_CMD 88
#define MLO_ADD_LINK_CMD 89
#define MLO_ARGC_CNT     2
#define MAC_HDR_LEN      6
#define NUM_OF_LINKS     2

/**
 * @enum sli_wifi_rail_cmd_internal_subtype_t
 * @brief Enumeration of Wi-Fi RAIL command subtypes.
 *
 * This enumeration defines the various subtypes of Wi-Fi RAIL commands used for specific operations.
 *
 * @details
 * Each subtype corresponds to a specific Wi-Fi operation or request, such as measuring noise density, 
 * starting or stopping ADC captures, configuring DPD, or resetting PER statistics.
 *
 * @var SLI_WIFI_SUBTYPE_SET_TX_POWER
 *  Subtype for setting raw transmission power.
 *
 * @var SLI_WIFI_SUBTYPE_MEAS_NOISE_DENS
 *   Subtype for measuring noise density.
 *
 * @var SLI_WIFI_SUBTYPE_RUN_CAPTURE
 *   Subtype for starting or stopping ADC run capture.
 *
 * @var SLI_WIFI_SUBTYPE_ENABLE_DCM
 *   Subtype for enabling DCM (Dual Carrier Modulation).
 *
 * @var SLI_WIFI_SUBTYPE_TX_STOP_DUT
 *   Subtype for stopping the Device Under Test (DUT) transmission.
 *
 * @var SLI_WIFI_SUBTYPE_CONFIG_DPD
 *   Subtype for configuring Digital Pre-Distortion (DPD) settings.
 *
 * @var SLI_WIFI_SUBTYPE_DPD_LUT_GAIN
 *   Subtype for reading back DPD LUT (Look-Up Table) gain values.
 *
 * @var SLI_WIFI_SUBTYPE_SET_CHANNEL
 *   Subtype for setting the channel operations.
 *
 * @var SLI_WIFI_SUBTYPE_PER_RESET
 *   Subtype for resetting Packet Error Rate (PER) statistics.
 *
 * @var SLI_WIFI_SUBTYPE_PER_GET_RESULTS
 *   Subtype for retrieving Packet Error Rate (PER) results.
 *
 * @var SLI_WIFI_SUBTYPE_POPULATE_DPD_LUT
 *   Subtype for populating DPD LUT (Look-Up Table) with gain values.
 *
 * @var SLI_WIFI_SUBTYPE_TX_IR_CALIBRATION
 *   Subtype for transmitting IR (Interference Reduction) calibration.
 *
 * @var SLI_WIFI_SUBTYPE_GET_TX_IR_CALIBRATION
 *  Subtype for getting IR (Interference Reduction) calibration values.
 *
 * @var SLI_WIFI_SUBTYPE_SET_RX_IR_CALIBRATION
 *  Subtype for receiving IR (Interference Reduction) calibration.
 *
 * @var SLI_WIFI_SUBTYPE_GET_RX_IR_CALIBRATION
 * Subtype for getting IR (Interference Reduction) calibration values.
 *
 * @var SLI_WIFI_SUBTYPE_GET_TX_POWER
 * Subtype for getting transmission power.
 *
 * @var SLI_WIFI_SUBTYPE_READ_AUX_ADC
 * Subtype for reading Auxiliary ADC (Analog-to-Digital Converter) values.
 *
 * @var SLI_WIFI_SUBTYPE_TX_DAC_PLAY
 * Subtype for transmitting DAC (Digital-to-Analog Converter) play.
 *
 * @var SLI_WIFI_SUBTYPE_CONFIG_MLO
 * Subtype for configuring MLO (Multi-Link Operation).
 *
 * @var SLI_WIFI_SUBTYPE_MLO_SET_LINK
 * Subtype for setting MLO link parameters.
 *
 * @var SLI_WIFI_SUBTYPE_MLO_ADD_REM_LINK
 * Subtype for adding or removing MLO link.
 *
 * @var SLI_WIFI_SUBTYPE_LOAD_PHY
 * Subtype for loading PHY (Physical Layer) configuration.
 */
typedef enum {
  SLI_WIFI_SUBTYPE_SET_TX_POWER             = 0,  ///< Set raw transmission power.
  SLI_WIFI_SUBTYPE_MEAS_NOISE_DENS          = 2,  ///< Measure noise density.
  SLI_WIFI_SUBTYPE_RUN_CAPTURE              = 3,  ///< Start or stop ADC run capture.
  SLI_WIFI_SUBTYPE_ENABLE_DCM               = 5,  ///< Enable Dual Carrier Modulation (DCM).
  SLI_WIFI_SUBTYPE_TX_STOP_DUT              = 6,  ///< Stop Device Under Test (DUT) transmission.
  SLI_WIFI_SUBTYPE_CONFIG_DPD               = 7,  ///< Configure Digital Pre-Distortion (DPD) settings.
  SLI_WIFI_SUBTYPE_DPD_LUT_GAIN             = 8,  ///< Read back DPD LUT gain values.
  SLI_WIFI_SUBTYPE_SET_CHANNEL              = 9,  ///< Setting the channel operations.
  SLI_WIFI_SUBTYPE_PER_RESET                = 11, ///< Reset Packet Error Rate (PER) statistics.
  SLI_WIFI_SUBTYPE_PER_GET_RESULTS          = 12, ///< Retrieve Packet Error Rate (PER) results.
  SLI_WIFI_SUBTYPE_POPULATE_DPD_LUT         = 13, ///< Populate DPD LUT (Look-Up Table) with gain values.
  SLI_WIFI_SUBTYPE_TX_IR_CALIBRATION        = 14, ///< Transmit IR calibration.
  SLI_WIFI_SUBTYPE_GET_TX_IR_CALIBRATION    = 15, ///< Get IR calibration values.
  SLI_WIFI_SUBTYPE_SET_RX_IR_CALIBRATION    = 16, ///< Receive IR calibration.
  SLI_WIFI_SUBTYPE_GET_RX_IR_CALIBRATION    = 17, ///< Get IR calibration values.
  SLI_WIFI_SUBTYPE_GET_TX_POWER             = 18, ///< Set transmission power.
  SLI_WIFI_SUBTYPE_READ_AUX_ADC             = 19, ///< Read Auxiliary ADC values.
  SLI_WIFI_SUBTYPE_TX_DAC_PLAY              = 20, ///< Transmit DAC (Digital-to-Analog Converter) play.
  SLI_WIFI_SUBTYPE_CONFIG_MLO               = 23, ///< Configure MLO (Multi-Link Operation).
  SLI_WIFI_SUBTYPE_MLO_SET_LINK             = 24, ///< Set MLO link parameters.
  SLI_WIFI_SUBTYPE_MLO_ADD_REM_LINK         = 25, ///< Add or remove MLO link.
  SLI_WIFI_SUBTYPE_LOAD_PHY                 = 26, ///< Load PHY (Physical Layer) configuration.
  SLI_WIFI_SUBTYPE_SET_SYTH_CONFIG          = 27, ///< Set SYTH (Synthetic) configuration.
  SLI_WIFI_SUBTYPE_READ_HMATRIX             = 28, ///< Read H-Matrix configuration.
  SLI_WIFI_SUBTYPE_QUERY_COMMAND            = 29, ///< Query command.
  SLI_WIFI_SUBTYPE_STATICS_COMMAND          = 30, ///< Statics command.
  SLI_WIFI_SUBTYPE_CONFIG_FREQ_PLANNING     = 31, ///< Configure frequency planning parameters.
  SLI_WIFI_SUBTYPE_GET_FREQ_PLANNING_PARAMS = 32, ///< Get frequency planning parameters.
  SLI_WIFI_SUBTYPE_SET_GET_DIG_CLK_DIV      = 33, ///< Set/Get digital clock divider and enable/disable clock blanking.
  SLI_WIFI_SUBTYPE_ENABLE_DISABLE_DIG_FREQ  = 34, ///< Enable/Disable digital frequency plan.
  SLI_WIFI_SUBTYPE_TRANSMIT_LOOPBACK        = 35, ///< Transmit loopback.
} sli_wifi_rail_cmd_internal_subtype_t;

/**
 * @struct sli_wifi_request_per_reset_t
 * @brief Structure representing the request to reset the PER (Packet Error Rate) statistics in Wi-Fi.
 *
 * This structure is used to send a request to reset the PER statistics.
 */
typedef struct {
  sli_wifi_frame_body_type_t frame_body_type; ///< PER reset request structure.
} sli_wifi_request_per_reset_t;

/**
 * @struct sli_wifi_request_adc_run_capture_t
 * @brief Structure representing the request to start or stop a capture operation in Wi-Fi.
 *
 * This structure is used to configure and control capture operations.
 */
typedef struct {
  sli_wifi_frame_body_type_t frame_body_type;
  uint8_t input_source;
  uint8_t trigger_start_sel;
  uint16_t no_of_samples_before_trigger;
  uint16_t no_of_samples;
  uint8_t capture_mode;
  uint8_t trigger_type;
  uint8_t stop_mode;
  uint32_t flags;
} sli_wifi_request_adc_run_capture_t; ///< ADC run capture request structure.

typedef struct {
  uint8_t source;
  uint8_t trigger_select;
  uint8_t clk_sel;
  uint16_t num_of_samples_bef_trigger;
  uint16_t num_samples;
  uint8_t capture_mode;
  uint8_t trigger_type;
  uint8_t stop_mode;
  uint32_t flags;
} sli_wifi_request_adc_params_t;

/**
 * @struct sli_wifi_request_meansnoise_density_t
 * @brief Structure representing the request to measure noise density in Wi-Fi.
 *
 * This structure is used to send a request to measure the noise density.
 */
typedef struct {
  sli_wifi_frame_body_type_t frame_body_type; ///< Means noise density request structure.
} sli_wifi_request_measnoise_density_t;

/**
 * @struct sli_wifi_config_dpd_t
 * @brief Structure representing the configuration for Digital Pre-Distortion (DPD) in Wi-Fi.
 *
 * This structure contains parameters to configure and control DPD settings, such as LUT index and flags.
 *
 * @details
 * - DPD is used to improve the linearity of power amplifiers in Wi-Fi systems.
 * - The structure allows resetting the LUT, setting the LUT index, and enabling/disabling DPD.
 */
typedef struct {
  uint32_t flags; ///< Flags to control DPD settings (e.g., reset LUT, set LUT index, enable DPD).
} sli_wifi_config_dpd_t;

/**
 * @struct sli_wifi_request_config_dpd_t
 * @brief Structure representing the configuration for Digital Pre-Distortion (DPD) in Wi-Fi.
 *
 * This structure contains parameters to configure and control DPD settings, such as LUT index and flags.
 *
 * @details
 * - DPD is used to improve the linearity of power amplifiers in Wi-Fi systems.
 * - The structure allows resetting the LUT, setting the LUT index, and enabling/disabling DPD.
 */
typedef struct {
  sli_wifi_frame_body_type_t frame_body_type; ///< DPD configuration request structure.
  sli_wifi_config_dpd_t config_dpd;           ///< DPD configuration structure.
} sli_wifi_request_config_dpd_t;

/**
 * @struct sli_wifi_dpd_data_config_t
 * @brief Structure representing the configuration for Digital Pre-Distortion (DPD) in Wi-Fi.
 * 
 * This structure contains parameters to configure and control DPD settings, such as LUT index and LUT values.
 * 
 * @details
 * - DPD is used to improve the linearity of power amplifiers in Wi-Fi systems.
 * - The structure allows specifying the loopback gain and provides a buffer for LUT (Look-Up Table) values.
 */
typedef struct {
  uint32_t flags;                  ///< Flags for DPD configuration.
  uint32_t lut[SLI_WIFI_LUT_GAIN]; ///< Look-Up Table (LUT) values for DPD configuration.
} sli_wifi_dpd_data_config_t;

/**
 * @struct sli_wifi_request_dpd_response_t
 * @brief Structure representing the request to reset Digital Pre-Distortion (DPD) settings in Wi-Fi.
 *
 * This structure contains parameters to configure and reset the DPD settings, including loopback gain and LUT (Look-Up Table) values.
 *
 * @details
 * - DPD reset is used to recalibrate the power amplifier for improved linearity.
 * - The structure allows specifying the loopback gain and provides a buffer for LUT values.
 */
typedef struct {
  sli_wifi_frame_body_type_t frame_body_type; ///< DPD response structure.
  sli_wifi_dpd_data_config_t dpd_config;      ///< DPD response structure.
} sli_wifi_request_dpd_response_t;

/**
 * @struct sl_wifi_channel_t
 * @brief Wi-Fi channel configuration.
 */
typedef struct {
  uint16_t channel;              ///< Channel number
  uint16_t power;                ///< Power level in dBm
  sl_wifi_band_t band;           ///< Wi-Fi radio band of type @ref sl_wifi_band_t
  sl_wifi_bandwidth_t bandwidth; ///< Channel bandwidth of type @ref sl_wifi_bandwidth_t
  uint32_t reserved[4];          ///< Reserved for future use
} sli_wifi_channel_config_t;

/**
 * @struct sli_wifi_request_channel_t
 * @brief Structure representing the request to set the channel in Wi-Fi.
 *
 * This structure contains parameters to set the channel number for the request.
 */
typedef struct {
  sli_wifi_frame_body_type_t frame_body_type;
  sli_wifi_channel_config_t channel; ///< set the channel number for the request
} sli_wifi_request_channel_t;

typedef struct {
  uint32_t calibration_flags;
  // Bit 0 → Set TxDCOC
  // Bit 1 → Set IR cal compensations
  // Bit 2 → Trigger TX DCOC cal
  // Bit 3 → Trigger IR Cal
  uint32_t TXDCOC[SLI_WIFI_CALIBRATION_DATA_COUNT];
  uint32_t TXIRCal[SLI_WIFI_CALIBRATION_DATA_COUNT];
} sli_wifi_tx_cal_t;

typedef struct {
  sli_wifi_frame_body_type_t frame_body_type;
  sli_wifi_tx_cal_t TxIRCal_config;
} sli_wifi_request_tx_ir_calibration_t;

typedef struct {
  uint32_t calibration_flags;
  // Bit 0 → Set rxDCOC
  // Bit 1 → Set RX IR cal compensations
  // Bit 2 → Trigger RX DCOC cal
  // Bit 3 → Trigger RX IR Cal
  uint32_t RXDCOC[SLI_WIFI_CALIBRATION_DATA_COUNT];
  uint32_t RXIRCal[SLI_WIFI_CALIBRATION_DATA_COUNT];
} sli_wifi_rx_cal_t;

typedef struct {
  sli_wifi_frame_body_type_t frame_body_type;
  sli_wifi_rx_cal_t RxIRCal_config;
} sli_wifi_request_rx_ir_calibration_t;

typedef struct {
  sli_wifi_frame_body_type_t frame_body_type;
  uint32_t TxPower_config;
} sli_wifi_request_raw_tx_power_t;

typedef struct {
  sli_wifi_frame_body_type_t frame_body_type;
} sli_wifi_request_get_tx_power_t;

typedef struct {
  uint32_t flags; /*Valids for the presence of below fields*/
  uint32_t reserved[SLI_WIFI_GET_TX_POWER_LOG_RESERVED_COUNT];
} sli_wifi_response_get_tx_power_log_t;

typedef struct {
  sli_wifi_frame_body_type_t frame_body_type;
  uint32_t AUX_ADC_Type;
  uint32_t flags;
} sli_wifi_request_aux_adc_t;

typedef struct {
  uint32_t flags; /*Valids for the presence of below fields*/
  uint32_t reserved[4];
} sli_wifi_response_get_aux_adc_data_t;

typedef struct {
  sli_wifi_frame_body_type_t frame_body_type;
  uint32_t enable;                              ///< Enable/disable flag for TX DAC
  uint32_t size;                                ///< Size of DAC data in bytes
  uint32_t dac_data[SLI_WIFI_TX_DAC_DATA_SIZE]; ///< DAC data array
} sli_wifi_request_tx_dac_t;

typedef struct {
  uint8_t mlo_dut_addr[MAC_HDR_LEN];
  uint8_t mlo_peer_addr[MAC_HDR_LEN];
  uint8_t chan_num;
  uint8_t band;
  uint8_t ch_width;
  uint8_t pwr_host;
} mlo_link_params_t;

typedef struct {
  uint8_t num_of_links;
  uint8_t mld_peer_addr[MAC_HDR_LEN];
  mlo_link_params_t link[NUM_OF_LINKS];
} mlo_config_t;

typedef struct {
  sli_wifi_frame_body_type_t frame_body_type;
  mlo_config_t mlo_config;
} sli_wifi_request_mlo_configuration_t;

typedef struct {
  uint32_t status;
  uint32_t duration;
} mlo_set_link_resp_t;

typedef struct {
  uint8_t link_id;
  uint8_t mld_mlo_addr;
  uint8_t mld_peer_addr[MAC_HDR_LEN];
} mlo_set_link_t;

typedef struct {
  sli_wifi_frame_body_type_t frame_body_type;
  mlo_set_link_t link;
} sli_wifi_request_mlo_set_link_t;

typedef struct {
  uint8_t add_remove;
  uint8_t link_id;
  uint8_t mld_peer_addr[MAC_HDR_LEN];
  mlo_link_params_t link;
} mlo_add_link_t;

typedef struct {
  sli_wifi_frame_body_type_t frame_body_type;
  mlo_add_link_t mlo_add_rem_link;
} sli_wifi_request_mlo_add_rem_link_t;

typedef struct {
  sli_wifi_frame_body_type_t frame_body_type;
} sli_wifi_request_loadphy_t;

typedef struct {
  uint32_t configs[SLI_WIFI_SET_SYTH_CONFIG_COUNT];
} sli_wifi_set_syth_config_t;

typedef struct {
  uint32_t configs[SLI_WIFI_READ_HMATRIX_CONFIG_COUNT];
} sli_wifi_read_hmatrix_t;

typedef struct {
  uint8_t data[SLI_WIFI_READ_HMATRIX_RESPONSE_DATA_SIZE];
} sli_wifi_read_hmatrix_response_t;

typedef struct {
  uint32_t configs[SLI_WIFI_QUERY_COMMAND_CONFIG_COUNT];
} sli_wifi_query_command_t;

typedef struct {
  uint8_t data[SLI_WIFI_QUERY_COMMAND_RESPONSE_DATA_SIZE];
} sli_wifi_query_command_response_t;

typedef struct {
  uint32_t configs[SLI_WIFI_STATICS_COMMAND_CONFIG_COUNT];
} sli_wifi_statics_command_t;

typedef struct {
  uint8_t data[SLI_WIFI_STATICS_COMMAND_RESPONSE_DATA_SIZE];
} sli_wifi_statics_command_response_t;

typedef struct {
  sli_wifi_frame_body_type_t frame_body_type;
  sli_wifi_set_syth_config_t syth_config;
} sli_wifi_set_syth_config_request_t;

typedef struct {
  sli_wifi_frame_body_type_t frame_body_type;
  sli_wifi_read_hmatrix_t read_hmatrix;
} sli_wifi_read_hmatrix_request_t;

typedef struct {
  sli_wifi_frame_body_type_t frame_body_type;
  sli_wifi_query_command_t query_command;
} sli_wifi_query_command_request_t;

typedef struct {
  sli_wifi_frame_body_type_t frame_body_type;
  sli_wifi_statics_command_t statics_command;
} sli_wifi_statics_command_request_t;

typedef struct {
  uint32_t enable;
  uint32_t flags;
  uint32_t size;
  uint8_t dac_data[SLI_WIFI_TRANSMIT_LOOPBACK_REQUEST_SIZE];
} sli_wifi_transmit_loopback_config_t;

typedef struct {
  sli_wifi_frame_body_type_t frame_body_type;
  sli_wifi_transmit_loopback_config_t config;
} sli_wifi_transmit_loopback_request_t;

typedef struct {
  uint8_t data[SLI_WIFI_TRANSMIT_LOOPBACK_DATA_RESPONSE_SIZE];
} sli_wifi_transmit_loopback_response_t;

/**
 * @struct sli_wifi_freq_planning_params_t
 * @brief Structure representing frequency planning configuration parameters.
 *
 * This structure contains the parameters for configuring frequency planning.
 */
typedef struct {
  uint32_t params[SLI_WIFI_FREQ_PLANNING_PARAMS_COUNT]; ///< Frequency planning parameters array
} sli_wifi_freq_planning_params_t;

/**
 * @struct sli_wifi_request_config_freq_planning_t
 * @brief Structure representing the request to configure frequency planning parameters.
 *
 * This structure is used to send frequency planning configuration to the device.
 */
typedef struct {
  sli_wifi_frame_body_type_t frame_body_type;       ///< Frame body type
  sli_wifi_freq_planning_params_t freq_plan_params; ///< Frequency planning parameters
} sli_wifi_request_config_freq_planning_t;

/**
 * @struct sli_wifi_freq_planning_configs_t
 * @brief Structure representing frequency planning configuration data.
 *
 * This structure contains the configuration data for frequency planning.
 */
typedef struct {
  uint32_t configs[SLI_WIFI_FREQ_PLANNING_CONFIG_COUNT]; ///< Frequency planning configuration array
} sli_wifi_freq_planning_configs_t;

/**
 * @struct sli_wifi_request_get_freq_planning_t
 * @brief Structure representing the request to get frequency planning parameters.
 *
 * This structure is used to request frequency planning configuration from the device.
 */
typedef struct {
  sli_wifi_frame_body_type_t frame_body_type;     ///< Frame body type
  sli_wifi_freq_planning_configs_t input_configs; ///< Input configuration parameters
} sli_wifi_request_get_freq_planning_t;

/**
 * @struct sli_wifi_response_get_freq_planning_t
 * @brief Structure representing the response for get frequency planning parameters.
 *
 * This structure contains the frequency planning parameters returned from the device.
 */
typedef struct {
  uint32_t configs[SLI_WIFI_FREQ_PLANNING_CONFIG_COUNT]; ///< Response frequency planning configuration array
} sli_wifi_response_get_freq_planning_t;

/**
 * @struct sli_wifi_dig_clk_params_t
 * @brief Structure representing digital clock divider parameters.
 *
 * This structure contains parameters for setting/getting digital clock divider and clock blanking.
 */
typedef struct {
  uint32_t params[SLI_WIFI_DIG_CLK_PARAMS_COUNT]; ///< Digital clock parameters array
} sli_wifi_dig_clk_params_t;

/**
 * @struct sli_wifi_request_dig_clk_div_t
 * @brief Structure representing the request to set/get digital clock divider.
 *
 * This structure is used to configure digital clock divider and enable/disable clock blanking.
 */
typedef struct {
  sli_wifi_frame_body_type_t frame_body_type; ///< Frame body type
  sli_wifi_dig_clk_params_t dig_clk_params;   ///< Digital clock parameters
} sli_wifi_request_dig_clk_div_t;

/**
 * @struct sli_wifi_dig_freq_params_t
 * @brief Structure representing digital frequency plan parameters.
 *
 * This structure contains parameters for enabling/disabling digital frequency plan.
 */
typedef struct {
  uint32_t params[SLI_WIFI_DIG_FREQ_PARAMS_COUNT]; ///< Digital frequency parameters array
} sli_wifi_dig_freq_params_t;

/**
 * @struct sli_wifi_request_dig_freq_plan_t
 * @brief Structure representing the request to enable/disable digital frequency plan.
 *
 * This structure is used to enable or disable digital frequency plan.
 */
typedef struct {
  sli_wifi_frame_body_type_t frame_body_type; ///< Frame body type
  sli_wifi_dig_freq_params_t dig_freq_params; ///< Digital frequency parameters
} sli_wifi_request_dig_freq_plan_t;

sl_status_t sli_wifi_populate_dpd_lut(sl_wifi_interface_t interface, sli_wifi_dpd_data_config_t config_dpd);
sl_status_t sli_wifi_txir_calib(sl_wifi_interface_t interface,
                                sli_wifi_tx_cal_t tx_cal_conf,
                                sli_wifi_tx_cal_t *tx_cal_data);
sl_status_t sli_wifi_get_txir_dcoc_calibration(sl_wifi_interface_t interface, sli_wifi_tx_cal_t *tx_ir_dcoc_calib);
sl_status_t sli_wifi_rxir_calib(sl_wifi_interface_t interface,
                                sli_wifi_rx_cal_t rx_cal_conf,
                                sli_wifi_rx_cal_t *rx_cal_data);
sl_status_t sli_wifi_get_rxircal_data(sl_wifi_interface_t interface, sli_wifi_rx_cal_t *rx_cal_data);
sl_status_t sli_wifi_set_raw_tx_power(uint32_t tx_power_config);
sl_status_t sli_wifi_get_tx_power(sli_wifi_response_get_tx_power_log_t *get_tx_power);
sl_status_t sli_wifi_read_aux_adc(sl_wifi_interface_t interface,
                                  sli_wifi_response_get_aux_adc_data_t *aux_adc,
                                  uint32_t aux_adc_type,
                                  uint32_t flag);
sl_status_t sli_wifi_tx_dac(sl_wifi_interface_t interface, uint32_t enable, uint32_t *tx_dac_data, uint32_t size);
sl_status_t sli_wifi_config_mlo(sl_wifi_interface_t interface, mlo_config_t mlo_config);
sl_status_t sli_wifi_mlo_set_link(sl_wifi_interface_t interface,
                                  mlo_set_link_resp_t *mlo_set_link,
                                  mlo_set_link_t link);
sl_status_t sli_wifi_mlo_add_rem_link(sl_wifi_interface_t interface, mlo_add_link_t mlo_add_rem_link);
sl_status_t sli_wifi_per_reset_statistics(sl_wifi_interface_t interface);
sl_status_t sli_wifi_measure_noise_density(sl_wifi_interface_t interface, void *data_buffer);
sl_status_t sli_wifi_configure_chipscope_capture(sl_wifi_interface_t interface,
                                                 void *data_buffer,
                                                 sli_wifi_request_adc_params_t *adc_run_capture);
sl_status_t sli_wifi_config_dpd(sl_wifi_interface_t interface, sli_wifi_config_dpd_t config_dpd);
sl_status_t sli_wifi_readback_dpd_lut_gain(sl_wifi_interface_t interface,
                                           sli_wifi_config_dpd_t config_dpd,
                                           sli_wifi_dpd_data_config_t *readback_dpd_gain);
sl_status_t sli_wifi_set_channel_config(sl_wifi_interface_t interface, sli_wifi_channel_config_t channel);
sl_status_t sli_wifi_loadphy(sl_wifi_interface_t interface);
sl_status_t sli_wifi_configure_frequency_planning(sl_wifi_interface_t interface,
                                                  sli_wifi_freq_planning_params_t freq_plan_params);
sl_status_t sli_wifi_get_frequency_planning_params(sl_wifi_interface_t interface,
                                                   sli_wifi_freq_planning_configs_t input_configs,
                                                   sli_wifi_response_get_freq_planning_t *freq_plan_response);
sl_status_t sli_wifi_set_get_dig_clk_div(sl_wifi_interface_t interface, sli_wifi_dig_clk_params_t dig_clk_params);
sl_status_t sli_wifi_enable_disable_dig_freq_plan(sl_wifi_interface_t interface,
                                                  sli_wifi_dig_freq_params_t dig_freq_params);

sl_status_t sli_wifi_set_syth_config(sl_wifi_interface_t interface, sli_wifi_set_syth_config_t *syth_config);
sl_status_t sli_wifi_read_hmatrix(sl_wifi_interface_t interface,
                                  sli_wifi_read_hmatrix_t *read_hmatrix,
                                  sli_wifi_read_hmatrix_response_t *read_hmatrix_response);
sl_status_t sli_wifi_query_command(sl_wifi_interface_t interface,
                                   sli_wifi_query_command_t *query_command,
                                   sli_wifi_query_command_response_t *query_command_response);
sl_status_t sli_wifi_statics_command(sl_wifi_interface_t interface,
                                     sli_wifi_statics_command_t *statics_command,
                                     sli_wifi_statics_command_response_t *statics_command_response);

// Taking request directly instead of config to avoid extra allocation as buffer could be huge
sl_status_t sli_wifi_transmit_loopback(sl_wifi_interface_t interface,
                                       sli_wifi_transmit_loopback_request_t *request,
                                       sli_wifi_transmit_loopback_response_t *response);
#endif // SLI_WIFI_PER_MODE_H
