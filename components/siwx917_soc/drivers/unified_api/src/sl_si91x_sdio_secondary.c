/*******************************************************************************
 * @file  sl_si91x_sdio_secondary.c
 * @brief
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

#include "sl_si91x_sdio_secondary.h"
#include "sl_si91x_peripheral_sdio_secondary.h"
#include "sl_si91x_sdio_secondary_drv_config.h"

/*******************************************************************************
 ***************************  DEFINES / MACROS   ********************************
 ******************************************************************************/
#define SDIO_SECONDARY_RELEASE_VERSION 0 // SDIO Secondary Release version
#define SDIO_SECONDARY_SQA_VERSION     0 // SDIO Secondary SQA version
#define SDIO_SECONDARY_DEV_VERSION     2 // SDIO Secondary Developer version

/*******************************************************************************
 ******************************   VARIABLES   **********************************
 ******************************************************************************/

#if defined(__CC_ARM)
// Keil alignment to 1024 bytes
__align(1024) RSI_GPDMA_DESC_T GPDMADesc[NUMGPDMADESC];
#endif // defined (__CC_ARM)

// IAR support
#if defined(__ICCARM__)
#pragma data_alignment = 1024
static RSI_GPDMA_DESC_T GPDMADesc[NUMGPDMADESC];
#endif // defined (__ICCARM__)

#if defined(__GNUC__)
RSI_GPDMA_DESC_T GPDMADesc[NUMGPDMADESC];
#endif // defined (__GNUC__)

static uint32_t memBuff[30]; //gpdma driver memory context, driver context
                             //size is less than 30 bypes that's why size is 30
uint32_t memSize = 0, *devMem = NULL;
RSI_GPDMA_HANDLE_T GPDMAHandle;

/*******************************************************************************
 ************************       GLOBAL FUNCTIONS      **************************
 ******************************************************************************/
extern __INLINE uint16_t sl_si91x_sdio_secondary_get_block_cnt(void);
extern __INLINE uint16_t sl_si91x_sdio_secondary_get_block_len(void);
extern __INLINE void sl_si91x_sdio_secondary_set_tx_blocks(uint8_t no_of_blocks);
extern __INLINE void sl_si91x_sdio_secondary_set_interrupts(uint32_t flags);
extern __INLINE void sl_si91x_sdio_secondary_clear_interrupts(uint32_t flags);

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/
static sl_status_t sl_si91x_gpdma_init();
static void sl_si91x_fill_rx_descriptors(uint8_t *data_buf);
static void sl_si91x_fill_tx_descriptors(uint8_t *data_buff, uint8_t num_of_blocks);
static void sl_si91x_setup_rx_channel_desc(uint8_t *data_buf);
static void sl_si91x_setup_tx_channel_desc(uint8_t *data_buf, uint8_t num_of_blocks);

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/
static sl_sdio_secondary_callback_t user_callback = NULL; // SDIO Secondary user callback function variable
static sl_sdio_secondary_gpdma_callback_t user_gpdma_callback =
  NULL; // SDIO Secondary user gpdma callback function varialbe

/*******************************************************************************
 ***************************   LOCAL FUNCTIONS   *******************************
 ******************************************************************************/

/*******************************************************************************
 * @fn     void GPDMA_Handler(void)
 * @brief  GPDMA Interrupt Handler. Clears the dma interrupt triggered for the 
 *         SDIO channel. And calls the user callback handler.
 * @param  None
 * @return None
 ******************************************************************************/
void GPDMA_Handler(void)
{
  RSI_GPDMA_InterruptHandler(GPDMAHandle);
}

/*******************************************************************************
 * @fn     void SDIO_Handler(void)
 * @brief  SDIO Interrupt Handler ,clear sdio primary got , sets the event and 
 *         calls the user callback function to user level handling
 * @param  None
 * @return None
 ******************************************************************************/
void SDIO_Handler(void)
{
  uint8_t events = 0;

  // clear the command53 write interrupt
  sl_si91x_sdio_secondary_clear_interrupts(BIT(0));

  events |= HOST_INTR_RECEIVE_EVENT;

  user_callback(events);
}

