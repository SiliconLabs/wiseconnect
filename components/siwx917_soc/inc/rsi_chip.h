/*******************************************************************************
* @file  rsi_chip.h
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

#ifndef __RSI_CHIP_H__
#define __RSI_CHIP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "base_types.h"
#include "rsi_ccp_common.h"
#include "core_cm4.h"
#include "rsi_error.h"
#ifdef __ICCARM__
#include "cmsis_iar.h"
#endif

/**/
#include "rsi_error.h"
#include "rsi_timers.h"
#include "rsi_ulpss_clk.h"
#include "rsi_pll.h"
#include "rsi_power_save.h"
#include "rsi_ps_config.h"
#include "rsi_egpio.h"
#include "rsi_fim.h"
#ifndef CHIP_917
#include "rsi_cci.h"
#endif
#include "rsi_crc.h"
#include "rsi_efuse.h"
#include "rsi_ulpss_clk.h"
#include "rsi_pwm.h"
#include "rsi_qei.h"
#include "rsi_qspi_proto.h"
#include "rsi_qspi.h"
#include "rsi_rng.h"
#include "rsi_gpdma.h"
#include "rsi_ct.h"
#include "rsi_sio.h"
#include "rsi_timers.h"
#include "rsi_udma.h"
#include "rsi_rtc.h"
#include "rsi_reg_spi.h"
#include "rsi_processor_sensor.h"
#include "rsi_retention.h"
#include "rsi_temp_sensor.h"
#include "rsi_time_period.h"
#include "rsi_wwdt.h"
#include "rsi_efuse.h"
#include "rsi_ipmu.h"
#include "rsi_adc.h"
#include "rsi_dac.h"
#include "rsi_comparator.h"
#include "rsi_wurx.h"
#ifndef CHIP_917
#include "rsi_vad.h"
#endif
#include "rsi_smih.h"
#include "rsi_sdioh.h"
#include "rsi_sdmem.h"
#include "rsi_opamp.h"
#include "aux_reference_volt_config.h"

#include "rsi_rom_egpio.h"
#include "rsi_rom_timer.h"
#include "rsi_rom_udma.h"
#include "rsi_rom_udma_wrapper.h"
#include "rsi_rom_ct.h"
#include "rsi_rom_gpdma.h"
#include "rsi_rom_clks.h"
#include "rsi_rom_ulpss_clk.h"
#include "rsi_rom_qspi.h"
#include "rsi_rom_fim.h"
#include "rsi_rom_efuse.h"
#include "rsi_rom_crc.h"
#include "rsi_rom_rng.h"
#include "rsi_rom_pwm.h"
#include "rsi_rom_power_save.h"

#include "rsi_system_config.h"

#ifdef __cplusplus
}
#endif
#endif /* __RSI_CHIP_H__ */
