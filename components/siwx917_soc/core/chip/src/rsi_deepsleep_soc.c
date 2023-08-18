/*******************************************************************************
* @file  rsi_deepsleep_soc.c
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
#include "rsi_ccp_common.h"
#include "rsi_chip.h"
#include "rsi_board.h"

void fpuInit(void);
#define NWPAON_MEM_HOST_ACCESS_CTRL_CLEAR_1 (*(volatile uint32_t *)(0x41300000 + 0x4))
#define NWPAON_MEM_HOST_ACCESS_CTRL_SET_1   (*(volatile uint32_t *)(0x41300000 + 0x0))
#define M4SS_TASS_CTRL_SET_REG              (*(volatile uint32_t *)(0x24048400 + 0x34))
#define M4SS_TASS_CTRL_CLEAR_REG            (*(volatile uint32_t *)(0x24048400 + 0x38))
#define M4SS_TASS_CTRL_CLR_REG              (*(volatile uint32_t *)(0x24048400 + 0x38))
#define MAX_NVIC_REGS                       4   // Max Interrupts register
#define MAX_IPS                             240 // Max Interrupt Priority registers
#define MAX_SHP                             12  //Max System Handlers Priority registers
uint32_t nvic_enable[MAX_NVIC_REGS] = { 0 };
uint8_t nvic_ip_reg[MAX_IPS]        = { 0 };
uint8_t scs_shp_reg[MAX_SHP]        = { 0 };
extern uint32_t __sp;
volatile uint32_t msp_value, psp_value, control_reg_val;
/**
 * @fn           void RSI_Save_Context(void)
 * @brief        This function is to save Stack pointer value and Control registers.
 *
 */
void RSI_Save_Context(void)
{
  msp_value       = __get_MSP();
  psp_value       = __get_PSP();
  control_reg_val = __get_CONTROL();
}
/**
 * @fn           void RSI_Restore_Context(void)
 * @brief        This function is to Restore Stack pointer value and Control registers.
 *
 */
STATIC INLINE void RSI_Restore_Context(void)
{
  __set_CONTROL(control_reg_val);
  __set_PSP(psp_value);
  __set_MSP(msp_value);
}
#if defined(__CC_ARM) /*------------------ARM CC Compiler -----------------*/
/**
 * @fn          __asm  void RSI_PS_SaveCpuContext(void)
 * @brief        This API is used to save the CPU status register into RAM, this API should be used when sleep with RET is required
 * @return       none
 */
__asm void RSI_PS_SaveCpuContext(void)
{
  IMPORT __sp;
  PUSH{ r0 };
  PUSH{ r1 };
  PUSH{ r2 };
  PUSH{ r3 };
  PUSH{ r4 };
  PUSH{ r5 };
  PUSH{ r6 };
  PUSH{ r7 };
  PUSH{ r8 };
  PUSH{ r9 };
  PUSH{ r10 };
  PUSH{ r11 };
  PUSH{ r12 };
  PUSH{ r14 };
  LDR r0, = __sp;
  MRS r1, msp;
  STR r1, [r0];
  WFI;
}

/**
 * @fn           void RSI_PS_RestoreCpuContext(void)
 * @brief        This API is used to restore the current CPU processing content from (POP) stack
 * @return       none
 */
