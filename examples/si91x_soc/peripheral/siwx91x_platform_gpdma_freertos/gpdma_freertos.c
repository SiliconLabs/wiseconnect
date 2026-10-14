/***************************************************************************/ /**
 * @file gpdma_freertos.c
 * @brief GPDMA memory-to-memory example under FreeRTOS (single translation unit)
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 ******************************************************************************/
#include <stdbool.h>

#include "cmsis_os2.h"
#include "gpdma_freertos.h"
#include "rsi_debug.h"
#include "sl_si91x_gpdma.h"

/*******************************************************************************
  *******************************   DEFINES   ***********************************
  ******************************************************************************/
#define GPDMA_MAX_TRANSFER_LENGTH_CHANNEL0 1024 * 4 // Maximum transfer length allowed for given channel
#define GPDMA_TRANSFER_LENGTH              1024 * 4 // Transfer length in bytes
#define SL_GPDMA_SIMPLE_TRANSFER           1
#define GPDMA_CHANNEL                      0
#define SL_SI91X_GPDMA_SOURCE_BURST        16
#define SL_SI91X_GPDMA_DESTINATION_BURST   16
// Calculate the maximum number of descriptors needed
#define SL_MAX_NUMBER_OF_DESCRIPTORS_CHANNEL0 \
  ((GPDMA_MAX_TRANSFER_LENGTH_CHANNEL0 + MAX_TRANSFER_PER_DESCRIPTOR - 1) / MAX_TRANSFER_PER_DESCRIPTOR)

/** Set from DMA transfer-complete callback; waited on by the GPDMA task */
#define GPDMA_EVENT_TRANSFER_COMPLETE (1U << 0)
/** Set from DMA error callbacks so the task does not block forever on failure */
#define GPDMA_EVENT_ERROR (1U << 1)

#define GPDMA_EVENTS_DONE (GPDMA_EVENT_TRANSFER_COMPLETE | GPDMA_EVENT_ERROR)

/*******************************************************************************
  ********************************  BUFFERS  ************************************
  ******************************************************************************/

sl_si91x_gpdma_descriptor_t
  sl_descriptors_memory[SL_MAX_NUMBER_OF_DESCRIPTORS_CHANNEL0]; // Memory for storing the descriptors
uint8_t src[GPDMA_TRANSFER_LENGTH] = { 0 };                     // Source buffer for transfer
uint8_t dst[GPDMA_TRANSFER_LENGTH] = { 0 };                     // Destination buffer for transfer
uint32_t gpdma_channel             = GPDMA_CHANNEL;             // GPDMA Channel used
/*******************************************************************************
  ***************************  LOCAL VARIABLES  ********************************
  ******************************************************************************/
static osEventFlagsId_t gpdma_event_flags = NULL;

/*******************************************************************************
  **********************  Local Function prototypes   ***************************
  ******************************************************************************/
static void transfer_complete_callback(void);
static void gpdma_controller_error_callback(void);
static void hresp_error_callback(void);
static void fetch_done_callback(void);
static void signal_gpdma_error(void);
static bool gpdma_example_init(void);

/*******************************************************************************
   **********************  Local Function definitions   ***************************
   ******************************************************************************/

static void signal_gpdma_error(void)
{
  if (gpdma_event_flags != NULL) {
    (void)osEventFlagsSet(gpdma_event_flags, GPDMA_EVENT_ERROR);
  }
}

static void transfer_complete_callback(void)
{
  SL_PRINT_STRING_ERROR("Transfer complete callback\r\n");
  if (gpdma_event_flags != NULL) {
    (void)osEventFlagsSet(gpdma_event_flags, GPDMA_EVENT_TRANSFER_COMPLETE);
  }
}

static void gpdma_controller_error_callback(void)
{
  SL_PRINT_STRING_ERROR("GPDMA controller error callback\r\n");
  signal_gpdma_error();
}

