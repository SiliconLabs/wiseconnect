/**
 *  ******* @file       rsi_ipmu_config.c
 *   ******* @version    1.18
 *    ******* @date       04 APR 2018
 *     *******
 *      ******* Copyright(C) Silicon Labs 2016
 *       ******* All rights reserved by Silicon Labs.
 *        *******
 *         ******* @section License
 *          ******* This program should be used on your own responsibility.
 *           ******* Silicon Labs assumes no responsibility for any losses
 *            ******* incurred by customers or third parties arising from the use of this file.
 *             *******
 *              ******* @brief This file contains the programming values for IPMU
 *               **               ***               ***
 *                *                *                *                *                *                *** @section Description
 *                 *                 *                 *                 *                 *                 ***
 *                  *                  *                  *                  *                  *                  ***
 *                   *                   *                   *                   *                   *                   ***/

// Revision History
// // // // // // // // // Date           |  version number  |  Modification
// // // // // // // // // 08/01/2017        1.0                Initial version
// // // // // // // // // 03/19/2018        1.15                Latest
// // // // // // // // // 04/04/2018        1.17               WuRX LDO Config Updates
// // // // // // // // // 05/04/2018        1.18               Added pmu_LP_config, pmu_HP_config, pmu_socldo_0p95, pmu_socldo_1p0
// // // // // // // // // 05/16/2018        1.18               Modified PMU_LP_config, pmu_HP_config
// // // // // // // // // 05/25/2018        1.18               Merged PMU_LP_COMFIG into PMU_COMMON_CONFIG
// // // // // // // // // 08/02/2018        1.19               Added features to support RO_32K_00, LP_EXTCAP_DCDC, pmu structures
// // // // //
// // // //
// // //
// //
// //
// //
//

#ifdef CHIP_9118
#include <data_types.h>

#ifdef ONEBOXE
#define _COMMON_IPMU_RAM_ __attribute__((section(".rodata .rodata.")));
#else
#define _COMMON_IPMU_RAM_ __attribute__((section(".common_tcm_rodata")));
#endif

#else
#define _COMMON_IPMU_RAM_ ;
typedef unsigned int uint32;
#endif

uint32 ipmu_common_config[] _COMMON_IPMU_RAM_ //IPMU1
  uint32 ipmu_common_config[] = {
    8,          0x2405A49C, 0x62800001, 0x2405A498, 0xA83E002F, 0x2405A4A0, 0xA8200020, 0x2405A4A4, 0xA4800001,
    0x2405A4A4, 0x84000001, 0x2405A500, 0x9BC00019, 0x2405A494, 0x10800001, 0x2405A414, 0x63000001,
  };
uint32 pmu_common_config[] _COMMON_IPMU_RAM_ //PMU1
  uint32 pmu_common_config[] = {
    17,         0x24058740, 0xA440000A, 0x24058740, 0x600000BF, 0x24058748, 0x60000026, 0x24058748, 0x94400003,
    0x2405874C, 0x70000560, 0x24058768, 0x20C00003, 0x24058744, 0x9CC00001, 0x24058760, 0x10800001, 0x24058778,
    0x41400007, 0x24058758, 0x580000CB, 0x24058774, 0xA826249A, 0x24058774, 0x41800004, 0x24058740, 0x83400008,
    0x24058770, 0x7B000005, 0x24058770, 0x10000004, 0x24058774, 0xAD000002, 0x24058770, 0x84000001,
  };
uint32 pmu_LP_config[] _COMMON_IPMU_RAM_ //PMU2
  uint32 pmu_LP_config[] = {
    6,          0x24058774, 0x41800007, 0x24058748, 0x83400001, 0x24058740, 0x83400003,
    0x24058770, 0x7B000002, 0x24058770, 0x10000007, 0x24058774, 0xAD000002,
  };
uint32 pmu_HP_config[] _COMMON_IPMU_RAM_ //PMU3
  uint32 pmu_HP_config[] = {
    5,          0x24058774, 0xA826249A, 0x24058748, 0x83400004, 0x24058740,
    0x83400009, 0x24058770, 0x7B00000C, 0x24058770, 0x10000004,
  };
uint32 pmu_socldo_0p95[] _COMMON_IPMU_RAM_ //PMU4
  uint32 pmu_socldo_0p95[] = {
    1,
    0x24058778,
    0x18000008,
  };
uint32 pmu_socldo_1p0[] _COMMON_IPMU_RAM_ //PMU5
  uint32 pmu_socldo_1p0[] = {
    1,
    0x24058778,
    0x18000009,
  };
uint32 ultrasleep_less_than_2V[] _COMMON_IPMU_RAM_ //PMU6
  uint32 ultrasleep_less_than_2V[] = {
    2, 0x24058760, 0xAC800003, 0x24058770, 0xAD400001,
  };