/*******************************************************************************
 * @fn          void GPDMATransferComplete(RSI_GPDMA_HANDLE_T GPDMAHandle, RSI_GPDMA_DESC_T *pTranDesc)
 * @brief       GPDMA controller transfer descriptor chain complete callback
 * @param[in]   GPDMAHandle : structure variable to driver context handle
 * @param[in]   pTranDesc   : Pointer to transfer descriptor
 * @return      None
 ******************************************************************************/
void GPDMATransferComplete(RSI_GPDMA_HANDLE_T GPDMAHandle_p, RSI_GPDMA_DESC_T *pTranDesc, uint32_t dmaCh)
{
  (void)pTranDesc;
  (void)GPDMAHandle_p;
  user_gpdma_callback(dmaCh);
}

/*******************************************************************************
 * @fn          void GPDMATransferDescComplete(RSI_GPDMA_HANDLE_T GPDMAHandle, RSI_GPDMA_DESC_T *pTranDesc)
 * @brief       GPDMA controller transfer descriptor complete callback
 * @param[in]   GPDMAHandle: structure variable to driver context handle
 * @param[in]   pTranDesc  : Pointer to transfer descriptor
 * @return      None
 ******************************************************************************/
void GPDMATransferDescComplete(RSI_GPDMA_HANDLE_T GPDMAHandle_p, RSI_GPDMA_DESC_T *pTranDesc, uint32_t dmaCh)
{
  (void)pTranDesc;
  (void)GPDMAHandle_p;
  (void)dmaCh;
}

/*******************************************************************************
 * @fn          void GPDMATransferError(RSI_GPDMA_HANDLE_T GPDMAHandle, RSI_GPDMA_DESC_T *pTranDesc)
 * @brief       GPDMA controller transfer descriptor error callback
 * @param[in]   GPDMAHandle : structure variable to driver context handle
 * @param[in]   pTranDesc   : Pointer to transfer descriptor
 * @return      None
 ******************************************************************************/
void GPDMATransferError(RSI_GPDMA_HANDLE_T GPDMAHandle_p, RSI_GPDMA_DESC_T *pTranDesc, uint32_t dmaCh)
{
  (void)pTranDesc;
  (void)GPDMAHandle_p;
  (void)dmaCh;
}

/*******************************************************************************
 * @fn          void GPDMATransferDescFetchComplete(RSI_GPDMA_HANDLE_T GPDMAHandle, RSI_GPDMA_DESC_T *pTranDesc)
 * @brief       DMA controller transfer descriptor error callback
 * @param[in]   GPDMAHandle: structure variable to driver context handle
 * @param[in]   pTranDesc  : Pointer to transfer descriptor
 * @return      None
 ******************************************************************************/
void GPDMATransferDescFetchComplete(RSI_GPDMA_HANDLE_T GPDMAHandle_p, RSI_GPDMA_DESC_T *pTranDesc, uint32_t dmaCh)
{
  (void)pTranDesc;
  (void)GPDMAHandle_p;
  (void)dmaCh;
}

/*******************************************************************************
 * @fn          void sl_si91x_fill_rx_descriptors(uint8_t *data_buf)
 * @brief       Filled RX GPDMA descriptor
 * @param[in]   data_buf : Pointer to RX buffer
 * @return      None
 ******************************************************************************/
