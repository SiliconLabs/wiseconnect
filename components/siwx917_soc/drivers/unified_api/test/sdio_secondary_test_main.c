/***************************************************************************/ /**
 * @file
 * @brief SDIO secondary Unity Test Functions
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
 ******************************************************************************
 ******************************************************************************/

#include "sl_si91x_sdio_secondary.h"
#include "rsi_board.h"
#include "unity.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define SL_SDIO_WR_INT_EN      BIT(0) // This bit is used to enable CMD53 write interrupt
#define SL_SDIO_RD_INT_EN      BIT(1) // This bit is used to enable CMD53 read interrupt
#define SL_SDIO_CSA_INT_EN     BIT(2) // This bit is used to enable CMD53 CSA interrupt
#define SL_SDIO_CMD52_INT_EN   BIT(3) // This bit is used to enable CMD52 interrupt
#define SL_SDIO_PWR_LEV_INT_EN BIT(4) // This bit is used to enable power level change interrupt
#define SL_SDIO_CRC_ERR_INT_EN BIT(5) // This bit is used to enable CRC error interrupt
#define SL_SDIO_ABORT_INT_EN   BIT(6) // This bit is used to enable abort interrupt
#define SL_SDIO_TOUT_INT_EN    BIT(7) // This bit is used to enable "read FIFO wait time over" interrupt

#define BLOCK_LEN        256
#define NO_OF_BLOCKS     4
#define XFER_BUFFER_SIZE (BLOCK_LEN * NO_OF_BLOCKS) // Buffer size is 256B*4 = 1KB

#define FUNCTIONAL_TEST 1 //To enable the functional Test fefine this macro with 1
/*******************************************************************************
 ***************************  Global Variables   *******************************
 ******************************************************************************/

/*******************************************************************************
 ***************************  Local Variables   *******************************
 ******************************************************************************/
volatile uint8_t host_intr_event = 0;
volatile uint8_t dmaDone         = 0;
uint8_t xfer_buffer[XFER_BUFFER_SIZE];

/*******************************************************************************
 ************************  Function Prototypes   ************************
 ******************************************************************************/

/***************************************************************************/ /**
 * @brief
 *   This is an application callback function for host interrupt events raised
 *   within the IRQ handler in the sdio slave peripheral driver
 *
 * @param[in] events
 *   Each bit has an event linked to it
 ******************************************************************************/
void application_callback(uint8_t events)
{
  if ((events & HOST_INTR_RECEIVE_EVENT) == 1) {
    host_intr_event = 1;
  }
}

/***************************************************************************/ /**
 * @brief
 *   This is an application callback function for the data transger success using gpdma
 *   within the IRQ handler in the sdio secondary peripheral driver
 *
 ******************************************************************************/
void gdpma_callbak(uint8_t dma_ch)
{
  dmaDone = 1;
}

/*******************************************************************************
 ************************  Test Function Prototypes  ****************************
 ******************************************************************************/
