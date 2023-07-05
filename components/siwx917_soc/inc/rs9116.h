/*******************************************************************************
* @file  rs9116.h
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

/** @addtogroup SiLabs Signals Inc.
 * @{
 */

/** @addtogroup RSI9116
 * @{
 */

#ifndef __RSI9116_H__
#define __RSI9116_H__

#include "base_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum {
  /* -------------------  Cortex-M4 Processor Exceptions Numbers  ------------------- */
  Reset_IRQn            = -15, /*!<   1  Reset Vector, invoked on Power up and warm reset                 */
  NonMaskableInt_IRQn   = -14, /*!<   2  Non maskable Interrupt, cannot be stopped or preempted           */
  HardFault_IRQn        = -13, /*!<   3  Hard Fault, all classes of Fault                                 */
  MemoryManagement_IRQn = -12, /*!<   4  Memory Management, MPU mismatch, including Access Violation
                                                         and No Match                                                          */
  BusFault_IRQn         = -11, /*!<   5  Bus Fault, Pre-Fetch-, Memory Access Fault, other address/memory
                                                         related Fault                                                         */
  UsageFault_IRQn       = -10, /*!<   6  Usage Fault, i.e. Undef Instruction, Illegal State Transition    */
  SVCall_IRQn           = -5,  /*!<  11  System Service Call via SVC instruction                          */
  DebugMonitor_IRQn     = -4,  /*!<  12  Debug Monitor                                                    */
  PendSV_IRQn           = -2,  /*!<  14  Pendable request for system service                              */
  SysTick_IRQn          = -1,  /*!<  15  System Tick Timer                                                */
  /* ---------------------  RSI9116 Specific Interrupt Numbers  --------------------- */
  TIMER0_IRQn                      = 2, /*!<   1  TIMER0                                                           */
  TIMER1_IRQn                      = 3, /*!<   2  TIMER1                                                           */
  TIMER2_IRQn                      = 4, /*!<   3  TIMER2                                                           */
  TIMER3_IRQn                      = 5,
  UDMA1_IRQn                       = 10, /*!< 10  UDMA1                                                           */
  ULPSS_USART_IRQn                 = 12, /*!<  12  ULPSS USART/UART                                                */
  I2C2_IRQn                        = 13, /*!<  13  I2C2                                                            */
  I2S1_IRQn                        = 14, /*!<  14  I2S2 */
  SSI2_IRQn                        = 16, /*!<  14  SSI2                                                            */
  FIM_IRQn                         = 17,
  ULP_EGPIO_PIN_IRQn               = 18,
  ULP_EGPIO_GROUP_IRQn             = 19,
  NPSS_TO_MCU_WDT_INTR_IRQn        = 20, /*!<  20 WDT interrupt */
  NPSS_TO_MCU_GPIO_INTR_IRQn       = 21,
  NPSS_TO_MCU_CMP_RF_WKP_INTR_IRQn = 22,
  NPSS_TO_MCU_BOD_INTR_IRQn        = 23,
  NPSS_TO_MCU_BUTTON_INTR_IRQn     = 24,
  NPSS_TO_MCU_SDC_INTR_IRQn        = 25,
  NPSS_TO_MCU_WIRELESS_INTR_IRQn   = 26,
  NPSS_MCU_INTR_IRQn               = 27,
  MCU_CAL_ALARM_IRQn               = 28, /*!<  Alarm interrupt */
  MCU_CAL_RTC_IRQn                 = 29, /*!<  Alarm interrupt */
  RPDMA_IRQn                       = 31, /*!<  31  RPDMA                                                            */
  UDMA0_IRQn                       = 33, /*!<  33  UDMA0                                                            */
  SCT_IRQn                         = 34, /*!<  34  SCT                                                              */
  HIF_IRQ0                         = 35, /*!<  35  HIF0                                                             */
  HIF_IRQ1                         = 36, /*!<  36  HIF1                                                             */
  SIO_IRQn                         = 37, /*!<  37  SIO                                                              */
  USART1_IRQn                      = 38, /*!<  38  USART0                                                           */
  USART2_IRQn                      = 39, /*!<  39  USART1  */
  USART3_IRQn                      = 40, /*!<  41  CAN0  	                                                         */
  EGPIO_WAKEUP_IRQn                = 41, /*!<  41  EGPIO_WAKEUP																										 */
  I2C0_IRQn                        = 42, /*!<  42  I2C0                                                             */
  SSI1_IRQn                        = 44, /*!<  44  SSI1                                                             */
  GSPI0_IRQn                       = 46, /*!<  46  GSPI0                                                            */
  SSI0_IRQn                        = 47, /*!<  47  SSI0                                                             */
  MCPWM_IRQn                       = 48, /*!<  48  MCPWM                                                            */
  QEI_IRQn                         = 49, /*!<  23  QEI                  */
  EGPIO_GROUP_0_IRQn               = 50, /*!<  50  EGPIO_GROUP_0                                                    */
  EGPIO_GROUP_1_IRQn               = 51, /*!<  51  EGPIO_GROUP_1                                                    */
  EGPIO_PIN_0_IRQn                 = 52, /*!<  52  EGPIO_PIN_0                                                      */
  EGPIO_PIN_1_IRQn                 = 53, /*!<  53  EGPIO_PIN_1                                                      */
  EGPIO_PIN_2_IRQn                 = 54, /*!<  54  EGPIO_PIN_2                                                      */
  EGPIO_PIN_3_IRQn                 = 55, /*!<  55  EGPIO_PIN_3                                                      */
  EGPIO_PIN_4_IRQn                 = 56, /*!<  56  EGPIO_PIN_4                                                      */
  EGPIO_PIN_5_IRQn                 = 57, /*!<  57  EGPIO_PIN_5                                                      */
  EGPIO_PIN_6_IRQn                 = 58, /*!<  58  EGPIO_PIN_6                                                      */
  EGPIO_PIN_7_IRQn                 = 59, /*!<  59  EGPIO_PIN_7                                                      */
  QSPI_IRQn                        = 60, /*!<  60  QSPI                                                             */
  I2C1_IRQn                        = 61, /*!<  61  I2C1                                                             */
  ETHERNET_IRQn                    = 62, /*!<  62  ETHERNET                                                         */
  ETHERNET_PMT_IRQn                = 63, /*!<  63  ETHERNET PMT                                                     */
  I2S0_IRQn                        = 64,
  CAN1_IRQn                        = 66, /*!<  67  CAN1                                                             */
  SDMEM_IRQn                       = 68, /*!<  68  SDMEM interrput*/
  PLL_CLOCK_IRQn                   = 69, /*!<  69  PLL CLOCK INTERRUOT */
  CCI_IRQn                         = 71  /*!<  71  CCI                                                              */
} IRQn_Type;

/** @addtogroup Configuration_of_CMSIS
 * @{
 */

/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* ----------------Configuration of the Cortex-M4 Processor and Core Peripherals---------------- */
#define __CM4_REV              0x0100 /*!< Cortex-M4 Core Revision                                               */
#define __MPU_PRESENT          1      /*!< MPU present or not                                                    */
#define __NVIC_PRIO_BITS       4      /*!< Number of Bits used for Priority Levels                               */
#define __Vendor_SysTickConfig 0      /*!< Set to 1 if different SysTick Config is used                          */
#define __FPU_PRESENT          1      /*!< FPU present or not                                                    */
/** @} */                             /* End of group Configuration_of_CMSIS */

#include "core_cm4.h"      /*!< Cortex-M4 processor and core peripherals                              */
#include "system_rs9116.h" /*!< RSI9116 System                                                        */

/* ================================================================================ */
/* ================       Device Specific Peripheral Section       ================ */
/* ================================================================================ */

/** @addtogroup Device_Peripheral_Registers
 * @{
 */

/* -------------------  Start of section using anonymous unions  ------------------ */
#if defined(__CC_ARM)
#pragma push
#pragma anon_unions
#elif defined(__ICCARM__)
#pragma language = extended
#elif defined(__GNUC__)
/* anonymous unions are enabled by default */
#elif defined(__TMS470__)
/* anonymous unions are enabled by default */
#elif defined(__TASKING__)
#pragma warning 586
#else
#warning Not supported compiler type
#endif

typedef struct {
  union {
    __IO uint32_t INTR_REG; /*!< Interrupt Register                                                    */

    struct {
      __IO uint32_t RPDMAC_INT_STAT_0 : 1; /*!< Interrupt Status                                                      */
      __IO uint32_t RPDMAC_INT_STAT_1 : 1; /*!< Interrupt Status                                                      */
      __IO uint32_t RPDMAC_INT_STAT_2 : 1; /*!< Interrupt Status                                                      */
      __IO uint32_t RPDMAC_INT_STAT_3 : 1; /*!< Interrupt Status                                                      */
      __IO uint32_t RPDMAC_INT_STAT_4 : 1; /*!< Interrupt Status                                                      */
      __IO uint32_t RPDMAC_INT_STAT_5 : 1; /*!< Interrupt Status                                                      */
      __IO uint32_t RPDMAC_INT_STAT_6 : 1; /*!< Interrupt Status                                                      */
      __IO uint32_t RPDMAC_INT_STAT_7 : 1; /*!< Interrupt Status                                                      */
    } INTR_REG_b;                          /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t INTR_MASK_REG; /*!< Interrupt Mask Register                                               */

    struct {
      __IO uint32_t : 8;                  /*!< Interrupt Mask for channel 0                                          */
      __IO uint32_t LINK_LIST_DONE_0 : 1; /*!< Interrupt Mask for channel 1                                          */
      __IO uint32_t LINK_LIST_DONE_1 : 1; /*!< Interrupt Mask for channel 2                                          */
      __IO uint32_t LINK_LIST_DONE_2 : 1; /*!< Interrupt Mask for channel 3                                          */
      __IO uint32_t LINK_LIST_DONE_3 : 1; /*!< Interrupt Mask for channel 4                                          */
      __IO uint32_t LINK_LIST_DONE_4 : 1; /*!< Interrupt Mask for channel 5                                          */
      __IO uint32_t LINK_LIST_DONE_5 : 1; /*!< Interrupt Mask for channel 6                                          */
      __IO uint32_t LINK_LIST_DONE_6 : 1; /*!< Interrupt Mask for channel 7                                          */
      __IO uint32_t LINK_LIST_DONE_7 : 1;
      __IO uint32_t TRANSFER_DONE_0 : 1; /*!< Interrupt Mask for channel 1                                          */
      __IO uint32_t TRANSFER_DONE_1 : 1; /*!< Interrupt Mask for channel 2                                          */
      __IO uint32_t TRANSFER_DONE_2 : 1; /*!< Interrupt Mask for channel 3                                          */
      __IO uint32_t TRANSFER_DONE_3 : 1; /*!< Interrupt Mask for channel 4                                          */
      __IO uint32_t TRANSFER_DONE_4 : 1; /*!< Interrupt Mask for channel 5                                          */
      __IO uint32_t TRANSFER_DONE_5 : 1; /*!< Interrupt Mask for channel 6                                          */
      __IO uint32_t TRANSFER_DONE_6 : 1; /*!< Interrupt Mask for channel 7                                          */
      __IO uint32_t TRANSFER_DONE_7 : 1;
      __IO uint32_t : 8;

    } INTR_MASK_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t INTR_STAT_REG; /*!< Interrupt Status Register                                             */

    struct {
      __IO uint32_t HRESP_ERR_0 : 1;              /*!< (1)Set if we receive hresp error or (2)transfer size/burst size
                                                         /h size mismatch (3)flow ctrl err                                     */
      __IO uint32_t RPDMAC_LINK_LIST_DONE_0 : 1;  /*!< Bit per channel: This bit is set when the next link list pointer
                                                         points to zero.                                                       */
      __IO uint32_t RPDMAC_PHRL_END_OF_TFR_0 : 1; /*!< peripheral indicates last burst or single transfer.Bit is set
                                                         at the end of last beat or transfer.                                  */
      __IO uint32_t RPDMAC_ERR_0 : 1; /*!< (1) transfer size/burst size /h size mismatch (2)flow ctrl err        */
      __IO uint32_t HRESP_ERR_1 : 1;  /*!< (1)Set if we receive hresp error or (2)transfer size/burst size
                                                         /h size mismatch (3)flow ctrl err                                     */
      __IO uint32_t RPDMAC_LINK_LIST_DONE_1 : 1;  /*!< Bit per channel: This bit is set when the next link list pointer
                                                         points to zero.                                                       */
      __IO uint32_t RPDMAC_PHRL_END_OF_TFR_1 : 1; /*!< peripheral indicates last burst or single transfer.Bit is set
                                                         at the end of last beat or transfer.                                  */
      __IO uint32_t RPDMAC_ERR_1 : 1; /*!< (1) transfer size/burst size /h size mismatch (2)flow ctrl err        */
      __IO uint32_t HRESP_ERR_2 : 1;  /*!< (1)Set if we receive hresp error or (2)transfer size/burst size
                                                         /h size mismatch (3)flow ctrl err                                     */
      __IO uint32_t RPDMAC_LINK_LIST_DONE_2 : 1;  /*!< Bit per channel: This bit is set when the next link list pointer
                                                         points to zero.                                                       */
      __IO uint32_t RPDMAC_PHRL_END_OF_TFR_2 : 1; /*!< peripheral indicates last burst or single transfer.Bit is set
                                                         at the end of last beat or transfer.                                  */
      __IO uint32_t RPDMAC_ERR_2 : 1; /*!< (1) transfer size/burst size /h size mismatch (2)flow ctrl err        */
      __IO uint32_t HRESP_ERR_3 : 1;  /*!< (1)Set if we receive hresp error or (2)transfer size/burst size
                                                         /h size mismatch (3)flow ctrl err                                     */
      __IO uint32_t RPDMAC_LINK_LIST_DONE_3 : 1;  /*!< Bit per channel: This bit is set when the next link list pointer
                                                         points to zero.                                                       */
      __IO uint32_t RPDMAC_PHRL_END_OF_TFR_3 : 1; /*!< peripheral indicates last burst or single transfer.Bit is set
                                                         at the end of last beat or transfer.                                  */
      __IO uint32_t RPDMAC_ERR_3 : 1; /*!< (1) transfer size/burst size /h size mismatch (2)flow ctrl err        */
      __IO uint32_t HRESP_ERR_4 : 1;  /*!< (1)Set if we receive hresp error or (2)transfer size/burst size
                                                         /h size mismatch (3)flow ctrl err                                     */
      __IO uint32_t RPDMAC_LINK_LIST_DONE_4 : 1;  /*!< Bit per channel: This bit is set when the next link list pointer
                                                         points to zero.                                                       */
      __IO uint32_t RPDMAC_PHRL_END_OF_TFR_4 : 1; /*!< peripheral indicates last burst or single transfer.Bit is set
                                                         at the end of last beat or transfer.                                  */
      __IO uint32_t RPDMAC_ERR_4 : 1; /*!< (1) transfer size/burst size /h size mismatch (2)flow ctrl err        */
      __IO uint32_t HRESP_ERR_5 : 1;  /*!< (1)Set if we receive hresp error or (2)transfer size/burst size
                                                         /h size mismatch (3)flow ctrl err                                     */
      __IO uint32_t RPDMAC_LINK_LIST_DONE_5 : 1;  /*!< Bit per channel: This bit is set when the next link list pointer
                                                         points to zero.                                                       */
      __IO uint32_t RPDMAC_PHRL_END_OF_TFR_5 : 1; /*!< peripheral indicates last burst or single transfer.Bit is set
                                                         at the end of last beat or transfer.                                  */
      __IO uint32_t RPDMAC_ERR_5 : 1; /*!< (1) transfer size/burst size /h size mismatch (2)flow ctrl err        */
      __IO uint32_t HRESP_ERR_6 : 1;  /*!< (1)Set if we receive hresp error or (2)transfer size/burst size
                                                         /h size mismatch (3)flow ctrl err                                     */
      __IO uint32_t RPDMAC_LINK_LIST_DONE_6 : 1;  /*!< Bit per channel: This bit is set when the next link list pointer
                                                         points to zero.                                                       */
      __IO uint32_t RPDMAC_PHRL_END_OF_TFR_6 : 1; /*!< peripheral indicates last burst or single transfer.Bit is set
                                                         at the end of last beat or transfer.                                  */
      __IO uint32_t RPDMAC_ERR_6 : 1; /*!< (1) transfer size/burst size /h size mismatch (2)flow ctrl err        */
      __IO uint32_t HRESP_ERR_7 : 1;  /*!< (1)Set if we receive hresp error or (2)transfer size/burst size
                                                         /h size mismatch (3)flow ctrl err                                     */
      __IO uint32_t RPDMAC_LINK_LIST_DONE_7 : 1;  /*!< Bit per channel: This bit is set when the next link list pointer
                                                         points to zero.                                                       */
      __IO uint32_t RPDMAC_PHRL_END_OF_TFR_7 : 1; /*!< peripheral indicates last burst or single transfer.Bit is set
                                                         at the end of last beat or transfer.                                  */
      __IO uint32_t RPDMAC_ERR_7 : 1; /*!< (1) transfer size/burst size /h size mismatch (2)flow ctrl err        */
    } INTR_STAT_REG_b;                /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t DMA_CHNL_ENABLE_REG; /*!< Channel Enable Register                                               */

    struct {
      __IO uint32_t CH_ENB_0 : 1; /*!< channel 0 enable                                                      */
      __IO uint32_t CH_ENB_1 : 1; /*!< channel 1 enable                                                      */
      __IO uint32_t CH_ENB_2 : 1; /*!< channel 2 enable                                                      */
      __IO uint32_t CH_ENB_3 : 1; /*!< channel 3 enable                                                      */
      __IO uint32_t CH_ENB_4 : 1; /*!< channel 4 enable                                                      */
      __IO uint32_t CH_ENB_5 : 1; /*!< channel 5 enable                                                      */
      __IO uint32_t CH_ENB_6 : 1; /*!< channel 6 enable                                                      */
      __IO uint32_t CH_ENB_7 : 1; /*!< channel 7 enable                                                      */
    } DMA_CHNL_ENABLE_REG_b;      /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t DMA_CHNL_SQUASH_REG; /*!< Channel Squash Register                                               */

    struct {
      __IO uint32_t CH_SQA_0 : 1; /*!< Channel 0 fifo will be cleared, FSM will be reset.                    */
      __IO uint32_t CH_SQA_1 : 1; /*!< Channel 1 fifo will be cleared, FSM will be reset.                    */
      __IO uint32_t CH_SQA_2 : 1; /*!< Channel 2 fifo will be cleared, FSM will be reset.                    */
      __IO uint32_t CH_SQA_3 : 1; /*!< Channel 3 fifo will be cleared, FSM will be reset.                    */
      __IO uint32_t CH_SQA_4 : 1; /*!< Channel 4 fifo will be cleared, FSM will be reset.                    */
      __IO uint32_t CH_SQA_5 : 1; /*!< Channel 5 fifo will be cleared, FSM will be reset.                    */
      __IO uint32_t CH_SQA_6 : 1; /*!< Channel 6 fifo will be cleared, FSM will be reset.                    */
      __IO uint32_t CH_SQA_7 : 1; /*!< Channel 7 fifo will be cleared, FSM will be reset.                    */
    } DMA_CHNL_SQUASH_REG_b;      /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t DMA_CHNL_LOCK_REG; /*!< Channel Lock Register                                                 */

    struct {
      __IO uint32_t CHNL_LOCK_0 : 1; /*!< When set entire DMA transfer is done, before other DMA request
                                                         is serviced. This bit is reset to zero up on completion of DMA        */
      __IO uint32_t CHNL_LOCK_1 : 1; /*!< When set entire DMA transfer is done, before other DMA request
                                                         is serviced. This bit is reset to zero up on completion of DMA        */
      __IO uint32_t CHNL_LOCK_2 : 1; /*!< When set entire DMA transfer is done, before other DMA request
                                                         is serviced. This bit is reset to zero up on completion of DMA        */
      __IO uint32_t CHNL_LOCK_3 : 1; /*!< When set entire DMA transfer is done, before other DMA request
                                                         is serviced. This bit is reset to zero up on completion of DMA        */
      __IO uint32_t CHNL_LOCK_4 : 1; /*!< When set entire DMA transfer is done, before other DMA request
                                                         is serviced. This bit is reset to zero up on completion of DMA        */
      __IO uint32_t CHNL_LOCK_5 : 1; /*!< When set entire DMA transfer is done, before other DMA request
                                                         is serviced. This bit is reset to zero up on completion of DMA        */
      __IO uint32_t CHNL_LOCK_6 : 1; /*!< When set entire DMA transfer is done, before other DMA request
                                                         is serviced. This bit is reset to zero up on completion of DMA        */
      __IO uint32_t CHNL_LOCK_7 : 1; /*!< When set entire DMA transfer is done, before other DMA request
                                                         is serviced. This bit is reset to zero up on completion of DMA        */
    } DMA_CHNL_LOCK_REG_b;           /*!< BitSize                                                               */
  };
} RPDMA_G_GLOBAL_Type;

typedef struct {
  union {
    __IO uint32_t LINK_LIST_PTR_REGS; /*!< Link List Register for channel 0 to 7                                 */

    struct {
      __IO uint32_t LINK_LIST_POINTER : 8; /*!< This is the address of the memory location from which we get
                                                         our next descriptor                                                   */
    } LINK_LIST_PTR_REGS_b;                /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t SRC_ADDR_REGS; /*!< Source Address Register for channel 0                                 */

    struct {
      __IO uint32_t SRC_ADDR : 32; /*!< This is the address of the memory location from which we get
                                                         our next descriptor                                                   */
    } SRC_ADDR_REGS_b;             /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t DEST_ADDR_REGS; /*!< Source Address Register for channel 0                                 */

    struct {
      __IO uint32_t DEST_ADDR : 32; /*!< This is the destination address to whih the data is sent              */
    } DEST_ADDR_REGS_b;             /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t CHNL_CTRL_REGS; /*!< Channel Control Register for channel 0 to 7                           */

    struct {
      __IO uint32_t DMA_BLK_SIZE : 12;     /*!< This is data to be transmitted. Loaded at the beginning of the
                                                         DMA transfer and decremented at every dma transaction.                */
      __IO uint32_t TRNS_TYPE : 2;         /*!< DMA transfer type                                                     */
      __IO uint32_t DMA_FLOW_CTRL : 2;     /*!< DMA flow control                                                      */
      __IO uint32_t MSTR_IF_FETCH_SEL : 1; /*!< This selects the MASTER IF from which data to be fetched              */
      __IO uint32_t MSTR_IF_SEND_SEL : 1;  /*!< This selects the MASTER IF from which data to be sent                 */
      __IO uint32_t DEST_DATA_WIDTH : 2;   /*!< Data transfer to destination.                                         */
      __IO uint32_t SRC_DATA_WIDTH : 2;    /*!< Data transfer from source.                                            */
      __IO uint32_t SRC_ALIGN : 1;         /*!< Reserved.Value set to 0 We do not do any singles. We just do
                                                         burst, save first 3 bytes in to residue buffer in one cycle,
                                                          In the next cycle send 4 bytes to fifo, save 3 bytes in to residue.
                                                          This continues on.                                                   */
      __IO uint32_t LINK_LIST_ON : 1;      /*!< This mode is set, when we do link listed operation                    */
      __IO uint32_t
        LINK_LIST_MSTR_SEL : 1; /*!< This mode is set, when we do link listed operation                    */
      __IO uint32_t
        SRC_ADDR_CONTIGUOUS : 1; /*!< Indicates Address is contiguous from previous                         */
      __IO uint32_t
        DEST_ADDR_CONTIGUOUS : 1;       /*!< Indicates Address is contiguous from previous                         */
      __IO uint32_t RETRY_ON_ERROR : 1; /*!< When this bit is set, if we recieve HRESPERR, We will retry
                                                         the DMA for that channel.                                             */
      __IO uint32_t LINK_INTERRUPT : 1; /*!< This bit is set in link list descriptor.Hard ware will send
                                                         an interrupt when the DMA transfer is done for the corresponding
                                                          link list address                                                    */
      __IO uint32_t SRC_FIFO_MODE : 1;  /*!< If set to 1; source address will not be incremented(means fifo
                                                         mode for source)                                                      */
      __IO uint32_t DEST_FIFO_MODE : 1; /*!< If set to 1; destination address will not be incremented(means
                                                         fifo mode for destination)                                            */
    } CHNL_CTRL_REGS_b;                 /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t MISC_CHNL_CTRL_REGS; /*!< Misc Channel Control Register for channel 0                           */

    struct {
      __IO uint32_t AHB_BURST_SIZE : 3;  /*!< Burst size                                                            */
      __IO uint32_t DEST_DATA_BURST : 6; /*!< Burst writes in beats to destination.(000000-64 beats .....111111-63
                                                         beats)                                                                */
      __IO uint32_t SRC_DATA_BURST : 6;  /*!< Burst writes in beats from source(000000-64 beats .....111111-63
                                                         beats)                                                                */
      __IO uint32_t DEST_CHNL_ID : 6;    /*!< This is the destination channel Id to which the data is sent.
                                                         Must be set up prior to DMA_CHANNEL_ENABLE                            */
      __IO uint32_t SRC_CHNL_ID : 6;     /*!< This is the source channel Id, from which the data is fetched.
                                                         must be set up prior to DMA_CHANNEL_ENABLE                            */
      __IO uint32_t DMA_PROT : 3;        /*!< Protection level to go with the data. It will be concatenated
                                                         with 1 b1 as there will be no opcode fetching and directly assign
                                                          to hprot in AHB interface                                            */
      __IO uint32_t MEM_FILL_ENABLE : 1; /*!< Enable for memory filling with either 1s or 0s.                       */
      __IO uint32_t MEM_ONE_FILL : 1;    /*!< Select for memory filling with either 1s or 0s.                       */
    } MISC_CHNL_CTRL_REGS_b;             /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t FIFO_CONFIG_REGS; /*!< FIFO Configuration Register for channel 1                             */

    struct {
      __IO uint32_t FIFO_STRT_ADDR : 6; /*!< Starting row address of channel                                       */
      __IO uint32_t FIFO_SIZE : 6;      /*!< Channel size                                                          */
    } FIFO_CONFIG_REGS_b;               /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t PRIORITY_CHNL_REGS; /*!< Priority Register for channel 0 to 7                                  */

    struct {
      __IO uint32_t RPDMAC_PRI_CH : 2; /*!< Set a value between 2 b00 to 2 b11. The channel having highest
                                                         number is the highest priority channel.                               */
    } PRIORITY_CHNL_REGS_b;            /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED2[57];
} RPDMA_C_CHANNEL_CONFIG_Type;

typedef struct {
  union {
    __IO uint32_t TMR_MATCH; /*!< Timer Match Lower Byte Register                                       */

    struct {
      __IO uint32_t TMR_MATCH : 32; /*!< This bits are used to program the lower significant 16-bits
                                                         of timer time out value in millisecond or number of system clocks
			 */
    } TMR_MATCH_b;                  /*!< BitSize                                                               */
  };

  //  union {
  //    __IO uint16_t  TMR_MATCH_H;                     /*!< Timer Match Higher Byte Register                                      */
  //
  //    struct {
  //      __IO uint16_t  TMR_MATCH_L: 16;               /*!< This bits are used to program the lower significant 16-bits
  //                                                         of timer time out value in millisecond or number of system clocks
  //                                                                                                                               */
  //    } TMR_MATCH_H_b;                                /*!< BitSize                                                               */
  //  };

  union {
    __IO uint16_t TMR_CNTRL; /*!< Timer Control Register                                                */

    struct {
      __O uint16_t TMR_START : 1;        /*!< This Bit are Used to start the timer timer gets reset upon setting
                                                         this bit                                                              */
      __O uint16_t TMR_INTR_CLR : 1;     /*!< This Bit are Used to clear the timer                                  */
      __IO uint16_t TMR_INTR_ENABLE : 1; /*!< This Bit are Used to enable the time out interrupt                    */
      __IO uint16_t TMR_TYPE : 2;        /*!< This Bit are Used to select the type of timer                         */
      __IO uint16_t TMR_MODE : 1;        /*!< This Bit are Used to select the mode working of timer                 */
      __O uint16_t TMR_STOP : 1;         /*!< This Bit are Used to stop the timer                                   */
      __IO uint16_t COUNTER_UP : 1;      /*!< For reading/tracking counter in up counting this bit has to
                                                         be set                                                                */
    } TMR_CNTRL_b;                       /*!< BitSize                                                               */
  };
} TIMERS_MATCH_CTRL_Type;

typedef struct {
  union {
    __IO uint32_t GPIO_CONFIG_REG; /*!< GPIO Configuration Register                                           */

    struct {
      __IO uint32_t DIRECTION : 1; /*!< Direction of the GPIO pin                                             */
      __IO uint32_t PORTMASK : 1;  /*!< Port mask value   			*/
      __IO uint32_t MODE : 4;      /*!< GPIO Pin Mode Used for GPIO Pin Muxing                                */
      uint32_t : 2;
      __IO uint32_t GROUP_INTERRUPT1_ENABLE : 1;   /*!< When set, the corresponding GPIO is pin is selected for group
                                                         intr 1 generation                                                     */
      __IO uint32_t GROUP_INTERRUPT1_POLARITY : 1; /*!< Decides the active value of the pin to be considered for group
                                                         interrupt 1 generation                                                */
      __IO uint32_t GROUP_INTERRUPT2_ENABLE : 1;   /*!< When set, the corresponding GPIO is pin is selected for group
                                                         intr 2 generation                                                     */
      __IO uint32_t GROUP_INTERRUPT2_POLARITY : 1; /*!< Decides the active value of the pin to be considered for group
                                                         interrupt 2 generation                                                */
      uint32_t RES : 20;
    } GPIO_CONFIG_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t BIT_LOAD_REG; /*!< Bit Load                                                              */

    struct {
      __IO uint32_t BIT_LOAD : 1; /*!< Loads 0th bit on to the pin on write. And reads the value on
                                                         pin on read into 0th bit                                              */
      uint32_t RES : 31;
    } BIT_LOAD_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t WORD_LOAD_REG; /*!< Word Load                                                             */

    struct {
      __IO uint32_t WORD_LOAD : 16; /*!< Loads 1 on the pin when any of the bit in load value is 1. On
                                                         read pass the bit status into all bits.                               */
      uint32_t RES : 16;
    } WORD_LOAD_REG_b; /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED3;
} EGPIO0_PIN_CONFIG_Type;

typedef struct {
  union {
    __IO uint32_t PORT_LOAD_REG; /*!< Port Load                                                             */

    struct {
      __IO uint32_t PORT_LOAD : 16; /*!< Loads the value on to pin on write. And reads the value of load
                                                         register on read                                                      */
      uint32_t RES : 16;
    } PORT_LOAD_REG_b; /*!< BitSize                                                               */
  };

  union {
    __O uint32_t PORT_SET_REG; /*!< Port Set Register                                                     */

    struct {
      __O uint32_t PORT_SET : 16; /*!< Sets the pin when corresponding bit is high. Writing zero has
                                                         no effect.                                                            */
      uint32_t RES : 16;
    } PORT_SET_REG_b; /*!< BitSize                                                               */
  };

  union {
    __O uint32_t PORT_CLEAR_REG; /*!< Port Clear Register                                                   */

    struct {
      __O uint32_t PORT_CLEAR : 16; /*!< Clears the pin when corresponding bit is high. Writing zero
                                                         has no effect.                                                        */
      uint32_t RES : 16;
    } PORT_CLEAR_REG_b; /*!< BitSize                                                               */
  };

  union {
    __O uint32_t PORT_MASKED_LOAD_REG; /*!< Port Masked Load Register                                             */

    struct {
      __IO uint32_t PORT_MASKED_LOAD : 16; /*!< Only loads into pins which are not masked. On read, pass only
                                                         status unmasked pins                                                  */
      uint32_t RES : 16;
    } PORT_MASKED_LOAD_REG_b; /*!< BitSize                                                               */
  };

  union {
    __O uint32_t PORT_TOGGLE_REG; /*!< Port Toggle Register                                                  */

    struct {
      __O uint32_t PORT_TOGGLE : 16; /*!< Toggles the pin when corresponding bit is high. Writing zero
                                                         has not effect.                                                       */
      uint32_t RES : 16;
    } PORT_TOGGLE_REG_b; /*!< BitSize                                                               */
  };

  union {
    __I uint32_t PORT_READ_REG; /*!< Port Read Register                                                    */

    struct {
      __I uint32_t PORT_READ : 16; /*!< Reads the value on GPIO pins irrespective of the pin mode.            */
      uint32_t RES : 16;
    } PORT_READ_REG_b; /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED4[2];
} EGPIO0_PORT_CONFIG_Type;

typedef struct {
  union {
    __IO uint32_t GPIO_INTR_CTRL; /*!< GPIO Interrupt Control Register                                       */

    struct {
      __IO uint32_t LEVEL_HIGH_ENABLE : 1; /*!< enables interrupt generation when pin level is 1                      */
      __IO uint32_t LEVEL_LOW_ENABLE : 1;  /*!< enables interrupt generation when pin level is 0                      */
      __IO uint32_t RISE_EDGE_ENABLE : 1;  /*!< enables interrupt generation when rising edge is detected on
                                                         pin                                                                   */
      __IO uint32_t FALL_EDGE_ENABLE : 1;  /*!< enables interrupt generation when Falling edge is detected on
                                                         pin                                                                   */
      __IO uint32_t MASK : 1;              /*!< Masks the interrupt. Interrupt will still be seen in status
                                                         register when enabled                                                 */
      uint32_t : 3;
      __IO uint32_t PIN_NUMBER : 4;  /*!< GPIO Pin to be chosen for interrupt generation                        */
      __IO uint32_t PORT_NUMBER : 2; /*!< GPIO Port to be chosen for interrupt generation                       */
      uint32_t RES : 18;
    } GPIO_INTR_CTRL_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t GPIO_INTR_STATUS; /*!< GPIO Interrupt Status Register                                        */

    struct {
      __IO uint32_t INTERRUPT_STATUS : 1; /*!< Gets set when interrupt is enabled and occurs.                        */
      __IO uint32_t RISE_EDGE_STATUS : 1; /*!< Gets set when rise edge is enabled and occurs.                        */
      __IO uint32_t FALL_EDGE_STATUS : 1; /*!< Gets set when Fall edge is enabled and occurs.                        */
      __O uint32_t MASK_SET : 1;          /*!< Mask set                                                              */
      __O uint32_t MASK_CLEAR : 1;        /*!< Mask Clear                                                            */
      uint32_t RES : 27;
    } GPIO_INTR_STATUS_b; /*!< BitSize                                                               */
  };
} EGPIO0_INTR_Type;

typedef struct {
  union {
    __IO uint32_t GPIO_GRP_INTR_CTRL_REG; /*!< GPIO Interrupt 0 Control Register                                     */

    struct {
      __IO uint32_t AND_OR : 1;           /*!< AND/OR                                                                */
      __IO uint32_t LEVEL_EDGE : 1;       /*!< Level/Edge                                                            */
      __IO uint32_t ENABLE_WAKEUP : 1;    /*!< For wakeup generation, actual pin status has to be seen(before
                                                         double ranking point)                                                 */
      __IO uint32_t ENABLE_INTERRUPT : 1; /*!< Enable Interrupt                                                      */
      __IO uint32_t MASK : 1;             /*!< Mask    			*/
      uint32_t RES : 27;

    } GPIO_GRP_INTR_CTRL_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      GPIO_GRP_INTR_STATUS_REG; /*!< GPIO Interrupt 0 Status Register                                      */

    struct {
      __IO uint32_t INTERRUPT_STATUS : 1; /*!< Interrupt status is available in this bit when interrupt is
                                                         enabled and generated. When 1 is written, interrupt gets cleared.     */
      __IO uint32_t WAKEUP : 1;           /*!< Double ranked version of wakeup. Gets set when wakeup is enabled
                                                         and occurs. When 1 is written it gets cleared                         */
      uint32_t : 1;
      __IO uint32_t MASK_SET : 1;   /*!< Gives zero on read                                                    */
      __IO uint32_t MASK_CLEAR : 1; /*!< Gives zero on read                                                    */
      uint32_t RES : 27;
    } GPIO_GRP_INTR_STATUS_REG_b; /*!< BitSize                                                               */
  };
} EGPIO0_GPIO_GRP_INTR_Type;

/* ================================================================================ */
/* ================                      I2C0                      ================ */
/* ================================================================================ */

/**
 * @brief Inter Integrated Circuit(I2C) is programmable control bus that provides support for the communications link between integrated circuits in a system  (I2C0)
 */

typedef struct { /*!< I2C0 Structure                                                        */

  union {
    __IO uint16_t IC_CON; /*!< I2C Control Register                                                  */

    struct {
      __IO uint16_t MASTER_MODE : 1; /*!< This bit controls whether the DW_apb_i2c master is enabled            */
      __IO uint16_t SPEED : 2;       /*!< These bits control at which speed the DW_apb_i2c operates             */
      __IO uint16_t IC_10BITADDR_SLAVE : 1; /*!< This bit controls whether the DW_apb_i2c responds to 7-bit or
                                                         10-bit addresses.                                                     */
      __I uint16_t IC_10BITADDR_MASTER : 1; /*!< This bit controls whether the DW_apb_i2c responds to 7- or 10-bit
                                                         addresses                                                             */
      __IO uint16_t IC_RESTART_EN : 1;      /*!< Determines whether RESTART conditions may be sent when acting
                                                         as a master                                                           */
      __IO uint16_t IC_SLAVE_DISABLE : 1; /*!< This bit controls whether I2C has its slave disabled                  */
      __IO uint16_t
        STOP_DET_IFADDRESSED : 1;      /*!< none                                                                  */
      __IO uint16_t TX_EMPTY_CTRL : 1; /*!< This bit controls the generation of the TX_EMPTY interrupt            */
      __I uint16_t
        RX_FIFO_FULL_HLD_CTRL : 1; /*!< none                                                                  */
    } IC_CON_b;                    /*!< BitSize                                                               */
  };
  __I uint16_t RESERVED;

  union {
    __IO uint16_t IC_TAR; /*!< I2C Target Address Register                                           */

    struct {
      __IO uint16_t IC_TAR : 10;     /*!< This is the target address for any master transaction                 */
      __IO uint16_t GC_OR_START : 1; /*!< If bit 11 (SPECIAL) is set to 1, then this bit indicates whether
                                                         a General Call or START byte command is to be performed by the
                                                          DW_apb_i2c                                                           */
      __IO uint16_t SPECIAL : 1;     /*!< This bit indicates whether software performs a General Call
                                                         or START BYTE command                                                 */
      __IO uint16_t IC_10BITADDR_MASTER : 1; /*!< This bit controls whether the DW_apb_i2c starts its transfers
                                                         in 7-or 10-bit addressing mode when acting as a master                */
      __IO uint16_t DEVICE_ID : 1;           /*!< This bit indicates whether a Device-ID of a particular slave
                                                         mentioned in IC_TAR[6:0] is to be performed by the DW_apb_i2c
                                                          Master                                                               */
    } IC_TAR_b; /*!< BitSize                                                               */
  };
  __I uint16_t RESERVED1;

  union {
    __IO uint16_t IC_SAR; /*!< I2C Slave Address Register                                            */

    struct {
      __IO uint16_t IC_SAR : 10; /*!< The IC_SAR holds the slave address when the I2C is operating
                                                         as a slave. For 7-bitaddressing, only IC_SAR[6:0] is used.            */
    } IC_SAR_b;                  /*!< BitSize                                                               */
  };
  __I uint16_t RESERVED2;

  union {
    __IO uint32_t IC_HS_MADDR; /*!< I2C High Speed Master Mode Code Address Register                      */

    struct {
      __IO uint32_t IC_HS_MAR : 3; /*!< This bit field holds the value of the I2C HS mode master code         */
    } IC_HS_MADDR_b;               /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t IC_DATA_CMD; /*!< I2C Rx/Tx Data Buffer and Command Register                            */

    struct {
      __IO uint32_t DAT : 8;            /*!< This register contains the data to be transmitted or received
                                                         on the I2C bus                                                        */
      __O uint32_t CMD : 1;             /*!< This bit controls whether a read or a write is performed              */
      __O uint32_t STOP : 1;            /*!< This bit controls whether a STOP is issued after the byte is
                                                         sent or received                                                      */
      __O uint32_t RESTART : 1;         /*!< This bit controls whether a RESTART is issued before the byte
                                                         is sent or received                                                   */
      __I uint32_t FIRST_DATA_BYTE : 1; /*!< Indicates the first data byte received after the address phase
                                                         for receive transfer in Master receiver or Slave receiver mode        */
    } IC_DATA_CMD_b;                    /*!< BitSize                                                               */
  };

  union {
    union {
      __IO uint32_t IC_UFM_SCL_HCNT; /*!< Ultra-Fast mode I2C Clock High Count Register                         */

      struct {
        __IO uint32_t IC_UFM_SCL_HCNT : 16; /*!< This register must be set before any I2C bus transaction can
                                                         take place to ensure proper I/O timing. This register sets the
                                                          SCL clock high-period count for Ultra-Fast speed                     */
      } IC_UFM_SCL_HCNT_b; /*!< BitSize                                                               */
    };

    union {
      __IO uint32_t IC_SS_SCL_HCNT; /*!< Standard Speed I2C Clock SCL High Count Register                      */

      struct {
        __IO uint32_t IC_SS_SCL_HCNT : 16; /*!< This register must be set before any I2C bus transaction can
                                                         take place to ensure proper I/O timing                                */
      } IC_SS_SCL_HCNT_b;                  /*!< BitSize                                                               */
    };
  };

  union {
    union {
      __IO uint32_t IC_UFM_SCL_LCNT; /*!< Ultra Fast mode I2C Clock High Count Register                         */

      struct {
        __IO uint32_t IC_UFM_SCL_LCNT : 16; /*!< This register must be set before any I2C bus transaction can
                                                         take place to ensure proper I/O timing. This register sets the
                                                          SCL clock low-period count for Ultra-Fast speed                      */
      } IC_UFM_SCL_LCNT_b; /*!< BitSize                                                               */
    };

    union {
      __IO uint32_t IC_SS_SCL_LCNT; /*!< Standard Speed I2C Clock SCL Low Count Register                       */

      struct {
        __IO uint32_t IC_SS_SCL_LCNT : 16; /*!< This register must be set before any I2C bus transaction can
                                                         take place to ensure proper I/O timing                                */
      } IC_SS_SCL_LCNT_b;                  /*!< BitSize                                                               */
    };
  };

  union {
    union {
      __IO uint32_t IC_UFM_TBUF_CNT; /*!< Ultra Fast mode TBuf Idle Count Register                              */

      struct {
        __IO uint32_t IC_UFM_SPKLEN : 16; /*!< none                                                                  */
      } IC_UFM_TBUF_CNT_b;                /*!< BitSize                                                               */
    };

    union {
      __IO uint16_t IC_FS_SCL_HCNT; /*!< Fast Speed I2C Clock SCL High Count Register                          */

      struct {
        __IO uint16_t IC_FS_SCL_HCNT : 16; /*!< This register must be set before any I2C bus transaction can
                                                         take place to ensure proper I/O timing                                */
      } IC_FS_SCL_HCNT_b;                  /*!< BitSize                                                               */
    };
  };

  union {
    __IO uint16_t IC_FS_SCL_LCNT; /*!< Fast Speed I2C Clock SCL Low Count Register                           */

    struct {
      __IO uint16_t IC_FS_SCL_LCNT : 16; /*!< This register must be set before any I2C bus transaction can
                                                         take place to ensure proper I/O timing                                */
    } IC_FS_SCL_LCNT_b;                  /*!< BitSize                                                               */
  };
  __I uint16_t RESERVED3;

  union {
    __IO uint16_t IC_HS_SCL_HCNT; /*!< High Speed I2C Clock SCL High Count Register                          */

    struct {
      __IO uint16_t IC_HS_SCL_HCNT : 16; /*!< This register must be set before any I2C bus transaction can
                                                         take place to ensure proper I/O timing                                */
    } IC_HS_SCL_HCNT_b;                  /*!< BitSize                                                               */
  };
  __I uint16_t RESERVED4;

  union {
    __IO uint16_t IC_HS_SCL_LCNT; /*!< High Speed I2C Clock SCL Low Count Register                           */

    struct {
      __IO uint16_t IC_HS_SCL_LCNT : 16; /*!< This register must be set before any I2C bus transaction can
                                                         take place to ensure proper I/O timing                                */
    } IC_HS_SCL_LCNT_b;                  /*!< BitSize                                                               */
  };
  __I uint16_t RESERVED5;

  union {
    __I uint16_t IC_INTR_STAT; /*!< I2C Interrupt Status Register                                         */

    struct {
      __I uint16_t R_RX_UNDER : 1;    /*!< Set if the processor attempts to read the receive buffer when
                                                         it is empty by reading from the IC_DATA_CMD register                  */
      __I uint16_t R_RX_OVER : 1;     /*!< Set if the receive buffer is completely filled to IC_RX_BUFFER_DEPTH
                                                         and an additional byte is received from an external I2C device        */
      __IO uint16_t R_RX_FULL : 1;    /*!< Set when the receive buffer reaches or goes above the RX_TL
                                                         threshold in the IC_RX_TL register.                                   */
      __I uint16_t R_TX_OVER : 1;     /*!< Set during transmit if the transmit buffer is filled to IC_TX_BUFFER_DEPTH
                                                         and the processor attempts to issue another I2C command by writing
                                                          to the IC_DATA_CMD register.                                         */
      __I uint16_t R_TX_EMPTY : 1;    /*!< This bit is set to 1 when the transmit buffer is at or below
                                                         the threshold value set in the IC_TX_TL register.                     */
      __I uint16_t R_RD_REQ : 1;      /*!< This bit is set to 1 when DW_apb_i2c is acting as a slave and
                                                         another I2C master is attempting to read data from DW_apb_i2c.        */
      __I uint16_t R_TX_ABRT : 1;     /*!< This bit indicates if DW_apb_i2c, as an I2C transmitter, is
                                                         unable to complete the intended actions on the contents of the
                                                          transmit FIFO                                                        */
      __I uint16_t R_RX_DONE : 1;     /*!< When the DW_apb_i2c is acting as a slave-transmitter, this bit
                                                         is set to 1 if the master does not acknowledge a transmitted
                                                          byte                                                                 */
      __I uint16_t R_ACTIVITY : 1;    /*!< This bit captures DW_apb_i2c activity and stays set until it
                                                         is cleared                                                            */
      __I uint16_t R_STOP_DET : 1;    /*!< Indicates whether a STOP condition has occurred on the I2C interface
                                                         regardless of whether DW_apb_i2c is operating in slave or master
                                                          mode.                                                                */
      __I uint16_t R_START_DET : 1;   /*!< Indicates whether a START or RESTART condition has occurred
                                                         on the I2C interface regardless of whether DW_apb_i2c is operating
                                                          in slave or master mode.                                             */
      __I uint16_t R_GEN_CALL : 1;    /*!< Set only when a General Call address is received and it is acknowledged */
      __I uint16_t R_RESTART_DET : 1; /*!< Indicates whether a RESTART condition has occurred on the I2C
                                                         interface when DW_apb_i2c is operating in slave mode and the
                                                          slave is the addressed slave                                         */
      __I uint16_t R_MST_ON_HOLD : 1; /*!< Indicates whether a master is holding the bus and the Tx FIFO
                                                         is empty.                                                             */
      __I uint16_t
        R_SCL_STUCK_AT_LOW : 1; /*!< Indicates whether the SCL Line is stuck at low for the IC_SCL_STUCK_LOW_TI
                                                         MOUT number of ic_clk periods                                         */
    } IC_INTR_STAT_b;           /*!< BitSize                                                               */
  };
  __I uint16_t RESERVED6;

  union {
    __IO uint32_t IC_INTR_MASK; /*!< I2C Interrupt Mask Register                                           */

    struct {
      __IO uint32_t M_RX_UNDER : 1;   /*!< This bit mask their corresponding interrupt status bits in the
                                                         IC_INTR_STAT register.                                                */
      __IO uint32_t M_RX_OVER : 1;    /*!< This bit mask their corresponding interrupt status bits in the
                                                         IC_INTR_STAT register.                                                */
      __IO uint32_t M_RX_FULL : 1;    /*!< This bit mask their corresponding interrupt status bits in the
                                                         IC_INTR_STAT register.                                                */
      __IO uint32_t M_TX_OVER : 1;    /*!< This bit mask their corresponding interrupt status bits in the
                                                         IC_INTR_STAT register                                                 */
      __IO uint32_t M_TX_EMPTY : 1;   /*!< This bit mask their corresponding interrupt status bits in the
                                                         IC_INTR_STAT register.                                                */
      __IO uint32_t M_RD_REQ : 1;     /*!< This bit mask their corresponding interrupt status bits in the
                                                         IC_INTR_STAT register.                                                */
      __IO uint32_t M_TX_ABRT : 1;    /*!< This bit mask their corresponding interrupt status bits in the
                                                         IC_INTR_STAT register.                                                */
      __IO uint32_t M_RX_DONE : 1;    /*!< This bit mask their corresponding interrupt status bits in the
                                                         IC_INTR_STAT register.                                                */
      __IO uint32_t M_ACTIVITY : 1;   /*!< This bit mask their corresponding interrupt status bits in the
                                                         IC_INTR_STAT register.                                                */
      __IO uint32_t M_STOP_DET : 1;   /*!< This bit mask their corresponding interrupt status bits in the
                                                         IC_INTR_STAT register.                                                */
      __IO uint32_t M_START_DET : 1;  /*!< This bit mask their corresponding interrupt status bits in the
                                                         IC_INTR_STAT register.                                                */
      __IO uint32_t M_GEN_CALL : 1;   /*!< This bit mask their corresponding interrupt status bits in the
                                                         IC_INTR_STAT register.                                                */
      __I uint32_t M_RESTART_DET : 1; /*!< Indicates whether a RESTART condition has occurred on the I2C
                                                         interface when DW_apb_i2c is operating in slave mode and the
                                                          slave is the addressed slave                                         */
      __I uint32_t M_MST_ON_HOLD : 1; /*!< Indicates whether a master is holding the bus and the Tx FIFO
                                                         is empty.                                                             */
      __I uint32_t
        M_SCL_STUCK_AT_LOW : 1; /*!< Indicates whether the SCL Line is stuck at low for the IC_SCL_STUCK_LOW_TI
                                                         MOUT number of ic_clk periods                                         */
    } IC_INTR_MASK_b;           /*!< BitSize                                                               */
  };

  union {
    __I uint32_t IC_RAW_INTR_STAT; /*!< I2C Raw Interrupt Status Register                                     */

    struct {
      __I uint32_t RX_UNDER : 1;    /*!< Set if the processor attempts to read the receive buffer when
                                                         it is empty by reading from the IC_DATA_CMD register                  */
      __I uint32_t RX_OVER : 1;     /*!< Set if the receive buffer is completely filled to IC_RX_BUFFER_DEPTH
                                                         and an additional byte is received from an external I2C device        */
      __IO uint32_t RX_FULL : 1;    /*!< Set when the receive buffer reaches or goes above the RX_TL
                                                         threshold in the IC_RX_TL register.                                   */
      __I uint32_t TX_OVER : 1;     /*!< Set during transmit if the transmit buffer is filled to IC_TX_BUFFER_DEPTH
                                                         and the processor attempts to issue another I2C command by writing
                                                          to the IC_DATA_CMD register.                                         */
      __I uint32_t TX_EMPTY : 1;    /*!< This bit is set to 1 when the transmit buffer is at or below
                                                         the threshold value set in the IC_TX_TL register.                     */
      __I uint32_t RD_REQ : 1;      /*!< This bit is set to 1 when DW_apb_i2c is acting as a slave and
                                                         another I2C master is attempting to read data from DW_apb_i2c.        */
      __I uint32_t TX_ABRT : 1;     /*!< This bit indicates if DW_apb_i2c, as an I2C transmitter, is
                                                         unable to complete the intended actions on the contents of the
                                                          transmit FIFO                                                        */
      __I uint32_t RX_DONE : 1;     /*!< When the DW_apb_i2c is acting as a slave-transmitter, this bit
                                                         is set to 1 if the master does not acknowledge a transmitted
                                                          byte                                                                 */
      __I uint32_t ACTIVITY : 1;    /*!< This bit captures DW_apb_i2c activity and stays set until it
                                                         is cleared                                                            */
      __I uint32_t STOP_DET : 1;    /*!< Indicates whether a STOP condition has occurred on the I2C interface
                                                         regardless of whether DW_apb_i2c is operating in slave or master
                                                          mode.                                                                */
      __I uint32_t START_DET : 1;   /*!< Indicates whether a START or RESTART condition has occurred
                                                         on the I2C interface regardless of whether DW_apb_i2c is operating
                                                          in slave or master mode.                                             */
      __I uint32_t GEN_CALL : 1;    /*!< Set only when a General Call address is received and it is acknowledged */
      __I uint32_t RESTART_DET : 1; /*!< Indicates whether a RESTART condition has occurred on the I2C
                                                         interface when DW_apb_i2c is operating in slave mode and the
                                                          slave is the addressed slave                                         */
      __I uint32_t MST_ON_HOLD : 1; /*!< Indicates whether a master is holding the bus and the Tx FIFO
                                                         is empty.                                                             */
      __I uint32_t
        SCL_STUCK_AT_LOW : 1; /*!< Indicates whether the SCL Line is stuck at low for the IC_SCL_STUCK_LOW_TI
                                                         MOUT number of ic_clk periods                                         */
    } IC_RAW_INTR_STAT_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t IC_RX_TL; /*!< I2C Receive FIFO Threshold Register                                   */

    struct {
      __IO uint32_t RX_TL : 8; /*!< Receive FIFO Threshold Level                                          */
    } IC_RX_TL_b;              /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t IC_TX_TL; /*!< I2C Transmit FIFO Threshold Register                                  */

    struct {
      __IO uint32_t TX_TL : 8; /*!< Transmit FIFO Threshold Level                                         */
    } IC_TX_TL_b;              /*!< BitSize                                                               */
  };

  union {
    __I uint32_t IC_CLR_INTR; /*!< Clear Combined and Individual Interrupt Register                      */

    struct {
      __I uint32_t CLR_INTR : 1; /*!< Read this register to clear the combined interrupt, all individual
                                                         interrupts, and the IC_TXABRT_SOURCE register                         */
    } IC_CLR_INTR_b;             /*!< BitSize                                                               */
  };

  union {
    __I uint32_t IC_CLR_RX_UNDER; /*!< Clear RX_UNDER Interrupt Register                                     */

    struct {
      __I uint32_t CLR_RX_UNDER : 1; /*!< Read this register to clear the RX_UNDER interrupt (bit 0) of
                                                         the IC_RAW_INTR_STAT register.                                        */
    } IC_CLR_RX_UNDER_b;             /*!< BitSize                                                               */
  };

  union {
    __I uint32_t IC_CLR_RX_OVER; /*!< Clear RX_OVER Interrupt Register                                      */

    struct {
      __I uint32_t CLR_RX_OVER : 1; /*!< Read this register to clear the RX_OVER interrupt (bit 1) of
                                                         the IC_RAW_INTR_STAT register                                         */
    } IC_CLR_RX_OVER_b;             /*!< BitSize                                                               */
  };

  union {
    __I uint32_t IC_CLR_TX_OVER; /*!< Clear TX_OVER Interrupt Register                                      */

    struct {
      __I uint32_t CLR_TX_OVER : 1; /*!< Read this register to clear the TX_OVER interrupt (bit 3) of
                                                         the IC_RAW_INTR_STAT register.                                        */
    } IC_CLR_TX_OVER_b;             /*!< BitSize                                                               */
  };

  union {
    __I uint32_t IC_CLR_RD_REQ; /*!< Clear RD_REQ Interrupt Register                                       */

    struct {
      __I uint32_t CLR_RD_REQ : 1; /*!< Read this register to clear the RD_REQ interrupt (bit 5) of
                                                         the IC_RAW_INTR_STAT register.                                        */
    } IC_CLR_RD_REQ_b;             /*!< BitSize                                                               */
  };

  union {
    __I uint32_t IC_CLR_TX_ABRT; /*!< Clear TX_ABRT Interrupt Register                                      */

    struct {
      __I uint32_t CLR_TX_ABRT : 1; /*!< Read this register to clear the TX_ABRT interrupt (bit 6) of
                                                         the C_RAW_INTR_STAT register, and the IC_TX_ABRT_SOURCE register      */
    } IC_CLR_TX_ABRT_b;             /*!< BitSize                                                               */
  };

  union {
    __I uint32_t IC_CLR_RX_DONE; /*!< Clear RX_DONE Interrupt Register                                      */

    struct {
      __I uint32_t CLR_RX_DONE : 1; /*!< Read this register to clear the RX_DONE interrupt (bit 7) of
                                                         the IC_RAW_INTR_STAT register                                         */
    } IC_CLR_RX_DONE_b;             /*!< BitSize                                                               */
  };

  union {
    __I uint32_t IC_CLR_ACTIVITY; /*!< Clear ACTIVITY Interrupt Register                                     */

    struct {
      __I uint32_t CLR_ACTIVITY : 1; /*!< Reading this register clears the ACTIVITY interrupt if the I2C
                                                         is not active any more                                                */
    } IC_CLR_ACTIVITY_b;             /*!< BitSize                                                               */
  };

  union {
    __I uint32_t IC_CLR_STOP_DET; /*!< Clear STOP_DET Interrupt Register                                     */

    struct {
      __I uint32_t CLR_STOP_DET : 1; /*!< Read this register to clear the STOP_DET interrupt (bit 9) of
                                                         the IC_RAW_INTR_STAT register.                                        */
    } IC_CLR_STOP_DET_b;             /*!< BitSize                                                               */
  };

  union {
    __I uint32_t IC_CLR_START_DET; /*!< Clear START_DET Interrupt Register                                    */

    struct {
      __I uint32_t CLR_START_DET : 1; /*!< Read this register to clear the START_DET interrupt (bit 10)
                                                         of the IC_RAW_INTR_STAT register                                      */
    } IC_CLR_START_DET_b;             /*!< BitSize                                                               */
  };

  union {
    __I uint32_t IC_CLR_GEN_CALL; /*!< Clear GEN_CALL Interrupt Register                                     */

    struct {
      __I uint32_t CLR_GEN_CALL : 1; /*!< Read this register to clear the GEN_CALL interrupt (bit 11)
                                                         of IC_RAW_INTR_STAT register                                          */
    } IC_CLR_GEN_CALL_b;             /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t IC_ENABLE; /*!< Clear GEN_CALL Interrupt Register                                     */

    struct {
      __IO uint32_t EN : 1;           /*!< Controls whether the DW_apb_i2c is enabled                            */
      __IO uint32_t ABORT : 1;        /*!< When set, the controller initiates the transfer abort                 */
      __IO uint32_t TX_CMD_BLOCK : 1; /*!< none                                                                  */
    } IC_ENABLE_b;                    /*!< BitSize                                                               */
  };

  union {
    __I uint32_t IC_STATUS; /*!< I2C Status Register                                                   */

    struct {
      __I uint32_t ACTIVITY : 1;     /*!< I2C Activity Status                                                   */
      __I uint32_t TFNF : 1;         /*!< Transmit FIFO Not Full                                                */
      __I uint32_t TFE : 1;          /*!< Transmit FIFO Completely Empty                                        */
      __I uint32_t RFNE : 1;         /*!< Receive FIFO Not Empty                                                */
      __I uint32_t RFF : 1;          /*!< Receive FIFO Completely Full                                          */
      __I uint32_t MST_ACTIVITY : 1; /*!< Master FSM Activity Status                                            */
      __I uint32_t SLV_ACTIVITY : 1; /*!< Slave FSM Activity Status                                             */
    } IC_STATUS_b;                   /*!< BitSize                                                               */
  };

  union {
    __I uint32_t IC_TXFLR; /*!< I2C Transmit FIFO Level Register                                      */

    struct {
      __I uint32_t TXFLR : 4; /*!< Contains the number of valid data entries in the transmit FIFO.       */
    } IC_TXFLR_b;             /*!< BitSize                                                               */
  };

  union {
    __I uint32_t IC_RXFLR; /*!< I2C Receive FIFO Level Register                                       */

    struct {
      __I uint32_t RXFLR : 4; /*!< Receive FIFO Level. Contains the number of valid data entries
                                                         in the receive FIFO                                                   */
    } IC_RXFLR_b;             /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t IC_SDA_HOLD; /*!< I2C SDA Hold Time Length Register                                     */

    struct {
      __IO uint32_t IC_SDA_HOLD : 16; /*!< Sets the required SDA hold time in units of ic_clk period             */
    } IC_SDA_HOLD_b;                  /*!< BitSize                                                               */
  };

  union {
    __I uint32_t IC_TX_ABRT_SOURCE; /*!< I2C Transmit Abort Source Register                                    */

    struct {
      __I uint32_t ABRT_7B_ADDR_NOACK : 1; /*!< 1: Master is in 7-bit addressing mode and the address sent was
                                                         not acknowledged by any slave                                         */
      __I uint32_t ABRT_10ADDR1_NOACK : 1; /*!< 1: Master is in 10-bit address mode and the first 10-bit address
                                                         byte was not acknowledged by any slave                                */
      __I uint32_t ABRT_10ADDR2_NOACK : 1; /*!< 1: Master is in 10-bit address mode and the second address byte
                                                         of the 10-bit address was not acknowledged by any slave               */
      __I uint32_t
        ABRT_TXDATA_NOACK : 1;           /*!< 1: This is a master-mode only bit. Master has received an acknowledgement
                                                         for the address, but when it sent data byte(s) following the
                                                          address, it did not receive an acknowledge from the remote slave(s)
			 */
      __I uint32_t ABRT_GCALL_NOACK : 1; /*!< 1: DW_apb_i2c in master mode sent a General Call and no slave
                                                         on the bus acknowledged the General Call                              */
      __I uint32_t ABRT_GCALL_READ : 1;  /*!< 1: DW_apb_i2c in master mode sent a General Call but the user
                                                         programmed the byte following the General Call to be a read
                                                          from the bus (IC_DATA_CMD[9] is set to 1)                            */
      __I uint32_t ABRT_HS_ACKDET : 1;   /*!< 1: Master is in High Speed mode and the High Speed Master code
                                                         was acknowledged                                                      */
      __I uint32_t ABRT_SBYTE_ACKDET : 1;    /*!< 1: Master has sent a START Byte and the START Byte was acknowledged
                                                         (wrong behavior)                                                      */
      __I uint32_t ABRT_HS_NORSTRT : 1;      /*!< 1: The restart is disabled (IC_RESTART_EN bit (IC_CON[5]) =
                                                         0) and the user is trying to use the master to transfer data
                                                          in High Speed mode                                                   */
      __I uint32_t ABRT_SBYTE_NORSTRT : 1;   /*!< 1: The restart is disabled (IC_RESTART_EN bit (IC_CON[5]) =
                                                         0) and the user is trying to send a START Byte                        */
      __I uint32_t ABRT_10B_RD_NORSTRT : 1;  /*!< 1: The restart is disabled (IC_RESTART_EN bit (IC_CON[5]) =
                                                         0) and the master sends a read command in 10-bit addressing
                                                          mode                                                                 */
      __I uint32_t ABRT_MASTER_DIS : 1;      /*!< 1: User tries to initiate a Master operation with the Master
                                                         mode disabled                                                         */
      __I uint32_t ARB_LOST : 1;             /*!< 1: Master has lost arbitration, or if IC_TX_ABRT_SOURCE[14]
                                                         is also set, then the slave transmitter has lost arbitration          */
      __I uint32_t ABRT_SLVFLUSH_TXFIFO : 1; /*!< 1: Slave has received a read command and some data exists in
                                                         the TX FIFO so the slave issues a TX_ABRT interrupt to flush
                                                          old data in TX FIFO                                                  */
      __I uint32_t ABRT_SLV_ARBLOST : 1;     /*!< 1: Slave lost the bus while transmitting data to a remote master.
                                                         IC_TX_ABRT_SOURCE[12] is set at the same time                         */
      __I uint32_t ABRT_SLVRD_INTX : 1;      /*!< 1: When the processor side responds to a slave mode request
                                                         for data to be transmitted to a remote master and user writes
                                                          a 1 in CMD (bit 8) of IC_DATA_CMD register                           */
      __I uint32_t ABRT_USER_ABRT : 1;       /*!< This is a master-mode-only bit. Master has detected the transfer
                                                         abort (IC_ENABLE[1]).                                                 */
      __I uint32_t
        ABRT_SDA_STUCK_AT_LOW : 1; /*!< Master detects the SDA is Stuck at low for the IC_SDA_STUCK_AT_LOW_TIMEOUT
                                                         value of ic_clks                                                      */
      __I uint32_t ABRT_DEVICE_NOACK : 1; /*!< Master initiates the DEVICE_ID transfer and the device ID sent
                                                         is not acknowledged by any slave                                      */
      __I uint32_t
        ABRT_DEVICE_SLVADDR_NOACK : 1;    /*!< Master is initiating the DEVICE_ID transfer and the slave address
                                                         sent was not acknowledged by any slave                                */
      __I uint32_t ABRT_DEVICE_WRITE : 1; /*!< Master is initiating the DEVICE_ID transfer and the Tx- FIFO
                                                         consists of write commands.                                           */
      uint32_t : 2;
      __I uint32_t TX_FLUSH_CNT : 9; /*!< This field indicates the number of Tx FIFO data commands that
                                                         are flushed due to TX_ABRT interrupt                                  */
    } IC_TX_ABRT_SOURCE_b;           /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t IC_SLV_DATA_NACK_ONLY; /*!< Generate Slave Data NACK Register                                     */

    struct {
      __IO uint32_t ACK_NACK : 1; /*!< Generate NACK. This NACK generation only occurs when DW_apb_i2c
                                                         is a slave receiver.                                                  */
    } IC_SLV_DATA_NACK_ONLY_b;    /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t IC_DMA_CR; /*!< DMA Control Register                                                  */

    struct {
      __IO uint32_t RDMAE : 1; /*!< Receive DMA Enable                                                    */
      __IO uint32_t TDMAE : 1; /*!< Transmit DMA Enable.This bit enables/disables the transmit FIFO
                                                         DMA channel                                                           */
    } IC_DMA_CR_b;             /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t IC_DMA_TDLR; /*!< DMA Transmit Data Level Register                                      */

    struct {
      __IO uint32_t DMATDL : 3; /*!< This bit field controls the level at which a DMA request is
                                                         made by the transmit logic                                            */
    } IC_DMA_TDLR_b;            /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t IC_DMA_RDLR; /*!< I2C Receive Data Level Register                                       */

    struct {
      __IO uint32_t DMARDL : 3; /*!< This bit field controls the level at which a DMA request is
                                                         made by the receive logic                                             */
    } IC_DMA_RDLR_b;            /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t IC_SDA_SETUP; /*!< I2C SDA Setup Register                                                */

    struct {
      __IO uint32_t SDA_SETUP : 8; /*!< This register controls the amount of time delay (in terms of
                                                         number of ic_clk clock periods)                                       */
    } IC_SDA_SETUP_b;              /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t IC_ACK_GENERAL_CALL; /*!< I2C ACK General Call Register                                         */

    struct {
      __IO uint32_t ACK_GEN_CALL : 1; /*!< ACK General Call                                                      */
    } IC_ACK_GENERAL_CALL_b;          /*!< BitSize                                                               */
  };

  union {
    __I uint32_t IC_ENABLE_STATUS; /*!< I2C Enable Status Register                                            */

    struct {
      __I uint32_t IC_EN : 1;                   /*!< This bit always reflects the value driven on the output port
                                                         ic_en.                                                                */
      __I uint32_t SLV_DISABLED_WHILE_BUSY : 1; /*!< This bit indicates if a potential or active Slave operation
                                                         has been aborted due to the setting ofthe IC_ENABLE register
                                                          from 1 to 0                                                          */
      __I uint32_t SLV_RX_DATA_LOST : 1; /*!< Slave Received Data Lost                                              */
    } IC_ENABLE_STATUS_b;                /*!< BitSize                                                               */
  };

  union {
    union {
      __IO uint32_t IC_FS_SPKLEN; /*!< I2C SS and FS Spike Suppression Limit Register                        */

      struct {
        __IO uint32_t IC_FS_SPKLEN : 8; /*!< This register sets the duration, measured in ic_clk cycles,
                                                         of the longest spike in the SCL or SDA lines that are filtered
                                                          out bythe spike suppression logic                                    */
      } IC_FS_SPKLEN_b;                 /*!< BitSize                                                               */
    };

    union {
      __IO uint32_t IC_UFM_SPKLEN; /*!< I2C Ultra Fast mode Spike suppression Register                        */

      struct {
        __IO uint32_t IC_UFM_SPKLEN : 8; /*!< This register is used to store the duration, measured in ic_clk
                                                         cycles, of the longest spike that is filtered out by the spike
                                                          suppression logic when the component is operating in Ultra-Fast
                                                          mode                                                                 */
      } IC_UFM_SPKLEN_b;                 /*!< BitSize                                                               */
    };
  };

  union {
    __IO uint32_t IC_HS_SPKLEN; /*!< I2C HS Spike Suppression Limit Register                               */

    struct {
      __IO uint32_t IC_HS_SPKLEN : 8; /*!< This register sets the duration, measured in ic_clk cycles,
                                                         of the longest spike in the SCL or SDA lines that are filtered
                                                          out bythe spike suppression logic                                    */
    } IC_HS_SPKLEN_b;                 /*!< BitSize                                                               */
  };

  union {
    __I uint32_t IC_CLR_RESTART_DET; /*!< Clear RESTART_DET Interrupt Register                                  */

    struct {
      __I uint32_t CLR_RESTART_DET : 1; /*!< Read this register to clear the RESTART_DET interrupt (bit 12)
                                                         ofthe IC_RAW_INTR_STAT registe                                        */
    } IC_CLR_RESTART_DET_b;             /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      IC_SCL_STUCK_AT_LOW_TIMEOUT; /*!< I2C SCL Stuck at Low Timeout                                          */

    struct {
      __IO uint32_t
        IC_SCL_STUCK_LOW_TIMEOUT : 32; /*!< Generates the interrupt to indicate SCL stuck at low if it detects
                                                         the SCL stuck at low for the IC_SCL_STUCK_LOW_TIMEOUT in units
                                                          of ic_clk period                                                     */
    } IC_SCL_STUCK_AT_LOW_TIMEOUT_b;   /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      IC_SDA_STUCK_AT_LOW_TIMEOUT; /*!< I2C SDA Stuck at Low Timeout                                          */

    struct {
      __IO uint32_t
        IC_SDA_STUCK_LOW_TIMEOUT : 32; /*!< Initiates the recovery of SDA line , if it detects the SDA stuck
                                                         at low for the IC_SDA_STUCK_LOW_TIMEOUT in units of ic_clk period.    */
    } IC_SDA_STUCK_AT_LOW_TIMEOUT_b;   /*!< BitSize                                                               */
  };

  union {
    __I uint32_t IC_CLR_SCL_STUCK_DET; /*!< Clear SCL Stuck at Low Detect Interrupt Register                      */

    struct {
      __I uint32_t CLR_SCL_STUCK : 1; /*!< Read this register to clear the SCL_STUCK_DET interrupt               */
    } IC_CLR_SCL_STUCK_DET_b;         /*!< BitSize                                                               */
  };

  union {
    __I uint32_t IC_DEVICE_ID; /*!< I2C Device ID                                                         */

    struct {
      __I uint32_t DEVICE_ID : 24; /*!< Contains the Device-ID of the component assigned through the
                                                         configuration parameter                                               */
    } IC_DEVICE_ID_b;              /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED7[14];

  union {
    __I uint32_t IC_COMP_PARAM_1; /*!< I2C HS Spike Suppression Limit Register                               */

    struct {
      __I uint32_t APB_DATA_WIDTH : 2;     /*!< APB Data Width                                                        */
      __I uint32_t MAX_SPEED_MODE : 2;     /*!< Maximum Speed Mode                                                    */
      __I uint32_t HC_COUNT_VALUES : 1;    /*!< Hard Code the count values                                            */
      __I uint32_t INTR_IO : 1;            /*!< Single Interrupt Output port                                          */
      __I uint32_t HAS_DMA : 1;            /*!< DMA Handshake Interface signal                                        */
      __I uint32_t ADD_ENCODED_PARAMS : 1; /*!< Add Encoded Parameters                                                */
      __I uint32_t RX_BUFFER_DEPTH : 8;    /*!< Depth of receive buffer;the buffer is 8 bits wide;2 to 256            */
      __I uint32_t TX_BUFFER_DEPTH : 8;    /*!< Depth of Transmit buffer;the buffer is 8 bits wide;2 to 256           */
    } IC_COMP_PARAM_1_b;                   /*!< BitSize                                                               */
  };

  union {
    __I uint32_t IC_COMP_VERSION; /*!< I2C Component Version Register                                        */

    struct {
      __I uint32_t IC_COMP_VERSION : 32; /*!< Signifies the component version                                       */
    } IC_COMP_VERSION_b;                 /*!< BitSize                                                               */
  };

  union {
    __I uint32_t IC_COMP_TYPE; /*!< I2C Component Type Register                                           */

    struct {
      __I uint32_t IC_COMP_TYPE : 32; /*!< Design ware Component Type number = 0x44_57_01_40                     */
    } IC_COMP_TYPE_b;                 /*!< BitSize                                                               */
  };
} I2C0_Type;

typedef struct {
  union {
    __IO uint32_t PWM_DEADTIME_A; /*!< NONE                                                                  */

    struct {
      __IO uint32_t DEADTIME_A_CH : 6; /*!< Dead time A value to load into dead time counter A of channel0        */
    } PWM_DEADTIME_A_b;                /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t PWM_DEADTIME_B; /*!< NONE                                                                  */

    struct {
      __IO uint32_t DEADTIME_B_CH : 6; /*!< Dead time B value to load into deadtime counter B of channel0         */
    } PWM_DEADTIME_B_b;                /*!< BitSize                                                               */
  };
} MCPWM_DEADTIME_Type;

/* ================================================================================ */
/* ================                      MCPWM                     ================ */
/* ================================================================================ */

/**
 * @brief The Motor Control PWM (MCPWM) controller is used to generate a periodic pulse waveform, which is useful in motor control and power control applications (MCPWM)
 */

typedef struct { /*!< MCPWM Structure                                                       */

  union {
    __I uint32_t PWM_INTR_STS; /*!< PWM Interrupt Status Register                                         */

    struct {
      __I uint32_t
        RISE_PWM_TIME_PERIOD_MATCH_INTR_CH0 : 1; /*!< This time base interrupt for 0th channel without considering
                                                         postscaler                                                            */
      __I uint32_t
        PWM_TIME_PRD_MATCH_INTR_CH0 : 1; /*!< This time base interrupt for 0th channel, which considers postscaler
                                                         value                                                                 */
      __I uint32_t FLT_A_INTR : 1;       /*!< When the fault A pin is driven low, this interrupt is raised.         */
      __I uint32_t FLT_B_INTR : 1;       /*!< When the fault B pin is driven low, this interrupt is raised.         */
      __I uint32_t
        RISE_PWM_TIME_PERIOD_MATCH_INTR_CH1 : 1; /*!< This time base interrupt for 1st channel without considering
                                                         postscaler value                                                      */
      __I uint32_t
        PWM_TIME_PRD_MATCH_INTR_CH1 : 1; /*!< This time base interrupt for 1st channel, which considers postscaler
                                                         value.                                                                */
      __I uint32_t
        RISE_PWM_TIME_PERIOD_MATCH_INTR_CH2 : 1; /*!< This time base interrupt for 2nd channel without considering
                                                         postscaler value.                                                     */
      __I uint32_t
        PWM_TIME_PRD_MATCH_INTR_CH2 : 1; /*!< This time base interrupt for 2nd channel, which considers postscaler
                                                         value                                                                 */
      __I uint32_t
        RISE_PWM_TIME_PERIOD_MATCH_INTR_CH3 : 1; /*!< This time base interrupt for 3rd channel without considering
                                                         postscaler value.                                                     */
      __I uint32_t
        PWM_TIME_PRD_MATCH_INTR_CH3 : 1; /*!< This time base interrupt for 3rd channel, which considers postscaler
                                                         value.                                                                */
    } PWM_INTR_STS_b;                    /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t PWM_INTR_UNMASK; /*!< PWM Interrupt Unmask Register                                         */

    struct {
      __IO uint32_t PWM_INTR_UNMASK : 16; /*!< NONE                                                                  */
    } PWM_INTR_UNMASK_b;                  /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t PWM_INTR_MASK; /*!< PWM Interrupt mask Register                                           */

    struct {
      __IO uint32_t PWM_INTR_UNMASK : 16; /*!< NONE                                                                  */
    } PWM_INTR_MASK_b;                    /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t PWM_INTR_ACK; /*!< PWM Interrupt Acknowledgement Register                                */

    struct {
      __IO uint32_t
        RISE_PWM_TIME_PERIOD_MATCH_CH0_ACK : 1; /*!< NONE                                                          */
      __IO uint32_t
        PWM_TIME_PRD_MATCH_INTR_CH0_ACK : 1; /*!< NONE                                                             */
      __IO uint32_t FLT_A_INTR_ACK : 1; /*!< NONE                                                                  */
      __IO uint32_t FLT_B_INTR_ACK : 1; /*!< NONE                                                                  */
      __IO uint32_t
        RISE_PWM_TIME_PERIOD_MATCH_CH1_ACK : 1; /*!< NONE                                                          */
      __IO uint32_t
        PWM_TIME_PRD_MATCH_INTR_CH1_ACK : 1; /*!< NONE                                                             */
      __IO uint32_t
        RISE_PWM_TIME_PERIOD_MATCH_CH2_ACK : 1; /*!< NONE                                                          */
      __IO uint32_t
        PWM_TIME_PRD_MATCH_INTR_CH2_ACK : 1; /*!< NONE                                                             */
      __IO uint32_t
        RISE_PWM_TIME_PERIOD_MATCH_CH3_ACK : 1; /*!< NONE                                                          */
      __IO uint32_t
        PWM_TIME_PRD_MATCH_INTR_CH3_ACK : 1; /*!< NONE                                                             */
    } PWM_INTR_ACK_b; /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED0[6];

  union {
    __IO uint32_t PWM_TIME_PRD_WR_REG_CH0; /*!< NONE                                                                  */

    struct {
      __IO uint32_t WR_VALUE_CH0 : 16; /*!< Value to update the base timer period register of channel 0           */
    } PWM_TIME_PRD_WR_REG_CH0_b;       /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_TIME_PRD_CNTR_WR_REG_CH0; /*!< NONE                                                                  */

    struct {
      __IO uint32_t WR_REG_CH0 : 16;  /*!< To update the base time counter initial value for channel 0           */
    } PWM_TIME_PRD_CNTR_WR_REG_CH0_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_TIME_PRD_PARAM_REG_CH0; /*!< NONE                                                                  */

    struct {
      __IO uint32_t
        TMR_OPEARATING_MODE_CH0 : 3; /*!< Base timer operating mode for channel0                                */
      uint32_t : 1;
      __IO uint32_t
        PWM_TIME_PRD_PRE_SCALAR_VALUE_CH0 : 3; /*!< Base timer input clock prescale select value for channel0.     */
      uint32_t : 1;
      __IO uint32_t
        PWM_TIME_PRD_POST_SCALAR_VALUE_CH0 : 4; /*!< Time base output post scale bits for channel0                 */
    } PWM_TIME_PRD_PARAM_REG_CH0_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_TIME_PRD_CTRL_REG_CH0; /*!< NONE                                                                  */

    struct {
      __IO uint32_t CNTR_RST : 1;       /*!< Time period counter soft reset                                        */
      __IO uint32_t EN_FRM_REG_CH0 : 1; /*!< Base timer enable for channnel0                                       */
      __IO uint32_t PWM_SFT_RST : 1;    /*!< MC PWM soft reset                                                     */
    } PWM_TIME_PRD_CTRL_REG_CH0_b;      /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_TIME_PRD_STS_REG_CH0; /*!< NONE                                                                  */

    struct {
      __IO uint32_t DIR_STS_CH0 : 1; /*!< Time period counter direction status for channel0                     */
    } PWM_TIME_PRD_STS_REG_CH0_b;    /*!< BitSize                                                               */
  };

  union {
    __I uint32_t
      PWM_TIME_PRD_CNTR_VALUE_CH0; /*!< NONE                                                                  */

    struct {
      __IO uint32_t CNTR_VALUE_CH0 : 16; /*!< Time period counter current value for channel0                        */
    } PWM_TIME_PRD_CNTR_VALUE_CH0_b;     /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED1[4];

  union {
    __IO uint32_t PWM_DT_CTRL_SET_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t IMDT_DT_UPDATE_EN : 4; /*!< Enable to update the duty cycle immediately                           */
      __IO uint32_t DT_UPDATE_DISABLE : 4; /*!< Duty cycle register updation disable. There is a separate bit
                                                         for each channel                                                      */
    } PWM_DT_CTRL_SET_REG_b;               /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t PWM_DT_CTRL_RESET_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t IMDT_DT_UPDATE_EN : 4; /*!< Enable to update the duty cycle immediately                           */
      __IO uint32_t DT_UPDATE_DISABLE : 4; /*!< Duty cycle register updation disable. There is a separate bit
                                                         for each channel.                                                     */
    } PWM_DT_CTRL_RESET_REG_b;             /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t DUTYCYCLE[4]; /*!< NONE                                                                  */

    struct {
      __IO uint32_t
        PWM_DC_REG_WR_VALUE_CH : 16; /*!< Duty cycle value for channel0                                         */
    } DUTYCYCLE_b[4];                /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED2[4];

  union {
    __IO uint32_t
      PWM_DEADTIME_CTRL_SET_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t
        DEADTIME_SELECT_ACTIVE : 4; /*!< Dead time select bits for PWM going active                            */
      __IO uint32_t
        DEADTIME_SELECT_INACTIVE : 4; /*!< Dead time select bits for PWM going inactive                          */
      __IO uint32_t
        DEADTIME_DISABLE_FRM_REG : 4; /*!< Dead time counter soft reset for each channel.                        */
    } PWM_DEADTIME_CTRL_SET_REG_b;    /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_DEADTIME_CTRL_RESET_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t
        DEADTIME_SELECT_ACTIVE : 4; /*!< Dead time select bits for PWM going active                            */
      __IO uint32_t
        DEADTIME_SELECT_INACTIVE : 4; /*!< Dead time select bits for PWM going inactive                          */
      __IO uint32_t
        DEADTIME_DISABLE_FRM_REG : 4; /*!< Dead time counter soft reset for each channel.                        */
    } PWM_DEADTIME_CTRL_RESET_REG_b;  /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_DEADTIME_PRESCALE_SELECT_A; /*!< NONE                                                                  */

    struct {
      __IO uint32_t
        DEADTIME_PRESCALE_SELECT_A : 4; /*!< Dead time prescale selection bits for unit A.                         */
    } PWM_DEADTIME_PRESCALE_SELECT_A_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_DEADTIME_PRESCALE_SELECT_B; /*!< NONE                                                                  */

    struct {
      __IO uint32_t
        DEADTIME_PRESCALE_SELECT_B : 4; /*!< Dead time prescale selection bits for unit B                          */
    } PWM_DEADTIME_PRESCALE_SELECT_B_b; /*!< BitSize                                                               */
  };
  MCPWM_DEADTIME_Type DEADTIME[4]; /*!< DEADTIME0                                                             */
  __I uint32_t RESERVED3[8];

  union {
    __IO uint32_t
      PWM_OP_OVERRIDE_CTRL_SET_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t OP_OVERRIDE_SYNC : 1; /*!< Output override is synced with pwm time period depending on
                                                         operating mode                                                        */
    } PWM_OP_OVERRIDE_CTRL_SET_REG_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_OP_OVERRIDE_CTRL_RESET_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t OP_OVERRIDE_SYNC : 1; /*!< Output override is synced with pwm time period depending on
                                                         operating mode                                                        */
    } PWM_OP_OVERRIDE_CTRL_RESET_REG_b;   /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_OP_OVERRIDE_ENABLE_SET_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t
        PWM_OP_OVERRIDE_ENABLE_REG : 8; /*!< Pwm output over ride enable                                           */
    } PWM_OP_OVERRIDE_ENABLE_SET_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_OP_OVERRIDE_ENABLE_RESET_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t
        PWM_OP_OVERRIDE_ENABLE_REG : 8;   /*!< Pwm output over ride enable                                           */
    } PWM_OP_OVERRIDE_ENABLE_RESET_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_OP_OVERRIDE_VALUE_SET_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t OP_OVERRIDE_VALUE : 8; /*!< Pwm output over ride value.                                           */
    } PWM_OP_OVERRIDE_VALUE_SET_REG_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_OP_OVERRIDE_VALUE_RESET_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t OP_OVERRIDE_VALUE : 8; /*!< Pwm output over ride value.                                           */
    } PWM_OP_OVERRIDE_VALUE_RESET_REG_b;   /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_FLT_OVERRIDE_CTRL_SET_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t FLT_A_MODE : 1;    /*!< Fault A mode                                                          */
      __IO uint32_t FLT_B_MODE : 1;    /*!< Fault B mode                                                          */
      __IO uint32_t OP_POLARITY_H : 1; /*!< Ouput polarity for high (H3, H2, H1, H0) side signals                 */
      __IO uint32_t OP_POLARITY_L : 1; /*!< Ouput polarity for low (L3, L2, L1, L0) side signals.                 */
      __IO uint32_t FLT_A_ENABLE : 4;  /*!< Fault A enable. Separate enable bit is present for channel            */
      __IO uint32_t FLT_B_ENABLE : 4;  /*!< Fault B enable. Separate enable bit is present for channel            */
      __IO uint32_t IO_MODE : 4;       /*!< PWM I/O pair mode                                                     */
    } PWM_FLT_OVERRIDE_CTRL_SET_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_FLT_OVERRIDE_CTRL_RESET_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t FLT_A_MODE : 1;    /*!< Fault B mode                                                          */
      __IO uint32_t FLT_B_MODE : 1;    /*!< Fault B mode                                                          */
      __IO uint32_t OP_POLARITY_H : 1; /*!< Ouput polarity for high (H3, H2, H1, H0) side signals                 */
      __IO uint32_t OP_POLARITY_L : 1; /*!< Ouput polarity for low (L3, L2, L1, L0) side signals.                 */
      __IO uint32_t FLT_A_ENABLE : 4;  /*!< Fault A enable. Separate enable bit is present for channel            */
      __IO uint32_t FLT_B_ENABLE : 4;  /*!< Fault B enable. Separate enable bit is present for channel            */
      __IO uint32_t
        COMPLEMENTARY_MODE : 4;          /*!< PWM I/O pair mode                                                     */
    } PWM_FLT_OVERRIDE_CTRL_RESET_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_FLT_A_OVERRIDE_VALUE_REG; /*!< Fault input A PWM override value                                      */

    struct {
      __IO uint32_t
        PWM_FLT_A_OVERRIDE_VALUE_L0 : 1; /*!< 0 bit for L0                                                         */
      __IO uint32_t
        PWM_FLT_A_OVERRIDE_VALUE_L1 : 1; /*!< 1 bit for L1                                                         */
      __IO uint32_t
        PWM_FLT_A_OVERRIDE_VALUE_L2 : 1; /*!< 2 bit for L2                                                         */
      __IO uint32_t
        PWM_FLT_A_OVERRIDE_VALUE_L3 : 1; /*!< 3 bit for L3                                                         */
      __IO uint32_t
        PWM_FLT_A_OVERRIDE_VALUE_H0 : 1; /*!< 4 bit for H0                                                         */
      __IO uint32_t
        PWM_FLT_A_OVERRIDE_VALUE_H1 : 1; /*!< 5 bit for H1                                                         */
      __IO uint32_t
        PWM_FLT_A_OVERRIDE_VALUE_H2 : 1; /*!< 6 bit for H2                                                         */
      __IO uint32_t
        PWM_FLT_A_OVERRIDE_VALUE_REG_H3 : 1; /*!< 7 bit for H3                                                     */
    } PWM_FLT_A_OVERRIDE_VALUE_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_FLT_B_OVERRIDE_VALUE_REG; /*!< Fault input B PWM override value                                      */

    struct {
      __IO uint32_t
        PWM_FLT_B_OVERRIDE_VALUE_L0 : 1; /*!< 0 bit for L0                                                         */
      __IO uint32_t
        PWM_FLT_B_OVERRIDE_VALUE_L1 : 1; /*!< 1 bit for L1                                                         */
      __IO uint32_t
        PWM_FLT_B_OVERRIDE_VALUE_L2 : 1; /*!< 2 bit for L2                                                         */
      __IO uint32_t
        PWM_FLT_B_OVERRIDE_VALUE_L3 : 1; /*!< 3 bit for L3                                                         */
      __IO uint32_t
        PWM_FLT_B_OVERRIDE_VALUE_H0 : 1; /*!< 4 bit for H0                                                         */
      __IO uint32_t
        PWM_FLT_B_OVERRIDE_VALUE_H1 : 1; /*!< 5 bit for H1                                                         */
      __IO uint32_t
        PWM_FLT_B_OVERRIDE_VALUE_H2 : 1; /*!< 6 bit for H2                                                         */
      __IO uint32_t
        PWM_FLT_B_OVERRIDE_VALUE_H3 : 1; /*!< 7 bit for H3                                                         */
    } PWM_FLT_B_OVERRIDE_VALUE_REG_b;    /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t PWM_SVT_CTRL_SET_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t SVT_ENABLE_FRM : 1;    /*!< Special event trigger enable. This is used to enable generation
                                                         special event trigger                                                 */
      __IO uint32_t SVT_DIRECTION_FRM : 1; /*!< Special event trigger for time base direction                         */
    } PWM_SVT_CTRL_SET_REG_b;              /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t PWM_SVT_CTRL_RESET_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t SVT_ENABLE_FRM : 1;    /*!< Special event trigger enable. This is used to enable generation
                                                         special event trigger                                                 */
      __IO uint32_t SVT_DIRECTION_FRM : 1; /*!< Special event trigger for time base direction                         */
    } PWM_SVT_CTRL_RESET_REG_b;            /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t PWM_SVT_PARAM_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t SVT_POSTSCALAR_SELECT : 4; /*!< Special event trigger enable. This is used to enable generation
                                                         special event trigger                                                 */
    } PWM_SVT_PARAM_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_SVT_COMPARE_VALUE_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t PWM_SVT_COMPARE_VALUE : 16; /*!< Special event compare value. This is used to compare with pwm
                                                         time period counter to generate special event trigger                 */
    } PWM_SVT_COMPARE_VALUE_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t PWM_TIME_PRD_WR_REG_CH1; /*!< NONE                                                                  */

    struct {
      __IO uint32_t WR_VALUE_CH1 : 16; /*!< Value to update the base timer period register of channel 1           */
    } PWM_TIME_PRD_WR_REG_CH1_b;       /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_TIME_PRD_CNTR_WR_REG_CH1; /*!< NONE                                                                  */

    struct {
      __IO uint32_t WR_REG_CH1 : 16;  /*!< To update the base time counter initial value for channel 1           */
    } PWM_TIME_PRD_CNTR_WR_REG_CH1_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_TIME_PRD_PARAM_REG_CH1; /*!< NONE                                                                  */

    struct {
      __IO uint32_t
        TMR_OPEARATING_MODE_CH1 : 3; /*!< Base timer operating mode for channel1                                */
      uint32_t : 1;
      __IO uint32_t
        PRE_SCALAR_VALUE_CH1 : 3; /*!< Base timer input clock prescale select value for channel1.            */
      uint32_t : 1;
      __IO uint32_t
        POST_SCALAR_VALUE_CH1 : 4;  /*!< Time base output post scale bits for channel1                         */
    } PWM_TIME_PRD_PARAM_REG_CH1_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_TIME_PRD_CTRL_REG_CH1; /*!< NONE                                                                  */

    struct {
      __IO uint32_t CNTR_RST : 1;    /*!< Time period counter soft reset                                        */
      __IO uint32_t EN_FRM_CH1 : 1;  /*!< Base timer enable for channnel1                                       */
      __IO uint32_t PWM_SFT_RST : 1; /*!< MC PWM soft reset                                                     */
    } PWM_TIME_PRD_CTRL_REG_CH1_b;   /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_TIME_PRD_STS_REG_CH1; /*!< NONE                                                                  */

    struct {
      __IO uint32_t DIR_STS_CH1 : 1; /*!< Time period counter direction status for channel1.                    */
    } PWM_TIME_PRD_STS_REG_CH1_b;    /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_TIME_PRD_CNTR_VALUE_CH1; /*!< NONE                                                                  */

    struct {
      __IO uint32_t
        PWM_TIME_PRD_CNTR_VALUE_CH1 : 1; /*!< Time period counter current value for channel1                       */
    } PWM_TIME_PRD_CNTR_VALUE_CH1_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t PWM_TIME_PRD_WR_REG_CH2; /*!< NONE                                                                  */

    struct {
      __IO uint32_t WR_VALUE_CH2 : 1; /*!< Value to update the base timer period register of channel 2           */
    } PWM_TIME_PRD_WR_REG_CH2_b;      /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_TIME_PRD_CNTR_WR_REG_CH2; /*!< NONE                                                                  */

    struct {
      __IO uint32_t CNTR_WR_REG_CH2 : 1; /*!< To update the base time counter initial value for channel 2           */
    } PWM_TIME_PRD_CNTR_WR_REG_CH2_b;    /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_TIME_PRD_PARAM_REG_CH2; /*!< NONE                                                                  */

    struct {
      __IO uint32_t
        TMR_OPEARATING_MODE_CH2 : 3; /*!< Base timer operating mode for channel2                                */
      uint32_t : 1;
      __IO uint32_t
        PRE_SCALAR_VALUE_CH2 : 3; /*!< Base timer input clock prescale select value for channel2.            */
      uint32_t : 1;
      __IO uint32_t
        POST_SCALAR_VALUE_CH2 : 4;  /*!< Time base output post scale bits for channel2                         */
    } PWM_TIME_PRD_PARAM_REG_CH2_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_TIME_PRD_CTRL_REG_CH2; /*!< NONE                                                                  */

    struct {
      __IO uint32_t CNTR_RST : 1;    /*!< Time period counter soft reset                                        */
      __IO uint32_t EN_FRM_CH2 : 1;  /*!< Base timer enable for channnel2                                       */
      __IO uint32_t PWM_SFT_RST : 1; /*!< MC PWM soft reset                                                     */
    } PWM_TIME_PRD_CTRL_REG_CH2_b;   /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_TIME_PRD_STS_REG_CH2; /*!< NONE                                                                  */

    struct {
      __IO uint32_t DIR_STS_CH2 : 1; /*!< Time period counter direction status for channel2.                    */
    } PWM_TIME_PRD_STS_REG_CH2_b;    /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_TIME_PRD_CNTR_VALUE_CH2; /*!< NONE                                                                  */

    struct {
      __IO uint32_t CNTR_VALUE_CH2 : 1; /*!< Time period counter current value for channel3                        */
    } PWM_TIME_PRD_CNTR_VALUE_CH2_b;    /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t PWM_TIME_PRD_WR_REG_CH3; /*!< NONE                                                                  */

    struct {
      __IO uint32_t WR_VALUE_CH3 : 16; /*!< To update the base time counter initial value for channel 3           */
    } PWM_TIME_PRD_WR_REG_CH3_b;       /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_TIME_PRD_CNTR_WR_REG_CH3; /*!< NONE                                                                  */

    struct {
      __IO uint32_t WR_VALUE_CH3 : 16; /*!< Value to update the base timer period register of channel 3           */
    } PWM_TIME_PRD_CNTR_WR_REG_CH3_b;  /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_TIME_PRD_PARAM_REG_CH3; /*!< NONE                                                                  */

    struct {
      __IO uint32_t
        TMR_OPEARATING_MODE_CH3 : 3; /*!< Base timer operating mode for channel3                                */
      uint32_t : 1;
      __IO uint32_t
        PRE_SCALAR_VALUE_CH3 : 3; /*!< Base timer input clock prescale select value for channel2.            */
      uint32_t : 1;
      __IO uint32_t
        POST_SCALAR_VALUE_CH3 : 4;  /*!< Time base output postscale bits for channel3                          */
    } PWM_TIME_PRD_PARAM_REG_CH3_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_TIME_PRD_CTRL_REG_CH3; /*!< NONE                                                                  */

    struct {
      __IO uint32_t CNTR_RST : 1;    /*!< Time period counter soft reset                                        */
      __IO uint32_t EN_FRM_CH3 : 1;  /*!< Base timer enable for channnel3                                       */
      __IO uint32_t PWM_SFT_RST : 1; /*!< MC PWM soft reset                                                     */
    } PWM_TIME_PRD_CTRL_REG_CH3_b;   /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_TIME_PRD_STS_REG_CH3; /*!< NONE                                                                  */

    struct {
      __IO uint32_t DIR_STS_CH3 : 1; /*!< Time period counter direction status for channel3.                    */
    } PWM_TIME_PRD_STS_REG_CH3_b;    /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PWM_TIME_PRD_CNTR_VALUE_CH3; /*!< NONE                                                                  */

    struct {
      __IO uint32_t CNTR_VALUE_CH3 : 16; /*!< Time period counter current value for channe3                         */
    } PWM_TIME_PRD_CNTR_VALUE_CH3_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t PWM_TIME_PRD_COMMON_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t USE_0TH_TIMER_ONLY : 1; /*!< Instead of use four base timers for four channels, use only
                                                         one base timer for all channels.                                      */
      __IO uint32_t
        PWM_TIME_PRD_COMMON_TIMER_VALUE : 2; /*!< Base timers select to generate special event trigger             */
      __IO uint32_t USE_EXT_TIMER_TRIG_FRM_REG : 1; /*!< Enable to use external trigger for base time counter increment
                                                         or decrement.                                                         */
    } PWM_TIME_PRD_COMMON_REG_b; /*!< BitSize                                                               */
  };
} MCPWM_Type;

/* ================================================================================ */
/* ================                      UDMA0                     ================ */
/* ================================================================================ */

/**
 * @brief DMA Performs data transfers along with Addresses and control information (UDMA0)
 */

typedef struct { /*!< UDMA0 Structure                                                       */

  union {
    __I uint32_t DMA_STATUS; /*!< UDMA Status Register                                                  */

    struct {
      __I uint32_t MASTER_ENABLE : 1; /*!< Enable status of controller                                           */
      uint32_t : 3;
      __I uint32_t STATE : 4; /*!< Current state of the control state machine                            */
      uint32_t : 8;
      __I uint32_t CHNLS_MINUS1 : 5; /*!< Number of available DMA channels minus one                            */
      uint32_t : 7;
      __I uint32_t TEST_STATUS : 4; /*!< To reduce the gate count you can configure the controller             */
    } DMA_STATUS_b;                 /*!< BitSize                                                               */
  };

  union {
    __O uint32_t DMA_CFG; /*!< DMA Configuration                                                     */

    struct {
      __O uint32_t MASTER_EN : 1; /*!< Enable for the controller                                             */
      uint32_t : 4;
      __O uint32_t CHNL_PROT_CTRL : 3; /*!< Sets the AHB-Lite protection by controlling the HPROT[3:1]]
                                                         signal levels as follows Bit[7]-Controls HPROT[3] to indicate
                                                          if cacheable access is occurring Bit[6]-Controls HPROT[2] to
                                                          indicate if cacheable access is occurring Bit[5]-Controls HPROT[1]
                                                          to indicate if cacheable access is occurring                         */
    } DMA_CFG_b;                       /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t CTRL_BASE_PTR; /*!< Channel Control Data Base Pointer                                     */

    struct {
      uint32_t : 10;
      __IO uint32_t CTRL_BASE_PTR : 22; /*!< Pointer to the base address of the primary data structure             */
    } CTRL_BASE_PTR_b;                  /*!< BitSize                                                               */
  };

  union {
    __I uint32_t ALT_CTRL_BASE_PTR; /*!< Channel Alternate Control Data Base Pointer                           */

    struct {
      __I uint32_t ALT_CTRL_BASE_PTR : 32; /*!< Base address of the alternative data structure                        */
    } ALT_CTRL_BASE_PTR_b;                 /*!< BitSize                                                               */
  };

  union {
    __I uint32_t DMA_WAITONREQUEST_STATUS; /*!< Channel Wait on request status register                               */

    struct {
      __IO uint32_t
        DMA_WAITONREQ_STATUS : 32; /*!< Per Channel wait on request status                                    */
    } DMA_WAITONREQUEST_STATUS_b;  /*!< BitSize                                                               */
  };

  union {
    __O uint32_t CHNL_SW_REQUEST; /*!< Channel Software Request                                              */

    struct {
      __O uint32_t CHNL_SW_REQUEST : 32; /*!< Set the appropriate bit to generate a software DMA request on
                                                         the corresponding DMA channel                                         */
    } CHNL_SW_REQUEST_b;                 /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t CHNL_USEBURST_SET; /*!< UDMA Channel use burst set                                            */

    struct {
      __IO uint32_t CHNL_USEBURST_SET : 32; /*!< The use burst status, or disables dma_sreq[C] from generating
                                                         DMA requests.                                                         */
    } CHNL_USEBURST_SET_b; /*!< BitSize                                                               */
  };

  union {
    __O uint32_t CHNL_USEBURST_CLR; /*!< UDMA Channel use burst clear                                          */

    struct {
      __O uint32_t CHNL_USEBURST_CLR : 32; /*!< Set the appropriate bit to enable dma_sreq[] to generate requests     */
    } CHNL_USEBURST_CLR_b;                 /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t CHNL_REQ_MASK_SET; /*!< UDMA Channel request mask set Register                                */

    struct {
      __IO uint32_t CHNL_REQ_MASK_SET : 32; /*!< Returns the request mask status of dma_req[] and dma_sreq[],
                                                         or disables the corresponding channel from generating DMA requests    */
    } CHNL_REQ_MASK_SET_b; /*!< BitSize                                                               */
  };

  union {
    __O uint32_t CHNL_REQ_MASK_CLR; /*!< UDMA Channel request mask clear                                       */

    struct {
      __O uint32_t CHNL_REQ_MASK_CLR : 32; /*!< Set the appropriate bit to enable DMA requests for the channel
                                                         corresponding to dma_req[] and dma_sreq[]                             */
    } CHNL_REQ_MASK_CLR_b;                 /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t CHNL_ENABLE_SET; /*!< UDMA Channel enable register                                          */

    struct {
      __IO uint32_t CHNL_ENABLE_SET : 32; /*!< This Bits are Used to Load the 16bits of Source address               */
    } CHNL_ENABLE_SET_b;                  /*!< BitSize                                                               */
  };

  union {
    __O uint32_t CHNL_ENABLE_CLR; /*!< UDMA Channel enable clear register                                    */

    struct {
      __IO uint32_t CHNL_ENABLE_CLR : 32; /*!< Set the appropriate bit to disable the corresponding DMA channel      */
    } CHNL_ENABLE_CLR_b;                  /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t CHNL_PRI_ALT_SET; /*!< UDMA Channel primary or alternate set                                 */

    struct {
      __IO uint32_t CHNL_PRI_ALT_SET : 32; /*!< Returns the channel control data structure status or selects
                                                         the alternate data structure for the corresponding DMA channel
			 */
    } CHNL_PRI_ALT_SET_b;                  /*!< BitSize                                                               */
  };

  union {
    __O uint32_t CHNL_PRI_ALT_CLR; /*!< UDMA Channel primary alternate clear                                  */

    struct {
      __O uint32_t CHNL_PRI_ALT_CLR : 32; /*!< Set the appropriate bit to select the primary data structure
                                                         for the corresponding DMA channel                                     */
    } CHNL_PRI_ALT_CLR_b;                 /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t CHNL_PRIORITY_SET; /*!< UDMA Channel Priority Set                                             */

    struct {
      __IO uint32_t CHNL_PRIORITY_SET : 1; /*!< Set the appropriate bit to select the primary data structure
                                                         for the corresponding DMA channel                                     */
    } CHNL_PRIORITY_SET_b;                 /*!< BitSize                                                               */
  };

  union {
    __O uint32_t CHNL_PRIORITY_CLR; /*!< UDMA Channel Priority Clear                                           */

    struct {
      __O uint32_t CHNL_PRIORITY_CLR : 32; /*!< Set the appropriate bit to select the default priority level
                                                         for the specified DMA channel                                         */
    } CHNL_PRIORITY_CLR_b;                 /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED[3];

  union {
    __IO uint32_t ERR_CLR; /*!< UDMA Bus Error Clear Register                                         */

    struct {
      __IO uint32_t ERR_CLR : 1; /*!< Returns the status of dma_err                                         */
    } ERR_CLR_b;                 /*!< BitSize                                                               */
  };

  __IO uint32_t SKIP_DESC_FETCH_REG;
  __I uint32_t RESERVED1[491];

  union {
    __IO uint32_t UDMA_DONE_STATUS_REG; /*!< UDMA Done status Register                                             */

    struct {
      __IO uint32_t
        DONE_STATUS_CHANNEL_0 : 1; /*!< UDMA done Status of the channel 0                                     */
      __IO uint32_t
        DONE_STATUS_CHANNEL_1 : 1; /*!< UDMA done Status of the channel 1                                     */
      __IO uint32_t
        DONE_STATUS_CHANNEL_2 : 1; /*!< UDMA done Status of the channel 2                                     */
      __IO uint32_t
        DONE_STATUS_CHANNEL_3 : 1; /*!< UDMA done Status of the channel 3                                     */
      __IO uint32_t
        DONE_STATUS_CHANNEL_4 : 1; /*!< UDMA done Status of the channel 4                                     */
      __IO uint32_t
        DONE_STATUS_CHANNEL_5 : 1; /*!< UDMA done Status of the channel 5                                     */
      __IO uint32_t
        DONE_STATUS_CHANNEL_6 : 1; /*!< UDMA done Status of the channel 6                                     */
      __IO uint32_t
        DONE_STATUS_CHANNEL_7 : 1; /*!< UDMA done Status of the channel 7                                     */
      __IO uint32_t
        DONE_STATUS_CHANNEL_8 : 1; /*!< UDMA done Status of the channel 8                                     */
      __IO uint32_t
        DONE_STATUS_CHANNEL_9 : 1; /*!< UDMA done Status of the channel 9                                     */
      __IO uint32_t
        DONE_STATUS_CHANNEL_10 : 1; /*!< UDMA done Status of the channel 10                                    */
      __IO uint32_t
        DONE_STATUS_CHANNEL_11 : 1; /*!< UDMA done Status of the channel 3                                     */
      __IO uint32_t
        DONE_STATUS_CHANNEL_12 : 1; /*!< UDMA done Status of the channel 12                                    */
      __IO uint32_t
        DONE_STATUS_CHANNEL_13 : 1; /*!< UDMA done Status of the channel 13                                    */
      __IO uint32_t
        DONE_STATUS_CHANNEL_14 : 1; /*!< UDMA done Status of the channel 14                                    */
      __IO uint32_t
        DONE_STATUS_CHANNEL_15 : 1; /*!< UDMA done Status of the channel 15                                    */
      __IO uint32_t
        DONE_STATUS_CHANNEL_16 : 1; /*!< UDMA done Status of the channel 16                                    */
      __IO uint32_t
        DONE_STATUS_CHANNEL_17 : 1; /*!< UDMA done Status of the channel 17                                    */
      __IO uint32_t
        DONE_STATUS_CHANNEL_18 : 1; /*!< UDMA done Status of the channel 18                                    */
      __IO uint32_t
        DONE_STATUS_CHANNEL_19 : 1; /*!< UDMA done Status of the channel 19                                    */
      __IO uint32_t
        DONE_STATUS_CHANNEL_20 : 1; /*!< UDMA done Status of the channel 3                                     */
      __IO uint32_t
        DONE_STATUS_CHANNEL_21 : 1; /*!< UDMA done Status of the channel 21                                    */
      __IO uint32_t
        DONE_STATUS_CHANNEL_22 : 1; /*!< UDMA done Status of the channel 22                                    */
      __IO uint32_t
        DONE_STATUS_CHANNEL_23 : 1; /*!< UDMA done Status of the channel 23                                    */
      __IO uint32_t
        DONE_STATUS_CHANNEL_24 : 1; /*!< UDMA done Status of the channel 24                                    */
      __IO uint32_t
        DONE_STATUS_CHANNEL_25 : 1; /*!< UDMA done Status of the channel 25                                    */
      __IO uint32_t
        DONE_STATUS_CHANNEL_26 : 1; /*!< UDMA done Status of the channel 26                                    */
      __IO uint32_t
        DONE_STATUS_CHANNEL_27 : 1; /*!< UDMA done Status of the channel 27                                    */
      __IO uint32_t
        DONE_STATUS_CHANNEL_28 : 1; /*!< UDMA done Status of the channel 28                                    */
      __IO uint32_t
        DONE_STATUS_CHANNEL_29 : 1; /*!< UDMA done Status of the channel 29                                    */
      __IO uint32_t
        DONE_STATUS_CHANNEL_30 : 1; /*!< UDMA done Status of the channel 30                                    */
      __IO uint32_t
        DONE_STATUS_CHANNEL_31 : 1; /*!< UDMA done Status of the channel 31                                    */
    } UDMA_DONE_STATUS_REG_b;       /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t CHANNEL_STATUS_REG; /*!< Channel status Register                                               */

    struct {
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_0 : 1; /*!< Reading 1 indicates that the channel 0 is busy                    */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_1 : 1; /*!< Reading 1 indicates that the channel 1 is busy                    */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_2 : 1; /*!< Reading 1 indicates that the channel 2 is busy                    */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_3 : 1; /*!< Reading 1 indicates that the channel 3 is busy                    */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_4 : 1; /*!< Reading 1 indicates that the channel 4 is busy                    */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_5 : 1; /*!< Reading 1 indicates that the channel 5 is busy                    */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_6 : 1; /*!< Reading 1 indicates that the channel 6 is busy                    */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_7 : 1; /*!< Reading 1 indicates that the channel 7 is busy                    */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_8 : 1; /*!< Reading 1 indicates that the channel 8 is busy                    */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_9 : 1; /*!< Reading 1 indicates that the channel 9 is busy                    */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_10 : 1; /*!< Reading 1 indicates that the channel 10 is busy                  */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_11 : 1; /*!< Reading 1 indicates that the channel 11 is busy                  */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_12 : 1; /*!< Reading 1 indicates that the channel 12 is busy                  */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_13 : 1; /*!< Reading 1 indicates that the channel 13 is busy                  */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_14 : 1; /*!< Reading 1 indicates that the channel 14 is busy                  */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_15 : 1; /*!< Reading 1 indicates that the channel 15 is busy                  */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_16 : 1; /*!< Reading 1 indicates that the channel 16 is busy                  */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_17 : 1; /*!< Reading 1 indicates that the channel 17 is busy                  */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_18 : 1; /*!< Reading 1 indicates that the channel 18 is busy                  */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_19 : 1; /*!< Reading 1 indicates that the channel 19 is busy                  */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_20 : 1; /*!< Reading 1 indicates that the channel 20 is busy                  */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_21 : 1; /*!< Reading 1 indicates that the channel 21 is busy                  */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_22 : 1; /*!< Reading 1 indicates that the channel 22 is busy                  */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_23 : 1; /*!< Reading 1 indicates that the channel 23 is busy                  */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_24 : 1; /*!< Reading 1 indicates that the channel 24 is busy                  */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_25 : 1; /*!< Reading 1 indicates that the channel 25 is busy                  */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_26 : 1; /*!< Reading 1 indicates that the channel 26 is busy                  */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_27 : 1; /*!< Reading 1 indicates that the channel 27 is busy                  */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_28 : 1; /*!< Reading 1 indicates that the channel 28 is busy                  */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_29 : 1; /*!< Reading 1 indicates that the channel 29 is busy                  */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_30 : 1; /*!< Reading 1 indicates that the channel 30 is busy                  */
      __IO uint32_t
        BUSY_OR_IDEAL_STATUS_CHANNEL_31 : 1; /*!< Reading 1 indicates that the channel 31 is busy                  */
    } CHANNEL_STATUS_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PERIPHERAL_SELECT_CHANNEL0_REG; /*!< Peripheral Select Channel0 register                                   */

    struct {
      __IO uint32_t
        PERIPHERAL_SELECT_CHANNEL0 : 6; /*!< Selects which peripheral will be selected for the channel 0           */
    } PERIPHERAL_SELECT_CHANNEL0_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PERIPHERAL_SELECT_CHANNEL1; /*!< Peripheral Select Channel1 register                                   */

    struct {
      __IO uint32_t
        PERIPHERAL_SELECT_CHANNEL1 : 6; /*!< Selects which peripheral will be selected for the channel 1           */
    } PERIPHERAL_SELECT_CHANNEL1_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PERIPHERAL_SELECT_CHANNEL2; /*!< Peripheral Select Channel2 register                                   */

    struct {
      __IO uint32_t
        PERIPHERAL_SELECT_CHANNEL2 : 6; /*!< Selects which peripheral will be selected for the channel 2           */
    } PERIPHERAL_SELECT_CHANNEL2_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PERIPHERAL_SELECT_CHANNEL3; /*!< Peripheral Select Channel3 register                                   */

    struct {
      __IO uint32_t
        PERIPHERAL_SELECT_CHANNEL3 : 6; /*!< Selects which peripheral will be selected for the channel 3           */
    } PERIPHERAL_SELECT_CHANNEL3_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PERIPHERAL_SELECT_CHANNEL4; /*!< Peripheral Select Channel4 register                                   */

    struct {
      __IO uint32_t
        PERIPHERAL_SELECT_CHANNEL4 : 6; /*!< Selects which peripheral will be selected for the channel 4           */
    } PERIPHERAL_SELECT_CHANNEL4_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PERIPHERAL_SELECT_CHANNEL5; /*!< Peripheral Select Channel5 register                                   */

    struct {
      __IO uint32_t
        PERIPHERAL_SELECT_CHANNEL5 : 6; /*!< Selects which peripheral will be selected for the channel 3           */
    } PERIPHERAL_SELECT_CHANNEL5_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PERIPHERAL_SELECT_CHANNEL6; /*!< Peripheral Select Channel6 register                                   */

    struct {
      __IO uint32_t
        PERIPHERAL_SELECT_CHANNEL6 : 6; /*!< Selects which peripheral will be selected for the channel 6           */
    } PERIPHERAL_SELECT_CHANNEL6_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PERIPHERAL_SELECT_CHANNEL7; /*!< Peripheral Select Channel7 register                                   */

    struct {
      __IO uint32_t
        PERIPHERAL_SELECT_CHANNEL7 : 6; /*!< Selects which peripheral will be selected for the channel 7           */
    } PERIPHERAL_SELECT_CHANNEL7_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint16_t UDMA_CONFIG_CTRL_REG; /*!< DMA Controller Transfer Length Register                               */

    struct {
      __IO uint16_t
        SINGLE_REQUEST_ENABLE : 1; /*!< Enabled signal for single request                                     */
    } UDMA_CONFIG_CTRL_REG_b;      /*!< BitSize                                                               */
  };

  union {
    __IO uint16_t UDMA_INTR_MASK_REG; /*!< DMA Controller Interrupt mask register                               */

    struct {
      __IO uint16_t UDMA_INTR_MASK : 12; /*!< DMA Controller Interrupt mask                                     */
    } UDMA_INTR_MASK_REG_b;              /*!< BitSize                                                               */
  };

} UDMA0_Type;

/* ================================================================================ */
/* ================                     RPDMA_G                    ================ */
/* ================================================================================ */

/**
 * @brief RPDMAC (dma controller) is an AMBA complaint peripheral unit supports 8-channels (RPDMA_G)
 */

typedef struct { /*!< RPDMA_G Structure                                                     */
  __I uint32_t RESERVED[1057];
  RPDMA_G_GLOBAL_Type GLOBAL; /*!< (null)                                                                */
} RPDMA_G_Type;

/* ================================================================================ */
/* ================                     RPDMA_C                    ================ */
/* ================================================================================ */

/**
 * @brief RPDMAC (dma controller) is an AMBA complaint peripheral unit supports 8-channels (RPDMA_C)
 */

typedef struct { /*!< RPDMA_C Structure                                                     */
  RPDMA_C_CHANNEL_CONFIG_Type
    CHANNEL_CONFIG[8]; /*!< CHANNEL_CONFIG0                                                       */
} RPDMA_C_Type;

/* ================================================================================ */
/* ================                      HWRNG                     ================ */
/* ================================================================================ */

/**
 * @brief Random numbers generated are 16-bit random numbers and are generated using either the True random number generator or the Pseudo random number generator. (HWRNG)
 */

typedef struct { /*!< HWRNG Structure                                                       */

  union {
    __IO uint32_t HWRNG_CTRL_REG; /*!< Random Number Generator Control Register                              */

    struct {
      __IO uint32_t HWRNG_RNG_ST : 1;  /*!< This bit is used to start the true number generation.                 */
      __IO uint32_t HWRNG_PRBS_ST : 1; /*!< This bit is used to start the pseudo random number generation         */
    } HWRNG_CTRL_REG_b;                /*!< BitSize                                                               */
  };

  union {
    __I uint32_t HWRNG_RAND_NUM_REG; /*!< Hardware Random Number Register                                       */

    struct {
      __I uint32_t HWRNG_RAND_NUM : 32; /*!< Generated random number can be read from this register.               */
    } HWRNG_RAND_NUM_REG_b;             /*!< BitSize                                                               */
  };
} HWRNG_Type;

/* ================================================================================ */
/* ================                     TIMERS                     ================ */
/* ================================================================================ */

/**
 * @brief TIMER can be used to generate various timing events for the software (TIMERS)
 */

typedef struct {                        /*!< TIMERS Structure                                                      */
  TIMERS_MATCH_CTRL_Type MATCH_CTRL[4]; /*!< MATCH_CTRL0                                                           */
  //__I  uint32_t  RESERVED[6];
  __I uint32_t RESERVED[24];

  union {
    __IO uint16_t TMR_STAT; /*!< Timer Status Register                                                 */

    struct {
      __IO uint16_t TMR0_INTR_STATUS : 1; /*!< This bit indicates status of the interrupt generated by timer
                                                         0                                                                     */
      __IO uint16_t TMR1_INTR_STATUS : 1; /*!< This bit indicates status of the interrupt generated by timer
                                                         1                                                                     */
      __IO uint16_t TMR2_INTR_STATUS : 1; /*!< This bit indicates status of the interrupt generated by timer
                                                         2                                                                     */
      __IO uint16_t TMR3_INTR_STATUS : 1; /*!< This bit indicates status of the interrupt generated by timer
                                                         3                                                                     */
    } TMR_STAT_b;                         /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t TMR_US_PERIOD_INT; /*!< Timer micro second period Integral Part Register                      */

    struct {
      __IO uint32_t TMR_US_PERIOD_INT : 16; /*!< This bits are used to program the integer part of number of
                                                         clock cycles per microseconds of the system clock used                */
    } TMR_US_PERIOD_INT_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t TMR_US_PERIOD_FRAC; /*!< Timer microsecond period Fractional Part Register                     */

    struct {
      __IO uint32_t TMR_US_PERIOD_FRAC : 8; /*!< This bits are used to program the fractional part of number
                                                         of clock cycles per microseconds of the system clock used             */
    } TMR_US_PERIOD_FRAC_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t TMR_MS_PERIOD_INT; /*!< Timer 256 microsecond period Integral Part Register                   */

    struct {
      __IO uint32_t TMR_MS_PERIOD_INT : 16; /*!< This bits are used to program the integer part of number of
                                                         clock cycles per 256 microseconds of the system clock used            */
    } TMR_MS_PERIOD_INT_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t TMR_MS_PERIOD_FRAC; /*!< Timer 256 microsecond period Fractional Part Register                 */

    struct {
      __IO uint32_t TMR_MS_PERIOD_FRAC : 8; /*!< This bits are used to program the fractional part of number
                                                         of clock cycles per 256 microseconds of the system clock used
			 */
    } TMR_MS_PERIOD_FRAC_b; /*!< BitSize                                                               */
  };
} TIMERS_Type;

/* ================================================================================ */
/* ================                      CAN0                      ================ */
/* ================================================================================ */

/*CAN Frame */
typedef struct stc_can_std_frame {
  uint32_t DLC : 4;
  uint32_t RES_X0 : 2;
  uint32_t REMOTE_FRAME : 1;
  uint32_t FRAME_FORMAT : 1;
  uint32_t ID3_ID10 : 8;
  uint32_t RES_X1 : 4;
  uint32_t REMOTE_FRAME_X : 1;
  uint32_t ID0_ID2 : 3;
  uint32_t DATA0 : 8;
} stc_can_std_frame_t;
/**
 * @brief The DCAN is a standalone CAN (Controller Area Network) controller widely used in automotive and industrial applications. (CAN0)
 */

typedef struct { /*!< CAN0 Structure                                                        */

  union {
    __IO uint8_t MR; /*!< Mode Register                                                         */

    struct {
      __IO uint8_t AFM : 1; /*!< hardware acceptance filter scheme                                     */
      __IO uint8_t LOM : 1; /*!< Listen Only Mode                                                      */
      __IO uint8_t RM : 1;  /*!< Reset Mode       */
      __IO uint8_t RES : 5;
    } MR_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint8_t CMR; /*!< Command Register                                                      */

    struct {
      uint8_t : 1;
      __O uint8_t AT : 1; /*!< Abort Transmission                                                    */
      __O uint8_t TR : 1; /*!< Transmit Request                                                      */
      __IO uint8_t RES : 5;
    } CMR_b; /*!< BitSize                                                               */
  };

  union {
    __I uint8_t SR; /*!< Status register                                                       */

    struct {
      __I uint8_t BS : 1; /*!< Bus Off Status                                                        */
      __I uint8_t ES : 1; /*!< Error Status                                                          */
      __I uint8_t TS : 1; /*!< Transmit Status                                                       */
      __I uint8_t RS : 1; /*!< Receive Status                                                        */
      uint8_t : 1;
      __I uint8_t TBS : 1; /*!< Transmit Buffer Status                                                */
      __I uint8_t DSO : 1; /*!< Data Overrun Status                                                   */
      __I uint8_t RBS : 1; /*!< Data Overrun Status                                                   */
    } SR_b;                /*!< BitSize                                                               */
  };

  union {
    __IO uint8_t ISR_IACK; /*!< Interrupt Status/Acknowledge Register                                 */

    struct {
      __IO uint8_t DOI : 1; /*!< Data Overrun Interrupt                                                */
      __IO uint8_t BEI : 1; /*!< Bus Error Interrupt                                                   */
      __IO uint8_t TI : 1;  /*!< Transmission Interrupt                                                */
      __IO uint8_t RI : 1;  /*!< Receive Interrupt                                                     */
      __IO uint8_t EPI : 1; /*!< Error Passive Interrupt                                               */
      __IO uint8_t EWI : 1; /*!< Error Warning Interrupt                                               */
      __IO uint8_t ALI : 1; /*!< Arbitration Lost Interrupt                                            */
      __IO uint8_t RES : 1;
    } ISR_IACK_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint8_t IMR; /*!< Interrupt Mask register.Setting appropriate bit in IMR register
                                                         enables interrupt assigned to it, clearing disables this interrupt
		 */

    struct {
      __IO uint8_t DOIM : 1; /*!< mask for DOI interrupt                                                */
      __IO uint8_t BEIM : 1; /*!< mask for BEI interrupt                                                */
      __IO uint8_t TIM : 1;  /*!< mask for TI interrupt                                                 */
      __IO uint8_t RIM : 1;  /*!< mask for RI interrupt                                                 */
      __IO uint8_t EPIM : 1; /*!< mask for EPI interrupt                                                */
      __IO uint8_t EWIM : 1; /*!< mask for EWI interrupt                                                */
      __IO uint8_t ALIM : 1; /*!< mask for ALI interrupt                                                */
      __IO uint8_t RES : 1;
    } IMR_b; /*!< BitSize                                                               */
  };

  union {
    __I uint8_t RMC; /*!< Receive Message Counter                                               */

    struct {
      __I uint8_t RMC : 5; /*!< number of stored message frames                                       */
      __IO uint8_t RES : 3;
    } RMC_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint8_t BTR0; /*!< BUS TIMING REGISTER 0                                                 */

    struct {
      __IO uint8_t BRP : 6; /*!< Baud Rate Pre scaler                                                  */
      __IO uint8_t SJW : 2; /*!< Synchronization Jump Width                                            */
    } BTR0_b;               /*!< BitSize                                                               */
  };

  union {
    __IO uint8_t BTR1; /*!< BUS TIMING REGISTER 1.define the length of bit period, location
                                                         of the sample point and number of samples to be taken at each
                                                          sample point                                                         */

    struct {
      __IO uint8_t TSEG1 : 4; /*!< Number of clock cycles per Time Segment 1                             */
      __IO uint8_t TSEG2 : 3; /*!< Number of clock cycles per Time Segment 1                             */
      __IO uint8_t SAM : 1;   /*!< Number of bus level samples                                           */
    } BTR1_b;                 /*!< BitSize                                                               */
  };

  union {
    __O uint32_t TXBUF; /*!< TRANSMIT BUFFER REGISTER                                              */

    struct {
      stc_can_std_frame_t stcCanFrame;
    } TXBUF_b; /*!< BitSize                                                               */
  };

  union {
    __I uint32_t RXBUF; /*!< RECEIVE BUFFER REGISTER                                               */

    struct {
      __I uint32_t RXBUF0 : 8; /*!< Receive Buffer Register is used to read CAN frames received
                                                         by the DCAN core from CAN network                                     */
      __I uint32_t RXBUF1 : 8; /*!< Receive Buffer Register is used to read CAN frames received
                                                         by the DCAN core from CAN network                                     */
      __I uint32_t RXBUF2 : 8; /*!< Receive Buffer Register is used to read CAN frames received
                                                         by the DCAN core from CAN network                                     */
      __I uint32_t RXBUF3 : 8; /*!< Receive Buffer Register is used to read CAN frames received
                                                         by the DCAN core from CAN network                                     */
    } RXBUF_b;                 /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t ACR; /*!< ACCEPTANCE CODE REGISTER                                              */

    struct {
      __IO uint32_t ACR0 : 8; /*!< The acceptance code registers contains bit patterns of messages
                                                         to be received                                                        */
      __IO uint32_t ACR1 : 8; /*!< The acceptance code registers contains bit patterns of messages
                                                         to be received                                                        */
      __IO uint32_t ACR2 : 8; /*!< The acceptance code registers contains bit patterns of messages
                                                         to be received                                                        */
      __IO uint32_t ACR3 : 8; /*!< The acceptance code registers contains bit patterns of messages
                                                         to be received                                                        */
    } ACR_b;                  /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t AMR; /*!< ACCEPTANCE MASK REGISTER                                              */

    struct {
      __IO uint32_t AMR0 : 8; /*!< Acceptance mask registers defines which bit positions will be
                                                         compared and which ones are do not care. Setting certain AMR
                                                          bit define corresponding bit in ACR as do not care.                  */
      __IO uint32_t AMR1 : 8; /*!< acceptance mask registers defines which bit positions will be
                                                         compared and which onesare do not care. Setting certain AMR
                                                          bit define corresponding bit in ACR as do not care.                  */
      __IO uint32_t AMR2 : 8; /*!< acceptance mask registers defines which bit positions will be
                                                         compared and which onesare do not care. Setting certain AMR
                                                          bit define corresponding bit in ACR as do not care.                  */
      __IO uint32_t AMR3 : 8; /*!< acceptance mask registers defines which bit positions will be
                                                         compared and which onesare do not care. Setting certain AMR
                                                          bit define corresponding bit in ACR as do not care.                  */
    } AMR_b;                  /*!< BitSize                                                               */
  };

  union {
    __I uint8_t ECC; /*!< ERROR CODE CAPTURE REGISTER                                           */

    struct {
      __I uint8_t BER : 1;   /*!< Bit Error occurred                                                    */
      __I uint8_t STFER : 1; /*!< stuff error occurred                                                  */
      __I uint8_t CRCER : 1; /*!< CRC error occurred                                                    */
      __I uint8_t FRMER : 1; /*!< Frame error occurred                                                  */
      __I uint8_t ACKER : 1; /*!< Acknowledgement error occurred                                        */
      __I uint8_t EDIR : 1;  /*!< direction of transfer while error occurred                            */
      __I uint8_t TXWRN : 1; /*!< set when TXERR counter is greater than or equal to 96                 */
      __I uint8_t RXWRN : 1; /*!< set when RXERR counter is greater than or equal to 96                 */
    } ECC_b;                 /*!< BitSize                                                               */
  };

  union {
    __I uint8_t RXERR; /*!< RECEIVE ERROR COUNTER REGISTER                                        */

    struct {
      __I uint8_t RXERR : 8; /*!< The RXERR register reflects current value of the receive error
                                                         counter                                                               */
    } RXERR_b;               /*!< BitSize                                                               */
  };

  union {
    __I uint8_t TXERR; /*!< RECEIVE ERROR COUNTER REGISTER                                        */

    struct {
      __I uint8_t TXERR : 8; /*!< The TXERR register reflects current value of the transmit error
                                                         counter                                                               */
    } TXERR_b;               /*!< BitSize                                                               */
  };

  union {
    __I uint8_t ALC; /*!< ARBITRATION LOST CODE CAPTURE REGISTER                                */

    struct {
      __I uint8_t ALC : 5; /*!< Arbitration Lost Capture                                              */
    } ALC_b;               /*!< BitSize                                                               */
  };
} CAN0_Type;

/* ================================================================================ */
/* ================                       QEI                      ================ */
/* ================================================================================ */

/**
 * @brief The Motor Control PWM (MCPWM) controller is used to generate a periodic pulse waveform, which is useful in motor control and power control applications (QEI)
 */

typedef struct { /*!< QEI Structure                                                         */

  union {
    __I uint32_t QEI_STATUS_REG; /*!< QEI Status Register                                                   */

    struct {
      __I uint32_t QEI_INDEX : 1;         /*!< This is a direct value from the position signal generator             */
      __I uint32_t QEI_POSITION_B : 1;    /*!< This is a direct value from the position signal generator.Value
                                                         refers to the signal Position_B from the generator.                   */
      __I uint32_t QEI_POSITION_A : 1;    /*!< This is a direct value from the position signal generator.Value
                                                         refers to the signal Position_A from the generator.                   */
      __I uint32_t POSITION_CNTR_ERR : 1; /*!< Count Error Status Flag bit                                           */
      __I uint32_t
        POSITION_CNTR_DIRECTION : 1; /*!< Position Counter Direction Status bit                                 */
    } QEI_STATUS_REG_b;              /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t QEI_CTRL_REG_SET; /*!< QEI Control Set Register                                              */

    struct {
      __I uint32_t QEI_SFT_RST : 1;        /*!< Quadrature encoder soft reset. It is self reset signal.               */
      __IO uint32_t QEI_SWAP_PHASE_AB : 1; /*!< Phase A and Phase B Input Swap Select bit                             */
      __IO uint32_t
        POS_CNT_RST_WITH_INDEX_EN : 1; /*!< Phase A and Phase B Input Swap Select bit                             */
      uint32_t : 1;
      __IO uint32_t
        POS_CNT_DIRECTION_CTRL : 1; /*!< NONE                                                                  */
      __IO uint32_t
        POS_CNT_DIR_FRM_REG : 1; /*!< Position Counter Direction indication from user                       */
      uint32_t : 2;
      __IO uint32_t INDEX_CNT_RST_EN : 1; /*!< NONE                                                                  */
      __IO uint32_t
        DIGITAL_FILTER_BYPASS : 1;  /*!< NONE                                                                  */
      __IO uint32_t TIMER_MODE : 1; /*!< NONE                                                                  */
      __IO uint32_t
        START_VELOCITY_CNTR : 1;          /*!< Starting the velocity counter. It is self reset bit.                  */
      __IO uint32_t QEI_STOP_IN_IDLE : 1; /*!< NONE                                                                  */
      __IO uint32_t
        QEI_POS_CNT_16_BIT_MODE : 1;   /*!< Qei position counter 16 bit mode enable                               */
      __IO uint32_t POS_CNT_RST : 1;   /*!< 1=position counter is going to reset                                  */
      __IO uint32_t INDEX_CNT_RST : 1; /*!< 1= index counter is going to reset.                                   */
    } QEI_CTRL_REG_SET_b;              /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t QEI_CTRL_REG_RESET; /*!< PWM Interrupt mask Register                                           */

    struct {
      __I uint32_t QEI_SFT_RST : 1;        /*!< Quadrature encoder soft reset. It is self reset signal                */
      __IO uint32_t QEI_SWAP_PHASE_AB : 1; /*!< Phase A and Phase B Input Swap Select bit                             */
      __IO uint32_t
        POS_CNT_RST_WITH_INDEX_EN : 1; /*!< Phase A and Phase B Input Swap Select bit                             */
      uint32_t : 1;
      __IO uint32_t
        POS_CNT_DIRECTION_CTRL : 1; /*!< NONE                                                                  */
      __IO uint32_t
        POS_CNT_DIR_FRM_REG : 1; /*!< Position Counter Direction indication from user                       */
      uint32_t : 2;
      __IO uint32_t INDEX_CNT_RST_EN : 1; /*!< NONE                                                                  */
      __IO uint32_t
        DIGITAL_FILTER_BYPASS : 1;  /*!< NONE                                                                  */
      __IO uint32_t TIMER_MODE : 1; /*!< NONE                                                                  */
      __IO uint32_t
        START_VELOCITY_CNTR : 1;          /*!< Starting the velocity counter. It is self reset bit.                  */
      __IO uint32_t QEI_STOP_IN_IDLE : 1; /*!< NONE                                                                  */
      __IO uint32_t
        QEI_POS_CNT_16_BIT_MODE : 1;   /*!< Qei position counter 16 bit mode enable                               */
      __IO uint32_t POS_CNT_RST : 1;   /*!< 1=position counter is going to reset                                  */
      __IO uint32_t INDEX_CNT_RST : 1; /*!< 1= index counter is going to reset.                                   */
    } QEI_CTRL_REG_RESET_REg_b;        /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t QEI_CNTLR_INIT_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t QEI_ENCODING_MODE : 2; /*!< NONE                                                                  */
      uint32_t : 2;
      __IO uint32_t INDEX_MATCH_VALUE : 2; /*!< These bits allow user to specify the state of position A and
                                                         B during index pulse generation.                                      */
      __IO uint32_t DF_CLK_DIVIDE_SLT : 4; /*!< Digital Filter Clock Divide Select bits                               */
      __IO uint32_t
        UNIDIRECTIONAL_VELOCITY : 1; /*!< Uni directional velocity enable.                                      */
      __IO uint32_t
        UNIDIRECTIONAL_INDEX : 1;       /*!< Uni directional index enable.                                         */
      __IO uint32_t INDEX_CNT_INIT : 1; /*!< Index counter initial value in unidirectional index enable mode.      */
    } QEI_CNTLR_INIT_REG_b;             /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t QEI_INDEX_CNT_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t QEI_INDEX_CNT : 16; /*!< Index counter value.User can initialize/change the index counter
                                                         using this register                                                   */
    } QEI_INDEX_CNT_REG_b;              /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t QEI_INDEX_MAX_CNT_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t
        QEI_INDEX_MAX_CNT : 16; /*!< Qei index maximum count.                                              */
    } QEI_INDEX_MAX_CNT_REG_b;  /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t QEI_POSITION_CNT_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t QEI_POSITION_CNT_WR_VALUE_L : 16; /*!< This is used to program/change the value of position counter
                                                         status[15:0]                                                          */
      __IO uint32_t QEI_POSITION_CNT_WR_VALUE_H : 16; /*!< This is used to program/change the value of position counter
                                                         status[31:16].                                                        */
    } QEI_POSITION_CNT_REG_b; /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED;

  union {
    __IO uint32_t
      QEI_POSITION_MAX_CNT_LSW_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t QEI_POSITION_MAX_CNT_L : 16; /*!< Qei position maximum count [15:0].This is a maximum count value
                                                         that is allowed to increment in the position counter                  */
      __IO uint32_t QEI_POSITION_MAX_CNT_H : 16; /*!< Qei position maximum count [31:16].This is a maximum count value
                                                         that is allowed to increment in the position counter                  */
    } QEI_POSITION_MAX_CNT_LSW_REG_b; /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED1;

  union {
    __I uint32_t QEI_INTR_STS_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t
        QEI_POSITION_CNT_RESET_INTR_LEV : 1; /*!< This is raised when the position counter reaches it's extremes   */
      __I uint32_t QEI_INDEX_CNT_MATCH_INTR_LEV : 1; /*!< This is raised when index counter reaches max value loaded in
                                                         to index_max_cnt register.                                            */
      __I uint32_t
        POSITION_CNTR_ERR_INTR_LEV : 1; /*!< Whenever number of possible positions are mismatched with actual
                                                         positions are received between two index pulses this will raised      */
      __I uint32_t
        VELOCITY_LESS_THAN_INTR_LEV : 1; /*!< When velocity count is less than the value given in velocity_value_to_comp
                                                         are register, interrupt is raised                                     */
      __I uint32_t
        QEI_POSITION_CNT_MATCH_INTR_LEV : 1; /*!< This is raised when the position counter reaches position match
                                                         value, which is programmable.                                         */
    } QEI_INTR_STS_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t QEI_INTR_ACK_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t
        QEI_POSITION_CNT_RESET_INTR_LEV : 1; /*!< Qei_position_cnt_reset_intr_ack                                  */
      __IO uint32_t
        QEI_INDEX_CNT_MATCH_INTR_LEV : 1; /*!< NONE                                                                */
      __IO uint32_t
        POSITION_CNTR_ERR_INTR_LEV : 1; /*!< Position_cntr_err_intr_ack                                            */
      __IO uint32_t
        VELOCITY_LESS_THAN_INTR_LEV : 1; /*!< Velocity_less_than_intr_ack                                          */
      __IO uint32_t
        QEI_POSITION_CNT_MATCH_INTR_LEV : 1; /*!< Qei_position_cnt_match_intr_ack                                  */
    } QEI_INTR_ACK_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t QEI_INTR_MASK_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t
        QEI_POSITION_CNT_RESET_INTR_MASK : 1; /*!< Qei_position_cnt_reset_intr_mask                                */
      __IO uint32_t
        QEI_INDEX_CNT_MATCH_INTR_MASK : 1; /*!< Qei_index_cnt_match_intr_mask                                      */
      __IO uint32_t
        POSITION_CNTR_ERR_INTR_MASK : 1; /*!< Position_cntr_err_intr_mask                                          */
      __IO uint32_t
        VELOCITY_LESS_THAN_INTR_MASK : 1; /*!< Velocity_less_than_intr_mask                                        */
      __IO uint32_t
        QEI_POSITION_CNT_MATCH_INTR_MASK : 1; /*!< Qei_position_cnt_match_intr_mask                                */
    } QEI_INTR_MASK_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t QEI_INTR_UNMASK_REg; /*!< NONE                                                                  */

    struct {
      __IO uint32_t
        QEI_POSITION_CNT_RESET_INTR_UNMASK : 1; /*!< Qei_position_cnt_reset_intr_unmask                            */
      __IO uint32_t
        QEI_INDEX_CNT_MATCH_INTR_UNMASK : 1; /*!< Qei_index_cnt_match_intr_unmask                                  */
      __IO uint32_t
        POSITION_CNTR_ERR_INTR_UNMASK : 1; /*!< Position_cntr_err_intr_unmask                                      */
      __IO uint32_t
        VELOCITY_LESS_THAN_INTR_UNMASK : 1; /*!< Velocity_less_than_intr_unmask                                    */
      __IO uint32_t
        QEI_POSITION_CNT_MATCH_INTR_UNMASK : 1; /*!< Qei_position_cnt_match_intr_unmask                            */
    } QEI_INTR_UNMASK_REg_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t QEI_CLK_FREQ_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t QEI_CLK_FREQ : 9; /*!< Indication of clock frequency on which QEI controller is running.     */
    } QEI_CLK_FREQ_REG_b;             /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t QEI_DELTA_TIME_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t
        DELTA_TIME_FOR_VELOCITY : 20; /*!< Delta time LSW to compute velocity                                    */
    } QEI_DELTA_TIME_REG_b;           /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED2;

  union {
    __IO uint32_t QEI_VELOCITY_REG; /*!< NONE                                                                  */

    struct {
      __I uint32_t VELOCITY_VALUE_TO_COMPARE_L : 16; /*!< If read :Velocity value to compare with velocity count LSW If
                                                         write :Velocity LSW count to compare using TA firmware                */
      __O uint32_t VELOCITY_VALUE_TO_COMPARE_H : 16; /*!< If write :Velocity value to compare with velocity count MSW.
                                                         If read :Velocity MSW count to compare using TA firmware              */
    } QEI_VELOCITY_REG_b; /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED3;

  union {
    __IO uint32_t QEI_POSITION_MATCH_REG; /*!< NONE                                                                  */

    struct {
      __IO uint32_t
        POSTION_MATCH_VALUE_L : 16; /*!< Position match value to compare the position counter.                 */
      __IO uint32_t
        POSTION_MATCH_VALUE_H : 16; /*!< Position match value to compare the position counter.                 */
    } QEI_POSITION_MATCH_REG_b;     /*!< BitSize                                                               */
  };
} QEI_Type;

/* ================================================================================ */
/* ================                      UART0                     ================ */
/* ================================================================================ */

/**
 * @brief Universal Asynchronous Receiver/Transmitter is for serial communication with peripherals,modems and datasets (UART0)
 */

typedef struct { /*!< UART0 Structure                                                       */

  union {
    union {
      __IO uint32_t DLL; /*!< Divisor Latch Low                                                     */

      struct {
        __IO uint32_t DLL : 8; /*!< Lower 8-bits of a 16-bit, read/write, Divisor Latch register
                                                         that contains the baud ratedivisor for the UART                       */
      } DLL_b;                 /*!< BitSize                                                               */
    };

    union {
      __O uint32_t THR; /*!< Transmit Holding Register                                             */

      struct {
        __O uint32_t THR : 8; /*!< Data to be transmitted on serial output port                          */
      } THR_b;                /*!< BitSize                                                               */
    };

    union {
      __I uint32_t RBR; /*!< Receive Buffer Register                                               */

      struct {
        __I uint32_t RBR : 8; /*!< Receive Buffer Field                                                  */
      } RBR_b;                /*!< BitSize                                                               */
    };
  };

  union {
    union {
      __IO uint32_t IER; /*!< Interrupt Enable Register                                             */

      struct {
        __IO uint32_t ERBFI : 1; /*!< Enable Received Data Available Interrupt                              */
        __IO uint32_t ETBEI : 1; /*!< Enable Transmit Holding Register Empty Interrupt                      */
        __IO uint32_t ELSI : 1;  /*!< Enable Receiver Line Status Interrupt                                 */
        __IO uint32_t EDSSI : 1; /*!< Enable Modem Status Interrupt                                         */
        uint32_t : 3;
        __IO uint32_t PTIME : 1; /*!< Programmable THRE Interrupt Mode Enable                               */
      } IER_b;                   /*!< BitSize                                                               */
    };

    union {
      __IO uint32_t DLH; /*!< Divisor Latch High                                                    */

      struct {
        __IO uint32_t DLH : 8; /*!< Upper 8-bits of a 16-bit, read/write, Divisor Latch register
                                                         that contains the baud ratedivisor for the UART                       */
      } DLH_b;                 /*!< BitSize                                                               */
    };
  };

  union {
    union {
      __O uint32_t FCR; /*!< FIFO Control Register                                                 */

      struct {
        __O uint32_t FIFOE : 1;  /*!< This enables/disables the transmit (XMIT) and receive (RCVR)
                                                         FIFOs                                                                 */
        __O uint32_t RFIFOR : 1; /*!< RCVR FIFO Reset                                                       */
        __O uint32_t XFIFOR : 1; /*!< XMIT FIFO Reset                                                       */
        __O uint32_t DMAM : 1;   /*!< DMA signalling mode                                                   */
        __O uint32_t TET : 2;    /*!< TX Empty Trigger                                                      */
        __O uint32_t RT : 2;     /*!< This is used to select the trigger level in the receiver FIFO
                                                         at which the Received Data Available Interrupt is generated
				 */
      } FCR_b;                   /*!< BitSize                                                               */
    };

    union {
      __I uint32_t IIR; /*!< Interrupt Identity Register                                           */

      struct {
        __I uint32_t IID : 4; /*!< Interrupt ID                                                          */
        uint32_t : 2;
        __I uint32_t FIFOSE : 2; /*!< This is used to indicate whether the FIFOs are enabled or disabled.
				 */
      } IIR_b;                   /*!< BitSize                                                               */
    };
  };

  union {
    __IO uint32_t LCR; /*!< Line Control Register                                                 */

    struct {
      __IO uint32_t DLS : 2;          /*!< Data Length Select,This is used to select the number of data
                                                         bits per character that the peripheral transmits and receives
			 */
      __IO uint32_t STOP : 1;         /*!< This is used to select the number of stop bits per character
                                                         that the peripheral transmits and receives                            */
      __IO uint32_t PEN : 1;          /*!< This bit is used to enable and disable parity generation and
                                                         detection in transmitted and received serial character                */
      __IO uint32_t EPS : 1;          /*!< This is used to select between even and odd parity                    */
      __IO uint32_t STICK_PARITY : 1; /*!< This bit is used to force parity value                                */
      __IO uint32_t BC : 1;           /*!< This is used to cause a break condition to be transmitted to
                                                         the receiving device                                                  */
      __IO uint32_t DLAB : 1;         /*!< This bit is used to enable reading and writing of the Divisor
                                                         Latch register to set the baud rate of the UART                       */
    } LCR_b;                          /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t MCR; /*!< Modem Control Register                                                */

    struct {
      __IO uint32_t DTR : 1;  /*!< This is used to directly control the Data Terminal Ready (dtr_n)
                                                         output                                                                */
      __IO uint32_t RTS : 1;  /*!< This is used to directly control the Request to Send (rts_n)
                                                         output                                                                */
      __IO uint32_t OUT1 : 1; /*!< This is used to directly control the user-designated Output1
                                                         (out1_n) output                                                       */
      __IO uint32_t OUT2 : 1; /*!< This is used to directly control the user-designated Output2
                                                         (out2_n) output                                                       */
      __IO uint32_t LB : 1;   /*!< This is used to put the UART into a diagnostic mode for test
                                                         purposes                                                              */
      __IO uint32_t AFCE : 1; /*!< This is used to directly control the user-designated Output2
                                                         (out2_n) output                                                       */
      __IO uint32_t SIRE : 1; /*!< This is used to enable/disable the IrDA SIR Mode features             */
    } MCR_b;                  /*!< BitSize                                                               */
  };

  union {
    __I uint32_t LSR; /*!< Line Status Register                                                  */

    struct {
      __I uint32_t DR : 1;   /*!< This is used to indicate that the receiver contains at least
                                                         one character in the RBR or the receiver FIFO                         */
      __I uint32_t OE : 1;   /*!< This is used to indicate the occurrence of an overrun error           */
      __I uint32_t PE : 1;   /*!< This is used to indicate the occurrence of a parity error in
                                                         the receiver if the Parity Enable (PEN) bit (LCR[3]) is set
			 */
      __I uint32_t FE : 1;   /*!< This is used to indicate the occurrence of a framing error in
                                                         the receiver                                                          */
      __I uint32_t BI : 1;   /*!< his is used to indicate the detection of a break sequence on
                                                         the serial input data                                                 */
      __I uint32_t THRE : 1; /*!< Transmit Holding Register Empty bit                                   */
      __I uint32_t TEMT : 1; /*!< Transmitter Empty bit                                                 */
      __I uint32_t RFE : 1;  /*!< This is used to indicate if there is at least one parity error,framing
                                                         error, or break indication in the FIFO                                */
    } LSR_b;                 /*!< BitSize                                                               */
  };

  union {
    __I uint32_t MSR; /*!< Modem Status Register                                                 */

    struct {
      __I uint32_t DCTS : 1; /*!< This is used to indicate that the modem control line cts_n has
                                                         changed since the last time the MSR was read                          */
      __I uint32_t DDSR : 1; /*!< This is used to indicate that the modem control line dsr_n has
                                                         changed since the last time the MSR was read                          */
      __I uint32_t TERI : 1; /*!< This is used to indicate that a change on the input ri_n(from
                                                         an active-low to an inactive-high state) has occurred since
                                                          the last time the MSR was read                                       */
      __I uint32_t DDCD : 1; /*!< This is used to indicate that the modem control line dcd_n has
                                                         changed since the last time the MSR was read                          */
      __I uint32_t CTS : 1;  /*!< This is used to indicate the current state of the modem control
                                                         line cts_n                                                            */
      __I uint32_t DSR : 1;  /*!< This is used to indicate the current state of the modem control
                                                         line dsr_n                                                            */
      __I uint32_t RI : 1;   /*!< This is used to indicate the current state of the modem control
                                                         line ri_n                                                             */
      __I uint32_t DCD : 1;  /*!< This is used to indicate the current state of the modem control
                                                         line dcd_n                                                            */
    } MSR_b;                 /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t SCR; /*!< Scratch pad Register                                                  */

    struct {
      __IO uint32_t SCRATCH_PAD : 8; /*!< This register is for programmers to use as a temporary storage
                                                         space. It has no defined purpose                                      */
    } SCR_b;                         /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t LPDLL; /*!< Low Power Divisor Latch Low Register                                  */

    struct {
      __IO uint32_t LOW_POWER_DLL : 8; /*!< This register makes up the lower 8-bits of a 16-bit, read/write,
                                                         Low Power Divisor Latch register that contains the baud rate
                                                          divisor for the UART, which must give a baud rate of 115.2K
			 */
    } LPDLL_b;                         /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t LPDLH; /*!< Low Power Divisor Latch High Register                                 */

    struct {
      __IO uint32_t LOW_POWER_DLH : 8; /*!< This register makes up the upper 8-bits of a 16-bit, read/write,
                                                         Low Power Divisor Latch register that contains the baud rate
                                                          divisor for the UART, which must give a baud rate of 115200
			 */
    } LPDLH_b;                         /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED[6];

  union {
    __IO uint32_t HDEN; /*!< none                                                                  */

    struct {
      __IO uint32_t FULL_DUPLEX_MODE : 1; /*!< none                                                                  */
      __IO uint32_t TX_MODE_RX_MODE : 1;  /*!< This signal is valid when full_duplex_mode is disabled                */
    } HDEN_b;                             /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED1[5];

  union {
    __IO uint32_t SMCR; /*!< none                                                                  */

    struct {
      __IO uint32_t SYNC_MODE : 1; /*!< none                                                                  */
      __IO uint32_t MST_MODE : 1;  /*!< none                                                                  */
      uint32_t : 2;
      __IO uint32_t CONTI_CLK_MODE : 1; /*!< none                                                                  */
      __IO uint32_t START_STOP_EN : 1;  /*!< none                                                                  */
    } SMCR_b;                           /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED2[5];

  union {
    __IO uint32_t FAR; /*!< none                                                                  */

    struct {
      __IO uint32_t SYNC_MODE : 1; /*!< none                                                                  */
    } FAR_b;                       /*!< BitSize                                                               */
  };

  union {
    __I uint32_t TFR; /*!< none                                                                  */

    struct {
      __IO uint32_t TX_FIFO_RD : 8; /*!< Transmit FIFO Read                                                    */
    } TFR_b;                        /*!< BitSize                                                               */
  };

  union {
    __I uint32_t RFW; /*!< none                                                                  */

    struct {
      __IO uint32_t RFWD : 8; /*!< Receive FIFO Write Data                                               */
      __IO uint32_t RFPE : 1; /*!< Receive FIFO Parity Error                                             */
      __IO uint32_t RFFE : 1; /*!< Receive FIFO Framing Error                                            */
    } RFW_b;                  /*!< BitSize                                                               */
  };

  union {
    __I uint32_t USR; /*!< UART Status Register                                                  */

    struct {
      __I uint32_t BUSY : 1; /*!< Indicates that a serial transfer is in progress                       */
      __I uint32_t TFNF : 1; /*!< To Indicate that the transmit FIFO is not full                        */
      __I uint32_t TFE : 1;  /*!< To Indicate that the transmit FIFO is completely empty                */
      __I uint32_t RFNE : 1; /*!< To Indicate that the receive FIFO contains one or more entries        */
      __I uint32_t RFE : 1;  /*!< To Indicate that the receive FIFO is completely full                  */
    } USR_b;                 /*!< BitSize                                                               */
  };

  union {
    __I uint32_t TFL; /*!< Transmit FIFO Level                                                   */

    struct {
      __I uint32_t FIFO_ADDR_WIDTH : 30; /*!< Transmit FIFO Level. This is indicates the number of data entries
                                                         in the transmit FIFO.                                                 */
    } TFL_b;                             /*!< BitSize                                                               */
  };

  union {
    __I uint32_t RFL; /*!< Receive FIFO Level                                                    */

    struct {
      __I uint32_t FIFO_ADDR_WIDTH : 30; /*!< Receive FIFO Level. This is indicates the number of data entries
                                                         in the receive FIFO.                                                  */
    } RFL_b;                             /*!< BitSize                                                               */
  };

  union {
    __O uint32_t SRR; /*!< Software Reset Register                                               */

    struct {
      __O uint32_t UR : 1;  /*!< UART Reset                                                            */
      __O uint32_t RFR : 1; /*!< RCVR FIFO Reset                                                       */
      __O uint32_t XFR : 1; /*!< XMIT FIFO Reset                                                       */
    } SRR_b;                /*!< BitSize                                                               */
  };

  union {
    __I uint32_t SRTS; /*!< Shadow Request to Send                                                */

    struct {
      __I uint32_t SRTS : 1; /*!< Shadow Request to Send.                                               */
    } SRTS_b;                /*!< BitSize                                                               */
  };

  union {
    __I uint32_t SBCR; /*!< Shadow Break Control Register                                         */

    struct {
      __I uint32_t SBCR : 1; /*!< Shadow Break Control Bit                                              */
    } SBCR_b;                /*!< BitSize                                                               */
  };

  union {
    __I uint32_t SDMAM; /*!< Shadow DMA Mode                                                       */

    struct {
      __I uint32_t SDMAM : 1; /*!< Shadow DMA Mode                                                       */
    } SDMAM_b;                /*!< BitSize                                                               */
  };

  union {
    __I uint32_t SFE; /*!< Shadow FIFO Enable                                                    */

    struct {
      __I uint32_t SFE : 1; /*!< Shadow FIFO Enable                                                    */
    } SFE_b;                /*!< BitSize                                                               */
  };

  union {
    __I uint32_t SRT; /*!< Shadow RCVR Trigger                                                   */

    struct {
      __I uint32_t SRT : 2; /*!< Shadow RCVR Trigger                                                   */
    } SRT_b;                /*!< BitSize                                                               */
  };

  union {
    __I uint32_t STET; /*!< Shadow TX Empty Trigger                                               */

    struct {
      __I uint32_t STET : 2; /*!< Shadow TX Empty Trigger                                               */
    } STET_b;                /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t HTX; /*!< Halt Transmit                                                         */

    struct {
      __IO uint32_t HALT_TX : 1; /*!< This register is use to halt transmissions for testing                */
    } HTX_b;                     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t DMASA; /*!< DMA Software Acknowledge                                              */

    struct {
      __O uint32_t DMA_SOFTWARE_ACK : 1; /*!< This register is use to perform a DMA software acknowledge if
                                                         a transfer needs to be terminated due to an error condition
			 */
    } DMASA_b;                           /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t TCR; /*!< Transceiver Control Register                                          */

    struct {
      __IO uint32_t RS485_EN : 1;  /*!< RS485 Transfer Enable                                                 */
      __IO uint32_t RE_POL : 1;    /*!< Receiver Enable Polarity                                              */
      __IO uint32_t DE_POL : 1;    /*!< Driver Enable Polarity                                                */
      __IO uint32_t XFER_MODE : 2; /*!< Transfer Mode                                                         */
    } TCR_b;                       /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t DE_EN; /*!< Driver Output Enable Register                                         */

    struct {
      __IO uint32_t DE_EN : 1; /*!< DE Enable control                                                     */
    } DE_EN_b;                 /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t RE_EN; /*!< Receiver Output Enable Register                                       */

    struct {
      __IO uint32_t RE_EN : 1; /*!< RE Enable control                                                     */
    } RE_EN_b;                 /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t DET; /*!< Driver Output Enable Timing Register                                  */

    struct {
      __IO uint32_t DE_ASSERT_TIME : 8; /*!< Driver enable assertion time.                                         */
      uint32_t : 8;
      __IO uint32_t DE_DE_ASSERT_TIME : 8; /*!< Driver enable de-assertion time.                                      */
    } DET_b;                               /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t TAT; /*!< TurnAround Timing Register                                            */

    struct {
      __IO uint32_t DE_RE : 16; /*!< Driver Enable to Receiver Enable TurnAround time.                     */
      __IO uint32_t RE_DE : 16; /*!< Receiver Enable to Driver Enable TurnAround time.                     */
    } TAT_b;                    /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t DLF; /*!< Divisor Latch Fraction Register                                       */

    struct {
      __IO uint32_t DLF : 6; /*!< Fractional part of divisor.                                           */
    } DLF_b;                 /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t RAR; /*!< Receive Address Register                                              */

    struct {
      __IO uint32_t RAR : 8; /*!< This is an address matching register during receive mode              */
    } RAR_b;                 /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t TAR; /*!< Transmit Address Register                                             */

    struct {
      __IO uint32_t TAR : 8; /*!< This is an address matching register during transmit mode             */
    } TAR_b;                 /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t LCR_EXT; /*!< Line Extended Control Register                                        */

    struct {
      __IO uint32_t DLS_E : 1;         /*!< Extension for DLS                                                     */
      __IO uint32_t ADDR_MATCH : 1;    /*!< Address Match Mode                                                    */
      __IO uint32_t SEND_ADDR : 1;     /*!< Send address control bit.                                             */
      __IO uint32_t TRANSMIT_MODE : 1; /*!< Transmit mode control bit                                             */
    } LCR_EXT_b;                       /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED3[9];

  union {
    __I uint32_t CPR; /*!< Component Parameter Register                                          */

    struct {
      __I uint32_t APB_DATA_WIDTH : 2; /*!< none                                                                  */
      uint32_t : 2;
      __I uint32_t AFCE_MODE : 1;       /*!< none                                                                  */
      __I uint32_t THRE_MODE : 1;       /*!< none                                                                  */
      __I uint32_t SIR_MODE : 1;        /*!< none                                                                  */
      __I uint32_t SIR_LP_MODE : 1;     /*!< none                                                                  */
      __I uint32_t ADDITIONAL_FEAT : 1; /*!< none                                                                  */
      __I uint32_t FIFO_ACCESS : 1;     /*!< none                                                                  */
      __I uint32_t FIFO_STAT : 1;       /*!< none                                                                  */
      __I uint32_t SHADOW : 1;          /*!< none                                                                  */
      __I uint32_t
        UART_ADD_ENCODED_PARAMS : 1; /*!< none                                                                  */
      __I uint32_t DMA_EXTRA : 1;    /*!< none                                                                  */
      uint32_t : 2;
      __I uint32_t FIFO_MODE : 8; /*!< none                                                                  */
    } CPR_b;                      /*!< BitSize                                                               */
  };

  union {
    __I uint32_t UCV; /*!< UART Component Version                                                */

    struct {
      __I uint32_t UART_COMP_VER : 32; /*!< ASCII value for each number in the version, followed by *             */
    } UCV_b;                           /*!< BitSize                                                               */
  };

  union {
    __I uint32_t CTR; /*!< Component Type Register                                               */

    struct {
      __I uint32_t UART_COMP_VER : 32; /*!< This register contains the peripherals identification code.           */
    } CTR_b;                           /*!< BitSize                                                               */
  };
} USART0_Type;

/* ================================================================================ */
/* ================                      GSPI0                     ================ */
/* ================================================================================ */

/**
 * @brief GSPI, or Generic SPI, is a module which has been derived from QSPI. GSPI can act only as a master  (GSPI0)
 */

typedef struct { /*!< GSPI0 Structure                                                       */

  union {
    __IO uint32_t GSPI_CLK_CONFIG; /*!< GSPI Clock Configuration Register                                     */

    struct {
      __IO uint32_t GSPI_CLK_SYNC : 1; /*!< If the clock frequency to FLASH (spi_clk) and SOC clk is same.        */
      __IO uint32_t GSPI_CLK_EN : 1;   /*!< GSPI clock enable                                                     */
    } GSPI_CLK_CONFIG_b;               /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t GSPI_BUS_MODE; /*!< GSPI Bus Mode Register                                                */

    struct {
      __IO uint32_t GSPI_DATA_SAMPLE_EDGE : 1; /*!< Samples MISO data on clock edges. This should be ZERO for mode3
                                                         clock                                                                 */
      __IO uint32_t
        GSPI_CLK_MODE_CSN0 : 1; /*!< NONE                                                                  */
      __IO uint32_t
        GSPI_CLK_MODE_CSN1 : 1; /*!< NONE                                                                  */
      __IO uint32_t
        GSPI_CLK_MODE_CSN2 : 1; /*!< NONE                                                                  */
      __IO uint32_t
        GSPI_CLK_MODE_CSN3 : 1; /*!< NONE                                                                  */
      __IO uint32_t
        GSPI_GPIO_MODE_ENABLES : 6; /*!< These bits are used to map GSPI on GPIO pins                          */
      __IO uint32_t
        SPI_9115_FEATURE_EN : 1; /*!< 9115 specific features are enabled with this enable                   */
    } GSPI_BUS_MODE_b;           /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED0[2];

  union {
    __IO uint32_t GSPI_MANUAL_CONFIG1; /*!< GSPI Manual Configuration 1 Register                                  */

    struct {
      __IO uint32_t GSPI_MANUAL_CSN : 1; /*!< SPI CS in manual mode                                                 */
      __IO uint32_t GSPI_MANUAL_WR : 1;  /*!< Write enable for manual mode when CS is low.                          */
      __IO uint32_t GSPI_MANUAL_RD : 1;  /*!< Read enable for manual mode when CS is low                            */
      __IO uint32_t
        GSPI_MANUAL_RD_CNT : 10; /*!< Indicates total number of bytes to be read                            */
      __IO uint32_t GSPI_MANUAL_CSN_SELECT : 2; /*!< Indicates which CSn is valid. Can be programmable in manual
                                                         mode                                                                  */
      __IO uint32_t
        SPI_FULL_DUPLEX_ENA : 1; /*!< Full duplex mode enable                                               */
    } GSPI_MANUAL_CONFIG1_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t GSPI_MANUAL_CONFIG2; /*!< GSPI Manual Configuration 2 Register                                  */

    struct {
      __IO uint32_t
        GSPI_WR_DATA_SWAP_MNL_CSN0 : 1; /*!< Swap the write data inside the GSPI controller it-self.               */
      __IO uint32_t
        GSPI_WR_DATA_SWAP_MNL_CSN1 : 1; /*!< Swap the write data inside the GSPI controller it-self.               */
      __IO uint32_t
        GSPI_WR_DATA_SWAP_MNL_CSN2 : 1; /*!< Swap the write data inside the GSPI controller it-self.               */
      __IO uint32_t
        GSPI_WR_DATA_SWAP_MNL_CSN3 : 1; /*!< Swap the write data inside the GSPI controller it-self.               */
      __IO uint32_t
        GSPI_RD_DATA_SWAP_MNL_CSN0 : 1; /*!< Swap the read data inside the GSPI controller it-self.                */
      __IO uint32_t
        GSPI_RD_DATA_SWAP_MNL_CSN1 : 1; /*!< Swap the read data inside the GSPI controller it-self.                */
      __IO uint32_t
        GSPI_RD_DATA_SWAP_MNL_CSN2 : 1; /*!< Swap the read data inside the GSPI controller it-self.                */
      __IO uint32_t
        GSPI_RD_DATA_SWAP_MNL_CSN3 : 1; /*!< Swap the read data inside the GSPI controller it-self.                */
      __IO uint32_t
        GSPI_MANUAL_SIZE_FRM_REG : 1; /*!< Manual reads and manual writes                                        */
      uint32_t : 1;
      __IO uint32_t
        TAKE_GSPI_MANUAL_WR_SIZE_FRM_REG : 1; /*!< NONE                                                            */
      __IO uint32_t MANUAL_GSPI_MODE : 1; /*!< Internally the priority is given to manual mode                       */
      __IO uint32_t LOOP_BACK_EN : 1;     /*!< Internally sets the loop back                        */
    } GSPI_MANUAL_CONFIG2_b;              /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t GSPI_MANUAL_WRITE_DATA2; /*!< GSPI Manual Write Data 2 Register                                     */

    struct {
      __IO uint32_t
        GSPI_MANUAL_WRITE_DATA2 : 4; /*!< Number of bits to be written in write mode                            */
      uint32_t : 3;
      __IO uint32_t USE_PREV_LENGTH : 1; /*!< Use previous length                                                   */
    } GSPI_MANUAL_WRITE_DATA2_b;         /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t GSPI_FIFO_THRLD; /*!< GSPI FIFO Threshold Register                                          */

    struct {
      __IO uint32_t FIFO_AEMPTY_THRLD : 4; /*!< FIFO almost empty threshold                                           */
      __IO uint32_t FIFO_AFULL_THRLD : 4;  /*!< FIFO almost full threshold                                            */
      __IO uint32_t WFIFO_RESET : 1;       /*!< Write FIFO reset                                                      */
      __IO uint32_t RFIFO_RESET : 1;       /*!< read FIFO reset                                                       */
    } GSPI_FIFO_THRLD_b;                   /*!< BitSize                                                               */
  };

  union {
    __I uint32_t GSPI_MANUAL_STATUS; /*!< GSPI Manual Status Register                                           */

    struct {
      __I uint32_t GSPI_BUSY : 1;          /*!< State of Manual mode                                                  */
      __I uint32_t FIFO_FULL_WFIFO_S : 1;  /*!< Full status indication for Wfifo in manual mode                       */
      __I uint32_t FIFO_AFULL_WFIFO_S : 1; /*!< Almost full status indication for Wfifo in manual mode                */
      __I uint32_t FIFO_EMPTY_WFIFO : 1;   /*!< Empty status indication for Wfifo in manual mode                      */
      uint32_t : 1;
      __I uint32_t FIFO_FULL_RFIFO : 1; /*!< Full status indication for Rfifo in manual mode                       */
      uint32_t : 1;
      __I uint32_t FIFO_EMPTY_RFIFO_S : 1; /*!< Empty status indication for Rfifo in manual mode                      */
      __I uint32_t
        FIFO_AEMPTY_RFIFO_S : 1;           /*!< Empty status indication for Rfifo in manual mode                      */
      __I uint32_t GSPI_MANUAL_RD_CNT : 1; /*!< This is a result of 10 bits ORing counter                             */
      __IO uint32_t GSPI_MANUAL_CSN : 1;   /*!< Provide the status of chip select signal                              */
    } GSPI_MANUAL_STATUS_b;                /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t GSPI_INTR_MASK; /*!< GSPI Interrupt Mask Register                                          */

    struct {
      __IO uint32_t GSPI_INTR_MASK_EN : 1; /*!< GSPI Interrupt mask bit                                               */
      __IO uint32_t
        FIFO_AEMPTY_RFIFO_MASK : 1; /*!< NONE                                                                  */
      __IO uint32_t
        FIFO_AFULL_RFIFO_MASK : 1; /*!< NONE                                                                  */
      __IO uint32_t
        FIFO_AEMPTY_WFIFO_MASK : 1; /*!< NONE                                                                  */
      __IO uint32_t
        FIFO_AFULL_WFIFO_MASK : 1; /*!< NONE                                                                  */
      __IO uint32_t
        FIFO_FULL_WFIFO_MASK : 1; /*!< NONE                                                                  */
      __IO uint32_t
        FIFO_EMPTY_RFIFO_MASK : 1; /*!< NONE                                                                  */
    } GSPI_INTR_MASK_b;            /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t GSPI_INTR_UNMASK; /*!< GSPI Interrupt Unmask Register                                        */

    struct {
      __IO uint32_t
        GSPI_INTR_UNMASK_EN : 1; /*!< GSPI Interrupt unmask bit                                             */
      __IO uint32_t
        FIFO_AEMPTY_RFIFO_UNMASK : 1; /*!< NONE                                                                  */
      __IO uint32_t
        FIFO_AFULL_RFIFO_UNMASK : 1; /*!< NONE                                                                  */
      __IO uint32_t
        FIFO_AEMPTY_WFIFO_UNMASK : 1; /*!< NONE                                                                  */
      __IO uint32_t
        FIFO_AFULL_WFIFO_UNMASK : 1; /*!< NONE                                                                  */
      __IO uint32_t
        FIFO_FULL_WFIFO_UNMASK : 1; /*!< NONE                                                                  */
      __IO uint32_t
        FIFO_EMPTY_RFIFO_UNMASK : 1; /*!< NONE                                                                  */
    } GSPI_INTR_UNMASK_b;            /*!< BitSize                                                               */
  };

  union {
    __I uint32_t GSPI_INTR_STS; /*!< GSPI Interrupt Status Register                                        */

    struct {
      __I uint32_t GSPI_INTR_LVL : 1; /*!< GSPI Interrupt status bit                                             */
      __I uint32_t
        FIFO_AEMPTY_RFIFO_LVL : 1; /*!< NONE                                                                  */
      __I uint32_t
        FIFO_AFULL_RFIFO_LVL : 1; /*!< NONE                                                                  */
      __IO uint32_t
        FIFO_AEMPTY_WFIFO_LVL : 1; /*!< NONE                                                                  */
      __I uint32_t
        FIFO_AFULL_WFIFO_LVL : 1; /*!< NONE                                                                  */
      __I uint32_t
        FIFO_FULL_WFIFO_LVL : 1; /*!< NONE                                                                  */
      __IO uint32_t
        FIFO_EMPTY_RFIFO_LVL : 1; /*!< NONE                                                                  */
    } GSPI_INTR_STS_b;            /*!< BitSize                                                               */
  };

  union {
    __O uint32_t GSPI_INTR_ACK; /*!< GSPI Interrupt Acknowledge Register                                   */

    struct {
      __O uint32_t GSPI_INTR_ACK : 1; /*!< GSPI Interrupt status bit                                             */
      __O uint32_t
        FIFO_AEMPTY_RFIFO_ACK : 1; /*!< NONE                                                                  */
      __O uint32_t
        FIFO_AFULL_RFIFO_ACK : 1; /*!< NONE                                                                  */
      __O uint32_t
        FIFO_AEMPTY_WFIFO_ACK : 1; /*!< NONE                                                                  */
      __O uint32_t
        FIFO_AFULL_WFIFO_ACK : 1; /*!< NONE                                                                  */
      __O uint32_t
        FIFO_FULL_WFIFO_ACK : 1; /*!< NONE                                                                  */
      __O uint32_t
        FIFO_EMPTY_RFIFO_ACK : 1; /*!< NONE                                                                  */
    } GSPI_INTR_ACK_b;            /*!< BitSize                                                               */
  };

  union {
    __I uint32_t GSPI_STS_MC; /*!< GSPI State Machine Monitor Register                                   */

    struct {
      __I uint32_t BUS_CTRL_PSTATE : 4; /*!< Provides SPI bus controller present state                             */
    } GSPI_STS_MC_b;                    /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t GSPI_CLK_DIV; /*!< GSPI Clock Division Factor Register                                   */

    struct {
      __IO uint32_t GSPI_CLK_DIV_FACTOR : 8; /*!< Provides GSPI clock division factor to the clock divider, which
                                                         takes SOC clock as input clock and generates required clock
                                                          according to division factor                                         */
    } GSPI_CLK_DIV_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t GSPI_MANUAL_CONFIG3; /*!< GSPI Manual Configuration 3 Register                                  */

    struct {
      __IO uint32_t SPI_MANUAL_RD_LNTHTO_BC : 15; /*!< Bits are used to indicate the total number of bytes to read
                                                         from flash during read operation                                      */
    } GSPI_MANUAL_CONFIG3_b; /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED1[16];

  union {
    __IO uint32_t GSPI_FIFO; /*!< GSPI FIFO Registers                                  */

    struct {
      __IO uint32_t
        SPI_FIFO; /*!< GSPI FIFO registers used for write and read the data in to FIFO Registers                                     */
    } GSPI_FIFO_b; /*!< BitSize                                                               */
  };

} GSPI0_Type;

/* ================================================================================ */
/* ================                      SSI0                      ================ */
/* ================================================================================ */

/**
 * @brief Synchronous Serial Interface(SSI) (SSI0)
 */

typedef struct { /*!< SSIMaster1 Structure                                                  */

  union {
    __IO uint32_t CTRLR0; /*!< Control Register 0                                                    */

    struct {
      __IO uint32_t DFS : 4;     /*!< Select the data frame length (4-bit to 16-bit serial data transfers)  */
      __IO uint32_t FRF : 2;     /*!< Frame Format. Selects which serial protocol transfers the data.       */
      __IO uint32_t SCPHE : 1;   /*!< The serial clock phase selects the relationship of the serial
                                                         clock with the slave select signal                                    */
      __IO uint32_t SCPL : 1;    /*!< Used to select the polarity of the inactive serial clock, which
                                                         is held inactive when the DW_apb_ssimaster is not actively transferring
                                                          data on the serial bus                                               */
      __IO uint32_t TMOD : 2;    /*!< Selects the mode of transfer for serial communication                 */
      __IO uint32_t SLV_OE : 1;  /*!< DW_apb_ssi is configured as a serial-slave device                     */
      __IO uint32_t SRL : 1;     /*!< Shift Register Loop Used for testing purposes only                    */
      __IO uint32_t CFS : 4;     /*!< Control Frame Size Selects the length of the control word for
                                                         the Micro wire frame format                                            */
      __IO uint32_t DFS_MAX : 5; /*!< Used to select the data frame size in 32-bit mode			                */
      __IO uint32_t SPI_FRF : 2; /*!< SPI Frame Format.Selects data frame format for transmitting or receiving data. */
    } CTRLR0_b;                  /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t CTRLR1; /*!< Control Register 1                                                    */

    struct {
      __IO uint32_t NDF : 16; /*!< Number of Data Frames.When TMOD = 10 or TMOD = 11, this register
                                                         field sets the number of data frames to be continuously received
                                                          by the DW_apb_ssi                                                    */
    } CTRLR1_b;               /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t SSIENR; /*!< SSI Enable Register                                                   */

    struct {
      __IO uint32_t SSI_EN : 1; /*!< Enables and disables all DW_apb_ssi operations                        */
    } SSIENR_b;                 /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t MWCR; /*!< Micro wire Control Register                                           */

    struct {
      __IO uint32_t MWMOD : 1; /*!< The Micro wire transfer is sequential or non-sequential               */
      __IO uint32_t MDD : 1;   /*!< The direction of the data word when the Micro wire serial protocol
                                                       			is used                                                             */
      __IO uint32_t MHS : 1;   /*!< Microwire handshaking. Handshaking is enabled or disabled*/
    } MWCR_b;                  /*!< BitSize                                                               */
  };
  union {
    __IO uint32_t SER; /*!< Slave Enable Register*/

    struct {
      __IO uint32_t SSI_NUM_SLAVES : 2; /*!< Slave select enable flag. */
    } SER_b;                            /*!< Bit Size*/
  };

  union {
    __IO uint32_t BAUDR; /*!< Baud Rate Select Register                                             */

    struct {
      __IO uint32_t SCKDV : 16; /*!< SSI Clock Divider.The LSB for this field is always set to 0
                                                         and is unaffected by a writeoperation, which ensures an even
                                                          value is held in this register                                       */
    } BAUDR_b;                  /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t TXFTLR; /*!< Transmit FIFO Threshold Level Register                                */

    struct {
      __IO uint32_t TFT : 4; /*!< Controls the level of entries (or below) at which the transmit
                                                         FIFO controller triggers an interrupt                                 */
    } TXFTLR_b;              /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t RXFTLR; /*!< Receive FIFO Threshold Level                                          */

    struct {
      __IO uint32_t RFT : 4; /*!< Controls the level of entries (or above) at which the receive
                                                         FIFO controller triggers an interrupt                                 */
    } RXFTLR_b;              /*!< BitSize                                                               */
  };

  union {
    __I uint32_t TXFLR; /*!< Transmit FIFO Level Register                                          */

    struct {
      __I uint32_t TXTFL : 5; /*!< Contains the number of valid data entries in the transmit FIFO
			 */
    } TXFLR_b;                /*!< BitSize                                                               */
  };

  union {
    __I uint32_t RXFLR; /*!< Receive FIFO Level Register                                           */

    struct {
      __I uint32_t RXTFL : 5; /*!< Contains the number of valid data entries in the receive FIFO
			 */
    } RXFLR_b;                /*!< BitSize                                                               */
  };

  union {
    __I uint32_t SR; /*!< Status Register                                                       */

    struct {
      __IO uint32_t BUSY : 1;     /*!< indicates that a serial transfer is in progress                       */
      __I uint32_t TFNF : 1;      /*!< Set when the transmit FIFO contains one or more empty locations
                                                         and is cleared when the FIFO is full                                  */
      __I uint32_t TFE_EMPTY : 1; /*!< When the transmit FIFO is completely empty this bit is set            */
      __I uint32_t RFNE : 1;      /*!< Set when the receive FIFO contains one or more entries and is
                                                         cleared when the receive FIFO is empty                                */
      __I uint32_t RFF_FULL : 1;  /*!< When the receive FIFO is completely full this bit is set              */
      __I uint32_t TXE : 1;       /*!< Set if the transmit FIFO is empty when a transfer is started
			 */
    } SR_b;                       /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t IMR; /*!< Interrupt Mask Register                                               */

    struct {
      __IO uint32_t TXEIM : 1; /*!< Transmit FIFO Empty Interrupt Mask                                    */
      __IO uint32_t TXOIM : 1; /*!< Transmit FIFO Overflow Interrupt Mask                                 */
      __IO uint32_t RXUIM : 1; /*!< Receive FIFO Underflow Interrupt Mask                                 */
      __IO uint32_t RXOIM : 1; /*!< Receive FIFO Overflow Interrupt Mask                                  */
      __IO uint32_t RXFIM : 1; /*!< Receive FIFO Full Interrupt Mask                                      */
    } IMR_b;                   /*!< BitSize                                                               */
  };

  union {
    __I uint32_t ISR; /*!< Interrupt Status Register                                             */

    struct {
      __I uint32_t TXEIS : 1; /*!< Transmit FIFO Empty Interrupt Status                                  */
      __I uint32_t TXOIS : 1; /*!< Transmit FIFO Overflow Interrupt Status                               */
      __I uint32_t RXUIS : 1; /*!< Receive FIFO Underflow Interrupt Status                               */
      __I uint32_t RXOIS : 1; /*!< Receive FIFO Overflow Interrupt Status                                */
      __I uint32_t RXFIS : 1; /*!< Receive FIFO Full Interrupt Status                                    */
    } ISR_b;                  /*!< BitSize                                                               */
  };

  union {
    __I uint32_t RISR; /*!< Raw Interrupt Status Register                                         */

    struct {
      __I uint32_t TXEIR : 1; /*!< Transmit FIFO Empty Raw Interrupt Status                              */
      __I uint32_t TXOIR : 1; /*!< Transmit FIFO Overflow Raw Interrupt Status                           */
      __I uint32_t RXUIR : 1; /*!< Receive FIFO Underflow Raw Interrupt Status                           */
      __I uint32_t RXOIR : 1; /*!< Receive FIFO Overflow Raw Interrupt Status                            */
      __I uint32_t RXFIR : 1; /*!< Receive FIFO Full Raw Interrupt Status                                */
    } RISR_b;                 /*!< BitSize                                                               */
  };

  union {
    __I uint32_t TXOICR; /*!< Transmit FIFO Overflow Interrupt Clear Register                       */

    struct {
      __I uint32_t TXOICR : 1; /*!< Clear Transmit FIFO Overflow Interrupt This register reflects
                                                         the status of the interrupt                                           */
    } TXOICR_b;                /*!< BitSize                                                               */
  };

  union {
    __I uint32_t RXOICR; /*!< Receive FIFO Overflow Interrupt Clear Register                        */

    struct {
      __I uint32_t RXOICR : 1; /*!< This register reflects the status of the interrupt A read from
                                                         this register clears the ssi_rxo_intr interrupt                       */
    } RXOICR_b;                /*!< BitSize                                                               */
  };

  union {
    __I uint32_t RXUICR; /*!< Receive FIFO Underflow Interrupt Clear Register                       */

    struct {
      __I uint32_t RXUICR : 1; /*!< This register reflects the status of the interrupt A read from
                                                         this register clears the ssi_rxu_intr interrupt                       */
    } RXUICR_b;                /*!< BitSize                                                               */
  };

  union {
    __I uint32_t MSTICR; /*!< Multi-Master Interrupt Clear Register                                 */

    struct {
      __I uint32_t RXCHDT : 1; /*!< This register reflects the status of the interrupt A read from
                                                         this register clears the ssi_mst_intr interrupt                       */
    } MSTICR_b;                /*!< BitSize                                                               */
  };

  union {
    __I uint32_t ICR; /*!< Interrupt Clear Register                                              */

    struct {
      __I uint32_t ICR : 1; /*!< This register is set if any of the interrupts below are active
                                                         A read clears the ssi_txo_intr, ssi_rxu_intr, ssi_rxo_intr,
                                                          and the ssi_mst_intr interrupts                                      */
    } ICR_b;                /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t DMACR; /*!< DMA Control Register                                                  */

    struct {
      __IO uint32_t RDMAE : 1; /*!< This bit enables/disables the receive FIFO DMA channel                */
      __IO uint32_t TDMAE : 1; /*!< This bit enables/disables the transmit FIFO DMA channel               */
    } DMACR_b;                 /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t DMATDLR; /*!< DMA Transmit Data Level                                               */

    struct {
      __IO uint32_t DMATDL : 4; /*!< This bit field controls the level at which a DMA request is
                                                         made by the transmit logic                                            */
    } DMATDLR_b;                /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t DMARDLR; /*!< DMA Receive Data Level Register                                       */

    struct {
      __IO uint32_t DMARDL : 4; /*!< This bit field controls the level at which a DMA request is
                                                         made by the receive logic                                             */
    } DMARDLR_b;                /*!< BitSize                                                               */
  };

  union {
    __I uint32_t IDR; /*!< Identification Register                                               */

    struct {
      __I uint32_t IDCODE : 32; /*!< This register contains the peripherals identification code            */
    } IDR_b;                    /*!< BitSize                                                               */
  };

  union {
    __I uint32_t SSI_COMP_VERSION; /*!< coreKit version ID register                                           */

    struct {
      __I uint32_t SSI_COMP_VERSION : 32; /*!< Contains the hex representation of the Synopsys component version
			 */
    } SSI_COMP_VERSION_b;                 /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t DR; /*!< Data Register                                                         */

    struct {
      __IO uint32_t DR : 16; /*!< When writing to this register must right-justify the data             */
    } DR_b;                  /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED1[35];

  union {
    __IO uint32_t RX_SAMPLE_DLY; /*!< Rx Sample Delay Register                                              */

    struct {
      __IO uint32_t RSD : 16; /*!< Receive Data (rxd) Sample Delay. This register is used to delay
                                                         the sample of the rxdinput signal.                                    */
    } RX_SAMPLE_DLY_b;        /*!< BitSize                                                               */
  };
  union {
    __IO uint32_t SPI_CTRLR0; /*!< SPI Control Register                                         */

    struct {
      __IO uint32_t TRANS_TYPE : 2;  /*!<Address and Instrction transfer format                              */
      __IO uint32_t ADDR_L : 4;      /*!< Length of the Address to be Transmitted                           */
      __I uint32_t RESERVED : 2;     /*!<                           */
      __IO uint32_t INST_L : 2;      /*!<Dual/Quad mode instruction length in bits             */
      __I uint32_t RESERVE : 1;      /*!<                           */
      __IO uint32_t WAIT_CYCLES : 3; /*!< Receive FIFO Full Raw Interrupt Status                                */
    } SPI_CTRLR0_b;                  /*!< BitSize                                                               */
  };

} SSI0_Type;

/* ================================================================================ */
/* ================                       SIO                      ================ */
/* ================================================================================ */

/**
 * @brief SERIAL GENERAL PERPOSE INPUT/OUTPUT (SIO)
 */

typedef struct { /*!< SIO Structure                                                         */

  union {
    __IO uint32_t ENABLE_REG; /*!< ENABLE REGISTER                                                       */

    struct {
      __IO uint32_t
        SIO_OPERATION_ENABLE : 16; /*!< Contains the Enables for all SIO                                      */
    } ENABLE_REG_b;                /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t PAUSE_REG; /*!< PAUSE REGISTER                                                        */

    struct {
      __IO uint32_t
        SIO_POSITION_COUNTER_DISABLE : 16; /*!< Contains sio position counter disable for all SIOs                  */
    } PAUSE_REG_b;                         /*!< BitSize                                                               */
  };

  union {
    __I uint32_t GPIO_IN_REG; /*!< GPIO Input Register                                                   */

    struct {
      __I uint32_t DATA : 32; /*!< GPIO input pin status                                                 */
    } GPIO_IN_REG_b;          /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t GPIO_OUT_REG; /*!< GPIO Output Register                                                  */

    struct {
      __IO uint32_t DATA : 32; /*!< Value to be loaded on GPIO out pins                                   */
    } GPIO_OUT_REG_b;          /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t GPIO_OEN_REG; /*!< GPIO Output enable Register                                           */

    struct {
      __IO uint32_t DATA : 32; /*!< OEN for the GPIO pins                                                 */
    } GPIO_OEN_REG_b;          /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t GPIO_INTR_EN_SET_REG; /*!< GPIO Interrupt Enable Set Register                                    */

    struct {
      __IO uint32_t INTR_ENABLE_SET : 16; /*!< gpio interrupt enable set register for all SIOs                       */
    } GPIO_INTR_EN_SET_REG_b;             /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t GPIO_INTR_EN_CLEAR_REG; /*!< GPIO Interrupt Enable Clear Register                                  */

    struct {
      __I uint32_t INTR_ENABLE_CLEAR : 16; /*!< gpio interrupt enable Clear register for all SIOs                     */
    } GPIO_INTR_EN_CLEAR_REG_b;            /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t GPIO_INTR_MASK_SET_REG; /*!< GPIO Interrupt Enable Clear Register                                  */

    struct {
      __IO uint32_t INTR_MASK_SET : 16; /*!< Common gpio interrupt mask set register for all SIOs                  */
    } GPIO_INTR_MASK_SET_REG_b;         /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      GPIO_INTR_MASK_CLEAR_REG; /*!< GPIO Interrupt Enable Clear Register                                  */

    struct {
      __I uint32_t INTR_MASK_CLEAR : 16; /*!< gpio interrupt mask clear register for all SIOs                       */
    } GPIO_INTR_MASK_CLEAR_REG_b;        /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t GPIO_INTR_STATUS_REG; /*!< GPIO Interrupt Status Register                                        */

    struct {
      __I uint32_t INTR_MASK_CLEAR : 16; /*!< gpio interrupt mask clear register for all SIOs                       */
    } GPIO_INTR_STATUS_REG_b;            /*!< BitSize                                                               */
  };

  union {
    __I uint32_t SHIFT_COUNTER[16]; /*!< Shift counter register                                                */

    struct {
      __I uint32_t SHIFT_COUNTER : 14; /*!< shift counter current value                                           */
    } SHIFT_COUNTER_b[16];             /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t BUFFER_REG[16]; /*!< Buffer Register                                                       */

    struct {
      __IO uint32_t DATA : 32; /*!< Data to load into the shift register                                  */
    } BUFFER_REG_b[16];        /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      SHIFT_COUNT_PRELOAD_REG[16]; /*!< Shift counter Reload Register                                         */

    struct {
      __IO uint32_t RELOAD_VALUE : 14; /*!< division factor required to generate shift clock                      */
      uint32_t : 1;
      __IO uint32_t REVERSE_LOAD : 1; /*!< When set, the data on APB is loaded to buffer is reverse order        */
    } SHIFT_COUNT_PRELOAD_REG_b[16];  /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t DATA_POS_COUNT_REG[16]; /*!< Data Position Counter Register                                        */

    struct {
      __IO uint32_t RELOAD_VALUE : 8;     /*!< No. of shifts to happen before reloading the shift register
                                                         with data/ pausing the operation                                      */
      __IO uint32_t POSITION_COUNTER : 8; /*!< The position counter can be loaded via AHB                            */
    } DATA_POS_COUNT_REG_b[16];           /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t CONFIG_REG[16]; /*!< Configuration Register                                                */

    struct {
      __IO uint32_t FULL_ENABLE : 1;  /*!< When set, fifo full indication would be asserted when internal
                                                         buffer is full                                                        */
      __IO uint32_t EMPTY_ENABLE : 1; /*!< When set, fifo full indication would be asserted when internal
                                                         buffer is empty                                                       */
      __IO uint32_t EDGE_SEL : 1;     /*!< edge selection                                                        */
      __IO uint32_t CLK_SEL : 1;      /*!< clock selection                                                       */
      __IO uint32_t
        IGNORE_FIRST_SHIFT_CONDITION : 1; /*!< data shift condition                                                */
      __IO uint32_t
        FLOW_CONTROL_ENABLED : 1; /*!< flow control                                                          */
      __IO uint32_t
        PATTERN_MATCH_ENABLE : 1;       /*!< pattern match                                                         */
      __IO uint32_t QUALIFIER_MODE : 1; /*!< qualifier mode                                                        */
      __IO uint32_t QUALIFY_CLOCK : 1;  /*!< qualify clock                                                         */
      __IO uint32_t INVERT_CLOCK : 1;   /*!< invert clock                                                          */
      __IO uint32_t PARALLEL_MODE : 2;  /*!< No. of bits to shift/capture at valid clk edge                        */
      __IO uint32_t
        PIN_DETECTION_MODE : 2;        /*!< Pin mode to be considered for gpio interrupt                          */
      __IO uint32_t SET_CLK_OUT : 1;   /*!< When high sets the sio clock_out port. This is used only when
                                                         sio is not enabled                                                    */
      __IO uint32_t RESET_CLK_OUT : 1; /*!< When high resets the sio clock_out port. This is used only when
                                                         sio is not enabled                                                    */
      __IO uint32_t
        LOAD_DATA_POS_CNTR_VIA_APB : 1; /*!< When set, data position counter can be loaded via APB                 */
    } CONFIG_REG_b[16];                 /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PATTERN_MATCH_MASK_REG_SLICE_0; /*!< Pattern Match Mask Register 0                                         */

    struct {
      __IO uint32_t
        MATCH_MASK_LOWER16_BITS : 32;   /*!< Enable for lower 16 bits                                              */
    } PATTERN_MATCH_MASK_REG_SLICE_0_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PATTERN_MATCH_MASK_REG_slice_1; /*!< Pattern Match Mask Register Slice 1                                   */

    struct {
      __IO uint32_t
        MATCH_MASK_LOWER16_BITS : 32;   /*!< Enable for lower 16 bits                                              */
    } PATTERN_MATCH_MASK_REG_slice_1_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PATTERN_MATCH_MASK_REG_SLICE_2; /*!< Pattern Match Mask Register Slice 2                                   */

    struct {
      __IO uint32_t
        MATCH_MASK_LOWER16_BITS : 32;   /*!< Enable for lower 16 bits                                              */
    } PATTERN_MATCH_MASK_REG_SLICE_2_b; /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED[5];

  union {
    __IO uint32_t
      PATTERN_MATCH_MASK_REG_SLICE_8; /*!< Pattern Match Mask Register Slice 8                                   */

    struct {
      __IO uint32_t
        MATCH_MASK_LOWER16_BITS : 32;   /*!< Enable for lower 16 bits                                              */
    } PATTERN_MATCH_MASK_REG_SLICE_8_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PATTERN_MATCH_MASK_REG_SLICE_9; /*!< Pattern Match Mask Register Slice 9                                   */

    struct {
      __IO uint32_t
        MATCH_MASK_LOWER16_BITS : 32;   /*!< Enable for lower 16 bits                                              */
    } PATTERN_MATCH_MASK_REG_SLICE_9_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PATTERN_MATCH_MASK_REG_SLICE_10; /*!< Pattern Match Mask Register Slice 10                                  */

    struct {
      __IO uint32_t
        MATCH_MASK_LOWER16_BITS : 32;    /*!< Enable for lower 16 bits                                              */
    } PATTERN_MATCH_MASK_REG_SLICE_10_b; /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED1[5];

  union {
    __IO uint32_t
      PATTERN_MATCH_REG_SLICE_0; /*!< Pattern Match Mask Register Slice 0                                   */

    struct {
      __IO uint32_t
        PATTERN_MATCH_LOWER16_BITS : 32; /*!< Lower 16-bits of pattern to be detected                               */
    } PATTERN_MATCH_REG_SLICE_0_b;       /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PATTERN_MATCH_REG_SLICE_1; /*!< Pattern Match Mask Register Slice 1                                   */

    struct {
      __IO uint32_t
        PATTERN_MATCH_LOWER16_BITS : 32; /*!< Lower 16-bits of pattern to be detected                               */
    } PATTERN_MATCH_REG_SLICE_1_b;       /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PATTERN_MATCH_REG_SLICE_2; /*!< Pattern Match Mask Register Slice 2                                   */

    struct {
      __IO uint32_t
        PATTERN_MATCH_LOWER16_BITS : 32; /*!< Lower 16-bits of pattern to be detected                               */
    } PATTERN_MATCH_REG_SLICE_2_b;       /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED2[5];

  union {
    __IO uint32_t
      PATTERN_MATCH_REG_SLICE_8; /*!< Pattern Match Mask Register Slice 8                                   */

    struct {
      __IO uint32_t
        PATTERN_MATCH_LOWER16_BITS : 32; /*!< Lower 16 bits of pattern to be detected                               */
    } PATTERN_MATCH_REG_SLICE_8_b;       /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PATTERN_MATCH_REG_SLICE_9; /*!< Pattern Match Mask Register Slice 9                                   */

    struct {
      __IO uint32_t
        PATTERN_MATCH_LOWER16_BITS : 32; /*!< Lower 16 bits of pattern to be detected                               */
    } PATTERN_MATCH_REG_SLICE_9_b;       /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PATTERN_MATCH_REG_SLICE_10; /*!< Pattern Match Mask Register Slice 10                                  */

    struct {
      __IO uint32_t
        PATTERN_MATCH_LOWER16_BITS : 32; /*!< Lower 16 bits of pattern to be detected                               */
    } PATTERN_MATCH_REG_SLICE_10_b;      /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED3[7];

  union {
    __IO uint32_t SHIFT_INTR_EN_SET_REG; /*!< Shift Interrupt Enable Set Register                                   */

    struct {
      __IO uint32_t INTR_ENABLE_SET : 16; /*!< Common shift interrupt enable set register for all SIOs               */
    } SHIFT_INTR_EN_SET_REG_b;            /*!< BitSize                                                               */
  };

  union {
    __I uint32_t SHIFT_INTR_EN_CLEAR_REG; /*!< Shift Interrupt Enable Clear Register                                 */

    struct {
      __I uint32_t INRT_ENABLE_CLEAR : 16; /*!< Common shift interrupt enable Clear register for all SIOs             */
    } SHIFT_INTR_EN_CLEAR_REG_b;           /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t SHIFT_INTR_MASK_SET_REG; /*!< Shift Interrupt Mask Set Register                                     */

    struct {
      __IO uint32_t
        INRT_ENABLE_CLEAR : 16;  /*!< Common shift interrupt enable Set register for all SIOs               */
    } SHIFT_INTR_MASK_SET_REG_b; /*!< BitSize                                                               */
  };

  union {
    __I uint32_t
      SHIFT_INTR_MASK_CLEAR_REG; /*!< Shift Interrupt Mask Clear Register                                   */

    struct {
      __I uint32_t INTR_MASK_CLEAR : 16; /*!< Common shift interrupt mask clear register for all SIOs               */
    } SHIFT_INTR_MASK_CLEAR_REG_b;       /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t SHIFT_INTR_STATUS_REG; /*!< Shift Interrupt Status Register                                       */

    struct {
      __IO uint32_t INTR_ENABLE_SET : 16; /*!< Common shift interrupt mask clear register for all SIOs               */
    } SHIFT_INTR_STATUS_REG_b;            /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t SWAP_INTR_EN_SET_REG; /*!< Swap Interrupt Enable Set Register                                    */

    struct {
      __IO uint32_t INTR_ENABLE_SET : 16; /*!< Swap interrupt enable set register for all SIOs                       */
    } SWAP_INTR_EN_SET_REG_b;             /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t SWAP_INTR_EN_CLEAR_REG; /*!< Swap Interrupt Enable Clear Register                                  */

    struct {
      __I uint32_t INTR_ENABLE_CLEAR : 16; /*!< Swap interrupt enable Clear register for all SIOs                     */
    } SWAP_INTR_EN_CLEAR_REG_b;            /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t SWAP_INTR_MASK_SET_REG; /*!< Swap Interrupt Mask Set Register                                      */

    struct {
      __IO uint32_t INTR_MASK_SET : 16; /*!< Common swap interrupt mask set register for all SIOs                  */
    } SWAP_INTR_MASK_SET_REG_b;         /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      SWAP_INTR_MASK_CLEAR_REG; /*!< Swap Interrupt Mask Clear Register                                    */

    struct {
      __I uint32_t INTR_MASK_CLEAR : 16; /*!< Common swap interrupt mask Clear register for all SIOs                */
    } SWAP_INTR_MASK_CLEAR_REG_b;        /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t SWAP_INTR_STATUS_REG; /*!< Swap Interrupt Statusr Register                                       */

    struct {
      __IO uint32_t INTR_ENABLE_SET : 16; /*!< Common swap interrupt status register for all SIOs                    */
    } SWAP_INTR_STATUS_REG_b;             /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PATTERN_MATCH_INTR_EN_SET_REG; /*!< Pattern Match Interrupt Enable Set Register                           */

    struct {
      __IO uint32_t INTR_ENABLE_SET : 16; /*!< Common pattern or buffer under run interrupt enable set register
                                                         for all SIOs. Each bit corresponds to one SIO                         */
    } PATTERN_MATCH_INTR_EN_SET_REG_b;    /*!< BitSize                                                               */
  };

  union {
    __I uint32_t
      PATTERN_MATCH_INTR_EN_CLEAR_REG; /*!< Pattern Match Interrupt Enable Clear Register                         */

    struct {
      __I uint32_t INRT_ENABLE_CLEAR : 16; /*!< Common pattern or buffer under run interrupt enable clear register
                                                         for all SIOs                                                          */
    } PATTERN_MATCH_INTR_EN_CLEAR_REG_b;   /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PATTERN_MATCH_INTR_MASK_SET_REG; /*!< Pattern Match Interrupt Mask Set Register                             */

    struct {
      __IO uint32_t INTR_MASK_SET : 16;  /*!< Common pattern or buffer under run interrupt mask set register
                                                         for all SIOs                                                          */
    } PATTERN_MATCH_INTR_MASK_SET_REG_b; /*!< BitSize                                                               */
  };

  union {
    __I uint32_t
      PATTERN_MATCH_INTR_MASK_CLEAR_REG; /*!< Pattern Match Interrupt Mask Clear Register                          */

    struct {
      __I uint32_t INTR_MASK_CLEAR : 16;   /*!< Common pattern or buffer under run interrupt mask clear register
                                                         for all SIOs                                                          */
    } PATTERN_MATCH_INTR_MASK_CLEAR_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      PATTERN_MATCH_INTR_STATUS_REG; /*!< Pattern Match Interrupt Status Register                               */

    struct {
      __IO uint32_t INTR_STATUS : 16;  /*!< Common pattern interrupt status register for all SIOs                 */
    } PATTERN_MATCH_INTR_STATUS_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t BUFFER_INTR_STATUS_REG; /*!< Buffer Interrupt Status Register                                      */

    struct {
      __IO uint32_t INTR_STATUS : 16; /*!< Common pattern interrupt status register for all SIOs                 */
    } BUFFER_INTR_STATUS_REG_b;       /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t OUT_MUX_REG[16]; /*!< Output muxing Register                                                */

    struct {
      __IO uint32_t DOUT_OEN_SEL : 3; /*!< OEN select for GPIO pin 0                                             */
      __IO uint32_t DOUT_SEL : 3;     /*!< Output mux select for GPIO pin 0                                      */
    } OUT_MUX_REG_b[16];              /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t INPUT_MUX_REG[16]; /*!< Input muxing Register                                                 */

    struct {
      __IO uint32_t CLK_SEL : 3;          /*!< Input clock select for SIO 0                                          */
      __IO uint32_t QUALIFIER_SELECT : 2; /*!< qualifier select                                                      */
      __IO uint32_t QUALIFIER_MODE : 2;   /*!< qualifier mode                                                        */
      __IO uint32_t DIN_SEL : 3;          /*!< Data in mux select                                                    */
    } INPUT_MUX_REG_b[16];                /*!< BitSize                                                               */
  };
} SIO_Type;

/* ================================================================================ */
/* ================                      QSPI                      ================ */
/* ================================================================================ */

/**
 * @brief The queued serial peripheral interface module provides a serial peripheral interface with queued transfer capability  (QSPI)
 */

typedef struct { /*!< QSPI Structure                                                        */

  union {
    __IO uint32_t QSPI_CLK_CONFIG; /*!< QSPI Clock Configuration Register                                     */

    struct {
      __IO uint32_t QSPI_AUTO_CSN_HIGH_CNT : 5; /*!< Minimum SOC clock cycles, during which QSPI auto csn should
                                                         be high between consecutive CSN assertions                            */
      __IO uint32_t QSPI_CLK_SYNC : 1;          /*!< If the clock frequency to FLASH(spi_clk) and QSPI(hclk) controller
                                                         is same, this bit can be set to one to by-pass the syncros results
                                                          in time consumption                                                  */
      uint32_t : 2;
      __IO uint32_t QSPI_CLK_EN : 1; /*!< QSPI clock enable                                                     */
    } QSPI_CLK_CONFIG_b;             /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t QSPI_BUS_MODE; /*!< QSPI Bus Mode Register                                                */

    struct {
      __IO uint32_t
        QSPI_9116_FEATURE_EN : 1; /*!< 9115 specific features are enabled with this enable                   */
      __IO uint32_t QSPI_MAN_MODE_CONF_CSN0 : 2; /*!< Configures the QSPI flash for Single/Dual/Quad mode operation
                                                         in manual mode                                                        */
      __IO uint32_t AUTO_MODE_RESET : 1;  /*!< QSPI Auto controller reset. This is not a Self clearing bit           */
      __IO uint32_t QSPI_PREFETCH_EN : 1; /*!< Pre-fetch of data from the model which is connected to QSPI,
                                                         automatically with out reading on AHB and is supplied to AHB,
                                                          when address is matched with AHB read transaction address            */
      __IO uint32_t QSPI_WRAP_EN : 1;     /*!< Model wrap is considered with this bit and uses wrap instruction
                                                         to read from FLASH                                                    */
      __IO uint32_t
        QSPI_AUTO_MODE_FRM_REG : 1; /*!< QSPI Mode of Operation                                                */
      uint32_t : 1;
      __IO uint32_t QSPI_D2_OEN_CSN0 : 1;  /*!< Direction Control for SPI_IO2 in case of dual/single mode for
                                                         chip select0 csn0. It is used both in Auto and Manual Mode            */
      __IO uint32_t QSPI_D3_OEN_CSN0 : 1;  /*!< Direction Control for SPI_IO3 in case of dual/single mode for
                                                         chip select0 csn0. It is used both in Auto and Manual Mode.           */
      __IO uint32_t QSPI_D2_DATA_CSN0 : 1; /*!< Value of SPI_IO2 in case of dual/single mode for chip select0
                                                         csn0. It is used both in Auto and Manual Mode.                        */
      __IO uint32_t QSPI_D3_DATA_CSN0 : 1; /*!< Value of SPI_IO3 in case of dual/single mode for chip select0
                                                         csn0. It is used both in Auto and Manual Mode                         */
      __IO uint32_t QSPI_D2_OEN_CSN1 : 1;  /*!< Direction Control for SPI_IO2 in case of dual/single mode for
                                                         chip select1 csn1                                                     */
      __IO uint32_t QSPI_D3_OEN_CSN1 : 1;  /*!< Direction Control for SPI_IO3 in case of dual/single mode for
                                                         chip select1 csn1                                                     */
      __IO uint32_t QSPI_D2_DATA_CSN1 : 1; /*!< Direction Control for SPI_IO3 in case of dual/single mode for
                                                         chip select1 csn1                                                     */
      __IO uint32_t QSPI_D3_DATA_CSN1 : 1; /*!< Value of SPI_IO3 in case of dual/single mode for chip select1
                                                         csn1                                                                  */
      __IO uint32_t
        QSPI_DATA_SAMPLE_EDGE : 1; /*!< Samples MISO data on clock edges                                      */
      __IO uint32_t
        QSPI_CLK_MODE_CSN0 : 1; /*!< QSPI Clock Mode                                                       */
      __IO uint32_t
        QSPI_CLK_MODE_CSN1 : 1; /*!< QSPI Clock Mode                                                       */
      __IO uint32_t
        QSPI_CLK_MODE_CSN2 : 1; /*!< QSPI Clock Mode                                                       */
      __IO uint32_t
        QSPI_CLK_MODE_CSN3 : 1; /*!< QSPI Clock Mode                                                       */
      uint32_t : 3;
      __IO uint32_t QSPI_D2_OEN_CSN2 : 1;  /*!< Direction Control for SPI_IO2 in case of dual/single mode for
                                                         chip select2 csn2                                                     */
      __IO uint32_t QSPI_D3_OEN_CSN2 : 1;  /*!< Direction Control for SPI_IO3 in case of dual/single mode for
                                                         chip select2 csn2                                                     */
      __IO uint32_t QSPI_D2_DATA_CSN2 : 1; /*!< Value of SPI_IO2 in case of dual/single mode for chip select2
                                                         csn2                                                                  */
      __IO uint32_t QSPI_D3_DATA_CSN2 : 1; /*!< Value of SPI_IO3 in case of dual/single mode for chip select2
                                                         csn2                                                                  */
      __IO uint32_t QSPI_D2_OEN_CSN3 : 1;  /*!< Direction Control for SPI_IO2 in case of dual/single mode for
                                                         chip select3 csn3                                                     */
      __IO uint32_t QSPI_D3_OEN_CSN3 : 1;  /*!< Direction Control for SPI_IO3 in case of dual/single mode for
                                                         chip select3 csn3                                                     */
      __IO uint32_t QSPI_D2_DATA_CSN3 : 1; /*!< Value of SPI_IO2 in case of dual/single mode for chip select3
                                                         csn3                                                                  */
      __IO uint32_t QSPI_D3_DATA_CSN3 : 1; /*!< Value of SPI_IO3 in case of dual/single mode for chip select3
                                                         csn3                                                                  */
    } QSPI_BUS_MODE_b;                     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t QSPI_AUTO_CONFIG_1; /*!< QSPI Auto Controller Configuration 1 Register                         */

    struct {
      __IO uint32_t
        QSPI_EXT_BYTE_MODE_CSN0 : 2; /*!< Mode of operation of QSPI in the extra byte phase                     */
      __IO uint32_t
        QSPI_DUMMY_MODE_CSN0 : 2; /*!< Mode of operation of QSPI in instruction phase                        */
      __IO uint32_t
        QSPI_ADDR_MODE_CSN0 : 2; /*!< Mode of operation of QSPI in instruction phase                        */
      __IO uint32_t
        QSPI_CMD_MODE_CSN0 : 2; /*!< Mode of operation of QSPI in instruction phase                        */
      __IO uint32_t
        QSPI_DATA_MODE_CSN0 : 2; /*!< Mode of operation of QSPI in DATA phase                               */
      __IO uint32_t QSPI_EXTRA_BYTE_CSN0 : 8;    /*!< Value of the extra byte to be transmitted, if the extra byte
                                                         mode is enabled                                                       */
      __IO uint32_t QSPI_EXTRA_BYTE_EN_CSN0 : 2; /*!< Value of the extra byte to be transmitted, if the extra byte
                                                         mode is enabled                                                       */
      uint32_t : 3;
      __IO uint32_t QSPI_PG_JUMP_CSN0 : 1; /*!< NONE                                                                  */
      __IO uint32_t QSPI_DUMMY_BYTES_INCR_CSN0 : 4; /*!< Specifies the number of dummy bytes 0 to 7 for the selected
                                                         SPI mode                                                              */
      __IO uint32_t QSPI_DUMMY_BYTES_WRAP_CSN0 : 4; /*!< Specifies the number of dummy bytes 0 to 7 for the selected
                                                         SPI mode in case of wrap instruction                                  */
    } QSPI_AUTO_CONFIG_1_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t QSPI_AUTO_CONFIG_2; /*!< QSPI Auto Controller Configuration 2 Register                         */

    struct {
      __IO uint32_t
        QSPI_RD_DATA_SWAP_AUTO_CSN0 : 1; /*!< NONE                                                                 */
      __IO uint32_t
        QSPI_ADR_SIZE_16_BIT_AUTO_MODE_CSN0 : 1; /*!< NONE                                                         */
      __IO uint32_t
        QSPI_CONTI_RD_EN_CSN0 : 1; /*!< NONE                                                                  */
      __IO uint32_t
        DUMMY_BYTES_WR_RD_CSN0 : 1; /*!< Dummy bytes to the model to be read or to be write                    */
      __IO uint32_t
        QSPI_DUMMY_BYTES_JMP_CSN : 4;      /*!< Dummy cycles to be selected in case of JUMP                           */
      __IO uint32_t QSPI_RD_INST_CSN0 : 8; /*!< Read instruction to be used for the selected SPI modes and when
                                                         wrap                                                                  */
      __IO uint32_t QSPI_RD_WRAP_INT_CSN0 : 8; /*!< Read instruction to be used, when wrap mode is supported by
                                                         QSPI flash                                                            */
      __IO uint32_t
        QSPI_PG_JUMP_INST_CSN0 : 8; /*!< Read instruction to be used, when Page jump is to be used             */
    } QSPI_AUTO_CONFIG_2_b;         /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t QSPI_MANUAL_CONFIG1; /*!< QSPI Manual Configuration 1 Register                                  */

    struct {
      __IO uint32_t QSPI_MANUAL_CSN : 1; /*!< SPI CS in manual mode                                                 */
      __IO uint32_t QSPI_MANUAL_WR : 1;  /*!< Write enable for manual mode when CS is low                           */
      __IO uint32_t QSPI_MANUAL_RD : 1;  /*!< Read enable for manual mode when CS is low                            */
      __IO uint32_t QSPI_MANUAL_RD_CNT : 10; /*!< Indicates total number of bytes to be read along with 31:27
                                                         bits of this register.Maximum length supported is 32k bytes           */
      __IO uint32_t
        QSPI_MANUAL_CSN_SELECT : 2; /*!< Indicates which CSn is valid                                          */
      uint32_t : 4;
      __IO uint32_t
        QSPI_MANUAL_SIZE_FRM_REG : 2; /*!< Manual reads and manual writes follow this size                       */
      __IO uint32_t
        TAKE_QSPI_MANUAL_WR_SIZE_FRM_REG : 1; /*!< NONE                                                            */
      __IO uint32_t
        QSPI_FULL_DUPLEX_EN : 1; /*!< Full duplex mode enable.                                              */
      uint32_t : 2;
      __IO uint32_t
        HW_CTRLD_QSPI_MODE_CTRL : 1; /*!< Hardware controlled qspi mode in between AUTO and manual              */
      __IO uint32_t
        QSPI_MANUAL_QSPI_MODE : 1; /*!< Internally the priority is given to manual mode                       */
    } QSPI_MANUAL_CONFIG1_b;       /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t QSPI_MANUAL_CONFIG2; /*!< QSPI Manual Configuration 2 Register                                  */

    struct {
      __IO uint32_t
        QSPI_WR_DATA_SWAP_MNL_CSN0 : 1; /*!< Swap the write data inside the QSPI controller it-self                */
      __IO uint32_t
        QSPI_WR_DATA_SWAP_MNL_CSN1 : 1; /*!< Swap the write data inside the QSPI controller it-self.               */
      __IO uint32_t
        QSPI_WR_DATA_SWAP_MNL_CSN2 : 1; /*!< Swap the write data inside the QSPI controller itself.                */
      __IO uint32_t
        QSPI_WR_DATA_SWAP_MNL_CSN3 : 1; /*!< Swap the write data inside the QSPI controller itself.                */
      __IO uint32_t
        QSPI_RD_DATA_SWAP_MNL_CSN0 : 1; /*!< Swap the read data inside the QSPIcontroller it self.                 */
      __IO uint32_t
        QSPI_RD_DATA_SWAP_MNL_CSN1 : 1; /*!< Swap the read data inside the QSPIcontroller itself.                  */
      __IO uint32_t
        QSPI_RD_DATA_SWAP_MNL_CSN2 : 1; /*!< Swap the read data inside the QSPIcontroller it-self                  */
      __IO uint32_t
        QSPI_RD_DATA_SWAP_MNL_CSN3 : 1; /*!< Swap the read data inside the QSPIcontroller itself                   */
      __IO uint32_t QSPI_MAN_MODE_CONF_CSN1 : 2; /*!< Configures the QSPI flash for Single/Dual/Quad mode operation
                                                         in manual mode for chip select1 csn1                                  */
      __IO uint32_t
        QSPI_MAN_MODE_CONF_CSN2 : 2; /*!< Configures the QSPI flash for Single or Dual or Quad mode operation
                                                         in manual mode for chip select2 csn2                                  */
      __IO uint32_t
        QSPI_MAN_MODE_CONF_CSN3 : 2;  /*!< Configures the QSPI flash for Single or Dual or Quad mode operation
                                                         in manual mode for chip select3 csn3                                  */
      __IO uint32_t LOOP_BACK_EN : 1; /*!< Internal loop back test mode.                                         */
      __IO uint32_t
        QSPI_MANUAL_DDR_PHASE : 1;       /*!< DDR operations can be performed even in manual mode                   */
      __IO uint32_t QSPI_DDR_CLK_EN : 1; /*!< DDR operations can be performed even in manual mode                   */
    } QSPI_MANUAL_CONFIG2_b;             /*!< BitSize                                                               */
  };
  __IO uint32_t RESERVED3;

  union {
    __IO uint32_t QSPI_FIFO_THRLD; /*!< QSPI FIFO Threshold Register                                          */

    struct {
      __IO uint32_t FIFO_AEMPTY_THRLD : 4; /*!< FIFO almost empty threshold                                           */
      __IO uint32_t FIFO_AFULL_THRLD : 4;  /*!< FIFO almost full threshold                                            */
      __IO uint32_t WFIFO_RESET : 1;       /*!< Write fifo reset                                                      */
      __IO uint32_t RFIFO_RESET : 1;       /*!< Read fifo reset                                                       */
    } QSPI_FIFO_THRLD_b;                   /*!< BitSize                                                               */
  };

  union {
    __I uint32_t QSPI_MANUAL_STATUS; /*!< QSPI Manual Status Register                                           */

    struct {
      __I uint32_t QSPI_BUSY : 1;          /*!< State of Manual mode.                                                 */
      __I uint32_t FIFO_FULL_WFIFO_S : 1;  /*!< Status indication for Wfifo in manual mode                            */
      __I uint32_t FIFO_AFULL_WFIFO_S : 1; /*!< Status indication for Wfifo in manual mode                            */
      __I uint32_t FIFO_EMPTY_WFIFO : 1;   /*!< Status indication for Wfifo in manual mode                            */
      __I uint32_t FIFO_AEMPTY_WFIFO : 1;  /*!< Status indication for Wfifo in manual mode                            */
      __I uint32_t FIFO_FULL_RFIFO : 1;    /*!< Status indication for Rfifo in manual mode                            */
      __I uint32_t FIFO_AFULL_RFIFO : 1;   /*!< Status indication for Rfifo in manual mode                            */
      __I uint32_t FIFO_EMPTY_RFIFO_S : 1; /*!< Status indication for Rfifo in manual mode                            */
      __I uint32_t
        FIFO_AEMPTY_RFIFO_S : 1;           /*!< Status indication for Rfifo in manual mode                            */
      __I uint32_t GSPI_MANUAL_RD_CNT : 1; /*!< This is a result of 10 bits ORing counter                             */
      __I uint32_t
        AUTO_MODE_FSM_IDLE_SCLK : 1;   /*!< Auto mode idle signal to track auto controller is busy or idle.       */
      __I uint32_t QSPI_AUTO_MODE : 1; /*!< QSPI controller status.                                               */
      __I uint32_t QSPI_AUTO_MODE_FRM_REG_SCLK : 1; /*!< QSPI auto mode status. Valid only when HW_CTRLD_QSPI_MODE_CTRL
                                                         is zero.                                                              */
      __I uint32_t HW_CTRLD_MODE_SCLK : 1; /*!< QSPI mode status in HW_CTRLD_MODE                                     */
      __I uint32_t
        HW_CTRLD_MODE_CTRL_SCLK : 1; /*!< HW_CTRLD_MODE status                                                  */
    } QSPI_MANUAL_STATUS_b;          /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t QSPI_INTR_MASK; /*!< QSPI Interrupt Mask Register                                          */

    struct {
      __IO uint32_t QSPI_INTR_MASK : 1; /*!< Interrupt Status bit                                                  */
      __IO uint32_t
        FIFO_AEMPTY_RFIFO_MASK : 1; /*!< NONE                                                                  */
      __IO uint32_t
        FIFO_AFULL_RFIFO_MASK : 1; /*!< NONE                                                                  */
      __IO uint32_t
        FIFO_AEMPTY_WFIFO_MASK : 1; /*!< NONE                                                                  */
      __IO uint32_t
        FIFO_AFULL_WFIFO_MASK : 1; /*!< NONE                                                                  */
      __IO uint32_t
        FIFO_FULL_WFIFO_MASK : 1; /*!< NONE                                                                  */
      __IO uint32_t
        FIFO_EMPTY_RFIFO_MASK : 1; /*!< NONE                                                                  */
    } QSPI_INTR_MASK_b;            /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t QSPI_INTR_UNMASK; /*!< QSPI Interrupt Unmask Register                                        */

    struct {
      __IO uint32_t QSPI_INTR_UNMASK : 1; /*!< Interrupt Status bit                                                  */
      __IO uint32_t
        FIFO_AEMPTY_RFIFO_UN : 1; /*!< NONE                                                                  */
      __IO uint32_t
        FIFO_AFULL_RFIFO_UNMASK : 1; /*!< NONE                                                                  */
      __IO uint32_t
        FIFO_AEMPTY_WFIFO_UNMASK : 1; /*!< NONE                                                                  */
      __IO uint32_t
        FIFO_AFULL_WFIFO_UNMASK : 1; /*!< NONE                                                                  */
      __IO uint32_t
        FIFO_FULL_WFIFO_UNMASK : 1; /*!< NONE                                                                  */
      __IO uint32_t
        FIFO_EMPTY_RFIFO_UNMASK : 1; /*!< NONE                                                                  */
    } QSPI_INTR_UNMASK_b;            /*!< BitSize                                                               */
  };

  union {
    __I uint32_t QSPI_INTR_STS; /*!< QSPI Interrupt Status Register                                        */

    struct {
      __I uint32_t QSPI_INTR_LVL : 1; /*!< Interrupt Status bit                                                  */
      __I uint32_t
        FIFO_AEMPTY_RFIFO_LVL : 1; /*!< NONE                                                                  */
      __I uint32_t
        FIFO_AFULL_RFIFO_LVL : 1; /*!< NONE                                                                  */
      __I uint32_t
        FIFO_AEMPTY_WFIFO_LVL : 1; /*!< NONE                                                                  */
      __I uint32_t
        FIFO_AFULL_WFIFO_LVL : 1; /*!< NONE                                                                  */
      __I uint32_t
        FIFO_FULL_WFIFO_LVL : 1; /*!< NONE                                                                  */
      __I uint32_t
        FIFO_EMPTY_RFIFO_LVL : 1; /*!< NONE                                                                  */
    } QSPI_INTR_STS_b;            /*!< BitSize                                                               */
  };

  union {
    __O uint32_t QSPI_INTR_ACK; /*!< QSPI Interrupt Acknowledge Register                                   */

    struct {
      __O uint32_t QSPI_INTR_ACK : 1; /*!< Interrupt Status bit                                                  */
      __O uint32_t
        FIFO_AEMPTY_RFIFO_ACK : 1; /*!< NONE                                                                  */
      __O uint32_t
        FIFO_AFULL_RFIFO_ACK : 1; /*!< NONE                                                                  */
      __O uint32_t
        FIFO_AEMPTY_WFIFO_ACK : 1; /*!< NONE                                                                  */
      __O uint32_t
        FIFO_AFULL_WFIFO_ACK : 1; /*!< NONE                                                                  */
      __O uint32_t
        FIFO_FULL_WFIFO_ACK : 1; /*!< NONE                                                                  */
      __O uint32_t
        FIFO_EMPTY_RFIFO_ACK : 1; /*!< NONE                                                                  */
    } QSPI_INTR_ACK_b;            /*!< BitSize                                                               */
  };

  union {
    __I uint32_t QSPI_STS_MC; /*!< QSPI State Machine Monitor Register                                   */

    struct {
      __I uint32_t BUS_CTRL_PSTATE : 4;    /*!< Bus controller present state                                          */
      __I uint32_t AUTO_CTRL_PSTATE : 3;   /*!< Auto controller present state                                         */
      __I uint32_t QSPI_MASTER_PSTATE : 3; /*!< Qspi master present state                                             */
    } QSPI_STS_MC_b;                       /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t QSPI_AUTO_CONFIG_1_CSN1; /*!< QSPI Auto Controller Configuration 1 CSN1 Register                    */

    struct {
      __IO uint32_t
        QSPI_EXT_BYTE_MODE_CSN1 : 2; /*!< Mode of operation of QSPI in instruction phase.                       */
      __IO uint32_t
        QSPI_DUMMY_MODE_CSN1 : 2; /*!< Mode of operation of QSPI in instruction phase                        */
      __IO uint32_t
        QSPI_ADDR_MODE_CSN1 : 2; /*!< Mode of operation of QSPI in instruction phase.                       */
      __IO uint32_t
        QSPI_CMD_MODE_CSN1 : 2; /*!< Mode of operation of QSPI in instruction phase.                       */
      __IO uint32_t
        QSPI_DATA_MODE_CSN1 : 2; /*!< Mode of operation of QSPI in DATA phase.                              */
      __I uint32_t QSPI_EXTRA_BYTE_CSN1 : 8; /*!< Value of the extra byte to be transmitted, if the extra byte
                                                         mode is enabled.                                                      */
      __IO uint32_t
        QSPI_EXTRA_BYTE_EN_CSN1 : 2; /*!< Mode of operation of QSPI in DATA phase.                              */
      uint32_t : 3;
      __O uint32_t QSPI_PG_JUMP_CSN1 : 1; /*!< NONE                                                                  */
      __I uint32_t QSPI_DUMMY_BYTES_INCR_CSN1 : 4; /*!< Specifies the number of dummy bytes 0 to 7 for the selected
                                                         SPI mode.                                                             */
      __I uint32_t QSPI_DUMMY_BYTES_WRAP_CSN1 : 4; /*!< Specifies the number of dummy bytes 0 to 7 for the selected
                                                         SPI mode in case of wrap instruction.                                 */
    } QSPI_AUTO_CONFIG_1_CSN1_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      QSPI_AUTO_CONFIG_2_CSN1_REG; /*!< QSPI Auto Controller Configuration 2 CSN1 Register                    */

    struct {
      __IO uint32_t QSPI_RD_SWAP_AUTO_CSN1 : 1; /*!< Swap the read data from the flash in byte order for chip select1
                                                         csn1 in auto mode.                                                    */
      __IO uint32_t
        QSPI_ADR_SIZE_16BIT_AUTO_MODE_CSN1 : 1; /*!< NONE                                                          */
      __IO uint32_t
        QSPI_CONTI_RD_EN_CSN1 : 1;         /*!< Continuous read enable bit.                                           */
      __IO uint32_t DUMMY_BYTES_WR_RD : 1; /*!< Dummy bytes to the model to be read or to be write.                   */
      __IO uint32_t
        QSPI_DUMMY_BYTES_JMP_CSN1 : 4;     /*!< Dummy cycles to be selected in case of JUMP                           */
      __IO uint32_t QSPI_RD_INST_CSN1 : 8; /*!< Read instruction to be used for the selected SPI modes and when
                                                         wrap is not needed or supported                                       */
      __IO uint32_t QSPI_RD_WRAP_INST_CSN1 : 8; /*!< Read instruction to be used for the selected SPI modes and when
                                                         wrap is not needed or supported                                       */
      __IO uint32_t
        QSPI_PG_JMP_INST_CSN1 : 8;   /*!< Read instruction to be used, when Page jump is to be used.            */
    } QSPI_AUTO_CONFIG_2_CSN1_REG_b; /*!< BitSize                                                               */
  };
  __IO uint32_t QSPI_MANUAL_RDWR_FIFO[16]; /*!< QSPI FIFOs                                                            */
  union {
    __IO uint32_t QSPI_MANUAL_WRITE_DATA2; /*!< QSPI Manual Write Data 2 Register                                     */

    struct {
      __IO uint32_t
        QSPI_MANUAL_WRITE_DATA2 : 5; /*!< Number of bits to be written in write mode                            */
      uint32_t : 2;
      __IO uint32_t USE_PREV_LENGTH : 1; /*!< Use previous length.                                                  */
    } QSPI_MANUAL_WRITE_DATA2_b;         /*!< BitSize                                                               */
  };
  union {
    __IO uint32_t QSPI_AUTO_CONFIG_1_CSN2; /*!< QSPI Auto Controller Configuration 1 CSN2 Register                    */

    struct {
      __IO uint32_t
        QSPI_EXT_BYTE_MODE_CSN2 : 2; /*!< Mode of operation of QSPI in instruction phase.                       */
      __IO uint32_t
        QSPI_DUMMY_MODE_CSN2 : 2; /*!< Mode of operation of QSPI in instruction phase                        */
      __IO uint32_t
        QSPI_ADDR_MODE_CSN2 : 2; /*!< Mode of operation of QSPI in instruction phase.                       */
      __IO uint32_t
        QSPI_CMD_MODE_CSN2 : 2; /*!< Mode of operation of QSPI in instruction phase.                       */
      __IO uint32_t
        QSPI_DATA_MODE_CSN2 : 2; /*!< Mode of operation of QSPI in DATA phase.                              */
      __I uint32_t QSPI_EXTRA_BYTE_CSN2 : 8; /*!< Value of the extra byte to be transmitted, if the extra byte
                                                         mode is enabled.                                                      */
      __IO uint32_t
        QSPI_EXTRA_BYTE_EN_CSN2 : 2; /*!< Mode of operation of QSPI in DATA phase.                              */
      uint32_t : 3;
      __O uint32_t QSPI_PG_JUMP_CSN2 : 1; /*!< NONE                                                                  */
      __I uint32_t QSPI_DUMMY_BYTES_INCR_CSN2 : 4; /*!< Specifies the number of dummy bytes 0 to 7 for the selected
                                                         SPI mode.                                                             */
      __I uint32_t QSPI_DUMMY_BYTES_WRAP_CSN2 : 4; /*!< Specifies the number of dummy bytes 0 to 7 for the selected
                                                         SPI mode in case of wrap instruction.                                 */
    } QSPI_AUTO_CONFIG_1_CSN2_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      QSPI_AUTO_CONFIG_2_CSN2_REG; /*!< QSPI Auto Controller Configuration 2 CSN2 Register                    */

    struct {
      __IO uint32_t QSPI_RD_SWAP_AUTO_CSN2 : 1; /*!< Swap the read data from the flash in byte order for chip select1
                                                         csn1 in auto mode.                                                    */
      __IO uint32_t
        QSPI_ADR_SIZE_16BIT_AUTO_MODE_CSN2 : 1; /*!< NONE                                                          */
      __IO uint32_t
        QSPI_CONTI_RD_EN_CSN2 : 1; /*!< Continuous read enable bit.                                           */
      __IO uint32_t
        DUMMY_BYTES_WR_RD_CSN2 : 1; /*!< Dummy bytes to the model to be read or to be write.                   */
      __IO uint32_t
        QSPI_DUMMY_BYTES_JMP_CSN2 : 4;     /*!< Dummy cycles to be selected in case of JUMP                           */
      __IO uint32_t QSPI_RD_INST_CSN2 : 8; /*!< Read instruction to be used for the selected SPI modes and when
                                                         wrap is not needed or supported                                       */
      __IO uint32_t QSPI_RD_WRAP_INST_CSN2 : 8; /*!< Read instruction to be used for the selected SPI modes and when
                                                         wrap is not needed or supported                                       */
      __IO uint32_t
        QSPI_PG_JMP_INST_CSN2 : 8;   /*!< Read instruction to be used, when Page jump is to be used.            */
    } QSPI_AUTO_CONFIG_2_CSN2_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t QSPI_AUTO_CONFIG_1_CSN3; /*!< QSPI Auto Controller Configuration 1 CSN3 Register                    */

    struct {
      __IO uint32_t
        QSPI_EXT_BYTE_MODE_CSN3 : 2; /*!< Mode of operation of QSPI in instruction phase.                       */
      __IO uint32_t
        QSPI_DUMMY_MODE_CSN3 : 2; /*!< Mode of operation of QSPI in instruction phase                        */
      __IO uint32_t
        QSPI_ADDR_MODE_CSN3 : 2; /*!< Mode of operation of QSPI in instruction phase.                       */
      __IO uint32_t
        QSPI_CMD_MODE_CSN3 : 2; /*!< Mode of operation of QSPI in instruction phase.                       */
      __IO uint32_t
        QSPI_DATA_MODE_CSN3 : 2; /*!< Mode of operation of QSPI in DATA phase.                              */
      __I uint32_t QSPI_EXTRA_BYTE_CSN3 : 8; /*!< Value of the extra byte to be transmitted, if the extra byte
                                                         mode is enabled.                                                      */
      __IO uint32_t
        QSPI_EXTRA_BYTE_EN_CSN3 : 2; /*!< Mode of operation of QSPI in DATA phase.                              */
      uint32_t : 3;
      __O uint32_t QSPI_PG_JUMP_CSN3 : 1; /*!< NONE                                                                  */
      __I uint32_t QSPI_DUMMY_BYTES_INCR_CSN2 : 4; /*!< Specifies the number of dummy bytes 0 to 7 for the selected
                                                         SPI mode.                                                             */
      __I uint32_t QSPI_DUMMY_BYTES_WRAP_CSN2 : 4; /*!< Specifies the number of dummy bytes 0 to 7 for the selected
                                                         SPI mode in case of wrap instruction.                                 */
    } QSPI_AUTO_CONFIG_1_CSN3_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      QSPI_AUTO_CONFIG_2_CSN3_REG; /*!< QSPI Auto Controller Configuration 2 CSN3 Register                    */

    struct {
      __IO uint32_t QSPI_RD_SWAP_AUTO_CSN3 : 1; /*!< Swap the read data from the flash in byte order for chip select1
                                                         csn1 in auto mode.                                                    */
      __IO uint32_t
        QSPI_ADR_SIZE_16BIT_AUTO_MODE_CSN3 : 1; /*!< NONE                                                          */
      __IO uint32_t
        QSPI_CONTI_RD_EN_CSN3 : 1; /*!< Continuous read enable bit.                                           */
      __IO uint32_t
        DUMMY_BYTES_WR_RD_CSN3 : 1; /*!< Dummy bytes to the model to be read or to be write.                   */
      __IO uint32_t
        QSPI_DUMMY_BYTES_JMP_CSN3 : 4;     /*!< Dummy cycles to be selected in case of JUMP                           */
      __IO uint32_t QSPI_RD_INST_CSN3 : 8; /*!< Read instruction to be used for the selected SPI modes and when
                                                         wrap is not needed or supported                                       */
      __IO uint32_t QSPI_RD_WRAP_INST_CSN3 : 8; /*!< Read instruction to be used for the selected SPI modes and when
                                                         wrap is not needed or supported                                       */
      __IO uint32_t
        QSPI_PG_JMP_INST_CSN3 : 8;   /*!< Read instruction to be used, when Page jump is to be used.            */
    } QSPI_AUTO_CONFIG_2_CSN3_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      QSPI_AUTO_CONFIG_3_CSN0_REG; /*!< QSPI Auto Controller Configuration 3 CSN0 Register                    */

    struct {
      uint32_t : 12;
      __IO uint32_t
        QSPI_DDR_CMD_MODE_CSN0 : 1; /*!< DDR Command mode                                                      */
      __IO uint32_t
        QSPI_DDR_ADDR_MODE_CSN0 : 1; /*!< DDR Address mode                                                      */
      __IO uint32_t
        QSPI_DDR_DUMMY_MODE_CSN0 : 1; /*!< DDR Address mode                                                      */
      __IO uint32_t
        QSPI_DDR_EXTRA_MODE_CSN0 : 1; /*!< DDR Address mode                                                      */
      __IO uint32_t
        QSPI_DDR_DATA_MODE_CSN0 : 1; /*!< DDR Address mode                                                      */
    } QSPI_AUTO_CONFIG_3_CSN0_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      QSPI_AUTO_CONFIG_3_CSN1_REG; /*!< QSPI Auto Controller Configuration 3 CSN1 Register                    */

    struct {
      uint32_t : 12;
      __IO uint32_t
        QSPI_DDR_CMD_MODE_CSN1 : 1; /*!< DDR Command mode                                                      */
      __IO uint32_t
        QSPI_DDR_ADDR_MODE_CSN1 : 1; /*!< DDR Address mode                                                      */
      __IO uint32_t
        QSPI_DDR_DUMMY_MODE_CSN1 : 1; /*!< DDR Address mode                                                      */
      __IO uint32_t
        QSPI_DDR_EXTRA_MODE_CSN1 : 1; /*!< DDR Address mode                                                      */
      __IO uint32_t
        QSPI_DDR_DATA_MODE_CSN1 : 1; /*!< DDR Address mode                                                      */
    } QSPI_AUTO_CONFIG_3_CSN1_REG_b; /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED[11];

  union {
    __IO uint32_t
      OCTASPI_BUS_CONTROLLER_2_REG; /*!< none                                                                  */

    struct {
      __IO uint32_t SET_IP_MODE : 1;    /*!< This bit enables the qspi interface pins into HiZ mode                */
      __IO uint32_t AES_NONCE_INIT : 1; /*!< This bit enables the AES initialization with nonce                    */
      __IO uint32_t AES_SEC_ENABLE : 1; /*!< This bit enables the AES security enable or not                       */
      __IO uint32_t DUAL_MODE_EN : 1;   /*!< Dual flash mode enable control.                                       */
      __IO uint32_t CSN0_2_CSN : 2;     /*!< Map csn0 to the programmed csn. It is valid for both manual
                                                         and auto modes                                                        */
      __IO uint32_t CSN1_2_CSN : 2;     /*!< Map csn1 to the programmed csn. It is valid for both manual
                                                         and auto modes                                                        */
      __IO uint32_t CSN2_2_CSN : 2;     /*!< Map csn2 to the programmed csn. It is valid for both manual
                                                         and auto modes                                                        */
      __IO uint32_t CSN3_2_CSN : 2;     /*!< Map csn3 to the programmed csn. It is valid for both manual
                                                         and auto modes                                                        */
      __IO uint32_t AES_SEC_ENABLE_SG1 : 1; /*!< This bit enables the AES security enable or not for segment
                                                         1                                                                     */
      __IO uint32_t AES_SEC_ENABLE_SG2 : 1; /*!< This bit enables the AES security enable or not for segment
                                                         2                                                                     */
      __IO uint32_t AES_SEC_ENABLE_SG3 : 1; /*!< This bit enables the AES security enable or not for segment
                                                         3                                                                     */
      __IO uint32_t AES_SEC_ENABLE_SG4 : 1; /*!< This bit enables the AES security enable or not for segment
                                                         4                                                                     */
      __IO uint32_t
        DUAL_MODE_SWAP_LINES : 1; /*!< This bit controls the 8 lines of qspi with 4 bit swap manner          */
      uint32_t : 2;
      __IO uint32_t
        DUAL_STAGE_EN_MANUAL : 1;     /*!< Dual stage en for dual flash mode                                     */
    } OCTASPI_BUS_CONTROLLER_2_REG_b; /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED1[15];

  union {
    __IO uint32_t
      QSPI_SRAM_CTRL_CSN_REG[4]; /*!< none                                                                  */

    struct {
      __IO uint32_t BIT_8_MODE : 1;      /*!< Flash 8bit (1 byte) boundary mode                                     */
      __IO uint32_t BYTE_32_MODE : 1;    /*!< Flash 32 byte boundary mode                                           */
      __IO uint32_t ADDR_16BIT_MODE : 1; /*!< Send only lower 16bits of Address enable.                             */
      uint32_t : 5;
      __IO uint32_t CMD_MODE : 2;  /*!< writing cmd mode                                                      */
      __IO uint32_t ADDR_MODE : 2; /*!< writing address mode                                                  */
      __IO uint32_t DATA_MODE : 2; /*!< writing address mode                                                  */
      uint32_t : 2;
      __IO uint32_t WR_CMD : 8;    /*!< Command to be used for writing                                        */
    } QSPI_SRAM_CTRL_CSN_REG_b[4]; /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED2[2];
  __IO uint32_t SEMI_AUTO_MODE_ADDR_REG; /*!< Byte address to read the data from flash in semi auto mode.
                                                         It is valid only semi auto mode enable bit is asserted                */

  union {
    __IO uint32_t
      SEMI_AUTO_MODE_CONFIG_REG; /*!< none                                                                  */

    struct {
      __IO uint32_t
        QSPI_SEMI_AUTO_BSIZE : 8; /*!< This is burst size to read data from flash in semi auto mode          */
      __IO uint32_t
        QSPI_SEMI_AUTO_HSIZE : 2;  /*!< Indicates number of bytes valid in each transaction                   */
    } SEMI_AUTO_MODE_CONFIG_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      SEMI_AUTO_MODE_CONFIG2_REG; /*!< none                                                                  */

    struct {
      __IO uint32_t QSPI_SEMI_AUTO_RD_CNT : 12; /*!< Total number of bytes to be read flash continuously from the
                                                         address given by SEMI_AUTO_MODE_ADDR_REG                              */
      __IO uint32_t QSPI_SEMI_AUTO_MODE_EN : 1; /*!< Enable for semi auto mode read operation. Make sure manual mode
                                                         read/write operation is completed before asserting this bit           */
      __IO uint32_t QSPI_SEMI_AUTO_RD_BUSY : 1; /*!< Indicates status of semi auto mode read status. If it is high,
                                                         semi auto mode read operation is progressing                          */
    } SEMI_AUTO_MODE_CONFIG2_REG_b; /*!< BitSize                                                               */
  };
} QSPI_Type;

/* ================================================================================ */
/* ================                       CRC                      ================ */
/* ================================================================================ */

/**
 * @brief CRC is used in all wireless communication as a first data integrity check (CRC)
 */

typedef struct { /*!< CRC Structure                                                         */

  union {
    __IO uint32_t GEN_CTRL_SET_REG; /*!< General control set register                                          */

    struct {
      __IO uint32_t SOFT_RST : 16; /*!< Soft reset. This clears the FIFO and settles all the state machines
                                                         to their IDLE                                                         */
    } GEN_CTRL_SET_REG_b;          /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED;

  union {
    __I uint32_t GEN_STS_REG; /*!< General status register                                               */

    struct {
      __I uint32_t CALC_DONE : 1;          /*!< When the computation of final CRC with the data out of fifo,
                                                         this will get set to 1 otherwise 0                                    */
      __I uint32_t DIN_NUM_BYTES_DONE : 1; /*!< When number of bytes requested for computation of final CRC
                                                         is read from fifo by internal FSM, this will get set to 1 otherwise
                                                          0.                                                                   */
    } GEN_STS_REG_b;                       /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t POLYNOMIAL_REG; /*!< General status register                                               */

    struct {
      __IO uint32_t POLYNOMIAL : 32; /*!< Polynomial register. This register holds the polynomial with
                                                         which the final CRC is computed.When writePolynomial will be
                                                          updated.When read read polynomial.                                   */
    } POLYNOMIAL_REG_b;              /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t POLYNOMIAL_CTRL_SET_REG; /*!< Polynomial control set register                                       */

    struct {
      __IO uint32_t POLYNOMIAL_WIDTH_SET : 5; /*!< Polynomial width set. Number of bits/width of the polynomial
                                                         has to be written here for the computation of final CRC. If
                                                          a new width has to be configured, clear the existing length
                                                          first by writing 0x1f in polynomial_ctrl_reset register.When
                                                          read, actual polynomial width is read.                               */
    } POLYNOMIAL_CTRL_SET_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      POLYNOMIAL_CTRL_RESET_REG; /*!< Polynomial control set register                                       */

    struct {
      __IO uint32_t POLYNOMIAL_WIDTH_RESET : 5; /*!< Polynomial width reset. If a new width has to be configured,
                                                         clear the existing length first by writing 0x1f. When read,
                                                          actual polynomial width is read.                                     */
    } POLYNOMIAL_CTRL_RESET_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t LFSR_INIT_REG; /*!< Polynomial control set register                                       */

    struct {
      __IO uint32_t LFSR_INIT : 32; /*!< This holds LFSR initialization value. When ever LFSR needs to
                                                         be initialized, this has to be updated with the init value and
                                                          trigger init_lfsr in LFSR_INIT_CTRL_SET register.For example,
                                                          in WiFi case, 0xffffffff is used as init value of LFSR.              */
    } LFSR_INIT_REG_b;              /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t LFSR_INIT_CTRL_SET_REG; /*!< LFSR state initialization control set register                        */

    struct {
      __IO uint32_t CLEAR_LFSR : 1; /*!< Clear LFSR state. When this is set, LFSR state is cleared to
                                                         0                                                                     */
      __IO uint32_t INIT_LFSR : 1;  /*!< Initialize LFSR state. When this is set LFSR state will be initialized
                                                         with LFSR_INIT_VAL/bit swapped LFSR_INIT_VAL in the next cycle        */
      __IO uint32_t USE_SWAPPED_INIT_VAL : 1; /*!< Use bit swapped init value. If this is set bit swapped version
                                                         of LFSR init value will be loaded / initialized to LFSR state         */
    } LFSR_INIT_CTRL_SET_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      LFSR_INIT_CTRL_RESET_REG; /*!< LFSR state initialization control reset register                      */

    struct {
      uint32_t : 2;
      __IO uint32_t USE_SWAPPED_INIT_VAL : 1; /*!< Use bit swapped init value. If this is set bit swapped version
                                                         of LFSR init value will be loaded / initialized to LFSR state         */
    } LFSR_INIT_CTRL_RESET_REG_b; /*!< BitSize                                                               */
  };

  union {
    __O uint32_t DIN_FIFO_REG; /*!< Data input FIFO register                                              */

    struct {
      __O uint32_t DIN_FIFO : 32; /*!< FIFO input port is mapped to this register. Data on which the
                                                         final CRC has to be computed has to be loaded to this FIFO            */
    } DIN_FIFO_REG_b;             /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t DIN_CTRL_SET_REG; /*!< Input data control set register                                       */

    struct {
      __IO uint32_t DIN_WIDTH : 5;          /*!< Valid number of bits in the input data in din_width_from_reg
                                                         set mode. Before writing a new value into this, din_ctrl_reset_reg
                                                          has to be written with 0x1f to clear this field as these are
                                                          set/clear bits.                                                      */
      __IO uint32_t DIN_WIDTH_FROM_REG : 1; /*!< Valid number of bits in the input data. In default, number of
                                                         valid bits in the input data is taken from ULI (uli_be). If
                                                          this is set,whatever is the input size, only din_ctrl_reg[4:0]
                                                          is taken as valid length/width for inout data.                       */
      __IO uint32_t DIN_WIDTH_FROM_CNT : 1; /*!< Valid number of bits in the input data. In default, number of
                                                         valid bits in the input data is taken from ULI (uli_be). If
                                                          this is set, a mix of ULI length and number of bytes remaining
                                                          will form the valid bits (which ever is less that will be considered
                                                          as valid bits).                                                      */
      __IO uint32_t USE_SWAPPED_DIN : 1;    /*!< Use bit swapped input data. If this is set, input data will
                                                         be swapped and filled in to FIFO.Whatever read out from FIFO
                                                          will be directly fed to LFSR engine.                                 */
      __IO uint32_t RESET_FIFO_PTRS : 1;    /*!< Reset fifo pointer. This clears the FIFO.When this is set, FIFO
                                                         will be cleared.                                                      */
      uint32_t : 15;
      __IO uint32_t FIFO_AEMPTY_THRESHOLD : 4; /*!< FIFO almost empty threshold value. This has to be cleared by
                                                         writing 0x0f000000 into din_ctrl_reset before updating any new
                                                          value.                                                               */
      __IO uint32_t FIFO_AFULL_THRESHOULD : 4; /*!< FIFO almost full threshold value. This has to be cleared by
                                                         writing 0xf0000000 into din_ctrl_reset before updating any new
                                                          value                                                                */
    } DIN_CTRL_SET_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t DIN_CTRL_RESET_REG; /*!< Input data control set register                                       */

    struct {
      __IO uint32_t DIN_WIDTH : 5;          /*!< Valid number of bits in the input data in din_width_from_reg
                                                         set mode. Before writing a new value into this, din_ctrl_reset_reg
                                                          has to be written with 0x1f to clear this field as these are
                                                          set/clear bits.                                                      */
      __IO uint32_t DIN_WIDTH_FROM_REG : 1; /*!< Valid number of bits in the input data. In default, number of
                                                         valid bits in the input data is taken from ULI (uli_be). If
                                                          this is set,whatever is the input size, only din_ctrl_reg[4:0]
                                                          is taken as valid length/width for inout data.                       */
      __IO uint32_t DIN_WIDTH_FROM_CNT : 1; /*!< Valid number of bits in the input data. In default, number of
                                                         valid bits in the input data is taken from ULI (uli_be). If
                                                          this is set, a mix of ULI length and number of bytes remaining
                                                          will form the valid bits (which ever is less that will be considered
                                                          as valid bits).                                                      */
      __IO uint32_t USE_SWAPPED_DIN : 1;    /*!< Use bit swapped input data. If this is set input data will be
                                                         swapped and filled in to FIFO.Whatever read out from FIFO will
                                                          be directly fed to LFSR engine.                                      */
      __IO uint32_t RESET_FIFO_PTRS : 1;    /*!< Reset fifo pointer. This clears the FIFO.When this is set, FIFO
                                                         will be cleared.                                                      */
      uint32_t : 15;
      __IO uint32_t FIFO_AEMPTY_THRESHOLD : 4; /*!< FIFO almost empty threshold value. This has to be cleared by
                                                         writing 0x0f000000 into din_ctrl_reset before updating any new
                                                          value.                                                               */
      __IO uint32_t FIFO_AFULL_THRESHOULD : 4; /*!< FIFO almost full threshold value. This has to be cleared by
                                                         writing 0xf0000000 into din_ctrl_reset before updating any new
                                                          value                                                                */
    } DIN_CTRL_RESET_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t DIN_NUM_BYTES_REG; /*!< Data input FIFO register                                              */

    struct {
      __IO uint32_t DIN_NUM_BYTES : 32; /*!< in out data number of bytes                                           */
    } DIN_NUM_BYTES_REG_b;              /*!< BitSize                                                               */
  };

  union {
    __I uint32_t DIN_STS_REG; /*!< Input data status register                                            */

    struct {
      __I uint32_t FIFO_EMPTY : 1;  /*!< FIFO empty indication status                                          */
      __I uint32_t FIFO_AEMPTY : 1; /*!< FIFO almost empty indication status.                                  */
      __I uint32_t FIFO_AFULL : 1;  /*!< FIFO almost full indication status                                    */
      __I uint32_t FIFO_FULL : 1;   /*!< FIFO full indication status                                           */
      __I uint32_t FIFO_OCC : 6;    /*!< FIFO occupancy                                                        */
    } DIN_STS_REG_b;                /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t LFSR_STATE_REG; /*!< LFSR state register                                                   */

    struct {
      __IO uint32_t LFSR_STATE : 32; /*!< If LFSR dynamic loading is required this can be used for writing
                                                         the LFSR state directly.                                              */
    } LFSR_STATE_REG_b;              /*!< BitSize                                                               */
  };
} CRC_Type;

/* ================================================================================ */
/* ================                      EFUSE                     ================ */
/* ================================================================================ */

/**
 * @brief The EFUSE controller is used to provide an interface to one time program memory (EFUSE macro) to perform write and read operations (EFUSE)
 */

typedef struct { /*!< EFUSE Structure                                                       */

  union {
    __IO uint32_t EFUSE_DA_ADDR_REG; /*!< Direct Access Registers                                               */

    struct {
      __IO uint32_t ADDR_BITS : 16; /*!< These bits specifies the address to write or read from EFUSE
                                                         macro model                                                           */
    } EFUSE_DA_ADDR_REG_b;          /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t EFUSE_DA_CTRL_SET_REG; /*!< Direct Access Set Registers                                           */

    struct {
      __IO uint32_t PGENB : 1;  /*!< Set Program enable                                                    */
      __IO uint32_t CSB : 1;    /*!< Set Chip Enable                                                       */
      __IO uint32_t STROBE : 1; /*!< Set strobe enable                                                     */
      __IO uint32_t LOAD : 1;   /*!< Set Load enable                                                       */
    } EFUSE_DA_CTRL_SET_REG_b;  /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t EFUSE_DA_CTRL_CLEAR_REG; /*!< Direct Access Clear Registers                                         */

    struct {
      __IO uint32_t PGENB : 1;   /*!< Set Program enable                                                    */
      __IO uint32_t CSB : 1;     /*!< Set Chip Enable                                                       */
      __IO uint32_t STROBE : 1;  /*!< Set strobe enable                                                     */
      __IO uint32_t LOAD : 1;    /*!< Set Load enable                                                       */
    } EFUSE_DA_CTRL_CLEAR_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t EFUSE_CTRL_REG; /*!< Control Register                                                      */

    struct {
      __IO uint32_t EFUSE_EN : 1;             /*!< This bit specifies whether the EFUSE module is enabled or not
			 */
      __IO uint32_t EFUSE_DIRECT_PATH_EN : 1; /*!< This bit specifies whether the EFUSE direct path is enabled
                                                         or not for direct accessing of the EFUSE pins                         */
    } EFUSE_CTRL_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t EFUSE_READ_ADDR_REG; /*!< Read address Register                                                 */

    struct {
      __IO uint32_t READ_ADDR : 12; /*!< These bits specifies the address from which read operation has
                                                         to be performed                                                       */
      uint32_t : 3;
      __O uint32_t DO_READ : 1; /*!< Enables read FSM after EFUSE is enabled                               */
    } EFUSE_READ_ADDR_REG_b;    /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t EFUSE_READ_DATA_REG; /*!< Read address Register                                                 */

    struct {
      __IO uint32_t READ_DATA : 8; /*!< These bits specifies the data bits that are read from a given
                                                         address specified in the EFUSE_READ_ADDRESS_REGISTER bits 8:0
			 */
      uint32_t : 7;
      __I uint32_t READ_FSM : 1; /*!< Indicates read fsm is done. After this read data is available
                                                         in EFUSE_READ_DATA_REGISTER bits 7:0                                  */
    } EFUSE_READ_DATA_REG_b;     /*!< BitSize                                                               */
  };

  union {
    __I uint32_t EFUSE_STATUS_REG; /*!< Read address Register                                                 */

    struct {
      __IO uint32_t EFUSE_ENABLED : 1; /*!< This bit specifies whether the EFUSE is enabled or not                */
      uint32_t : 1;
      __I uint32_t EFUSE_DOUT_SYNC : 8; /*!< This bit specifies the 8-bit data read out from the EFUSE macro.
                                                         This is synchronized with pclk                                        */
    } EFUSE_STATUS_REG_b;               /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t EFUSE_RD_TMNG_PARAM_REG; /*!< none                                                                  */

    struct {
      __IO uint32_t tSUR_CS : 4; /*!< CSB to STROBE setup time into read mode                               */
      __IO uint32_t tSQ : 4;     /*!< Q7-Q0 access time from STROBE rising edge                             */
      __IO uint32_t tHRA : 4;    /*!< for 32x8 macro: A4 A0 to STROBE hold time into Read mode5122x8
                                                         macro: A8 A0 to STROBE hold time into Read mode                       */
    } EFUSE_RD_TMNG_PARAM_REG_b; /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED;

  union {
    __IO uint32_t
      EFUSE_MEM_MAP_LENGTH_REG; /*!< none                                                                  */

    struct {
      __IO uint32_t EFUSE_MEM_MAP_LEN : 1; /*!< 0: 8 bit read 1: 16 bit read                                          */
    } EFUSE_MEM_MAP_LENGTH_REG_b;          /*!< BitSize                                                               */
  };
  __IO uint32_t
    EFUSE_READ_BLOCK_STARTING_LOCATION_REG;    /*!< Starting address from which the read has to be blocked. Once
                                                         the end address is written,it cannot be changed till power on
                                                          reset is given                                                       */
  __IO uint32_t EFUSE_READ_BLOCK_END_LOCATION; /*!< End address till which the read has to be blocked. Once the
                                                         end address is written , it cannot be changed till power on
                                                          reset is given                                                       */

  union {
    __IO uint32_t EFUSE_READ_BLOCK_ENABLE_REG; /*!< The Transmit Poll Demand register enables the Transmit DMA to
                                                         check whether or not the currentdescriptor is owned by DMA            */

    struct {
      __IO uint32_t efuse_read_block_enable : 1; /*!< Enable for blocking the read access from a programmable memory
                                                         location                                                              */
    } EFUSE_READ_BLOCK_ENABLE_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t EFUSE_DA_CLR_STROBE_REG; /*!< none                                                                  */

    struct {
      __IO uint32_t
        STROBE_CLEAR_COUNT_VALUE : 9; /*!< none                                                                  */
      __IO uint32_t STROBE_EN : 1;    /*!< none                                                                  */
    } EFUSE_DA_CLR_STROBE_REG_b;      /*!< BitSize                                                               */
  };
} EFUSE_Type;

/* ================================================================================ */
/* ================                       CCI                      ================ */
/* ================================================================================ */

/**
 * @brief CCI module helps external memories and peripherals to communicate with internal AHB bus with less number of pins (CCI)
 */

typedef struct { /*!< CCI Structure                                                         */
  /*0x00*/
  union {
    __IO uint32_t CCI_CONTROL_REG; /*!< Direct Access Registers                                               */

    struct {
      __IO uint32_t RESERVED : 2;     /*!<RESERVED                                                              */
      __IO uint32_t SLAVE_ENABLE : 3; /*!< Number of slaves                                                      */
      __IO uint32_t
        EARLY_BURST_TERMINATE_SUPRT : 1;   /*!< Support for Early Burst Termination                                  */
      __IO uint32_t ADDR_WIDTH_CONFIG : 2; /*!< address width configuration of slave                                  */
      __IO uint32_t TRANSLATE_ENABLE : 1;  /*!< translation enable                                                    */
      __IO uint32_t INTERFACE_WIDTH : 2;
      __IO uint32_t MODE : 1;           /*!< This bit represents mode of the interface                             */
      __IO uint32_t PRG_CALIB : 1;      /*!< By setting this bit, calibration turn around delay value will
                                                            be configured                                                         */
      __IO uint32_t SLAVE_PRIORITY : 3; /*!< This bits will represents priority of the slaves                      */
      __IO uint32_t TIME_OUT_PRG : 10;  /*!< configurable time out value for slave response.                       */
      __IO uint32_t RESERVED1 : 5;
      __IO uint32_t CCI_CNTRL_ENABLE : 1; /*!< CCI cntrl enable for gpio pads initialisation*/
    } CCI_CONTROL_REG_b;                  /*!< BitSize                                                               */
  };
  /*0x04*/
  __IO uint32_t CCI_LSB_A_S1_REG; /*!< Lower Address of slave 0 supported Make sure that slave0 is enabled   */
  /*0x08*/
  __IO uint32_t CCI_LSB_A_S2_REG; /*!< Lower Address of slave 0 supported Make sure that slave1 is enabled */
  /*0x0C*/
  __IO uint32_t CCI_LSB_A_S3_REG; /*!< Lower Address of slave 0 supported Make sure that slave2 is enabled*/
  /*0x10*/
  __IO uint32_t CCI_MSB_A_S1_REG; /*!< upper Address of slave 0 supported Make sure that slave0 is enabled */
  /*0x14*/
  __IO uint32_t CCI_MSB_A_S2_REG; /*!< upper Address of slave 0 supported Make sure that slave1 is enabled */
  /*0x18*/
  __IO uint32_t CCI_MSB_A_S3_REG; /*!< upper Address of slave 0 supported Make sure that slave2 is enabled */

  /*0x1C*/
  __IO uint32_t CCI_LP_CONTROL1_REG; /*!< low power control - 1Configuration.It holds the lower 32 bits
                                                         low power control values of the peer chip, once low power mode
                                                          triggered then this bits will be transferred on to the interface     */
  /*0x20*/
  __IO uint32_t CCI_LP_CONTROL2_REG; /*!< low power control - 2 Configuration.It holds 64 to 33 bits the
                                                         low power control values of the peer chip, once low power mode
                                                          triggered then this bits will be transferred on to the interface     */
  /*0x24*/
  union {
    __IO uint32_t CCI_LP_CONTROL3_REG;
    struct {
      __IO uint32_t IP_CONTROL_3 : 16; /*!< low power control - 3 Configuration.It holds 80 to 64 bits the */
      __IO uint32_t RESERVED : 16;     /*low power control values of the peer chip, once low power mode  */
    } CCI_LP_CONTROL3_REG_b;           /*triggered then this bits will be transferred on to the interface     */
  };
  /*0x28*/
  union {
    __IO uint32_t
      CCI_MODE_INTR_STATUS_REG; /*!< Interrupt Status                                                      */

    struct {
      __O uint32_t CALIB_MODE : 1;     /*!< This bit is used to trigger the calibration mode                      */
      __O uint32_t LOW_POWER_MODE : 1; /*!< This bit is used to trigger the low power mode                        */
      __O uint32_t INTERRUPT_MODE : 1; /*!< This bit is used to trigger the interrupt message mode                */
      __IO uint32_t INTR_CLEAR : 4;    /*!< By setting this bits will clear the interrupt status                  */
      __IO uint32_t SLAVE_RESET_MODE : 1;
      __IO uint32_t CSN_SLT_FOR_CALIB_INTR_MODE : 3;
      __I uint32_t INTR_STATUS : 4; /*!< This bits will represents the status of the interrupt                 */
      __IO uint32_t RESERVED : 17;
    } CCI_MODE_INTR_STATUS_REG_b; /*!< BitSize                                                               */
  };
  /*0x2C*/
  __IO uint32_t RESERVED;
  /*0x30*/
  __IO uint32_t RESERVED1;
  /*0x34*/
  union {
    __IO uint32_t CCI_CALIB_DELAY_CFG_REG; /*!< Calibration delay Configuration                                       */

    struct {
      __IO uint32_t CALIB_DELAY_CFG : 21; /*!< This register holds the turn around delays for the three ms
                                                        chips                                                                 */
      __IO uint32_t RESERVED : 11;        /*!< reserver */
    } CCI_CALIB_DELAY_CFG_REG_b;          /*!< BitSize                                                               */
  };
  /*0x38*/
  union {
    __IO uint32_t CCI_CALIB_BUF_DELAY_REG; /*!< Calibration Maximum delay                                             */

    struct {
      __IO uint32_t CALIB_BUF_DELAY : 21; /*!< This register holds the maximum tuning delays for the three
                                                         ms chips                                                              */
      __I uint32_t CALIBRATION_FAIL : 3;  /*!< this bits will represents the status of the calibration               */
      __IO uint32_t RESERVED : 8;
    } CCI_CALIB_BUF_DLELAY_REG_b; /*!< BitSize                                                               */
  };
  /*0x3C*/
  __IO uint32_t CCI_LP_CONTROL4_REG;
  /*0x40*/
  __IO uint32_t CCI_LP_CONTROL5_REG;

  __IO uint32_t RESERVED2[111];
  /*0x200*/
  union {
    __IO uint32_t CCI_FIFO_THRESHOLD_REG; /*!< upper Address of slave 0 supported                                    */
    struct {
      __IO uint32_t
        FIFO_AFULL_THRESHOLD : 5; /*!< Alomost Full Threshold                                                */
      __IO uint32_t
        FIFO_AEMPTY_THRESHOLD : 5; /*!< Almost Empty Threshold                                                */
      __IO uint32_t RESERVED : 22;
    } CCI_FIFO_THRESHOLD_REG_b; /*!< BitSize                                                               */
  };

  /*0x204*/
  __IO uint32_t TRANSLATION_ADDRESS_REG; /*!< Address offset for translation address                                */

  /*0x208*/
  union {
    __IO uint32_t
      CCI_PREFETCH_CONTROL_REG; /*!< Interrupt Status                                                      */

    struct {
      __IO uint32_t
        CCI_PREFECT_ENABLE : 1; /*!<cci pre-fetch enable:1: prefetch is enable on AHB read operation. In this mode,
			                                                                next AHB read transaction is perfected and kept in asych FIFO,
                                                                  		which is readily available for next consecutive AHB read address.
                                                                      0 : prefetech operation is disable                    */
      __IO uint32_t
        CCI_2X_CLK_ENBALE_FOR_DDR_MODE : 1; /*!< This is an enable for CCI 2x clock in DDR mode.                       */
      __O uint32_t INTERRUPT_MODE : 1;
      __IO uint32_t RESERVED : 29;

    } CCI_PREFETCH_CONTROL_REG_b; /*!< BitSize                                                               */
  };
} CCI_Type;

/* ================================================================================ */
/* ================                      I2S0                      ================ */
/* ================================================================================ */

/**
 * @brief I2S(Inter-IC Sound) is transferring two-channel digital audio data from one IC device to another (I2S0)
 */

typedef struct {
  union {
    union {
      __O uint32_t LTHR; /*!< Left Receive Buffer Register                                          */

      struct {
        __O uint32_t LTHR : 24; /*!< The Left Stereo Data to be transmitted serially from the Transmitted
                                                         channel output                                                        */
      } LTHR_b;                 /*!< BitSize                                                               */
    };

    union {
      __I uint32_t LRBR; /*!< Left Receive Buffer Register                                          */

      struct {
        __I uint32_t LRBR : 24; /*!< Data received serially from the received channel input                */
      } LRBR_b;                 /*!< BitSize                                                               */
    };
  };

  union {
    union {
      __O uint32_t RTHR; /*!< Right Transmit Holding Register                                       */

      struct {
        __O uint32_t RTHR : 24; /*!< The Right Stereo Data to be transmitted serially from the Transmit
                                                         channel output written through this register                          */
      } RTHR_b;                 /*!< BitSize                                                               */
    };

    union {
      __I uint32_t RRBR; /*!< Right Receive Buffer Register                                         */

      struct {
        __I uint32_t RRBR : 24; /*!< The Right Stereo Data received serially from the received channel
                                                         input through this register                                           */
      } RRBR_b;                 /*!< BitSize                                                               */
    };
  };

  union {
    __IO uint32_t RER; /*!< Receive Enable Register                                               */

    struct {
      __IO uint32_t RXCHEN : 1; /*!< This Bit enables/disables a receive channel independently of
                                                         all other channels                                                    */
    } RER_b;                    /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t TER; /*!< Transmit Enable Register                                              */

    struct {
      __IO uint32_t TXCHEN : 1; /*!< This Bit enables/disables a transmit channel independently of
                                                         all other channels                                                    */
    } TER_b;                    /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t RCR; /*!< Receive Configuration Register                                        */

    struct {
      __IO uint32_t WLEN : 3; /*!< This Bits are used to program the desired data resolution of
                                                         the receiver and enables LSB of the incoming left or right word
			 */
    } RCR_b;                  /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t TCR; /*!< Transmit Configuration Register                                       */

    struct {
      __IO uint32_t WLEN : 3; /*!< This Bits are used to program the desired data resolution of
                                                         the transmitter and ensure that MSB of the data is transmitted
                                                          first.                                                               */
    } TCR_b;                  /*!< BitSize                                                               */
  };

  union {
    __I uint32_t ISR; /*!< Interrupt Status Register                                             */

    struct {
      __I uint32_t RXDA : 1; /*!< Receive Data Available                                                */
      __I uint32_t RXFO : 1; /*!< Receive Data FIFO                                                     */
      uint32_t : 2;
      __I uint32_t TXFE : 1; /*!< Transmit FIFO Empty                                                   */
      __I uint32_t TXFO : 1; /*!< Transmit FIFO                                                         */
    } ISR_b;                 /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t IMR; /*!< Interrupt Mask Register                                               */

    struct {
      __IO uint32_t RXDAM : 1; /*!< RX Data Available Mask Interrupt                                      */
      __IO uint32_t RXFOM : 1; /*!< RX FIFO Overrun Mask Interrupt                                        */
      uint32_t : 2;
      __IO uint32_t TXFEM : 1; /*!< TX FIFO Empty Interrupt                                               */
      __IO uint32_t TXFOM : 1; /*!< TX FIFO Overrun Interrupt                                             */
    } IMR_b;                   /*!< BitSize                                                               */
  };

  union {
    __I uint32_t ROR; /*!< Receive Overrun Register                                              */

    struct {
      __I uint32_t RXCHO : 1; /*!< Read this bit to clear the RX FIFO data overrun interrupt             */
    } ROR_b;                  /*!< BitSize                                                               */
  };

  union {
    __I uint32_t TOR; /*!< Transmit Overrun Register                                             */

    struct {
      __I uint32_t TXCHO : 1; /*!< Read this bit to clear the TX FIFO data overrun interrupt             */
    } TOR_b;                  /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t RFCR; /*!< Receive FIFO Configuration Register0                                  */

    struct {
      __IO uint32_t RXCHDT : 4; /*!< This bits program the trigger level in the RX FIFO at which
                                                         the data available interrupt is generated                             */
    } RFCR_b;                   /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t TFCR; /*!< Transmit FIFO Configuration Register                                  */

    struct {
      __IO uint32_t TXCHET : 4; /*!< This bits program the trigger level in the TX FIFO at which
                                                         the Empty Threshold Reached interrupt is generated                    */
    } TFCR_b;                   /*!< BitSize                                                               */
  };

  union {
    __O uint32_t RFF; /*!< Receive FIFO Flush                                                    */

    struct {
      __O uint32_t RXCHFR : 1; /*!< Writing a 1 to this register flushes an individual RX FIFO RX
                                                         channel or block must be disable prior to writing to this bit         */
    } RFF_b;                   /*!< BitSize                                                               */
  };

  union {
    __O uint32_t TFF; /*!< Transmit FIFO Flush                                                   */

    struct {
      __O uint32_t TXCHFR : 1; /*!< Writing a 1 to this register flushes an individual TX FIFO TX
                                                         channel or block must be disable prior to writing to this bit
			 */
    } TFF_b;                   /*!< BitSize                                                               */
  };
  __I uint32_t RSVD0; /*!< none                                                                  */
  __I uint32_t RSVD1; /*!< none                                                                  */
} I2S0_CHANNEL_CONFIG_Type;

/* ================================================================================ */
/* ================                      I2S1                      ================ */
/* ================================================================================ */

/**
 * @brief I2S(Inter-IC Sound) is transferring two-channel digital audio data from one IC device to another (I2S1)
 */

typedef struct { /*!< I2S1 Structure                                                        */

  union {
    __IO uint32_t IER; /*!< I2S Enable Register                                                   */

    struct {
      __IO uint32_t IEN : 1; /*!< I2S Enable                                                    */
    } IER_b;                 /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t IRER; /*!< I2S Receiver Block Enable Register                                    */

    struct {
      __IO uint32_t RXEN : 1; /*!< Receive Block Enable, Bit Overrides any Individual Receive Channel
                                                         Enables                                                               */
    } IRER_b;                 /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t ITER; /*!< Transmitter Block Enable                                              */

    struct {
      __IO uint32_t TXEN : 1; /*!< Transmitter Block Enable, Bit Overrides any Individual Transmit
                                                         Channel Enables                                                       */
    } ITER_b;                 /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t CER; /*!< Clock Enable Register                                                 */

    struct {
      __IO uint32_t CLKEN : 1; /*!< Clock generation enable/disable                                       */
    } CER_b;                   /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t CCR; /*!< Clock Configuration Register                                          */

    struct {
      __IO uint32_t SCLKG : 3; /*!< These bits are used to program the gating of sclk                     */
      __IO uint32_t WSS : 2;   /*!< These bits are used to program the number of sclk cycles              */
    } CCR_b;                   /*!< BitSize                                                               */
  };

  union {
    __O uint32_t RXFFR; /*!< Receiver Block FIFO Reset Register                                    */

    struct {
      __O uint32_t RXFFR : 1; /*!< Writing a 1 To This Register Flushes All The RX FIFO's Receiver
                                                         Block Must be Disable Prior to Writing This Bit                       */
    } RXFFR_b;                /*!< BitSize                                                               */
  };

  union {
    __O uint32_t TXFFR; /*!< Transmitter Block FIFO Reset Register                                 */

    struct {
      __O uint32_t TXFFR : 1; /*!< Writing a 1 To This Register Flushes All The RX FIFO's Receiver
                                                         Block Must be Disable Prior to Writing This Bit                       */
    } TXFFR_b;                /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED0;
  I2S0_CHANNEL_CONFIG_Type
    CHANNEL_CONFIG[4]; /*!< CHANNEL_CONFIG0                                                       */
  __I uint32_t RESERVED1[40];

  union {
    __I uint32_t RXDMA; /*!< Receiver Block DMA Register                                           */

    struct {
      __I uint32_t RXDMA : 32; /*!< Used to cycle repeatedly through the enabled receive channels
                                                         Reading stereo data pairs                                             */
    } RXDMA_b;                 /*!< BitSize                                                               */
  };

  union {
    __O uint32_t RRXDMA; /*!< Reset Receiver Block DMA Register                                     */

    struct {
      __O uint32_t RRXDMA : 1; /*!< Writing a 1 to this self-clearing register resets the RXDMA
                                                         register                                                              */
    } RRXDMA_b;                /*!< BitSize                                                               */
  };

  union {
    __O uint32_t TXDMA; /*!< Transmitter Block DMA Register                                        */

    struct {
      __O uint32_t TXDMA : 32; /*!< Used to cycle repeatedly through the enabled transmit channels
                                                         allow to writing of stereo data pairs                                 */
    } TXDMA_b;                 /*!< BitSize                                                               */
  };

  union {
    __O uint32_t RTXDMA; /*!< Reset Transmitter Block DMA Register                                  */

    struct {
      __O uint32_t RTXDMA : 1; /*!< Writing a 1 to this self-clearing register resets the TXDMA
                                                         register                                                              */
    } RTXDMA_b;                /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED2[8];

  union {
    __I uint32_t I2S_COMP_PARAM_2; /*!< Component Parameter 2 Register                                        */

    struct {
      __I uint32_t I2S_RX_WORDSIZE_0 : 3; /*!< On Read returns the value of word size of receiver channel 0
			 */
      __I uint32_t I2S_RX_WORDSIZE_1 : 3; /*!< On Read returns the value of word size of receiver channel 1
			 */
      uint32_t : 1;
      __I uint32_t I2S_RX_WORDSIZE_2 : 3; /*!< On Read returns the value of word size of receiver channel 2
			 */
      __I uint32_t I2S_RX_WORDSIZE_3 : 3; /*!< On Read returns the value of word size of receiver channel 3
			 */
    } I2S_COMP_PARAM_2_b;                 /*!< BitSize                                                               */
  };

  union {
    __I uint32_t I2S_COMP_PARAM_1; /*!< Component Parameter 1 Register                                        */

    struct {
      __I uint32_t APB_DATA_WIDTH : 2; /*!< Width of APB data bus                                                 */
      __I uint32_t
        I2S_FIFO_DEPTH_GLOBAL : 2;       /*!< Determines FIFO depth for all channels                                */
      __I uint32_t I2S_FIFO_MODE_EN : 1; /*!< Determines whether component act as Master or Slave                   */
      __I uint32_t
        I2S_TRANSMITTER_BLOCK : 1;         /*!< Shows the presence of the transmitter block                           */
      __I uint32_t I2S_RECEIVER_BLOCK : 1; /*!< Shows the presence of the receiver block                              */
      __I uint32_t I2S_RX_CHANNELS : 2;    /*!< Returns the number of receiver channels                               */
      __I uint32_t I2S_TX_CHANNELS : 2;    /*!< Returns the number of transmitter channels                            */
      uint32_t : 5;
      __I uint32_t I2S_TX_WORDSIZE_0 : 3; /*!< Returns the value of word size of transmitter channel 0               */
      __I uint32_t I2S_TX_WORDSIZE_1 : 3; /*!< Returns the value of word size of transmitter channel 1               */
      __I uint32_t I2S_TX_WORDSIZE_2 : 3; /*!< Returns the value of word size of transmitter channel 2               */
      __I uint32_t I2S_TX_WORDSIZE_3 : 3; /*!< On Read returns the value of word size of transmitter channel
                                                         3                                                                     */
    } I2S_COMP_PARAM_1_b;                 /*!< BitSize                                                               */
  };

  union {
    __I uint32_t I2S_COMP_VERSION_REG; /*!< Component Version ID                                                  */

    struct {
      __I uint32_t I2S_COMP_VERSION : 32; /*!< Return the component version(1.02)                                    */
    } I2S_COMP_VERSION_REG_b;             /*!< BitSize                                                               */
  };

  union {
    __I uint32_t I2S_COMP_TYPE_REG; /*!< Component Type                                                        */

    struct {
      __I uint32_t I2S_COMP_TYPE : 32; /*!< Return the component type                                             */
    } I2S_COMP_TYPE_REG_b;             /*!< BitSize                                                               */
  };
} I2S0_Type;
/* ================================================================================ */
/* ================                     IID_AES                    ================ */
/* ================================================================================ */

/**
 * @brief The AES module provides AES encoding and decoding functionality. It can be used in a microprocessor based environment (IID_AES)
 */

typedef struct { /*!< IID_AES Structure                                                     */

  union {
    __IO uint32_t AES_KCR; /*!< AES Key Control register                                              */

    struct {
      __IO uint32_t AES_KEY_CHNG_REQ : 1; /*!< Programming 1 clears the current key and starts a request a
                                                         for a new keyAuto-reverts to 0 as soon as the request is accepted
			 */
      __IO uint32_t AES_KEY_SIZE : 1;     /*!< Size of the AES key 0: 128-bit 1: 256-bit                             */
      uint32_t : 5;
      __IO uint32_t AES_KEY_SRC : 1; /*!< Source of the AES key 0: Interface 1: Register                        */
    } AES_KCR_b;                     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t AES_MODE_REG; /*!< AES Mode register                                                     */

    struct {
      __IO uint32_t AES_MODE : 8; /*!< The AES Mode register defines which mode of AES is used.              */
    } AES_MODE_REG_b;             /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t AES_ACT_REG; /*!< AES Action register                                                   */

    struct {
      __IO uint32_t AES_ACTION : 2; /*!< The AES Mode register defines which mode of AES is used.              */
    } AES_ACT_REG_b;                /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED[5];

  union {
    __I uint32_t AES_SR_REG; /*!< AES Status register                                                   */

    struct {
      __I uint32_t AES_BUSY : 1; /*!< Indicates that the AES core is processing data                        */
      uint32_t : 1;
      __I uint32_t AES_CLEAR_DONE : 1;  /*!< Indicates that the Clear action is finished                           */
      __I uint32_t AES_KEY_PRESENT : 1; /*!< Indicates that the Clear action is finished                           */
      uint32_t : 1;
      __I uint32_t AES_KEY_REQ : 1;  /*!< Indicates that a key must be provided                                 */
      __I uint32_t AES_DATA_REQ : 1; /*!< Indicates that data must be provided                                  */
      __I uint32_t AES_DATA_AV : 1;  /*!< Indicates that data is available                                      */
    } AES_SR_REG_b;                  /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED1[7];

  union {
    __O uint32_t AES_KEY_REG; /*!< The AES Key register is used to program a key into the AES module.    */

    struct {
      __O uint32_t AES_KEY : 32; /*!< 4 writes of 32 bits make up the 128-bit key for AES, 8 writes
                                                         make up the 256-bit key                                               */
    } AES_KEY_REG_b;             /*!< BitSize                                                               */
  };

  union {
    __O uint32_t AES_DIN_REG; /*!< AES Data In register                                                  */

    struct {
      __O uint32_t AES_DIN : 32; /*!< Data for encoding or decoding, 4 writes of 32 bits make up a
                                                         128-bit data word                                                     */
    } AES_DIN_REG_b;             /*!< BitSize                                                               */
  };

  union {
    __I uint32_t AES_DOUT_REG; /*!< AES Data out register                                                 */

    struct {
      __I uint32_t AES_DOUT : 32; /*!< Result from encoding or decoding, 4 reads of 32 bits make up
                                                         a 128-bit data word                                                   */
    } AES_DOUT_REG_b;             /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED2[36];

  union {
    union {
      __O uint32_t AES_IF_SR_C_REG; /*!< AES Interface Status Clear register                                   */

      struct {
        __O uint32_t IFB_ERROR : 1; /*!< Clears the if_error bit                                               */
      } AES_IF_SR_C_REG_b;          /*!< BitSize                                                               */
    };

    union {
      __I uint32_t AES_IF_SR_REG; /*!< AES Interface Status register                                         */

      struct {
        __I uint32_t IF_ERROR : 1; /*!< Indicates that an interface error has occurred                        */
      } AES_IF_SR_REG_b;           /*!< BitSize                                                               */
    };
  };

  union {
    __IO uint32_t AES_TEST_REG; /*!< AES Test register                                                     */

    struct {
      __IO uint32_t AES_BIST_ENABLE : 1; /*!< Isolates the iid_aes module and runs a BIST                           */
      uint32_t : 3;
      __IO uint32_t AES_BIST_RUNNING : 1; /*!< BIST is in progress or finishing up                                   */
      __IO uint32_t AES_BIST_ACTIVE : 1;  /*!< Indicates that the BIST is running                                    */
      __IO uint32_t AES_BIST_OK : 1;      /*!< Indicates that the BIST has passed                                    */
      __IO uint32_t AES_BIST_ERROR : 1;   /*!< Indicates that the BIST has failed                                    */
    } AES_TEST_REG_b;                     /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED3[6];

  union {
    __I uint32_t AES_VER_REG; /*!< AES Version register                                                  */

    struct {
      __I uint32_t AES_VERSION : 32; /*!< Version of iid_aes                                                    */
    } AES_VER_REG_b;                 /*!< BitSize                                                               */
  };
} IID_AES_Type;

/* ================================================================================ */
/* ================                     IID_QK                     ================ */
/* ================================================================================ */

/**
 * @brief The purpose of Quiddikey is to provide secure key storage without storing the key. (IID_QK)
 */

typedef struct { /*!< IID_QK Structure                                                      */

  union {
    __O uint32_t QK_CR_REG; /*!< Quiddikey Control register.The Quiddikey Control register defines
                                                         which command must be executed next.                                  */

    struct {
      __O uint32_t QK_ZEROIZE : 1; /*!< Begin Zeroize operation and go to Error state                         */
      __O uint32_t QK_ENROLL : 1;  /*!< Begin Enroll operation                                                */
      __O uint32_t QK_START : 1;   /*!< Begin Start operation                                                 */
      __O uint32_t QK_SET_IK : 1;  /*!< Begin Set Intrinsic Key operation                                     */
      __O uint32_t QK_SET_UK : 1;  /*!< Begin Set User Key operation                                          */
      __O uint32_t QK_SET_XK : 1;  /*!< Begin Set External Key operation                                      */
      __O uint32_t QK_GET_KEY : 1; /*!< Begin Get Key operation                                               */
    } QK_CR_REG_b;                 /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t QK_KIDX_REG; /*!< The Quiddikey Key Index register defines the key index for the
                                                         next set_key command                                                  */

    struct {
      __IO uint32_t QK_KEY_INDEX : 4; /*!< Key index for Set Key operations                                      */
    } QK_KIDX_REG_b;                  /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t QK_KSZ_REG; /*!< Quiddikey Key Size register                                           */

    struct {
      __IO uint32_t QK_KEY_SIZE : 6; /*!< Key size for Set Key operations                                       */
    } QK_KSZ_REG_b;                  /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t QK_KT_REG; /*!< Quiddikey Key Size register                                           */

    struct {
      __IO uint32_t QK_KEY_TARGET : 1; /*!< Target of reconstructed key                                           */
    } QK_KT_REG_b;                     /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED[4];

  union {
    __I uint32_t QK_SR_REG; /*!< Quiddikey Status register                                             */

    struct {
      __I uint32_t QK_BUSY : 1;   /*!< Indicates that operation is in progress                               */
      __I uint32_t QK_OK : 1;     /*!< Last operation was successful                                         */
      __I uint32_t QK_ERROR : 1;  /*!< Quiddikey is in the Error state and no operations can be performed
			 */
      __I uint32_t QK_XO_AV : 1;  /*!< Next part of XKPD is available                                        */
      __I uint32_t QK_KI_REQ : 1; /*!< Request for next part of key                                          */
      __I uint32_t QK_KO_AV : 1;  /*!< Next part of key is available                                         */
      __I uint32_t QK_CI_REQ : 1; /*!< Request for next part of AC/KC                                        */
      __I uint32_t QK_CO_AV : 1;  /*!< Next part of AC/KC is available                                       */
    } QK_SR_REG_b;                /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED1;

  union {
    __I uint32_t QK_AR_REG; /*!< Quiddikey allow register                                              */

    struct {
      __I uint32_t QK_ALLOW_ENROLL : 1;  /*!< Enroll operation is allowed                                           */
      __I uint32_t QK_ALLOW_START : 1;   /*!< Start operation is allowed                                            */
      __I uint32_t QK_ALLOW_SET_KEY : 1; /*!< Set Key operations are allowed                                        */
      __I uint32_t QK_ALLOW_GET_KEY : 1; /*!< Get Key operation is allowed                                          */
      uint32_t : 3;
      __I uint32_t QK_ALLOW_BIST : 1; /*!< BIST is allowed to be started                                         */
    } QK_AR_REG_b;                    /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED2[5];

  union {
    __O uint32_t QK_KI_REG; /*!< Quiddikey Key Input register                                          */

    struct {
      __IO uint32_t QK_KI : 32; /*!< Key input data                                                        */
    } QK_KI_REG_b;              /*!< BitSize                                                               */
  };

  union {
    __O uint32_t QK_CI_REG; /*!< Quiddikey Code Input register                                         */

    struct {
      __IO uint32_t QK_CI : 32; /*!< AC/KC input data                                                      */
    } QK_CI_REG_b;              /*!< BitSize                                                               */
  };

  union {
    __I uint32_t QK_CO_REG; /*!< Quiddikey Code Output register                                        */

    struct {
      __I uint32_t QK_CO : 32; /*!< AC/KC output data                                                     */
    } QK_CO_REG_b;             /*!< BitSize                                                               */
  };

  union {
    __I uint32_t QK_XO_REG; /*!< Quiddikey XKPD Output register                                        */

    struct {
      __I uint32_t QK_XO : 32; /*!< XKPD output data                                                      */
    } QK_XO_REG_b;             /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED3[4];

  union {
    __I uint32_t QK_KO_IDX_REG; /*!< Quiddikey Key Output Index register                                   */

    struct {
      __I uint32_t qk_ko_index : 4; /*!< Key index for the key that is currently output via the Key Output
                                                         register                                                              */
    } QK_KO_IDX_REG_b;              /*!< BitSize                                                               */
  };

  union {
    __I uint32_t QK_KO_REG; /*!< Quiddikey Code Output register                                        */

    struct {
      __I uint32_t QK_KO : 32; /*!< Key output data                                                       */
    } QK_KO_REG_b;             /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED4[29];

  union {
    union {
      __I uint32_t QK_IF_SR_C_REG; /*!< Quiddikey Interface Status register                                   */

      struct {
        __I uint32_t IF_ERROR : 1; /*!< Clears the if_error bit                                               */
      } QK_IF_SR_C_REG_b;          /*!< BitSize                                                               */
    };

    union {
      __I uint32_t QK_IF_SR_REG; /*!< Quiddikey Interface Status register                                   */

      struct {
        __I uint32_t IF_ERROR : 1; /*!< Indicates that an interface error has occurred                        */
      } QK_IF_SR_REG_b;            /*!< BitSize                                                               */
    };
  };

  union {
    __IO uint32_t QK_TEST_REG; /*!< QK Test register                                                      */

    struct {
      __IO uint32_t QK_BIST_ENABLE : 1; /*!< Isolates the iid_quiddikey module and runs a BIST                     */
      uint32_t : 3;
      __IO uint32_t QK_BIST_RUNNING : 1; /*!< BIST is in progress or finishing up                                   */
      __IO uint32_t QK_BIST_ACTIVE : 1;  /*!< Indicates that the BIST is running                                    */
      __IO uint32_t QK_BIST_OK : 1;      /*!< Indicates that the BIST has passed                                    */
      __IO uint32_t QK_BIST_ERROR : 1;   /*!< Indicates that the BIST has failed                                    */
    } QK_TEST_REG_b;                     /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED5[6];

  union {
    __I uint32_t QK_VER_REG; /*!< QK Version register                                                   */

    struct {
      __I uint32_t QK_VERSION : 32; /*!< Version of iid_qk                                                     */
    } QK_VER_REG_b;                 /*!< BitSize                                                               */
  };
} IID_QK_Type;

/* ================================================================================ */
/* ================                    IID_RPINE                   ================ */
/* ================================================================================ */

/**
 * @brief none (IID_RPINE)
 */

typedef struct { /*!< IID_RPINE Structure                                                   */

  union {
    __IO uint32_t IID_BIST_CTRL_REG; /*!< Quiddikey Control register.The Quiddikey Control register defines
                                                         which command must be executed next.                                  */

    struct {
      __IO uint32_t QK_BIST_ENABLE : 1;  /*!< none                                                                  */
      __IO uint32_t AES_BIST_ENABLE : 1; /*!< none                                                                  */
      __IO uint32_t KH_BIST_ENABLE : 1;  /*!< none                                                                  */
    } IID_BIST_CTRL_REG_b;               /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t IID_BIST_STATUS_REG; /*!< none                                                                  */

    struct {
      __IO uint32_t QK_BIST_ACTIVE : 1;   /*!< none                                                                  */
      __IO uint32_t QK_BIST_ERROR : 1;    /*!< Indicates that the BIST has failed                                    */
      __IO uint32_t QK_BIST_OK : 1;       /*!< Indicates that the BIST has passed                                    */
      __IO uint32_t QK_BIST_RUNNING : 1;  /*!< Indicates that the BIST is running                                    */
      __IO uint32_t AES_BIST_ACTIVE : 1;  /*!< none                                                                  */
      __IO uint32_t AES_BIST_ERROR : 1;   /*!< none                                                                  */
      __IO uint32_t AES_BIST_OK : 1;      /*!< Indicates that the BIST has passed                                    */
      __IO uint32_t AES_BIST_RUNNING : 1; /*!< Indicates that the BIST is running                                    */
      __IO uint32_t KH_BIST_STATUS : 1;   /*!< none                                                                  */
    } IID_BIST_STATUS_REG_b;              /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t IID_CTRL_REG; /*!< none                                                                  */

    struct {
      __IO uint32_t AES_MAX_KEY_SIZE : 1; /*!< 1 256 bit key, 0 128 bit key                                          */
      __IO uint32_t SOURCE_KEY_KH : 1;    /*!< When set KH will source the key to AES engine. When this is
                                                         not QK key output is connected to AES key input                       */
      __IO uint32_t LATCH_KEY_KH : 1;     /*!< When set KH will latch the key given by QK. When this is not
                                                         QK key output is connected to AES key input                           */
      __IO uint32_t KH_RESET_N : 1;       /*!< 0 KH will be in reset 1 Out of reset                                  */
      __IO uint32_t KH_KEY_SIZE : 1;      /*!< 0 128 bit key 1 256 bit key This is used by KH                        */
      __IO uint32_t KH_CLOCK_RATIO : 3;   /*!< Indicates the division factor to be used for generating kh_clk.
			 */
    } IID_CTRL_REG_b;                     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t WKE_CTRL_REG; /*!< none                                                                  */

    struct {
      __IO uint32_t ENABLE_WKE : 1;    /*!< When WKE will be enabled. This is a self clearing bit. Once
                                                         enabled WKE can not be disabled till process is done                  */
      __IO uint32_t WKE_KEY_SIZE : 1;  /*!< 0 128 bit size 1 256 bit size                                         */
      __IO uint32_t WKE_FLUSH : 1;     /*!< When set, WKE will flush out the data from AES. When WEK is
                                                         active, firmware reads to AES engine are masked. This gets cleared
                                                          once four dwords are read from AES                                   */
      __IO uint32_t WKE_COMPARE : 1;   /*!< When set, WKE will compare the data from AES engine with the
                                                         data provided by firmware                                             */
      __IO uint32_t WKE_SET_KEY : 1;   /*!< This has to be set after key available from AES                       */
      __IO uint32_t KEY_CODE_DONE : 1; /*!< This has to be set after reading key code                             */
    } WKE_CTRL_REG_b;                  /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED;

  union {
    __IO uint32_t IID_AES_CTRL_REG; /*!< none                                                                  */

    struct {
      __IO uint32_t KEY_REQ_IN_DMA_PATH : 1;         /*!< Include key req in dma path. With this KEY Also can be loaded
                                                         using DMA.                                                            */
      __IO uint32_t AES_MAX_KEY_SIZE_FRM_REG : 1;    /*!< This is valid only when aes_max_key_size_frm_reg_en is set.
			 */
      __IO uint32_t AES_MAX_KEY_SIZE_FRM_REG_EN : 1; /*!< When set, WKE will flush out the data from AES. When WEK is
                                                         active, firmware reads to AES engine are masked. This gets cleared
                                                          once four dwords are read from AES                                   */
      __IO uint32_t OTP_KEY_LOADING : 1;             /*!< When set, WKE will compare the data from AES engine with the
                                                         data provided by firmware                                             */
    } IID_AES_CTRL_REG_b; /*!< BitSize                                                               */
  };

  union {
    __I uint32_t IID_AES_STS_REG; /*!< none                                                                  */

    struct {
      __I uint32_t DIN_FIFO_FULL : 1;   /*!< Input data fifo full indication                                       */
      __I uint32_t DOUT_FIFO_EMPTY : 1; /*!< Output data fifo empty indication                                     */
    } IID_AES_STS_REG_b;                /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED1;

  union {
    __IO uint32_t WKE_STATUS_REG; /*!< none                                                                  */

    struct {
      __IO uint32_t WKE_ACTIVE : 1; /*!< Will be high when WKE is active                                       */
      __IO uint32_t
        WKE_KEY_FEED_IN_PROGRESS : 1; /*!< Will be high when WKE is feeding key to AES engine                    */
      __IO uint32_t
        WKE_FLUSH_IN_PROGRESS : 1; /*!< Will be high when WKE flushing out the data from AES                  */
      __IO uint32_t
        WKE_COMPARE_IN_PROGRESS : 1; /*!< Will be high when WKE is comparing the data from AES                  */
      __IO uint32_t
        WKE_SET_KEY_IN_PROGRESS : 1;   /*!< Will be high when WKE is doing set key operation with QK              */
      __IO uint32_t WKE_KEY_READY : 1; /*!< Firmware has to load the authentication, which will be compared
                                                         with AES output, when this bit is low                                 */
      __IO uint32_t WKE_CMP_DATA_READY : 1; /*!< Firmware has to load the authentication, which will be compared
                                                         with AES output, when this bit is low                                 */
      __IO uint32_t WKE_COMPARE_FAIL : 1;   /*!< This bit will be set when authentication data comparison fails
			 */
    } WKE_STATUS_REG_b; /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED2;
  __IO uint32_t WKE_DATA_REG; /*!< none                                                                  */
} IID_RPINE_Type;

/* ================================================================================ */
/* ================                      SCT0                      ================ */
/* ================================================================================ */

/**
 * @brief Configurable timer is used in counting clocks, events and states with reference clock external clock and system clock  (SCT0)
 */

typedef struct { /*!< SCT0 Structure                                                        */

  union {
    __IO uint32_t GEN_CTRL_SET_REG; /*!< General control set register                                          */

    struct {
      __IO uint32_t COUNTER_IN_32_BIT_MODE : 1;        /*!< Counter_1 and Counter_0 will be merged and used as a single
                                                         32 bit counter                                                        */
      __IO uint32_t SOFT_RESET_COUNTER_0_FRM_REG : 1;  /*!< This is applied to 32 bits of counter only when the counter
                                                         is in 32 bit counter mode otherwise this will be applied to
                                                          only lower 16 bits of counter                                        */
      __IO uint32_t PERIODIC_EN_COUNTER_0_FRM_REG : 1; /*!< This is applied to 32 bits of counter only when the counter
                                                         is in 32 bit counter mode otherwise this will be applied to
                                                          only lower 16 bits of counter                                        */
      __IO uint32_t
        COUNTER_0_TRIG_FRM_REG : 1;        /*!< This enables the counter to run/active                                */
      __IO uint32_t COUNTER_0_UP_DOWN : 2; /*!< This enables the counter to run in up/down/up-down/down-up directions */
      __IO uint32_t COUNTER_0_SYNC_TRIG : 1; /*!< This is applied to 32 bits of counter only when the counter
                                                         is in 32 bit counter mode otherwise this will be applied to
                                                          only lower 16 bits of counter.This enables the counter to run/active
                                                          when sync is found.                                                  */
      __IO uint32_t BUF_REG_0_EN : 1;        /*!< Buffer register gets enabled for MATCH REG. MATCH_BUF_REG is
                                                         always available and whenever this bit is set only, gets copied
                                                          to MATCH REG.                                                        */
      uint32_t : 9;
      __IO uint32_t
        SOFT_RESET_COUNTER_1_FRM_REG : 1; /*!< This resets the counter on the write                                */
      __IO uint32_t
        PERIODIC_EN_COUNTER_1_FRM_REG : 1; /*!< This resets the counter on the write                               */
      __IO uint32_t
        COUNTER_1_TRIG_FRM_REG : 1;        /*!< This enables the counter to run/active                                */
      __IO uint32_t COUNTER_1_UP_DOWN : 2; /*!< This enables the counter to run in upward direction                   */
      __IO uint32_t COUNTER_1_SYNC_TRIG : 1; /*!< This is applied to 32 bits of counter only when the counter
                                                         is in 32 bit counter mode otherwise this will be applied to
                                                          only lower 16 bits of counter.This enables the counter to run/active
                                                          when sync is found.                                                  */
      __IO uint32_t BUF_REG_1_EN : 1;        /*!< Buffer register gets enabled for MATCH REG. MATCH_BUF_REG is
                                                         always available and whenever this bit is set only, gets copied
                                                          to MATCH REG.                                                        */
    } GEN_CTRL_SET_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t GEN_CTRL_RESET_REG; /*!< General control reset register                                        */

    struct {
      __IO uint32_t COUNTER_IN_32_BIT_MODE : 1;        /*!< Counter_1 and Counter_0 will be merged and used as a single
                                                         32 bit counter                                                        */
      __IO uint32_t SOFT_RESET_COUNTER_0_FRM_REG : 1;  /*!< This is applied to 32 bits of counter only when the counter
                                                         is in 32 bit counter mode otherwise this will be applied to
                                                          only lower 16 bits of counter                                        */
      __IO uint32_t PERIODIC_EN_COUNTER_0_FRM_REG : 1; /*!< This is applied to 32 bits of counter only when the counter
                                                         is in 32 bit counter mode otherwise this will be applied to
                                                          only lower 16 bits of counter                                        */
      __IO uint32_t
        COUNTER_0_TRIG_FRM_REG : 1;        /*!< This enables the counter to run/active                                */
      __IO uint32_t COUNTER_0_UP_DOWN : 2; /*!< This enables the counter to run in up/down/up-down/down-up directions */
      __IO uint32_t COUNTER_0_SYNC_TRIG : 1; /*!< This is applied to 32 bits of counter only when the counter
                                                         is in 32 bit counter mode otherwise this will be applied to
                                                          only lower 16 bits of counter.This enables the counter to run/active
                                                          when sync is found.                                                  */
      __IO uint32_t BUF_REG_0_EN : 1;        /*!< Buffer register gets enabled for MATCH REG. MATCH_BUF_REG is
                                                         always available and whenever this bit is set only, gets copied
                                                          to MATCH REG.                                                        */
      uint32_t : 9;
      __IO uint32_t
        SOFT_RESET_COUNTER_1_FRM_REG : 1; /*!< This resets the counter on the write                                */
      __IO uint32_t
        PERIODIC_EN_COUNTER_1_FRM_REG : 1; /*!< This resets the counter on the write                               */
      __IO uint32_t
        COUNTER_1_TRIG_FRM : 1;            /*!< This enables the counter to run/active                                */
      __IO uint32_t COUNTER_1_UP_DOWN : 2; /*!< This enables the counter to run in upward direction                   */
      __IO uint32_t COUNTER_1_SYNC_TRIG : 1; /*!< This is applied to 32 bits of counter only when the counter
                                                         is in 32 bit counter mode otherwise this will be applied to
                                                          only lower 16 bits of counter.This enables the counter to run/active
                                                          when sync is found.                                                  */
      __IO uint32_t BUF_REG_1_EN : 1;        /*!< Buffer register gets enabled for MATCH REG. MATCH_BUF_REG is
                                                         always available and whenever this bit is set only, gets copied
                                                          to MATCH REG.                                                        */
    } GEN_CTRL_RESET_REG_b; /*!< BitSize                                                               */
  };

  union {
    __I uint32_t INTR_STS; /*!< Interrupt status                                                      */

    struct {
      __I uint32_t INTR_0_L : 1;      /*!< Indicates the FIFO full signal of channel-0                           */
      __I uint32_t FIFO_0_FULL_L : 1; /*!< Indicates the FIFO full signal of channel-0                           */
      __I uint32_t
        COUNTER_0_IS_ZERO_L : 1; /*!< Counter 0 hit zero in active mode.                                    */
      __I uint32_t
        COUNTER_0_IS_PEAK_L : 1; /*!< Counter 0 hit peak (MATCH) in active mode.                            */
      uint32_t : 12;
      __I uint32_t INTR_1_L : 1;      /*!< Indicates the FIFO full signal of channel-1                           */
      __I uint32_t FIFO_1_FULL_L : 1; /*!< Indicates the FIFO full signal of channel-1                           */
      __I uint32_t
        COUNTER_1_IS_ZERO_L : 1; /*!< Counter 1 hit zero in active mode.                                    */
      __I uint32_t
        COUNTER_1_IS_PEAK_L : 1; /*!< Counter 1 hit peak (MATCH) in active mode.                            */
    } INTR_STS_b;                /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t INTR_MASK; /*!< Interrupts mask                                                       */

    struct {
      __IO uint32_t INTR_0_L : 1;      /*!< Interrupt mask signal.                                                */
      __IO uint32_t FIFO_0_FULL_L : 1; /*!< Interrupt mask signal.                                                */
      __IO uint32_t
        COUNTER_0_IS_ZERO_L : 1; /*!< Interrupt mask signal.                                                */
      __IO uint32_t
        COUNTER_0_IS_PEAK_L : 1; /*!< Interrupt mask signal.                                                */
      uint32_t : 12;
      __IO uint32_t INTR_1_L : 1;      /*!< Interrupt mask signal.                                                */
      __IO uint32_t FIFO_1_FULL_L : 1; /*!< Interrupt mask signal.                                                */
      __IO uint32_t
        COUNTER_1_IS_ZERO_L : 1; /*!< Interrupt mask signal.                                                */
      __IO uint32_t
        COUNTER_1_IS_PEAK_L : 1; /*!< Interrupt mask signal.                                                */
    } INTR_MASK_b;               /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t INTR_UNMASK; /*!< Interrupts unmask                                                     */

    struct {
      __IO uint32_t INTR_0_L : 1;      /*!< Interrupt unmask signal.                                              */
      __IO uint32_t FIFO_0_FULL_L : 1; /*!< Interrupt unmask signal.                                              */
      __IO uint32_t
        COUNTER_0_IS_ZERO_L : 1; /*!< Interrupt unmask signal.                                              */
      __IO uint32_t
        COUNTER_0_IS_PEAK_L : 1; /*!< Interrupt unmask signal.                                              */
      uint32_t : 12;
      __IO uint32_t INTR_1_L : 1;      /*!< Interrupt unmask signal.                                              */
      __IO uint32_t FIFO_1_FULL_L : 1; /*!< Interrupt unmask signal.                                              */
      __IO uint32_t
        COUNTER_1_IS_ZERO_L : 1; /*!< Interrupt unmask signal.                                              */
      __IO uint32_t
        COUNTER_1_IS_PEAK_L : 1; /*!< Interrupt unmask signal.                                              */
    } INTR_UNMASK_b;             /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t INTR_ACK; /*!< Interrupt clear/ack register                                          */

    struct {
      __IO uint32_t INTR_0_L : 1;      /*!< Interrupt ack signal.                                                 */
      __IO uint32_t FIFO_0_FULL_L : 1; /*!< Interrupt ack signal.                                                 */
      __IO uint32_t
        COUNTER_0_IS_ZERO_L : 1; /*!< Interrupt ack signal.                                                 */
      __IO uint32_t
        COUNTER_0_IS_PEAK_L : 1; /*!< Interrupt ack signal.                                                 */
      uint32_t : 12;
      __IO uint32_t INTR_1_L : 1;      /*!< Interrupt ack signal.                                                 */
      __IO uint32_t FIFO_1_FULL_L : 1; /*!< Interrupt ack signal.                                                 */
      __IO uint32_t
        COUNTER_1_IS_ZERO_L : 1; /*!< Interrupt ack signal.                                                 */
      __IO uint32_t
        COUNTER_1_IS_PEAK_L : 1; /*!< Interrupt ack signal.                                                 */
    } INTR_ACK_b;                /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t MATCH_REG; /*!< Match value register                                                  */

    struct {
      __IO uint32_t COUNTER_0_MATCH : 16; /*!< This will be used as lower match                                      */
      __IO uint32_t COUNTER_1_MATCH : 16; /*!< This will be used as upper match                                      */
    } MATCH_REG_b;                        /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t MATCH_BUF_REG; /*!< Match Buffer register                                                 */

    struct {
      __IO uint32_t COUNTER_0_MATCH_BUF : 16; /*!< This gets copied to MATCH register if bug_reg_0_en is set. Copying
                                                         is done when counter 0 is active and hits 0.                          */
      __IO uint32_t COUNTER_1_MATCH_BUF : 16; /*!< This gets copied to MATCH register if bug_reg_1_en is set.Copying
                                                         is done when counter 1 is active and hits 0.                          */
    } MATCH_BUF_REG_b; /*!< BitSize                                                               */
  };

  union {
    __I uint32_t CAPTURE_REG; /*!< Capture Register                                                      */

    struct {
      __I uint32_t COUNTER_0_CAPTURE : 16; /*!< This is a latched value of counter lower part when the selected
                                                         capture_event occurs                                                  */
      __I uint32_t COUNTER_1_CAPTURE : 16; /*!< This is a latched value of counter upper part when the selected
                                                         capture_event occurs                                                  */
    } CAPTURE_REG_b;                       /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t COUNTER_REG; /*!< Counter Register                                                      */

    struct {
      __IO uint32_t COUNTER0 : 16; /*!< This holds the value of counter-0                                     */
      __IO uint32_t COUNTER1 : 16; /*!< This holds the value of counter-1                                     */
    } COUNTER_REG_b;               /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t OCU_CTRL_REG; /*!< OCU control register                                                  */

    struct {
      __IO uint32_t OUTPUT_IS_OCU_0 : 1; /*!< Indicates whether the output is in OCU mode or not for channel-0      */
      __IO uint32_t SYNC_WITH_0 : 3;     /*!< Indicates whether the other channel is in sync with this channel      */
      __IO uint32_t OCU_DMA_MODE_0 : 1;  /*!< Indicates whether the OCU DMA mode is active or not for channel
                                                         0                                                                     */
      __IO uint32_t OCU_8_16_MODE_0 : 1; /*!< Indicates whether entire 16 bits or only 8-bits of the channel
                                                         0 are used in OCU mode                                                */
      __IO uint32_t MAKE_OUTPUT_0_HIGH_SEL : 3; /*!< Check counter ocus for possibilities. When this is hit output
                                                         will be made high.                                                    */
      __IO uint32_t MAKE_OUTPUT_0_LOW_SEL : 3;  /*!< Check counter ocus for possibilities. When this is hit output
                                                         will be made low.                                                     */
      uint32_t : 4;
      __IO uint32_t OUTPUT_IS_OCU_1 : 1; /*!< Indicates whether the output is in OCU mode or not for channel
                                                         1                                                                     */
      __IO uint32_t SYNC_WITH_1 : 3;     /*!< Indicates whether the other channel is in sync with this channel      */
      __IO uint32_t OCU_DMA_MODE_1 : 1;  /*!< Indicates whether the OCU DMA mode is active or not for channel
                                                         1                                                                     */
      __IO uint32_t OCU_8_16_MODE_1 : 1; /*!< Indicates whether entire 16 bits or only 8-bits of the channel
                                                         1 are used in OCU mode                                                */
      __IO uint32_t MAKE_OUTPUT_1_HIGH_SEL : 3; /*!< Check counter ocus for possibilities. When this is hit output
                                                         will be made high.                                                    */
      __IO uint32_t MAKE_OUTPUT_1_LOW_SEL : 3;  /*!< Check counter ocus for possibilities. When this is hit output
                                                         will be made low.                                                     */
    } OCU_CTRL_REG_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t OCU_COMPARE_REG; /*!< OCU Compare Register                                                  */

    struct {
      __IO uint32_t OCU_COMPARE_0 : 16; /*!< Holds the threshold value of present OCU period which denotes
                                                         the number of clock cycles for which the OCU output should be
                                                          considered (counter 0)                                               */
      __IO uint32_t OCU_COMPARE_1 : 16; /*!< Holds the threshold value of present OCU period which denotes
                                                         the number of clock cycles for which the OCU output should be
                                                          considered (counter 1)                                               */
    } OCU_COMPARE_REG_b;                /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t OCU_COMPARE2_REG; /*!< OCU Compare2 Register                                                 */

    struct {
      __IO uint32_t OCU_COMPARE2_0 : 16; /*!< Holds the threshold value of present OCU period2 which denotes
                                                         the number of clock cycles for which the OCU output should be
                                                          considered (counter 0)                                               */
      __IO uint32_t OCU_COMPARE2_1 : 16; /*!< Holds the threshold value of present OCU period2 which denotes
                                                         the number of clock cycles for which the OCU output should be
                                                          considered (counter 1)                                               */
    } OCU_COMPARE2_REG_b;                /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t OCU_SYNC_REG; /*!< OCU Synchronization Register                                          */

    struct {
      __IO uint32_t
        OCU_SYNC_CHANNEL0 : 16; /*!< Starting point of channel 0 for synchronization purpose               */
      __IO uint32_t
        OCU_SYNC_CHANNEL1 : 16; /*!< Starting point of channel 1 for synchronization purpose               */
    } OCU_SYNC_REG_b;           /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t OCU_COMPARE_NXT_REG; /*!< PWM compare next register                                             */

    struct {
      __IO uint32_t
        OCU_COMPARE_NXT_COUNTER1 : 16; /*!< OCU output should be high for counter 1                               */
      __IO uint32_t
        OCU_COMPARE_NXT_COUNTER0 : 16; /*!< PWM output should be high for counter 0                               */
    } OCU_COMPARE_NXT_REG_b;           /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t WFG_CTRL_REG; /*!< WFG control register                                                  */

    struct {
      __IO uint32_t MAKE_OUTPUT_0_TGL_0_SEL : 3; /*!< Check the counter ocus possibilities for description for channel
                                                         0.                                                                    */
      __IO uint32_t MAKE_OUTPUT_0_TGL_1_SEL : 3; /*!< Check the counter ocus possibilities for description for channel
                                                         0.                                                                    */
      uint32_t : 2;
      __IO uint32_t
        WFG_TGL_CNT_0_PEAK : 8; /*!< WFG mode output toggle count clock for channel 0.                     */
      __IO uint32_t MAKE_OUTPUT_1_TGL_0_SEL : 3; /*!< Check the counter ocus possibilities for description for channel
                                                         1.                                                                    */
      __IO uint32_t MAKE_OUTPUT_1_TGL_1_SEL : 3; /*!< Check the counter ocus possibilities for description for channel
                                                         1.                                                                    */
      uint32_t : 2;
      __IO uint32_t
        WFG_TGL_CNT_1_PEAK : 8; /*!< WFG mode output toggle count clock for channel 1                      */
    } WFG_CTRL_REG_b;           /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t OCU_COMPARE2_NXT_REG; /*!< PWM compare next register                                             */

    struct {
      __IO uint32_t
        OCU_COMPARE2_NXT_COUNTER0 : 16; /*!< OCU output should be high for counter 1                               */
      __IO uint32_t
        OCU_COMPARE2_NXT_COUNTER1 : 16; /*!< PWM output should be high for counter 0                               */
    } OCU_COMPARE2_NXT_REG_b;           /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED[3];

  union {
    __IO uint32_t START_COUNTER_EVENT_SEL; /*!< Start counter event select register                                   */

    struct {
      __IO uint32_t START_COUNTER_0_EVENT_SEL : 6; /*!< For two 16 bit counters mode: Event select for starting the
                                                         Counter 0 For 32 bit counter mode: Event select for starting
                                                          counter                                                              */
      uint32_t : 10;
      __IO uint32_t START_COUNTER_1_EVENT_SEL : 6; /*!< For two 16 bit counters mode: Event select for starting the
                                                         Counter 1. For 32 bit counter mode: Invalid. Please refer to
                                                          events table for description                                         */
    } START_COUNTER_EVENT_SEL_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t START_COUNTER_AND_EVENT; /*!< Start counter AND event register                                      */

    struct {
      __IO uint32_t
        START_COUNTER_0_AND_EVENT : 4; /*!< For two 16 bit counter mode: AND expression valids for AND event
                                                         in start Counter 0 event For 32 bit counter mode AND expression
                                                          valids for AND event in start counter event                          */
      uint32_t : 4;
      __IO uint32_t
        START_COUNTER_0_AND_VLD : 4; /*!< none                                                                  */
      uint32_t : 4;
      __IO uint32_t START_COUNTER_1_AND_EVENT : 4; /*!< For two 16 bit counters mode: AND expression valids for AND
                                                         event in start counter event For 32 bit counter mode : Invalid
			 */
      uint32_t : 4;
      __IO uint32_t
        START_COUNTER_1_AND_VLD : 4; /*!< none                                                                  */
    } START_COUNTER_AND_EVENT_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t START_COUNTER_OR_EVENT; /*!< Start counter OR event register                                       */

    struct {
      __IO uint32_t START_COUNTER_0_OR_EVENT : 4; /*!< For two 16 bit counter mode: OR expression valids for OR event
                                                         in start Counter 0 event For 32 bit counter mode OR expression
                                                          valids for OR event in start counter event                           */
      uint32_t : 4;
      __IO uint32_t
        START_COUNTER_0_OR_VLD : 4; /*!< none                                                                  */
      uint32_t : 4;
      __IO uint32_t START_COUNTER_1_OR_EVENT : 4; /*!< For two 16 bit counters mode: OR expression valids for OR event
                                                         in start counter event For 32 bit counter mode : Invalid.             */
      uint32_t : 4;
      __IO uint32_t
        START_COUNTER_1_OR_VLD : 4; /*!< none                                                                  */
    } START_COUNTER_OR_EVENT_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      CONTINUE_COUNTER_EVENT_SEL; /*!< Continue counter event select register                                */

    struct {
      __IO uint32_t
        CONTINUE_COUNTER_0_EVENT_SEL : 6; /*!< For two 16 bit counters mode: Event select for continuing the
                                                         Counter 0 For 32 bit counter mode: Event select for continuing
                                                          counter                                                              */
      uint32_t : 10;
      __IO uint32_t
        CONTINUE_COUNTER_1_EVENT_SEL : 6; /*!< For two 16 bit counters mode: Event select for continuing the
                                                         Counter 1 For 32 bit counter mode: Invalid.                           */
    } CONTINUE_COUNTER_EVENT_SEL_b;       /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      CONTINUE_COUNTER_AND_EVENT; /*!< Continue counter AND event register                                   */

    struct {
      __IO uint32_t
        CONTINUE_COUNTER_0_AND_EVENT : 4; /*!< For two 16 bit counter mode: AND expression valids for AND event
                                                         in continue Counter 0 event For 32 bit counter mode AND expression
                                                          valids for AND event in continue counter event.                      */
      uint32_t : 4;
      __IO uint32_t
        CONTINUE_COUNTER_0_AND_VLD : 4; /*!< none                                                                  */
      uint32_t : 4;
      __IO uint32_t CONTINUE_COUNTER_1_AND_EVENT : 4; /*!< For two 16 bit counters mode: AND expression valids for AND
                                                         event in continue counter event For 32 bit counter mode : Invalid
			 */
      uint32_t : 4;
      __IO uint32_t
        CONTINUE_COUNTER_1_AND_VLD : 4; /*!< none                                                                  */
    } CONTINUE_COUNTER_AND_EVENT_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      CONTINUE_COUNTER_OR_EVENT; /*!< Continue counter OR event register                                    */

    struct {
      __IO uint32_t
        CONTINUE_COUNTER_0_OR_EVENT : 4; /*!< For two 16 bit counter mode: OR expression valids for OR event
                                                         in continue Counter 0 event For 32 bit counter mode OR expression
                                                          valids for OR event in continue counter event                        */
      uint32_t : 4;
      __IO uint32_t
        CONTINUE_COUNTER_0_OR_VLD : 4; /*!< none                                                                  */
      uint32_t : 4;
      __IO uint32_t
        CONTINUE_COUNTER_1_OR_EVENT : 4; /*!< For two 16 bit counters mode: OR expression valids for OR event
                                                         in continue counter event For 32 bit counter mode : Invalid
			 */
      uint32_t : 4;
      __IO uint32_t
        CONTINUE_COUNTER_1_OR_VLD : 4; /*!< none                                                                  */
    } CONTINUE_COUNTER_OR_EVENT_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t STOP_COUNTER_EVENT_SEL; /*!< Stop counter event select register                                    */

    struct {
      __IO uint32_t STOP_COUNTER_0_EVENT_SEL : 6; /*!< For two 16 bit counters mode: Event select for Stopping the
                                                         Counter 0 For 32 bit counter mode: Event select for Stopping
                                                          counter                                                              */
      uint32_t : 10;
      __IO uint32_t STOP_COUNTER_1_EVENT_SEL : 6; /*!< For two 16 bit counters mode: Event select for Stopping the
                                                         Counter 1 For 32 bit counter mode: Invalid                            */
    } STOP_COUNTER_EVENT_SEL_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t STOP_COUNTER_AND_EVENT; /*!< Stop counter AND event register                                       */

    struct {
      __IO uint32_t STOP_COUNTER_0_AND_EVENT : 4; /*!< For two 16 bit counter mode: AND expression valids for AND event
                                                         in stop Counter 0 event For 32 bit counter mode AND expression
                                                          valids for AND event in stop counter event                           */
      uint32_t : 4;
      __IO uint32_t
        STOP_COUNTER_0_AND_VLD : 4; /*!< none                                                                  */
      uint32_t : 4;
      __IO uint32_t STOP_COUNTER_1_AND_EVENT : 4; /*!< For two 16 bit counters mode: AND expression valids for AND
                                                         event in stop counter event For 32 bit counter mode : Invalid
			 */
      uint32_t : 4;
      __IO uint32_t
        STOP_COUNTER_1_AND_VLD : 4; /*!< none                                                                  */
    } STOP_COUNTER_AND_EVENT_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t STOP_COUNTER_OR_EVENT; /*!< Stop counter OR event register                                        */

    struct {
      __IO uint32_t STOP_COUNTER_0_OR_EVENT : 4; /*!< For two 16 bit counter mode: OR expression valids for OR event
                                                         in Stop Counter 0 event For 32 bit counter mode OR expression
                                                          valids for OR event in Stop counter event                            */
      uint32_t : 4;
      __IO uint32_t
        STOP_COUNTER_0_OR_VLD : 4; /*!< none                                                                  */
      uint32_t : 4;
      __IO uint32_t STOP_COUNTER_1_OR_EVENT : 4; /*!< For two 16 bit counters mode: OR expression valids for OR event
                                                         in Stop counter event For 32 bit counter mode : Invalid               */
      uint32_t : 4;
      __IO uint32_t
        STOP_COUNTER_1_OR_VLD : 4; /*!< none                                                                  */
    } STOP_COUNTER_OR_EVENT_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t HALT_COUNTER_EVENT_SEL; /*!< Halt counter event select register                                    */

    struct {
      __IO uint32_t
        HALT_COUNTER_0_EVENT_SEL : 6; /*!< For two 16 bit counters mode: Event select for Halting the Counter
                                                         0 For 32 bit counter mode: Event select for Halting counter*/
      __IO uint32_t RESUME_HALT_COUNTER_0 : 1;
      uint32_t : 10;
      __IO uint32_t
        HALT_COUNTER_1_EVENT_SEL : 6; /*!< For two 16 bit counters mode: Event select for Halting the Counter
                                                         1 For 32 bit counter mode: Invalid                                    */
      __IO uint32_t RESUME_HALT_COUNTER_1 : 1;
    } HALT_COUNTER_EVENT_SEL_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t HALT_COUNTER_AND_EVENT; /*!< Halt counter AND event register                                       */

    struct {
      __IO uint32_t HALT_COUNTER_0_AND_EVENT : 4; /*!< For two 16 bit counter mode: AND expression valids for AND event
                                                         in stop Counter 0 event For 32 bit counter mode AND expression
                                                          valids for AND event in stop counter event                           */
      uint32_t : 4;
      __IO uint32_t
        HALT_COUNTER_0_AND_VLD : 4; /*!< none                                                                  */
      uint32_t : 4;
      __IO uint32_t HALT_COUNTER_1_AND_EVENT : 4; /*!< For two 16 bit counters mode: AND expression valids for AND
                                                         event in stop counter event For 32 bit counter mode : Invalid
			 */
      uint32_t : 4;
      __IO uint32_t
        HALT_COUNTER_1_AND_VLD : 4; /*!< none                                                                  */
    } HALT_COUNTER_AND_EVENT_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t HALT_COUNTER_OR_EVENT; /*!< Halt counter OR event register                                        */

    struct {
      __IO uint32_t HALT_COUNTER_0_OR_EVENT : 4; /*!< For two 16 bit counter mode: OR expression valids for OR event
                                                         in Halt Counter 0 event For 32 bit counter mode OR expression
                                                          valids for OR event in Halt counter event                            */
      uint32_t : 4;
      __IO uint32_t
        HALT_COUNTER_0_OR_VLD : 4; /*!< none                                                                  */
      uint32_t : 4;
      __IO uint32_t HALT_COUNTER_1_OR_EVENT : 4; /*!< For two 16 bit counters mode: OR expression valids for OR event
                                                         in Halt counter event For 32 bit counter mode : Invalid               */
      uint32_t : 4;
      __IO uint32_t
        HALT_COUNTER_1_OR_VLD : 4; /*!< none                                                                  */
    } HALT_COUNTER_OR_EVENT_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      INCREMENT_COUNTER_EVENT_SEL; /*!< Increment counter event select register                               */

    struct {
      __IO uint32_t INCREMENT_COUNTER_0_EVENT_SEL : 6; /*!< For two 16 bit counters mode: Event select for Incrementing
                                                         the Counter 0 For 32 bit counter mode: Event select for Incrementing
                                                          counter                                                              */
      uint32_t : 10;
      __IO uint32_t INCREMENT_COUNTER_1_EVENT_SEL : 6; /*!< For two 16 bit counters mode: Event select for Incrementing
                                                         the Counter 1 For 32 bit counter mode: Invalid                        */
    } INCREMENT_COUNTER_EVENT_SEL_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      INCREMENT_COUNTER_AND_EVENT; /*!< Increment counter AND event register                                  */

    struct {
      __IO uint32_t
        INCREMENT_COUNTER_0_AND_EVENT : 4; /*!< For two 16 bit counter mode: AND expression valids for AND event
                                                         in stop Counter 0 event For 32 bit counter mode AND expression
                                                          valids for AND event in stop counter event                           */
      uint32_t : 4;
      __IO uint32_t
        INCREMENT_COUNTER_0_AND_VLD : 4; /*!< none                                                                 */
      uint32_t : 4;
      __IO uint32_t INCREMENT_COUNTER_1_AND_EVENT : 4; /*!< For two 16 bit counters mode: AND expression valids for AND
                                                         event in stop counter event For 32 bit counter mode : Invalid
			 */
      uint32_t : 4;
      __IO uint32_t
        INCREMENT_COUNTER_1_AND_VLD : 4; /*!< none                                                                 */
    } INCREMENT_COUNTER_AND_EVENT_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      INCREMENT_COUNTER_OR_EVENT; /*!< Increment counter OR event register                                   */

    struct {
      __IO uint32_t
        INCREMENT_COUNTER_0_OR_EVENT : 4; /*!< For two 16 bit counter mode: OR expression valids for OR event
                                                         in Increment Counter 0 event For 32 bit counter mode OR expression
                                                          valids for OR event in Increment counter event                       */
      uint32_t : 4;
      __IO uint32_t
        INCREMENT_COUNTER_0_OR_VLD : 4; /*!< none                                                                  */
      uint32_t : 4;
      __IO uint32_t
        INCREMENT_COUNTER_1_OR_EVENT : 4; /*!< For two 16 bit counters mode: OR expression valids for OR event
                                                         in Increment counter event For 32 bit counter mode : Invalid
			 */
      uint32_t : 4;
      __IO uint32_t
        INCREMENT_COUNTER_1_OR_VLD : 4; /*!< none                                                                  */
    } INCREMENT_COUNTER_OR_EVENT_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      CAPTURE_COUNTER_EVENT_SEL; /*!< Capture counter event select register                                 */

    struct {
      __IO uint32_t CAPTURE_COUNTER_0_EVENT_SEL : 6; /*!< For two 16 bit counters mode: Event select for Capturing the
                                                         Counter 0 For 32 bit counter mode: Event select for Capturing
                                                          counter                                                              */
      uint32_t : 10;
      __IO uint32_t CAPTURE_COUNTER_1_EVENT_SEL : 6; /*!< For two 16 bit counters mode: Event select for Capturing the
                                                         Counter 1 For 32 bit counter mode : Invalid                           */
    } CAPTURE_COUNTER_EVENT_SEL_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      CAPTURE_COUNTER_AND_EVENT; /*!< Capture counter AND event register                                    */

    struct {
      __IO uint32_t
        CAPTURE_COUNTER_0_AND_EVENT : 4; /*!< For two 16 bit counter mode: AND expression valids for AND event
                                                         in stop Counter 0 event For 32 bit counter mode AND expression
                                                          valids for AND event in stop counter event                           */
      uint32_t : 4;
      __IO uint32_t
        CAPTURE_COUNTER_0_AND_VLD : 4; /*!< none                                                                  */
      uint32_t : 4;
      __IO uint32_t CAPTURE_COUNTER_1_AND_EVENT : 4; /*!< For two 16 bit counters mode: AND expression valids for AND
                                                         event in stop counter event For 32 bit counter mode : Invalid
			 */
      uint32_t : 4;
      __IO uint32_t
        CAPTURE_COUNTER_1_AND_VLD : 4; /*!< none                                                                  */
    } CAPTURE_COUNTER_AND_EVENT_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t
      CAPTURE_COUNTER_OR_EVENT; /*!< Capture counter OR event register                                     */

    struct {
      __IO uint32_t CAPTURE_COUNTER_0_OR_EVENT : 4; /*!< For two 16 bit counter mode: OR expression valids for OR event
                                                         in Capture Counter 0 event For 32 bit counter mode OR expression
                                                          valids for OR event in Capture counter event                         */
      uint32_t : 4;
      __IO uint32_t
        CAPTURE_COUNTER_0_OR_VLD : 4; /*!< none                                                                  */
      uint32_t : 4;
      __IO uint32_t
        CAPTURE_COUNTER_1_OR_EVENT : 4; /*!< For two 16 bit counters mode: OR expression valids for OR event
                                                         in Capture counter event For 32 bit counter mode : Invalid            */
      uint32_t : 4;
      __IO uint32_t
        CAPTURE_COUNTER_1_OR_VLD : 4; /*!< none                                                                  */
    } CAPTURE_COUNTER_OR_EVENT_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t OUTPUT_EVENT_SEL; /*!< Output event select register                                          */

    struct {
      __IO uint32_t OUTPUT_EVENT_SEL_0 : 6; /*!< For two 16 bit counters mode: Event select for output event
                                                         from Counter 0 For 32 bit counter mode: Event select for output
                                                          event                                                                */
      uint32_t : 10;
      __IO uint32_t OUTPUT_EVENT_SEL_1 : 6; /*!< For two 16 bit counters mode: Event select for output event
                                                         from counter 1 For 32 bit counter mode : Invalid                      */
    } OUTPUT_EVENT_SEL_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t OUTPUT_AND_EVENT_REG; /*!< Output AND event Register                                             */

    struct {
      __IO uint32_t
        OUTPUT_0_AND_EVENT : 4; /*!< AND expression for AND event in output Counter_0 event.               */
      uint32_t : 4;
      __IO uint32_t OUTPUT_0_AND_VLD : 4; /*!< AND expression for AND event in output Counter_0 event.               */
      uint32_t : 4;
      __IO uint32_t
        OUTPUT_1_AND_EVENT : 4; /*!< AND expression for AND event in output Counter_1 event.               */
      uint32_t : 4;
      __IO uint32_t OUTPUT_1_AND_VLD : 4; /*!< AND expression for AND event in output Counter_1 event.               */
    } OUTPUT_AND_EVENT_REG_b;             /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t OUTPUT_OR_EVENT; /*!< Output OR event Register                                              */

    struct {
      __IO uint32_t OUTPUT_0_OR_EVENT : 4; /*!< OR expression for OR event in output Counter_0 event                  */
      uint32_t : 4;
      __IO uint32_t OUTPUT_0_OR_VLD : 4; /*!< Indicates which bits in 3:0 are valid for considering OR event        */
      uint32_t : 4;
      __IO uint32_t OUTPUT_1_OR_EVENT : 4; /*!< OR expression for OR event in output Counter_0 event                  */
      uint32_t : 4;
      __IO uint32_t OUTPUT_1_OR_VLD : 4; /*!< Indicates which bits in 3:0 are valid for considering OR event        */
    } OUTPUT_OR_EVENT_b;                 /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t INTR_EVENT_SEL; /*!< Interrupt Event Select Register                                       */

    struct {
      __IO uint32_t INTR_EVENT_SEL_0 : 6; /*!< For two 16 bit counters mode: Event select for interrupt event
                                                         from Counter 0 For 32 bit counter mode: Event select for output
                                                          event                                                                */
      uint32_t : 10;
      __IO uint32_t INTR_EVENT_SEL_1 : 6; /*!< For two 16 bit counters mode: Event select for interrupt event
                                                         from counter 1 For 32 bit counter mode : Invalid                      */
    } INTR_EVENT_SEL_b;                   /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t INTR_AND_EVENT; /*!< Interrupt AND Event Register                                          */

    struct {
      __IO uint32_t INTR_0_AND_EVENT : 4; /*!< None                                                                  */
      uint32_t : 4;
      __IO uint32_t INTR_0_AND_VLD : 4; /*!< None                                                                  */
      uint32_t : 4;
      __IO uint32_t INTR_1_AND_EVENT : 4; /*!< None                                                                  */
      uint32_t : 4;
      __IO uint32_t INTR_1_AND_VLD : 4; /*!< None                                                                  */
    } INTR_AND_EVENT_b;                 /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t INTR_OR_EVENT_REG; /*!< Interrupt OR Event Register                                           */

    struct {
      __IO uint32_t INTR_0_OR_EVENT : 4; /*!< None                                                                  */
      uint32_t : 4;
      __IO uint32_t INTR_0_OR_VLD : 4; /*!< None                                                                  */
      uint32_t : 4;
      __IO uint32_t INTR_1_OR_EVENT : 4; /*!< None                                                                  */
      uint32_t : 4;
      __IO uint32_t INTR_1_OR_VLD : 4; /*!< None                                                                  */
    } INTR_OR_EVENT_REG_b;             /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t RE_FE_RFE_LEV0_LEV1_EVENT_ENABLE_REG;

    struct {
      __IO uint32_t RE_EN : 4; /*!< None                                                                  */
      __IO uint32_t FE_EN : 4;
      __IO uint32_t RFE_EN : 4; /*!< None                                                                  */
      __IO uint32_t LEV0_EN : 4;
      __IO uint32_t LEV1_EN : 4; /*!< None                                                                  */
      uint32_t : 12;
    } RE_FE_RFE_LEV0_LEV1_EVENT_ENABLE_REG_b; /*!< BitSize                                                               */
  };

} SCT0_Type;

/* ================================================================================ */
/* ================                   SCT_MUX_REG                  ================ */
/* ================================================================================ */

/**
 * @brief Configurable timer is used in counting clocks, events and states with reference clock external clock and system clock  (SCT_MUX_REG)
 */

typedef struct { /*!< SCT_MUX_REG Structure                                                 */

  union {
    __IO uint32_t SCT_MUX_SEL_0_REG; /*!< MUX_SEL_0_REG Register                                                */

    struct {
      __IO uint32_t MUX_SEL_0 : 4; /*!< Select value to select first output value fifo_0_full[0] out
                                                         of all the fifo_0_full_muxed signals of counter 0                     */
    } SCT_MUX_SEL_0_REG_b;         /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t SCT_MUX_SEL_1_REG; /*!< MUX_SEL_1_REG Register                                                */

    struct {
      __IO uint32_t MUX_SEL_1 : 4; /*!< Select value to select first output value fifo_0_full[1] out
                                                         of all the fifo_0_full_muxed signals of counter 0                     */
    } SCT_MUX_SEL_1_REG_b;         /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t SCT_MUX_SEL_2_REG; /*!< MUX_SEL_2_REG Register                                                */

    struct {
      __IO uint32_t MUX_SEL_2 : 4; /*!< Select value to select first output value fifo_1_full[0] out
                                                         of all the fifo_1_full_muxed signals of counter 1                     */
    } SCT_MUX_SEL_2_REG_b;         /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t SCT_MUX_SEL_3_REG; /*!< MUX_SEL_3_REG Register                                                */

    struct {
      __IO uint32_t MUX_SEL_3 : 4; /*!< Select value to select first output value fifo_1_full[1] out
                                                         of all the fifo_1_full_muxed signals of counter 1                     */
    } SCT_MUX_SEL_3_REG_b;         /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED[2];

  union {
    __IO uint32_t OUTPUT_EVENT1_ADC_SEL; /*!< OUTPUT_EVENT_ADC_SEL Register                                         */

    struct {
      __IO uint32_t OUTPUT_EVENT_ADC_SEL : 4; /*!< Select signals to select one output event out of all the output
                                                         events output_event_0 output_event_1, output_event_2, output_event_3
                                                          to enable ADC module                                                 */
    } OUTPUT_EVENT1_ADC_SEL_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t OUTPUT_EVENT2_ADC_SEL; /*!< OUTPUT_EVENT_ADC_SEL Register                                         */

    struct {
      __IO uint32_t OUTPUT_EVENT_ADC_SEL : 4; /*!< Select signals to select one output event out of all the output
                                                         events output_event_0 output_event_1, output_event_2, output_event_3
                                                          to enable ADC module                                                 */
    } OUTPUT_EVENT2_ADC_SEL_b; /*!< BitSize                                                               */
  };
} SCT_MUX_REG_Type;

/* ================================================================================ */
/* ================                    ETHERNET                    ================ */
/* ================================================================================ */

/**
 * @brief Ether MAC 10/100/1000 Universal enables a host to transmit and receive data over Ethernet in compliance with the IEEE 802.3-2002standard (ETHERNET)
 */

typedef struct {
  union {
    __IO uint32_t MAC_ADDR_HIGH_REG; /*!< The MAC Address1 High register holds the upper 16 bits of the
                                                         6-byte second MAC address of thestation.                              */

    struct {
      __IO uint32_t MAC_ADDR1_HIGH : 16; /*!< This field contains the upper 16 bits (47:32) of the 6-byte
                                                         second MAC address.                                                   */
      uint32_t : 8;
      __IO uint32_t MBC_ADDR1_HIGH : 6; /*!< Mask Byte Control                                                     */
      __IO uint32_t SA : 1;             /*!< When this bit is set, the MAC Address1[47:0] is used to compare
                                                         with the SAfields of the received frame                               */
      __IO uint32_t AE : 1;             /*!< When this bit is set, the Address filter module uses the second
                                                         MAC address forperfect filtering                                      */
    } MAC_ADDR_HIGH_REG_b;              /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t MAC_ADDR_LOW_REG; /*!< The MAC Address1 Low register holds the lower 32 bits of the
                                                         6-byte second MAC address of the station                              */

    struct {
      __IO uint32_t MAC_ADDR1_LOW : 32; /*!< This field contains the upper 16 bits (47:32) of the 6-byte
                                                         second MAC address.                                                   */
    } MAC_ADDR_LOW_REG_b;               /*!< BitSize                                                               */
  };
} ETHERNET_MAC_16_ADDR_Type;

typedef struct {
  union {
    __IO uint32_t MAC_ADDR_HIGH_REG; /*!< The MAC Address1 High register holds the upper 16 bits of the
                                                         6-byte second MAC address of thestation.                              */

    struct {
      __IO uint32_t MAC_ADDR1_HIGH : 16; /*!< This field contains the upper 16 bits (47:32) of the 6-byte
                                                         second MAC address.                                                   */
      uint32_t : 8;
      __IO uint32_t MBC_ADDR1_HIGH : 6; /*!< Mask Byte Control                                                     */
      __IO uint32_t SA : 1;             /*!< When this bit is set, the MAC Address1[47:0] is used to compare
                                                         with the SAfields of the received frame                               */
      __IO uint32_t AE : 1;             /*!< When this bit is set, the Address filter module uses the second
                                                         MAC address forperfect filtering                                      */
    } MAC_ADDR_HIGH_REG_b;              /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t MAC_ADDR0_LOW_REG19; /*!< The MAC Address1 Low register holds the lower 32 bits of the
                                                         6-byte second MAC address of thestation                               */

    struct {
      __IO uint32_t MAC_ADDR1_LOW : 32; /*!< This field contains the upper 16 bits (47:32) of the 6-byte
                                                         second MAC address.                                                   */
    } MAC_ADDR0_LOW_REG19_b;            /*!< BitSize                                                               */
  };
} ETHERNET_MAC_31_ADDR_Type;

/* ================================================================================ */
/* ================                    ETHERNET                    ================ */
/* ================================================================================ */

/**
 * @brief Ether MAC 10/100/1000 Universal enables a host to transmit and receive data over Ethernet in compliance with the IEEE 802.3-2002standard (ETHERNET)
 */

typedef struct { /*!< ETHERNET Structure                                                    */

  union {
    __IO uint32_t MAC_CONFIG_REG0; /*!< This is the operation mode register for the MAC.                      */

    struct {
      uint32_t : 2;
      __IO uint32_t RE : 1;   /*!< Receiver Enable                                                       */
      __IO uint32_t TE : 1;   /*!< Transmitter Enable                                                    */
      __IO uint32_t DC : 1;   /*!< Deferral Check                                                        */
      __IO uint32_t BL : 2;   /*!< Back-Off Limit                                                        */
      __IO uint32_t ACS : 1;  /*!< Automatic Pad/CRC Stripping                                           */
      __IO uint32_t LUD : 1;  /*!< Link Up/Down                                                          */
      __IO uint32_t DR : 1;   /*!< Disable Retry                                                         */
      __IO uint32_t IPC : 1;  /*!< Checksum Offload                                                      */
      __IO uint32_t DM : 1;   /*!< Duplex Mode                                                           */
      __IO uint32_t LM : 1;   /*!< Loop-back Mode                                                        */
      __IO uint32_t DO : 1;   /*!< Disable Receive Own                                                   */
      __IO uint32_t FES : 1;  /*!< Indicates the speed in Fast Ethernet (MII) mode:                      */
      __IO uint32_t PS : 1;   /*!< Selects between GMII and MII:                                         */
      __IO uint32_t DCRS : 1; /*!< Disable Carrier Sense During Transmission                             */
      __IO uint32_t IFG : 3;  /*!< Inter-Frame Gap These bits control the minimum IFG between frames
                                                         during transmission                                                   */
      __IO uint32_t JE : 1;   /*!< Jumbo Frame Enable                                                    */
      __IO uint32_t BE : 1;   /*!< Frame Burst Enable                                                    */
      __IO uint32_t JD : 1;   /*!< Jabber Disable                                                        */
      __IO uint32_t WD : 1;   /*!< Watchdog Disable When this bit is set, the GMAC disables the
                                                         watchdog timer on the receiver, andcan receive frames of up
                                                          to 16,384 bytes                                                      */
      __IO uint32_t TC : 1;   /*!< Transmit Configuration in RGMII/SGMII                                 */
    } MAC_CONFIG_REG0_b;      /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t MAC_FRAME_FILTER_REG1; /*!< This is the operation mode register for the MAC.                      */

    struct {
      __IO uint32_t PR : 1;   /*!< Promiscuous Mode                                                      */
      __IO uint32_t HUC : 1;  /*!< Hash Unicast                                                          */
      __IO uint32_t HMC : 1;  /*!< Hash Multicast                                                        */
      __IO uint32_t DAIF : 1; /*!< DA Inverse Filtering                                                  */
      __IO uint32_t PM : 1;   /*!< Pass All Multicast                                                    */
      __IO uint32_t DBF : 1;  /*!< Disable Broadcast Frames                                              */
      __IO uint32_t PCF : 2;  /*!< Pass Control Frames                                                   */
      __IO uint32_t SAIF : 1; /*!< SA Inverse Filtering                                                  */
      __IO uint32_t SAF : 1;  /*!< Source Address Filter Enable                                          */
      __IO uint32_t HPF : 1;  /*!< Hash or Perfect Filter                                                */
      uint32_t : 20;
      __IO uint32_t RA : 1;    /*!< Receive All                                                           */
    } MAC_FRAME_FILTER_REG1_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t HASH_TABLE_HIGH_REG2; /*!< This is the operation mode register for the MAC.                      */

    struct {
      __IO uint32_t HTH : 32; /*!< Hash Table HighThis field contains the upper 32 bits of Hash
                                                         table.                                                                */
    } HASH_TABLE_HIGH_REG2_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t HASH_TABLE_LOW_REG3; /*!< This is the operation mode register for the MAC.                      */

    struct {
      __IO uint32_t HTL : 32; /*!< Hash Table LowThis field contains the lower 32 bits of Hash
                                                         table.                                                                */
    } HASH_TABLE_LOW_REG3_b;  /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t GMII_ADDR_REG4; /*!< This is the operation mode register for the MAC.                      */

    struct {
      __IO uint32_t GB : 1; /*!< GMII Busy                                                             */
      __IO uint32_t GW : 1; /*!< GMII Write. When set this bit tells the PHY that this will be
                                                         a Write operation using the GMIIData register                         */
      __IO uint32_t CR : 3; /*!< CSR Clock Range. The CSR Clock Range selection determines the
                                                         clk_csr_i frequency and isused to decide the frequency of the
                                                          MDC clock                                                            */
      uint32_t : 1;
      __IO uint32_t GR : 5; /*!< GMII RegisterThese bits select the desired GMII register in
                                                         the selected PHY device                                               */
      __IO uint32_t PR : 5; /*!< Physical Layer AddressThis field tells which of the 32 possible
                                                         PHY devices are being accessed                                        */
    } GMII_ADDR_REG4_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t GMII_DATA_REG5; /*!< This is the operation mode register for the MAC.                      */

    struct {
      __IO uint32_t GD : 16; /*!< GMII Data.This contains the 16-bit data value read from the
                                                         PHY after a Management Read operation or the 16-bit data value
                                                          to be written to the PHY before a ManagementWrite operation          */
    } GMII_DATA_REG5_b;      /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t FLOW_CTRL_REG6; /*!< The Flow Control register controls the generation and reception
                                                         of the Control (Pause Command)frames by the GMAC's Flow control
                                                          module.                                                              */

    struct {
      __IO uint32_t FCB_BPA : 1; /*!< Flow Control Busy/Back pressure Activate                              */
      __IO uint32_t TFE : 1;     /*!< Transmit Flow Control Enable                                          */
      __IO uint32_t RFE : 1;     /*!< Receive Flow Control Enable                                           */
      __IO uint32_t UP : 1;      /*!< Unicast Pause Frame Detect                                            */
      __IO uint32_t PLT : 2;     /*!< Pause Low Threshold                                                   */
      __IO uint32_t DZPQ : 1;    /*!< Disable Zero-Quanta Pause                                             */
      uint32_t : 9;
      __IO uint32_t PT : 16; /*!< Pause Time                                                            */
    } FLOW_CTRL_REG6_b;      /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t VLAN_TAG_REG7; /*!< The Flow Control register controls the generation and reception
                                                         of the Control (Pause Command)frames by the GMAC's Flow control
                                                          module.                                                              */

    struct {
      __IO uint32_t VL : 16; /*!< VLAN Tag Identifier.This contains the 802.1Q VLAN Tag to identify
                                                         the VLAN frames, and is used tocompare with the 15th and 16th
                                                          bytes of the receiving frames for VLAN frames                        */
    } VLAN_TAG_REG7_b;       /*!< BitSize                                                               */
  };
  __I uint32_t VERSION_REG8; /*!< The Version register s contents identify the version of the
                                                         core.                                                                 */
  __I uint32_t RESERVED;
  __IO uint32_t RMT_WKUP_FMFILTER_REG; /*!< This is the address through which the remote Wake-up Frame Filter
                                                         registers(wkupfmfilter_reg) are written/read by the Application.This
                                                          register contains the higher 16 bits of the 7th MAC address.
	 */

  union {
    __IO uint32_t PMT_CTRL_STS_REG; /*!< The PMT CSR program the request wake-up events and monitor the
                                                         wake-up events.                                                       */

    struct {
      __IO uint32_t PD : 1;  /*!< When set, all received frames will be dropped                         */
      __IO uint32_t MPE : 1; /*!< Magic Packet Enable.When set, enables generation of a power
                                                         management event due to MagicPacket reception                         */
      __IO uint32_t WFE : 1; /*!< Wake-Up Frame Enable.When set, enables generation of a power
                                                         management event due to wakeupframe reception                         */
      uint32_t : 2;
      __IO uint32_t MPR : 1; /*!< When set, this bit indicates the power management event was
                                                         generated bythe reception of a Magic Packet                           */
      __IO uint32_t WFR : 1; /*!< When set, this bit indicates the power management event was
                                                         generateddue to reception of a wake-up frame                          */
      uint32_t : 2;
      __IO uint32_t GU : 1; /*!< When set, enables any unicast packet filtered by the GMAC (DAF)
                                                         addressrecognition to be a wake-up frame                              */
      uint32_t : 21;
      __IO uint32_t WFFRPR : 1; /*!< When set, resets the Remote Wake-up Frame Filter register pointer
                                                         to000                                                                 */
    } PMT_CTRL_STS_REG_b;       /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED1[2];

  union {
    __I uint32_t INTR_STS_REG14; /*!< The Interrupt Status register contents identify the events in
                                                         the GMAC-CORE that can generateinterrupt                              */

    struct {
      __I uint32_t RGMII_INTR_STS : 1; /*!< This bit is set due to any change in value of the Link Status
                                                         of RGMII interface                                                    */
      __I uint32_t
        PCS_LINK_STATUS_CHANGED : 1; /*!< This bit is set due to any change in Link Status in the TBI/RTBI/SGMII
                                                         PHYinterface                                                          */
      __I uint32_t
        PCS_AUTO_NEGOTIATION_COMPLETE : 1; /*!< This bit is set when the Auto-negotiation is completed in the
                                                         TBI/RTBI/SGMII PHYinterface                                           */
      __I uint32_t PMT_INTR_STS : 1;       /*!< This bit is set when the Auto-negotiation is completed in the
                                                         TBI/RTBI/SGMII PHYinterface                                           */
      __I uint32_t MMC_INTR_STS : 1;       /*!< This bit is set high whenever any of bits 7:5 is set high and
                                                         cleared only when all ofthese bits are low.                           */
      __I uint32_t MMC_RX_INTR_STS : 1;    /*!< MMC Receive Interrupt Status                                          */
      __I uint32_t MMC_TX_INTR_STS : 1;    /*!< MMC Transmit Interrupt Status                                         */
      __I uint32_t
        MMC_RX_CH_OFF_INTR_STS : 1; /*!< MMC Receive Checksum Offload Interrupt Status                         */
    } INTR_STS_REG14_b;             /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t INTR_MASK_REG15; /*!< The Interrupt Mask Register bits enables the user to mask the
                                                         interrupt signal due to the correspondingevent in the Interrupt
                                                          Status Register                                                      */

    struct {
      __IO uint32_t RGMII_M : 1;             /*!< This bit when set, will disable the assertion of the interrupt
                                                         signal due to the settingof RGMII Interrupt Status bit in Register14. */
      __IO uint32_t PCS_LINK_M : 1;          /*!< This bit when set, will disable the assertion of the interrupt
                                                         signal due to the settingof RGMII Interrupt Status bit in Register14. */
      __IO uint32_t PCS_AN_COMPLETION_M : 1; /*!< This bit when set, will disable the assertion of the interrupt
                                                         signal due to the settingof RGMII Interrupt Status bit in Register14. */
      __IO uint32_t PMT_M : 1;               /*!< This bit when set, will disable the assertion of the interrupt
                                                         signal due to the settingof PMT Interrupt Status bit in Register14    */
    } INTR_STS_REG15_b; /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t MAC_ADDR0_HIGH_REG; /*!< The MAC Address0 High register holds the upper 16 bits of the
                                                         6-byte first MAC address of the station                               */

    struct {
      __IO uint32_t MAC_ADDR0_HIGH : 16; /*!< This field contains the upper 16 bits (47:32) of the 6-byte
                                                         first MAC address                                                     */
      uint32_t : 15;
      __I uint32_t MO : 1;  /*!< Always 1                                                              */
    } MAC_ADDR0_HIGH_REG_b; /*!< BitSize                                                               */
  };
  __IO uint32_t MAC_ADDR0_LOW_REG; /*!< The MAC Address0 Low register holds the lower 32 bits of the
                                                         6-byte first MAC address of the station                               */
  ETHERNET_MAC_16_ADDR_Type
    MAC_16_ADDR[15]; /*!< MAC_16_ADDR0                                                          */

  union {
    __IO uint32_t AN_CTRL_REG48; /*!< The AN Control register enables and/or restarts auto-negotiation.     */

    struct {
      uint32_t : 9;
      __IO uint32_t RAN : 1; /*!< Restart Auto-Negotiation                                              */
      uint32_t : 2;
      __IO uint32_t ANE : 1; /*!< Auto-Negotiation Enable                                               */
      uint32_t : 1;
      __IO uint32_t ELE : 1; /*!< External Loopback Enable                                              */
      uint32_t : 1;
      __IO uint32_t ECD : 1;       /*!< Enable Comma Detect                                                   */
      __IO uint32_t LR : 1;        /*!< Enable Comma Detect                                                   */
      __IO uint32_t SGMII_RAL : 1; /*!< SGMII RAL Control                                                     */
    } AN_CTRL_REG48_b;             /*!< BitSize                                                               */
  };

  union {
    __I uint32_t AN_CTRL_REG49; /*!< The AN Status register indicates the link and the auto-negotiation
                                                         status                                                                */

    struct {
      uint32_t : 2;
      __I uint32_t LS : 1;  /*!< Link Status                                                           */
      __I uint32_t ANA : 1; /*!< Auto-Negotiation Ability                                              */
      uint32_t : 1;
      __I uint32_t ANC : 1; /*!< Auto-Negotiation Complete                                             */
      uint32_t : 2;
      __I uint32_t ES : 1; /*!< Extended Status                                                       */
    } AN_CTRL_REG49_b;     /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t ANA_ADV_REG50; /*!< The Auto-Negotiation Advertisement register indicates the link
                                                         and the auto-negotiation status.                                      */

    struct {
      uint32_t : 5;
      __IO uint32_t FD : 1;  /*!< Full-Duplex.This bit, when set high, indicates that the GMAC
                                                         supports Full-Duplex.                                                 */
      __IO uint32_t HD : 1;  /*!< Half-Duplex                                                           */
      __IO uint32_t PSE : 2; /*!< Pause Encoding                                                        */
      uint32_t : 3;
      __IO uint32_t RFE : 2; /*!< Remote Fault Encoding                                                 */
      uint32_t : 1;
      __I uint32_t NP : 1; /*!< Next Page Support                                                     */
    } ANA_ADV_REG50_b;     /*!< BitSize                                                               */
  };

  union {
    __I uint32_t ANL_PA_REG51; /*!< The Auto-Negotiation Link Partner Ability register contains
                                                         the advertised ability of the link partner                            */

    struct {
      uint32_t : 5;
      __I uint32_t FD : 1;   /*!< Full-Duplex.This bit, when set high, indicates that the GMAC
                                                         supports Full-Duplex.                                                 */
      __IO uint32_t HD : 1;  /*!< Half-Duplex                                                           */
      __IO uint32_t PSE : 2; /*!< Pause Encoding                                                        */
      uint32_t : 3;
      __IO uint32_t RFE : 2; /*!< Remote Fault Encoding                                                 */
      __IO uint32_t ACK : 1; /*!< Acknowledge                                                           */
      __I uint32_t NP : 1;   /*!< Next Page Support                                                     */
    } ANL_PA_REG51_b;        /*!< BitSize                                                               */
  };

  union {
    __I uint32_t ANE_REG52; /*!< The Auto-Negotiation Expansion register indicates whether a
                                                         new base page from the link partner hasbeen received                  */

    struct {
      uint32_t : 1;
      __I uint32_t NPR : 1; /*!< New Page Received                                                     */
      __I uint32_t NPA : 1; /*!< Next Page Ability                                                     */
    } ANE_REG52_b;          /*!< BitSize                                                               */
  };

  union {
    __I uint32_t TBI_EXT_STS_REG53; /*!< The TBI Extended Status Register register indicates all modes
                                                         of operation of the GMAC.                                             */

    struct {
      uint32_t : 14;
      __I uint32_t GHD : 1; /*!< 1000BASE-X Half-Duplex Capable                                        */
      __I uint32_t GFD : 1; /*!< 1000BASE-X Full-Duplex Capable                                        */
    } TBI_EXT_STS_REG53_b;  /*!< BitSize                                                               */
  };

  union {
    __I uint32_t TBI_EXT_STS_REG54; /*!< The SGMII/RGMII Status register indicates the status signals
                                                         received by the SGMII/RGMII from the PHY                              */

    struct {
      __I uint32_t LINK_MODE : 1;   /*!< Indicates the current mode of operation of the link                   */
      __I uint32_t LINK_SPEED : 2;  /*!< Indicates the current speed of the link:                              */
      __I uint32_t LINK_STATUS : 1; /*!< Indicates whether the link is up 1 or down 0.                         */
    } TBI_EXT_STS_REG54_b;          /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED2[9];

  union {
    __IO uint32_t MMC_CTRL_REG; /*!< The MMC Control register establishes the operating mode of the
                                                         management counters.                                                  */

    struct {
      __IO uint32_t MMC_CR : 1;  /*!< Counters Reset.When set, all counters will be reset. This bit
                                                         will be cleared automaticallyafter 1 clock cycle                      */
      __IO uint32_t MMC_CSR : 1; /*!< Counter Stop Roll over.When set, counter after reaching maximum
                                                         value will not roll over to zero.                                     */
      __IO uint32_t MMC_RR : 1;  /*!< Reset on Read. When set, counter after reaching maximum value
                                                         will not roll over to zero.                                           */
      __IO uint32_t MMC_CF : 1;  /*!< MMC Counter Freeze.When set, this bit freezes all the MMC counters
                                                         to their current value.                                               */
    } MMC_CTRL_REG_b;            /*!< BitSize                                                               */
  };

  union {
    __I uint32_t MMC_RX_INTR_REG; /*!< The MMC Receive Interrupt register maintains the interrupts
                                                         generated when receive statistic countersreach half their maximum
                                                          values                                                               */

    struct {
      __I uint32_t BIT_0 : 1;  /*!< The bit is set when the rx framecount_gb counter reaches half
                                                         themaximum value                                                      */
      __I uint32_t BIT_1 : 1;  /*!< The bit is set when the rxoctetcount_gb counter reaches half
                                                         the maximumvalue.                                                     */
      __I uint32_t BIT_2 : 1;  /*!< The bit is set when the rxoctetcount_g counter reaches half
                                                         the maximumvalue.                                                     */
      __I uint32_t BIT_3 : 1;  /*!< The bit is set when the rxbroadcastframes_g counter reaches
                                                         half themaximum value                                                 */
      __I uint32_t BIT_4 : 1;  /*!< The bit is set when the rxmulticastframes_g counter reaches
                                                         half themaximum value.                                                */
      __I uint32_t BIT_5 : 1;  /*!< The bit is set when the rxcrcerror counter reaches half the
                                                         maximumvalue.                                                         */
      __I uint32_t BIT_6 : 1;  /*!< The bit is set when the rxalignmenterror counter reaches half
                                                         themaximum value                                                      */
      __I uint32_t BIT_7 : 1;  /*!< The bit is set when the rxrunterror counter reaches half the
                                                         maximumvalue.                                                         */
      __I uint32_t BIT_8 : 1;  /*!< The bit is set when the rxjabbererror counter reaches half the
                                                         maximumvalue.                                                         */
      __I uint32_t BIT_9 : 1;  /*!< The bit is set when the rxundersize_g counter reaches half the
                                                         maximumvalue.                                                         */
      __I uint32_t BIT_10 : 1; /*!< The bit is set when the rxoversize_g counter reaches half the
                                                         maximumvalue.                                                         */
      __I uint32_t BIT_11 : 1; /*!< The bit is set when the rx64octets_gb counter reaches half the
                                                         maximumvalue.                                                         */
      __I uint32_t BIT_12 : 1; /*!< The bit is set when the rx65to127octets_gb counter reaches half
                                                         themaximum value.                                                     */
      __I uint32_t BIT_13 : 1; /*!< The bit is set when the rx128to255octets_gb counter reaches
                                                         half themaximum value.                                                */
      __I uint32_t BIT_14 : 1; /*!< The bit is set when the rx256to511octets_gb counter reaches
                                                         half themaximum value                                                 */
      __I uint32_t BIT_15 : 1; /*!< The bit is set when the rx512to1023octets_gb counter reaches
                                                         half themaximum value.                                                */
      __I uint32_t BIT_16 : 1; /*!< The bit is set when the rx1024tomaxoctets_gb counter reaches
                                                         half themaximum value.                                                */
      __I uint32_t BIT_17 : 1; /*!< The bit is set when the rxunicastframes_gb counter reaches half
                                                         themaximum value.                                                     */
      __I uint32_t BIT_18 : 1; /*!< The bit is set when the rx length error counter reaches half
                                                         the maximumvalue.                                                     */
      __I uint32_t BIT_19 : 1; /*!< The bit is set when the rx out of range type counter reaches
                                                         half themaximum value                                                 */
      __I uint32_t BIT_20 : 1; /*!< The bit is set when the rx pause frames counter reaches half
                                                         the maximum value                                                     */
      __I uint32_t BIT_21 : 1; /*!< The bit is set when the rx fifo overflow counter reaches half
                                                         the maximumvalue.                                                     */
      __I uint32_t BIT_22 : 1; /*!< The bit is set when the rxvlanframes_gb counter reaches half
                                                         themaximum value.                                                     */
      __I uint32_t BIT_23 : 1; /*!< The bit is set when the rx watchdog error counter reaches half
                                                         themaximum value.                                                     */
    } MMC_RX_INTR_REG_b;       /*!< BitSize                                                               */
  };

  union {
    __I uint32_t MMC_TX_INTR_REG; /*!< The MMC Transmit Interrupt register maintains the interrupts
                                                         generated when transmit statisticcounters reach half their maximum
                                                          values                                                               */

    struct {
      __I uint32_t BIT_0 : 1;  /*!< The bit is set when the txoctetcount_gb counter reaches half
                                                         the maximum value.                                                    */
      __I uint32_t BIT_1 : 1;  /*!< The bit is set when the txframecount_gb counter reaches half
                                                         the maximum value.                                                    */
      __I uint32_t BIT_2 : 1;  /*!< The bit is set when the txbroadcastframes_g counter reaches
                                                         half the maximum value.                                               */
      __I uint32_t BIT_3 : 1;  /*!< The bit is set when the txmulticastframes_g counter reaches
                                                         half the maximum value.                                               */
      __I uint32_t BIT_4 : 1;  /*!< The bit is set when the tx64to127octets_gb counter reaches half
                                                         the maximum value.                                                    */
      __I uint32_t BIT_5 : 1;  /*!< The bit is set when the tx65to127octets_gb counter reaches half
                                                         the maximum value                                                     */
      __I uint32_t BIT_6 : 1;  /*!< The bit is set when the tx128to255octets_gb counter reaches
                                                         half the maximum value.                                               */
      __I uint32_t BIT_7 : 1;  /*!< The bit is set when the tx256to511octets_gb counter reaches
                                                         half the maximum value.                                               */
      __I uint32_t BIT_8 : 1;  /*!< The bit is set when the tx512to1023octets_gb counter reaches
                                                         half the maximum value                                                */
      __I uint32_t BIT_9 : 1;  /*!< The bit is set when the tx1024tomaxoctets_gb counter reaches
                                                         half the maximum value                                                */
      __I uint32_t BIT_10 : 1; /*!< The bit is set when the txunicastframes_gb counter reaches half
                                                         the maximum value.                                                    */
      __I uint32_t BIT_11 : 1; /*!< The bit is set when the txmulticastframes_gb counter reaches
                                                         half the maximum value.                                               */
      __I uint32_t BIT_12 : 1; /*!< The bit is set when the txbroadcastframes_gb counter reaches
                                                         half the maximum value                                                */
      __I uint32_t BIT_13 : 1; /*!< The bit is set when the txunderflowerror counter reaches half
                                                         the maximum value.                                                    */
      __I uint32_t BIT_14 : 1; /*!< The bit is set when the txsinglecol_g counter reaches half the
                                                         maximum value                                                         */
      __I uint32_t BIT_15 : 1; /*!< The bit is set when the txmulticol_g counter reaches half the
                                                         maximum value                                                         */
      __I uint32_t BIT_16 : 1; /*!< The bit is set when the txdeferred counter reaches half the
                                                         maximum value                                                         */
      __I uint32_t BIT_17 : 1; /*!< The bit is set when the txlatecol counter reaches half the maximum
                                                         value                                                                 */
      __I uint32_t BIT_18 : 1; /*!< The bit is set when the txexesscol counter reaches half the
                                                         maximum value.                                                        */
      __I uint32_t BIT_19 : 1; /*!< The bit is set when the txcarriererror counter reaches half
                                                         the maximum value                                                     */
      __I uint32_t BIT_20 : 1; /*!< The bit is set when the txoctetcount_g counter reaches half
                                                         the maximum value                                                     */
      __I uint32_t BIT_21 : 1; /*!< The bit is set when the txframecount_g counter reaches half
                                                         the maximum value.                                                    */
      __I uint32_t BIT_22 : 1; /*!< The bit is set when the txoexcessdef counter reaches half the
                                                         maximum value                                                         */
      __I uint32_t BIT_23 : 1; /*!< The bit is set when the tx pause frames error counter reaches
                                                         half the maximum value.                                               */
      __I uint32_t BIT_24 : 1; /*!< The bit is set when the txvlanframes_g counter reaches half
                                                         the maximum value                                                     */
    } MMC_TX_INTR_REG_b;       /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t MMC_RX_INTR_MASK_REG; /*!< MMC Receive Interrupt Mask register maintains the masks for
                                                         the interrupts generated whenreceive statistic counters reach
                                                          half their maximum value                                             */

    struct {
      __I uint32_t BIT_0 : 1;  /*!< Setting this bit masks the interrupt when the rxframecount_gb
                                                         counter reaches halfthe maximum value                                 */
      __I uint32_t BIT_1 : 1;  /*!< Setting this bit masks the interrupt when the rxoctetcount_gb
                                                         counter reaches halfthe maximum value                                 */
      __I uint32_t BIT_2 : 1;  /*!< Setting this bit masks the interrupt when the rxoctetcount_g
                                                         counter reaches half the maximumvalue.                                */
      __I uint32_t BIT_3 : 1;  /*!< Setting this bit masks the interrupt when the rxbroadcastframes_g
                                                         counter reaches half themaximum value                                 */
      __I uint32_t BIT_4 : 1;  /*!< The bit is set when the rxmulticastframes_g counter reaches
                                                         half themaximum value.                                                */
      __I uint32_t BIT_5 : 1;  /*!< Setting this bit masks the interrupt when the rxcrcerror counter
                                                         reaches half the maximumvalue.                                        */
      __I uint32_t BIT_6 : 1;  /*!< The bit is set when the rxalignmenterror counter reaches half
                                                         themaximum value                                                      */
      __I uint32_t BIT_7 : 1;  /*!< Setting this bit masks the interrupt when the rxrunterror counter
                                                         reaches half the maximumvalue.                                        */
      __I uint32_t BIT_8 : 1;  /*!< Setting this bit masks the interrupt when the rxjabbererror
                                                         counter reaches half the maximumvalue.                                */
      __I uint32_t BIT_9 : 1;  /*!< Setting this bit masks the interrupt when the rxundersize_g
                                                         counter reaches half the maximumvalue.                                */
      __I uint32_t BIT_10 : 1; /*!< The bit is set when the rxoversize_g counter reaches half the
                                                         maximumvalue.                                                         */
      __I uint32_t BIT_11 : 1; /*!< Setting this bit masks the interrupt when the rx64octets_gb
                                                         counter reaches half the maximumvalue.                                */
      __I uint32_t BIT_12 : 1; /*!< Setting this bit masks the interruptwhen the rx65to127octets_gb
                                                         counter reaches half themaximum value.                                */
      __I uint32_t BIT_13 : 1; /*!< Setting this bit masks the interrupt when the rx128to255octets_gb
                                                         counter reaches half themaximum value.                                */
      __I uint32_t BIT_14 : 1; /*!< Setting this bit masks the interrupt when the rx256to511octets_gb
                                                         counter reaches half themaximum value                                 */
      __I uint32_t BIT_15 : 1; /*!< Setting this bit masks the interrupt when the rx512to1023octets_gb
                                                         counter reaches half themaximum value.                                */
      __I uint32_t BIT_16 : 1; /*!< Setting this bit masks the interrupt when the rx1024tomaxoctets_gb
                                                         counter reaches half themaximum value.                                */
      __I uint32_t BIT_17 : 1; /*!< Setting this bit masks the interrupt when the rxunicastframes_gb
                                                         counter reaches half themaximum value.                                */
      __I uint32_t BIT_18 : 1; /*!< Setting this bit masks the interrupt when the rx length error
                                                         counter reaches half the maximumvalue.                                */
      __I uint32_t BIT_19 : 1; /*!< Setting this bit masks the interrupt when the rx out of range
                                                         type counter reaches half themaximum value                            */
      __I uint32_t BIT_20 : 1; /*!< Setting this bit masks the interrupt when the rxpauseframes
                                                         counter reaches halfthe maximum value                                 */
      __I uint32_t BIT_21 : 1; /*!< Setting this bit masks the interrupt when the rxfifooverflow
                                                         counter reaches half themaximum value                                 */
      __I uint32_t BIT_22 : 1; /*!< Setting this bit masks the interrupt when the rxvlanframes_gb
                                                         counter reaches halfthe maximum value                                 */
      __I uint32_t BIT_23 : 1; /*!< Setting this bit masks the interrupt when the rxframecount_gb
                                                         counter reaches halfthe maximum value                                 */
    } MMC_RX_INTR_MASK_REG_b;  /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t MMC_TX_INTR_MASK_REG; /*!< The MMC Transmit Interrupt Mask register maintains the masks
                                                         for the interrupts generated whentransmit statistic counters
                                                          reach half their maximum value                                       */

    struct {
      __I uint32_t BIT_0 : 1;  /*!< Setting this bit masks the interrupt when the txoctetcount_gb
                                                         counter reaches halfthe maximum value                                 */
      __I uint32_t BIT_1 : 1;  /*!< Setting this bit masks the interrupt when the txframecount_gb
                                                         counter reaches halfthe maximum value                                 */
      __I uint32_t BIT_2 : 1;  /*!< Setting this bit masks the interrupt when the txbroadcastframes_g
                                                         counter reaches half the maximum value.                               */
      __I uint32_t BIT_3 : 1;  /*!< Setting this bit masks the interrupt when the txmulticastframes_g
                                                         counter reaches half the maximum value.                               */
      __I uint32_t BIT_4 : 1;  /*!< Setting this bit masks the interrupt when the tx64to127octets_gb
                                                         counter reaches half the maximum value.                               */
      __I uint32_t BIT_5 : 1;  /*!< Setting this bit masks the interrupt when the tx65to127octets_gb
                                                         counter reaches half the maximum value                                */
      __I uint32_t BIT_6 : 1;  /*!< Setting this bit masks the interrupt when the tx128to255octets_gb
                                                         counter reaches half the maximum value.                               */
      __I uint32_t BIT_7 : 1;  /*!< Setting this bit masks the interrupt when the tx256to511octets_gb
                                                         counter reaches half the maximum value.                               */
      __I uint32_t BIT_8 : 1;  /*!< Setting this bit masks the interrupt when the tx512to1023octets_gb
                                                         counter reaches half the maximum value                                */
      __I uint32_t BIT_9 : 1;  /*!< Setting this bit masks the interrupt when the tx1024tomaxoctets_gb
                                                         counter reaches half the maximum value                                */
      __I uint32_t BIT_10 : 1; /*!< Setting this bit masks the interrupt when the txunicastframes_gb
                                                         counter reaches half the maximum value.                               */
      __I uint32_t BIT_11 : 1; /*!< Setting this bit masks the interrupt when the txmulticastframes_gb
                                                         counter reaches half the maximum value.                               */
      __I uint32_t BIT_12 : 1; /*!< Setting this bit masks the interrupt when the txbroadcastframes_gb
                                                         counter reaches half the maximum value                                */
      __I uint32_t BIT_13 : 1; /*!< Setting this bit masks the interrupt when the txunderflowerror
                                                         counter reaches half the maximum value.                               */
      __I uint32_t BIT_14 : 1; /*!< Setting this bit masks the interrupt when the txsinglecol_g
                                                         counter reaches half the maximum value                                */
      __I uint32_t BIT_15 : 1; /*!< Setting this bit masks the interrupt when the txmulticol_g counter
                                                         reaches half the maximum value                                        */
      __I uint32_t BIT_16 : 1; /*!< Setting this bit masks the interrupt when the txdeferred counter
                                                         reaches half the maximum value                                        */
      __I uint32_t BIT_17 : 1; /*!< Setting this bit masks the interrupt when the txlatecol counter
                                                         reaches half the maximum value                                        */
      __I uint32_t BIT_18 : 1; /*!< Setting this bit masks the interrupt when the txexesscol counter
                                                         reaches half the maximum value.                                       */
      __I uint32_t BIT_19 : 1; /*!< Setting this bit masks the interrupt when the txcarriererror
                                                         counter reaches half the maximum value                                */
      __I uint32_t BIT_20 : 1; /*!< Setting this bit masks the interrupt when the txoctetcount_g
                                                         counter reaches half the maximum value                                */
      __I uint32_t BIT_21 : 1; /*!< Setting this bit masks the interrupt when the txframecount_g
                                                         counter reaches halfthe maximum value.                                */
      __I uint32_t BIT_22 : 1; /*!< Setting this bit masks the interrupt when the txoexcessdef counter
                                                         reaches half themaximum value.                                        */
      __I uint32_t BIT_23 : 1; /*!< Setting this bit masks the interrupt when the txpauseframes
                                                         counter reaches halfthe maximum value.                                */
      __I uint32_t BIT_24 : 1; /*!< Setting this bit masks the interrupt when the txvlanframes_g
                                                         counter reaches halfthe maximum value.                                */
    } MMC_TX_INTR_MASK_REG_b;  /*!< BitSize                                                               */
  };

  __I uint32_t RESERVED3[59];

  union {
    __IO uint32_t MMC_RX_CHOFF_INTR_MASK_REG; /*!< The MMC Receive Checksum Offload Interrupt Mask register maintains
                                                         the masks for the interrupts generated when the receive IPC
                                                          (Checksum Offload) statistic counters reach half their maximum
                                                          value                                                                */

    struct {
      __IO uint32_t BIT_0 : 1;  /*!< Setting this bit masks the interrupt when the rxipv4_gd_frms
                                                         counter reaches halfthe maximum value.                                */
      __IO uint32_t BIT_1 : 1;  /*!< Setting this bit masks the interrupt when the rxipv4_gd_frms
                                                         counter reaches halfthe maximum value.                                */
      __IO uint32_t BIT_2 : 1;  /*!< Setting this bit masks the interrupt when the rxipv4_gd_frms
                                                         counter reaches halfthe maximum value.                                */
      __IO uint32_t BIT_3 : 1;  /*!< Setting this bit masks the interrupt when the rxipv4_frag_frms
                                                         counter reaches halfthe maximum value                                 */
      __IO uint32_t BIT_4 : 1;  /*!< Setting this bit masks the interrupt when the rxipv4_udsbl_frms
                                                         counter reacheshalf the maximum value                                 */
      __IO uint32_t BIT_5 : 1;  /*!< Setting this bit masks the interrupt when the rxipv6_gd_frms
                                                         counter reaches halfthe maximum value                                 */
      __IO uint32_t BIT_6 : 1;  /*!< Setting this bit masks the interrupt when the rxipv6_hdrerr_frms
                                                         counter reacheshalf the maximum value                                 */
      __IO uint32_t BIT_7 : 1;  /*!< Setting this bit masks the interrupt when the rxipv6_nopay_frms
                                                         counter reacheshalf the maximum value.                                */
      __IO uint32_t BIT_8 : 1;  /*!< Setting this bit masks the interrupt when the rxudp_gd_frms
                                                         counter reaches halfthe maximum value                                 */
      __IO uint32_t BIT_9 : 1;  /*!< Setting this bit masks the interrupt when the rxudp_err_frms
                                                         counter reaches halfthe maximum value                                 */
      __IO uint32_t BIT_10 : 1; /*!< Setting this bit masks the interrupt when the rxtcp_gd_frms
                                                         counter reaches halfthe maximum value                                 */
      __IO uint32_t BIT_11 : 1; /*!< Setting this bit masks the interrupt when the rxtcp_err_frms
                                                         counter reaches halfthe maximum value                                 */
      __IO uint32_t BIT_12 : 1; /*!< Setting this bit masks the interrupt when the rxicmp_gd_frms
                                                         counter reaches halfthe maximum value                                 */
      __IO uint32_t BIT_13 : 1; /*!< Setting this bit masks the interrupt when the rxicmp_err_frms
                                                         counter reaches halfthe maximum value.                                */
      uint32_t : 2;
      __IO uint32_t BIT_16 : 1;     /*!< Setting this bit masks the interrupt when the rxipv4_gd_octets
                                                         counter reaches halfthe maximum value                                 */
      __IO uint32_t BIT_17 : 1;     /*!< Setting this bit masks the interrupt when the rxipv4_hdrerr_octets
                                                         counter reacheshalf the maximum value.                                */
      __IO uint32_t BIT_18 : 1;     /*!< Setting this bit masks the interrupt when the rxipv4_nopay_octets
                                                         counter reacheshalf the maximum value.                                */
      __IO uint32_t BIT_19 : 1;     /*!< Setting this bit masks the interrupt when the rxipv4_frag_octets
                                                         counter reacheshalf the maximum value                                 */
      __IO uint32_t BIT_20 : 1;     /*!< Setting this bit masks the interrupt when the rxipv4_udsbl_octets
                                                         counter reacheshalf the maximum value.                                */
      __IO uint32_t BIT_21 : 1;     /*!< Setting this bit masks the interrupt when the rxipv6_gd_octets
                                                         counter reaches halfthe maximum value.                                */
      __IO uint32_t BIT_22 : 1;     /*!< Setting this bit masks the interrupt when the rxipv6_hdrerr_octets
                                                         counter reacheshalf the maximum value.                                */
      __IO uint32_t BIT_23 : 1;     /*!< Setting this bit masks the interrupt when the rxipv6_nopay_octets
                                                         counter reacheshalf the maximum value                                 */
      __IO uint32_t BIT_24 : 1;     /*!< Setting this bit masks the interrupt when the rxudp_gd_octets
                                                         counter reaches halfthe maximum value.                                */
      __IO uint32_t BIT_25 : 1;     /*!< Setting this bit masks the interrupt when the rxudp_err_octets
                                                         counter reaches halfthe maximum value                                 */
      __IO uint32_t BIT_26 : 1;     /*!< Setting this bit masks the interrupt when the rxtcp_gd_octets
                                                         counter reaches halfthe maximum value.                                */
      __IO uint32_t BIT_27 : 1;     /*!< Setting this bit masks the interrupt when the rxtcp_err_octets
                                                         counter reaches halfthe maximum value.                                */
      __IO uint32_t BIT_28 : 1;     /*!< Setting this bit masks the interrupt when the rxicmp_gd_octets
                                                         counter reacheshalf the maximum value                                 */
      __IO uint32_t BIT_29 : 1;     /*!< Setting this bit masks the interrupt when the rxicmp_err_octets
                                                         counter reacheshalf the maximum value.                                */
    } MMC_RX_CHOFF_INTR_MASK_REG_b; /*!< BitSize                                                               */
  };

  __I uint32_t RESERVED4;
  union {
    __I uint32_t MMC_RX_CHOFF_INTR_REG; /*!< The MMC Receive Checksum Offload Interrupt register maintains
                                                         the interrupts generated whenreceive IPC statistic counters
                                                          reach half their maximum values                                      */

    struct {
      __I uint32_t BIT_0 : 1;  /*!< The bit is set when the rxipv4_gd_frms counter reaches half
                                                         the maximum value                                                     */
      __I uint32_t BIT_1 : 1;  /*!< The bit is set when the rxipv4_hdrerr_frms counter reaches half
                                                         the maximumvalue.                                                     */
      __I uint32_t BIT_2 : 1;  /*!< The bit is set when the rxipv4_nopay_frms counter reaches half
                                                         the maximumvalue.                                                     */
      __IO uint32_t BIT_3 : 1; /*!< The bit is set when the rxipv4_frag_frms counter reaches half
                                                         the maximumvalue                                                      */
      __I uint32_t BIT_4 : 1;  /*!< The bit is set when the rxipv4_udsbl_frms counter reaches half
                                                         the maximumvalue                                                      */
      __I uint32_t BIT_5 : 1;  /*!< The bit is set when the rxipv6_gd_frms counter reaches half
                                                         the maximum value                                                     */
      __I uint32_t BIT_6 : 1;  /*!< The bit is set when the rxipv6_hdrerr_frms counter reaches half
                                                         the maximumvalue.                                                     */
      __I uint32_t BIT_7 : 1;  /*!< The bit is set when the rxipv6_nopay_frms counter reaches half
                                                         the maximumvalue                                                      */
      __I uint32_t BIT_8 : 1;  /*!< The bit is set when the rxudp_gd_frms counter reaches half the
                                                         maximum value                                                         */
      __I uint32_t BIT_9 : 1;  /*!< The bit is set when the rxudp_err_frms counter reaches half
                                                         the maximum value                                                     */
      __I uint32_t BIT_10 : 1; /*!< The bit is set when the rxtcp_gd_frms counter reaches half the
                                                         maximum value                                                         */
      __I uint32_t BIT_11 : 1; /*!< The bit is set when the rxtcp_err_frms counter reaches half
                                                         the maximum value.                                                    */
      __I uint32_t BIT_12 : 1; /*!< The bit is set when the rxicmp_gd_frms counter reaches half
                                                         the maximumvalue.                                                     */
      __I uint32_t BIT_13 : 1; /*!< The bit is set when the rxicmp_err_frms counter reaches half
                                                         the maximumvalue.                                                     */
      uint32_t : 2;
      __I uint32_t BIT_16 : 1; /*!< The bit is set when the rxipv4_gd_octets counter reaches half
                                                         the maximumvalue                                                      */
      __I uint32_t BIT_17 : 1; /*!< The bit is set when the rxipv4_hdrerr_octets counter reaches
                                                         half the maximumvalue.                                                */
      __I uint32_t BIT_18 : 1; /*!< The bit is set when the rxipv4_nopay_octets counter reaches
                                                         half the maximumvalue                                                 */
      __I uint32_t BIT_19 : 1; /*!< The bit is set when the rxipv4_frag_octets counter reaches half
                                                         the maximumvalue.                                                     */
      __I uint32_t BIT_20 : 1; /*!< The bit is set when the rxipv4_udsbl_octets counter reaches
                                                         half the maximumvalue.                                                */
      __I uint32_t BIT_21 : 1; /*!< The bit is set when the rxipv6_gd_octets counter reaches half
                                                         the maximumvalue.                                                     */
      __I uint32_t BIT_22 : 1; /*!< The bit is set when the rxipv6_hdrerr_octets counter reaches
                                                         half the maximumvalue.                                                */
      __I uint32_t BIT_23 : 1; /*!< The bit is set when the rxipv6_nopay_octets counter reaches
                                                         half the maximumvalue.                                                */
      __I uint32_t BIT_24 : 1; /*!< The bit is set when the rxudp_gd_octets counter reaches half
                                                         the maximumvalue.                                                     */
      __I uint32_t BIT_25 : 1; /*!< The bit is set when the rxudp_err_octets counter reaches half
                                                         the maximumvalue                                                      */
      __I uint32_t BIT_26 : 1; /*!< The bit is set when the rxtcp_gd_octets counter reaches half
                                                         the maximumvalue.                                                     */
      __I uint32_t BIT_27 : 1; /*!< The bit is set when the rxtcp_err_octets counter reaches half
                                                         the maximumvalue.                                                     */
      __I uint32_t BIT_28 : 1; /*!< The bit is set when the rxicmp_gd_octets counter reaches half
                                                         the maximumvalue                                                      */
      __I uint32_t BIT_29 : 1; /*!< The bit is set when the rxicmp_err_octets counter reaches half
                                                         the maximumvalue.                                                     */
    } MMC_RX_CHOFF_INTR_REG_b; /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED5[382];
  ETHERNET_MAC_31_ADDR_Type
    MAC_31_ADDR[15]; /*!< MAC_31_ADDR0                                                          */
  __I uint32_t RESERVED6[481];

  union {
    __IO uint32_t BUS_MODE_REG0; /*!< The Bus Mode register establishes the bus operating modes for
                                                         the DMA.                                                              */

    struct {
      __IO uint32_t SWR : 1; /*!< Software Reset                                                        */
      __IO uint32_t DA : 1;  /*!< DMA Arbitration scheme                                                */
      __IO uint32_t DSL : 5; /*!< Descriptor Skip Length                                                */
      uint32_t : 1;
      __IO uint32_t PBL : 6;      /*!< Programmable Burst Length.These bits indicate the maximum number
                                                         of beats to be transferred in one DMAtransaction                      */
      __IO uint32_t PR : 2;       /*!< Rx:Tx priority ratio                                                  */
      __IO uint32_t FB : 1;       /*!< Fixed Burst                                                           */
      __IO uint32_t RPBL : 6;     /*!< RxDMA PBL                                                             */
      __IO uint32_t USP : 1;      /*!< Use Separate PBL                                                      */
      __IO uint32_t PBL_MODE : 1; /*!< When set high, this bit multiplies the PBL value programmed
                                                         four times                                                            */
      __IO uint32_t AAL : 1;      /*!< Address-Aligned Beats                                                 */
    } BUS_MODE_REG0_b;            /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t TX_POLL_DEMAND_REG1; /*!< The Transmit Poll Demand register enables the Transmit DMA to
                                                         check whether or not the currentdescriptor is owned by DMA            */

    struct {
      __IO uint32_t TPD : 32; /*!< Transmit Poll Demand                                                  */
    } TX_POLL_DEMAND_REG1_b;  /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t RX_POLL_DEMAND_REG; /*!< The Receive Poll Demand register enables the receive DMA to
                                                         check for new descriptors                                             */

    struct {
      __IO uint32_t RPD : 32; /*!< Receive Poll Demand                                                   */
    } RX_POLL_DEMAND_REG_b;   /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t RC_DESC_LIST_ADDR_REG3; /*!< The Receive Descriptor List Address register points to the start
                                                         of the Receive Descriptor List                                        */

    struct {
      __I uint32_t START_RX_LIST : 32; /*!< This field contains the base address of the First Descriptor
                                                         in the Receive Descriptor list.                                       */
    } RC_DESC_LIST_ADDR_REG3_b;        /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t TX_DESC_LIST_ADDR_REG4; /*!< The Receive Descriptor List Address register points to the start
                                                         of the Receive Descriptor List                                        */

    struct {
      __I uint32_t START_TX_LIST : 32; /*!< This field contains the base address of the First Descriptor
                                                         in the Transmit Descriptor list.                                      */
    } TX_DESC_LIST_ADDR_REG4_b;        /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t STATUS_REG5; /*!< The Status register contains all the status bits that the DMA
                                                         reports to the host                                                   */

    struct {
      __IO uint32_t TI : 1;  /*!< Transmit Interrupt.This bit indicates that frame transmission
                                                         is finished and TDES1 31 is set inthe First Descriptor.               */
      __IO uint32_t TPS : 1; /*!< Transmit Process Stopped                                              */
      __IO uint32_t TU : 1;  /*!< Transmit Buffer Unavailable                                           */
      __IO uint32_t TJT : 1; /*!< Transmit Jabber Time out                                              */
      __IO uint32_t OVF : 1; /*!< Receive Overflow                                                      */
      __IO uint32_t UNF : 1; /*!< Transmit Underflow                                                    */
      __IO uint32_t RI : 1;  /*!< Receive Interrupt                                                     */
      __IO uint32_t RU : 1;  /*!< Receive Buffer Unavailable                                            */
      __IO uint32_t RPS : 1; /*!< Receive Process Stopped                                               */
      __IO uint32_t RWT : 1; /*!< Receive Watchdog Time out                                             */
      __IO uint32_t ETI : 1; /*!< Early Transmit Interrupt                                              */
      uint32_t : 2;
      __IO uint32_t FBI : 1;  /*!< Fatal Bus Error Interrupt                                             */
      __IO uint32_t ERI : 1;  /*!< Early Receive Interrupt                                               */
      __IO uint32_t AIS : 1;  /*!< Abnormal Interrupt Summary                                            */
      __IO uint32_t NIS : 1;  /*!< Normal Interrupt Summary                                              */
      __I uint32_t RS : 3;    /*!< Receive Process State                                                 */
      __I uint32_t TS : 3;    /*!< Transmit Process State                                                */
      __I uint32_t EB_23 : 1; /*!< Error Bits.These bits indicate the type of error that caused
                                                         a Bus Error                                                           */
      __I uint32_t EB_24 : 1; /*!< Error Bits.These bits indicate the type of error that caused
                                                         a Bus Error                                                           */
      __I uint32_t EB_25 : 1; /*!< Error Bits.These bits indicate the type of error that caused
                                                         a Bus Error                                                           */
      __I uint32_t GLI : 1;   /*!< GMAC Line interface Interrupt                                         */
      __I uint32_t GMI : 1;   /*!< GMAC MMC Interrupt                                                    */
      __I uint32_t GPI : 1;   /*!< GMAC PMT Interrupt                                                    */
    } STATUS_REG5_b;          /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t OPR_MODE_REG6; /*!< The Operation Mode register establishes the Transmit and Receive
                                                         operating modes and commands                                          */

    struct {
      uint32_t : 1;
      __IO uint32_t SR : 1;  /*!< Start/Stop Receive                                                    */
      __IO uint32_t OSF : 1; /*!< Operate on Second Frame                                               */
      __I uint32_t RTC : 2;  /*!< Receive Threshold Control.These two bits control the threshold
                                                         level of the MTL Receive FIFO.                                        */
      uint32_t : 1;
      __IO uint32_t FUF : 1; /*!< Forward Undersized Good Frames                                        */
      __IO uint32_t FEF : 1; /*!< Forward Error Frames                                                  */
      __IO uint32_t EFC : 1; /*!< Enable HW flow control                                                */
      __IO uint32_t RFA : 2; /*!< Threshold for activating flow control                                 */
      __IO uint32_t RFD : 2; /*!< Threshold for deactivating flow control                               */
      __IO uint32_t ST : 1;  /*!< Start/Stop Transmission Command                                       */
      __IO uint32_t TTC : 3; /*!< Transmit Threshold Control                                            */
      uint32_t : 3;
      __IO uint32_t FTF : 1;   /*!< When this bit is set, the transmit FIFO controller logic is
                                                         reset to its defaultvalues and thus all data in the Tx FIFO
                                                          is lost/flushed                                                      */
      __IO uint32_t TSF : 1;   /*!< Transmit Store and Forward                                            */
      __IO uint32_t RFD_2 : 1; /*!< MSB of Threshold for Deactivating Flow Control                        */
      __IO uint32_t RFA_2 : 1; /*!< MSB of Threshold for Activating Flow Control                          */
      __IO uint32_t DFF : 1;   /*!< Disable Flushing of Received Frames                                   */
      __IO uint32_t RSF : 1;   /*!< Receive Store and Forward                                             */
      __IO uint32_t DT : 1;    /*!< Disable Dropping of TCP/IP Checksum Error Frames                      */
    } OPR_MODE_REG6_b;         /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t IER_REG7; /*!< The Interrupt Enable register enables the interrupts reported
                                                         by Register5                                                          */

    struct {
      __IO uint32_t TIE : 1; /*!< Transmit Interrupt Enable                                             */
      __IO uint32_t TSE : 1; /*!< Transmit Stopped Enable                                               */
      __IO uint32_t TUE : 1; /*!< Transmit Buffer Unavailable Enable                                    */
      __IO uint32_t TJE : 1; /*!< Transmit Jabber Time out Enable                                       */
      __IO uint32_t OVE : 1; /*!< Overflow Interrupt Enable                                             */
      __IO uint32_t UNE : 1; /*!< Underflow Interrupt Enable                                            */
      __IO uint32_t RIE : 1; /*!< Receive Interrupt Enable                                              */
      __IO uint32_t RUE : 1; /*!< Receive Buffer Unavailable Enable                                     */
      __IO uint32_t RSE : 1; /*!< Receive Stopped Enable                                                */
      __IO uint32_t RWE : 1; /*!< Receive Watchdog Time out Enable                                      */
      __IO uint32_t ETE : 1; /*!< Early Transmit Interrupt Enable                                       */
      uint32_t : 2;
      __IO uint32_t FBE : 1; /*!< Fatal Bus Error Enable                                                */
      __IO uint32_t ERE : 1; /*!< Early Receive Interrupt Enable                                        */
      __IO uint32_t AIE : 1; /*!< Abnormal Interrupt Summary Enable                                     */
      __IO uint32_t NIE : 1; /*!< Normal Interrupt Summary Enable                                       */
    } IER_REG7_b;            /*!< BitSize                                                               */
  };

  union {
    __IO uint32_t MFBOC_REG8; /*!< The Interrupt Enable register enables the interrupts reported
                                                         by Register5                                                          */

    struct {
      __IO uint32_t TIE : 16;              /*!< Indicates the number of frames missed by the controller due
                                                         to the Host ReceiveBuffer being unavailable                           */
      __IO uint32_t OVERFLOW_BIT_M : 1;    /*!< Overflow bit for Missed Frame Counter                                 */
      __IO uint32_t MISSEED_FRAMES : 11;   /*!< Indicates the number of frames missed by the application              */
      __IO uint32_t OVERFLOW_BIT_FIFO : 1; /*!< Overflow bit for Missed Frame Counter                                 */
    } MFBOC_REG8_b;                        /*!< BitSize                                                               */
  };
  __I uint32_t RESERVED7[9];
  __I uint32_t CHTX_DESC_REG18;     /*!< Host Transmit Descriptor Address Pointer Cleared on Reset. Pointer
                                                         updated by DMA during operationBuffer being unavailable               */
  __I uint32_t CHRX_DESC_REG19;     /*!< Host Receive Descriptor Address PointerCleared on Reset. Pointer
                                                         updated by DMA during operation                                       */
  __I uint32_t CHRX_DESC_REG20;     /*!< Host Transmit Buffer Address PointerCleared on Reset. Pointer
                                                         updated by DMA during operation                                       */
  __I uint32_t CHRX_BUF_ADDR_REG21; /*!< Host Receive Buffer Address PointerCleared on Reset. Pointer
                                                         updated by DMA during operation                                       */
} ETHERNET_Type;

/* ================================================================================ */
/* ================                     EGPIO0                     ================ */
/* ================================================================================ */

/**
 * @brief ENHANCED GENERAL PERPOSE INPUT/OUTPUT (EGPIO0)
 */

typedef struct {                         /*!< EGPIO0 Structure                                                      */
  EGPIO0_PIN_CONFIG_Type PIN_CONFIG[90]; /*!< PIN_CONFIG0                                                           */
  __I uint32_t RESERVED[664];
  EGPIO0_PORT_CONFIG_Type PORT_CONFIG[6]; /*!< PORT_CONFIG0                                                          */
  __I uint32_t RESERVED1[80];
  EGPIO0_INTR_Type INTR[6]; /*!< INTR0                                                                 */
  __I uint32_t RESERVED2[4];
  EGPIO0_GPIO_GRP_INTR_Type
    GPIO_GRP_INTR[4]; /*!< GPIO_GRP_INTR0                                                        */
} EGPIO_Type;

typedef struct { /*!<  SDIO structure           */

  union {
    __IO uint32_t SDIO_INTR_FN1_STATUS_CLEAR_REG; /*!<  SDIO Function1 Interrupt Enable Register   */

    struct {
      __IO uint32_t
        SDIO_WR_INT_CLR : 1; /*!<  This bit is used to enable CMD53 write interrupt. =1 Interrupt is enabled =0 - Interrupt is disabled */
      __IO uint32_t SDIO_RD_INT_CLR : 1;      /*!<  This bit is used to enable CMD53 read interrupt */
      __IO uint32_t SDIO_CSA_INT_CLR : 1;     /*!<  This bit is used to enable CMD53 CSA interrupt */
      __IO uint32_t SDIO_CMD52_INT_CLR : 1;   /*!<  This bit is used to enable CMD52 interrupt.   */
      __IO uint32_t SDIO_PWR_LEV_INT_CLR : 1; /*!<  This bit is used to enable power level change interrupt. */
      __IO uint32_t SDIO_CRC_ERR_INT_CLR : 1; /*!<  This bit is used to enable CRC error interrupt.*/
      __IO uint32_t SDIO_ABORT_INT_CLR : 1;   /*!<  This bit is used to enable abort interrupt   */
      __IO uint32_t SDIO_TOUT_INT_CLR : 1;    /*!<  This bit is used to enable ?read FIFO wait time over? interrupt. */
    } SDIO_INTR_FN1_STATUS_CLEAR_REG_b;
  };

  union {
    __IO uint32_t SDIO_INTR_FN1_ENABLE_REG; /*!<  SDIO Function1 Interrupt Enable Register   */

    struct {
      __IO uint32_t
        SDIO_WR_INT_EN : 1; /*!<  This bit is used to enable CMD53 write interrupt. =1 Interrupt is enabled =0 - Interrupt is disabled */
      __IO uint32_t SDIO_RD_INT_EN : 1;      /*!<  This bit is used to enable CMD53 read interrupt */
      __IO uint32_t SDIO_CSA_INT_EN : 1;     /*!<  This bit is used to enable CMD53 CSA interrupt */
      __IO uint32_t SDIO_CMD52_INT_EN : 1;   /*!<  This bit is used to enable CMD52 interrupt.   */
      __IO uint32_t SDIO_PWR_LEV_INT_EN : 1; /*!<  This bit is used to enable power level change interrupt. */
      __IO uint32_t SDIO_CRC_ERR_INT_EN : 1; /*!<  This bit is used to enable CRC error interrupt.*/
      __IO uint32_t SDIO_ABORT_INT_EN : 1;   /*!<  This bit is used to enable abort interrupt   */
      __IO uint32_t SDIO_TOUT_INT_EN : 1;    /*!<  This bit is used to enable ?read FIFO wait time over? interrupt. */
    } SDIO_INTR_FN1_ENABLE_REG_b;
  };
  union {
    __IO uint32_t SDIO_INTR_FN1_MASK_REG; /*!<  SDIO Function1 Interrupt Mask Register */
    struct {
      __IO uint32_t SDIO_WR_INT_MSK : 1; /*!<  This bit is used to mask CMD53 write interrupt. */
      __IO uint32_t SDIO_RD_INT_MSK : 1; /*!<  This bit is used to mask CMD53 read interrupt */
      __IO uint32_t
        SDIO_CSA_MSK : 1; /*!<  This bit is used to mask CMD53 CSA interrupt.Setting this bit will mask the interrupt Clearing this bit has no effect */
      __IO uint32_t SDIO_CMD52_MSK : 1;   /*!<  This bit is used to mask CMD52 interrupt */
      __IO uint32_t SDIO_PWR_LEV_MSK : 1; /*!<  This bit is used to mask power level change interrupt */
      __IO uint32_t SDIO_CRC_ERR_MSK : 1; /*!<  This bit is used to mask CRC error interrupt */
      __IO uint32_t
        SDIO_ABORT_MSK : 2; /*!<  This bit is used to mask abort interrupt Setting this bit will mask the interrupt Clearing this bit has no effect */
      __IO uint32_t SDIO_TOUT_MSK : 1; /*!<  This bit is used to mask "read FIFO wait time over" interrupt */
    } SDIO_INTR_FN1_MASK_REG_b;
  };
  union {
    __IO uint32_t SDIO_INTR_FN1_UNMASK_REG; /*!<  SDIO Function1 Interrupt Unmask Register */
    struct {
      __IO uint32_t SDIO_WR_INT_UNMSK : 1;  /*!<  This bit is used to unmask CMD53 write interrupt. */
      __IO uint32_t SDIO_RD_INT_UNMSK : 1;  /*!<  This bit is used to unmask CMD53 read interrupt */
      __IO uint32_t SDIO_CSA_UNMSK : 1;     /*!<  This bit is used to unmask CMD53 CSA interrupt */
      __IO uint32_t SDIO_CMD52_UNMSK : 1;   /*!<  This bit is used to unmask CMD52 interrupt */
      __IO uint32_t SDIO_PWR_LEV_UNMSK : 1; /*!<  This bit is used to unmask power level change interrupt */
      __IO uint32_t SDIO_CRC_ERR_UNMSK : 1; /*!<  This bit is used to unmask CRC error interrupt. */
      __IO uint32_t SDIO_ABORT_UNMSK : 2;   /*!<  This bit is used to unmask abort interrupt */
      __IO uint32_t SDIO_TOUT_UNMSK : 1;    /*!<  This bit is used to unmask ?read FIFO wait time over? interrupt */
    } SDIO_INTR_FN1_UNMASK_REG_b;
  };
  union {
    __I uint32_t SDIO_BLK_LEN_REG; /*!< SDIO Block Length Register */
    struct {
      __I uint32_t SDIO_BLK_LEN : 12; /*!< Length of each block for the last received CMD53 */
      uint32_t : 4;                   /*!<  reseerved for future use */
    } SDIO_BLK_LEN_REG_b;
  };

  union {
    __I uint32_t SDIO_BLK_CNT_REG; /*!<SDIO Block Count Register */
    struct {
      __I uint32_t SDIO_BLK_CNT : 9; /*!< Block count for the last received CMD53 */
      uint32_t : 7;                  /*!< Reserved for future use. */
    } SDIO_BLK_CNT_REG_b;
  };

  union {
    __I uint32_t SDIO_ADDRESS_REG; /*!< SDIO Address Register */
    struct {
      __I uint32_t SDIO_ADDR : 16; /*!< Lower 16-bits of the 17-bit address field in the last received CMD53 */

    } SDIO_ADDRESS_REG_b;
  };
  __IO uint32_t SDIO_CMD52_RDATA_REGISTER;

  __IO uint32_t SDIO_CMD52_WDATA_REGISTER;

  union {
    __I uint32_t SDIO_INTR_STATUS_REG; /*!<  SDIO Interrupt Function Number Register */
    struct {
      __I uint32_t SDIO_INTR_FN_NUM : 3; /*!< Indicates the function number to which interrupt is pending.
                                                         This register is provided to enable the software to decode the interrupt register easily.
                                                         Once this interrupt is cleared this register gets the next function number to which interrupt is pending (if simultaneous interrupts are pending).
                                                         2 - function 2 3 - function 3 4 - function 4 5 - function 5 0 - there is no pending interrupt */
      uint32_t : 13;                     /*!<  reserved */
    } SDIO_INTR_STATUS_REG_b;
  };

  union {
    __I uint32_t SDIO_INTR_FN_NUMBER_REG; /*!<  SDIO Interrupt Function Number Register */
    struct {
      __I uint32_t SDIO_INTR_FN_NUM : 3; /*!< Indicates the function number to which interrupt is pending.
                                                         This register is provided to enable the software to decode the interrupt register easily.
                                                         Once this interrupt is cleared this register gets the next function number to which interrupt is pending (if simultaneous interrupts are pending).
                                                         2 - function 2 3 - function 3 4 - function 4 5 - function 5 0 - there is no pending interrupt */
      uint32_t : 13;                     /*!<  reserved */
    } SDIO_INTR_FN_NUMBER_REG_b;
  };

  union {
    __IO uint32_t SDIO_FIFO_STATUS_REG; /*!< SDIO FIFO Status Register */
    struct {
      __I uint32_t SDIO_WFIFO_FULL : 1;     /*!< When set, indicates the WFIFO is almost full*/
      __I uint32_t SDIO_WFIFO_AFULL : 1;    /*!< When set, indicates that WFIFO is almost full.*/
      __I uint32_t SDIO_RFIFO_EMPTY : 1;    /*!< When set, indicates the RFIFO is empty */
      __I uint32_t SDIO_RFIFO_AEMPTY : 1;   /*!< When set, indicates that RFIFO is almost empty.*/
      __I uint32_t SDIO_CURRENT_FN_NUM : 3; /*!< Indicates the function number of the last received command */
      __I uint32_t
        SDIO_BUS_CONTROL_STATE : 5; /*!< SDIO bus control state 7 - When set, indicates FSM is in idle state 8 - When set, indicates FSM is in CMD52 read state 9 - When set, indicates FSM is in CMD52 write state 10 - When set, indicates FSM is
                                                         CMD53 read state 11 - When set, indicates FSM is CMD53 write state */
    } SDIO_FIFO_STATUS_REG_b;
  };

  union {
    __I uint32_t SDIO_FIFO_OCC_REG; /*!< SDIO FIFO Occupancy Register */
    struct {
      __I uint32_t SDIO_WFIFO_OCC : 8;   /*!<  Indicates the occupancy level of the write FIFO */
      __I uint32_t SDIO_RFIFO_AVAIL : 8; /*!<  Indicates the available space in the read FIFO */
    } SDIO_FIFO_OCC_REG_b;
  };

  union {
    __I uint32_t SDIO_HOST_INTR_SET_REG; /*!< SDIO Host Interrupt Set Register */
    struct {
      __I uint32_t SDIO_INTSET_FN2 : 1; /*!< This bit is used to raise an interrupt to host for function2. */
      __I uint32_t SDIO_INTSET_FN3 : 1; /*!< This bit is used to raise an interrupt to host for function3 */
      __I uint32_t SDIO_INTSET_FN4 : 1; /*!< This bit is used to raise an interrupt to host for function4 */
      __I uint32_t SDIO_INTSET_FN5 : 1; /*!< This bit is used to raise an interrupt to host for function5 */
      uint32_t : 12;                    /*!< reserved */
    } SDIO_HOST_INTR_SET_REG_b;
  };

  union {
    __I uint32_t SDIO_HOST_INTR_CLEAR_REG; /*!<  SDIO Host Interrupt Clear Register */
    struct {
      __I uint32_t SDIO_INTCLR_FN2 : 1; /*!<  This bit is used to clear the interrupt to host for function2 */
      __I uint32_t SDIO_INTCLR_FN3 : 1; /*!<  This bit is used to clear the interrupt to host from function3 */
      __I uint32_t SDIO_INTCLR_FN4 : 1; /*!<  This bit is used to clear the interrupt to host for function4.*/
      __I uint32_t SDIO_INTCLR_FN5 : 1; /*!< This bit is used to clear the interrupt to host for function5 */
      uint32_t : 12;
    } SDIO_HOST_INTR_CLEAR_REG_b;
  };

  __IO uint32_t RESERVED;
  __O uint32_t SDIO_RFIFO_DATA_REG[256]; /*!< SDIO Read FIFO Data Register  */

  __I uint32_t SDIO_WFIFO_DATA_REG[256]; /*!< SDIO Write FIFO Data Register   */

  union {
    __IO uint32_t SDIO_INTR_FN2_STATUS_CLEAR_REG; /*!<  SDIO Function1 Interrupt Enable Register   */

    struct {
      __IO uint32_t
        SDIO_WR_INT_CLR : 1; /*!<  This bit is used to enable CMD53 write interrupt. =1 Interrupt is enabled =0 - Interrupt is disabled */
      __IO uint32_t SDIO_RD_INT_CLR : 1;      /*!<  This bit is used to enable CMD53 read interrupt */
      __IO uint32_t SDIO_CSA_INT_CLR : 1;     /*!<  This bit is used to enable CMD53 CSA interrupt */
      __IO uint32_t SDIO_CMD52_INT_CLR : 1;   /*!<  This bit is used to enable CMD52 interrupt.   */
      __IO uint32_t SDIO_PWR_LEV_INT_CLR : 1; /*!<  This bit is used to enable power level change interrupt. */
      __IO uint32_t SDIO_CRC_ERR_INT_CLR : 1; /*!<  This bit is used to enable CRC error interrupt.*/
      __IO uint32_t SDIO_ABORT_INT_CLR : 1;   /*!<  This bit is used to enable abort interrupt   */
      __IO uint32_t SDIO_TOUT_INT_CLR : 1;    /*!<  This bit is used to enable ?read FIFO wait time over? interrupt. */
    } SDIO_INTR_FN2_STATUS_CLEAR_REG_b;
  };

  union {
    __IO uint32_t SDIO_INTR_FN2_ENABLE_REG; /*!<  SDIO Function1 Interrupt Enable Register   */

    struct {
      __IO uint32_t
        SDIO_WR_INT_EN : 1; /*!<  This bit is used to enable CMD53 write interrupt. =1‘- Interrupt is enabled =0‘- Interrupt is disabled */
      __IO uint32_t SDIO_RD_INT_EN : 1;      /*!<  This bit is used to enable CMD53 read interrupt */
      __IO uint32_t SDIO_CSA_INT_EN : 1;     /*!<  This bit is used to enable CMD53 CSA interrupt */
      __IO uint32_t SDIO_CMD52_INT_EN : 1;   /*!<  This bit is used to enable CMD52 interrupt*/
      __IO uint32_t SDIO_PWR_LEV_INT_EN : 1; /*!<  This bit is used to enable power level change interrupt. */
      __IO uint32_t SDIO_CRC_ERR_INT_EN : 1; /*!<  This bit is used to enable CRC error interrupt.*/
      __IO uint32_t SDIO_ABORT_INT_EN : 1;   /*!<  This bit is used to enable abort interrupt   */
      __IO uint32_t SDIO_TOUT_INT_EN : 1;    /*!<  This bit is used to enable ?read FIFO wait time over? interrupt. */
    } SDIO_INTR_FN2_ENABLE_REG_b;
  };
  union {
    __IO uint32_t SDIO_INTR_FN2_MASK_REG; /*!<  SDIO Function1 Interrupt Mask Register */
    struct {
      __IO uint32_t SDIO_WR_INT_MSK : 1; /*!<  This bit is used to mask CMD53 write interrupt. */
      __IO uint32_t SDIO_RD_INT_MSK : 1; /*!<  This bit is used to mask CMD53 read interrupt */
      __IO uint32_t
        SDIO_CSA_MSK : 1; /*!<  This bit is used to mask CMD53 CSA interrupt.Setting this bit will mask the interrupt Clearing this bit has no effect */
      __IO uint32_t SDIO_CMD52_MSK : 1;   /*!<  This bit is used to mask CMD52 interrupt */
      __IO uint32_t SDIO_PWR_LEV_MSK : 1; /*!<  This bit is used to mask power level change interrupt */
      __IO uint32_t SDIO_CRC_ERR_MSK : 1; /*!<  This bit is used to mask CRC error interrupt */
      __IO uint32_t
        SDIO_ABORT_MSK : 2; /*!<  This bit is used to mask abort interrupt Setting this bit will mask the interrupt Clearing this bit has no effect */
      __IO uint32_t SDIO_TOUT_MSK : 1; /*!<  This bit is used to mask "read FIFO wait time over" interrupt */
    } SDIO_INTR_FN2_MASK_REG_b;
  };
  union {
    __IO uint32_t SDIO_INTR_FN2_UNMASK_REG; /*!<  SDIO Function1 Interrupt Unmask Register */
    struct {
      __IO uint32_t SDIO_WR_INT_UNMSK : 1;  /*!<  This bit is used to unmask CMD53 write interrupt. */
      __IO uint32_t SDIO_RD_INT_UNMSK : 1;  /*!<  This bit is used to unmask CMD53 read interrupt */
      __IO uint32_t SDIO_CSA_UNMSK : 1;     /*!<  This bit is used to unmask CMD53 CSA interrupt */
      __IO uint32_t SDIO_CMD52_UNMSK : 1;   /*!<  This bit is used to unmask CMD52 interrupt */
      __IO uint32_t SDIO_PWR_LEV_UNMSK : 1; /*!<  This bit is used to unmask power level change interrupt */
      __IO uint32_t SDIO_CRC_ERR_UNMSK : 1; /*!<  This bit is used to unmask CRC error interrupt. */
      __IO uint32_t SDIO_ABORT_UNMSK : 2;   /*!<  This bit is used to unmask abort interrupt */
      __IO uint32_t SDIO_TOUT_UNMSK : 1;    /*!<  This bit is used to unmask ?read FIFO wait time over? interrupt */
    } SDIO_INTR_FN2_UNMASK_REG_b;
  };

  union {
    __IO uint32_t SDIO_INTR_FN3_STATUS_CLEAR_REG; /*!<  SDIO Function1 Interrupt Enable Register   */

    struct {
      __IO uint32_t
        SDIO_WR_INT_CLR : 1; /*!<  This bit is used to enable CMD53 write interrupt. =1 Interrupt is enabled =0 - Interrupt is disabled */
      __IO uint32_t SDIO_RD_INT_CLR : 1;      /*!<  This bit is used to enable CMD53 read interrupt */
      __IO uint32_t SDIO_CSA_INT_CLR : 1;     /*!<  This bit is used to enable CMD53 CSA interrupt */
      __IO uint32_t SDIO_CMD52_INT_CLR : 1;   /*!<  This bit is used to enable CMD52 interrupt.   */
      __IO uint32_t SDIO_PWR_LEV_INT_CLR : 1; /*!<  This bit is used to enable power level change interrupt. */
      __IO uint32_t SDIO_CRC_ERR_INT_CLR : 1; /*!<  This bit is used to enable CRC error interrupt.*/
      __IO uint32_t SDIO_ABORT_INT_CLR : 1;   /*!<  This bit is used to enable abort interrupt   */
      __IO uint32_t SDIO_TOUT_INT_CLR : 1;    /*!<  This bit is used to enable ?read FIFO wait time over? interrupt. */
    } SDIO_INTR_FN3_STATUS_CLEAR_REG_b;
  };

  union {
    __IO uint32_t SDIO_INTR_FN3_ENABLE_REG; /*!<  SDIO Function1 Interrupt Enable Register   */

    struct {
      __IO uint32_t
        SDIO_WR_INT_EN : 1; /*!<  This bit is used to enable CMD53 write interrupt. =1‘- Interrupt is enabled =0‘- Interrupt is disabled */
      __IO uint32_t SDIO_RD_INT_EN : 1;      /*!<  This bit is used to enable CMD53 read interrupt */
      __IO uint32_t SDIO_CSA_INT_EN : 1;     /*!<  This bit is used to enable CMD53 CSA interrupt */
      __IO uint32_t SDIO_CMD52_INT_EN : 1;   /*!<  This bit is used to enable CMD52 interrupt*/
      __IO uint32_t SDIO_PWR_LEV_INT_EN : 1; /*!<  This bit is used to enable power level change interrupt. */
      __IO uint32_t SDIO_CRC_ERR_INT_EN : 1; /*!<  This bit is used to enable CRC error interrupt.*/
      __IO uint32_t SDIO_ABORT_INT_EN : 1;   /*!<  This bit is used to enable abort interrupt   */
      __IO uint32_t SDIO_TOUT_INT_EN : 1;    /*!<  This bit is used to enable ?read FIFO wait time over? interrupt. */
    } SDIO_INTR_FN3_ENABLE_REG_b;
  };

  union {
    __IO uint32_t SDIO_INTR_FN3_MASK_REG; /*!<  SDIO Function1 Interrupt Mask Register */
    struct {
      __IO uint32_t SDIO_WR_INT_MSK : 1; /*!<  This bit is used to mask CMD53 write interrupt. */
      __IO uint32_t SDIO_RD_INT_MSK : 1; /*!<  This bit is used to mask CMD53 read interrupt */
      __IO uint32_t
        SDIO_CSA_MSK : 1; /*!<  This bit is used to mask CMD53 CSA interrupt.Setting this bit will mask the interrupt Clearing this bit has no effect */
      __IO uint32_t SDIO_CMD52_MSK : 1;   /*!<  This bit is used to mask CMD52 interrupt */
      __IO uint32_t SDIO_PWR_LEV_MSK : 1; /*!<  This bit is used to mask power level change interrupt */
      __IO uint32_t SDIO_CRC_ERR_MSK : 1; /*!<  This bit is used to mask CRC error interrupt */
      __IO uint32_t
        SDIO_ABORT_MSK : 2; /*!<  This bit is used to mask abort interrupt Setting this bit will mask the interrupt Clearing this bit has no effect */
      __IO uint32_t SDIO_TOUT_MSK : 1; /*!<  This bit is used to mask "read FIFO wait time over" interrupt */
    } SDIO_INTR_FN3_MASK_REG_b;
  };
  union {
    __IO uint32_t SDIO_INTR_FN3_UNMASK_REG; /*!<  SDIO Function1 Interrupt Unmask Register */
    struct {
      __IO uint32_t SDIO_WR_INT_UNMSK : 1;  /*!<  This bit is used to unmask CMD53 write interrupt. */
      __IO uint32_t SDIO_RD_INT_UNMSK : 1;  /*!<  This bit is used to unmask CMD53 read interrupt */
      __IO uint32_t SDIO_CSA_UNMSK : 1;     /*!<  This bit is used to unmask CMD53 CSA interrupt */
      __IO uint32_t SDIO_CMD52_UNMSK : 1;   /*!<  This bit is used to unmask CMD52 interrupt */
      __IO uint32_t SDIO_PWR_LEV_UNMSK : 1; /*!<  This bit is used to unmask power level change interrupt */
      __IO uint32_t SDIO_CRC_ERR_UNMSK : 1; /*!<  This bit is used to unmask CRC error interrupt. */
      __IO uint32_t SDIO_ABORT_UNMSK : 2;   /*!<  This bit is used to unmask abort interrupt */
      __IO uint32_t SDIO_TOUT_UNMSK : 1;    /*!<  This bit is used to unmask ?read FIFO wait time over? interrupt */
    } SDIO_INTR_FN3_UNMASK_REG_b;
  };

  union {
    __IO uint32_t SDIO_INTR_FN4_STATUS_CLEAR_REG; /*!<  SDIO Function1 Interrupt Enable Register   */

    struct {
      __IO uint32_t
        SDIO_WR_INT_CLR : 1; /*!<  This bit is used to enable CMD53 write interrupt. =1 Interrupt is enabled =0 - Interrupt is disabled */
      __IO uint32_t SDIO_RD_INT_CLR : 1;      /*!<  This bit is used to enable CMD53 read interrupt */
      __IO uint32_t SDIO_CSA_INT_CLR : 1;     /*!<  This bit is used to enable CMD53 CSA interrupt */
      __IO uint32_t SDIO_CMD52_INT_CLR : 1;   /*!<  This bit is used to enable CMD52 interrupt.   */
      __IO uint32_t SDIO_PWR_LEV_INT_CLR : 1; /*!<  This bit is used to enable power level change interrupt. */
      __IO uint32_t SDIO_CRC_ERR_INT_CLR : 1; /*!<  This bit is used to enable CRC error interrupt.*/
      __IO uint32_t SDIO_ABORT_INT_CLR : 1;   /*!<  This bit is used to enable abort interrupt   */
      __IO uint32_t SDIO_TOUT_INT_CLR : 1;    /*!<  This bit is used to enable ?read FIFO wait time over? interrupt. */
    } SDIO_INTR_FN4_STATUS_CLEAR_REG_b;
  };

  union {
    __IO uint32_t SDIO_INTR_FN4_ENABLE_REG; /*!<  SDIO Function1 Interrupt Enable Register   */

    struct {
      __IO uint32_t
        SDIO_WR_INT_EN : 1; /*!<  This bit is used to enable CMD53 write interrupt. =1 - Interrupt is enabled =0 -Interrupt is disabled */
      __IO uint32_t SDIO_RD_INT_EN : 1;      /*!<  This bit is used to enable CMD53 read interrupt */
      __IO uint32_t SDIO_CSA_INT_EN : 1;     /*!<  This bit is used to enable CMD53 CSA interrupt */
      __IO uint32_t SDIO_CMD52_INT_EN : 1;   /*!<  This bit is used to enable CMD52 interrupt*/
      __IO uint32_t SDIO_PWR_LEV_INT_EN : 1; /*!<  This bit is used to enable power level change interrupt */
      __IO uint32_t SDIO_CRC_ERR_INT_EN : 1; /*!<  This bit is used to enable CRC error interrupt*/
      __IO uint32_t SDIO_ABORT_INT_EN : 1;   /*!<  This bit is used to enable abort interrupt   */
      __IO uint32_t SDIO_TOUT_INT_EN : 1;    /*!<  This bit is used to enable ?read FIFO wait time over? interrupt */
    } SDIO_INTR_FN4_ENABLE_REG_b;
  };

  union {
    __IO uint32_t SDIO_INTR_FN4_MASK_REG; /*!<  SDIO Function1 Interrupt Mask Register */
    struct {
      __IO uint32_t SDIO_WR_INT_MSK : 1; /*!<  This bit is used to mask CMD53 write interrupt. */
      __IO uint32_t SDIO_RD_INT_MSK : 1; /*!<  This bit is used to mask CMD53 read interrupt */
      __IO uint32_t
        SDIO_CSA_MSK : 1; /*!<  This bit is used to mask CMD53 CSA interrupt.Setting this bit will mask the interrupt Clearing this bit has no effect */
      __IO uint32_t SDIO_CMD52_MSK : 1;   /*!<  This bit is used to mask CMD52 interrupt */
      __IO uint32_t SDIO_PWR_LEV_MSK : 1; /*!<  This bit is used to mask power level change interrupt */
      __IO uint32_t SDIO_CRC_ERR_MSK : 1; /*!<  This bit is used to mask CRC error interrupt */
      __IO uint32_t
        SDIO_ABORT_MSK : 2; /*!<  This bit is used to mask abort interrupt Setting this bit will mask the interrupt Clearing this bit has no effect */
      __IO uint32_t SDIO_TOUT_MSK : 1; /*!<  This bit is used to mask "read FIFO wait time over" interrupt */
    } SDIO_INTR_FN4_MASK_REG_b;
  };
  union {
    __IO uint32_t SDIO_INTR_FN4_UNMASK_REG; /*!<  SDIO Function1 Interrupt Unmask Register */
    struct {
      __IO uint32_t SDIO_WR_INT_UNMSK : 1;  /*!<  This bit is used to unmask CMD53 write interrupt. */
      __IO uint32_t SDIO_RD_INT_UNMSK : 1;  /*!<  This bit is used to unmask CMD53 read interrupt */
      __IO uint32_t SDIO_CSA_UNMSK : 1;     /*!<  This bit is used to unmask CMD53 CSA interrupt */
      __IO uint32_t SDIO_CMD52_UNMSK : 1;   /*!<  This bit is used to unmask CMD52 interrupt */
      __IO uint32_t SDIO_PWR_LEV_UNMSK : 1; /*!<  This bit is used to unmask power level change interrupt */
      __IO uint32_t SDIO_CRC_ERR_UNMSK : 1; /*!<  This bit is used to unmask CRC error interrupt. */
      __IO uint32_t SDIO_ABORT_UNMSK : 2;   /*!<  This bit is used to unmask abort interrupt */
      __IO uint32_t SDIO_TOUT_UNMSK : 1;    /*!<  This bit is used to unmask ?read FIFO wait time over? interrupt */
    } SDIO_INTR_FN4_UNMASK_REG_b;
  };

  union {
    __IO uint32_t SDIO_INTR_FN5_STATUS_CLEAR_REG; /*!<  SDIO Function1 Interrupt Enable Register   */

    struct {
      __IO uint32_t
        SDIO_WR_INT_CLR : 1; /*!<  This bit is used to enable CMD53 write interrupt. =1 Interrupt is enabled =0 - Interrupt is disabled */
      __IO uint32_t SDIO_RD_INT_CLR : 1;      /*!<  This bit is used to enable CMD53 read interrupt */
      __IO uint32_t SDIO_CSA_INT_CLR : 1;     /*!<  This bit is used to enable CMD53 CSA interrupt */
      __IO uint32_t SDIO_CMD52_INT_CLR : 1;   /*!<  This bit is used to enable CMD52 interrupt.   */
      __IO uint32_t SDIO_PWR_LEV_INT_CLR : 1; /*!<  This bit is used to enable power level change interrupt. */
      __IO uint32_t SDIO_CRC_ERR_INT_CLR : 1; /*!<  This bit is used to enable CRC error interrupt.*/
      __IO uint32_t SDIO_ABORT_INT_CLR : 1;   /*!<  This bit is used to enable abort interrupt   */
      __IO uint32_t SDIO_TOUT_INT_CLR : 1;    /*!<  This bit is used to enable ?read FIFO wait time over? interrupt. */
    } SDIO_INTR_FN5_STATUS_CLEAR_REG_b;
  };

  union {
    __IO uint32_t SDIO_INTR_FN5_ENABLE_REG; /*!<  SDIO Function1 Interrupt Enable Register   */

    struct {
      __IO uint32_t
        SDIO_WR_INT_EN : 1; /*!<  This bit is used to enable CMD53 write interrupt. =1 Interrupt is enabled =0‘- Interrupt is disabled */
      __IO uint32_t SDIO_RD_INT_EN : 1;      /*!<  This bit is used to enable CMD53 read interrupt */
      __IO uint32_t SDIO_CSA_INT_EN : 1;     /*!<  This bit is used to enable CMD53 CSA interrupt */
      __IO uint32_t SDIO_CMD52_INT_EN : 1;   /*!<  This bit is used to enable CMD52 interrupt*/
      __IO uint32_t SDIO_PWR_LEV_INT_EN : 1; /*!<  This bit is used to enable power level change interrupt. */
      __IO uint32_t SDIO_CRC_ERR_INT_EN : 1; /*!<  This bit is used to enable CRC error interrupt.*/
      __IO uint32_t SDIO_ABORT_INT_EN : 1;   /*!<  This bit is used to enable abort interrupt   */
      __IO uint32_t SDIO_TOUT_INT_EN : 1;    /*!<  This bit is used to enable ?read FIFO wait time over? interrupt. */
    } SDIO_INTR_FN5_ENABLE_REG_b;
  };

  union {
    __IO uint32_t SDIO_INTR_FN5_MASK_REG; /*!<  SDIO Function1 Interrupt Mask Register */
    struct {
      __IO uint32_t SDIO_WR_INT_MSK : 1; /*!<  This bit is used to mask CMD53 write interrupt. */
      __IO uint32_t SDIO_RD_INT_MSK : 1; /*!<  This bit is used to mask CMD53 read interrupt */
      __IO uint32_t
        SDIO_CSA_MSK : 1; /*!<  This bit is used to mask CMD53 CSA interrupt.Setting this bit will mask the interrupt Clearing this bit has no effect */
      __IO uint32_t SDIO_CMD52_MSK : 1;   /*!<  This bit is used to mask CMD52 interrupt */
      __IO uint32_t SDIO_PWR_LEV_MSK : 1; /*!<  This bit is used to mask power level change interrupt */
      __IO uint32_t SDIO_CRC_ERR_MSK : 1; /*!<  This bit is used to mask CRC error interrupt */
      __IO uint32_t
        SDIO_ABORT_MSK : 2; /*!<  This bit is used to mask abort interrupt Setting this bit will mask the interrupt Clearing this bit has no effect */
      __IO uint32_t SDIO_TOUT_MSK : 1; /*!<  This bit is used to mask "read FIFO wait time over" interrupt */
    } SDIO_INTR_FN5_MASK_REG_b;
  };
  union {
    __IO uint32_t SDIO_INTR_FN5_UNMASK_REG; /*!<  SDIO Function1 Interrupt Unmask Register */
    struct {
      __IO uint32_t SDIO_WR_INT_UNMSK : 1;  /*!<  This bit is used to unmask CMD53 write interrupt. */
      __IO uint32_t SDIO_RD_INT_UNMSK : 1;  /*!<  This bit is used to unmask CMD53 read interrupt */
      __IO uint32_t SDIO_CSA_UNMSK : 1;     /*!<  This bit is used to unmask CMD53 CSA interrupt */
      __IO uint32_t SDIO_CMD52_UNMSK : 1;   /*!<  This bit is used to unmask CMD52 interrupt */
      __IO uint32_t SDIO_PWR_LEV_UNMSK : 1; /*!<  This bit is used to unmask power level change interrupt */
      __IO uint32_t SDIO_CRC_ERR_UNMSK : 1; /*!<  This bit is used to unmask CRC error interrupt. */
      __IO uint32_t SDIO_ABORT_UNMSK : 2;   /*!<  This bit is used to unmask abort interrupt */
      __IO uint32_t SDIO_TOUT_UNMSK : 1;    /*!<  This bit is used to unmask ?read FIFO wait time over? interrupt */
    } SDIO_INTR_FN5_UNMASK_REG_b;
  };
  union {
    __IO uint32_t SDIO_ERROR_COND_CHK_ENABLE_REG; /*!<SDIO error condition check enable register  */
    struct {
      __IO uint32_t
        SDIO_CRC_EN : 1; /*!<  When set, stops the DMA from doing data accesses till CRC error interrupt is cleared */
      __IO uint32_t
        SDIO_ABORT_EN : 1; /*!<  When set, stops the DMA from doing data accesses till ABORT interrupt is cleared */
      __IO uint32_t
        SDIO_SPI_RD_DATA_ERROR_EN : 1; /*!< When set, stops the DMA from doing data accesses till read data error interrupt is cleared in SPI mode  */
      uint32_t : 13;
    } SDIO_ERROR_COND_CHK_ENABLE_REG_b;
  };

  union {
    __IO uint32_t SDIO_ERROR_COND_STATE_REG; /*!< SDIO error condition state register  */
    struct {
      __IO uint32_t SDIO_ERROR_BYTE_CNT : 12; /*!< Indicates byte count when one of the error condition occurred*/
      uint32_t : 4;                           /*!< reserved  */
      __IO uint32_t SDIO_ERROR_BLK_CNT : 1;   /*!< Indicates block count when one of error condition occurred */
      uint32_t : 9;                           /*!< reserved*/
    } SDIO_ERROR_COND_STATE_REG_b;
  };

  union {
    __I uint32_t SDIO_BOOT_CONFIG_VALS_0_REG; /*!< SDIO Boot Config Values Register 0  */
    struct {
      __I uint32_t OCR_R : 24; /*!< Operating conditions. The value written by bootloader can be read here  */
      __I uint32_t
        CSA_MSBYTE : 24; /*!< MS byre of CSA address. Lower 24 bits of CSA will come through SDIO CSA registers. Whenever CSA access is done,
                                                         32-bit address will be prepared using these fields.   */
    } SDIO_BOOT_CONFIG_VALS_0_REG_b;
  };

  union {
    __I uint32_t SDIO_BOOT_CONFIG_VALS_1_REG; /*!< SDIO Boot Config Values Register 1  */
    struct {
      __I uint32_t
        NO_OF_IO_FUNCTIONS : 3; /*!< Indicates number functions supported. The value written by bootloader can be read here.  */
      __I uint32_t COMBOCARD : 1; /*!< When set, combo mode will be enabled  */
      __I uint32_t
        SDMEM_IGNOTRE_SDMEM_PRESENT : 1; /*!< When set, sdmem_present signal, coming from GPIO, will be ignored.  */
      __I uint32_t
        ADMEM_DRIVE_HIZ_MB_READ : 1; /*!< When set, High will be driven in the second cycle of interrupt period during sd memory mb read transfer */
      __I uint32_t
        SDMEM_DISABLE_INTERRUPT_MB_READ : 1; /*!< When set, interrupt will be not be driven during sd memory mb read transfer */
      __I uint32_t
        IGNORE_DISABLE_HS : 1; /*!< if ignore_disable_hs is set, sdmem_disable_high_speed_switching coming from combo mode module is ignored  */
      uint32_t : 24;
    } SDIO_BOOT_CONFIG_VALS_1_REG_b;
  };
} SDIO0_Type;

/* ================================================================================ */
/* ================              SPI SLAVE register map             ================ */
/* ================================================================================ */
typedef struct {

  union {
    __IO uint16_t SPI_HOST_INTR; /*!< SPI Host interupt resgister */
    struct {
      __IO uint16_t SPI_HOST_INTR : 8; /*!< These bits indicate the interrupt vector */
    } SPI_HOST_INTR_b;
  };

  union {
    __IO uint16_t SPI_RFIFO_START; /*!< SPI FIFO start Level Register */
    struct {
      __IO uint16_t SPI_RFIFO_ST : 8; /*!< Minimum FIFO occupancy level */
    } SPI_RFIFO_START_b;
  };

  __IO uint16_t SPI_RFIFO_AFULL_LEV;  /*!< TODO */
  __IO uint16_t SPI_RFIFO_AEMPTY_LEV; /*!< TODO */

  union {
    __IO uint16_t SPI_MODE; /*!< SPI mode Register */
    struct {
      __IO uint16_t SPEED_MODE : 1; /*!< This bit enables the SPI high speed mode
                                                                          0 - Low speed mode
                                                                          1 - High speed mode
			 */
      __IO uint16_t FIX_EN : 1;
    } SPI_MODE_b;
  };

  union {
    __IO uint16_t SPI_INTR_STATUS; /*!< SPI interrupt status register*/
    struct {
      __IO uint16_t SPI_WR_REQ : 1;      /*!< Write request received interrupt */
      __IO uint16_t SPI_RD_REQ : 1;      /*!< Read request received interrupt  */
      __IO uint16_t SPI_CS_DEASSERT : 1; /*!< SPI chip deassert interrupt      */

    } SPI_INTR_STATUS_b;
  };

  union {
    __IO uint16_t SPI_INTR_EN; /*!< SPI interrupt enable register*/
    struct {
      __IO uint16_t SPI_WR_INT_EN : 1;          /*!< SPI chip deassert  interrupt enable  */
      __IO uint16_t SPI_RD_INT_EN : 1;          /*!< Read request received interrupt enable */
      __IO uint16_t SPI_CS_DEASSERT_INT_EN : 1; /*!< Write request received interrupt enable */
    } SPI_INTR_EN_b;
  };

  union {
    __IO uint16_t SPI_INTR_MASK; /*!< SPI interrupt Mask register*/
    struct {
      __IO uint16_t SPI_WR_INT_MASK : 1;          /*!< Mask SPI chip deassert interrupt  */
      __IO uint16_t SPI_RD_INT_MASK : 1;          /*!< Mask Read request received interrupt */
      __IO uint16_t SPI_CS_DEASSERT_INT_MASK : 1; /*!< Mask write request received interrupt */
    } SPI_INTR_MASK_b;
  };

  union {
    __IO uint16_t SPI_INTR_UNMASK; /*!< SPI interrupt unmask register*/
    struct {
      __IO uint16_t SPI_WR_INT_UNMASK : 1;          /*!< Unmask SPI chip deassert interrupt  */
      __IO uint16_t SPI_RD_INT_UNMASK : 1;          /*!< Unmask Read request received interrupt */
      __IO uint16_t SPI_CS_DEASSERT_INT_UNMASK : 1; /*!< Unmask Write request received interrupt */
    } SPI_INTR_UNMASK_b;
  };

  __I uint16_t SPI_LENGTH;

  union {
    __I uint16_t SPI_COMMAND;
    struct {
      __I uint16_t SPI_CMD_C2 : 8;
      __I uint16_t SPI_CMD_C1 : 8;
    } SPI_COMMAND_b;
  };

  __I uint16_t SPI_DEV_ID;  /*!< SPI device ID */
  __I uint16_t SPI_VERSION; /*!< SPI version number*/

  union {
    __I uint16_t SPI_STATUS;
    struct {
      __I uint16_t SPI_RFIFO_FULL : 1;   /*!< Indicates Read FIFO is full
                                                                         0 -Not Full , 1 - Full  */
      __I uint16_t SPI_RFIFO_AFULL : 1;  /*!< Indicates Read FIFO is lomost full
                                                                          0 -Not almost Full , 1 - almost Full  */
      __I uint16_t SPI_WFIFO_EMPTY : 1;  /*!< Indicates Write FIFO is empty
                                                                         0 - write FIFO is empty , 1 - Write FIFO is not empty  */
      __I uint16_t SPI_WFIFO_AEMPTY : 1; /*!< Indicates Write FIFO is almost full
                                                                          0 - FIFO is alomost empty , 1 - FIFO is not almost empty  */
    } SPI_STATUS_b;
  };

  union {
    __I uint16_t SPI_BC_STATE;
    struct {
      __I uint16_t BC_IDLE : 1;
      __I uint16_t BC_EN : 1;
      __I uint16_t BC_CTRL_BUSY : 1;
      __I uint16_t BC_INT_REG_RD : 1;
      __I uint16_t BC_INT_REG_FIFO_WR : 1;
      __I uint16_t BC_INT_REG_WR_WAIT : 1;
      __I uint16_t BC_INT_REG_FIFO_RD : 1;
      __I uint16_t BC_BC_INT_REG_WR : 1;
      __I uint16_t BC_AHB_MASTER : 1;
      __I uint16_t AHB_MASTER_WAIT : 1;
      __I uint16_t AHB_SLAVE_WR_LEN : 1;
      __I uint16_t AHB_SLAVE_WR_CMD : 1;
      __I uint16_t AHB_SLAVE_WR_INTR : 1;
      __I uint16_t BC_AHB_SLAVE_WAIT : 1;
    } SPI_BC_STATE_b;
  };

  __I uint16_t RESERVED[8];

} RSI_SSPI_T;

/*POWER AND CLOCK REGISTER */

/* ================================================================================ */
/* ================                     M4_PLL                     ================ */
/* ================================================================================ */

/**
 * @brief Real timer is used in counting clocks, micro seconds, milli seconds, seconds and minutes with both ref clock and system (SoC) clock (M4_PLL)
 */

typedef struct { /*!< M4_PLL Structure                                                      */
  /*0x00*/
  union {
    __IO uint32_t CLK_ENABLE_SET_REG1; /*!< none                                                                  */
    struct {

      __IO uint32_t USART1_PCLK_ENABLE_b : 1;
      __IO uint32_t USART1_SCLK_ENABLE_b : 1;
      __IO uint32_t USART2_PCLK_ENABLE_b : 1;
      __IO uint32_t USART2_SCLK_ENABLE_b : 1;
      __IO uint32_t RESERVED0 : 1;
      __IO uint32_t RESERVED1 : 1;
      __IO uint32_t RESERVED2 : 1;
      __IO uint32_t RESERVED3 : 1;
      __IO uint32_t RESERVED4 : 1;
      __IO uint32_t SCT_CLK_ENABLE_b : 1;
      __IO uint32_t SCT_PCLK_ENABLE_b : 1;
      __IO uint32_t ICACHE_CLK_ENABLE_b : 1;
      __IO uint32_t ICACHE_CLK_2X_ENABLE_b : 1;
      __IO uint32_t RPDMA_HCLK_ENABLE_b : 1;
      __IO uint32_t SOC_PLL_SPI_CLK_ENABLE_b : 1;
      __IO uint32_t RESERVED5 : 1;
      __IO uint32_t IID_CLK_ENABLE_b : 1;
      __IO uint32_t SDIO_SYS_HCLK_ENABLE_b : 1;
      __IO uint32_t CRC_CLK_ENABLE_b : 1;
      __IO uint32_t M4SS_UM_CLK_STATIC_EN_b : 1;
      __IO uint32_t RESERVED6 : 1;
      __IO uint32_t ETH_HCLK_ENABLE_b : 1;
      __IO uint32_t HWRNG_PCLK_ENABLE_b : 1;
      __IO uint32_t GNSS_MEM_CLK_ENABLE_b : 1;
      __IO uint32_t CCI_PCLK_ENABLE_b : 1;
      __IO uint32_t CCI_HCLK_ENABLE_b : 1;
      __IO uint32_t CCI_CLK_ENABLE_b : 1;
      __IO uint32_t MASK_HOST_CLK_WAIT_FIX_b : 1;
      __IO uint32_t MASK31_HOST_CLK_CNT_b : 1;
      __IO uint32_t SD_MEM_INTF_CLK_ENABLE_b : 1;
      __IO uint32_t MASK_HOST_CLK_AVAILABLE_FIX_b : 1;
      __IO uint32_t ULPSS_CLK_ENABLE_b : 1;
    } CLK_ENABLE_SET_REG1_b; /*!< BitSize                                                               */
  };
  /*0x04*/
  union {
    __IO uint32_t CLK_ENABLE_CLEAR_REG1; /*!< none                                                                  */

    struct {
      __IO uint32_t USART1_PCLK_ENABLE_b : 1;
      __IO uint32_t USART1_SCLK_ENABLE_b : 1;
      __IO uint32_t USART2_PCLK_ENABLE_b : 1;
      __IO uint32_t USART2_SCLK_ENABLE_b : 1;
      __IO uint32_t RESERVED0 : 1;
      __IO uint32_t RESERVED1 : 1;
      __IO uint32_t RESERVED2 : 1;
      __IO uint32_t RESERVED3 : 1;
      __IO uint32_t RESERVED4 : 1;
      __IO uint32_t SCT_CLK_ENABLE_b : 1;
      __IO uint32_t SCT_PCLK_ENABLE_b : 1;
      __IO uint32_t ICACHE_CLK_ENABLE_b : 1;
      __IO uint32_t ICACHE_CLK_2X_ENABLE_b : 1;
      __IO uint32_t RPDMA_HCLK_ENABLE_b : 1;
      __IO uint32_t SOC_PLL_SPI_CLK_ENABLE_b : 1;
      __IO uint32_t RESERVED5 : 1;
      __IO uint32_t IID_CLK_ENABLE_b : 1;
      __IO uint32_t SDIO_SYS_HCLK_ENABLE_b : 1;
      __IO uint32_t CRC_CLK_ENABLE_b : 1;
      __IO uint32_t M4SS_UM_CLK_STATIC_EN_b : 1;
      __IO uint32_t RESERVED6 : 1;
      __IO uint32_t ETH_HCLK_ENABLE_b : 1;
      __IO uint32_t HWRNG_PCLK_ENABLE_b : 1;
      __IO uint32_t GNSS_MEM_CLK_ENABLE_b : 1;
      __IO uint32_t CCI_PCLK_ENABLE_b : 1;
      __IO uint32_t CCI_HCLK_ENABLE_b : 1;
      __IO uint32_t CCI_CLK_ENABLE_b : 1;
      __IO uint32_t MASK_HOST_CLK_WAIT_FIX_b : 1;
      __IO uint32_t MASK31_HOST_CLK_CNT_b : 1;
      __IO uint32_t SD_MEM_INTF_CLK_ENABLE_b : 1;
      __IO uint32_t MASK_HOST_CLK_AVAILABLE_FIX_b : 1;
      __IO uint32_t ULPSS_CLK_ENABLE_b : 1;

    } CLK_ENABLE_CLR_REG1_b; /*!< BitSize                                                               */
  };
  /*0x08*/
  union {
    __IO uint32_t CLK_ENABLE_SET_REG2; /*!< none                                                                  */
    struct {
      __IO uint32_t GEN_SPI_MST1_HCLK_ENABLE_b : 1;
      __IO uint32_t RESERVED0 : 1;
      __IO uint32_t CAN1_PCLK_ENABLE_b : 1;
      __IO uint32_t CAN1_CLK_ENABLE_b : 1;
      __IO uint32_t RESERVED1 : 1;
      __IO uint32_t RESERVED2 : 1;
      __IO uint32_t UDMA_HCLK_ENABLE_b : 1;
      __IO uint32_t I2C_BUS_CLK_ENABLE_b : 1;
      __IO uint32_t I2C_2_BUS_CLK_ENABLE_b : 1;
      __IO uint32_t SSI_SLV_PCLK_ENABLE_b : 1;
      __IO uint32_t SSI_SLV_SCLK_ENABLE_b : 1;
      __IO uint32_t QSPI_CLK_ENABLE_b : 1;
      __IO uint32_t QSPI_HCLK_ENABLE_b : 1;
      __IO uint32_t I2SM_SCLK_ENABLE_b : 1;
      __IO uint32_t I2SM_INTF_SCLK_ENABLE_b : 1;
      __IO uint32_t I2SM_PCLK_ENABLE_b : 1;
      __IO uint32_t RESERVED3 : 1;
      __IO uint32_t QE_PCLK_ENABLE_b : 1;
      __IO uint32_t MCPWM_PCLK_ENABLE_b : 1;
      __IO uint32_t RESERVED4 : 1;
      __IO uint32_t SGPIO_PCLK_ENABLE_b : 1;
      __IO uint32_t EGPIO_PCLK_ENABLE_b : 1;
      __IO uint32_t ARM_CLK_ENABLE_b : 1;
      __IO uint32_t SSI_MST_PCLK_ENABLE_b : 1;
      __IO uint32_t SSI_MST_SCLK_ENABLE_b : 1;
      __IO uint32_t MEM2_CLK_ENABLE_b : 1;
      __IO uint32_t MEM_CLK_ULP_ENABLE_b : 1;
      __IO uint32_t ROM_CLK_ENABLE_b : 1;
      __IO uint32_t PLL_INTF_CLK_ENABLE_b : 1;
      __IO uint32_t SEMAPHORE_CLK_ENABLE_b : 1;
      __IO uint32_t TOT_CLK_ENABLE_b : 1;
      __IO uint32_t RMII_SOFT_RESET_b : 1;

    } CLK_ENABLE_SET_REG2_b; /*!< BitSize                                                               */
  };
  /*0X0C*/
  union {
    __IO uint32_t CLK_ENABLE_CLEAR_REG2; /*!< none                                                                  */

    struct {
      __IO uint32_t GEN_SPI_MST1_HCLK_ENABLE_b : 1;
      __IO uint32_t RESERVED0 : 1;
      __IO uint32_t CAN1_PCLK_ENABLE_b : 1;
      __IO uint32_t CAN1_CLK_ENABLE_b : 1;
      __IO uint32_t RESERVED1 : 1;
      __IO uint32_t RESERVED2 : 1;
      __IO uint32_t UDMA_HCLK_ENABLE_b : 1;
      __IO uint32_t I2C_BUS_CLK_ENABLE_b : 1;
      __IO uint32_t I2C_2_BUS_CLK_ENABLE_b : 1;
      __IO uint32_t SSI_SLV_PCLK_ENABLE_b : 1;
      __IO uint32_t SSI_SLV_SCLK_ENABLE_b : 1;
      __IO uint32_t QSPI_CLK_ENABLE_b : 1;
      __IO uint32_t QSPI_HCLK_ENABLE_b : 1;
      __IO uint32_t I2SM_SCLK_ENABLE_b : 1;
      __IO uint32_t I2SM_INTF_SCLK_ENABLE_b : 1;
      __IO uint32_t I2SM_PCLK_ENABLE_b : 1;
      __IO uint32_t RESERVED3 : 1;
      __IO uint32_t QE_PCLK_ENABLE_b : 1;
      __IO uint32_t MCPWM_PCLK_ENABLE_b : 1;
      __IO uint32_t RESERVED4 : 1;
      __IO uint32_t SGPIO_PCLK_ENABLE_b : 1;
      __IO uint32_t EGPIO_PCLK_ENABLE_b : 1;
      __IO uint32_t ARM_CLK_ENABLE_b : 1;
      __IO uint32_t SSI_MST_PCLK_ENABLE_b : 1;
      __IO uint32_t SSI_MST_SCLK_ENABLE_b : 1;
      __IO uint32_t MEM2_CLK_ENABLE_b : 1;
      __IO uint32_t MEM_CLK_ULP_ENABLE_b : 1;
      __IO uint32_t ROM_CLK_ENABLE_b : 1;
      __IO uint32_t PLL_INTF_CLK_ENABLE_b : 1;
      __IO uint32_t SEMAPHORE_CLK_ENABLE_b : 1;
      __IO uint32_t TOT_CLK_ENABLE_b : 1;
      __IO uint32_t RMII_SOFT_RESET_b : 1;

    } CLK_ENABLE_CLR_REG2_b; /*!< BitSize                                                               */
  };
  /*0x10*/
  union {
    __IO uint32_t CLK_ENABLE_SET_REG3; /*!< none                                                                  */

    struct {
      __IO uint32_t BUS_CLK_ENABLE_b : 1;
      __IO uint32_t M4_CORE_CLK_ENABLE_b : 1;
      __IO uint32_t CM_BUS_CLK_ENABLE_b : 1;
      __IO uint32_t RESERVED : 1;
      __IO uint32_t MISC_CONFIG_PCLK_ENABLE_b : 1;
      __IO uint32_t EFUSE_CLK_ENABLE_b : 1;
      __IO uint32_t ICM_CLK_ENABLE_b : 1;
      __IO uint32_t MEM1_CLK_ENABLE_b : 1;
      __IO uint32_t MEM3_CLK_ENABLE_b : 1;
      __IO uint32_t RESERVED1 : 1;
      __IO uint32_t RESERVED2 : 1;
      __IO uint32_t RESERVED3 : 1;
      __IO uint32_t USB_PHY_CLK_IN_ENABLE_b : 1;
      __IO uint32_t QSPI_CLK_ONEHOT_ENABLE_b : 1;
      __IO uint32_t QSPI_M4_SOC_SYNC_b : 1;
      __IO uint32_t RESERVED4 : 1;
      __IO uint32_t EGPIO_CLK_ENABLE_b : 1;
      __IO uint32_t I2C_CLK_ENABLE_b : 1;
      __IO uint32_t I2C_2_CLK_ENABLE_b : 1;
      __IO uint32_t EFUSE_PCLK_ENABLE_b : 1;
      __IO uint32_t SGPIO_CLK_ENABLE_b : 1;
      __IO uint32_t TASS_M4SS_64K_SWITCH_CLK_ENABLE_b : 1;
      __IO uint32_t TASS_M4SS_128K_SWITCH_CLK_ENABLE_b : 1;
      __IO uint32_t TASS_M4SS_SDIO_SWITCH_CLK_ENABLE_b : 1;
      __IO uint32_t TASS_M4SS_USB_SWITCH_CLK_ENABLE_b : 1;
      __IO uint32_t ROM_MISC_STATIC_ENABLE_b : 1;
      __IO uint32_t M4_SOC_CLK_FOR_OTHER_ENABLE_b : 1;
      __IO uint32_t ICACHE_ENABLE_b : 1;
      __IO uint32_t RES : 4;
    } CLK_ENABLE_SET_REG3_b; /*!< BitSize                                                               */
  };
  /*0x14*/
  union {
    __IO uint32_t CLK_ENABLE_CLEAR_REG3; /*!< none                                                                  */

    struct {
      __IO uint32_t BUS_CLK_ENABLE_b : 1;
      __IO uint32_t M4_CORE_CLK_ENABLE_b : 1;
      __IO uint32_t CM_BUS_CLK_ENABLE_b : 1;
      __IO uint32_t RESERVED : 1;
      __IO uint32_t MISC_CONFIG_PCLK_ENABLE_b : 1;
      __IO uint32_t EFUSE_CLK_ENABLE_b : 1;
      __IO uint32_t ICM_CLK_ENABLE_b : 1;
      __IO uint32_t MEM1_CLK_ENABLE_b : 1;
      __IO uint32_t MEM3_CLK_ENABLE_b : 1;
      __IO uint32_t I2SS_INTF_SCLK_ENABLE_b : 1;
      __IO uint32_t RESERVED2 : 1;
      __IO uint32_t I2SS_SCLK_ENABLE_b : 1;
      __IO uint32_t USB_PHY_CLK_IN_ENABLE_b : 1;
      __IO uint32_t QSPI_CLK_ONEHOT_ENABLE_b : 1;
      __IO uint32_t QSPI_M4_SOC_SYNC_b : 1;
      __IO uint32_t RESERVED4 : 1;
      __IO uint32_t EGPIO_CLK_ENABLE_b : 1;
      __IO uint32_t I2C_CLK_ENABLE_b : 1;
      __IO uint32_t I2C_2_CLK_ENABLE_b : 1;
      __IO uint32_t EFUSE_PCLK_ENABLE_b : 1;
      __IO uint32_t SGPIO_CLK_ENABLE_b : 1;
      __IO uint32_t TASS_M4SS_64K_SWITCH_CLK_ENABLE_b : 1;
      __IO uint32_t TASS_M4SS_128K_SWITCH_CLK_ENABLE_b : 1;
      __IO uint32_t TASS_M4SS_SDIO_SWITCH_CLK_ENABLE_b : 1;
      __IO uint32_t TASS_M4SS_USB_SWITCH_CLK_ENABLE_b : 1;
      __IO uint32_t ROM_MISC_STATIC_ENABLE_b : 1;
      __IO uint32_t M4_SOC_CLK_FOR_OTHER_ENABLE_b : 1;
      __IO uint32_t ICACHE_ENABLE_b : 1;
      __IO uint32_t RES : 4;
    } CLK_ENABLE_CLR_REG3_b; /*!< BitSize                                                               */
  };
  /*0x18*/
  union {
    __IO uint32_t CLK_CONFIG_REG1; /*!< Clk Config Register1                                                  */

    struct {
      __IO uint32_t QSPI_CLK_SEL : 3;             //’D0
      __IO uint32_t QSPI_CLK_DIV_FAC : 6;         //’D1
      __IO uint32_t QSPI_CLK_SWALLOW_SEL : 1;     //’B 0
      __IO uint32_t RESERVED : 1;                 //’B 0
      __IO uint32_t SSI_MST_SCLK_DIV_FAC : 4;     //’D1
      __IO uint32_t SSI_MST_SCLK_SEL : 3;         //’D1
      __IO uint32_t PLL_INTF_CLK_SEL : 1;         //’B 0
      __IO uint32_t PLL_INTF_CLK_DIV_FAC : 4;     //’D2
      __IO uint32_t PLL_INTF_CLK_SWALLOW_SEL : 1; //’B 0
      __IO uint32_t GEN_SPI_MST1_SCLK_SEL : 3;    //’B0
      __IO uint32_t RESERVED1 : 5;

    } CLK_CONFIG_REG1_b; /*!< BitSize                                                               */
  };
  /*0x1C*/
  union {
    __IO uint32_t CLK_CONFIG_REG2; /*!< Clk Config Register2                                                  */

    struct {
      __IO uint32_t USART1_SCLK_SEL : 3;      //’D0
      __IO uint32_t USART1_SCLK_DIV_FAC : 4;  //’D1
      __IO uint32_t USART2_SCLK_SEL : 3;      //’D0
      __IO uint32_t USART2_SCLK_DIV_FAC : 4;  //’D1
      __IO uint32_t RESERVED3 : 3;            //’D0
      __IO uint32_t RESERVED2 : 4;            //’D1
      __IO uint32_t RESERVED1 : 2;            //’D0
      __IO uint32_t RESERVED : 1;             //’B0
      __IO uint32_t CCI_CLK_DIV_FAC : 4;      //’D1
      __IO uint32_t QSPI_ODD_DIV_SEL : 1;     //’B0
      __IO uint32_t USART1_SCLK_FRAC_SEL : 1; //’B0
      __IO uint32_t USART2_SCLK_FRAC_SEL : 1; //’B0
      __IO uint32_t RESERVED0 : 1;            //’B0

    } CLK_CONFIG_REG2_b; /*!< BitSize                                                               */
  };
  /*0x20*/
  union {
    __IO uint32_t CLK_CONFIG_REG3; /*!< Clk Config Register3                                                  */

    struct {
      __IO uint32_t CAN1_CLK_DIV_FAC : 8; //’D1
      __IO uint32_t MCU_CLKOUT_SEL : 4;
      __IO uint32_t MCU_CLKOUT_DIV_FAC : 6;
      __IO uint32_t MCU_CLKOUT_ENABLE : 1;
      __IO uint32_t RESERVED : 13;

    } CLK_CONFIG_REG3_b; /*!< BitSize                                                               */
  };
  /*0x24*/
  union {
    __IO uint32_t CLK_CONFIG_REG4; /*!< Clk Config Register4                                                  */

    struct {
      __IO uint32_t SOC_PLL_CLK_BYP_SEL : 2;
      __IO uint32_t I2S_PLL_CLK_BYP_SEL : 2;   //’D3
      __IO uint32_t MODEM_PLL_CLK_BYP_SEL : 2; //’D1
      __IO uint32_t INTF_PLL_CLK_BYP_SEL : 2;
      __IO uint32_t SOC_INTF_PLL_BYPCLK_CLKCLNR_ON : 1;
      __IO uint32_t SOC_INTF_PLL_BYPCLK_CLKCLNR_OFF : 1;
      __IO uint32_t RESERVED1 : 1;
      __IO uint32_t RESERVED2 : 1;
      __IO uint32_t I2S_PLL_BYPCLK_CLKCLNR_ON : 1;
      __IO uint32_t I2S_PLL_BYPCLK_CLKCLNR_OFF : 1;
      __IO uint32_t MODEM_PLL_BYPCLK_CLKCLNR_ON : 1;
      __IO uint32_t MODEM_PLL_BYPCLK_CLKCLNR_OFF : 1;
      __IO uint32_t BYPASS_SOC_PLL_CLK : 1;
      __IO uint32_t BYPASS_I2S_PLL_CLK : 1;
      __IO uint32_t BYPASS_MODEM_PLL_CLK1 : 1;
      __IO uint32_t BYPASS_MODEM_PLL_CLK2 : 1;
      __IO uint32_t BYPASS_INTF_PLL_CLK : 1;
      __IO uint32_t SLEEP_CLK_SEL : 2; //'H0
      __IO uint32_t USB_SYSCLK_CLKCLNR_ON : 1;
      __IO uint32_t USB_SYSCLK_CLKCLNR_OFF : 1;
      __IO uint32_t ULPSS_CLK_DIV_FAC : 6;
      __IO uint32_t CCI_CLK_SEL : 1; //’B0

    } CLK_CONFIG_REG4_b; /*!< BitSize                                                               */
  };
  /*0x28*/
  union {
    __IO uint32_t CLK_CONFIG_REG5; /*!< Clk Config Register5                                                  */

    struct {
      __IO uint32_t M4_SOC_CLK_SEL : 4;      // ’D0
      __IO uint32_t M4_SOC_CLK_DIV_FAC : 6;  //’D1
      __IO uint32_t I2S_CLK_SEL : 1;         //’B 0
      __IO uint32_t I2S_CLK_DIV_FAC : 6;     //’D1
      __IO uint32_t SCT_CLK_SEL : 3;         //’D0
      __IO uint32_t SCT_CLK_DIV_FAC : 6;     // ’D1
      __IO uint32_t M4_SOC_HOST_CLK_SEL : 1; // ’B0
      __I uint32_t RESERVED : 1;             //’B 0
      __IO uint32_t ULPSS_ODD_DIV_SEL : 1;   //’B0
      __IO uint32_t USB_CLK_SEL : 2;         // ’B0
      __IO uint32_t RESERVED1 : 1;
    } CLK_CONFIG_REG5_b; //28              /*!< BitSize                                                               */
  };

  __IO uint32_t RESERVED0; //0x2C
  __IO uint32_t RESERVED1; //0x30
  __IO uint32_t RESERVED2; //0x34
  __IO uint32_t RESERVED3; //0x38
  __IO uint32_t RESERVED4; //0x3C
  /*0x40*/
  union {
    __IO uint32_t SD_MEM_CLOCK_REG; /*!< SDMEM clock Register                                                  */

    struct {
      __IO uint32_t SD_MEM_INTF_CLK_DIV_FAC : 6;     //’D8
      __IO uint32_t SD_MEM_INTF_CLK_SEL : 3;         //’D0
      __IO uint32_t SD_MEM_INTF_CLK_SWALLOW_SEL : 1; //’B ; // 0
      __IO uint32_t RESERVED : 22;
    } SD_MEM_CLOCK_REG_b; /*!< BitSize                                                               */
  };
  /*0x44*/
  union {
    __IO uint32_t
      DYN_CLK_GATE_DISABLE_REG; /*!< Dynamic Clk Ctrl Disable Register                                     */

    struct {
      __IO uint32_t SDIO_SYS_HCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t BUS_CLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t RESERVED0 : 1;
      __IO uint32_t RESERVED1 : 1;
      __IO uint32_t GPDMA_HCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t EGPIO_PCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t SGPIO_PCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t TOT_CLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t HWRNG_PCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t USART1_SCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t USART1_PCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t USART2_SCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t USART2_PCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t RESERVED2 : 1;
      __IO uint32_t RESERVED3 : 1;
      __IO uint32_t SSI_SLV_SCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t SSI_SLV_PCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t RESERVED : 1;
      __IO uint32_t I2SM_INTF_SCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t SEMAPHORE_CLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t ARM_CLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t SSI_MST_SCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t MEM1_CLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t MEM2_CLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t MEM_CLK_ULP_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t MEM3_CLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t RESERVED4 : 1;
      __IO uint32_t RESERVED5 : 1;
      __IO uint32_t SSI_MST_PCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t ICACHE_DYN_GATING_DISABLE_b : 1;
      __IO uint32_t CCI_PCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t MISC_CONFIG_PCLK_DYN_CTRL_DISABLE_b : 1;
    } DYN_CLK_GATE_DISABLE_REG_b; /*!< BitSize                                                               */
  };

  __I uint32_t RESERVED5; //0x48
  __I uint32_t RESERVED6; //0x4C
  /*0x50*/
  union {
    __IO uint32_t PLL_ENABLE_SET_REG; /*!< PLL Enable SET Register                                               */

    struct {
      __IO uint32_t SOCPLL_SPI_SW_RESET : 1; //’B0
      __IO uint32_t RESERVED : 31;
    } PLL_ENABLE_SET_REG_b; /*!< BitSize                                                               */
  };
  /*0x54*/
  union {
    __IO uint32_t PLL_ENABLE_CLR_REG; /*!< PLL Enable Clear Register                                             */

    struct {
      __IO uint32_t SOCPLL_SPI_SW_RESET : 1; //’B0
      __IO uint32_t RESERVED : 31;
    } PLL_ENABLE_CLR_REG_b; /*!< BitSize                                                               */
  };
  /*0x58*/
  union {
    __O uint32_t PLL_STAT_REG; /*!< PLL Status Register                                                   */

    struct {
      __O uint32_t LCDPLL_LOCK : 1;
      __O uint32_t DDRPLL_LOCK : 1;
      __O uint32_t APPLL_LOCK : 1;
      __O uint32_t INTFPLL_LOCK : 1;
      __O uint32_t I2SPLL_LOCK : 1;
      __O uint32_t SOCPLL_LOCK : 1;
      __O uint32_t MODEMPLL_LOCK : 1;
      __O uint32_t PLL_LOCK_DATA_TRIG : 1;
      __O uint32_t M4_SOC_CLK_SWITCHED : 1;
      __O uint32_t QSPI_CLK_SWITCHED : 1;
      __O uint32_t USART1_SCLK_SWITCHED : 1;
      __O uint32_t USART2_SCLK_SWITCHED : 1;
      __O uint32_t GEN_SPI_MST1_SCLK_SWITCHED : 1;
      __O uint32_t SSI_MST_SCLK_SWITCHED : 1;
      __O uint32_t SD_MEM_INTF_CLK_SWITCHED : 1;
      __O uint32_t SCT_CLK_SWITCHED : 1;
      __O uint32_t M4_TA_SOC_CLK_SWITCHED_SDIO : 1;
      __O uint32_t I2S_CLK_SWITCHED : 1;
      __O uint32_t PLL_INTF_CLK_SWITCHED : 1;
      __O uint32_t RESERVED0 : 1;
      __O uint32_t RESERVED1 : 1;
      __O uint32_t SLEEP_CLK_SWITCHED : 1;
      __O uint32_t MCU_CLKOUT_SWITCHED : 1;
      __O uint32_t CCI_CLK_SWITCHED : 1;
      __O uint32_t M4_TA_SOC_CLK_SWITCHED_USB : 1;
      __O uint32_t CC_CLOCK_MUX_SWITCHED : 1;
      __O uint32_t TASS_M4SS_64K_CLK_SWITCHED : 1;
      __O uint32_t USART1_CLK_SWITCHED : 1;
      __O uint32_t USART2_CLK_SWITCHED : 1;
      __O uint32_t TASS_M4SS_128K_CLK_SWITCHED : 1;
      __O uint32_t CLK_FREE_OR_SLP_SWITCHED : 1;
      __O uint32_t ULP_REF_CLK_SWITCHED : 1;
    } PLL_STAT_REG_b; /*!< BitSize                                                               */
  };
  /*0x5C*/
  union {
    __IO uint32_t PLL_LOCK_INT_MASK_REG; /*!< PLL Lock Interrupt Mask Register                                      */

    struct {
      __IO uint32_t LCD_PLL_LOCK_MASK_BIT_OF_RISING_EDGE : 1;
      __IO uint32_t DDR_PLL_LOCK_MASK_BIT_OF_RISING_EDGE : 1;
      __IO uint32_t AP_PLL_LOCK_MASK_BIT_OF_RISING_EDGE : 1;
      __IO uint32_t INTF_PLL_LOCK_MASK_BIT_OF_RISING_EDGE : 1;
      __IO uint32_t I2S_PLL_LOCK_MASK_BIT_OF_RISING_EDGE : 1;
      __IO uint32_t SOC_PLL_LOCK_MASK_BIT_OF_RISING_EDGE : 1;
      __IO uint32_t MODEM_PLL_LOCK_MASK_BIT_OF_RISING_EDGE : 1;
      __IO uint32_t PLL_LOCK_DATA_TRIGGER_MASK_BIT_OF_RISING_EDGE : 1;
      __IO uint32_t LCD_PLL_LOCK_MASK_BIT_OF_FALLING_EDGE : 1;
      __IO uint32_t DDR_PLL_LOCK_MASK_BIT_OF_FALLING_EDGE : 1;
      __IO uint32_t AP_PLL_LOCK_MASK_BIT_OF_FALLING_EDGE : 1;
      __IO uint32_t INTF_PLL_LOCK_MASK_BIT_OF_FALLING_EDGE : 1;
      __IO uint32_t I2S_PLL_LOCK_MASK_BIT_OF_FALLING_EDGE : 1;
      __IO uint32_t SOC_PLL_LOCK_MASK_BIT_OF_FALLING_EDGE : 1;
      __IO uint32_t MODEM_PLL_LOCK_MASK_BIT_OF_FALLING_EDGE : 1;
      __IO uint32_t PLL_LOCK_DATA_TRIGGER_MASK_BIT_OF_FALLING_EDGE : 1;
      __IO uint32_t RESERVED : 16;

    } PLL_LOCK_INT_MASK_REG_b; /*!< BitSize                                                               */
  };
  /*0x60*/
  union {
    __IO uint32_t PLL_LOCK_INT_CLR_REG; /*!< PLL Lock Interrupt Clear Register                                     */

    struct {
      __IO uint32_t LCD_PLL_LOCK_CLEAR_BIT_OF_RISING_EDGE : 1;
      __IO uint32_t DDR_PLL_LOCK_CLEAR_BIT_OF_RISING_EDGE : 1;
      __IO uint32_t AP_PLL_LOCK_CLEAR_BIT_OF_RISING_EDGE : 1;
      __IO uint32_t INTF_PLL_LOCK_CLEAR_BIT_OF_RISING_EDGE : 1;
      __IO uint32_t I2S_PLL_LOCK_CLEAR_BIT_OF_RISING_EDGE : 1;
      __IO uint32_t SOC_PLL_LOCK_CLEAR_BIT_OF_RISING_EDGE : 1;
      __IO uint32_t MODEM_PLL_LOCK_CLEAR_BIT_OF_RISING_EDGE : 1;
      __IO uint32_t PLL_LOCK_DATA_TRIGGER_CLEAR_BIT_OF_RISING_EDGE : 1;
      __IO uint32_t LCD_PLL_LOCK_CLEAR_BIT_OF_FALLING_EDGE : 1;
      __IO uint32_t DDR_PLL_LOCK_CLEAR_BIT_OF_FALLING_EDGE : 1;
      __IO uint32_t AP_PLL_LOCK_CLEAR_BIT_OF_FALLING_EDGE : 1;
      __IO uint32_t INTF_PLL_LOCK_CLEAR_BIT_OF_FALLING_EDGE : 1;
      __IO uint32_t I2S_PLL_LOCK_CLEAR_BIT_OF_FALLING_EDGE : 1;
      __IO uint32_t SOC_PLL_LOCK_CLEAR_BIT_OF_FALLING_EDGE : 1;
      __IO uint32_t MODEM_PLL_LOCK_CLEAR_BIT_OF_FALLING_EDGE : 1;
      __IO uint32_t PLL_LOCK_DATA_TRIGGER_CLEAR_BIT_OF_FALLING_EDGE : 1;
      __IO uint32_t RESERVED : 16;
    } PLL_LOCK_INT_CLR_REG_b; /*!< BitSize                                                               */
  };

  /*0x64*/
  union {
    __IO uint32_t PLL_LOCK_INT_DATA_REG; /*!< PLL Lock Interrupt Data Register                                      */

    struct {
      __IO uint32_t LCD_PLL_LOCK : 1;
      __IO uint32_t DDR_PLL_LOCK : 1;
      __IO uint32_t AP_PLL_LOCK : 1;
      __IO uint32_t INTF_PLL_LOCK : 1;
      __IO uint32_t I2S_PLL_LOCK : 1;
      __IO uint32_t SOC_PLL_LOCK : 1;
      __IO uint32_t MODEM_PLL_LOCK : 1;
      __IO uint32_t RESERVED : 25;
    } PLL_LOCK_INT_DATA_REG_b; /*!< BitSize                                                               */
  };
  /*0x68*/
  union {
    __IO uint32_t SLEEP_CALIB_REG; /*!< Sleep Calibration Register                                            */

    struct {
      __IO uint32_t SLP_CALIB_START_b : 1;     // ’B 0
      __IO uint32_t SLP_CALIB_CYCLES : 2;      //’D0
      __IO uint32_t SLP_CALIB_DURATION_b : 16; //NA
      __IO uint32_t SLP_CALIB_DONE_b : 1;      // NA
      __IO uint32_t RES : 12;                  // NA
    } SLEEP_CALIB_REG_b; /*!< BitSize                                                               */
  };
  /*0x6C*/
  union {
    __IO uint32_t CLK_CALIB_CTRL_REG1; /*!< Clock Calibration Control Register1                                   */

    struct {
      __IO uint32_t CC_SOFT_RST_b : 1;        //’B 0
      __IO uint32_t CC_START_b : 1;           //’B 0
      __IO uint32_t CC_CHANGE_TEST_CLK_b : 1; //’B 0
      __IO uint32_t CC_CLKIN_SEL_b : 4;       //’D0
      __IO uint32_t RESERVED : 25;
    } CLK_CALIB_CTRL_REG1_b; /*!< BitSize                                                               */
  };
  /*0x70*/
  union {
    __IO uint32_t CLK_CALIB_CTRL_REG2; /*!< Clock Calibration Control Register2                                   */

    struct {
      __IO uint32_t CC_NUM_REF_CLKS : 32;
    } CLK_CALIB_CTRL_REG2_b; /*!< BitSize                                                               */
  };
  /*0x74*/
  union {
    __O uint32_t CLK_CALIB_STS_REG1; /*!< Clock Calibration Status Register1                                    */

    struct {
      __O uint32_t CC_DONE_b : 1;  // ’B 0
      __O uint32_t CC_ERROR_b : 1; //’B 0
      __IO uint32_t RESERVED : 30;
    } CLK_CALIB_STS_REG1_b; /*!< BitSize                                                               */
  };
  /*0x78*/
  union {
    __I uint32_t CLK_CALIB_STS_REG2; /*!< Clock Calibration Status Register1                                    */

    struct {
      __IO uint32_t CC_NUM_TEST_CLKS : 32;
    } CLK_CALIB_STS_REG2_b; /*!< BitSize                                                               */
  };
  /*0x7C*/
  union {
    __IO uint32_t CLK_CONFIG_REG6; /*!< Clock Calibration Status Register1                                    */

    struct {
      __IO uint32_t IID_KH_CLK_DIV_FAC : 3;  //’D1
      __IO uint32_t USB_PHY_CLK_DIV_FAC : 2; //’D1
      __IO uint32_t PADCFG_PCLK_DIV_FAC : 4;
      __IO uint32_t RESERVED : 23;
    } CLK_CONFIG_REG6_b; /*!< BitSize                                                               */
  };
  /*0x80*/
  union {
    __I uint32_t
      DYN_CLK_GATE_DISABLE_REG2; /*!< Clock Calibration Status Register1                                    */

    struct {
      __IO uint32_t SOC_PLL_SPI_CLK_DYN_CTRL_DISABLE_b : 1; //’B 0
      __IO uint32_t I2C_BUS_DYN_CTRL_DISABLE_b : 1;         //’B 0
      __IO uint32_t I2C_2_BUS_CLK_DYN_CTRL_DISABLE_b : 1;   //’B 0
      __IO uint32_t SCT_PCLK_DYN_CTRL_DISABLE_b : 1;        //’B 0
      __IO uint32_t CAN1_PCLK_DYN_CTRL_DISABLE_b : 1;       //’B 0
      __IO uint32_t I2SM_PCLK_DYN_CTRL_DISABLE_b : 1;       //’B 0
      __IO uint32_t EFUSE_CLK_DYN_CTRL_DISABLE_b : 1;       //’B 0
      __IO uint32_t EFUSE_PCLK_DYN_CTRL_DISABLE_b : 1;      //’B 0
      __IO uint32_t PWR_CTRL_CLK_DYN_CTRL_DISABLE_b : 1;    //’B 1
      __IO uint32_t RESERVED : 23;
    } DYN_CLK_GATE_DISABLE_REG2_b; /*!< BitSize                                                               */
  };
  /*0x84*/
  union {
    __IO uint32_t PLL_LOCK_INT_STATUS_REG; /*!< PLL Lock Interrupt Data Register                                      */

    struct {
      __IO uint32_t LCD_PLL_LOCK_OF_RISING_EDGE : 1;
      __IO uint32_t DDR_PLL_LOCK_OF_RISING_EDGE : 1;
      __IO uint32_t AP_PLL_LOCK_OF_RISING_EDGE : 1;
      __IO uint32_t INTF_PLL_LOCK_OF_RISING_EDGE : 1;
      __IO uint32_t I2S_PLL_LOCK_OF_RISING_EDGE : 1;
      __IO uint32_t SOC_PLL_LOCK_OF_RISING_EDGE : 1;
      __IO uint32_t MODEM_PLL_LOCK_OF_RISING_EDGE : 1;
      __IO uint32_t PLL_LOCK_DATA_TRIGGER_INTR_OF_RISING_EDGE : 1;
      __IO uint32_t LCD_PLL_LOCK_OF_FALLING_EDGE : 1;
      __IO uint32_t DDR_PLL_LOCK_OF_FALLING_EDGE : 1;
      __IO uint32_t AP_PLL_LOCK_OF_FALLING_EDGE : 1;
      __IO uint32_t INTF_PLL_LOCK_OF_FALLING_EDGE : 1;
      __IO uint32_t I2S_PLL_LOCK_OF_FALLING_EDGE : 1;
      __IO uint32_t SOC_PLL_LOCK_OF_FALLING_EDGE : 1;
      __IO uint32_t MODEM_PLL_LOCK_OF_FALLING_EDGE : 1;
      __IO uint32_t PLL_LOCK_DATA_TRIGGER_INTR_OF_FALLING_EDGE : 1;
      __IO uint32_t RESERVED : 16;
    } PLL_LOCK_INT_STATUS_REG_b; /*!< BitSize                                                               */
  };

} M4Clk_Type;

/*Time period*/

typedef struct {

  /*0x00*/
  union {
    __IO uint32_t
      MCU_CAL_RO_TIMEPERIOD_READ; /*!< none                                                                  */

    struct {
      __IO uint32_t TIMEPERIOD_RO : 25; /*!<Calibrated RO timeperiod    */
      __IO uint32_t RESERVED : 7;       /*!<    */
    } MCU_CAL_RO_TIMEPERIOD_READ_b;     /*!< BitSize                                                               */
  };
  /*0x04*/
  union {
    __IO uint32_t
      MCU_CAL_TIMER_CLOCK_PERIOD; /*!< none                                                                  */

    struct {
      __IO uint32_t RTC_TIMER_CLK_PERIOD : 25;              /*!<Calibrated RO timeperiod    */
      __IO uint32_t RESERVED : 6;                           /*!<    */
      __IO uint32_t SPI_RTC_TIMER_CLK_PERIOD_APPLIED_b : 1; /*!<    */
    } MCU_CAL_TIMER_CLOCK_PERIOD_b; /*!< BitSize                                                               */
  };

  /*0x08*/
  union {
    __IO uint32_t MCU_CAL_TEMP_PROG_REG; /*!< none                                                                  */

    struct {
      __IO uint32_t BYPASS_CALIB_PG : 1; /*!<Calibrated RO timeperiod    */
      __IO uint32_t RESERVED1 : 15;      /*!<    */
      __IO uint32_t MAX_TEMP_CHANGE : 5; /*!<    */
      __IO uint32_t TEMP_TRIGGER_TIME_SEL : 2;
      __IO uint32_t PERIODIC_TEMP_CALIB_EN : 1;
      __IO uint32_t RTC_TIMER_PERIOD_MUX_SEL : 1;
      __IO uint32_t RESERVED2 : 7;
    } MCU_CAL_TEMP_PROG_REG_b; /*!< BitSize                                                               */
  };

  /*0x0C*/
  union {
    __IO uint32_t MCU_CAL_START_REG; /*!< none                                                                  */

    struct {
      __IO uint32_t ALPHA_RO : 3;      /*!<Calibrated RO timeperiod    */
      __IO uint32_t ALPHA_RC : 3;      /*!<    */
      __IO uint32_t NO_OF_RO_CLKS : 4; /*!<    */
      __IO uint32_t NO_OF_RC_CLKS : 3;
      __IO uint32_t RC_SETTLE_TIME : 3;
      __IO uint32_t RO_TRIGGER_TIME_SEL : 2;
      __IO uint32_t RC_TRIGGER_TIME_SEL : 3;
      __IO uint32_t PERIODIC_RO_CALIB_EN : 1;
      __IO uint32_t PERIODIC_RC_CALIB_EN : 1;
      __IO uint32_t START_CALIB_RO : 1;
      __IO uint32_t START_CALIB_RC : 1;
      __IO uint32_t RC_XTAL_MUX_SEL : 1;
      __IO uint32_t LOW_POWER_TRIGGER_SEL : 1;
      __IO uint32_t VBATT_TRIGGER_TIME_SEL : 3;
      __IO uint32_t RESERVED2 : 2;
    } MCU_CAL_START_REG_b; /*!< BitSize                                                               */
  };
  /*0x10*/
  union {
    __IO uint32_t
      MCU_CAL_REF_CLK_SETTLE_REG; /*!< none                                                                  */

    struct {
      __IO uint32_t XTAL_SETTLE : 7; /*!<Calibrated RO timeperiod    */
      __IO uint32_t RESERVED1 : 9;
      __IO uint32_t VALID_RC_TIMEPERIOD : 1;
      __IO uint32_t VALID_RO_TIMEPERIOD : 1;
      __IO uint32_t RESERVED2 : 14;
    } MCU_CAL_REF_CLK_SETTLE_REG_b; /*!< BitSize                                                               */
  };

  /*0x14*/
  union {
    __IO uint32_t
      MCU_CAL_RC_TIMEPERIOD_READ; /*!< none                                                                  */

    struct {
      __IO uint32_t TIMEPERIOD_RC : 25; /*!<Calibrated RO timeperiod    */
      __IO uint32_t RESERVED : 7;
    } MCU_CAL_RC_TIMEPERIOD_READ_b; /*!< BitSize                                                               */
  };

  /*0x18*/
  union {
    __IO uint32_t
      MCU_CAL_REF_CLK_TIEMPERIOD_REG; /*!< none                                                                  */

    struct {
      __IO uint32_t TIMEPERIOD_REF_CLK : 24; /*!<Calibrated RO timeperiod    */
      __IO uint32_t RESERVED : 8;
    } MCU_CAL_REF_CLK_TIEMPERIOD_REG_b; /*!< BitSize                                                               */
  };

} TimePeriodCalib_t;

/*End of time period */

/*MCU WDT */

typedef struct {

  /*0x00*/
  union {
    __IO uint32_t MCU_WWD_INTERRUPT_TIMER; /*!< none                                                                  */

    struct {
      __IO uint32_t WWD_INTERRUPT_TIMER : 5; /*!<    */
      __IO uint32_t RESERVED1 : 27;          /*!<    */
    } MCU_WWD_INTERRUPT_TIMER_b; /*!< BitSize                                                               */
  };

  /*0x04*/
  union {
    __IO uint32_t
      MCU_WWD_SYSTEM_RESET_TIMER; /*!< none                                                                  */

    struct {
      __IO uint32_t WWD_SYSTEM_RESET_TIMER : 5; /*!<    */
      __IO uint32_t RESERVED1 : 27;             /*!<    */
    } MCU_WWD_SYSTEM_RESET_TIMER_b; /*!< BitSize                                                               */
  };

  /*0x08*/
  union {
    __IO uint32_t MCU_WWD_WINDOW_TIMER; /*!< none                                                                  */

    struct {
      __IO uint32_t WINDOW_TIMER : 4; /*!<    */
      __IO uint32_t RESERVED1 : 28;   /*!<    */
    } MCU_WWD_WINDOW_TIMER_b;         /*!< BitSize                                                               */
  };

  /*0x0C*/
  union {
    __IO uint32_t MCU_WWD_ARM_STUCK_EN; /*!< none                                                                  */

    struct {
      __IO uint32_t RESERVED1 : 16;
      __IO uint32_t PROCESSOR_STUCK_RESET_EN : 1;
      __IO uint32_t RESERVED2 : 15;
    } MCU_WWD_TIMER_SETTINGS_b; /*!< BitSize                                                               */
  };

  /*0x10*/
  union {
    __IO uint32_t MCU_WWD_MODE_AND_RSTART; /*!< none                                                                  */

    struct {
      __IO uint32_t WWD_MODE_RSTART : 1; /*!<    */
      __IO uint32_t RESERVED1 : 15;
      __IO uint32_t WWD_MODE_EN_STATUS : 8;
      __IO uint32_t RESERVED2 : 8;
    } MCU_WWD_MODE_AND_RSTART_b; /*!< BitSize                                                               */
  };

  /*0x14*/ __IO uint32_t RESERVED2;
  /*0x18*/
  union {
    __IO uint32_t MCU_WWD_KEY_ENABLE; /*!< none                                                                  */

    struct {
      __IO uint32_t WWD_RESET_BYPASS : 1; /*!<    */
      __IO uint32_t WWD_RESET_ULP_BYPAS : 1;
      __IO uint32_t RESERVED : 30;
    } MCU_WWD_KEY_ENABLE_b; /*!< BitSize                                                               */
  };
} MCU_WDT_t;

/*end of MCU WDT */

/*MCU Calender */
typedef struct {

  /*0x00*/
  __IO uint32_t RESERVED1;
  /*0x04*/
  __IO uint32_t RESERVED2;
  /*0x08*/
  __IO uint32_t RESERVED3;
  /*0x0C*/
  __IO uint32_t RESERVED4;
  /*0x10*/
  __IO uint32_t RESERVED5;
  /*0x14*/
  __IO uint32_t RESERVED6;
  /*0x18*/
  __IO uint32_t RESERVED7;
  /*0x1C*/
  union {
    __IO uint32_t MCU_CAL_ALARM_PROG_1; /*!< none                                                                  */

    struct {
      __IO uint32_t PROG_ALARM_MSEC : 10;
      __IO uint32_t PROG_ALARM_SEC : 6;
      __IO uint32_t PROG_ALARM_MIN : 6;
      __IO uint32_t PROG_ALARM_HOUR : 5;
      __IO uint32_t RESERVED1 : 5;
    } MCU_CAL_ALARM_PROG_1_b; /*!< BitSize                                                               */
  };
  /*0x20*/
  union {
    __IO uint32_t MCU_CAL_ALARM_PROG_2; /*!< none                                                                  */

    struct {
      __IO uint32_t PROG_ALARM_DAY : 5;
      __IO uint32_t RESERVED1 : 3;
      __IO uint32_t PROG_ALARM_MONTH : 4;
      __IO uint32_t RESERVED2 : 4;
      __IO uint32_t PROG_ALARM_YEAR : 7;
      __IO uint32_t PROG_ALARM_CENTURTY : 2;
      __IO uint32_t RESERVED3 : 6;
      __IO uint32_t ALARM_EN : 1;
    } MCU_CAL_ALARM_PROG_2_b; /*!< BitSize                                                               */
  };
  /*0x24*/
  union {
    __IO uint32_t MCU_CAL_POWERGATE_REG;
    struct {
      __IO uint32_t PG_EN_CALENDER : 1;
      __IO uint32_t ENABLE_CALENDER_COMBI : 1;
      __IO uint32_t RES : 30;
    } MCU_CAL_POWERGATE_REG_b;
  };
  /*0x28*/
  union {
    __IO uint32_t MCU_CAL_PROG_TIME_1; /*!< none                                                                  */
    struct {
      __IO uint32_t PROG_MSEC : 10;
      __IO uint32_t PROG_SEC : 6;
      __IO uint32_t PROG_MIN : 6;
      __IO uint32_t PROG_HOUR : 5;
      __IO uint32_t RESERVED2 : 5;
    } MCU_CAL_PROG_TIME_1_b; /*!< BitSize                                                               */
  };
  /*0x2C*/
  union {
    __IO uint32_t MCU_CAL_PROG_TIME_2; /*!< none                                                                  */
    struct {
      __IO uint32_t PROG_DAY : 5; /*!<    */
      __IO uint32_t PROG_WEEK_DAY : 3;
      __IO uint32_t PROG_MONTH : 4;
      __IO uint32_t RES : 4;
      __IO uint32_t PROG_YEAR : 7;
      __IO uint32_t PROG_CENTURY : 2;
      __IO uint32_t RESERVED1 : 6;
      __IO uint32_t PROG_TIME_TRIG : 1;
    } MCU_CAL_PROG_TIME_2_b; /*!< BitSize                                                               */
  };
  /*0x30*/
  union {
    __IO uint32_t MCU_CAL_READ_TIME_MSB; /*!< none                                                                  */
    struct {
      __IO uint32_t WEEK_DAY : 3;
      __IO uint32_t MONTHS_COUNT : 4;
      __IO uint32_t YEAR_COUNT : 7;
      __IO uint32_t CENTURY_COUNT : 2;
      __IO uint32_t RESERVED : 16;
    } MCU_CAL_READ_TIME_MSB_b; /*!< BitSize                                                               */
  };
  /*0x34*/
  union {
    __IO uint32_t MCU_CAL_READ_TIME_LSB; /*!< none                                                                  */
    struct {
      __IO uint32_t MILLISECONDS_COUNT : 10;
      __IO uint32_t SECONDS_COUNT : 6;
      __IO uint32_t MINS_COUNT : 6;
      __IO uint32_t HOURS_COUNT : 5;
      __IO uint32_t DAYS_COUNT : 5;
    } MCU_CAL_READ_TIME_LSB_b; /*!< BitSize                                                               */
  };
  /*0x38*/
  union {
    __IO uint32_t
      MCU_CAL_READ_COUNT_TIMER; /*!< none                                                                  */

    struct {
      __IO uint32_t READ_COUNT_TIMER : 27; /*!<    */
      __IO uint32_t RESERVED1 : 5;
    } MCU_CAL_READ_COUNT_TIMER_b; /*!< BitSize                                                               */
  };
  /*0x3C*/
  union {
    __IO uint32_t
      MCU_CAL_SLEEP_CLK_COUNTERS; /*!< none                                                                  */

    struct {
      __IO uint32_t SLEEP_CLK_DURATION : 12;
      __IO uint32_t RESERVED1 : 4;
      __IO uint32_t PCLK_COUNT_WRT_SLEEP_CLK : 12; /*!<    */
      __IO uint32_t RESERVED2 : 4;
    } MCU_CAL_SLEEP_CLK_COUNTERS_b; /*!< BitSize                                                               */
  };

  /*0x40*/
  union {
    __IO uint32_t MCU_CAL_KEY_EANBLE; /*!< none                                                                  */

    struct {
      __IO uint32_t RTC_KEY : 32; /*!<    */
    } MCU_CAL_KEY_EANBLE_b;       /*!< BitSize                                                               */
  };

} MCU_Calender_t;
/*End of MCU Calender*/

/*start MCU battery Flip Flops */

typedef struct {

  /*0x00*/
  union {
    __IO uint32_t
      M4SS_BYPASS_PWRCTRL_REG1; /*!< none                                                                  */

    struct {
      __IO uint32_t RES : 2;
      __IO uint32_t BYPASS_M4SS_PWRCTL_ULP_SD_MEM_b : 1;
      __IO uint32_t BYPASS_M4SS_PWRCTL_ULP_M4_ULP_AON_b : 1;
      __IO uint32_t BYPASS_M4SS_PWRCTL_ULP_EFUSE_b : 1;
      __IO uint32_t BYPASS_M4SS_PWRCTL_ULP_CCI_b : 1;
      __IO uint32_t BYPASS_M4SS_PWRCTL_ULP_PERI3_b : 1;
      __IO uint32_t BYPASS_M4SS_PWRCTL_ULP_PERI2_b : 1;
      __IO uint32_t BYPASS_M4SS_PWRCTL_ULP_PERI1_b : 1;
      __IO uint32_t BYPASS_M4SS_PWRCTL_ULP_RPDMA_b : 1;
      __IO uint32_t BYPASS_M4SS_PWRCTL_ULP_HIF_USB_b : 1;
      __IO uint32_t BYPASS_M4SS_PWRCTL_ULP_HIF_SDIO_SPI_b : 1;
      __IO uint32_t BYPASS_M4SS_PWRCTL_ULP_HIF_ETHERNET_b : 1;
      __IO uint32_t BYPASS_M4SS_PWRCTL_ULP_QSPI_b : 1;
      __IO uint32_t BYPASS_M4SS_PWRCTL_ULP_IID_b : 1;
      __IO uint32_t BYPASS_M4SS_PWRCTL_ULP_ICACHE_b : 1;
      __IO uint32_t BYPASS_M4SS_PWRCTL_ULP_M4_FPU_b : 1;
      __IO uint32_t BYPASS_M4SS_PWRCTL_ULP_M4_DEBUG_b : 1;
      __IO uint32_t BYPASS_M4SS_PWRCTL_ULP_M4_CORE_b : 1;
      __IO uint32_t BYPASS_M4SS_PWRCTL_ULP_AON_b : 1;
      __IO uint32_t RESERVED1 : 2;
      __IO uint32_t BYPASS_M4SS_PWRCTL_ULP_ROM_b : 1;
      __IO uint32_t RESERVED2 : 9;

    } M4SS_BYPASS_PWRCTRL_REG1_b; /*!< BitSize                                                               */
  };

  /*0x04*/
  union {
    __IO uint32_t
      M4SS_BYPASS_PWRCTRL_REG2; /*!< none                                                                  */

    struct {
      __IO uint32_t BYPASS_M4SS_PWRCTL_ULP_SRAM_1 : 10; /*!<   */
      __IO uint32_t RESERVED1 : 6;
      __IO uint32_t BYPASS_M4SS_PWRCTL_ULP_SRAM_2 : 4;
      __IO uint32_t RESERVED2 : 12;
    } M4SS_BYPASS_PWRCTRL_REG2_b; /*!< BitSize                                                               */
  };

  /*0x08*/
  union {
    __IO uint32_t M4SS_PWRCTRL_SET_REG; /*!< none                                                                  */

    struct {
      __IO uint32_t RES : 2;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_SD_MEM_b : 1;
      __IO uint32_t RESERVED0 : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_EFUSE_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_CCI_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_PERI3_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_PERI2_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_PERI1_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_RPDMA_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_HIF_USB_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_HIF_SDIO_SPI_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_HIF_ETHERNET_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_QSPI_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_IID_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_ICACHE_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_M4_FPU_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_M4_DEBUG_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_M4_CORE_b : 1;
      __IO uint32_t RESERVED1 : 3;
      __IO uint32_t M4SS_EXT_PWRGATE_EN_N_ULP_ROM_b : 1;
      __IO uint32_t RESERVED2 : 9;
    } M4SS_PWRCTRL_SET_REG_b; /*!< BitSize                                                               */
  };

  /*0x0C*/
  union {
    __IO uint32_t M4SS_PWRCTRL_CLEAR_REG; /*!< none                                                                  */

    struct {
      __IO uint32_t RES : 2;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_SD_MEM_b : 1;
      __IO uint32_t RESERVED0 : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_EFUSE_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_CCI_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_PERI3_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_PERI2_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_PERI1_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_RPDMA_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_HIF_USB_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_HIF_SDIO_SPI_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_HIF_ETHERNET_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_QSPI_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_IID_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_ICACHE_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_M4_FPU_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_M4_DEBUG_b : 1;
      __IO uint32_t M4SS_PWRGATE_EN_N_ULP_M4_CORE_b : 1;
      __IO uint32_t RESERVED1 : 3;
      __IO uint32_t M4SS_EXT_PWRGATE_EN_N_ULP_ROM_b : 1;
      __IO uint32_t RESERVED2 : 9;
    } M4SS_PWRCTRL_CLEAR_REG_b; /*!< BitSize                                                               */
  };

  /*0x10*/
  union {
    __IO uint32_t
      M4_SRAM_PWRCTRL_SET_REG1; /*!< none                                                                  */

    struct {
      __IO uint32_t M4SS_EXT_PWRGATE_EN_N_ULP_SRAM_1 : 10;
      __IO uint32_t RESERVED1 : 6;
      __IO uint32_t M4SS_EXT_PWRGATE_EN_N_ULP_SRAM_2 : 4;
      __IO uint32_t RESERVED2 : 12;
    } M4_SRAM_PWRCTRL_SET_REG1_b; /*!< BitSize                                                               */
  };

  /*0x14*/
  union {
    __IO uint32_t
      M4_SRAM_PWRCTRL_CLEAR_REG1; /*!< none                                                                  */

    struct {
      __IO uint32_t M4SS_EXT_PWRGATE_EN_N_ULP_SRAM_1 : 10;
      __IO uint32_t RESERVED1 : 6;
      __IO uint32_t M4SS_EXT_PWRGATE_EN_N_ULP_SRAM_2 : 4;
      __IO uint32_t RESERVED2 : 12;
    } M4_SRAM_PWRCTRL_CLEAR_REG1_b; /*!< BitSize                                                               */
  };

  /*0x18*/
  union {
    __IO uint32_t
      M4_SRAM_PWRCTRL_SET_REG2; /*!< none                                                                  */

    struct {
      __IO uint32_t M4SS_EXT_PWRGATE_EN_N_ULP_SRAM_PERI_1 : 10;
      __IO uint32_t RESERVED1 : 6;
      __IO uint32_t M4SS_EXT_PWRGATE_EN_N_ULP_SRAM_PERI_2 : 4;
      __IO uint32_t RESERVED2 : 12;
    } M4_SRAM_PWRCTRL_SET_REG2_b; /*!< BitSize                                                               */
  };

  /*0x1C*/
  union {
    __IO uint32_t
      M4_SRAM_PWRCTRL_CLEAR_REG2; /*!< none                                                                  */

    struct {
      __IO uint32_t M4SS_EXT_PWRGATE_EN_N_ULP_SRAM_PERI_1 : 10;
      __IO uint32_t RESERVED1 : 6;
      __IO uint32_t M4SS_EXT_PWRGATE_EN_N_ULP_SRAM_PERI_2 : 4;
      __IO uint32_t RESERVED2 : 12;
    } M4_SRAM_PWRCTRL_CLEAR_REG2_b; /*!< BitSize                                                               */
  };

  /*0x20*/
  union {
    __IO uint32_t
      M4_SRAM_PWRCTRL_SET_REG3; /*!< none                                                                  */

    struct {
      __IO uint32_t M4SS_SRAM_INPUT_DISABLE_ISOLATION_ULP_1 : 10;
      __IO uint32_t RESERVED1 : 6;
      __IO uint32_t M4SS_SRAM_INPUT_DISABLE_ISOLATION_ULP_2 : 4;
      __IO uint32_t RESERVED2 : 12;
    } M4_SRAM_PWRCTRL_SET_REG3_b; /*!< BitSize                                                               */
  };

  /*0x24*/
  union {
    __IO uint32_t
      M4_SRAM_PWRCTRL_CLEAR_REG3; /*!< none                                                                  */

    struct {
      __IO uint32_t M4SS_SRAM_INPUT_DISABLE_ISOLATION_ULP_1 : 10;
      __IO uint32_t RESERVED1 : 6;
      __IO uint32_t M4SS_SRAM_INPUT_DISABLE_ISOLATION_ULP_2 : 4;
      __IO uint32_t RESERVED2 : 12;
    } M4_SRAM_PWRCTRL_CLEAR_REG3_b; /*!< BitSize                                                               */
  };

  /*0x28*/
  union {
    __IO uint32_t
      M4_SRAM_PWRCTRL_SET_REG4; /*!< none                                                                  */

    struct {
      __IO uint32_t M4SS_SRAM_DS_1 : 15;
      __IO uint32_t RESERVED1 : 1;
      __IO uint32_t M4SS_SRAM_DS_2 : 12;
      __IO uint32_t RESERVED2 : 4;
    } M4_SRAM_PWRCTRL_SET_REG4_b; /*!< BitSize                                                               */
  };

  /*0x2C*/
  union {
    __IO uint32_t
      M4_SRAM_PWRCTRL_CLEAR_REG4; /*!< none                                                                  */

    struct {
      __IO uint32_t M4SS_SRAM_DS_1 : 15;
      __IO uint32_t RESERVED1 : 1;
      __IO uint32_t M4SS_SRAM_DS_2 : 12;
      __IO uint32_t RESERVED2 : 4;
    } M4_SRAM_PWRCTRL_CLEAR_REG4_b; /*!< BitSize                                                               */
  };
  /*0x30*/
  __IO uint32_t RESERVED2;

  /*0x34*/
  union {
    __IO uint32_t M4SS_TASS_CTRL_SET_REG; /*!< none                                                                  */

    struct {
      __IO uint32_t M4SS_CTRL_TASS_AON_PWRGATE_EN : 1;
      __IO uint32_t M4SS_CTRL_TASS_AON_DISABLE_ISOLATION_BYPASS : 1;
      __IO uint32_t M4SS_CTRL_TASS_AON_PWR_DMN_RST_BYPASS : 1;
      __IO uint32_t RESERVED1 : 13;
      __IO uint32_t M4SS_CTRL_TASS_EXT_PWRGATE_EN_N_ULP_ROM : 8;
      __IO uint32_t RESERVED2 : 8;
    } M4SS_TASS_CTRL_SET_REG_b; /*!< BitSize                                                               */
  };

  /*0x38*/
  union {
    __IO uint32_t
      M4SS_TASS_CTRL_CLEAR_REG; /*!< none                                                                  */

    struct {
      __IO uint32_t M4SS_CTRL_TASS_AON_PWRGATE_EN : 1;
      __IO uint32_t M4SS_CTRL_TASS_AON_DISABLE_ISOLATION_BYPASS : 1;
      __IO uint32_t M4SS_CTRL_TASS_AON_PWR_DMN_RST_BYPASS : 1;
      __IO uint32_t RESERVED1 : 13;
      __IO uint32_t M4SS_CTRL_TASS_EXT_PWRGATE_EN_N_ULP_ROM : 8;
      __IO uint32_t RESERVED2 : 8;
    } M4SS_TASS_CTRL_CLEAR_REG_b; /*!< BitSize                                                               */
  };

  /*0x3C*/
  union {
    __IO uint32_t M4_ULP_MODE_CONFIG; /*!< none                                                                  */

    struct {
      __IO uint32_t ULPMODE_ISOLATION_CTRL_ULPSS : 1;
      __IO uint32_t ULPMODE_ISOLATION_CTRL_M4SS_AON : 1;
      __IO uint32_t ULPMODE_ISOLATION_CTRL_M4_ULP : 1;
      __IO uint32_t ULPMODE_ISOLATION_CTRL_M4_CORE : 1;
      __IO uint32_t ULPMODE_ISOLATION_CTRL_M4_DEBUG : 1;
      __IO uint32_t ULPMODE_ISOLATION_CTRL_M4_ROM : 1;
      __IO uint32_t RES : 26;
    } M4_ULP_MODE_CONFIG_b; /*!< BitSize                                                               */
  };

  /*0x40*/
  union {
    __IO uint32_t
      ULPSS_BYPASS_PWRCTRL_REG; /*!< none                                                                  */

    struct {
      __IO uint32_t RES : 2;
      __IO uint32_t BYPASS_ULPTASS_PWRCTL_ULP_AON : 1;
      __IO uint32_t BYPASS_ULPSDCSS_PWRCTRL_ULP_AON : 1;
      __IO uint32_t RESERVED : 1;
      __IO uint32_t BYPASS_ULPTASS_PWRCTL_ULP_MISC : 1;
      __IO uint32_t BYPASS_ULPTASS_PWRCTL_ULP_CAP : 1;
      __IO uint32_t BYPASS_ULPTASS_PWRCTL_ULP_VAD : 1;
      __IO uint32_t BYPASS_ULPTASS_PWRCTL_ULP_UART : 1;
      __IO uint32_t BYPASS_ULPTASS_PWRCTL_ULP_SSI : 1;
      __IO uint32_t BYPASS_ULPTASS_PWRCTL_ULP_I2S : 1;
      __IO uint32_t BYPASS_ULPTASS_PWRCTL_ULP_I2C : 1;
      __IO uint32_t BYPASS_ULPTASS_PWRCTL_ULP_AUX : 1;
      __IO uint32_t BYPASS_ULPTASS_PWRCTL_ULP_IR : 1;
      __IO uint32_t BYPASS_ULPTASS_PWRCTL_ULP_UDMA : 1;
      __IO uint32_t BYPASS_ULPTASS_PWRCTL_ULP_FIM : 1;
      __IO uint32_t RESERVED2 : 3;
      __IO uint32_t BYPASS_ULPTASS_PWRCTL_ULP_SRAM : 10;
      __IO uint32_t RESERVED3 : 3;

    } ULPSS_BYPASS_PWRCTRL_REG_b; /*!< BitSize                                                               */
  };

  /*0x44*/
  union {
    __IO uint32_t ULPSS_PWRCTRL_SET_REG; /*!< none                                                                  */

    struct {
      __IO uint32_t RES : 18;
      __IO uint32_t ULPTASS_EXT_PWRGATE_EN_N_ULP_MISC : 1;
      __IO uint32_t ULPTASS_EXT_PWRGATE_EN_N_ULP_CAP : 1;
      __IO uint32_t ULPTASS_EXT_PWRGATE_EN_N_ULP_VAD : 1;
      __IO uint32_t ULPTASS_EXT_PWRGATE_EN_N_ULP_UART : 1;
      __IO uint32_t ULPTASS_EXT_PWRGATE_EN_N_ULP_SSI : 1;
      __IO uint32_t ULPTASS_EXT_PWRGATE_EN_N_ULP_I2S : 1;
      __IO uint32_t ULPTASS_EXT_PWRGATE_EN_N_ULP_I2C : 1;
      __IO uint32_t ULPTASS_EXT_PWRGATE_EN_N_ULP_AUX : 1;
      __IO uint32_t ULPTASS_EXT_PWRGATE_EN_N_ULP_IR : 1;
      __IO uint32_t ULPTASS_EXT_PWRGATE_EN_N_ULP_UDMA : 1;
      __IO uint32_t ULPTASS_EXT_PWRGATE_EN_N_ULP_FIM : 1;
      __IO uint32_t RESERVED : 3;

    } ULPSS_PWRCTRL_SET_REG_b; /*!< BitSize                                                               */
  };

  /*0x48*/
  union {
    __IO uint32_t ULPSS_PWRCTRL_CLEAR_REG; /*!< none                                                                  */

    struct {
      __IO uint32_t RES : 18;
      __IO uint32_t ULPTASS_EXT_PWRGATE_EN_N_ULP_MISC : 1;
      __IO uint32_t ULPTASS_EXT_PWRGATE_EN_N_ULP_CAP : 1;
      __IO uint32_t ULPTASS_EXT_PWRGATE_EN_N_ULP_VAD : 1;
      __IO uint32_t ULPTASS_EXT_PWRGATE_EN_N_ULP_UART : 1;
      __IO uint32_t ULPTASS_EXT_PWRGATE_EN_N_ULP_SSI : 1;
      __IO uint32_t ULPTASS_EXT_PWRGATE_EN_N_ULP_I2S : 1;
      __IO uint32_t ULPTASS_EXT_PWRGATE_EN_N_ULP_I2C : 1;
      __IO uint32_t ULPTASS_EXT_PWRGATE_EN_N_ULP_AUX : 1;
      __IO uint32_t ULPTASS_EXT_PWRGATE_EN_N_ULP_IR : 1;
      __IO uint32_t ULPTASS_EXT_PWRGATE_EN_N_ULP_UDMA : 1;
      __IO uint32_t ULPTASS_EXT_PWRGATE_EN_N_ULP_FIM : 1;
      __IO uint32_t RESERVED : 3;
    } ULPSS_PWRCTRL_CLEAR_REG_b; /*!< BitSize                                                               */
  };

  /*0x4C*/
  union {
    __IO uint32_t
      ULPSS_RAM_PWRCTRL_SET_REG1; /*!< none                                                                  */

    struct {
      __IO uint32_t ULPTASS_EXT_PWRGATE_EN_N_ULP_SRAM : 8;
      __IO uint32_t RESERVED1 : 8;
      __IO uint32_t RESERVED2 : 16;

    } ULPSS_RAM_PWRCTRL_SET_REG1_b; /*!< BitSize                                                               */
  };

  /*0x50*/
  union {
    __IO uint32_t
      ULPSS_RAM_PWRCTRL_CLEAR_REG1; /*!< none                                                                  */

    struct {
      __IO uint32_t ULPTASS_EXT_PWRGATE_EN_N_ULP_SRAM : 8;
      __IO uint32_t RESERVED1 : 8;
      __IO uint32_t RESERVED2 : 16;
    } ULPSS_RAM_PWRCTRL_CLEAR_REG1_b; /*!< BitSize                                                               */
  };

  /*0x54*/
  union {
    __IO uint32_t
      ULPSS_RAM_PWRCTRL_SET_REG2; /*!< none                                                                  */

    struct {
      __IO uint32_t ULPTASS_SRAM_INPUT_DISABLE_ISOLATION_ULP : 8;
      __IO uint32_t RESERVED1 : 8;
      __IO uint32_t SRAM_DS_ULP_PROC : 10;
      __IO uint32_t RESERVED2 : 6;
    } ULPSS_RAM_PWRCTRL_SET_REG2_b; /*!< BitSize                                                               */
  };

  /*0x58*/
  union {
    __IO uint32_t
      ULPSS_RAM_PWRCTRL_CLEAR_REG2; /*!< none                                                                  */

    struct {
      __IO uint32_t ULPTASS_SRAM_INPUT_DISABLE_ISOLATION_ULP : 8;
      __IO uint32_t RESERVED1 : 8;
      __IO uint32_t SRAM_DS_ULP_PROC : 10;
      __IO uint32_t RESERVED2 : 6;
    } ULPSS_RAM_PWRCTRL_CLEAR_REG2_b; /*!< BitSize                                                               */
  };

  /*0x5C*/
  union {
    __IO uint32_t ULPSS_RAM_PWRCTRL_SET_REG3;
    struct {
      __IO uint32_t ulptass_ext_pwrgate_en_n_ulp_SRAM_peri_1 : 8;
      __IO uint32_t RES : 24;
    } ULPSS_RAM_PWRCTRL_SET_REG3_b;
  };

  /*0x60*/

  union {
    __IO uint32_t ULPSS_RAM_PWRCTRL_CLEAR_REG3;
    struct {
      __IO uint32_t ulptass_ext_pwrgate_en_n_ulp_SRAM_peri_1 : 8;
      __IO uint32_t RES : 24;
    } ULPSS_RAM_PWRCTRL_CLEAR_REG3_b;
  };
  /*0x64*/
  union {
    __IO uint32_t MCU_FSM_CTRL_BYPASS; /*!< none                                                                  */

    struct {
      __IO uint32_t MCU_XTAL_EN_40MHZ_BYPASS_CTRL : 1;
      __IO uint32_t MCU_XTAL_EN_40MHZ_BYPASS : 1;
      __IO uint32_t MCU_PMU_SHUT_DOWN_BYPASS_CTRL : 1;
      __IO uint32_t MCU_PMU_SHUT_DOWN_BYPASS : 1;
      __IO uint32_t MCU_BUCK_BOOST_ENABLE_BYPASS_CTRL : 1;
      __IO uint32_t MCU_BUCK_BOOST_ENABLE_BYPASS : 1;
      __IO uint32_t RES : 26;
    } MCU_FSM_CTRL_BYPASS_b; /*!< BitSize                                                               */
  };

  /*0x68*/
  union {
    __IO uint32_t MCU_PMU_LDO_CTRL_SET; /*!< none                                                                  */

    struct {
      __IO uint32_t MCU_FLASH_LDO_EN : 1;
      __IO uint32_t MCU_SCO_LDO_EN : 1;
      __IO uint32_t MCU_DCDC_EN : 1;
      __IO uint32_t RESERVED : 14;
      __IO uint32_t MCU_SOC_LDO_LVL : 1;
      __IO uint32_t MCU_DCDC_LVL : 1;
      __IO uint32_t RES : 13;
    } MCU_PMU_LDO_CTRL_SET_b; /*!< BitSize                                                               */
  };

  /*0x6C	*/
  union {
    __IO uint32_t MCU_PMU_LDO_CTRL_CLEAR;
    struct {
      __IO uint32_t MCU_FLASH_LDO_EN : 1;
      __IO uint32_t MCU_SCO_LDO_EN : 1;
      __IO uint32_t MCU_DCDC_EN : 1;
      __IO uint32_t RESERVED : 14;
      __IO uint32_t MCU_SOC_LDO_LVL : 1;
      __IO uint32_t MCU_DCDC_LVL : 1;
      __IO uint32_t RES : 13;
    } MCU_PMU_LDO_CTRL_CLEAR_b;
  };
  /*0x70*/
  uint32_t RESERVED6;
  /*0x74*/
  uint32_t RESERVED7;
  /*0x78*/
  uint32_t RESERVED8;
  /*0x7C*/
  uint32_t RESERVED9;

  /*0x80*/
  union {
    __IO uint32_t PLLCCI_PWRCTRL_REG; /*!< none                                                                  */

    struct {
      __IO uint32_t I2SPLL_ISO_EN : 1;
      __IO uint32_t I2SPLL_BYPASS_ISO_GEN : 1;
      __IO uint32_t INTFPLL_ISO_EN : 1;
      __IO uint32_t INTFPLL_BYPASS_ISO_GEN : 1;
      __IO uint32_t SOCPLL_ISO_ENABLE : 1;
      __IO uint32_t SOCPLL_BYPASS_ISO_GEN : 1;
      __IO uint32_t SOCPLL_SPI_PG_EN : 1;
      __IO uint32_t SOCPLL_VDD13_ISO_EN_b : 1;
      __IO uint32_t RES : 24;
    } PLLCCI_PWRCTRL_REG_b; /*!< BitSize                                                               */
  };

  /*0x84*/
  union {
    __IO uint32_t DLL_PWRCTRL_REG;
    struct {
      __IO uint32_t QSPI_DLL_RX_ISO_ENABLE : 1;
      __IO uint32_t QSPI_DLL_RX_BYPASS_ISO_GEN : 1;
      __IO uint32_t QSPI_DLL_RX_PG_EN_N : 1;
      __IO uint32_t RESERVED : 1;
      __IO uint32_t QSPI_DLL_TX_ISO_ENABLE : 1;
      __IO uint32_t QSPI_DLL_TX_BYPASS_ISO_GEN : 1;
      __IO uint32_t QSPI_DLL_TX_PG_EN_N : 1;
      __IO uint32_t RESERVED1 : 25;
    } DLL_PWRCTRL_REG_b;
  };

  uint32_t RES1[30];
} MCU_Battery;

/*START OF MCU FSM*/
typedef struct {

  /*0x00*/
  union {
    __IO uint32_t MCU_FSM_SLEEP_CTRLS_AND_WAKEUP_MODE;
    struct {
      __IO uint32_t MCUFSM_SHUTDOWN_ENABLE : 1;
      __IO uint32_t RES : 1;
      __IO uint32_t LP_SLEEP_MODE_b : 1;
      __IO uint32_t M4SS_RAM_RETENTION_MODE_EN : 1;
      __IO uint32_t M4ULP_RAM_RETENTION_MODE_EN : 1;
      __IO uint32_t TA_RAM_RETENTION_MODE_EN : 1;
      __IO uint32_t ULPSS_RAM_RETENTION_MODE_EN : 1;
      __IO uint32_t M4ULP_RAM16K_RETENTION_MODE_EN : 1;
      __IO uint32_t LDO_SOC_ON : 1;
      __IO uint32_t LDO_FLASH_ON : 1;
      __O uint32_t PMU_DCDC_ON : 1;
      __IO uint32_t SKIP_XTAL_WAIT_TIME : 1;
      __IO uint32_t RESERVED1 : 2;
      __IO uint32_t MCUFSM_WAKEUP_NWPFSM : 1;
      __O uint32_t SLEEP_WAKEUP : 1;
      __IO uint32_t TIMER_BASED_WAKEUP_b : 1;
      __IO uint32_t HOST_BASED_WAKEUP_b : 1;
      __IO uint32_t WIRELESS_BASED_WAKEUP_b : 1;
      __IO uint32_t M4_PROC_BASED_WAKEUP_b : 1;
      __IO uint32_t GPIO_BASED_WAKEUP_b : 1;
      __IO uint32_t COMPR_BASED_WAKEUP_b : 1;
      __IO uint32_t WAKEIF_BASED_WAKEUP_b : 1;
      __IO uint32_t WIC_BASED_WAKEUP_b : 1;
      __IO uint32_t ULPSS_BASED_WAKEUP_b : 1;
      __IO uint32_t SDCSS_BASED_WAKEUP_b : 1;
      __IO uint32_t ALARM_BASED_WAKEUP_b : 1;
      __IO uint32_t SEC_BASED_WAKEUP_b : 1;
      __IO uint32_t MSEC_BASED_WAKEUP_b : 1;
      __IO uint32_t WDT_INTR_BASED_WAKEUP_b : 1;
      __IO uint32_t ULPSS_BASED_SLEEP : 1;
      __IO uint32_t SDCSS_BASED_SLEEP : 1;
    } MCU_FSM_SLEEP_CTRLS_AND_WAKEUP_MODE_b;
  };

  /*0x04*/
  union {
    __IO uint32_t MCU_FSM_PERI_CONFIG_REG;
    struct {
      __IO uint32_t ULP_MCU_MODE_EN : 1;
      __IO uint32_t M4SS_CONTEXT_SWITCH_TOP_ULP_MODE : 2;
      __IO uint32_t WICENREQ : 1;
      __IO uint32_t RESERVED1 : 12;
      __IO uint32_t BGPMU_SAMPLING_EN_R : 1;
      __IO uint32_t RESERVED2 : 15;

    } MCU_FSM_PERI_CONFIG_REG_b;
  };

  /*0x08*/
  union {
    __IO uint32_t GPIO_WAKEUP_REGISTER;
    struct {
      __IO uint32_t GPIO_0_WAKEUP : 1;
      __IO uint32_t GPIO_1_WAKEUP : 1;
      __IO uint32_t GPIO_2_WAKEUP : 1;
      __IO uint32_t GPIO_3_WAKEUP : 1;
      __IO uint32_t GPIO_4_WAKEUP : 1;
      __IO uint32_t RESERVED1 : 11;
      __IO uint32_t CONTINIOUS_START : 1;
      __IO uint32_t CONTINIOUS_TIMER_ENABLE : 1;
      __IO uint32_t DS_TIMER_SOFT_RESET : 1;
      __IO uint32_t RESERVED2 : 13;
    } GPIO_WAKEUP_REGISTER_b;
  };

  /*0x0C*/
  union {
    __IO uint32_t MCU_FSM_DEEP_SLEEP_DURATION_LSB_REG;
    struct {
      __IO uint32_t MCUFSM_DEEPSLEEP_DURATION_COUNT : 32;
    } MCU_FSM_DEEP_SLEEP_DURATION_LSB_REG_b;
  };

  /*0x10*/
  union {
    __IO uint32_t MCU_FSM_XTAL_AND_PMU_GOOD_COUNT_REG;
    struct {
      __IO uint32_t MCUFSM_PMU_POWERGOOD_DURATION_COUNT : 5;
      __IO uint32_t RESERVED1 : 11;
      __IO uint32_t MCUFSM_XTAL_GOODTIME_DURATION_COUNT : 5;
      __IO uint32_t RESERVED2 : 11;
    } MCU_FSM_XTAL_AND_PMU_GOOD_COUNT_REG_b;
  };

  /*0x14*/
  union {
    __IO uint32_t MCU_FSM_POWER_CTRL_AND_DELAY;
    struct {
      __IO uint32_t PS2_PMU_LDO_OFF_DELAY : 6;
      __IO uint32_t RESERVED : 2;
      __IO uint32_t PS4_SOCLDO_ON_DELAY : 4;
      __IO uint32_t PG4_BUCK_ON_DELAY : 4;
      __IO uint32_t FSM_PERI_SOC_LDO_EN : 1;
      __IO uint32_t FSM_PERI_DCDC_EN : 1;
      __IO uint32_t RESERVED2 : 1;
      __IO uint32_t POWER_MUX_SEL_ULPSS : 1;
      __IO uint32_t POWER_MUX_SEL_M4_ULP : 2;
      __IO uint32_t POWER_MUX_SEL_M4_ULP_RAM_16K : 2;
      __IO uint32_t POWER_MUX_SEL_M4_ULP_RAM : 2;
      __IO uint32_t POWER_MUX_SEL_ULPSS_RAM : 2;
      __IO uint32_t RESERVED3 : 4;
    } MCU_FSM_POWER_CTRL_AND_DELAY_b;
  };

  /*0x18*/
  union {
    __IO uint32_t MCU_FSM_CLKS_REG;
    struct {
      __IO uint32_t RESERVED : 2;
      __IO uint32_t HF_FSM_CLK_SELECT : 3;
      __IO uint32_t RESERVED1 : 11;
      __IO uint32_t HF_FSM_CLK_FREQ : 6;
      __IO uint32_t US_DIV_COUNT : 2;
      __IO uint32_t HF_FSM_GEN_2MHZ : 1;
      __IO uint32_t HF_FSM_CLK_EN : 1;
      __IO uint32_t RESERVED2 : 6;

    } MCU_FSM_CLKS_REG_b;
  };
  /*0x1C*/
  union {
    __IO uint32_t MCU_FSM_REF_CLK_REG;
    struct {
      __IO uint32_t M4SS_REF_CLK_SEL : 3;
      __IO uint32_t RESERVED1 : 4;
      __IO uint32_t M4SS_REF_CLK_CLEANER_OFF_b : 1;
      __IO uint32_t M4SS_REF_CLK_CLEANER_ON_b : 1;
      __IO uint32_t RESERVED2 : 3;
      __IO uint32_t TASS_REF_CLK_SEL : 3;
      __IO uint32_t RESERVED3 : 1;
      __IO uint32_t ULPSS_REF_CLK_SEL_b : 3;
      __IO uint32_t RESERVED4 : 4;
      __IO uint32_t ULPSS_REF_CLK_CLEANER_OFF_b : 1;
      __IO uint32_t ULPSS_REF_CLK_CLEANER_ON_b : 1;
      __IO uint32_t RESERVED5 : 3;
      __IO uint32_t SDCSS_CLK_SEL_b : 2;
      __IO uint32_t SDCSS_CLK_EN_b : 1;
      __IO uint32_t SDCSS_STATIC_CLK_EN_b : 1;
    } MCU_FSM_REF_CLK_REG_b;
  };
  /*0x20*/
  union {
    __IO uint32_t MCU_FSM_CLK_ENS_AND_FIRST_BOOTUP;
    struct {
      __IO uint32_t FIRST_BOOTUP_MCU_N_b : 1;
      __IO uint32_t RAM_RETENTION_STATUS_M4SS_b : 1;
      __IO uint32_t RETENTION_DOMAIN_ON_b : 1;
      __IO uint32_t CHIP_MODE_VALID_b : 1;
      __IO uint32_t STORAGE_DOMAIN_ON_b : 1;
      __IO uint32_t RESERVED : 10;
      __IO uint32_t MCU_FSM_RESET_N_SYNC_b : 1;
      __IO uint32_t MCU_ULP_32KHZ_RC_CLK_EN_b : 1;
      __IO uint32_t MCU_ULP_32KHZ_RO_CLK_EN_b : 1;
      __IO uint32_t MCU_ULP_32KHZ_XTAL_CLK_EN_b : 1;
      __IO uint32_t MCU_ULP_32MHZ_RC_CLK_EN_b : 1;
      __IO uint32_t MCU_ULP_20MHZ_RING_OSC_CLK_EN_b : 1;
      __IO uint32_t MCU_ULP_DOUBLER_CLK_EN_b : 1;
      __IO uint32_t MCU_ULP_40MHZ_CLK_EN_b : 1;
      __IO uint32_t RESERVED1 : 9;
    } MCU_FSM_CLK_ENS_AND_FIRST_BOOTUP_b;
  };

  /*0x24*/
  union {
    __IO uint32_t MCU_FSM_CRTL_PDM_AND_ENABLES;
    struct {
      __IO uint32_t ENABLE_WDT_IN_SLEEP : 1;
      __IO uint32_t ENABLE_WURX_DETECTION : 1;
      __IO uint32_t RESET_MCU_BBF_DM_EN : 1;
      __IO uint32_t DISABLE_TURNOFF_SRAM_PERI : 1;
      __IO uint32_t ENABLE_SRAM_DS_CRTL : 1;
      __IO uint32_t RESERVED : 11;
      __IO uint32_t POWER_ENABLE_FSM_PERI : 1;
      __IO uint32_t POWER_ENABLE_TIMESTAMPING : 1;
      __IO uint32_t POWER_ENABLE_DEEPSLEEP_TIMER : 1;
      __IO uint32_t POWER_ENABLE_RETENTION_DM : 1;
      __IO uint32_t RESERVED2 : 12;
    } MCU_FSM_CRTL_PDM_AND_ENABLES_b;
  };

  /*0x28*/
  union {
    __IO uint32_t MCU_GPIO_TIMESTAMPING_CONFIG;
    struct {
      __IO uint32_t ENABLE_GPIO_TIMESTAMPING_b : 1;
      __IO uint32_t TIMESTAMPING_ON_GPIO0_b : 1;
      __IO uint32_t TIMESTAMPING_ON_GPIO1_b : 1;
      __IO uint32_t TIMESTAMPING_ON_GPIO2_b : 1;
      __IO uint32_t TIMESTAMPING_ON_GPIO3_b : 1;
      __IO uint32_t TIMESTAMPING_ON_GPIO4_b : 1;
      __IO uint32_t RESERVED1 : 9;
      __IO uint32_t CLEAR_GPIO_TIMESTAMP_b : 1;
      __IO uint32_t RESERVED2 : 16;
    } MCU_GPIO_TIMESTAMPING_CONFIG_b;
  };

  /*0x2C*/
  union {
    __IO uint32_t MCU_GPIO_TIMESTAMP_READ;
    struct {
      __IO uint32_t GPIO_EVENT_COUNT_PARTIAL : 11;
      __IO uint32_t RESERVED : 5;
      __IO uint32_t GPIO_EVENT_COUNT_FULL : 11;
      __IO uint32_t RESERVED2 : 5;
    } MCU_GPIO_TIMESTAMP_READ_b;
  };

  /*0x30*/
  union {
    __IO uint32_t MCU_SLEEPHOLD_REQ;
    struct {
      __IO uint32_t SLEEPHOLDREQn : 1;
      __IO uint32_t SLEEPHOLDACKn : 1;
      __IO uint32_t RESERVED1 : 14;
      __IO uint32_t SELECT_FSM_MODE : 1;
      __IO uint32_t RESERVED2 : 15;
    };
  };

  /*0x34*/
  __IO uint32_t RESERVED4;

  /*0x38*/
  union {
    __IO uint32_t MCU_FSM_WAKEUP_STATUS_REG;
    struct {
      __IO uint32_t WAKEUP_STATUS : 11;
      __IO uint32_t RESERVED : 5;
      __IO uint32_t MCU_FIRST_POWERUP_POR : 1;
      __IO uint32_t MCU_FIRST_POWERUP_RESET_N : 1;
      __IO uint32_t RESERVED2 : 14;
    } MCU_FSM_WAKEUP_STATUS_REG_b;
  };

  /*0x3C*/
  union {
    __IO uint32_t MCU_FSM_WAKEUP_STATUS_CLEAR;
    struct {
      __IO uint32_t WWD_INTERRUPT_STATUS_CLEAR_b : 1;
      __IO uint32_t MILLI_SEC_BASED_STATUS_CLEAR_b : 1;
      __IO uint32_t RTC_SEC_BASED_STATUS_CLEAR_b : 1;
      __IO uint32_t RTC_ALARM_BASED_WAKEUP_STATUS_CLEAR_b : 1;
      __IO uint32_t COMP1_BASED_WAKEUP_STATUS_CLEAR_b : 1;
      __IO uint32_t COMP2_BASED_WAKEUP_STATUS_CLEAR_b : 1;
      __IO uint32_t COMP3_BASED_WAKEUP_STATUS_CLEAR_b : 1;
      __IO uint32_t COMP4_BASED_WAKEUP_STATUS_CLEAR_b : 1;
      __IO uint32_t COMP5_BASED_WAKEUP_STATUS_CLEAR_b : 1;
      __IO uint32_t COMP6_BASED_WAKEUP_STATUS_CLEAR_b : 1;
      __IO uint32_t RF_WAKEUP_CLEAR_b : 1;
      __IO uint32_t RESERVED : 21;
    } MCU_FSM_WAKEUP_STATUS_CLEAR_b;
  };

  /*0x40*/
  union {
    __IO uint32_t MCU_FSM_PMU_STATUS_REG;
    struct {
      __IO uint32_t SCDCDC_LP_MODE_EN : 1;
      __IO uint32_t BGPMU_SLEEP_EN_R : 1;
      __IO uint32_t RESERVED : 15;
      __IO uint32_t STANDBY_LDORF_R : 1;
      __IO uint32_t STANDBY_LDOSOC_R : 1;
      __IO uint32_t STANDBY_DC1P3_R : 1;
      __IO uint32_t POWERGOOD_LDOSOC : 1;
      __IO uint32_t POWERGOOD_LDORF : 1;
      __IO uint32_t POWERGOOD_DC1P3 : 1;
      __IO uint32_t LEVEL_OK_DC1P3 : 1;
      __IO uint32_t CL_FLAG_DC1P3 : 1;
      __IO uint32_t RESERVED1 : 7;
    } MCU_FSM_PMU_STATUS_REG_b;
  };

  /*0x44*/
  union {
    __IO uint32_t MCU_FSM_PMUX_CTRLS_RET;
    struct {
      __IO uint32_t POWER_SW_CTRL_TASS_RAM_IN_RETAIN : 1;
      __IO uint32_t POWER_SW_CTRL_M4SS_RAM_IN_RETAIN : 1;
      __IO uint32_t POWER_SW_CTRL_M4ULP_RAM_IN_RETAIN : 2;
      __IO uint32_t POWER_SW_CTRL_M4ULP_RAM16K_IN_RETAIN : 2;
      __IO uint32_t POWER_SW_CTRL_ULPSS_RAM_IN_RETAIN : 2;
      __IO uint32_t RESERVED : 24;
    } MCU_FSM_PMUX_CTRLS_RET_b;
  };
  /*0x48*/
  union {
    __IO uint32_t MCU_FSM_TOGGLE_COUNT;
    struct {
      __IO uint32_t TOGGLE_COUNT_RSTART : 1;
      __IO uint32_t RESERVED1 : 14;
      __IO uint32_t LATCH_TOGGLE_DATA : 1;
      __IO uint32_t GPIO_TOGGLE_COUNT : 12;
      __IO uint32_t RESERVED : 3;
      __IO uint32_t TOGGLE_DATA_READY : 1;
    } MCU_FSM_TOGGLE_COUNT_b;
  };

} MCU_Fsm_t;

/*End of mcu FSM */

/*Start process sensor */
typedef struct {
  union {
    __IO uint32_t PROCESS_SENSOR_ENABLE_AND_READ;
    struct {
      __IO uint32_t PROCESS_SENSOR_EN : 1;
      __IO uint32_t PS_RING_CLK_START : 1;
      __IO uint32_t PS_CLK_SW_ON : 1;
      __IO uint32_t PS_CLK_SW_OFF : 1;
      __IO uint32_t NUM_CYCLES : 2;
      __IO uint32_t RESERVED : 10;
      __IO uint32_t PS_COUNT : 17;
    };
  };
} MCU_ProcessSensor_t;
/*End process sensor */

/*End MCU battery Flip Flops */

typedef struct {
  /*0x1C*/
  union {
    __IO uint32_t NPSS_GPIO_CTRLS; /*!< none                                                                  */
    struct {
      __IO uint32_t NPSS_GPIO_MODE : 3;
      __IO uint32_t NPSS_GPIO_REN : 1;
      __IO uint32_t NPSS_GPIO_OEN : 1;
      __IO uint32_t NPSS_GPIO_OUT : 1;
      __IO uint32_t RESERVED1 : 2;
      __IO uint32_t NPSS_GPIO_POLARITY : 1;
      __IO uint32_t RESERVED2 : 7;
      __IO uint32_t USE_ULPSS_PAD : 1;
      __IO uint32_t RESERVED4 : 15;

    } NPSS_GPIO_CTRLS_b; /*!< BitSize                                                               */
  };
} NPSS_GPIO_CNTRL_Type;

/*START RETENTION  */
typedef struct {

  /*0x00*/
  union {
    __IO uint32_t MCURET_QSPI_WR_OP_DIS;
    struct {
      __IO uint32_t M4SS_QSPI_WRSR_WR_OP_DISABLE : 1;
      __O uint32_t TASS_QSPI_WRSR_WR_OP_DISABLE : 1;
      __IO uint32_t RESERVED : 30;
    } MCURET_QSPI_WR_OP_DIS_b;
  };

  /*0x04*/
  union {
    __IO uint32_t MCURET_BOOTSTATUS;
    struct {
      __IO uint32_t BOOT_STATUS : 31;
      __IO uint32_t RESERVED : 1;
    } MCURET_BOOTSTATUS_b;
  };

  /*0x08*/
  __IO uint32_t RESERVED2;

  /*0x0C*/
  union {
    __IO uint32_t CHIP_CONFIG_MCU_READ; /*!< none                                                                  */

    struct {
      __IO uint32_t DISABLE_M4 : 1;
      __IO uint32_t LIMIT_M4_FREQ_110MHZ_b : 1;
      __IO uint32_t DISABLE_M4_ULP_MODE : 1;
      __IO uint32_t DISABLE_USB : 1;
      __IO uint32_t DISABLE_ETHERNET : 1;
      __IO uint32_t DISABLE_CCI : 1;
      __IO uint32_t RESERVED : 4;
      __IO uint32_t M4_FLASH_SIZE : 3;
      __IO uint32_t DISABLE_FIM_COP : 1;
      __IO uint32_t DISABLE_VAP : 1;
      __IO uint32_t DISABLE_TOUCH : 1;
      __IO uint32_t DISABLE_WURX : 1;
      __IO uint32_t DISABLE_ANALOG_PERIPH : 1;
      __IO uint32_t DISABLE_JTAG : 1;
      __IO uint32_t DISABLE_M4SS_KH_ACCESS : 1;
      __IO uint32_t DISABLE_M4SS_ACCESS_FRM_TASS_SEC : 1;
      __IO uint32_t DISABLE_CAN_INTERFACE : 1;
      __IO uint32_t RESERVED1 : 10;

    } CHIP_CONFIG_MCU_READ_b; /*!< BitSize                                                               */
  };
  /*0x10 : TEST mux no need to write APIs for this */
  union {
    __IO uint32_t MCUAON_CTRL_REG4;
    struct {
      __IO uint32_t RES : 16;
      __IO uint32_t ULP_GPIO_2_TEST_MODE_OUT_SEL : 4;
      __IO uint32_t ULP_GPIO_1_TEST_MODE_OUT_SEL : 4;
      __IO uint32_t ULP_GPIO_0_TEST_MODE_OUT_SEL : 4;
      __IO uint32_t ULP_GPIOS_IN_TEST_MODE : 1;
      __IO uint32_t RESERVED : 3;
    };
  };
  /*0x14*/
  __IO uint32_t RESERVED4;

  /*0x18*/
  __IO uint32_t RESERVED5;

  /*0x1C*/
  NPSS_GPIO_CNTRL_Type NPSS_GPIO_CNTRL[5];
} MCU_RETENTION_Type;

/*END RETENTION  */

typedef struct {
  /*0x1C*/
  union {
    __IO uint32_t NPSS_GPIO_CTRLS; /*!< none                                                                  */
    struct {
      __IO uint32_t NPSS_GPIO_MODE : 3;
    } NPSS_GPIO_CTRLS_b; /*!< BitSize                                                               */
  };
} TEMPSENSOR_Type;

/*TEMPERATURE SENSOR START */
typedef struct {
  /*0x00*/
  union {
    __IO uint32_t
      TS_ENABLE_AND_TEMPERATURE_DONE; /*!< none                                                                  */
    struct {
      __IO uint32_t TEMP_SENS_EN : 1;
      __IO uint32_t REF_CLK_SEL : 1;
      __IO uint32_t CONT_COUNT_FREEZE : 10;
      __IO uint32_t TEMP_MEASUREMENT_DONE : 1;
      __IO uint32_t RESERVED : 19;
    } TS_ENABLE_AND_TEMPERATURE_DONE_b;
  }; /*!< BitSize                                                               */

  /*0x04*/
  union {
    __IO uint32_t TS_SLOPE_SET; /*!< none                                                                  */
    struct {
      __IO uint32_t SLOPE : 10;
      //FIXME :  improper bit locations in TRM
      __IO uint32_t RESERVED : 6;
      __IO uint32_t TEMPERATURE_SPI : 11;
      __IO uint32_t TEMP_UPDATED : 1;
      __IO uint32_t BJT_BASED_TEMP : 1;
      __IO uint32_t RESERVED1 : 3;
    } TS_SLOPE_SET_b; /*!< BitSize                                                               */
  };
  /*0x08*/
  union {
    __IO uint32_t
      TS_FE_COUNTS_NOMINAL_SETTINGS; /*!< none                                                                  */
    struct {
      __IO uint32_t F2_NOMINAL : 10;
      __IO uint32_t RESERVED : 6;
      __IO uint32_t NOMINAL_TEMPERATURE : 7;
      __IO uint32_t RESERVED1 : 9;
    } TS_FE_COUNTS_NOMINAL_SETTINGS_b; /*!< BitSize                                                               */
  };
  /*0x0C*/
  union {
    __IO uint32_t TS_COUNTS_READ; /*!< none                                                                  */
    struct {
      __IO uint32_t COUNT_F2 : 10;
      __IO uint32_t RESERVED : 6;
      __IO uint32_t COUNT_F1 : 10;
      __IO uint32_t RESERVED1 : 6;
    } TS_COUNTS_READ_B; /*!< BitSize                                                               */
  };
  /*0x10*/
  union {
    __IO uint32_t TEMPERATURE_READ; /*!< none                                                                  */
    struct {
      __IO uint32_t TEMPERATURE_RD : 11;
      __IO uint32_t RESERVED : 21;
    } TEMPERATURE_READ_b; /*!< BitSize                                                               */
  };
} NPSS_TEMPSENSOR_Type;

/*TEMPERATURE END */

/*MCU AON START*/
typedef struct {
  /*0x00*/
  union {
    __IO uint32_t
      MCUAON_NPSS_PWRCTRL_SET_REG; /*!< none                                                                  */
    struct {
      __IO uint32_t RES : 1;
      __IO uint32_t SLPSS_PWRGATE_EN_N_ULP_MCUBFFS : 1;
      __IO uint32_t SLPSS_PWRGATE_EN_N_ULP_MCUFSM : 1;
      __IO uint32_t SLPSS_PWRGATE_EN_N_ULP_MCURTC : 1;
      __IO uint32_t SLPSS_PWRGATE_EN_N_ULP_MCUWDT : 1;
      __IO uint32_t SLPSS_PWRGATE_EN_N_ULP_MCUPS : 1;
      __IO uint32_t SLPSS_PWRGATE_EN_N_ULP_MCUTS : 1;
      __IO uint32_t SLPSS_PWRGATE_EN_N_ULP_MCUSTORE1 : 1;
      __IO uint32_t SLPSS_PWRGATE_EN_N_ULP_MCUSTORE2 : 1;
      __IO uint32_t SLPSS_PWRGATE_EN_N_ULP_MCUSTORE3 : 1;
      __IO uint32_t SLPSS_PWRGATE_EN_N_ULP_TIMEPERIOD : 1;
      __IO uint32_t RESERVED : 5;
      __IO uint32_t SLPSS_PWRGATE_EN_N_ULP_NWPAPB_MCU_CTRL : 1;
      __IO uint32_t RESERVED1 : 15;
    } MCUAON_NPSS_PWRCTRL_SET_REG_b;
  };
  /*0x04*/
  union {
    __IO uint32_t
      MCUAON_NPSS_PWRCTRL_CLEAR_REG; /*!< none                                                                  */
    struct {
      __IO uint32_t RES : 1;
      __IO uint32_t SLPSS_PWRGATE_EN_N_ULP_MCUBFFS : 1;
      __IO uint32_t SLPSS_PWRGATE_EN_N_ULP_MCUFSM : 1;
      __IO uint32_t SLPSS_PWRGATE_EN_N_ULP_MCURTC : 1;
      __IO uint32_t SLPSS_PWRGATE_EN_N_ULP_MCUWDT : 1;
      __IO uint32_t SLPSS_PWRGATE_EN_N_ULP_MCUPS : 1;
      __IO uint32_t SLPSS_PWRGATE_EN_N_ULP_MCUTS : 1;
      __IO uint32_t SLPSS_PWRGATE_EN_N_ULP_MCUSTORE1 : 1;
      __IO uint32_t SLPSS_PWRGATE_EN_N_ULP_MCUSTORE2 : 1;
      __IO uint32_t SLPSS_PWRGATE_EN_N_ULP_MCUSTORE3 : 1;
      __IO uint32_t SLPSS_PWRGATE_EN_N_ULP_TIMEPERIOD : 1;
      __IO uint32_t RESERVED : 5;
      __IO uint32_t SLPSS_PWRGATE_EN_N_ULP_NWPAPB_MCU_CTRL : 1;
      __IO uint32_t RESERVED1 : 15;
    } MCUAON_NPSS_PWRCTRL_CLEAR_REG_b;
  };

  /*0x08*/
  __IO uint32_t RESERVED1;

  /*0x0C*/
  union {
    __IO uint32_t MCUAON_IPMU_RESET_CTRL; /*!< none                                                                  */
    struct {
      __IO uint32_t ULP_ANALOG_SPI_RESET_N : 1;
      __IO uint32_t IPMU_SPI_RESET_N : 1;
      __IO uint32_t RESERVED : 30;
    } MCUAON_IPMU_RESET_CTRL_b;
  };

  /*0x10*/
  union {
    __IO uint32_t MCUAON_SHELF_MODE; /*!< none                                                                  */
    struct {
      __IO uint32_t ENTER_SHELF_MODE : 16;
      __IO uint32_t SHUTDOWN_WAKEUP_MODE : 2;
      __IO uint32_t SHELF_MODE_GPIOBASED : 1;
      __IO uint32_t SHELF_MODE_WAKEUP_DELAY : 3;
      __IO uint32_t RESERVED1 : 10;
    } MCUAON_SHELF_MODE_b;
  };

  /*0x14*/
  union {
    __IO uint32_t MCUAON_GEN_CTRLS; /*!< none                                                                  */
    struct {
      __IO uint32_t XTAL_CLK_FROM_GPIO : 1;
      __IO uint32_t ULP_ANALOG_WAKEUP_ACCESS : 1;
      __IO uint32_t RES : 14;
      __IO uint32_t ENABLE_PDO : 1;
      __IO uint32_t NPSS_SUPPLY_0P9 : 1;
      __IO uint32_t RESERVED : 14;
    } MCUAON_GEN_CTRLS_b;
  };
  /*0x18*/
  union {
    __IO uint32_t MCUAON_PDO_CTRLS; /*!< none                                                                  */
    struct {
      __IO uint32_t SOC_B_IO_DOMAIN_EN_B : 1;
      __IO uint32_t SOC_L_IO_DOMAIN_EN_B : 1;
      __IO uint32_t SOC_T_IO_DOMAIN_EN_B : 1;
      __IO uint32_t QSPI_IO_DOMAIN_EN_B : 1;
      __IO uint32_t SDIO_IO_DOMAIN_EN_B : 1;
      __IO uint32_t RES : 27;
    } MCUAON_PDO_CTRLS_b;
  };

  /*0x1C*/
  union {
    __IO uint32_t MCUAON_WDT_CHIP_RST; /*!< none                                                                  */
    struct {
      __IO uint32_t MCU_WDT_BASED_CHIP_RESET : 1;
      __IO uint32_t RESERVED : 31;
    } MCUAON_WDT_CHIP_RST_b;
  };

  /*0x20*/
  union {
    __IO uint32_t
      MCUAON_KHZ_CLK_SEL_POR_RESET_STATUS; /*!< none                                                                  */
    struct {
      __IO uint32_t AON_KHZ_CLK_SEL : 3;
      __IO uint32_t AON_KHZ_CLK_SEL_CLOCK_SWITCHED : 1;
      __IO uint32_t RES : 12;
      __IO uint32_t MCU_FIRST_POWERUP_POR : 1;
      __IO uint32_t MCU_FIRST_POWERUP_RESET_N : 1;
      __IO uint32_t RESERVED : 14;
    } MCUAON_KHZ_CLK_SEL_POR_RESET_STATUS_b;
  };

} MCU_AON_Type;

/*MCU AON END */

/*START ULPSS CLOCKS */

typedef struct {
  union {
    __IO uint32_t ULP_SOC_GPIO_0_MODE_REG;
    struct {
      __IO uint32_t ULP_SOCGPIO_N_MODE : 3;
      __IO uint32_t RESERVED : 29;
    } ULP_SOC_GPIO_0_MODE_REG_b;
  };
} ULP_SOC_GPIO_MODE_t;

typedef struct {
  /*0x00*/
  union {
    __IO uint32_t ULP_MISC_SOFT_SET_REG;
    struct {
      __IO uint32_t PCM_ENABLE_b : 1;
      __IO uint32_t PCM_FSYNC_START_b : 1;
      __IO uint32_t BIT_RES : 2;
      __IO uint32_t IR_PCLK_EN_b : 1;
      __IO uint32_t PCLK_ENABLE_I2C_b : 1;
      __IO uint32_t CLK_ENABLE_I2S_b : 1;
      __IO uint32_t PCLK_ENABLE_SSI_MASTER_b : 1;
      __IO uint32_t SCLK_ENABLE_SSI_MASTER_b : 1;
      __IO uint32_t PCLK_ENABLE_UART_b : 1;
      __IO uint32_t SCLK_ENABLE_UART_b : 1;
      __IO uint32_t FIM_PCLK_ENABLE_b : 1;
      __IO uint32_t VAD_PCLK_ENABLE_b : 1;
      __IO uint32_t CLK_ENABLE_TIMER_b : 1;
      __IO uint32_t EGPIO_CLK_EN_b : 1;
      __IO uint32_t REG_ACCESS_SPI_CLK_EN_b : 1;
      __IO uint32_t FIM_CLK_EN_b : 1;
      __IO uint32_t VAD_CLK_EN_b : 1;
      __IO uint32_t CLK_ENABLE_ULP_MEMORIES_b : 1;
      __IO uint32_t EGPIO_PCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t EGPIO_PCLK_ENABLE_b : 1;
      __IO uint32_t TIMER_PCLK_EN_b : 1;
      __IO uint32_t AUX_ULP_EXT_TRIG_1_SEL_b : 1;
      __IO uint32_t AUX_ULP_EXT_TRIG_2_SEL_b : 1;
      __IO uint32_t AUX_SOC_EXT_TRIG_1_SEL_b : 1;
      __IO uint32_t AUX_SOC_EXT_TRIG_2_SEL_b : 1;
      __IO uint32_t ULPSS_M4SS_SLV_SEL_b : 1;
      __IO uint32_t ULPSS_TASS_QUASI_SYNC_b : 1;
      __IO uint32_t RESERVED : 2;
      __IO uint32_t FIM_AHB_CLK_ENABLE_b : 1;
      __IO uint32_t TOUCH_SENSOR_PCLK_ENABLE_b : 1;
    } ULP_MISC_SOFT_SET_REG_b;
  };

  /*0x04*/
  union {
    __IO uint32_t ULP_TA_PERI_ISO_REG;
    struct {
      __IO uint32_t UDMA_ISO_CNTRL_b : 1;
      __IO uint32_t IR_ISO_CNTRL_b : 1;
      __IO uint32_t I2C_ISO_CNTRL_b : 1;
      __IO uint32_t I2S_ISO_CNTRL_b : 1;
      __IO uint32_t SSI_ISO_CNTRL_b : 1;
      __IO uint32_t UART_ISO_CNTRL_b : 1;
      __IO uint32_t AUX_A2D_ISO_CNTRL_b : 1;
      __IO uint32_t VAD_ISO_CNTRL_b : 1;
      __IO uint32_t TOUCH_ISO_CNTRL_b : 1;
      __IO uint32_t PROC_MISC_ISO_CNTRL_b : 1;
      __IO uint32_t RESERVED0 : 1;
      __IO uint32_t RESERVED1 : 1;
      __IO uint32_t RESERVED2 : 1;
      __IO uint32_t RESERVED3 : 1;
      __IO uint32_t FIM_ISO_CNTRL_b : 1;
      __IO uint32_t MEM_2K_1_ISO_CNTRL_b : 1;
      __IO uint32_t MEM_2K_2_ISO_CNTRL_b : 1;
      __IO uint32_t MEM_2K_3_ISO_CNTRL_b : 1;
      __IO uint32_t MEM_2K_4_ISO_CNTRL_b : 1;
      __IO uint32_t MEM_2K_5_ISO_CNTRL_b : 1;
      __IO uint32_t MEM_2K_6_ISO_CNTRL_b : 1;
      __IO uint32_t MEM_2K_7_ISO_CNTRL_b : 1;
      __IO uint32_t MEM_2K_8_ISO_CNTRL_b : 1;
      __IO uint32_t RESERVED4 : 9;
    } ULP_TA_PERI_ISO_REG_b;
  };

  /*0x08*/
  union {
    __IO uint32_t ULP_TA_PERI_RESET_REG;
    struct {
      __IO uint32_t UDMA_SOFT_RESET_CNTRL_b : 1;
      __IO uint32_t IR_SOFT_RESET_CNTRL_b : 1;
      __IO uint32_t I2C_SOFT_RESET_CNTRL_b : 1;
      __IO uint32_t I2S_SOFT_RESET_CNTRL_b : 1;
      __IO uint32_t SSI_SOFT_RESET_CNTRL_b : 1;
      __IO uint32_t UART_SOFT_RESET_CNTRL_b : 1;
      __IO uint32_t AUX_A2D_SOFT_RESET_CNTRL_b : 1;
      __IO uint32_t VAD_SOFT_RESET_CNTRL_b : 1;
      __IO uint32_t TOUCH_SOFT_RESET_CNTRL_b : 1;
      __IO uint32_t PROC_MISC_SOFT_RESET_CNTRL_b : 1;
      __IO uint32_t RESERVED1 : 4;
      __IO uint32_t FIM_SOFT_RESET_CNTRL_b : 1;
      __IO uint32_t RESERVED : 17;

    } ULP_TA_PERI_RESET_REG_b;
  };

  /*0x0C*/
  __IO uint32_t RESERVED0;

  /*0x10*/
  __IO uint32_t RESERVED8;

  /*0x14*/
  union {
    __IO uint32_t ULP_TA_CLK_GEN_REG;
    struct {
      __IO uint32_t ULP_PROC_CLK_EN_b : 1;
      __IO uint32_t ULP_PROC_CLK_SEL : 4;
      __IO uint32_t ULP_PROC_CLK_DIV_FACTOR : 8;
      __IO uint32_t RES : 19;
    } ULP_TA_CLK_GEN_REG_b;
  };

  /*0x18*/
  union {
    __IO uint32_t ULP_I2C_SSI_CLK_GEN_REG;
    struct {
      __IO uint32_t ULP_I2C_CLK_EN_b : 1;
      __IO uint32_t RESERVED1 : 4;
      __IO uint32_t RESERVED2 : 8;
      __IO uint32_t RESERVED3 : 3;
      __IO uint32_t ULP_SSI_CLK_EN_b : 1;
      __IO uint32_t ULP_SSI_CLK_DIV_FACTOR : 7;
      __IO uint32_t RESERVED4 : 4;
      __IO uint32_t ULP_SSI_CLK_SEL : 4;
    } ULP_I2C_SSI_CLK_GEN_REG_b;
  };

  /*0x1C*/
  union {
    __IO uint32_t ULP_I2S_CLK_GEN_REG;
    struct {
      __IO uint32_t ULP_I2S_CLK_EN_b : 1;
      __IO uint32_t ULP_I2S_CLK_SEL_b : 4;
      __IO uint32_t ULP_I2S_CLKDIV_FACTOR : 8;
      __IO uint32_t ULP_I2S_MASTER_SLAVE_MODE_b : 1;
      __IO uint32_t ULP_I2S_SCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t RESERVED1 : 1;
      __IO uint32_t ULP_I2S_LOOP_BACK_MODE_b : 1;
      __IO uint32_t ULP_I2S_PCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t ULP_I2S_PCLK_EN_b : 1;
      __IO uint32_t RESERVED2 : 13;
    } ULP_I2S_CLK_GEN_REG_b;
  };

  /*0x20*/
  union {
    __IO uint32_t ULP_UART_CLK_GEN_REG;
    struct {
      __IO uint32_t ULP_UART_FRAC_CLK_SEL_b : 1;
      __IO uint32_t ULP_UART_CLK_SEL : 4;
      __IO uint32_t ULP_UART_CLKDIV_FACTOR : 3;
      __IO uint32_t RESERVED : 24;
    } ULP_UART_CLK_GEN_REG_b;
  };

  /*0x24*/
  union {
    __IO uint32_t M4LP_CTRL_REG;
    struct {
      __IO uint32_t RESERVED0 : 2;
      __IO uint32_t ULP_M4_CORE_CLK_ENABLE_b : 1;
      __IO uint32_t ULP_MEM_CLK_ULP_ENABLE_b : 1;
      __IO uint32_t ULP_MEM_CLK_ULP_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t RESERVED1 : 27;

    } M4LP_CTRL_REG_b;
  };

  /*0x28*/
  union {
    __IO uint32_t CLOCK_STAUS_REG;
    struct {
      __O uint32_t CLOCK_SWITCHED_UART_CLK_b : 1;
      __O uint32_t CLOCK_SWITCHED_I2S_CLK_b : 1;
      __O uint32_t CLOCK_SWITCHED_CORTEX_SLEEP_CLK_b : 1;
      __O uint32_t CLOCK_SWITCHED_PROC_CLK_b : 1;
      __O uint32_t CLOCK_SWITCHED_I2C_b : 1;
      __O uint32_t CLOCK_SWITCHED_SSI_b : 1;
      __O uint32_t CLOCK_SWITCHED_VAD_b : 1;
      __O uint32_t CLOCK_SWITCHED_AUXADC_b : 1;
      __O uint32_t CLOCK_SWITCHED_TIMER_b : 1;
      __O uint32_t CLOCK_SWITCHED_TOUCH_SENSOR_b : 1;
      __O uint32_t CLOCK_SWITCHED_FCLK_VAD_b : 1;
      __O uint32_t CLOCK_SWITCHED_SCLK_VAD_b : 1;
      __O uint32_t CLOCK_SWITCHED_SYSTICK_b : 1;
      __O uint32_t RESERVED : 19;
    } CLOCK_STAUS_REG_b;
  };

  /*0x2C*/
  union {
    __IO uint32_t ULP_TOUCH_CLK_GEN_REG;
    struct {
      __IO uint32_t ULP_TOUCH_CLK_EN_b : 1;
      __IO uint32_t ULP_TOUCH_CLK_SEL : 4;
      __IO uint32_t ULP_TOUCH_CLKDIV_FACTOR : 8;
      __IO uint32_t RESERVED : 19;
    } ULP_TOUCH_CLK_GEN_REG_b;
  };

  /*0x30*/
  union {
    __IO uint32_t ULP_TIMER_CLK_GEN_REG;
    struct {
      __IO uint32_t RESERVED1 : 1;
      __IO uint32_t ULP_TIMER_CLK_SEL : 4;
      __IO uint32_t RESERVED2 : 8;
      __IO uint32_t ULP_TIMER_IN_SYNC_b : 1;
      __IO uint32_t RESERVED3 : 18;
    } ULP_TIMER_CLK_GEN_REG_b;
  };

  /*0x34*/
  union {
    __IO uint32_t ULP_AUXADC_CLK_GEN_REG;
    struct {
      __IO uint32_t ULP_AUX_CLK_EN_b : 1;
      __IO uint32_t ULP_AUX_CLK_SEL : 4;
      __IO uint32_t ULP_AUX_CLKDIV_FACTOR : 8;
      __IO uint32_t RESERVED1 : 19;
    } ULP_AUXADC_CLK_GEN_REG_b;
  };

  /*0x38*/
  union {
    __IO uint32_t ULP_VAD_CLK_GEN_REG;
    struct {
      __IO uint32_t ULP_VAD_CLK_EN_b : 1;
      __IO uint32_t ULP_VAD_CLK_SEL : 3;
      __IO uint32_t ULP_VAD_FCLK_EN : 1;
      __IO uint32_t ULP_VAD_FCLK_SEL : 4;
      __IO uint32_t ULP_VAD_CLKDIV_FACTOR : 8;
      __IO uint32_t RESERVED : 15;

    } ULP_VAD_CLK_GEN_REG_b;
  };

  /*0x3C*/
  __IO uint32_t RESERVED1;

  /*0x40*/
  __IO uint32_t RESERVED2;

  /*0x44*/
  union {
    __IO uint32_t ULP_RM_RME_REG;
    struct {
      __IO uint32_t ULP_MEM_RME_b : 1;
      __IO uint32_t ULP_MEM_RM : 2;
      __IO uint32_t RESERVED1 : 1;
      __IO uint32_t ULP_MEM_RME_SRAM_b : 1;
      __IO uint32_t ULP_MEM_RM_SRAM : 2;
      __IO uint32_t RESERVED2 : 25;
    } ULP_RM_RME_REG_b;
  };

  /*0x48*/
  union {
    __IO uint32_t ULP_CLK_ENABLE_REG;
    struct {
      __IO uint32_t ULP_32KHZ_RO_CLK_EN_PROG_b : 1;
      __IO uint32_t ULP_32KHZ_RC_CLK_EN_PROG_b : 1;
      __IO uint32_t ULP_32KHZ_XTAL_CLK_EN_PROG_b : 1;
      __IO uint32_t ULP_DOUBLER_CLK_EN_PROG_b : 1;
      __IO uint32_t ULP_20MHZ_RO_CLK_EN_PROG_b : 1;
      __IO uint32_t ULP_32MHZ_RC_CLK_EN_PROG_b : 1;
      __IO uint32_t SOC_CLK_EN_PROG_b : 1;
      __IO uint32_t I2S_PLLCLK_EN_PROG_b : 1;
      __IO uint32_t REF_CLK_EN_IPS_PROG_b : 1;
      __IO uint32_t RESERVED : 23;
    } ULP_CLK_ENABLE_REG_b;
  };

  /*0x4C*/
  __IO uint32_t RESERVED3;

  /*0x50*/
  union {
    __IO uint32_t SYSTICK_CLK_GEN_REG;
    struct {
      __IO uint32_t SYSTICK_CLK_EN_b : 1;
      __IO uint32_t SYSTICK_CLK_SEL : 4;
      __IO uint32_t SYSTICK_CLKDIV_FACTOR : 8;
      __IO uint32_t RESERVED : 19;
    } SYSTICK_CLK_GEN_REG_b;
  };
  /*0x54*/
  __IO uint32_t RESERVED4;
  /*0x58*/
  __IO uint32_t RESERVED5;

  /*0x5C*/
  __IO uint32_t RESERVED6;

  /*0x60 -  0x9C*/
  ULP_SOC_GPIO_MODE_t ULP_SOC_GPIO_MODE_REG[16];

  /*0xA0*/
  union {
    __IO uint32_t ULP_DYN_CLK_CTRL_DISABLE;
    struct {
      __IO uint32_t I2C_PCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t I2S_CLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t SSI_MST_PCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t SSI_MST_SCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t UART_CLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t UART_SCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t TIMER_PCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t TIMER_SCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t REG_ACCESS_SPI_CLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t FIM_CLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t VAD_CLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t AUX_PCLK_EN_b : 1;
      __IO uint32_t AUX_CLK_EN_b : 1;
      __IO uint32_t AUX_MEM_EN_b : 1;
      __IO uint32_t AUX_PCLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t AUX_CLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t AUX_CLK_MEM_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t UDMA_CLK_ENABLE_b : 1;
      __IO uint32_t IR_CLK_ENABLE_b : 1;
      __IO uint32_t IR_CLK_DYN_CTRL_DISABLE_b : 1;
      __IO uint32_t RESERVED : 12;
    } ULP_DYN_CLK_CTRL_DISABLE_b;
  };

  //*0xA4*/
  union {
    __IO uint32_t SLP_SENSOR_CLK_REG;
    struct {
      __IO uint32_t DIVISON_FACTOR : 8;
      __IO uint32_t ENABLE_b : 1;
      __IO uint32_t RESERVED : 23;
    } SLP_SENSOR_CLK_REG_b;
  };
} ULPClk_Type;
/*END ULPSS CLOCKS*/

/*END OF POWER AND CLOCK REGISTERS */

/* fim  */

typedef struct { /*!< M4_FIM Structure                                                      */

  union {
    __IO uint32_t COP_MODE_INTR;

    struct {
      __IO uint32_t ULP_FIM_COP_MODE_LATCH : 1;
      __IO uint32_t MODE_TYPE : 8;
      __IO uint32_t RESERVED : 1;
      __IO uint32_t INTRPT : 1;
      __IO uint32_t RESERVED1 : 21;
    } COP_MODE_INTR_b;
  };

  union {
    __IO uint32_t COP_INPUT_ADDR_0;

    struct {
      __IO uint32_t ULP_OFFSET_ADDR_0 : 13; // data set 0 used by all modes
      __IO uint32_t RESERVED : 19;

    } COP_INPUT_ADDR_0_b;
  };

  union {
    __IO uint32_t COP_INPUT_ADDR_1;

    struct {
      __IO uint32_t ULP_OFFSET_ADDR_1 : 13; // data set 1 used by all modes except scalar and scaling
      __IO uint32_t RESERVED : 19;

    } COP_INPUT_ADDR_1_b;
  };

  union {
    __IO uint32_t COP_OUTPUT_ADDR;

    struct {
      __IO uint32_t ULP_ADDR_OUT_DATA : 13;
      __IO uint32_t RESERVED : 19;

    } COP_OUTPUT_ADDR_b;
  };

  union {
    __IO uint32_t COP_DATA0;

    struct {
      __IO uint32_t POLE_0_SCALAR : 32; // scalar value

    } COP_DATA0_b;
  };

  union {
    __IO uint32_t COP_DATA1;

    struct {
      __IO uint32_t POLE_1 : 32;

    } COP_DATA1_b;
  };

  union {
    __IO uint32_t COP_SAT_TRUNC_ROUND;

    struct {
      __IO uint32_t SAT : 5;   // saturated value
      __IO uint32_t TRUNC : 5; // truncated value
      __IO uint32_t SHIFT_RIGHT : 6;
      __IO uint32_t ROUND : 2;
      __IO uint32_t RESERVED : 14;
    } COP_SAT_TRUNC_ROUND_b;
  };

  union {
    __IO uint32_t COP_CONFIG_REG_0;

    struct {
      __IO uint32_t MATRIX_MODE_0 : 6; // no of columns in matrix1 and rows in matrix 2
      __IO uint32_t BUF_LEN_1 : 10;
      __IO uint32_t BUF_LEN_2 : 10;
      __IO uint32_t RESERVED : 6;

    } COP_CONFIG_REG_0_b;
  };

  union {
    __IO uint32_t COP_CONFIG_REG_1;

    struct {
      __IO uint32_t ULP_FIM_COP_START_TRIG : 1;
      __IO uint32_t RESERVED : 7;
      __IO uint32_t COMPLEX_FLAG : 2;          // not valid for matrix mode
      __IO uint32_t MATRIX_MODE_COLUMNS_2 : 6; // number of columns in matrix 2
      __IO uint32_t MATRIX_MODE_ROWS_1 : 6;    // number of rows in matrix 1
      __IO uint32_t INTERPOLATION_FACTOR : 6;
      __IO uint32_t RESERVED1 : 6;
    } COP_CONFIG_REG_1_b;
  };

} M4_FIM_TYPE;

typedef struct { /*!< NWP FSM one register Structure                                                      */

  union {
    __IO uint32_t TASS_REF_CLOCK_SELECT;
    struct {
      __IO uint32_t M4SS_REF_CLK_SEL_NWP : 3;
      __IO uint32_t RESERVED0 : 1;
      __IO uint32_t ULPSS_REF_CLK_SEL_NWP : 3;
      __IO uint32_t RESERVED1 : 9;
      __IO uint32_t TASS_REF_CLK_SEL_NWP : 3;
      __IO uint32_t RESERVED2 : 3;
      __IO uint32_t TASS_REF_CLK_CLEANER_OFF_NWP : 1;
      __IO uint32_t TASS_REF_CLK_CLEANER_ON_NWP : 1;
      __IO uint32_t RESERVED3 : 8;
    } TASS_REF_CLOCK_SELECT_b;
  };

} NWP_Fsm_t;

/* --------------------  End of section using anonymous unions  ------------------- */
#if defined(__CC_ARM)
#pragma pop
#elif defined(__ICCARM__)
/* leave anonymous unions enabled */
#elif defined(__GNUC__)
/* anonymous unions are enabled by default */
#elif defined(__TMS470__)
/* anonymous unions are enabled by default */
#elif defined(__TASKING__)
#pragma warning restore
#else
#warning Not supported compiler type
#endif

/* ================================================================================ */
/* ================              Peripheral memory map             ================ */
/* ================================================================================ */

#define I2C0_BASE        0x44010000UL
#define I2C1_BASE        0x47040000UL
#define I2C2_BASE        0x24040000UL
#define NWP_FSM_BASE     0x41300110UL
#define MCPWM_BASE       0x47070000UL
#define UDMA0_BASE       0x44030000UL
#define UDMA1_BASE       0x24078000UL
#define RPDMA_G_BASE     0x21080000UL
#define RPDMA_C_BASE     0x21081004UL
#define HWRNG_BASE       0x45090000UL
#define TIMERS_BASE      0x24042000UL
#define CAN1_BASE        0x45070000UL
#define QEI_BASE         0x47060000UL
#define UART1_BASE       0x44000000UL
#define USRT1_BASE       0x44000100UL
#define UART2_BASE       0x45020000UL
#define USRT2_BASE       0x45020100UL
#define ULP_UART_BASE    0x24041800UL
#define GSPI0_BASE       0x45030000UL
#define GSPI1_BASE       0x47030000UL
#define SSI0_BASE        0x44020000UL
#define SSISalve_BASE    0x45010000UL
#define SSI2_BASE        0x24040800UL
#define SIO_BASE         0x47000000UL
#define QSPI_BASE        0x12000000UL
#define CRC_BASE         0x45080000UL
#define EFUSE_BASE       0x4600C000UL
#define CCI_BASE         0x46170000UL
#define I2S0_BASE        0x47050000UL
#define I2S1_BASE        0x24040400UL
#define IID_AES_BASE     0x20480500UL
#define IID_QK_BASE      0x20480600UL
#define IID_RPINE_BASE   0x20480400UL
#define SCT0_BASE        0x45060000UL
#define SCT1_BASE        0x45061000UL
#define SCT2_BASE        0x45062000UL
#define SCT3_BASE        0x45063000UL
#define SCT_MUX_REG_BASE 0x4506F000UL
#define ETHERNET_BASE    0x20240000UL
#define EGPIO0_BASE      0x46130000UL
#define ULPSS_EGPIO_BASE 0x2404C000UL
#define SSDIO_BASE       0x40000000UL
#define SSPI_BASE        0x20200000UL
#define M4_FIM_BASE      0x24070000UL

/*Clock and power*/
#define M4_CLK_BASE               0x46000000UL
#define MCU_CALENDER_BASE         0x24048200UL
#define MCU_WDT_BASE              0x24048300UL
#define MCU_BATTERY_FF            0x24048400UL
#define MCU_RETENTION             0x24048600UL
#define MCU_TEMP_SENSOR_BASE      0x24048500UL
#define MCU_NPSS_AON_BASE         0x24048000UL
#define MCU_FSAM_BASE             0x24048100UL
#define MCU_PROCESSOR_SENSOR_BASE 0x24048540UL
#define ULP_CLK_BASE              0x24041400UL

/*End of clock and power */

/* ================================================================================ */
/* ================             Peripheral declaration             ================ */
/* ================================================================================ */

#define I2C0        ((I2C0_Type *)I2C0_BASE)
#define I2C1        ((I2C0_Type *)I2C1_BASE)
#define I2C2        ((I2C0_Type *)I2C2_BASE)
#define MCPWM       ((MCPWM_Type *)MCPWM_BASE)
#define UDMA0       ((UDMA0_Type *)UDMA0_BASE)
#define UDMA1       ((UDMA0_Type *)UDMA1_BASE)
#define RPDMA_G     ((RPDMA_G_Type *)RPDMA_G_BASE)
#define RPDMA_C     ((RPDMA_C_Type *)RPDMA_C_BASE)
#define HWRNG       ((HWRNG_Type *)HWRNG_BASE)
#define TIMERS      ((TIMERS_Type *)TIMERS_BASE)
#define CAN1        ((CAN0_Type *)CAN1_BASE)
#define QEI         ((QEI_Type *)QEI_BASE)
#define UART0       ((USART0_Type *)UART0_BASE)
#define USRT0       ((USART0_Type *)USRT0_BASE)
#define UART1       ((USART0_Type *)UART1_BASE)
#define USRT1       ((USART0_Type *)USRT1_BASE)
#define ULP_UART    ((USART0_Type *)ULP_UART_BASE)
#define GSPI0       ((GSPI0_Type *)GSPI0_BASE)
#define GSPI1       ((GSPI0_Type *)GSPI1_BASE)
#define SSI0        ((SSI0_Type *)SSI0_BASE)
#define SSISlave    ((SSI0_Type *)SSISalve_BASE)
#define SSI2        ((SSI0_Type *)SSI2_BASE)
#define SIO         ((SIO_Type *)SIO_BASE)
#define QSPI        ((QSPI_Type *)QSPI_BASE)
#define CRC         ((CRC_Type *)CRC_BASE)
#define EFUSE       ((EFUSE_Type *)EFUSE_BASE)
#define CCI         ((CCI_Type *)CCI_BASE)
#define I2S0        ((I2S0_Type *)I2S0_BASE)
#define I2S1        ((I2S0_Type *)I2S1_BASE)
#define IID_AES     ((IID_AES_Type *)IID_AES_BASE)
#define IID_QK      ((IID_QK_Type *)IID_QK_BASE)
#define IID_RPINE   ((IID_RPINE_Type *)IID_RPINE_BASE)
#define SCT0        ((SCT0_Type *)SCT0_BASE)
#define SCT1        ((SCT0_Type *)SCT1_BASE)
#define SCT2        ((SCT0_Type *)SCT2_BASE)
#define SCT3        ((SCT0_Type *)SCT3_BASE)
#define SCT_MUX_REG ((SCT_MUX_REG_Type *)SCT_MUX_REG_BASE)
#define ETHERNET    ((ETHERNET_Type *)ETHERNET_BASE)
#define EGPIO       ((EGPIO_Type *)EGPIO0_BASE)
#define ULPSSEGPIO  ((EGPIO_Type *)ULPSS_EGPIO_BASE)
#define SDIO        ((SDIO0_Type *)SSDIO_BASE)
#define SPI_SLAVE   ((RSI_SPI_SLAVE_T *)SSPI_BASE)
#define M4_FIM      ((M4_FIM_TYPE *)M4_FIM_BASE)

/*clock and power */
#define M4CLK           ((M4Clk_Type *)M4_CLK_BASE)
#define CALENDER        ((MCU_Calender_t *)MCU_CALENDER_BASE)
#define WDT             ((MCU_WDT_t *)MCU_WDT_BASE)
#define TIME_PERIOD     ((TimePeriodCalib_t *)MCU_CALENDER_BASE)
#define BATT_FF         ((MCU_Battery *)MCU_BATTERY_FF)
#define MCU_RET         ((MCU_RETENTION_Type *)MCU_RETENTION)
#define MCU_TEMP        ((NPSS_TEMPSENSOR_Type *)MCU_TEMP_SENSOR_BASE)
#define MCU_AON         ((MCU_AON_Type *)MCU_NPSS_AON_BASE)
#define MCU_FSM         ((MCU_Fsm_t *)MCU_FSAM_BASE)
#define PROCESSOR_SENSE ((MCU_ProcessSensor_t *)MCU_PROCESSOR_SENSOR_BASE)
#define ULPCLK          ((ULPClk_Type *)ULP_CLK_BASE)
#define NWP_FSM         ((NWP_Fsm_t *)NWP_FSM_BASE)
/*end of clock and power */

/** @} */ /* End of group Device_Peripheral_Registers */
/** @} */ /* End of group RSI9116 */
/** @} */ /* End of group SiLabs Signals Inc. */

#define MISC_CONFIG_BASE 0x46008000UL

#ifdef __cplusplus
}
#endif

#endif /* __RSI9116_H__*/
