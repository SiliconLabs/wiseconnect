/*******************************************************************************
* @file  rsi_ipmu.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

/**
 * Includes
 */
#ifndef __RSI_IPMU_H__
#define __RSI_IPMU_H__

#ifdef __cplusplus
extern "C" {
#endif

#define AT_EFUSE_DATA_1P19
#include "rsi_ccp_common.h"
#include "rsi_error.h"

/******************************************************
   * *                      Macros
   * ******************************************************/

#define REG_GSPI_BASE           0x24050000
#define NPSS_BASE_ADDR          0x24048000
#define NPSS_NWP_BASE_ADDR      0x41300000
#define MCURET_BOOTSTATUS       (volatile uint32_t *)(NPSS_BASE_ADDR + 0x604)
#define MCU_BBFF_STORAGE1_ADDR  (volatile uint32_t *)(NPSS_NWP_BASE_ADDR + 0x580)
#define TA_MANF_DATA_BASE_ADDR  0x04000000
#define MCU_MANF_DATA_BASE_ADDR 0x08000000
#define TA_FLASH                1

/*IPMU power gates */
#define CMP_NPSS_PG_ENB                  BIT(16) /*Power gate enable for BOD CORE*/
#define ULP_ANG_CLKS_PG_ENB              BIT(15) /*Power gate enable for CLKS CORE*/
#define ULP_ANG_PWRSUPPLY_PG_ENB         BIT(14) /*Power gate enable for BG SPI*/
#define WURX_PG_ENB                      BIT(13) /*Power gate enable for WURX*/
#define WURX_CORR_PG_ENB                 BIT(12) /*Power gate enable for WURX CORRELATION BLOCK*/
#define AUXADC_PG_ENB                    BIT(11) /*Power gate enable for AUXADC*/
#define AUXADC_BYPASS_ISO_GEN            BIT(10) /*power gate bypass for AUXADC*/
#define AUXADC_ISOLATION_ENABLE          BIT(9)  /*power gate isolation for AUXADC*/
#define AUXDAC_PG_ENB                    BIT(8)  /*Power gate enable for AUXDAC*/
#define AUXDAC_BYPASS_ISO_GEN            BIT(7)  /*power gate bypass for AUXDAC*/
#define AUXDAC_ISOLATION_ENABLE          BIT(6)  /*power gate isolation for AUXDAC*/
#define AUX_SUPPLY_ISOLATION_ENABLE      BIT(5)  /*Given to analog peripherals indicating the supply state*/
#define PMU_ANA_BYPASS_PWRGATE_EN_N      BIT(4)  /*To ON or OFF analog blocks in PMU when pwr manager is bypassed*/
#define PMU_SPI_BYPASS_ISO_GEN           BIT(3)  /*Bypass power manager for PMU_SPI*/
#define PMU_SPI_ISOLATION_ENABLE         BIT(2)  /*Bypass isoaltion enable signal for PMU_SPI isoaltion cells*/
#define PMU_BUCK_BYPASS_ISO_GEN          BIT(1)  /*Bypass power  manager for PMU BUCK*/
#define PMU_BUCK_BYPASS_ISOLATION_ENABLE BIT(0)  /*Bypass isoaltion enable signal for PMU_BUCK isoaltion cells*/

/*IPMU configuration defines*/
#define LATCH_TOP_SPI        BIT(4)
#define LATCH_TRANSPARENT_HF BIT(3)
#define LATCH_TRANSPARENT_LF BIT(2)

/*BG_SCDC_PROG_REG_1 defines*/
#define REF_SEL_LP_DCDC 0xFFFFFC7F

/*Registers */
#define BG_SCDC_PROG_REG_1 0x127
#define SELECT_BG_CLK      0x144
#define BG_SCDC_PROG_REG_2 0x128
//#define WURX_CORR_CALIB_REG  0x088
#define POWERGATE_REG_WRITE 0x142
//#define ULPCLKS_REFCLK_REG   0x106
//#define WURX_CORR_CALIB_REG  0x088

#define GSPI_CTRL_REG1 *(volatile uint32_t *)(REG_GSPI_BASE + 0x02)
#define SPI_ACTIVE     BIT(8)

#define POSITION_BITS_MASK 0x1F
#define LSB_POSITION       22
#define MSB_POSITION       27
#define MAX_BIT_LEN        22

#define ULPCLKS_32KRC_CLK_REG_ADDR 0x103

#define ENABLE_CALIB_DOMAIN          0x005200
#define SELECT_RO_CALIB              0x002310
#define TRIM_VALUE_BITS              0x0003f800
#define MASK_TRIM_VALUE_WRITE_BITS   0x1FC000
#define ULPCLKS_TRIM_SEL_REG_DEFAULT 0x005a14

#define MAX_RESP_BUF_FOR_IAP 3

#define BASE_OFFSET_BB_RF_IN_FLASH 424

#define PMU_SPI_BASE_ADDR 0x24050000

//! PMU
#define PMU_SPI_DIRECT_ACCESS(_x) *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0x8000 + ((_x) << 2))
//! IPMU
#define PMU_DIRECT_ACCESS(_x) *(volatile uint32_t *)(PMU_SPI_BASE_ADDR + 0xA000 + ((_x) << 2))
#define PMU_SPI               1

#define RC_CLK_MODE 1

#define XTAL_SEL 1

#define TS_SLOPE_SET_OFFSET         0x04
#define PMU_1P3_CTRL_REG_OFFSET     0x1D0
#define PMU_PFM_REG_OFFSET          0x1D1
#define PMU_ADC_REG_OFFSET          0x1D2
#define PMU_PID_REG1_OFFSET         0x1D3
#define PMU_PTAT_REG_OFFSET         0x1D5
#define PMU_LDO_REG_OFFSET          0x1D6
#define PMU_PWRTRAIN_REG_OFFSET     0x1D8
#define PMU_TESTMUX_REG1_OFFSET     0x1D9
#define PMU_TEST_MODES_OFFSET       0x1DA
#define SPARE_REG_1_OFFSET          0x1DC
#define SPARE_REG_2_OFFSET          0x1DD
#define BYPASS_CURR_CTRL_REG_OFFSET 0x1DE
#define SPARE_REG_3_OFFSET          0x1DF

#ifdef CHIP_9118
//FLASH OFFSET
#define __CALIB_DATA_OFFSET_          1024
#define IPMU_VALUES_OFFSET            (__CALIB_DATA_OFFSET_ + 168)
#define DUAL_FLASH_IPMU_VALUES_OFFSET MCU_MANF_DATA_BASE_ADDR
#endif

#ifdef CHIP_917
#if (((defined CHIP_917) || (defined CHIP_917_6x6)) && (defined CHIP_917B0))
#define PACKAGE_TYPE_VALUES_OFFSET_COMMON_FLASH 0x81B0292
#define SILICON_REV_VALUES_OFFSET_COMMON_FLASH  0x81B0293
#define COMMON_FLASH_IPMU_VALUES_OFFSET         0x81B0258

#define PACKAGE_TYPE_VALUES_OFFSET_DUAL_FLASH 0x800059B
#define SILICON_REV_VALUES_OFFSET_DUAL_FLASH  0x800059C
#define DUAL_FLASH_IPMU_VALUES_OFFSET         0x8000561
#endif
#if ((defined CHIP_917) && (!(defined CHIP_917B0)))
#define PACKAGE_TYPE_VALUES_OFFSET_COMMON_FLASH 0x82001DE
#define SILICON_REV_VALUES_OFFSET_COMMON_FLASH  0x82001DF
#define COMMON_FLASH_IPMU_VALUES_OFFSET         0x82001A4

#define PACKAGE_TYPE_VALUES_OFFSET_DUAL_FLASH 0x80004E0
#define SILICON_REV_VALUES_OFFSET_DUAL_FLASH  0x80004E1
#define DUAL_FLASH_IPMU_VALUES_OFFSET         0x80004A6
#endif
#define IPMU_VALUES_OFFSET 0
#endif
#define HP_LDO_MODE 1
#define SCDC_MODE   0
#ifdef ENABLE_1P8V
#define IPMU_MODE_VALUE HP_LDO_MODE
#else
#define IPMU_MODE_VALUE SCDC_MODE
#endif

#ifdef COMMON_FLASH_EN
#define MANF_DATA_BASE_ADDR COMMON_FLASH_IPMU_VALUES_OFFSET
#else
#define MANF_DATA_BASE_ADDR DUAL_FLASH_IPMU_VALUES_OFFSET
#endif

/* After changes */
#define MAGIC_WORD 0x5a

#define NWP_AHB_ADDR               0x41300000
#define ULP_TASS_MISC_CONFIG_REG   0x24041400
#define AUX_BASE_ADDR              0x24043800
#define TEMP_SENSOR_BASE_ADDRESS   0x24048500
#define TS_NOMINAL_SETTINGS_OFFSET 0x08
#define DIRECT                     2

#define MASK_BITS(A, B)                 (((1U << A) - 1) << B)
#define ULP_SPI                         0
#define ULPCLKS_DOUBLER_XTAL_REG_OFFSET 0x101
#define ULPCLKS_32KRO_CLK_REG_OFFSET    0x102
#define ULPCLKS_32KRC_CLK_REG_OFFSET    0x103
#define ULPCLKS_32MRC_CLK_REG_OFFSET    0x104
#define ULPCLKS_HF_RO_CLK_REG_OFFSET    0x105
#define ULPCLKS_REFCLK_REG_ADDR         0x106
#define ULPCLKS_TRIM_SEL_REG_ADDR       0x107
#define ULPCLKS_CALIB_REG_ADDR          0x10A
#define ULPCLKS_CALIB_REF_REG           0x10B
#define ULPCLKS_CALIB_DONE_REG_ADDR     0x10C
#define ULPCLKS_32KXTAL_CLK_REG_OFFSET  0x10E
#define BG_SCDC_PROG_REG_1_OFFSET       0x127
#define iPMU_SPARE_REG1_OFFSET          0x140
#ifdef CHIP_917
#define BG_SCDC_PROG_REG_3_ADDR 0x12B
#endif

#define HF_RC_CLK_MODE               1
#define MCU_PWR_CTRL_BASE_ADDR       0x24048400
#define MCU_PMU_LDO_CTRL_CLEAR_REG_1 *(volatile uint32 *)(MCU_PWR_CTRL_BASE_ADDR + 0x6C)
#define TEMP_SENSOR_BASE_ADDRESS     0x24048500
#define MCU_FSM_BASE_ADDRESS         0x24048100

//! ULP SPI (0x2405A000)
#define ULPCLKS_ADAPTIVE_REG_OFFSET         0x100
#define ULPCLKS_DOUBLER_XTAL_REG_OFFSET     0x101
#define ULPCLKS_32KRO_CLK_REG_OFFSET        0x102
#define ULPCLKS_32KRC_CLK_REG_OFFSET        0x103
#define ULPCLKS_32MRC_CLK_REG_OFFSET        0x104
#define ULPCLKS_HF_RO_CLK_REG_OFFSET        0x105
#define ULPCLKS_REF_CLK_REG_OFFSET          0x106
#define ULPCLKS_TRIM_SEL_REG_OFFSET         0x107
#define ULPCLKS_32KXTAL_CLK_REG_OFFSET      0x10E
#define BG_SLEEP_TIMER_REG_OFFSET           0x125
#define SCDC_CTRL_REG_0_OFFSET              0x126
#define BG_SCDC_PROG_REG_1_OFFSET           0x127
#define BG_SCDC_PROG_REG_2_OFFSET           0x128
#define BG_LDO_REG_OFFSET                   0x129
#define BG_SCDC_READ_BACK_OFFSET            0x12A
#define BG_BLACKOUT_REG_OFFSET              0x12B
#define iPMU_SPARE_REG1_OFFSET              0x140
#define iPMU_SPARE_REG2_OFFSET              0x141
#define POWERGATE_REG_WRITE_OFFSET          0x142
#define SELECT_BG_CLK_OFFSET                0x144
#define BOD_TEST_PG_VBATT_STATUS_REG_OFFSET 0x1E3
#define POWERGATE_REG_READ_OFFSET           0x342

//! PMU SPI (0x24058000)
#define PMU_1P3_CTRL_REG_OFFSET     0x1D0
#define PMU_PFM_REG_OFFSET          0x1D1
#define PMU_ADC_REG_OFFSET          0x1D2
#define PMU_PID_REG1_OFFSET         0x1D3
#define PMU_PTAT_REG_OFFSET         0x1D5
#define PMU_LDO_REG_OFFSET          0x1D6
#define PMU_PWRTRAIN_REG_OFFSET     0x1D8
#define PMU_TESTMUX_REG1_OFFSET     0x1D9
#define PMU_TEST_MODES_OFFSET       0x1DA
#define SPARE_REG_1_OFFSET          0x1DC
#define SPARE_REG_2_OFFSET          0x1DD
#define BYPASS_CURR_CTRL_REG_OFFSET 0x1DE
#define SPARE_REG_3_OFFSET          0x1DF

#define MCU_FSM_CLK_ENS_AND_FIRST_BOOTUP_OFFSET 0x20
#define MCU_FSM_PMU_STATUS_REG_OFFSET           0x40

//! NWP_PMU_CTRLS  defines
#define scdcdc_lp_mode_en BIT(0)
#define bgpmu_sleep_en    BIT(1)
#define standby_dc1p3     BIT(19)

#define MCU_AON_BASE_ADDR    0x24048000
#define MCU_FSM_PMU_CTRL     *(volatile uint32 *)(MCU_AON_BASE_ADDR + 0x140)
#define MCUAON_GEN_CTRLS_REG *(volatile uint32 *)(MCU_AON_BASE_ADDR + 0x14)
#define MCUAON_SHELF_MODE    *(volatile uint32 *)(MCU_AON_BASE_ADDR + 0x10)

//! MCUAON_GEN_CTRLS register defines
#define NPSS_SUPPLY_0P9_BIT BIT(17)
#define ENABLE_PDO_BIT      BIT(16)

#define NWP_AHB_ADDR             0x41300000
#define ULP_DIRECT_ACCESS(_x)    *(uint32 *)(NWP_AHB_ADDR + (_x))
#define NWPAON_POR_CTRL_BITS_REG 0x3C
#define NWP_FSM_FIRST_BOOTUP     0x0120

//! NWP_FSM_FIRST_BOOTUP  defines
#define nwp_ulp_32khz_xtal_clk_en     BIT(18)
#define nwp_ulp_32mhz_rc_clk_en       BIT(19)
#define nwp_ulp_20mhz_ring_osc_clk_en BIT(20)
#define nwp_ulp_doubler_clk_en        BIT(21)

#define TASS_PWR_CTRL_BASE_ADDR 0x41300400
#define TASS_FSM_CTRL_BYPASS    *(volatile uint32 *)(TASS_PWR_CTRL_BASE_ADDR + 0x1C)
//! TASS_FSM_CTRL_BYPASS defines
#define ta_xtal_en_40MHz_bypass_cntrl     BIT(0)
#define ta_xtal_en_40MHz_bypass           BIT(1)
#define ta_pmu_shut_down_bypass_cntrl     BIT(2)
#define ta_pmu_shut_down_bypass           BIT(3)
#define ta_buck_boost_enable_bypass_cntrl BIT(4)
#define ta_buck_boost_enable_bypass       BIT(5)

//! ULPCLKS_ADAPTIVE_REG defines
#define adapt_powergate_en BIT(3)

//! ULPCLKS_DOUBLER_XTAL_REG defines
#define doubler_en BIT(21)

//! ULPCLKS_32MRC_CLK_REG defines
#define rc_32mhz_en BIT(21)

//! ULPCLKS_HF_RO_CLK_REG defines
#define ro_hf_en BIT(21)

//! ULPCLKS_TRIM_SEL_REG defines
#define calib_powergate_en BIT(9)

//! ULPCLKS_32KXTAL_CLK_REG defines
#define xtal_32khz_en BIT(21)

#define pass_clk_40m_buffer_enable BIT(15)

//! iPMU_SPARE_REG2 defines
#define wurx_lvl_shift_en BIT(20)
#define wurx_pg_en_1      BIT(21)

//! POWERGATE_REG_WRITE defines
#define auxdac_pg_enb            BIT(8)
#define auxadc_pg_enb            BIT(11)
#define wurx_corr_pg_enb         BIT(12)
#define wurx_pg_enb              BIT(13)
#define ulp_ang_pwrsupply_pg_enb BIT(14)
#define ulp_ang_clks_pg_enb      BIT(15)
#define cmp_npss_pg_enb          BIT(16)

#define IPMU_HIGH_POWER_MODE 0
#define IPMU_LOW_POWER_MODE  1

//! SELECT_BG_CLK defines
#define latch_transparent_lf BIT(2)
#define latch_transparent_hf BIT(3)
#define latch_top_spi        BIT(4)

//! PMU_PFM_REG defines
#define ext_pfm_en1p3 BIT(17)

//! PMU_PTAT_REG defines
#define test_pfm_mode1p3 BIT(4)

//! PMU_LDO_REG defines
#define LDOSOC_DEFAULT_MODE_EN BIT(5)

//! PMU_PWRTRAIN_REG defines
#define BYPASS_LDORF_CTRL BIT(2)

//! BOD_TEST_PG_VBATT_STATUS_REG defines
#define bod_pwrgate_en_n_ulp_button_calib BIT(15)

//! MCU_FSM_CLK_ENS_AND_FIRST_BOOTUP defines
#define mcu_ulp_32khz_xtal_clk_en     BIT(18)
#define mcu_ulp_32mhz_rc_clk_en       BIT(19)
#define mcu_ulp_20mhz_ring_osc_clk_en BIT(20)
#define mcu_ulp_doubler_clk_en        BIT(21)

//! NWPAON_POR_CTRL_BITS defines
#define poc_cntrl_reg_0 BIT(0)

#define MCU_FSM_DIRECT_ACCESS(_x) *(volatile uint32 *)(MCU_FSM_BASE_ADDRESS + (_x))

#define TEMP_SENSOR_BJT *(volatile uint32 *)(0x240439E0)
#define temp_sens_en    BIT(0)

#define TS_SLOPE_SET_OFFSET        0x04
#define TS_NOMINAL_SETTINGS_OFFSET 0x08

//! BG_SCDC_PROG_REG_2 defines
#define scdcdc_sel    BIT(21)
#define testmode_0_en BIT(20)

//! SCDC_CTRL_REG_0 defines
#define ext_cap_en BIT(21)

//! PMU_LDO_REG defines
#define LDORF_DEFAULT_MODE_EN BIT(11)

#define LS_SHIFT      22
#define MS_SHIFT      27
#define IPMU_DATAMASK 0x3fffff

#define mcu_dcdc_lvl    BIT(18)
#define mcu_soc_ldo_lvl BIT(17)

/*32khz rc clock define  */
#define RC_TRIM_VALUE_LF                0x7F0
#define RO_TRIM_VALUE_LF                0x1F0
#define MASK32KRO_TRIM_VALUE_WRITE_BITS 0x1F0000
#define MASK32KRC_TRIM_VALUE_WRITE_BITS 0x1FC000
#define TRIM_MSB_32MHZ                  20
#define TRIM_LSB_32MHZ                  14
#define PARTICULAR_FREQ_MIN             10
#define PARTICULAR_FREQ_MAX             100
#define MIN_DIFF_FREQ                   3

/* 64khz rc clock define */
#define ENABLE_32KHZ_CLOCK_TRIM      0x40F03000
#define NPSS_REF_CLOCK_40MSOC        0x41A48000
#define NUMBER_HIGH_FRQ_CLOCK        0x42C4E390
#define LOW_FREQ_CLOCK_CAL           0x42B24210
#define ORIGINAL_REF_VALUE_AFTER_CAL 0x42C9C590

/******************************************************
   * *                    Structures
   * ******************************************************/

//! This structure contains format for retention_boot_status_word_0
typedef struct retention_boot_status_word_s {
#define SDIO_USB_WITH_TA         3
#define SDIO_WITH_TA_USB_WITH_M4 2
#define SDIO_WITH_M4_USB_WITH_TA 1
#define SDIO_USB_WITH_M4         0
  uint32_t m4_present : 1;
  uint32_t m4_flash_present : 1;
  uint32_t m4_flash_pinset : 4;
  uint32_t m4_flash_address_width_valid : 1;
  uint32_t m4_flash_address_width : 2;
  uint32_t select_host_inf_with_m4_valid : 1;
  uint32_t select_host_inf_with_m4 : 2;
  uint32_t m4_secure_boot_enable : 1;
  uint32_t m4_encrypt_firmware : 1;
  uint32_t host_if_with_ta : 1;
  uint32_t mcu_wdt_hw_timer : 1;
#ifdef CHIP_9118
#define NONE_MODE        0
#define NLINK            1
#define WISECONNECT      2
#define WCPLUS           3
#define MCU              4
#define WISEMCU          5
#define ACCELARATOR      6
#define WC_SIMULATANEOUS 7
#endif
#ifdef CHIP_917
//! Product modes
#define WISEMCU     0
#define WCPLUS      3
#define ACCELARATOR 4
#define WISECONNECT 6
#define NLINK       7
#define MCU         0xF // not supported
#endif
  uint32_t product_mode : 4;
  uint32_t m4_flash_type : 4;
  uint32_t m4_dual_flash : 1;
  uint32_t m4_csum : 1;
  uint32_t wise_aoc_mode : 1;
  uint32_t wise_aoc_from_m4_rom : 1;
  uint32_t m4_image_format : 1;
  uint32_t clean_ulp_wakeup : 1;
#define M4_IMAGE_VALID_IND BIT(30)
  uint32_t m4_image_valid : 1;
  uint32_t reserved : 1; /* one bit is reserved for hardware */
} retention_boot_status_word_t;

/* This structure contains format for efuse_dword0 */
typedef struct npss_boot_status_word_0_s {
  //! Data from EFUSE
  uint32_t usb_fsel_valid : 1;
  uint32_t mems_ref_clk_as_usb_phy_clk : 1;
  uint32_t modem_pll_as_usb_phy_clk : 1;
  uint32_t usb_phy_clk_fsel_external : 1;
  uint32_t usb_fsel : 3;
  uint32_t bypass_usb_detection : 1;
  //! Data derived by bootloder
  uint32_t host_sel_valid : 1;
  uint32_t host_sel : 3;
  uint32_t ta_flash_present : 1;
  uint32_t ta_flash_pinset : 4;
  uint32_t ta_flash_address_width_valid : 1;
  uint32_t ta_flash_address_width : 2;
  uint32_t ta_flash_type : 4;
  //  uint32_t ta_dual_flash : 1;
  uint32_t fips_enable : 1;
  uint32_t usb_fclk_div_factor : 2;
#define BBFF_DATA_VALID BIT(27)
  uint32_t bbff_data_valid : 1;
  //! Bits configured by FW
#define NWP_SOFT_RESET BIT(28)
  uint32_t soft_reset : 1;
#define FACTORY_RESET BIT(29)
  uint32_t factory_reset : 1;
#define TAMPER_RECOVERY BIT(30)
  uint32_t tamper_recovery : 1;
  uint32_t reserved : 1;
} npss_boot_status_word0_t;

#ifdef CHIP_9118
typedef struct efuse_ipmu_s {
  uint32_t trim_0p5na1 : 1;
  uint32_t trim_0p5na2 : 1;
  uint32_t bg_r_vdd_ulp : 4;
  uint32_t bg_r_ptat_vdd_ulp : 3;
  uint32_t resbank_trim : 2;
  uint32_t trim_sel : 7;
  uint32_t del_2x_sel : 6;
  uint32_t freq_trim : 5;
  uint32_t coarse_trim_16k : 2;
  uint32_t fine_trim_16k : 7;
  uint32_t coarse_trim_64k : 2;
  uint32_t fine_trim_64k : 7;
  uint32_t coarse_trim_32k : 2;
  uint32_t fine_trim_32k : 7;
  uint32_t xtal1_trim_32k : 4;
  uint32_t xtal2_trim_32k : 4;
  uint32_t trim_ring_osc : 7;
  uint32_t vbatt_status_1 : 6;
  uint32_t str_temp_slope : 10;
  uint32_t f2_nominal : 10;
  uint32_t str_nominal_temp : 7;
  uint32_t str_bjt_temp_sense_off : 16;
  uint32_t str_bjt_temp_sense_slope : 16;
#ifndef AT_EFUSE_DATA_1P19
  uint32_t reserved1 : 20;
#endif

#ifdef AT_EFUSE_DATA_1P19
  uint32_t trim_sel_20Mhz : 7; // Trim value for 20mzh rc
  uint32_t ro_32khz_00_trim : 5;
  uint32_t scdc_dcdc_trim : 3;
  uint32_t scdc_hpldo_trim : 3;
  uint32_t reserved1 : 2;
#endif
  uint32_t ldo_ctrl : 4;
#ifndef AT_EFUSE_DATA_1P19
  uint32_t reserved2 : 16;
#endif
#ifdef AT_EFUSE_DATA_1P19
  uint32_t vbg_tsbjt_efuse : 12;
  uint32_t retn_ldo_lptrim : 3;
  uint32_t reserved2 : 1;
#endif
  uint32_t auxadc_offset_diff : 12;
  uint32_t auxadc_invgain_diff : 16;
  uint32_t auxadc_offset_single : 12;
  uint32_t auxadc_invgain_single : 16;
  uint32_t set_vref1p3 : 4;

#ifndef AT_EFUSE_DATA_1P19
  uint32_t set_vref_isense1p3 : 2;
  uint32_t set_vref_adc : 2;
  uint32_t vtrim_ldosoc : 2;
#endif

#ifdef AT_EFUSE_DATA_1P19
  uint32_t reserved13 : 6;
#endif

  uint32_t trim_r1_resistorladder : 4;
#ifndef AT_EFUSE_DATA_1P19
  uint32_t enable_undershoot_reduction : 1;
  uint32_t select_vref_comp : 2;
#endif

#ifdef AT_EFUSE_DATA_1P19
  uint32_t retn_ldo_hptrim : 3;
#endif

#ifndef AT_EFUSE_DATA_1P19
  uint32_t pwr_gd_threshold_sel : 1;
  uint32_t sel_overshoot_control : 1;
  uint32_t ptat_load_ctrl : 3;
  uint32_t ctrl_soc : 4;
  uint32_t pt_gate_ctrl : 3;
  uint32_t default_mode_ctrl : 1;
  uint32_t ptat_load_enable : 1;
  uint32_t ldosoc_outputpulldown_sel : 1;
  uint32_t ldosoc_outputpulldown : 1;
#endif

#ifdef AT_EFUSE_DATA_1P19
  uint32_t reserved12 : 16;
#endif

  uint32_t scale_soc_ldo_vref : 1;

#ifndef AT_EFUSE_DATA_1P19
  uint32_t ctrl_rf : 4;
  uint32_t default_mode : 1;
  uint32_t test_ldopulldown_sel : 1;
  uint32_t test_ldopulldown : 1;
  uint32_t drive_n : 2;
  uint32_t drive_p : 2;
  uint32_t deadtime_ctrl_n2p : 4;
  uint32_t deadtime_ctrl_p2n : 4;
  uint32_t revi_offset_prog : 3;
  uint32_t tran_lo_ctr : 2;
  uint32_t tran_hi_ctr : 2;
  uint32_t tran_und_shoot_ctr : 3;
#endif

#ifdef AT_EFUSE_DATA_1P19
  uint32_t reserved11 : 7;
  uint32_t reserved10 : 12;
  uint32_t reserved9 : 10;
#endif

  uint32_t dpwm_freq_trim : 4;

#ifndef AT_EFUSE_DATA_1P19
  uint32_t pfmro_freq_trim : 3;
  uint32_t test_revi_delay : 1;
  uint32_t sel_sleep_nmos_ctrl : 1;
  uint32_t p_1p3 : 13;
  uint32_t i_steady_state1p3 : 13;
  uint32_t d_1p3 : 15;
  uint32_t i_soft_start1p3 : 13;
  uint32_t dither_en1p3 : 1;
  uint32_t auto_mode_tran_disable : 1;
#endif

#ifdef AT_EFUSE_DATA_1P19
  uint32_t reserved73 : 1;
  uint32_t reserved74 : 13;
  uint32_t reserved75 : 13;
  uint32_t reserved76 : 15;
  uint32_t reserved77 : 13;
  uint32_t reserved78 : 1;
  uint32_t reserved79 : 1;
#endif

  uint32_t pfm_pon_time_sel : 4;

#ifndef AT_EFUSE_DATA_1P19
  uint32_t pfm_non_time_sel : 3;
  uint32_t pwm_cont_prog : 3;
  uint32_t pfm_clk_up_del_sel : 3;
  uint32_t pwm_to_pfm_pulse_count_prog : 2;
  uint32_t pfm_to_pwm_pulse_count_prog : 2;
  uint32_t pfm_to_pwm_cur_prog : 3;
  uint32_t pwm_to_pfm_cur_prog : 3;
  uint32_t max_duty_cycle_threshold : 3;
  uint32_t min_duty_cycle_threshold : 3;
  uint32_t bypass_pfm_to_pwm_counter_1 : 1;
  uint32_t no_of_pfm_clk : 4;
  uint32_t adc_op_thresh_sel : 2;
#endif

#ifdef AT_EFUSE_DATA_1P19
  uint32_t reserved6;
  uint32_t reserved31 : 3;
  uint32_t reserved32 : 3;
  uint32_t reserved33 : 3;
  uint32_t reserved34 : 2;
  uint32_t reserved35 : 2;
  uint32_t reserved36 : 3;
  uint32_t reserved37 : 3;
  uint32_t reserved38 : 3;
  uint32_t reserved39 : 3;
  uint32_t reserved40 : 1;
  uint32_t reserved41 : 4;
  uint32_t reserved42 : 2;
#endif
  uint32_t reserved3 : 4;
  uint32_t reserved4[2];
  uint16_t reserved5;

} __attribute__((__packed__)) efuse_ipmu_t;
#endif

#ifdef CHIP_917
typedef struct efuse_ipmu_s {
  uint32_t trim_0p5na1 : 1;
  uint32_t bg_r_vdd_ulp : 5;
  uint32_t bg_r_ptat_vdd_ulp : 3;
  uint32_t reserved20 : 2; //Removed in RS9117
  uint32_t trim_sel : 7;
  uint32_t del_2x_sel : 6;
  uint32_t freq_trim : 5;
  uint32_t coarse_trim_16k : 2;
  uint32_t fine_trim_16k : 7;
  uint32_t coarse_trim_64k : 2;
  uint32_t fine_trim_64k : 7;
  uint32_t coarse_trim_32k : 2;
  uint32_t fine_trim_32k : 7;
  uint32_t xtal1_trim_32k : 4;
  uint32_t xtal2_trim_32k : 4;
  uint32_t trim_ring_osc : 7;
  uint32_t vbatt_status_1 : 6;
  uint32_t str_temp_slope : 10;
  uint32_t f2_nominal : 10;
  uint32_t str_nominal_temp : 7;
  uint32_t str_bjt_temp_sense_off : 16;
  uint32_t str_bjt_temp_sense_slope : 16;
  uint32_t trim_sel_20Mhz : 7; // Trim value for 20mzh rc
  uint32_t ro_32khz_00_trim : 5;
  uint32_t scdc_dcdc_trim : 3;
  uint32_t scdc_hpldo_trim : 3;
  uint32_t reserved1 : 2;
  uint32_t ldo_ctrl : 4;
  uint32_t vbg_tsbjt_efuse : 12;
  uint32_t retn_ldo_lptrim : 3;
  uint32_t reserved2 : 1;
  uint32_t auxadc_offset_diff : 12;
  uint32_t auxadc_invgain_diff : 16;
  uint32_t auxadc_offset_single : 12;
  uint32_t auxadc_invgain_single : 16;
  uint32_t set_vref1p3 : 4;
  uint32_t reserved13 : 6;
  uint32_t trim_r1_resistorladder : 4;
  uint32_t retn_ldo_hptrim : 3;
  uint32_t reserved12 : 16;
  uint32_t scale_soc_ldo_vref : 1;
  uint32_t reserved11 : 7;
  uint32_t reserved10 : 12;
  uint32_t reserved9 : 10;
  uint32_t dpwm_freq_trim : 4;
  uint32_t reserved73 : 32; // 73 and 74 togther as 50
  uint32_t reserved74 : 18; //
  uint32_t scdc_clk_freq : 5;
  uint32_t reserved7 : 6;
  uint32_t buck_ind_efuse : 4;
  uint32_t reserved31 : 32; // 31,32 and 33 togther as 80
  uint32_t reserved32 : 32;
  uint32_t reserved33 : 16;
} __attribute__((__packed__)) efuse_ipmu_t;
#endif

/* Clock trim APL structure */
typedef enum INPUT_CLOCK {
  ulp_ref_clk           = 0,
  ulp_20mhz_ringosc_clk = 2,
  sleep_clk             = 8,
  soc_pll_clk           = 6

} INPUT_CLOCK_T;

typedef enum SLEEP_CLOCK { khz_rc_clk = 0x0, khz_xtal_clk = 0x1, khz_ro_clk = 0x3, none = 0x233 } SLEEP_CLOCK_T;

/******************************************************
   * *                 Global Variables
   * ******************************************************/

/******************************************************
   * *               Function Declarations
   * ******************************************************/
/**
   * \ingroup   RSI_SPECIFIC_DRIVERS
   * \defgroup  RSI_IPMU_DRIVERS RSI:RS1xxxx IPMU
   *  @{
   *
   */
rsi_error_t RSI_IPMU_Xtal2bias_Efuse(void);
rsi_error_t RSI_IPMU_Xtal1bias_Efuse(void);
uint32_t RSI_IPMU_Delvbe_Tsbjt_Efuse(void);
rsi_error_t RSI_IPMU_Dpwmfreq_TrimEfuse(void);
rsi_error_t RSI_IPMU_Ldosoc_TrimEfuse(void);
rsi_error_t RSI_IPMU_Buck_TrimEfuse(void);
rsi_error_t RSI_IPMU_POCbias_Efuse(void);
rsi_error_t RSI_IPMU_Blackout_TrimEfuse(void);
rsi_error_t RSI_IPMU_Bg_TrimEfuse(void);
uint32_t RSI_IPMU_Auxadcgain_SeEfuse(void);
uint32_t RSI_IPMU_Auxadcoff_SeEfuse(void);
uint32_t RSI_IPMU_Auxadcoff_DiffEfuse(void);
uint32_t RSI_IPMU_Auxadcgain_DiffEfuse(void);
uint32_t RSI_IPMU_Vbg_Tsbjt_Efuse(void);
uint32_t RSI_IPMU_RO_TsEfuse(void);
rsi_error_t RSI_IPMU_Vbattstatus_TrimEfuse(void);
rsi_error_t RSI_IPMU_RC32khz_TrimEfuse(void);
rsi_error_t RSI_IPMU_RC64khz_TrimEfuse(void);
rsi_error_t RSI_IPMU_RC16khz_TrimEfuse(void);
rsi_error_t RSI_IPMU_RO32khz_TrimEfuse(void);
rsi_error_t RSI_IPMU_M20roOsc_TrimEfuse(void);
rsi_error_t RSI_IPMU_DBLR32M_TrimEfuse(void);
rsi_error_t RSI_IPMU_M20rcOsc_TrimEfuse(void);
rsi_error_t RSI_IPMU_PMUCommonConfig(void);
rsi_error_t RSI_IPMU_M32rc_OscTrimEfuse(void);
void RSI_IPMU_PowerGateSet(uint32_t mask_vlaue);
void RSI_IPMU_PowerGateClr(uint32_t mask_vlaue);
rsi_error_t RSI_IPMU_CommonConfig(void);
void RSI_IPMU_ClockMuxSel(uint8_t bg_pmu_clk);
uint32_t RSI_IPMU_32MHzClkClib(void);
rsi_error_t RSI_IPMU_ProgramConfigData(uint32_t *config);
void RSI_IPMU_InitCalibData(void);
void RSI_IPMU_UpdateIpmuCalibData_efuse(efuse_ipmu_t *ipmu_calib_data);
uint32_t RSI_APB_ProgramConfigData(uint32_t *config);
uint32_t RSI_IPMU_RO_TsConfig(void);
void RSI_Configure_DCDC_LowerVoltage(void);
void RSI_IPMU_32KHzRCClkClib(void);
void RSI_IPMU_32KHzROClkClib(void);
rsi_error_t RSI_IPMU_PocbiasCurrent11(void);
rsi_error_t RSI_IPMU_RO32khzTrim00Efuse(void);
rsi_error_t RSI_IPMU_RetnHP_Volttrim_Efuse(void);
rsi_error_t RSI_IPMU_PocbiasCurrent(void);
void RSI_IPMU_RetnLdoHpmode(void);
void RSI_IPMU_RetnLdoLpmode(void);
rsi_error_t RSI_IPMU_RetnLdo0p75(void);
rsi_error_t RSI_IPMU_RetnLdoVoltsel(void);
void RSI_IPMU_64KHZ_RCClktrim(void);
void RSI_IPMU_20M_ROClktrim(uint8_t clkfreq);
uint32_t RSI_Clks_Calibration(INPUT_CLOCK_T inputclk, SLEEP_CLOCK_T sleep_clk_type);
uint32_t RSI_Clks_Trim32MHzRC(uint32_t freq);
void RSI_IPMU_SCDC_Enable(void);
void RSI_IPMU_HP_LDO_Enable(void);
void RSI_Ipmu_Init(void);
void RSI_Configure_Ipmu_Mode(void);
void ipmu_init(void);
void configure_ipmu_mode(uint32_t mode);
uint32_t init_ipmu_calib_data(uint32_t m4_present);
void update_ipmu_data(uint32_t reg_addr, uint32_t reg_type, uint32_t data, uint32_t mask);
void update_efuse_system_configs(int data, uint32_t config_ptr[]);
rsi_error_t RSI_IPMU_BOD_ClksCommonconfig1(void);
rsi_error_t RSI_IPMU_BOD_ClksCommonconfig2(void);
rsi_error_t RSI_IPMU_Hpldo_volt_trim_efuse(void);
rsi_error_t RSI_IPMU_Scdc_volt_trim_efuse(void);
void RSI_IPMU_Reconfig_to_SCDCDC(void);
rsi_error_t RSI_IPMU_Lp_scdc_extcapmode(void);
rsi_error_t RSI_IPMU_BOD_Cmphyst(void);
/**
   * @} end of RSI_IPMU_DRIVERS
   */
/* @} end of RSI_IPMU_DRIVERS  */

#ifdef __cplusplus
}
#endif
#endif // RSI_IPMU_H