uint32 scdc_volt_sel1[] _COMMON_IPMU_RAM_ //SCDC0
  uint32 scdc_volt_sel1[] = {
    2, 0x2405A49C, 0x49C00000, 0x2405A4A0, 0xAD400000,
  };
uint32 scdc_volt_sel2[] _COMMON_IPMU_RAM_ //SCDC0_1
  uint32 scdc_volt_sel2[] = {
    2, 0x2405A498, 0xA34000F0, 0x2405A4A0, 0xAD400001,
  };
uint32 lp_scdc_extcapmode[] _COMMON_IPMU_RAM_ //SCDC1
  uint32 lp_scdc_extcapmode[] = {
    6,          0x2405A49C, 0x49C00000, 0x2405A4A0, 0xAD400000, 0x2405A49C, 0x62800005,
    0x2405A498, 0xA34000F8, 0x2405A4A0, 0x51800010, 0x2405A4A0, 0xAD400001,
  };
uint32 hp_ldo_voltsel[] _COMMON_IPMU_RAM_ //SCDC2
  uint32 hp_ldo_voltsel[] = {
    2, 0x2405A49C, 0x49C00000, 0x2405A4A0, 0xAD400000,
  };
uint32 hpldo_tran[] _COMMON_IPMU_RAM_ //SCDC3
  uint32 hpldo_tran[] = {
    1,
    0x2405A4A0,
    0xAD400000,
  };
uint32 SCDC_tran[] _COMMON_IPMU_RAM_ //SCDC4
  uint32 SCDC_tran[] = {
    3, 0x2405A498, 0xA34000F8, 0x2405A4A0, 0x51800010, 0x2405A4A0, 0xAD400001,
  };
uint32 retn_ldo_voltsel[] _COMMON_IPMU_RAM_ //RETN0
  uint32 retn_ldo_voltsel[] = {
    1,
    0x2405A4A4,
    0xA4800002,
  };
uint32 retn_ldo_0p75[] _COMMON_IPMU_RAM_ //RETN3
  uint32 retn_ldo_0p75[] = {
    1,
    0x2405A4A4,
    0xA4800001,
  };
uint32 retn_ldo_lpmode[] _COMMON_IPMU_RAM_ //RETN1
  uint32 retn_ldo_lpmode[] = {
    3, 0x2405A4A4, 0xA4800000, 0x2405A4A4, 0x84000001, 0x2405A4A4, 0xA4800001,
  };
uint32 retn_ldo_hpmode[] _COMMON_IPMU_RAM_ //RETN2
  uint32 retn_ldo_hpmode[] = {
    3, 0x2405A4A4, 0xA4800000, 0x2405A4A4, 0x84000000, 0x2405A4A4, 0xA4800001,
  };
uint32 poc_bias_current[] _COMMON_IPMU_RAM_ //POC1
  uint32 poc_bias_current[] = {
    1,
    0x2405A500,
    0x9C800000,
  };
uint32 poc_bias_current_11[] _COMMON_IPMU_RAM_ //POC2
  uint32 poc_bias_current_11[] = {
    1,
    0x2405A500,
    0x9C800003,
  };
uint32 ref_sel_pmu[] _COMMON_IPMU_RAM_ //BG_PMU_PFM
  uint32 ref_sel_pmu[] = {
    1,
    0x2405A49C,
    0x10000000,
  };
uint32 bg_sleep_time[] _COMMON_IPMU_RAM_ //BG_SLEEP1
  uint32 bg_sleep_time[] = {
    1,
    0x2405A494,
    0xAC800008,
  };
uint32 ana_perif_ptat_common_config1[] _COMMON_IPMU_RAM_ //ANAPERIF_BIAS_EN
  uint32 ana_perif_ptat_common_config1[] = {
    1,
    0x2405A49C,
    0x18C00001,
  };
uint32 ana_perif_ptat_common_config2[] _COMMON_IPMU_RAM_ //ANAPERIF_BIAS_DIS
  uint32 ana_perif_ptat_common_config2[] = {
    1,
    0x2405A49C,
    0x18C00000,
  };
uint32 ipmu_bod_clks_common_config1[] _COMMON_IPMU_RAM_ //BOD_BIAS_EN
  uint32 ipmu_bod_clks_common_config1[] = {
    1,
    0x2405A49C,
    0x21000001,
  };
uint32 ipmu_bod_clks_common_config2[] _COMMON_IPMU_RAM_ //BOD_BIAS_DIS
  uint32 ipmu_bod_clks_common_config2[] = {
    1,
    0x2405A49C,
    0x21000000,
  };
uint32 xtal1_khz_fast_start_en[] _COMMON_IPMU_RAM_ //XTAL1_FS_EN
  uint32 xtal1_khz_fast_start_en[] = {
    1,
    0x2405A438,
    0xA440000F,
  };