void sl_si91x_fill_rx_descriptors(uint8_t *data_buf)
{
  uint32_t j;
  RSI_GPDMA_DESC_T *pPrevDesc;
  uint32_t no_of_desc = 0;
  RSI_GPDMA_DESC_T XferCfg;

  // Get the SDIO block count
  no_of_desc = sl_si91x_sdio_secondary_get_block_cnt();

  pPrevDesc = NULL;
  for (j = 0; j < no_of_desc; j++) {

    // Channel Control Config
    XferCfg.chnlCtrlConfig.transSize        = sl_si91x_sdio_secondary_get_block_len();
    XferCfg.chnlCtrlConfig.transType        = PERIPHERAL_MEMORY;
    XferCfg.chnlCtrlConfig.dmaFlwCtrl       = DMA_FLW_CTRL;
    XferCfg.chnlCtrlConfig.mastrIfFetchSel  = MASTER0_FETCH_IFSEL;
    XferCfg.chnlCtrlConfig.mastrIfSendSel   = MASTER0_SEND_IFSEL;
    XferCfg.chnlCtrlConfig.destDataWidth    = DST_8_DATA_WIDTH;
    XferCfg.chnlCtrlConfig.srcDataWidth     = SRC_32_DATA_WIDTH;
    XferCfg.chnlCtrlConfig.srcAlign         = 0;
    XferCfg.chnlCtrlConfig.linkListOn       = 1;
    XferCfg.chnlCtrlConfig.linkListMstrSel  = LINK_MASTER_0_FTCH;
    XferCfg.chnlCtrlConfig.srcAddContiguous = 0;
    XferCfg.chnlCtrlConfig.dstAddContiguous = 0;
    XferCfg.chnlCtrlConfig.retryOnErr       = 0;
    XferCfg.chnlCtrlConfig.linkInterrupt    = 0;
    XferCfg.chnlCtrlConfig.srcFifoMode      = 1;
    XferCfg.chnlCtrlConfig.dstFifoMode      = 0;

    // Misc Channel Config
    XferCfg.miscChnlCtrlConfig.ahbBurstSize  = AHBBURST_SIZE_4;
    XferCfg.miscChnlCtrlConfig.destDataBurst = DST_BURST_SIZE_4;
    XferCfg.miscChnlCtrlConfig.srcDataBurst  = SRC_BURST_SIZE_4;
    XferCfg.miscChnlCtrlConfig.destChannelId = 0;
    XferCfg.miscChnlCtrlConfig.srcChannelId  = 30;
    XferCfg.miscChnlCtrlConfig.dmaProt       = 0;
    XferCfg.miscChnlCtrlConfig.memoryFillEn  = 0;
    XferCfg.miscChnlCtrlConfig.memoryOneFill = 0;

    XferCfg.dest = (data_buf + (j * sl_si91x_sdio_secondary_get_block_len()));
    XferCfg.src  = (uint32_t *)(RX_SOURCE_ADDR);
    if (RSI_GPDMA_BuildDescriptors(GPDMAHandle, &XferCfg, &GPDMADesc[j], pPrevDesc) != RSI_OK) {
    }
    pPrevDesc = &GPDMADesc[j];
  }
}

/*******************************************************************************
 * @fn          void sl_si91x_fill_tx_descriptors(uint8_t *data_buff, uint8_t num_of_blocks)
 * @brief       Filled TX GPDMA descriptor
 * @param[in]   data_buf      : Pointer to TX buffer
 * @param[in]   num_of_blocks : Numbers of block transmit.
 * @return      None
 ******************************************************************************/
