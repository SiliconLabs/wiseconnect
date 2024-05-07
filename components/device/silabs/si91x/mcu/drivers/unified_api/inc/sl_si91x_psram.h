/*******************************************************************************
 * @file  sl_si91x_psram.h
 * @brief PSRAM Driver API implementation
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
#ifndef __SL_SI91X_PSRAM_H_
#define __SL_SI91X_PSRAM_H_

#include <stdbool.h>
#include <stdint.h>
#include "rsi_qspi_proto.h"

/***************************************************************************/ /**
 * @addtogroup PSRAM PSRAM Driver
 * @ingroup SI91X_PERIPHERAL_APIS
 * @{
 *
 ******************************************************************************/

/*******************************************************************************
 ******************************   DEFINES   ************************************
 ******************************************************************************/

/* PSRAM commands */
#define PSRAM_READ_ID        (0x9F) ///< Read ID command
#define PSRAM_ENTER_QPI      (0x35) ///< Enter QPI interface mode command
#define PSRAM_EXIT_QPI       (0xF5) ///< Exit QPI interface mode command
#define PSRAM_RESET_EN       (0x66) ///< Reset Enable command
#define PSRAM_RESET          (0x99) ///< Reset command
#define PSRAM_BURST_LEN      (0xC0) ///< Burst Length Toggle command
#define PSRAM_MODE_REG_READ  (0xB5) ///< Mode Register Read command
#define PSRAM_MODE_REG_WRITE (0xB1) ///< Mode Register Write command
#define PSRAM_HALF_SLEEP     (0xC0) ///< Sleep Entry command

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#define PSRAM_READ_WRITE_PAGE_SIZE (16) ///< PSRAM Read Write Page Size

/* Types of Read Write */
#define NORMAL_RW 1
#define FAST_RW   2
#define QUAD_RW   3

#define MHz_(a)  (a * 1000000)
#define Mbit_(a) (a * 1000000)

#define D_CACHE_ENABLE

#ifdef D_CACHE_ENABLE
#define M4SS_DCACHE_BASE_ADDR   (0x44040000)
#define DCACHE_REG_CTRL         (*((uint32_t volatile *)(M4SS_DCACHE_BASE_ADDR + 0x010)))
#define DCACHE_REG_MAINT_STATUS (*((uint32_t volatile *)(M4SS_DCACHE_BASE_ADDR + 0x028)))
#endif

/** @endcond */

/* PSRAM Timings */
#define tXPHS_US (12)  ///< Sleep Exit chip select low pulse width in us
#define tXHS_US  (160) ///< Sleep Exit chip select low to CLK setup in us
#define tHS_US   (8)   ///< Minimum sleep duration in us

/*******************************************************************************
 *****************************   DATA TYPES   *********************************
 ******************************************************************************/

/// PSRAM return error code
typedef enum {
  PSRAM_SUCCESS,                ///< No error.
  PSRAM_FAILURE,                ///< Generic error
  PSRAM_UNKNOWN,                ///< Unknown request
  PSRAM_UNKNOWN_DEVICE,         ///< Unknown PSRAM Device
  PSRAM_CLOCK_INIT_FAILURE,     ///< Clock init failure
  PSRAM_NOT_INITIALIZED,        ///< PSRAM not initialized
  PSRAM_SUPPORTED_DEVICE,       ///< PSRAM Device supported
  PSRAM_DEVICE_MISMATCH,        ///< PSRAM Device mismatch
  PSRAM_INVALID_HSIZE,          ///< Invalid size of element
  PSRAM_NULL_ADDRESS,           ///< Null address
  PSRAM_INVALID_ADDRESS_LENGTH, ///< Invalid address length
  PSRAM_AUTO_MODE,              ///< PSRAM Auto mode
  PSRAM_MANUAL_MODE,            ///< PSRAM Manual mode
  PSRAM_UNSUPPORTED_SECURITY,   ///< Unsupported security
  PSRAM_MAX_SEC_SEGMENT_REACH   ///< Maximum secure segment reached
} sl_psram_return_type_t;

#if PSRAM_BURST_LEN_TOGGLE_SUPPORTED
/// Wrap burst size enum
typedef enum {
  _WRAP16, ///< Burst wrap size 16 Bytes
  _WRAP32, ///< Burst wrap size 32 Bytes
  _WRAP64, ///< Burst wrap size 64 Bytes
  _WRAP512 ///< Burst wrap size 512 Bytes
} sl_psram_burst_size_type_t;
#endif

/// PSRAM DMA status enum
typedef enum {
  DMA_NONE, ///< DMA status default
  DMA_DONE, ///< DMA transfer completed
  DMA_FAIL  ///< DMA transfer failed
} sl_psram_dma_status_type_t;

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/// PSRAM Read-Write Type
typedef enum {
  normal_readwrite = 1,
  fast_readwrite,
  quad_readwrite,
} PSRAMRWType;