static void hresp_error_callback(void)
{
  SL_PRINT_STRING_ERROR("HRESP error callback\r\n");
  signal_gpdma_error();
}

static void fetch_done_callback(void)
{
  SL_PRINT_STRING_ERROR("Fetch done callback\r\n");
}

/***************************************************************************/ /**
  * Initialize GPDMA example.
  * @return true if setup completed; false if a step failed.
  ******************************************************************************/
static bool gpdma_example_init(void)
{
  sl_status_t status                     = SL_STATUS_OK;
  uint32_t channel_priority              = 0;
  sl_gpdma_callback_pointer_t callback_t = { 0 };

  gpdma_event_flags = osEventFlagsNew(NULL);
  if (gpdma_event_flags == NULL) {
    /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
    SL_PRINT_STRING_ERROR("GPDMA event flags create failed\r\n");
    return false;
  }
#if (SL_GPDMA_SIMPLE_TRANSFER != 1)
  sl_si91x_gpdma_descriptor_config_t descriptor_config = { 0 };

  descriptor_config.chnlCtrlConfig.transSize       = (uint32_t)(MAX_TRANSFER_PER_DESCRIPTOR);
  descriptor_config.chnlCtrlConfig.transType       = SL_GPDMA_MEMORY_TO_MEMORY;        // Transfer type
  descriptor_config.chnlCtrlConfig.dmaFlwCtrl      = SL_GPDMAC_FL0W_CTRL;              // Flow control
  descriptor_config.chnlCtrlConfig.mastrIfFetchSel = SL_GPDMA_MASTER_ZERO;             //Master select for source
  descriptor_config.chnlCtrlConfig.mastrIfSendSel  = SL_GPDMA_MASTER_ONE;              // Master select for destination
  descriptor_config.chnlCtrlConfig.linkListMstrSel = SL_GPDMA_MASTER_ZERO;             //Master select for descriptor
  descriptor_config.chnlCtrlConfig.destDataWidth   = SL_GPDMA_DATA_WIDTH_32;           //Destination data width
  descriptor_config.chnlCtrlConfig.srcDataWidth    = SL_GPDMA_DATA_WIDTH_32;           //Source data width
  descriptor_config.chnlCtrlConfig.linkListOn      = SL_LINK_LIST_MODE_ENABLE;         //Linked list mode enable/disable
  descriptor_config.chnlCtrlConfig.linkInterrupt   = SL_LINK_LIST_INTERRUPT_ENABLE;    //Interupt per descriptor
  descriptor_config.chnlCtrlConfig.srcFifoMode     = SL_SOURCE_FIFO_MODE_DISABLE;      //FIFO mode for source
  descriptor_config.chnlCtrlConfig.dstFifoMode     = SL_DESTINATION_FIFO_MODE_DISABLE; //FIFO mode for destination

  descriptor_config.miscChnlCtrlConfig.ahbBurstSize  = SL_GPDMA_AHBBURST_SIZE_8;         //AHB burst size
  descriptor_config.miscChnlCtrlConfig.destDataBurst = SL_SI91X_GPDMA_DESTINATION_BURST; //Source data width
  descriptor_config.miscChnlCtrlConfig.srcDataBurst  = SL_SI91X_GPDMA_SOURCE_BURST;      //Destination data width
  descriptor_config.miscChnlCtrlConfig.dmaProt       = SL_DMA_PROTECTION_DISABLE;
  descriptor_config.miscChnlCtrlConfig.memoryFillEn  = SL_MEMORY_FILL_DISABLE; //Memory fill enable
  descriptor_config.miscChnlCtrlConfig.memoryOneFill = SL_MEMORY_ZERO_FILL;    //Memory fill zero/one
  descriptor_config.src                              = src;
  descriptor_config.dest                             = dst;
  descriptor_config.pNextLink                        = NULL;
#endif

  for (int i = 0; i < GPDMA_TRANSFER_LENGTH; i++) {
    src[i] = i; //Fill the source buffer with data
    dst[i] = 0; //Initialize the destination buffer to zero
  }

  //Call back functions, these are optional user can implement their own call back functions or give it NULL if not need
  callback_t.descripotr_fetch_complete_cb = fetch_done_callback;             // Fetch done callback
  callback_t.gpdmac_error_cb              = gpdma_controller_error_callback; // GPDMA controller error callback
  callback_t.hresp_error_cb               = hresp_error_callback;            // HRESP error callback
  callback_t.transfer_complete_cb         = transfer_complete_callback;      // Transfer complete callback

  //Allocate a channel for GPDMA transfer
  status = sl_si91x_gpdma_allocate_channel(&gpdma_channel, channel_priority, GPDMA_MAX_TRANSFER_LENGTH_CHANNEL0);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Allocate channel fail %lu \r\n", status);
    return false;
  }
  SL_PRINT_STRING_ERROR("Allocate channel success\r\n");

  //Register the callback functions for GPDMA transfer
  status = sl_si91x_gpdma_register_callbacks(gpdma_channel, &callback_t);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Register callback fail %lu \r\n", status);
    return false;
  }
  SL_PRINT_STRING_ERROR("Register callback success\r\n");