void sl_si91x_fill_tx_descriptors(uint8_t *data_buff, uint8_t num_of_blocks)
{
  uint32_t j;
  RSI_GPDMA_DESC_T *pPrevDesc;
  uint32_t no_of_desc = 0;
  RSI_GPDMA_DESC_T XferCfg;

  no_of_desc = num_of_blocks;

  pPrevDesc = NULL;
  for (j = 0; j < no_of_desc; j++) {
    // Channel Control Config
    XferCfg.chnlCtrlConfig.transSize        = TX_TRANSFER_BLOCK_SIZE;
    XferCfg.chnlCtrlConfig.transType        = MEMORY_PERIPHERAL;
    XferCfg.chnlCtrlConfig.dmaFlwCtrl       = DMA_FLW_CTRL;
    XferCfg.chnlCtrlConfig.mastrIfFetchSel  = MASTER0_FETCH_IFSEL;
    XferCfg.chnlCtrlConfig.mastrIfSendSel   = MASTER0_SEND_IFSEL;
    XferCfg.chnlCtrlConfig.destDataWidth    = DST_32_DATA_WIDTH;
    XferCfg.chnlCtrlConfig.srcDataWidth     = SRC_8_DATA_WIDTH;
    XferCfg.chnlCtrlConfig.srcAlign         = 0;
    XferCfg.chnlCtrlConfig.linkListOn       = 1;
    XferCfg.chnlCtrlConfig.linkListMstrSel  = LINK_MASTER_0_FTCH;
    XferCfg.chnlCtrlConfig.srcAddContiguous = 0;
    XferCfg.chnlCtrlConfig.dstAddContiguous = 0;
    XferCfg.chnlCtrlConfig.retryOnErr       = 0;
    XferCfg.chnlCtrlConfig.linkInterrupt    = 1;
    XferCfg.chnlCtrlConfig.srcFifoMode      = 0;
    XferCfg.chnlCtrlConfig.dstFifoMode      = 1;

    // Misc Channel Config
    XferCfg.miscChnlCtrlConfig.ahbBurstSize  = AHBBURST_SIZE_8;
    XferCfg.miscChnlCtrlConfig.destDataBurst = DST_BURST_SIZE_8;
    XferCfg.miscChnlCtrlConfig.srcDataBurst  = SRC_BURST_SIZE_8;
    XferCfg.miscChnlCtrlConfig.destChannelId = 31;
    XferCfg.miscChnlCtrlConfig.srcChannelId  = 0;
    XferCfg.miscChnlCtrlConfig.dmaProt       = 0;
    XferCfg.miscChnlCtrlConfig.memoryFillEn  = 0;
    XferCfg.miscChnlCtrlConfig.memoryOneFill = 0;

    XferCfg.dest = (uint32_t *)(TX_SOURCE_ADDR);
    XferCfg.src  = (data_buff + (j * TX_TRANSFER_BLOCK_SIZE));
    // Build the descriptor parameters
    if (RSI_GPDMA_BuildDescriptors(GPDMAHandle, &XferCfg, &GPDMADesc[j], pPrevDesc) != RSI_OK) {
    }
    pPrevDesc = &GPDMADesc[j];
  }
}

/*******************************************************************************
 * @fn          void sl_si91x_setup_rx_channel_desc(uint8_t *data_buf)
 * @brief       Setup GPDMA RX channel descriptor
 * @param[in]   data_buf : Pointer to RX buffer
 * @return      None
 ******************************************************************************/
void sl_si91x_setup_rx_channel_desc(uint8_t *data_buf)
{
  RSI_GPDMA_CHA_CFG_T chaCfg;

  chaCfg.channelPrio       = 0;
  chaCfg.descFetchDoneIntr = 0;
  chaCfg.hrespErr          = 1;
  chaCfg.gpdmacErr         = 1;
  chaCfg.xferDoneIntr      = 1;
  chaCfg.dmaCh             = GPDMA_CHNL1;

  RSI_GPDMA_SetupChannel(GPDMAHandle, (RSI_GPDMA_CHA_CFG_T *)&chaCfg);

  sl_si91x_fill_rx_descriptors(data_buf);

  RSI_GPDMA_SetupChannelTransfer(GPDMAHandle, GPDMA_CHNL1, GPDMADesc);
}

/*******************************************************************************
 * @fn          void sl_si91x_setup_tx_channel_desc(uint8_t *data_buf, uint8_t num_of_blocks)
 * @brief       Setup GPDMA TX channel descriptor
 * @param[in]   data_buf      : Pointer to TX buffer
 * @param[in]   num_of_blocks : Numbers of block transmit.
 * @return      None
 ******************************************************************************/
void sl_si91x_setup_tx_channel_desc(uint8_t *data_buf, uint8_t num_of_blocks)
{
  RSI_GPDMA_CHA_CFG_T chaCfg;

  chaCfg.channelPrio       = 0;
  chaCfg.descFetchDoneIntr = 0;
  chaCfg.hrespErr          = 1;
  chaCfg.gpdmacErr         = 1;
  chaCfg.xferDoneIntr      = 1;
  chaCfg.dmaCh             = GPDMA_CHNL0;

  // Setup channel 1 for trigger operation and M2M transfer
  RSI_GPDMA_SetupChannel(GPDMAHandle, (RSI_GPDMA_CHA_CFG_T *)&chaCfg);

  sl_si91x_fill_tx_descriptors(data_buf, num_of_blocks);

  RSI_GPDMA_SetupChannelTransfer(GPDMAHandle, GPDMA_CHNL0, GPDMADesc);
}