__asm void RSI_PS_RestoreCpuContext(void)
{
  IMPORT __sp;
  LDR r0, = __sp;
  LDR sp, [r0, #0];
  POP{ r14 };
  POP{ r12 };
  POP{ r11 };
  POP{ r10 };
  POP{ r9 };
  POP{ r8 };
  POP{ r7 };
  POP{ r6 };
  POP{ r5 };
  POP{ r4 };
  POP{ r3 };
  POP{ r2 };
  POP{ r1 };
  POP{ r0 };
  BX LR;
}
#endif /*------------------ARM CC Compiler -----------------*/

#if defined(__GNUC__) /*------------------ GNU Compiler ---------------------*/

/**
 * @fn           void RSI_PS_SaveCpuContext(void)
 * @brief        This API is used to save the CPU status register into RAM, this API should be used when sleep with RET is required
 * @return       none
 */
void RSI_PS_SaveCpuContext(void)
{
  __asm("push {r0}");
  __asm("push {r1}");
  __asm("push {r2}");
  __asm("push {r3}");
  __asm("push {r4}");
  __asm("push {r5}");
  __asm("push {r6}");
  __asm("push {r7}");
  __asm("push {r8}");
  __asm("push {r9}");
  __asm("push {r10}");
  __asm("push {r11}");
  __asm("push {r12}");
  __asm("push {r14}");

  /*R13 Stack pointer */
  __asm("mov %0, sp\n\t" : "=r"(__sp));
  __asm("WFI");
}

/**
 * @fn           void RSI_PS_RestoreCpuContext(void)
 * @brief        This API is used to restore the current CPU processing content from (POP) stack
 * @return       none
 */
void RSI_PS_RestoreCpuContext(void)
{
  __asm("ldr r0 , =__sp");
  __asm("ldr sp , [r0 , #0]");
  __asm("pop {r14}");
  __asm("pop {r12}");
  __asm("pop {r11}");
  __asm("pop {r10}");
  __asm("pop {r9}");
  __asm("pop {r8}");
  __asm("pop {r7}");
  __asm("pop {r6}");
  __asm("pop {r5}");
  __asm("pop {r4}");
  __asm("pop {r3}");
  __asm("pop {r2}");
  __asm("pop {r1}");
  __asm("pop {r0}");
}
#endif /*------------------ GNU Compiler ---------------------*/

#if defined(__ICCARM__) /*------------------ IAR Compiler ---------------------*/
/**
 * @fn           void RSI_PS_SaveCpuContext(void)
 * @brief        This API is used to save the CPU status register into RAM, this API should be used when sleep with RET is required
 * @return       none
 */
void RSI_PS_SaveCpuContext(void)
{
  __asm("push {r0}");
  __asm("push {r1}");
  __asm("push {r2}");
  __asm("push {r3}");
  __asm("push {r4}");
  __asm("push {r5}");
  __asm("push {r6}");
  __asm("push {r7}");
  __asm("push {r8}");
  __asm("push {r9}");
  __asm("push {r10}");
  __asm("push {r11}");
  __asm("push {r12}");
  __asm("push {r14}");

  /*R13 Stack pointer */
  __asm("mov %0, sp\n\t" : "=r"(__sp));
  __asm("WFI");
}

/**
 * @fn           void RSI_PS_RestoreCpuContext(void)
 * @brief        This API is used to restore the current CPU processing content from (POP) stack
 * @return       none
 */
void RSI_PS_RestoreCpuContext(void)
{
  __asm("ldr r0 , =__sp");
  __asm("ldr sp , [r0 , #0]");
  __asm("pop {r14}");
  __asm("pop {r12}");
  __asm("pop {r11}");
  __asm("pop {r10}");
  __asm("pop {r9}");
  __asm("pop {r8}");
  __asm("pop {r7}");
  __asm("pop {r6}");
  __asm("pop {r5}");
  __asm("pop {r4}");
  __asm("pop {r3}");
  __asm("pop {r2}");
  __asm("pop {r1}");
  __asm("pop {r0}");
}
#endif /*------------------ IAR Compiler ---------------------*/

/**
 * @fn          error_t RSI_PS_EnterDeepSleep(SLEEP_TYPE_T sleepType , uint8_t lf_clk_mode)
 * @brief	    This is the common API to keep the system in sleep state. from all possible active states.
 * @param[in]   sleepType   : selects the retention or non retention mode of processor. refer 'SLEEP_TYPE_T'.
 *                              SLEEP_WITH_RETENTION : When this is used, user must configure the which RAMs to be retained during sleep by using the 'RSI_PS_SetRamRetention()' function.
 * @param[in]   lf_clk_mode : This parameter is used to switch the processor clock from high frequency clock to low-frequency clock. This is used in some critical power save cases.
 *                            0: 'DISABLE_LF_MODE' Normal mode of operation , recommended in most of the applications.
 *                            1: 'LF_32_KHZ_RC' Processor clock is configured to low-frequency RC clock
 *                            2: 'LF_32_KHZ_XTAL' Processor clock is configured to low-frequency XTAL clock
 *                            3: 'EXTERNAL_CAP_MODE' Switches the supply to internal cap mode 0.95V.
 * @par           note
 \n User must ensure the selected clocks are active before selecting the 'LF_32_KHZ_RC' and 'LF_32_KHZ_XTAL' clocks to the processor using this API.
 * @return      Returns the execution status.
 */
error_t RSI_PS_EnterDeepSleep(SLEEP_TYPE_T sleepType, uint8_t lf_clk_mode)
{
  volatile int var = 0, enable_sdcss_based_wakeup = 0, enable_m4ulp_retention = 0, Temp;
  uint32_t ipmuDummyRead = 0, m4ulp_ram_core_status = 0, m4ulp_ram_peri_status = 0, disable_pads_ctrl = 0,
           ulp_proc_clk         = 0;
  volatile uint8_t in_ps2_state = 0, x = 0;

  /*Save the NVIC registers */
  for (var = 0; var < MAX_NVIC_REGS; ++var) {
    nvic_enable[var] = NVIC->ISER[var];
  }
  /* Save the Interrupt Priority Register */
  for (var = 0; var < MAX_IPS; ++var) {
    nvic_ip_reg[var] = NVIC->IP[var];
  }
  /* Save the System Handlers Priority Registers */
  for (var = 0; var < MAX_SHP; ++var) {
    scs_shp_reg[var] = SCB->SHP[var];
  }
  /*store the NPSS interrupt mask clear status*/
  npssIntrState = NPSS_INTR_MASK_CLR_REG;

  /*Clear AUX and DAC pg enables */
  if (!((MCU_FSM->MCU_FSM_SLEEP_CTRLS_AND_WAKEUP_MODE_b.SDCSS_BASED_WAKEUP_b)
        || (MCU_FSM->MCU_FSM_SLEEP_CTRLS_AND_WAKEUP_MODE_b.ULPSS_BASED_WAKEUP_b))) {
    RSI_IPMU_PowerGateClr(AUXDAC_PG_ENB | AUXADC_PG_ENB);
  }
  /*Check the PS2 state or not*/
  if (M4_ULP_SLP_STATUS_REG & ULP_MODE_SWITCHED_NPSS) {

    in_ps2_state = 1U;

    if (!((MCU_FSM->MCU_FSM_SLEEP_CTRLS_AND_WAKEUP_MODE_b.SDCSS_BASED_WAKEUP_b)
          || (MCU_FSM->MCU_FSM_SLEEP_CTRLS_AND_WAKEUP_MODE_b.ULPSS_BASED_WAKEUP_b))) {
      disable_pads_ctrl = (ULP_SPI_MEM_MAP(0x141) & BIT(11)); // ULP PADS PDO Status
      ULP_SPI_MEM_MAP(0x141) &= ~(BIT(11));                   // ULP PADS PDO OFF
      enable_sdcss_based_wakeup = 1;
      RSI_PS_SetWkpSources(SDCSS_BASED_WAKEUP);
    }

    if (!(MCU_FSM->MCU_FSM_SLEEP_CTRLS_AND_WAKEUP_MODE_b.M4ULP_RAM_RETENTION_MODE_EN_b)) {
      enable_m4ulp_retention = 1;
      m4ulp_ram_core_status  = RSI_PS_M4ssRamBanksGetPowerSts();
      m4ulp_ram_peri_status  = RSI_PS_M4ssRamBanksGetPeriPowerSts();
      RSI_PS_M4ssRamBanksPowerDown(RAM_BANK_7 | RAM_BANK_8 | RAM_BANK_9);
      RSI_PS_M4ssRamBanksPeriPowerDown(RAM_BANK_7 | RAM_BANK_8 | RAM_BANK_9);
#ifdef CHIP_9118
      RSI_PS_SetRamRetention(M4ULP_RAM_RETENTION_MODE_EN);
#endif
    }
  }
  /*Move to LP mode */
  RSI_IPMU_RetnLdoLpmode();
  if (sleepType == SLEEP_WITHOUT_RETENTION) {
    /*POC1 */
    RSI_IPMU_PocbiasCurrent();
    /*RO32K_00_EFUSE. */
    RSI_IPMU_RO32khzTrim00Efuse();
    /*RETN1 */
    RSI_IPMU_RetnLdoLpmode();
    /*RETN0 */
    RSI_IPMU_RetnLdoVoltsel();
  }

  // Before Sleep
  if (!((in_ps2_state) && (MCU_FSM->MCU_FSM_SLEEP_CTRLS_AND_WAKEUP_MODE_b.ULPSS_BASED_WAKEUP_b))) {
#if (XTAL_CAP_MODE == POWER_TARN_CONDITIONAL_USE)
    if (lf_clk_mode & BIT(4)) {
      RSI_PS_NpssPeriPowerUp(SLPSS_PWRGATE_ULP_MCUTS);
      /*configure the slope,nominal temperature and f2_nominal*/
      RSI_TS_Config(MCU_TEMP, 25);
      /*disable the bjt based temp sensor*/
      RSI_TS_RoBjtEnable(MCU_TEMP, 0);
      /*Enable the RO based temp sensor*/
      RSI_TS_Enable(MCU_TEMP, 1);
      /*update the temperature periodically*/
      RSI_Periodic_TempUpdate(TIME_PERIOD, 1, 0);
      /*read the temperature*/
      Temp = (int)RSI_TS_ReadTemp(MCU_TEMP);
      if (Temp > 45) {
        // disable the XTAL CAP mode
        RSI_IPMU_ProgramConfigData(lp_scdc_extcapmode);
      }
    }
#endif

#if (XTAL_CAP_MODE == POWER_TARN_ALWAYS_USE)
    // disable the XTAL CAP mode
    RSI_IPMU_ProgramConfigData(lp_scdc_extcapmode);
#endif
  }

  if (!((lf_clk_mode == HF_MHZ_RO))) {
    /*Clear IPMU BITS*/
    RSI_PS_LatchCntrlClr(LATCH_TOP_SPI | LATCH_TRANSPARENT_HF | LATCH_TRANSPARENT_LF);
  }

  ipmuDummyRead = MCU_FSM->MCU_FSM_CLK_ENS_AND_FIRST_BOOTUP;

  /*Update the SCB with Deep sleep BIT */
  SCB->SCR = 0x4;

  if (in_ps2_state) {
    /*Read processor clock */
    ulp_proc_clk = ULPCLK->ULP_TA_CLK_GEN_REG_b.ULP_PROC_CLK_SEL;

    /*LF processor clock configuration */
    switch (lf_clk_mode & 0x7) {
      case DISABLE_LF_MODE:
        /*Do nothing*/
        break;
      case LF_32_KHZ_RC:
        ULPCLK->ULP_TA_CLK_GEN_REG_b.ULP_PROC_CLK_SEL = 2U;
        break;
      case LF_32_KHZ_XTAL:
        ULPCLK->ULP_TA_CLK_GEN_REG_b.ULP_PROC_CLK_SEL = 3U;
        break;
      case HF_MHZ_RO:
        ULPCLK->ULP_TA_CLK_GEN_REG_b.ULP_PROC_CLK_SEL = 5U;
        break;
      default:
        return INVALID_PARAMETERS;
    }
    /* HF processor clock */
  }
#ifdef RSI_WITH_OS
  /* Save Stack pointer value and Control registers */
  RSI_Save_Context();
#endif
#if ((defined COMMON_FLASH_EN) && (!(defined(RAM_COMPILATION))))
  /* Reset M4_USING_FLASH bit before going to sleep */
  M4SS_P2P_INTR_CLR_REG = M4_USING_FLASH;
  /*Before M4 is going to deep sleep , set m4ss_ref_clk_mux_ctrl ,tass_ref_clk_mux_ctr, AON domain power supply controls form M4 to TA */
#ifdef CHIP_917B0
  MCUAON_CONTROL_REG4 |= (MCU_TASS_REF_CLK_SEL_MUX_CTRL);
#else
  NWPAON_MEM_HOST_ACCESS_CTRL_SET_1 = TASS_REF_CLK_MUX_CTRL;
#endif
#endif
  /*Enter sleep with retention*/
  if (sleepType == SLEEP_WITH_RETENTION) {
    /*If retention mode is enabled save the CPU context*/
    RSI_PS_SaveCpuContext();
  } else {
    /*Clear RAM retentions*/
    RSI_PS_ClrRamRetention(M4ULP_RAM16K_RETENTION_MODE_EN | TA_RAM_RETENTION_MODE_EN | M4ULP_RAM_RETENTION_MODE_EN
                           | M4SS_RAM_RETENTION_MODE_EN);
    /*do not save CPU context and go to deep sleep */
    __asm("WFI");
  }
#ifdef RSI_WITH_OS
  /* Restore Stack pointer value and Control registers */
  RSI_Restore_Context();
#endif
  /*Restore the default value to the processor clock */
  if ((in_ps2_state)) {
    ULPCLK->ULP_TA_CLK_GEN_REG_b.ULP_PROC_CLK_SEL = (unsigned int)(ulp_proc_clk & 0xF);
  }

#if ((defined COMMON_FLASH_EN) && (!(defined(RAM_COMPILATION))))
  /* Before TA is going to power save mode, set m4ss_ref_clk_mux_ctrl ,tass_ref_clk_mux_ctrl ,AON domain power supply controls form TA to M4 */
  RSI_Set_Cntrls_To_M4();
#endif
  /*Update the REG Access SPI division factor to increase the SPI read/write speed*/
  if (lf_clk_mode == HF_MHZ_RO) {
    RSI_SetRegSpiDivision(0U);
  } else {
    RSI_SetRegSpiDivision(1U);
  }
  /*IPMU dummy read to make IPMU block out of RESET*/
  ipmuDummyRead = ULP_SPI_MEM_MAP(0x144);
  // After Wakeup
  if (!((in_ps2_state) && (MCU_FSM->MCU_FSM_SLEEP_CTRLS_AND_WAKEUP_MODE_b.ULPSS_BASED_WAKEUP_b))) {
#if (XTAL_CAP_MODE == POWER_TARN_CONDITIONAL_USE)
    if (lf_clk_mode & BIT(4)) {
      // disable the XTAL CAP mode
      RSI_PS_NpssPeriPowerUp(SLPSS_PWRGATE_ULP_MCUTS);
      /*configure the slope,nominal temperature and f2_nominal*/
      RSI_TS_Config(MCU_TEMP, 25);
      /*disable the bjt based temp sensor*/
      RSI_TS_RoBjtEnable(MCU_TEMP, 0);
      /*Enable the RO based temp sensor*/
      RSI_TS_Enable(MCU_TEMP, 1);
      /*update the temperature periodically*/
      RSI_Periodic_TempUpdate(TIME_PERIOD, 1, 0);
      /*read the temperature*/
      Temp = (int)RSI_TS_ReadTemp(MCU_TEMP);
      if (Temp > 45) {
        //SCDC0
        RSI_IPMU_ProgramConfigData(scdc_volt_sel1);
        RSI_IPMU_ProgramConfigData(scdc_volt_trim_efuse);
        //SCDC0_1
        RSI_IPMU_ProgramConfigData(scdc_volt_sel2);
      }
    }
#endif

#if (XTAL_CAP_MODE == POWER_TARN_ALWAYS_USE)
    // disable the XTAL CAP mode
    //SCDC0
    RSI_IPMU_ProgramConfigData(scdc_volt_sel1);
    RSI_IPMU_ProgramConfigData(scdc_volt_trim_efuse);
    //SCDC0_1
    RSI_IPMU_ProgramConfigData(scdc_volt_sel2);
#endif
  }
  if (!(lf_clk_mode == HF_MHZ_RO)) {
    /*Spare register write sequence*/
    ipmuDummyRead          = ULP_SPI_MEM_MAP(0x1C1);
    ULP_SPI_MEM_MAP(0x141) = ipmuDummyRead;

    ipmuDummyRead          = ULP_SPI_MEM_MAP(0x1C0);
    ULP_SPI_MEM_MAP(0x140) = ipmuDummyRead;
    RSI_PS_LatchCntrlSet(LATCH_TOP_SPI);
  }
  if (in_ps2_state) {
    /*Come out  of LP  mode */
    /* enabling the RETN_LDO HP MODE */
    RSI_IPMU_RetnLdoHpmode();
  }
  if (sleepType == SLEEP_WITHOUT_RETENTION) {
    /* 	Increasing the bias current of RETN_LDO */
    RSI_IPMU_PocbiasCurrent11();
    RSI_IPMU_RO32khz_TrimEfuse();
    /* enabling the RETN_LDO HP MODE */
    RSI_IPMU_RetnLdoHpmode();
  }
  /*I2S-PLL Bypass*/
  *(volatile uint32_t *)(0x24041400 + 0x3C) |= BIT(0);

  if (enable_sdcss_based_wakeup) {
    RSI_PS_ClrWkpSources(SDCSS_BASED_WAKEUP);
    enable_sdcss_based_wakeup = 0;
  }
  if (enable_m4ulp_retention) {
    RSI_PS_M4ssRamBanksPowerUp(m4ulp_ram_core_status);
    RSI_PS_M4ssRamBanksPeriPowerUp(m4ulp_ram_peri_status);
    enable_m4ulp_retention = 0;
  }
  if (disable_pads_ctrl) {
    ULP_SPI_MEM_MAP(0x141) |= (BIT(11)); // ULP PADS PDO ON
    disable_pads_ctrl = 0;
  }
/*Start of M4 init after wake up  */
#ifdef DEBUG_UART
  fpuInit();
  /*Initialize UART after wake up*/
  DEBUGINIT();
#endif
  /*Restore the NVIC registers */
  for (var = 0; var < MAX_NVIC_REGS; ++var) {
    NVIC->ISER[var] = nvic_enable[var];
  }
  /*Restore the Interrupt Priority Register  */
  for (var = 0; var < MAX_IPS; ++var) {
    NVIC->IP[var] = nvic_ip_reg[var];
  }
  /* Restore the System Handlers Priority Registers */
  for (var = 0; var < MAX_SHP; ++var) {
    SCB->SHP[var] = scs_shp_reg[var];
  }
  /*Restore NPSS INTERRUPTS*/
  NPSS_INTR_MASK_CLR_REG = ~npssIntrState;
#ifndef CHIP_917
  M4CLK->CLK_ENABLE_SET_REG1_b.M4SS_UM_CLK_STATIC_EN_b = 0x1;
#endif
  for (x = 0; x < 200; x++) {
    __ASM("NOP");
  }
#ifndef CHIP_917
  M4CLK->CLK_ENABLE_CLR_REG1_b.M4SS_UM_CLK_STATIC_EN_b = 0x1;
#endif
  /* Restore the NPSS domains*/

  return RSI_OK;
}