/// PSRAM supported device list
typedef enum { APS1604M_SQR, APS6404L_SQRH, APS6404L_SQH, MAX_SUPPORTED_PSRAM_DEVICES } PSRAMDevType;

/// PSRAM device initialisation state enum
typedef enum { initialised, uninitialised, unknown } PSRAMStateType;

/** @endcond */

/// Read ID structure
typedef struct {
  uint8_t MFID;
  uint8_t KGD;
  uint8_t EID[6];
} sl_psram_id_type_t;

/// PSRAM configuration handle structure
struct sl_psram_info_type_t {
  PSRAMDevType deviceName;         ///< PSRAM Device enum value
  sl_psram_id_type_t deviceID;     ///< Device ID struct
  uint32_t devDensity;             ///< Device Density in bits
  uint32_t normalReadMAXFrequency; ///< Max frequency for normal read
  uint32_t fastReadMAXFrequency;   ///< Max frequency for fast read
  PSRAMRWType rwType;              ///< Read-Write type configuration
  spi_config_t spi_config;         ///< SPI Config for QSPI interface
  uint16_t defaultBurstWrapSize;   ///< Default burst wrap size
  uint16_t toggleBurstWrapSize;    ///< Toggle Burst Wrap size
};

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/// PSRAM status context
struct PSRAMStatusType {
  PSRAMStateType state;        ///< PSRAM state enum value
  uint8_t interfaceMode;       ///< QSPI Interface mode for PSRAM
  bool secureModeEnable;       ///< Secure mode enable status
  uint8_t secureSegmentNumber; ///< Secure segment number
  uint16_t burstSize;          ///< Burst Size
};

/// PSRAM pin configuration structure
struct PSRAMPinConfigType {
  uint8_t port;
  uint8_t pin;
  uint8_t mux;
  uint8_t pad;
};

/// DMA transfer status enum
typedef enum { IDLE, TX_RUNNING, RX_RUNNING, FAILED } XferStatusType;

/// DMA transfer context
struct xferContextType {
  XferStatusType xferStatus;
  uint32_t xferNextAddress;
  void *xferNextSourceAddress;
  uint32_t xferRemLength;
  uint8_t xferHsize;
  sl_psram_dma_status_type_t *done;
};

struct PSRAMSecureSegmentType {
  uint8_t segmentEnable;
  uint32_t lowerBoundary;
  uint32_t higherBoundary;
#define MAX_SEC_SEGMENTS 4
};

/** @endcond */

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************/ /**
 * @brief     
 *   Initialize the PSRAM Device 
 * 
 * @param none                                        
 *
 * @return      
 *   Status Code of the operation   
 * 
 * @note The configurations for initialization are taken from the PSRAM Device 
 * config header file. The selection of configuration file happens implicitely 
 * based on the radio board selection.         
 ******************************************************************************/
sl_psram_return_type_t sl_si91x_psram_init(void);

/***************************************************************************/ /**
 * @brief     
 *   Uninitialize the PSRAM Device
 * 
 * @param none
 *
 * @return      
 *   Status Code of the operation
 ******************************************************************************/
sl_psram_return_type_t sl_si91x_psram_uninit(void);

/***************************************************************************/ /**
 * @brief       Reset the PSRAM Device
 * @param none
 * @return      Status Code of the operation
 ******************************************************************************/
sl_psram_return_type_t sl_si91x_psram_reset(void);

/***************************************************************************/ /**
 * @brief       Write data to PSRAM in manual mode 
 * @param[in] addr
 *   PSRAM address for write operation                     
 * @param[in] SourceBuf
 *   Reference of the Source buffer
 * @param[in] hSize
 *   Size of each element
 * @param[in] num_of_elements
 *   Number of elements for write operation                        
 * @return      Status Code of the operation
 ******************************************************************************/
sl_psram_return_type_t sl_si91x_psram_manual_write_in_blocking_mode(uint32_t addr,
                                                                    void *SourceBuf,
                                                                    uint8_t hSize,
                                                                    uint32_t length);

/***************************************************************************/ /**
 * @brief       Read data from PSRAM in manual mode 
 * @param[in] addr
 *   PSRAM address for read operation
 * @param[in] hSize
 *   Size of each element
 * @param[in] num_of_elements
 *   Number of elements for read operation                     
 * @param[out] DestBuf
 *   Reference of the Destination buffer                       
 * @return      Status Code of the operation
 ******************************************************************************/
sl_psram_return_type_t sl_si91x_psram_manual_read_in_blocking_mode(uint32_t addr,
                                                                   void *DestBuf,
                                                                   uint8_t hSize,
                                                                   uint32_t length);

/***************************************************************************/ /**
 * @brief       Write data to PSRAM in manual mode using DMA
 * @param[in] addr
 *   PSRAM address for write operation                     
 * @param[in] SourceBuf
 *   Reference of the Source buffer
 * @param[in] hSize
 *   Size of each element
 * @param[in] length
 *   Number of elements for write operation
 * @param[out] dmastatus
 *   DMA operation completion status           
 * @return      Status Code of the operation
 ******************************************************************************/
