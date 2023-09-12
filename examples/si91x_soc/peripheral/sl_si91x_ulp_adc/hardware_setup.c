/*******************************************************************************
* @file  hardware_setup.c
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

/*Includes*/

#define NVIC_RTC       MCU_CAL_RTC_IRQn /*<! RTC NVIC enable   */
#define NVIC_RTC_ALARM MCU_CAL_ALARM_IRQn

#include "rsi_chip.h"
#include "rsi_board.h"
#include "rsi_ps_ram_func.h"
#include "rsi_ds_timer.h"

void hardware_setup(void)
{
  RSI_PS_FsmLfClkSel(KHZ_RC_CLK_SEL);
  /*Change the TASS reference clock to 32MHz RC clock
  NOTE: This should not be used in WiSeMCU mode , should be used in MCU only mode
  */
  RSI_ChangeTassRefClock();

  RSI_CLK_PeripheralClkDisable3(M4CLK,
                                M4_SOC_CLK_FOR_OTHER_ENABLE); /* Disable OTHER_CLK which is enabled at Start-up */
  RSI_ULPSS_TimerClkDisable(ULPCLK);                          /* Disable Timer clock which is enabled in Bootloader */

  RSI_ULPSS_DisableRefClks(MCU_ULP_40MHZ_CLK_EN);    /* Disabling 40MHz Clocks */
  RSI_ULPSS_DisableRefClks(MCU_ULP_32KHZ_RC_CLK_EN); /* Disabling LF_RC Clocks */

  RSI_PS_BodPwrGateButtonCalibDisable();                     /* Power-Down Button Calibration */
  RSI_IPMU_ProgramConfigData(ana_perif_ptat_common_config2); /* Disable PTAT for Analog Peripherals */
  RSI_IPMU_ProgramConfigData(ipmu_bod_clks_common_config2);  /* Disable PTAT for Brown-Out Detection Clocks */

  /* Power-Down Analog Peripherals */
  RSI_IPMU_PowerGateClr(AUXDAC_PG_ENB | AUXADC_PG_ENB | WURX_CORR_PG_ENB | WURX_PG_ENB | ULP_ANG_CLKS_PG_ENB
                        | CMP_NPSS_PG_ENB);

  /* Power-Down Domains in NPSS */
  RSI_PS_NpssPeriPowerDown(SLPSS_PWRGATE_ULP_MCUWDT | SLPSS_PWRGATE_ULP_MCUPS | SLPSS_PWRGATE_ULP_MCUTS
                           | SLPSS_PWRGATE_ULP_MCUSTORE2 | SLPSS_PWRGATE_ULP_MCUSTORE3);

  RSI_PS_PowerSupplyDisable(POWER_ENABLE_TIMESTAMPING);

  RSI_PS_SocPllSpiDisable();     /* Power-Down High-Frequency PLL Domain */
  RSI_PS_QspiDllDomainDisable(); /* Power-Down QSPI-DLL Domain */
  RSI_PS_XtalDisable();          /* Power-Down XTAL-OSC Domain */

  RSI_PS_WirelessShutdown(); /* Shutdown Wireless since Wireless Processor is not present */
  /* Change 32MHz-RC to 20MHz-RC to be used as Processor Clock in PS2 state */
  //RSI_IPMU_M20rcOsc_TrimEfuse();
  RSI_PS_FsmHfFreqConfig(20);
  system_clocks.rc_32mhz_clock = 20000000;

  /* by using this API we programmed the RTC timer clock in SOC
  MSB 8 bits for the Integer part &
  LSB 17bits for the Fractional part
  Ex: 32Khz clock = 31.25us ==> 31.25*2^17 = 4096000 = 0x3E8000*/
  /* Time Period Programming */
  RSI_TIMEPERIOD_TimerClkSel(TIME_PERIOD, 0x003E7FFF);

  /* Switching from PS4 to PS2 state */
  RSI_PS_PowerStateChangePs4toPs2(ULP_MCU_MODE,
                                  PWR_MUX_SEL_ULPSSRAM_SCDC_0_9,
                                  PWR_MUX_SEL_M4_ULP_RAM_SCDC_0_9,
                                  PWR_MUX_SEL_M4_ULP_RAM16K_SCDC_0_9,
                                  PWR_MUX_SEL_M4ULP_SCDC_0_6,
                                  PWR_MUX_SEL_ULPSS_SCDC_0_9,
                                  DISABLE_BG_SAMPLE_ENABLE,
                                  DISABLE_DC_DC_ENABLE,
                                  DISABLE_SOCLDO_ENABLE,
                                  DISABLE_STANDBYDC,
                                  DISABLE_TA192K_RAM_RET,
                                  DISABLE_M464K_RAM_RET);

#ifdef CHIP_917
  /* Power-Down Unused M4SS Domains*/

  RSI_PS_M4ssPeriPowerDown(M4SS_PWRGATE_ULP_QSPI_ICACHE | M4SS_PWRGATE_ULP_EFUSE_PERI | M4SS_PWRGATE_ULP_SDIO_SPI
                           | M4SS_PWRGATE_ULP_RPDMA);
  /* Power-Down Unused ULPSS Domains */
  RSI_PS_UlpssPeriPowerDown(ULPSS_PWRGATE_ULP_AUX | ULPSS_PWRGATE_ULP_CAP | ULPSS_PWRGATE_ULP_UART
                            | ULPSS_PWRGATE_ULP_SSI | ULPSS_PWRGATE_ULP_I2S | ULPSS_PWRGATE_ULP_I2C
                            | ULPSS_PWRGATE_ULP_IR | ULPSS_PWRGATE_ULP_UDMA | ULPSS_PWRGATE_ULP_FIM);
  /*Turn off ULPSS SRAM Core/Periphery domains*/
  RSI_PS_UlpssRamBanksPowerDown(ULPSS_2K_BANK_2 | ULPSS_2K_BANK_3);
  RSI_PS_UlpssRamBanksPeriPowerDown(ULPSS_2K_BANK_2 | ULPSS_2K_BANK_3);
  /*Turn off M4SS Unused SRAM Core/Periphery domains*/
  RSI_PS_M4ssRamBanksPowerDown(RAM_BANK_8 | RAM_BANK_9);
  RSI_PS_M4ssRamBanksPeriPowerDown(RAM_BANK_8 | RAM_BANK_9);
#endif
}