static void test_sdio_secondary_init(void);
static void test_register_and_unregister_callback(void);
static void test_sdio_secondary_get_version(void);
#if FUNCTIONAL_TEST
static void test_sdio_secondary_send(void);
static void test_sdio_secondary_receive(void);
#endif
/***************************************************************************/ /**
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  sl_system_init();
  UnityBeginGroup("SDIO Secondary");

  RUN_TEST(test_sdio_secondary_init, __LINE__);
  RUN_TEST(test_register_and_unregister_callback, __LINE__);
  RUN_TEST(test_sdio_secondary_get_version, __LINE__);
#if FUNCTIONAL_TEST
  RUN_TEST(test_sdio_secondary_send, __LINE__);
  RUN_TEST(test_sdio_secondary_receive, __LINE__);
#endif
  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

static void test_sdio_secondary_init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SDIO Secondary Initialization \n");
  sl_status_t status;

  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_sdio_secondary_init();
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, SDIO Secondary is initialized successfully \n");

  UnityPrintf("SDIO Secondary Initialization test completed \n");
}

static void test_register_and_unregister_callback(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SDIO Secondary register and unregister callback \n");
  sl_status_t status;

  UnityPrintf("Registering callback \n");
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");
  status = sl_si91x_sdio_secondary_register_event_callback(NULL, SL_SDIO_WR_INT_EN);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");
  status = sl_si91x_sdio_secondary_register_event_callback(application_callback, SL_SDIO_WR_INT_EN);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Callback is registered successfully \n");

  status = sl_si91x_sdio_secondary_register_event_callback(application_callback, SL_SDIO_WR_INT_EN);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_BUSY, status);
  UnityPrintf("Status of API is correct, Callback is already registered, cannot register another\n");
  UnityPrintf("Unregistering the callback \n");
  sl_si91x_sdio_secondary_unregister_event_callback(SL_SDIO_WR_INT_EN);
  UnityPrintf("Callback is unregistered successfully \n");
  status = sl_si91x_sdio_secondary_register_event_callback(application_callback, SL_SDIO_WR_INT_EN);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Callback is again registered successfully \n");
}

static void test_sdio_secondary_get_version(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SDIO Secondary Get Version \n");
  sl_sdio_secondary_version_t version;

  UnityPrintf("Testing API with valid parameter \n");
  version = sl_si91x_sdio_secondary_get_version();
  UnityPrintf("Verifying SDIO Release, SQA and Dev version \n");
  TEST_ASSERT_EQUAL_INT(0, version.release);
  TEST_ASSERT_EQUAL_INT(0, version.major);
  TEST_ASSERT_EQUAL_INT(2, version.minor);
  UnityPrintf("API version is correct\n");

  UnityPrintf("SDIO get version test completed \n");
}

#if FUNCTIONAL_TEST
static void test_sdio_secondary_send(void)
{
  sl_status_t status;
  UnityPrintf("\n");
  UnityPrintf("Testing SDIO Secondary send \n");

  // Filled data in tx buffer
  for (int i = 0; i < XFER_BUFFER_SIZE; i++) {
    xfer_buffer[i] = (i / 256) + 1;
  }

  SysTick_Config(SystemCoreClock / 1000);

  status = sl_si91x_sdio_secondary_init();
  if (status != SL_STATUS_OK) {
    DEBUGOUT("\r\nSDIO Secondary init failed\r\n");
  }
  DEBUGOUT("\r\nSDIO secondary init success\r\n");

  status = sl_si91x_sdio_secondary_register_event_callback(application_callback, SL_SDIO_WR_INT_EN);
  if (status != SL_STATUS_OK) {
    DEBUGOUT("\r\nSDIO Secondary callback function registration failed\r\n");
  }
  DEBUGOUT("\r\nSDIO Secondary callback function registration success\r\n");

  sl_si91x_sdio_secondary_gpdma_register_event_callback(gdpma_callbak);

  sl_si91x_sdio_secondary_send(NO_OF_BLOCKS, xfer_buffer);

  UnityPrintf("SDIO Secondary Send test completed \n");
}

static void test_sdio_secondary_receive(void)
{
  sl_status_t status;
  UnityPrintf("\n");
  UnityPrintf("Testing SDIO Secondary receive \n");

  SysTick_Config(SystemCoreClock / 1000);

  status = sl_si91x_sdio_secondary_init();
  if (status != SL_STATUS_OK) {
    DEBUGOUT("\r\nSDIO Secondary init failed\r\n");
  }
  DEBUGOUT("\r\nSDIO secondary init success\r\n");

  status = sl_si91x_sdio_secondary_register_event_callback(application_callback, SL_SDIO_WR_INT_EN);
  if (status != SL_STATUS_OK) {
    DEBUGOUT("\r\nSDIO Secondary callback function registration failed\r\n");
  }
  DEBUGOUT("\r\nSDIO Secondary callback function registration success\r\n");

  sl_si91x_sdio_secondary_gpdma_register_event_callback(gdpma_callbak);

  sl_si91x_sdio_secondary_receive(xfer_buffer);

  while (!(dmaDone & host_intr_event))
    ;

  UnityPrintf("SDIO Secondary Receive test completed \n");
}
#endif