sl_psram_return_type_t sl_si91x_psram_manual_write_in_dma_mode(uint32_t addr,
                                                               void *SourceBuf,
                                                               uint8_t hSize,
                                                               uint32_t length,
                                                               sl_psram_dma_status_type_t *dmastatus);

/***************************************************************************/ /**
 * @brief       Read data from PSRAM in manual mode using DMA
 * @param[in] addr
 *   PSRAM address for read operation
 * @param[in] hSize
 *   Size of each element
 * @param[in] length
 *   Number of elements for read operation                     
 * @param[out] DestBuf
 *   Reference of the Destination buffer
 * @param[out] dmastatus
 *   DMA operation completion status                            
 * @return      Status Code of the operation
 ******************************************************************************/
sl_psram_return_type_t sl_si91x_psram_manual_read_in_dma_mode(uint32_t addr,
                                                              void *DestBuf,
                                                              uint8_t hSize,
                                                              uint32_t length,
                                                              sl_psram_dma_status_type_t *dmaStatus);

#if PSRAM_HALF_SLEEP_SUPPORTED
/***************************************************************************/ /**
 * @brief       Put PSRAM Device in sleep 
 * @details Sleep mode is a feature which puts the PSRAM device in an ultra-low power state, 
 * while the stored data is retained.
 * @param none         
 * @return      Status Code of the operation
 ******************************************************************************/
sl_psram_return_type_t sl_si91x_psram_sleep(void);

/***************************************************************************/ /**
 * @brief       Exit PSRAM device from sleep               
 * @param none
 * @return      Status Code of the operation
 * @note This function call re-initializes the PSRAM device after wakeup sequence.
 ******************************************************************************/
sl_psram_return_type_t sl_si91x_psram_wakeup(void);
#endif

/***************************************************************************/ /**
 * @brief       Enable CTR encryption-decryption on PSRAM
 * @param[in] keySize
 *   Pass 128/256-Bit size                                           
 * @return      Status Code of the operation
 ******************************************************************************/
sl_psram_return_type_t sl_si91x_psram_enable_encry_decry(uint16_t keySize);

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY !***********************
/// @addtogroup PSRAM PSRAM Driver
/// @{
///
///   @details
///
///
///   @n @section PSRAM_Intro Introduction
///
///    PSRAM (Pseudo Static Random Access Memory) is a random-access memory whose
///    internal structure is based on dynamic memory with refresh control signals
///    generated internally, in the standby mode, so that it can mimic the functionality
///    of a static memory. It combines the high density of DRAM with the ease-of-use
///    of true SRAM. The M4 core communicates with the PSRAM via Quad SPI interface.
///
///   @n @section PSRAM_Config Configuration
///
///    # PSRAM Device Configuration
///
///    The PSRAM Driver offers configuring the following:
///    - Read-Write type
///     - Normal: This supported only in SPI interface mode. Supports maximum
///       frequency of 33MHz. Uses normal read and normal write command.
///     - Fast: Supported in SPI and QPI mode. Uses fast read and normal write.
///     - Quad IO: Supported in SPI and QPI mode. Uses fast quad read and write.
///    - Interface mode
///     - SPI Mode (Serial IO)
///     - QPI Mode (Quad IO)
///    - Operation frequency Source
///     - Interface PLL Clock
///     - ULP Reference Clock
///     - SoC PLL Clock
///     - M4_SOCCLKNOSWLSYNCCLKTREEGATED Clock
///
///    # Linker configurations
///
///    The text segment, data segment, bss, heap and stack can be placed in PSRAM by
///    installing the respective components present under "PSRAM Linker Configurations"
///    from "SOFTWARE COMPONENTS" GUI. Since PSRAM is already initialized in bootloader,
///    these components can be installed and the respective segments can be placed in
///    PSRAM without installing "PSRAM Core" component and without initializing psram
///    from application.
///
///   @li For more information on configuring available parameters refer to the respective peripheral example readme document.
///
///   @n @section PSRAM_Usage Usage
///
///    PSRAM Driver and QSPI are initialized by bootloader with Quad IO read-write type
///    and QPI interface. The application is not required to reinitialize PSRAM device
///    and QSPI unless the configurations required are different from the default set
///    by the bootloader.
///
///    The PSRAM device handle "PSRAM_Device" of type @ref sl_psram_info_type_t is defined
///    "in sl_si91x_psram_handle.c".
///
///    @ref sl_si91x_psram_uninit assumes that PSRAM was initialized with QPI mode and exits
///    QPI mode within definition. If the PSRAM configuration in bootcode has SPI mode enabled,
///    user is expected to comment the exit QPI mode function call in @ref sl_si91x_psram_uninit.
///
///    To reconfigure and initliaze PSRAM, set required configurations from PSRAM Core component
///    and call @ref sl_si91x_psram_init within application.
///
///
/// @} end group PSRAM ********************************************************/

#endif //__SL_SI91X_PSRAM_H_