uint32 xtal1_khz_fast_start_disable[] _COMMON_IPMU_RAM_ //XTAL1_FS_DIS
  uint32 xtal1_khz_fast_start_disable[] = {
    1,
    0x2405A438,
    0xA4400007,
  };
uint32 xtal2_khz_fast_start_en[] _COMMON_IPMU_RAM_ //XTAL2_FS_EN
  uint32 xtal2_khz_fast_start_en[] = {
    1,
    0x2405A438,
    0xA440000F,
  };
uint32 xtal2_khz_fast_start_disable[] _COMMON_IPMU_RAM_ //XTAL2_FS_DIS
  uint32 xtal2_khz_fast_start_disable[] = {
    1,
    0x2405A438,
    0xA4400007,
  };
uint32 button_wakeup_thresh[] _COMMON_IPMU_RAM_ //BOD_BUTTON
  uint32 button_wakeup_thresh[] = {
    1,
    0x2405A004,
    0xA0830556,
  };
uint32 bod_cmp_hyst[] _COMMON_IPMU_RAM_ //BOD_CMP_HYST
  uint32 bod_cmp_hyst[] = {
    1,
    0x2405A004,
    0x8000000,
  };
uint32 buck_op_ctrl[] _COMMON_IPMU_RAM_ //BUCK1
  uint32 buck_op_ctrl[] = {
    1,
    0x24058740,
    0xA440000A,
  };
uint32 ldosoc_op_ctrl[] _COMMON_IPMU_RAM_ //LDOSOC3
  uint32 ldosoc_op_ctrl[] = {
    1,
    0x24058758,
    0x2800000C,
  };
uint32 ldoflash_op_ctrl[] _COMMON_IPMU_RAM_ //LDORF3
  uint32 ldoflash_op_ctrl[] = {
    1,
    0x24058758,
    0x59800003,
  };
uint32 ro_tempsense_config[] _COMMON_IPMU_RAM_ //RO_TS_CONFIG
  uint32 ro_tempsense_config[] = {
    1,
    0x2405A4D0,
    0x800000F2,
  };
uint32 bjt_tempsense_config[] _COMMON_IPMU_RAM_ //BJT_TS_CONFIG
  uint32 bjt_tempsense_config[] = {
    1,
    0x2405A4D4,
    0x78000001,
  };
uint32 poc_bias_efuse[] _COMMON_IPMU_RAM_ //POC_BIAS_EFUSE
  uint32 poc_bias_efuse[] = {
    1,
    0x2405A500,
    0x9C800003,
  };
uint32 bg_trim_efuse[] _COMMON_IPMU_RAM_ //BG_TRIM_EFUSE
  uint32 bg_trim_efuse[] = {
    2, 0x2405A500, 0x63000000, 0x2405A49C, 0xAC000010,
  };
uint32 blackout_trim_efuse[] _COMMON_IPMU_RAM_ //POC3
  uint32 blackout_trim_efuse[] = {
    1,
    0x2405A500,
    0x5A800000,
  };
uint32 m32rc_osc_trim_efuse[] _COMMON_IPMU_RAM_ //M32RC_EFUSE
  uint32 m32rc_osc_trim_efuse[] = {
    1,
    0x2405A410,
    0xA380005A,
  };
uint32 m20rc_osc_trim_efuse[] _COMMON_IPMU_RAM_ //M20RC_EFUSE
  uint32 m20rc_osc_trim_efuse[] = {
    1,
    0x2405A410,
    0xA3800038,
  };
uint32 dblr_32m_trim_efuse[] _COMMON_IPMU_RAM_ //DBLR_EFUSE
  uint32 dblr_32m_trim_efuse[] = {
    1,
    0x2405A404,
    0xA3C00020,
  };
uint32 ro_32khz_trim_efuse[] _COMMON_IPMU_RAM_ //RO32K_EFUSE
  uint32 ro_32khz_trim_efuse[] = {
    1,
    0x2405A408,
    0xA4000007,
  };
uint32 ro_32khz_trim00_efuse[] _COMMON_IPMU_RAM_ //RO32K_00_EFUSE
  uint32 ro_32khz_trim00_efuse[] = {
    1,
    0x2405A408,
    0xA4000015,
  };
uint32 rc_16khz_trim_efuse[] _COMMON_IPMU_RAM_ //RC16K_EFUSE
  uint32 rc_16khz_trim_efuse[] = {
    1,
    0x2405A40C,
    0xA3000100,
  };
uint32 rc_64khz_trim_efuse[] _COMMON_IPMU_RAM_ //RC64K_EFUSE
  uint32 rc_64khz_trim_efuse[] = {
    1,
    0x2405A40C,
    0xA3000103,
  };