/*******************************************************************************
 * Initialize the gpdma
 ******************************************************************************/
sl_status_t sl_si91x_gpdma_init()
{
  uint32_t memSize_t, *devMem_p;
  RSI_GPDMA_INIT_T GPDMAInit;
  volatile bool done;

  // Clear stack structures before use
  memset(&GPDMAInit, 0, sizeof(RSI_GPDMA_INIT_T));

  // Get needed size for driver context memory
  memSize_t = RSI_GPDMA_GetMemSize();
  if (memSize_t > sizeof(memBuff)) {
    return SL_STATUS_INVALID_CONFIGURATION;
  }

  devMem_p = memBuff;

  // Initialize driver context parameters
  GPDMAInit.pUserData = (void *)&done;
  GPDMAInit.baseC     = (uint32_t)GPDMA_C;
  GPDMAInit.baseG     = (uint32_t)GPDMA_G;
  GPDMAInit.sramBase  = (uint32_t)&GPDMADesc;

  // Initialize driver
  GPDMAHandle = RSI_GPDMA_Init(devMem_p, &GPDMAInit);
  if (GPDMAHandle == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  // Enable GPDMA clock
  RSI_CLK_PeripheralClkEnable(M4CLK, RPDMA_CLK, ENABLE_STATIC_CLK);

  // Enable the interrupt for the DMA controller
  NVIC_EnableIRQ(GPDMA_IRQn);

  // Register error, descriptor completion, and descriptor chain completion callbacks for channel 1
  RSI_GPDMA_RegisterCallback(GPDMAHandle, RSI_GPDMA_XFERCOMPLETE_CB, (gpdmaTransferCompleteCB)GPDMATransferComplete);
  RSI_GPDMA_RegisterCallback(GPDMAHandle,
                             RSI_GPDMA_XFERDESCFETCHCOMPLETE_CB,
                             (gpdmaTransferDescFetchCompleteCB)GPDMATransferDescFetchComplete);
  RSI_GPDMA_RegisterCallback(GPDMAHandle, RSI_GPDMA_XFERHRESPERROR_CB, (gpdmaTransferCompleteCB)GPDMATransferError);
  RSI_GPDMA_RegisterCallback(GPDMAHandle, RSI_GPDMA_XFERGPDMACERROR_CB, (gpdmaTransferCompleteCB)GPDMATransferError);

  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API initializes the sdio secondary and gpdma initialization and returns
 * error if any in gpdma initialization
 ******************************************************************************/
sl_status_t sl_si91x_sdio_secondary_init(void)
{
  sl_status_t status;
  // Initialize the SDIO secondary
  sl_si91x_sdio_secondary_peripheral_init();
  // Initialize the GPDMA
  status = sl_si91x_gpdma_init();
  // Return the status
  return status;
}

/*******************************************************************************
 * To register the event callback
 * It registers the callback, i.e., stores the callback function address
 * and pass to the variable that is called in Interrupt Handler.
 * If another callback is registered without unregistering previous callback then, it
 * returns an error code, so it is mandatory to unregister the callback before registering
 * another callback.
 * It will returns error if any callback is already registered.
 ******************************************************************************/
sl_status_t sl_si91x_sdio_secondary_register_event_callback(sl_sdio_secondary_callback_t callback_event, uint32_t flag)
{
  sl_status_t status;
  do {
    // Validates the null pointer, if true returns error code
    if (callback_event == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    //Enable and set the SDIO interrupt
    sl_si91x_sdio_secondary_enable_interrupts(flag);

    sl_si91x_sdio_secondary_set_interrupts(flag);
    // To validate the function pointer if the parameters is not NULL then, it
    // returns an error code
    if (user_callback != NULL) {
      status = SL_STATUS_BUSY;
      break;
    }
    // User callback address is passed to the static variable which is called at the time of
    // interrupt
    user_callback = callback_event;
    // Returns SL_STATUS_OK if callback is successfully registered
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * It unregisters the callback, i.e., clear the callback function address
 * and pass NULL value to the variable
 ******************************************************************************/
void sl_si91x_sdio_secondary_unregister_event_callback(uint32_t flag)
{
  //Disbale the interrupts
  sl_si91x_sdio_secondary_disable_interrupts(flag);

  // Pass the NULL value to the static variable which is called at the time of
  // interrupt.
  // It is further validated in register callback API.
  user_callback = NULL;
}

/*******************************************************************************
 * To register the event callback
 * It registers the callback, i.e., stores the callback function address
 * and pass to the variable that is called in Interrupt Handler.
 * If another callback is registered without unregistering previous callback then, it
 * returns an error code, so it is mandatory to unregister the callback before registering
 * another callback.
 * It will returns error if any callback is already registered.
 ******************************************************************************/
sl_status_t sl_si91x_sdio_secondary_gpdma_register_event_callback(sl_sdio_secondary_gpdma_callback_t callback_event)
{
  sl_status_t status;
  do {
    // Validates the null pointer, if true returns error code
    if (callback_event == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // To validate the function pointer if the parameters is not NULL then, it
    // returns an error code
    if (user_gpdma_callback != NULL) {
      status = SL_STATUS_BUSY;
      break;
    }
    // User callback address is passed to the static variable which is called at the time of
    // interrupt
    user_gpdma_callback = callback_event;
    // Returns SL_STATUS_OK if callback is successfully registered
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * It unregisters the callback, i.e., clear the callback function address
 * and pass NULL value to the variable
 ******************************************************************************/
void sl_si91x_sdio_secondary_gpdma_unregister_event_callback(void)
{
  // Pass the NULL value to the static variable which is called at the time of
  // interrupt.
  // It is further validated in register callback API.
  user_gpdma_callback = NULL;
}

/*******************************************************************************
 * This API is used to receive the data from sdio primary ,this api will fill
 * the gpdma receive decscriptors and triggers the gpdma channel.
 * This API also enables the sdio fn interrupt to recive next packets
 ******************************************************************************/
void sl_si91x_sdio_secondary_receive(uint8_t *data_buf)
{
  sl_si91x_setup_rx_channel_desc(data_buf);

  // Trigger channel
  RSI_GPDMA_DMAChannelTrigger(GPDMAHandle, GPDMA_CHNL1);
  // Enable the SDIO write interrupt
  sl_si91x_sdio_secondary_set_interrupts(SL_SDIO_WR_INT_UNMSK);
}

/*******************************************************************************
 * This API is used to send the data from sdio seccondary ,this api will fill
 * the gpdma transfer decscriptors and triggers the gpdma channel.
 * This also enables the interrupt to sdio primary
 ******************************************************************************/
void sl_si91x_sdio_secondary_send(uint8_t num_of_blocks, uint8_t *data_buf)
{
  sl_si91x_setup_tx_channel_desc(data_buf, num_of_blocks);

  // Program number of blocks
  sl_si91x_sdio_secondary_set_tx_blocks(num_of_blocks);

  // Raise interrupt to host
  M4_HOST_INTR_STATUS_REG = BIT(3);

  // Trigger channel
  RSI_GPDMA_DMAChannelTrigger(GPDMAHandle, GPDMA_CHNL0);
}

/*******************************************************************************
 * To get the release, sqa and dev version of SDIO SECONDARY
 * It returns the structure for gspi version.
 * Structure includes three members:
 * - Release version
 * - Major version (SQA version)
 * - Minor version (Dev version)
 ******************************************************************************/
sl_sdio_secondary_version_t sl_si91x_sdio_secondary_get_version(void)
{
  sl_sdio_secondary_version_t version;
  version.release = SDIO_SECONDARY_RELEASE_VERSION;
  version.major   = SDIO_SECONDARY_SQA_VERSION;
  version.minor   = SDIO_SECONDARY_DEV_VERSION;
  return version;
}