#if SL_GPDMA_SIMPLE_TRANSFER
  //In simple transfer allocate a descriptor for GPDMA transfer
  status = sl_si91x_gpdma_allocate_descriptor(sl_descriptors_memory, GPDMA_TRANSFER_LENGTH, gpdma_channel);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Descriptor allocation failed %lu \r\n", status);
    return false;
  }
  SL_PRINT_STRING_ERROR("Descriptor allocation success\r\n");

#else

  //In normal transfer build a descriptor for GPDMA transfer
  status =
    sl_si91x_gpdma_build_descriptor(sl_descriptors_memory, &descriptor_config, GPDMA_TRANSFER_LENGTH, gpdma_channel);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Build descriptor fail %lu \r\n", status);
    return false;
  }
  SL_PRINT_STRING_ERROR("Build descriptor success\r\n");

#endif
  return true;
}

static void gpdma_task(void *argument)
{
  (void)argument;

  sl_status_t status = SL_STATUS_OK;
  ;
  uint32_t count = 0;
  uint32_t wait_result;

  if (!gpdma_example_init()) {
    SL_PRINT_STRING_ERROR("GPDMA init failed, exiting task\r\n");
    osThreadExit();
  }

  status = sl_si91x_gpdma_transfer(gpdma_channel, src, dst);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Transfer_fail %lu \r\n", status);
    osThreadExit();
  }

  wait_result = osEventFlagsWait(gpdma_event_flags, GPDMA_EVENTS_DONE, osFlagsWaitAny, osWaitForever);
  if (((wait_result & osFlagsError) != 0U) || ((wait_result & GPDMA_EVENT_ERROR) != 0U)
      || ((wait_result & GPDMA_EVENT_TRANSFER_COMPLETE) == 0U)) {
    SL_PRINT_STRING_ERROR("GPDMA wait failed or incomplete (flags 0x%lx)\r\n", wait_result);
    osThreadExit();
  }

  for (int i = 0; i < GPDMA_TRANSFER_LENGTH; i++) {
    if (src[i] != dst[i]) {
      break;
    }
    count++;
  }
  if (count == GPDMA_TRANSFER_LENGTH) {
    SL_PRINT_STRING_ERROR("GPDMA Transfer success\r\n");
  } else {
    SL_PRINT_STRING_ERROR("GPDMA Transfer fail\r\n");
    osThreadExit();
  }

  osThreadExit();
}

void gpdma_freertos_init(void)
{
  static const osThreadAttr_t attr = {
    .name       = "gpdma",
    .stack_size = 2048,
    .priority   = osPriorityLow1,
  };
  if (osThreadNew((osThreadFunc_t)gpdma_task, NULL, &attr) == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create gpdma thread\r\n");
  }
}