uint32 rc_32khz_trim_efuse[] _COMMON_IPMU_RAM_ //RC32K_EFUSE
  uint32 rc_32khz_trim_efuse[] = {
    1,
    0x2405A40C,
    0xA3000102,
  };
uint32 xtal1_bias_efuse[] _COMMON_IPMU_RAM_ //XTAL1_BIAS_EFUSE
  uint32 xtal1_bias_efuse[] = {
    1,
    0x2405A438,
    0x83400007,
  };
uint32 xtal2_bias_efuse[] _COMMON_IPMU_RAM_ //XTAL2_BIAS_EFUSE
  uint32 xtal2_bias_efuse[] = {
    1,
    0x2405A438,
    0x83400007,
  };
uint32 m20ro_osc_trim_efuse[] _COMMON_IPMU_RAM_ //M20RO_EFUSE
  uint32 m20ro_osc_trim_efuse[] = {
    1,
    0x2405A414,
    0xA3800027,
  };
uint32 vbatt_status_trim_efuse[] _COMMON_IPMU_RAM_ //VBATT_MON_EFUSE
  uint32 vbatt_status_trim_efuse[] = {
    1,
    0x2405A438,
    0x28000000,
  };
uint32 ro_ts_efuse[] _COMMON_IPMU_RAM_ //RO_TS_EFUSE
  uint32 ro_ts_efuse[] = {
    1,
    0x2405A4D8,
    0x4800010E,
  };
uint32 vbg_tsbjt_efuse[] _COMMON_IPMU_RAM_ //VBG_TS_EFUSE
  uint32 vbg_tsbjt_efuse[] = {
    1,
    0x2405A4DC,
    0x580004BF,
  };
uint32 delvbe_tsbjt_efuse[] _COMMON_IPMU_RAM_ //DELVBE_TS_EFUSE
  uint32 delvbe_tsbjt_efuse[] = {
    1,
    0x2405A4E0,
    0x500003D9,
  };
uint32 auxadc_off_diff_efuse[] _COMMON_IPMU_RAM_ //AUXADC_DIFFOFF_EFUSE
  uint32 auxadc_off_diff_efuse[] = {
    1,
    0x2405A4C0,
    0x58000064,
  };
uint32 auxadc_gain_diff_efuse[] _COMMON_IPMU_RAM_ //AUXADC_DIFFGAIN_EFUSE
  uint32 auxadc_gain_diff_efuse[] = {
    1,
    0x2405A4C4,
    0x78008000,
  };
uint32 auxadc_off_se_efuse[] _COMMON_IPMU_RAM_ //AUXADC_SEOFF_EFUSE
  uint32 auxadc_off_se_efuse[] = {
    1,
    0x2405A4C8,
    0x58000064,
  };
uint32 auxadc_gain_se_efuse[] _COMMON_IPMU_RAM_ //AUXADC_SEGAIN_EFUSE
  uint32 auxadc_gain_se_efuse[] = {
    1,
    0x2405A4CC,
    0x78008000,
  };
uint32 buck_trim_efuse[] _COMMON_IPMU_RAM_ //BUCK_TRIM_EFUSE
  uint32 buck_trim_efuse[] = {
    1,
    0x24058740,
    0xA440000A,
  };
uint32 ldosoc_trim_efuse[] _COMMON_IPMU_RAM_ //LDOSOC_TRIM_EFUSE
  uint32 ldosoc_trim_efuse[] = {
    1,
    0x2405877C,
    0x18000000,
  };
uint32 dpwm_freq_trim_efuse[] _COMMON_IPMU_RAM_ //DPWM_FREQ_TRIM_EFUSE
  uint32 dpwm_freq_trim_efuse[] = {
    1,
    0x24058748,
    0x83400004,
  };
uint32 scdc_volt_trim_efuse[] _COMMON_IPMU_RAM_ //SCDC_TRIM_EFUSE
  uint32 scdc_volt_trim_efuse[] = {
    1,
    0x2405A49C,
    0x62800000,
  };
uint32 hpldo_volt_trim_efuse[] _COMMON_IPMU_RAM_ //HPLDO_TRIM_EFUSE
  uint32 hpldo_volt_trim_efuse[] = {
    1,
    0x2405A49C,
    0x49C00004,
  };
uint32 retnHP_volt_trim_efuse[] _COMMON_IPMU_RAM_ //RETNHP_TRIM_EFUSE
  uint32 retnHP_volt_trim_efuse[] = {
    1,
    0x2405A4A4,
    0xA4800000,
  };
uint32 retnLP_volt_trim_efuse[] _COMMON_IPMU_RAM_ //RETNLP_TRIM_EFUSE
  uint32 retnLP_volt_trim_efuse[] = {
    1,
    0x2405A4A4,
    0xA4800000,
  };
