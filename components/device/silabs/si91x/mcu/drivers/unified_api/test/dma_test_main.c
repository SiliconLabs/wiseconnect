/***************************************************************************/ /**
 * @file main.c
 * @brief main() function.
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
#include "unity.h"
#include "sl_si91x_dma.h"
#include "sl_system_init.h"

#define BUFFER_SIZE 1024

static void test_si91x_dma_init(void);
static void test_si91x_dma_allocate_channel(void);
static void test_si91x_dma_deallocate_channel(void);
static void test_si91x_dma_register_callbacks(void);
static void test_si91x_dma_unregister_callbacks(void);
static void test_si91x_dma_simple_transfer(void);
static void test_si91x_dma_transfer(void);
static void test_si91x_dma_deinit(void);
void transfer_complete_callback_dmadrv(uint32_t channel, void *data);
void error_callback_dmadrv(uint32_t channel, void *data);
uint8_t data_comparision(uint32_t *src, uint32_t *dst);
void transfer_complete_callback_simultaneous(uint32_t channel, void *data);

uint32_t src[BUFFER_SIZE]      = { 0 };
uint32_t dst[BUFFER_SIZE]      = { 0 };
uint32_t src2[BUFFER_SIZE]     = { 0 };
uint32_t dst2[BUFFER_SIZE]     = { 0 };
volatile int transfer_complete = 0;

int main(void)
{
  sl_system_init();
  UnityBeginGroup("DMA");

  RUN_TEST(test_si91x_dma_init, __LINE__);
  RUN_TEST(test_si91x_dma_allocate_channel, __LINE__);
  RUN_TEST(test_si91x_dma_deallocate_channel, __LINE__);
  RUN_TEST(test_si91x_dma_register_callbacks, __LINE__);
  RUN_TEST(test_si91x_dma_unregister_callbacks, __LINE__);
  RUN_TEST(test_si91x_dma_simple_transfer, __LINE__);
  RUN_TEST(test_si91x_dma_transfer, __LINE__);
  RUN_TEST(test_si91x_dma_deinit, __LINE__);
  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

static void test_si91x_dma_init(void)
{
  sl_status_t status;
  sl_dma_init_t dma_init_t;
  UnityPrintf("\n");
  UnityPrintf("Testing DMA Initialization \n");
  UnityPrintf("Testing with invalid instance \n");
  dma_init_t.dma_number = 2;
  status                = sl_si91x_dma_init(&dma_init_t);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("invalid instance test success\n");
  UnityPrintf("Initializing UDMA0 instance\n");
  dma_init_t.dma_number = 0;
  status                = sl_si91x_dma_init(&dma_init_t);
  UnityPrintf("UDMA0 init success\n");
  UnityPrintf("Initializing UDMA1 instance\n");
  dma_init_t.dma_number = 1;
  status                = sl_si91x_dma_init(&dma_init_t);
  UnityPrintf("UDMA1 init success\n");
}

static void test_si91x_dma_allocate_channel(void)
{
  sl_status_t status;
  uint32_t channel;
  uint32_t channel_inc = 0;
  UnityPrintf("\n");
  UnityPrintf("Testing DMA channel allocation \n");
  UnityPrintf("Testing with NULL channel structure \n");
  status = sl_si91x_dma_allocate_channel(0, NULL, 0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("NULL channel check success\n");

  UnityPrintf("Testing with invalid instance no\n");
  channel = 0;
  status  = sl_si91x_dma_allocate_channel(2, &channel, 0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("invalid instance test success\n");

  UnityPrintf("Testing with invalid priority\n");
  status = sl_si91x_dma_allocate_channel(0, &channel, 2);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("invalid priority test success\n");

  UnityPrintf("Testing with invalid UDMA0 channel no\n");
  channel = SL_DMA0_CHANNEL_COUNT + 1;
  status  = sl_si91x_dma_allocate_channel(0, &channel, 0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("invalid UDMA0 channel no test pass\n");

  UnityPrintf("Testing with invalid UDMA1 channel no\n");
  channel = SL_ULP_DMA_CHANNEL_COUNT + 1;
  status  = sl_si91x_dma_allocate_channel(1, &channel, 0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("invalid UDMA1 channel no test pass\n");

  UnityPrintf("Testing auto UDMA0 channel allocation\n");
  do {
    channel = 0;
    status  = sl_si91x_dma_allocate_channel(0, &channel, 0);
    if (status == SL_STATUS_OK) {
      UnityPrintf("UDMA0 channel %d allocated\n", channel);
      channel_inc++;
    }
  } while (status == SL_STATUS_OK);
  TEST_ASSERT_EQUAL_HEX(SL_DMA0_CHANNEL_COUNT, channel_inc);
  UnityPrintf("auto UDMA0 channel allocation test pass\n");

  UnityPrintf("Testing auto UDMA1 channel allocation\n");
  channel_inc = 0;
  for (int i = 0; i < SL_ULP_DMA_CHANNEL_COUNT; i++) {
    channel = 0;
    status  = sl_si91x_dma_allocate_channel(1, &channel, 0);
    TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  }
  UnityPrintf("auto UDMA1 channel allocation test pass\n");

  UnityPrintf("allocating UDMA0 channel no 1 again\n");
  channel = 1;
  status  = sl_si91x_dma_allocate_channel(0, &channel, 0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_DMA_CHANNEL_ALLOCATED, status);
  UnityPrintf("UDMA0 channel no 1 not allocated, test pass\n");

  UnityPrintf("allocating UDMA1 channel no 1 again\n");
  channel = 1;
  status  = sl_si91x_dma_allocate_channel(1, &channel, 0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_DMA_CHANNEL_ALLOCATED, status);
  UnityPrintf("UDMA1 channel no 1 not allocated, test pass\n");

  UnityPrintf("De-allocating UDMA0 channels\n");
  for (channel = 1; channel <= SL_DMA0_CHANNEL_COUNT; channel++) {
    status = sl_si91x_dma_deallocate_channel(0, channel);
    TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
    //UnityPrintf("UDMA0 channel %d deallocated\n", channel);
  }
  TEST_ASSERT_EQUAL_HEX(SL_DMA0_CHANNEL_COUNT + 1, channel);
  channel = 0;
  UnityPrintf("De-allocated all UDMA0 channels\n");

  UnityPrintf("De-allocating UDMA1 channels\n");
  for (channel = 1; channel <= SL_ULP_DMA_CHANNEL_COUNT; channel++) {
    status = sl_si91x_dma_deallocate_channel(1, channel);
    TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
    //UnityPrintf("UDMA1 channel %d deallocated\n", channel);
  }
  TEST_ASSERT_EQUAL_HEX(SL_ULP_DMA_CHANNEL_COUNT + 1, channel);
  channel = 0;
  UnityPrintf("De-allocated all UDMA1 channels\n");

  UnityPrintf("Manual UDMA0 channel allocation\n");
  for (channel = 1; channel < (SL_DMA0_CHANNEL_COUNT + 1); channel++) {
    status = sl_si91x_dma_allocate_channel(0, &channel, 0);
    TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
    UnityPrintf("UDMA0 channel %d allocated\n", channel);
  }
  TEST_ASSERT_EQUAL_HEX(SL_DMA0_CHANNEL_COUNT + 1, channel);
  UnityPrintf("Manual UDMA0 channel allocation test pass\n");

  UnityPrintf("Manual UDMA1 channel allocation\n");
  for (channel = 1; channel < (SL_ULP_DMA_CHANNEL_COUNT + 1); channel++) {
    status = sl_si91x_dma_allocate_channel(1, &channel, 0);
    TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
    UnityPrintf("UDMA1 channel %d allocated\n", channel);
  }
  TEST_ASSERT_EQUAL_HEX(SL_ULP_DMA_CHANNEL_COUNT + 1, channel);
  UnityPrintf("Manual UDMA1 channel allocation test pass\n");

  UnityPrintf("De-allocating UDMA0 channels\n");
  for (channel = 1; channel <= SL_DMA0_CHANNEL_COUNT; channel++) {
    status = sl_si91x_dma_deallocate_channel(0, channel);
    TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
    //UnityPrintf("UDMA0 channel %d deallocated\n", channel);
  }
  TEST_ASSERT_EQUAL_HEX(SL_DMA0_CHANNEL_COUNT + 1, channel);
  channel = 0;
  UnityPrintf("De-allocated all UDMA0 channels\n");

  UnityPrintf("De-allocating UDMA1 channels\n");
  for (channel = 1; channel <= SL_ULP_DMA_CHANNEL_COUNT; channel++) {
    status = sl_si91x_dma_deallocate_channel(1, channel);
    TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
    //UnityPrintf("UDMA1 channel %d deallocated\n", channel);
  }
  TEST_ASSERT_EQUAL_HEX(SL_ULP_DMA_CHANNEL_COUNT + 1, channel);
  channel = 0;
  UnityPrintf("De-allocated all UDMA1 channels\n");
}

static void test_si91x_dma_deallocate_channel(void)
{
  sl_status_t status;
  uint32_t channel = 1;

  UnityPrintf("\n");
  UnityPrintf("Testing DMA channel deallocation \n");

  UnityPrintf("Testing with invalid instance no\n");
  status = sl_si91x_dma_deallocate_channel(2, channel);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("invalid instance no test pass\n");

  UnityPrintf("Testing UDMA0 with invalid channel no 0\n");
  channel = 0;
  status  = sl_si91x_dma_deallocate_channel(0, channel);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Deallocation fail, test pass\n");

  channel = SL_DMA0_CHANNEL_COUNT + 1;
  UnityPrintf("Testing UDMA0 with invalid channel no %d\n", channel);
  status = sl_si91x_dma_deallocate_channel(0, channel);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Deallocation fail, test pass\n");

  UnityPrintf("Testing UDMA1 with invalid channel no 0\n");
  channel = 0;
  status  = sl_si91x_dma_deallocate_channel(1, channel);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Deallocation fail, test pass\n");

  channel = SL_ULP_DMA_CHANNEL_COUNT + 1;
  UnityPrintf("Testing UDMA1 with invalid channel no %d\n", channel);
  status = sl_si91x_dma_deallocate_channel(1, channel);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Deallocation fail, test pass\n");

  UnityPrintf("Testing UDMA0 channel 1 multiple deallocation\n");
  channel = 1;
  status  = sl_si91x_dma_deallocate_channel(0, channel);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_DMA_CHANNEL_ALREADY_UNALLOCATED, status);
  UnityPrintf("Channel already deallocated, test pass\n");

  UnityPrintf("Testing UDMA1 channel 1 multiple deallocation\n");
  channel = 1;
  status  = sl_si91x_dma_deallocate_channel(1, channel);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_DMA_CHANNEL_ALREADY_UNALLOCATED, status);
  UnityPrintf("Channel already deallocated, test pass\n");

  UnityPrintf("UDMA0 channel 1 allocation\n");
  channel = 1;
  status  = sl_si91x_dma_allocate_channel(0, &channel, 0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("UDMA0 channel 1 allocated\n");
  UnityPrintf("Testing UDMA0 channel 1 deallocation\n");
  channel = 1;
  status  = sl_si91x_dma_deallocate_channel(0, channel);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Channel 1 deallocated, test pass\n");

  UnityPrintf("UDMA1 channel 1 allocation\n");
  channel = 1;
  status  = sl_si91x_dma_allocate_channel(1, &channel, 0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("UDMA1 channel 1 allocated\n");
  UnityPrintf("Testing UDMA1 channel 1 deallocation\n");
  channel = 1;
  status  = sl_si91x_dma_deallocate_channel(1, channel);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Channel 1 deallocated, test pass\n");
}

static void test_si91x_dma_register_callbacks(void)
{
  sl_status_t status;
  uint32_t channel = 1;
  sl_dma_callback_t callback_t;

  UnityPrintf("\n");
  UnityPrintf("Testing DMA register callback \n");

  UnityPrintf("Testing Invalid DMA instance \n");
  status = sl_si91x_dma_register_callbacks(2, channel, &callback_t);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Invalid DMA instance test pass \n");

  UnityPrintf("Testing Invalid channel no 0 for UDMA0 \n");
  channel = 0;
  status  = sl_si91x_dma_register_callbacks(0, channel, &callback_t);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Invalid channel no 0, test pass \n");

  channel = SL_DMA0_CHANNEL_COUNT + 1;
  UnityPrintf("Testing Invalid channel no %d for UDMA0 \n", channel);
  status = sl_si91x_dma_register_callbacks(0, channel, &callback_t);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Invalid channel no 33, test pass \n");

  UnityPrintf("Testing Invalid channel no 0 for UDMA1 \n");
  channel = 0;
  status  = sl_si91x_dma_register_callbacks(1, channel, &callback_t);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Invalid channel no 0, test pass \n");

  channel = SL_ULP_DMA_CHANNEL_COUNT + 1;
  UnityPrintf("Testing Invalid channel no %d for UDMA1 \n", channel);
  status = sl_si91x_dma_register_callbacks(1, channel, &callback_t);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Invalid channel no 13, test pass \n");

  UnityPrintf("Testing Null pointer passing \n");
  channel = 1;
  status  = sl_si91x_dma_register_callbacks(1, channel, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Null pointer passing test pass \n");

  UnityPrintf("Allocating UDMA0 channel 1 \n");
  channel = 1;
  status  = sl_si91x_dma_allocate_channel(0, &channel, 0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("UDMA0 channel 1 allocated\n");
  UnityPrintf("Registering callback for UDMA 0 channel 1 \n");
  status = sl_si91x_dma_register_callbacks(0, channel, &callback_t);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Registering callback for UDMA 0 channel 1 success \n");

  UnityPrintf("Allocating UDMA1 channel 1 \n");
  channel = 1;
  status  = sl_si91x_dma_allocate_channel(1, &channel, 0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("UDMA1 channel 1 allocated\n");
  UnityPrintf("Registering callback for UDMA 1 channel 1 \n");
  status = sl_si91x_dma_register_callbacks(1, channel, &callback_t);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Registering callback for UDMA 1 channel 1 success \n");
}

static void test_si91x_dma_unregister_callbacks(void)
{
  sl_status_t status;
  uint32_t channel = 1;

  UnityPrintf("\n");
  UnityPrintf("Testing DMA unregister callback \n");

  UnityPrintf("Testing Invalid DMA instance \n");
  status = sl_si91x_dma_unregister_callbacks(2, channel, SL_DMA_TRANSFER_DONE_CB | SL_DMA_ERROR_CB);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Invalid DMA instance test pass \n");

  UnityPrintf("Testing Invalid channel no 0 for UDMA0 \n");
  channel = 0;
  status  = sl_si91x_dma_unregister_callbacks(0, channel, SL_DMA_TRANSFER_DONE_CB | SL_DMA_ERROR_CB);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Invalid channel no 0, test pass \n");

  channel = SL_DMA0_CHANNEL_COUNT + 1;
  UnityPrintf("Testing Invalid channel no %d for UDMA0 \n", channel);
  status = sl_si91x_dma_unregister_callbacks(0, channel, SL_DMA_TRANSFER_DONE_CB | SL_DMA_ERROR_CB);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Invalid channel no 33, test pass \n");

  UnityPrintf("Testing Invalid channel no 0 for UDMA1 \n");
  channel = 0;
  status  = sl_si91x_dma_unregister_callbacks(1, channel, SL_DMA_TRANSFER_DONE_CB | SL_DMA_ERROR_CB);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Invalid channel no 0, test pass \n");

  channel = SL_ULP_DMA_CHANNEL_COUNT + 1;
  UnityPrintf("Testing Invalid channel no %d for UDMA1 \n", channel);
  status = sl_si91x_dma_unregister_callbacks(1, channel, SL_DMA_TRANSFER_DONE_CB | SL_DMA_ERROR_CB);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Invalid channel no 13, test pass \n");

  UnityPrintf("Testing Invalid callback type \n");
  channel = 1;
  status  = sl_si91x_dma_unregister_callbacks(1, channel, 0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Invalid callback type, test pass \n");

  UnityPrintf("Unregistering UDMA0 channel 1 transfer complete callback \n");
  channel = 1;
  status  = sl_si91x_dma_unregister_callbacks(0, channel, SL_DMA_TRANSFER_DONE_CB);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Unregistered UDMA0 channel 1 transfer complete callback \n");

  UnityPrintf("Unregistering UDMA0 channel 1 error callback \n");
  status = sl_si91x_dma_unregister_callbacks(0, channel, SL_DMA_ERROR_CB);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Unregistered UDMA0 channel 1 error callback \n");

  UnityPrintf("Unregistering UDMA1 channel 11 callbacks \n");
  channel = 1;
  status  = sl_si91x_dma_unregister_callbacks(1, channel, SL_DMA_TRANSFER_DONE_CB | SL_DMA_ERROR_CB);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Unregistered UDMA1 channel 10 callback \n");
}

static void test_si91x_dma_simple_transfer(void)
{
  sl_status_t status;
  uint32_t channel = 1;
  sl_dma_callback_t callback_t;

  UnityPrintf("\n");
  UnityPrintf("Testing DMA simple transfer \n");

  UnityPrintf("Testing Invalid DMA instance \n");
  status = sl_si91x_dma_simple_transfer(2, channel, src, dst, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Invalid DMA instance test pass \n");

  UnityPrintf("Testing Invalid channel no 0 for UDMA0 \n");
  channel = 0;
  status  = sl_si91x_dma_simple_transfer(0, channel, src, dst, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Invalid channel no 0, test pass \n");

  channel = SL_DMA0_CHANNEL_COUNT + 1;
  UnityPrintf("Testing Invalid channel no %d for UDMA0 \n", channel);
  status = sl_si91x_dma_simple_transfer(0, channel, src, dst, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Invalid channel no 33, test pass \n");

  UnityPrintf("Testing Invalid channel no 0 for UDMA1 \n");
  channel = 0;
  status  = sl_si91x_dma_simple_transfer(1, channel, src, dst, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Invalid channel no 0, test pass \n");

  channel = SL_ULP_DMA_CHANNEL_COUNT + 1;
  UnityPrintf("Testing Invalid channel no %d for UDMA1 \n", channel);
  status = sl_si91x_dma_simple_transfer(1, channel, src, dst, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Invalid channel no 13, test pass \n");

  UnityPrintf("Testing with NULL src addr \n");
  channel = 2;
  status  = sl_si91x_dma_simple_transfer(0, channel, NULL, dst, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Invalid src addr, test pass \n");

  UnityPrintf("Testing with NULL dst addr \n");
  status = sl_si91x_dma_simple_transfer(0, channel, src, NULL, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Invalid dst addr, test pass \n");

  UnityPrintf("Testing with invalid data size \n");
  status = sl_si91x_dma_simple_transfer(0, channel, src, dst, 0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Invalid data size, test pass \n");

  UnityPrintf("Testing with unallocated channel \n");
  status = sl_si91x_dma_simple_transfer(0, channel, src, dst, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_DMA_CHANNEL_UNALLOCATED, status);
  UnityPrintf("Channel unallocated, test pass \n");

  UnityPrintf("Allocate channel 2 in UDMA0\n");
  status = sl_si91x_dma_allocate_channel(0, &channel, 0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("UDMA0 Channel 2 Allocated\n");
  UnityPrintf("Register transfer complete callback for channel 2 in UDMA0\n");
  callback_t.transfer_complete_cb = transfer_complete_callback_dmadrv;
  status                          = sl_si91x_dma_register_callbacks(0, channel, &callback_t);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Transfer complete callback registered\n");
  for (int i = 0; i < BUFFER_SIZE; i++) {
    src[i] = i;
  }
  UnityPrintf("Start transfer\n");
  status = sl_si91x_dma_simple_transfer(0, channel, src, dst, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  while (!transfer_complete)
    ;
  transfer_complete = 0;
  UnityPrintf("Transfer complete success\n");
  status = data_comparision(src, dst);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Data comparision success\n");

  UnityPrintf("Allocate channel 2 in UDMA1\n");
  status = sl_si91x_dma_allocate_channel(1, &channel, 0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("UDMA1 Channel 2 Allocated\n");
  UnityPrintf("Register transfer complete callback for channel 2 in UDMA1\n");
  callback_t.transfer_complete_cb = transfer_complete_callback_dmadrv;
  status                          = sl_si91x_dma_register_callbacks(1, channel, &callback_t);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  for (int i = 0; i < BUFFER_SIZE; i++) {
    dst[i] = 0;
  }
  UnityPrintf("Transfer complete callback registered\n");
  UnityPrintf("Start transfer\n");
  status = sl_si91x_dma_simple_transfer(1, channel, src, dst, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  while (!transfer_complete)
    ;
  transfer_complete = 0;
  UnityPrintf("Transfer complete success\n");
  status = data_comparision(src, dst);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Data comparision success\n");
  for (int i = 0; i < BUFFER_SIZE; i++) {
    dst[i] = 0;
  }

  UnityPrintf("Start transfers for channel2 and channel3\n");
  status = sl_si91x_dma_deallocate_channel(0, channel);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  status = sl_si91x_dma_allocate_channel(0, &channel, 0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  callback_t.transfer_complete_cb = transfer_complete_callback_simultaneous;
  status                          = sl_si91x_dma_register_callbacks(0, channel, &callback_t);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  //Allocate channel 3
  channel = 3;
  status  = sl_si91x_dma_allocate_channel(0, &channel, 1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  callback_t.transfer_complete_cb = transfer_complete_callback_simultaneous;
  status                          = sl_si91x_dma_register_callbacks(0, channel, &callback_t);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  status = sl_si91x_dma_simple_transfer(0, 3, src2, dst2, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  status = sl_si91x_dma_simple_transfer(0, 2, src, dst, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  while (transfer_complete != 5)
    ;
  UnityPrintf("\nXfer complete for channel 2 and 3 \n");
  transfer_complete = 0;
}

static void test_si91x_dma_transfer(void)
{
  sl_status_t status;
  uint32_t channel = 1;
  sl_dma_callback_t callback_t;
  sl_dma_xfer_t dma_xfer = { 0 };

  //Update source and destination address
  dma_xfer.src_addr  = src;
  dma_xfer.dest_addr = dst;
  //Update Source and destination data increment
  dma_xfer.src_inc = SL_TRANSFER_SRC_INC_32;
  dma_xfer.dst_inc = SL_TRANSFER_DST_INC_32;
  //Update the single data transfer size
  dma_xfer.xfer_size = SL_TRANSFER_SIZE_32;
  //Update total DMA transfer size
  dma_xfer.transfer_count = BUFFER_SIZE;
  //Update DMA transfer type and mode
  dma_xfer.dma_mode      = SL_DMA_BASIC_MODE;
  dma_xfer.transfer_type = SL_DMA_MEMORY_TO_MEMORY;

  UnityPrintf("\n");
  UnityPrintf("Testing DMA simple transfer \n");

  UnityPrintf("Testing Invalid DMA instance \n");
  status = sl_si91x_dma_transfer(2, channel, &dma_xfer);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Invalid DMA instance test pass \n");

  UnityPrintf("Testing Invalid UDMA0 invalid channel \n");
  channel = SL_DMA0_CHANNEL_COUNT + 1;
  status  = sl_si91x_dma_transfer(0, channel, &dma_xfer);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Invalid UDMA0 invalid channel, test pass \n");

  UnityPrintf("Testing Invalid UDMA1 invalid channel \n");
  channel = SL_ULP_DMA_CHANNEL_COUNT + 1;
  status  = sl_si91x_dma_transfer(1, channel, &dma_xfer);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Invalid UDMA1 invalid channel, test pass \n");

  UnityPrintf("Testing Invalid transfer count \n");
  channel                 = 5;
  dma_xfer.transfer_count = 0;
  status                  = sl_si91x_dma_transfer(0, channel, &dma_xfer);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Invalid transfer count, test pass \n");
  dma_xfer.transfer_count = BUFFER_SIZE;

  UnityPrintf("Testing Invalid src addr \n");
  channel           = 5;
  dma_xfer.src_addr = NULL;
  status            = sl_si91x_dma_transfer(0, channel, &dma_xfer);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Invalid src addr, test pass \n");
  dma_xfer.src_addr = src;

  UnityPrintf("Testing Invalid dst addr \n");
  channel            = 5;
  dma_xfer.dest_addr = NULL;
  status             = sl_si91x_dma_transfer(0, channel, &dma_xfer);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Invalid dst addr, test pass \n");
  dma_xfer.dest_addr = dst;

  UnityPrintf("Testing Invalid dst inc \n");
  channel          = 5;
  dma_xfer.dst_inc = 4;
  status           = sl_si91x_dma_transfer(0, channel, &dma_xfer);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Invalid dst inc, test pass \n");
  dma_xfer.dst_inc = SL_TRANSFER_DST_INC_32;

  UnityPrintf("Testing Invalid src inc \n");
  channel          = 5;
  dma_xfer.src_inc = 4;
  status           = sl_si91x_dma_transfer(0, channel, &dma_xfer);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Invalid src inc, test pass \n");
  dma_xfer.src_inc = SL_TRANSFER_SRC_INC_32;

  UnityPrintf("Allocate channel 5 of UDMA0 \n");
  status = sl_si91x_dma_allocate_channel(0, &channel, 0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("UDMA0 channel 5 allocated\n");

  UnityPrintf("Register transfer complete callback for channel 5 \n");
  callback_t.transfer_complete_cb = transfer_complete_callback_dmadrv;
  status                          = sl_si91x_dma_register_callbacks(0, channel, &callback_t);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("UDMA0 channel 5 calback registered\n");

  UnityPrintf("start transfer \n");
  status = sl_si91x_dma_transfer(0, channel, &dma_xfer);
  while (!transfer_complete)
    ;
  UnityPrintf("\n");
  transfer_complete = 0;

  UnityPrintf("Allocate channel 5 of UDMA1 \n");
  status = sl_si91x_dma_allocate_channel(1, &channel, 0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("UDMA1 channel 5 allocated\n");

  UnityPrintf("Register transfer complete callback for channel 5 \n");
  callback_t.transfer_complete_cb = transfer_complete_callback_dmadrv;
  status                          = sl_si91x_dma_register_callbacks(1, channel, &callback_t);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("UDMA1 channel 5 calback registered\n");

  UnityPrintf("start transfer \n");
  status = sl_si91x_dma_transfer(1, channel, &dma_xfer);
  while (!transfer_complete)
    ;
  UnityPrintf("\n");
  transfer_complete = 0;
}

static void test_si91x_dma_deinit(void)
{
  sl_status_t status;
  UnityPrintf("\n");
  UnityPrintf("Testing DMA de-init \n");

  UnityPrintf("Testing Invalid DMA instance \n");
  status = sl_si91x_dma_deinit(2);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Invalid DMA instance, test pass \n");

  UnityPrintf("Testing UDMA0 deinit \n");
  status = sl_si91x_dma_deinit(0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("UDMA0 deinit success \n");

  UnityPrintf("Testing UDMA1 deinit \n");
  status = sl_si91x_dma_deinit(1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("UDMA1 deinit success \n");
}

uint8_t data_comparision(uint32_t *src, uint32_t *dst)
{
  uint32_t status = 0;

  for (int i = 0; i < BUFFER_SIZE; i++) {
    if (src[i] != dst[i]) {
      status = 1;
      break;
    }
  }
  return status;
}
void transfer_complete_callback_dmadrv(uint32_t channel, void *data)
{
  UNUSED_PARAMETER(data);
  UNUSED_PARAMETER(channel);
  transfer_complete = 1;
  UnityPrintf("\r\ndma transfer complete");
}

void error_callback_dmadrv(uint32_t channel, void *data)
{
  UNUSED_PARAMETER(channel);
  UNUSED_PARAMETER(data);
}

void transfer_complete_callback_simultaneous(uint32_t channel, void *data)
{
  UNUSED_PARAMETER(data);
  switch (channel) {
    case 1:
      transfer_complete += 2;
      break;
    case 2:
      transfer_complete += 3;
      break;
    default:
      break;
  }
}
