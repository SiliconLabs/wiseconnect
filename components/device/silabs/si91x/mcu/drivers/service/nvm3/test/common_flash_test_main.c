/***************************************************************************/
/**
 * @file common_flash_intf_test_main.c
 * @brief Common Flash Interface Unity Test Functions
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
#include "nvm3_default.h"
#include "nvm3.h"
#include "nvm3_default_config.h"
#include "sl_si91x_common_flash_intf.h"
#include "nvm3_hal_flash.h"
#include "rsi_debug.h"
#include "unity.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define TEST_SECTOR_ADDRESS      0x00000000 // Example sector address
#define TEST_ADDRESS             0x00000000 // Example address
#define TEST_LENGTH              256        // Example length
#define TEST_AUTO_MODE           1          // Example auto mode
#define TEST_NVM3_DEFAULT_HANDLE nvm3_defaultHandle
// Maximum number of data objects saved
#define TEST_MAX_OBJECT_COUNT 10

// Max and min keys for data objects
#define TEST_MIN_DATA_KEY NVM3_KEY_MIN
#define TEST_MAX_DATA_KEY (TEST_MIN_DATA_KEY + TEST_MAX_OBJECT_COUNT - 1)

// Key of write counter object
#define TEST_WRITE_COUNTER_KEY TEST_MAX_OBJECT_COUNT

// Key of delete counter object
#define TEST_DELETE_COUNTER_KEY (TEST_WRITE_COUNTER_KEY + 1)

extern char linker_nvm_begin;
__attribute__((used)) uint8_t test_nvm3_default_storage[NVM3_DEFAULT_NVM_SIZE] __attribute__((section(".simee")));
#define NVM3_BASE (&linker_nvm_begin)

/*******************************************************************************
 ***************************  Global Variables   *******************************
 ******************************************************************************/

/*******************************************************************************
 ************************  Static Function Prototypes   ************************
 ******************************************************************************/

/*******************************************************************************
 ***************************  Local Variables   *******************************
 ******************************************************************************/
size_t len;
static char buffer[TEST_LENGTH];
nvm3_ObjectKey_t keys[TEST_MAX_OBJECT_COUNT];

#if (NVM3_DEFAULT_CACHE_SIZE != 0)
static nvm3_CacheEntry_t defaultCache[NVM3_DEFAULT_CACHE_SIZE];
#endif

nvm3_Init_t test_nvm3_defaultInitData = {
  (nvm3_HalPtr_t)NVM3_BASE,
  NVM3_DEFAULT_NVM_SIZE,
#if (NVM3_DEFAULT_CACHE_SIZE != 0)
  defaultCache,
#else
  NULL,
#endif
  NVM3_DEFAULT_CACHE_SIZE,
  NVM3_DEFAULT_MAX_OBJECT_SIZE,
  NVM3_DEFAULT_REPACK_HEADROOM,
  &nvm3_halFlashHandle,
#if defined(NVM3_SECURITY)
  &nvm3_halCryptoHandle,
  NVM3_DEFAULT_SECURITY_TYPE,
#endif
};

nvm3_Init_t *test_nvm3_defaultInit = &test_nvm3_defaultInitData;

/*******************************************************************************
 ************************  Test Function Prototypes  ****************************
 ******************************************************************************/
void test_nvm3_open(void);
void test_nvm3_close(void);
void test_nvm3_writeCounter(void);
void test_nvm3_writeData(void);
void test_nvm3_readData(void);
void test_readPartialData(void);
void test_nvm3_getObjectInfo(void);
void test_nvm3_enumObjects(void);
void test_nvm3_enumDeletedObjects(void);
void test_nvm3_deleteObject(void);
void test_nvm3_readCounter(void);
void test_nvm3_incrementCounter(void);
void test_nvm3_eraseAll(void);
void test_nvm3_getEraseCount(void);
void test_nvm3_setEraseCount(void);
void test_nvm3_repack(void);
void test_nvm3_repackNeeded(void);
void test_nvm3_resize(void);
void test_nvm3_countObjects(void);
void test_nvm3_countDeletedObjects(void);

/******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  UnityBeginGroup("Common Flash Interface");

  RUN_TEST(test_nvm3_open, __LINE__);
  RUN_TEST(test_nvm3_writeCounter, __LINE__);
  RUN_TEST(test_nvm3_writeData, __LINE__);
  RUN_TEST(test_nvm3_getObjectInfo, __LINE__);
  RUN_TEST(test_nvm3_readData, __LINE__);
  RUN_TEST(test_nvm3_readCounter, __LINE__);
  RUN_TEST(test_nvm3_enumDeletedObjects, __LINE__);
  RUN_TEST(test_nvm3_enumObjects, __LINE__);
  RUN_TEST(test_readPartialData, __LINE__);
  RUN_TEST(test_nvm3_deleteObject, __LINE__);
  RUN_TEST(test_nvm3_incrementCounter, __LINE__);
  RUN_TEST(test_nvm3_eraseAll, __LINE__);
  RUN_TEST(test_nvm3_setEraseCount, __LINE__);
  RUN_TEST(test_nvm3_getEraseCount, __LINE__);
  RUN_TEST(test_nvm3_repack, __LINE__);
  RUN_TEST(test_nvm3_repackNeeded, __LINE__);
  RUN_TEST(test_nvm3_resize, __LINE__);
  RUN_TEST(test_nvm3_countObjects, __LINE__);
  RUN_TEST(test_nvm3_countDeletedObjects, __LINE__);
  RUN_TEST(test_nvm3_close, __LINE__);

  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

/*******************************************************************************
 * Function to test initialization of NVM3
 ******************************************************************************/
void test_nvm3_open(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing NVM3 Init \n");
  sl_status_t status;

  UnityPrintf("Testing with null parameter \n");
  status = nvm3_open(NULL, test_nvm3_defaultInit);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with null parameter successfully \n");

  test_nvm3_defaultInit->nvmSize = NVM3_DEFAULT_NVM_SIZE + 1;
  UnityPrintf("Testing with invalid parameters \n");
  status = nvm3_open(TEST_NVM3_DEFAULT_HANDLE, test_nvm3_defaultInit);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NVM3_ALIGNMENT_INVALID, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  test_nvm3_defaultInit->cacheEntryCount = NVM3_DEFAULT_CACHE_SIZE + 1;
  UnityPrintf("Testing with invalid parameters \n");
  status = nvm3_open(TEST_NVM3_DEFAULT_HANDLE, test_nvm3_defaultInit);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NVM3_ALIGNMENT_INVALID, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  test_nvm3_defaultInit->maxObjectSize = NVM3_DEFAULT_MAX_OBJECT_SIZE + 1;
  UnityPrintf("Testing with invalid parameters \n");
  status = nvm3_open(TEST_NVM3_DEFAULT_HANDLE, test_nvm3_defaultInit);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NVM3_ALIGNMENT_INVALID, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  test_nvm3_defaultInit->repackHeadroom = 1;
  UnityPrintf("Testing with invalid parameters \n");
  status = nvm3_open(TEST_NVM3_DEFAULT_HANDLE, test_nvm3_defaultInit);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NVM3_ALIGNMENT_INVALID, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  //  test_nvm3_defaultInit->cacheEntryCount = NVM3_DEFAULT_CACHE_SIZE;
  //  test_nvm3_defaultInit->maxObjectSize = NVM3_DEFAULT_MAX_OBJECT_SIZE;
  //  test_nvm3_defaultInit->repackHeadroom = 0;
  //  UnityPrintf("Testing with correct parameters \n");
  //  status = nvm3_open(TEST_NVM3_DEFAULT_HANDLE, nvm3_defaultInit);
  //  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  //  UnityPrintf("Status of API is correct, NVM3 Init successfully \n");

  UnityPrintf("NVM3 Init test completed \n");
}

/*******************************************************************************
 * Function to test deinitialization of NVM3
 ******************************************************************************/
void test_nvm3_close(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing NVM3 deinit \n");
  sl_status_t status;

  UnityPrintf("Testing with null parameter \n");
  status = nvm3_close(NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with null parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 0;
  UnityPrintf("Testing with invalid parameters \n");
  status = nvm3_close(TEST_NVM3_DEFAULT_HANDLE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NOT_INITIALIZED, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 1;
  UnityPrintf("Testing with correct parameters \n");
  status = nvm3_close(TEST_NVM3_DEFAULT_HANDLE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, NVM3 deinit successfully \n");

  UnityPrintf("NVM3 deinit test completed \n");
}

/*******************************************************************************
 * Function to test writing to NVM3
 ******************************************************************************/
void test_nvm3_writeData(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing NVM3 Write \n");
  sl_status_t status;
  uint8_t write_data1[12] = { "Silicon labs" };
  nvm3_ObjectKey_t key    = 1;

  UnityPrintf("Testing with null parameter \n");
  status = nvm3_writeData(NULL, key, (unsigned char *)write_data1, 12);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with null parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 0;
  UnityPrintf("Testing with invalid parameter \n");
  status = nvm3_writeData(TEST_NVM3_DEFAULT_HANDLE, key, (unsigned char *)write_data1, 12);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NOT_INITIALIZED, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 1;
  UnityPrintf("Testing with invalid parameter \n");
  status = nvm3_writeData(TEST_NVM3_DEFAULT_HANDLE, NVM3_KEY_MASK + 1, (unsigned char *)write_data1, 12);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_KEY, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  //  UnityPrintf("Testing with correct parameters \n");
  //  status = nvm3_writeData(TEST_NVM3_DEFAULT_HANDLE, key, (unsigned char *)write_data1, 12);
  //  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  //  UnityPrintf("Status of API is correct, NVM3 Write successfully \n");

  UnityPrintf("NVM3 Write test completed \n");
}

/*******************************************************************************
 * Function to test reading from NVM3 flash
 ******************************************************************************/
void test_nvm3_readData(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing NVM3 Read \n");
  sl_status_t status;

  UnityPrintf("Testing with null parameter \n");
  status = nvm3_readData(NULL, 1, buffer, len);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with null parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 0;
  UnityPrintf("Testing with invalid parameter \n");
  status = nvm3_readData(TEST_NVM3_DEFAULT_HANDLE, 1, buffer, len);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NOT_INITIALIZED, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 1;
  UnityPrintf("Testing with invalid parameter \n");
  status = nvm3_readData(TEST_NVM3_DEFAULT_HANDLE, NVM3_KEY_MASK + 1, buffer, len);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_KEY, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  //  UnityPrintf("Testing with correct parameters \n");
  //  status = nvm3_readData(TEST_NVM3_DEFAULT_HANDLE, 1, buffer, len);
  //  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  //  if (SL_STATUS_OK == status) {
  //    buffer[len] = '\0';
  //    UnityPrintf("Read data from key 1:\r\n");
  //    UnityPrintf("%s\r\n", buffer);
  //  } else {
  //    UnityPrintf("Error reading data from key 1 \r\n");
  //  }
  //  UnityPrintf("Status of API is correct, NVM3 Read successfully \n");

  UnityPrintf("NVM3 Read test completed \n");
}

/*******************************************************************************
 * Function to test get object info from NVM3 flash
 ******************************************************************************/
void test_nvm3_getObjectInfo(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing NVM3 get object info \n");
  sl_status_t status;
  uint32_t type;

  UnityPrintf("Testing with null parameter \n");
  status = nvm3_getObjectInfo(NULL, 1, &type, &len);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with null parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 0;
  UnityPrintf("Testing with invalid parameter \n");
  status = nvm3_getObjectInfo(TEST_NVM3_DEFAULT_HANDLE, 1, &type, &len);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NOT_INITIALIZED, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 1;
  UnityPrintf("Testing with invalid parameter \n");
  status = nvm3_getObjectInfo(TEST_NVM3_DEFAULT_HANDLE, NVM3_KEY_MASK + 1, &type, &len);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_KEY, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  //  UnityPrintf("Testing with correct parameters \n");
  //  status = nvm3_getObjectInfo(TEST_NVM3_DEFAULT_HANDLE, 1, &type, &len);
  //  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  //  UnityPrintf("Status of API is correct, NVM3 Read successfully \n");
  //
  //  UnityPrintf("NVM3 get object info test completed \n");
}

/*******************************************************************************
 * Function to test write counter.
 ******************************************************************************/
void test_nvm3_writeCounter(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing NVM3 write counter \n");
  sl_status_t status;
  uint32_t type;

  UnityPrintf("Testing with null parameter \n");
  status = nvm3_writeCounter(NULL, 1, 0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with null parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 0;
  UnityPrintf("Testing with invalid parameter \n");
  status = nvm3_writeCounter(TEST_NVM3_DEFAULT_HANDLE, 1, 0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NOT_INITIALIZED, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 1;
  UnityPrintf("Testing with invalid parameter \n");
  status = nvm3_writeCounter(TEST_NVM3_DEFAULT_HANDLE, NVM3_KEY_MASK + 1, 0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_KEY, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  status = nvm3_getObjectInfo(TEST_NVM3_DEFAULT_HANDLE, TEST_WRITE_COUNTER_KEY, &type, &len);
  if ((status != SL_STATUS_OK) || (type != NVM3_OBJECTTYPE_COUNTER)) {
    nvm3_writeCounter(TEST_NVM3_DEFAULT_HANDLE, TEST_WRITE_COUNTER_KEY, 0);
  }
  UnityPrintf("Status of API is correct, NVM3 write counter for write counter key successfully \n");

  status = nvm3_getObjectInfo(TEST_NVM3_DEFAULT_HANDLE, TEST_DELETE_COUNTER_KEY, &type, &len);
  if ((status != SL_STATUS_OK) || (type != NVM3_OBJECTTYPE_COUNTER)) {
    nvm3_writeCounter(TEST_NVM3_DEFAULT_HANDLE, TEST_DELETE_COUNTER_KEY, 0);
  }
  UnityPrintf("Status of API is correct, NVM3 write counter for delete counter key successfully \n");

  UnityPrintf("NVM3 write counter test completed \n");
}

/*******************************************************************************
 * Function to test get enum deleted objects.
 ******************************************************************************/
void test_nvm3_enumDeletedObjects(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing NVM3 get enum deleted objects \n");
  size_t objects_count, i;

  objects_count = nvm3_enumDeletedObjects(TEST_NVM3_DEFAULT_HANDLE,
                                          (uint32_t *)keys,
                                          sizeof(keys) / sizeof(keys[0]),
                                          TEST_MIN_DATA_KEY,
                                          TEST_MAX_DATA_KEY);
  TEST_ASSERT_EQUAL(0, objects_count);
  UnityPrintf("Status of API is correct, NVM3 get enum deleted objects successfully \n");

  if (objects_count == 0) {
    UnityPrintf("No deleted objects found\r\n");
  } else {
    UnityPrintf("Keys of objects deleted from NVM3:\r\n");
    for (i = 0; i < objects_count; i++) {
      UnityPrintf("> %lu\r\n", keys[i]);
    }
  }

  UnityPrintf("NVM3 get enum deleted objects test completed \n");
}

/*******************************************************************************
 * Function to test get enum objects.
 ******************************************************************************/
void test_nvm3_enumObjects(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing NVM3 get enum objects \n");
  sl_status_t status;
  uint32_t type;
  size_t objects_count, i;

  objects_count = nvm3_enumObjects(TEST_NVM3_DEFAULT_HANDLE,
                                   (uint32_t *)keys,
                                   sizeof(keys) / sizeof(keys[0]),
                                   TEST_MIN_DATA_KEY,
                                   TEST_MAX_DATA_KEY);
  TEST_ASSERT_EQUAL(0, objects_count);
  UnityPrintf("Status of API is correct, NVM3 get enum objects successfully \n");

  if (objects_count == 0) {
    printf("No stored objects found\r\n");
  } else {
    printf("Keys and contents of objects stored in NVM3:\r\n");
    for (i = 0; i < objects_count; i++) {
      status = nvm3_getObjectInfo(TEST_NVM3_DEFAULT_HANDLE, keys[i], &type, &len);
      if (type == NVM3_OBJECTTYPE_DATA) {
        status = nvm3_readData(TEST_NVM3_DEFAULT_HANDLE, keys[i], buffer, len);
        TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
        buffer[len] = '\0';
        UnityPrintf("> %lu: %s\r\n", keys[i], buffer);
      }
    }
  }

  UnityPrintf("NVM3 get enum objects test completed \n");
}

/*******************************************************************************
 * Function to test read counter.
 ******************************************************************************/
void test_nvm3_readCounter(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing NVM3 read counter \n");
  sl_status_t status;
  uint32_t counter = 0;

  UnityPrintf("Testing with null parameter \n");
  status = nvm3_readCounter(NULL, TEST_DELETE_COUNTER_KEY, &counter);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with null parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 0;
  UnityPrintf("Testing with invalid parameter \n");
  status = nvm3_readCounter(TEST_NVM3_DEFAULT_HANDLE, TEST_DELETE_COUNTER_KEY, &counter);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NOT_INITIALIZED, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 1;
  UnityPrintf("Testing with invalid parameter \n");
  status = nvm3_readCounter(TEST_NVM3_DEFAULT_HANDLE, NVM3_KEY_MASK + 1, &counter);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_KEY, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  //  UnityPrintf("Testing with correct parameters \n");
  //  status = nvm3_readCounter(TEST_NVM3_DEFAULT_HANDLE, TEST_DELETE_COUNTER_KEY, &counter);
  //  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  //  UnityPrintf("Status of API is correct, NVM3 read counter successfully \n");

  //  UnityPrintf("Testing with correct parameters \n");
  //  status = nvm3_readCounter(TEST_NVM3_DEFAULT_HANDLE, TEST_WRITE_COUNTER_KEY, &counter);
  //  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  //  UnityPrintf("Status of API is correct, NVM3 read counter successfully \n");

  UnityPrintf("NVM3 read counter test completed \n");
}

/*******************************************************************************
 * Function to test read partial data.
 ******************************************************************************/
void test_readPartialData(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing NVM3 read partial data \n");
  sl_status_t status;
  uint32_t counter = 0;
  size_t ofs       = 0;

  UnityPrintf("Testing with null parameter \n");
  status = nvm3_readPartialData(NULL, 1, &counter, ofs, len);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with null parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 0;
  UnityPrintf("Testing with invalid parameter \n");
  status = nvm3_readPartialData(TEST_NVM3_DEFAULT_HANDLE, 1, &counter, ofs, len);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NOT_INITIALIZED, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 1;
  UnityPrintf("Testing with invalid parameter \n");
  status = nvm3_readPartialData(TEST_NVM3_DEFAULT_HANDLE, NVM3_KEY_MASK + 1, &counter, ofs, len);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_KEY, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  //  UnityPrintf("Testing with correct parameters \n");
  //  status = nvm3_readPartialData(TEST_NVM3_DEFAULT_HANDLE, 1, &counter, ofs, len);
  //  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  //  UnityPrintf("Status of API is correct, NVM3 read partial data successfully \n");

  UnityPrintf("NVM3 read partial data test completed \n");
}

/*******************************************************************************
 * Function to test delete object.
 ******************************************************************************/
void test_nvm3_deleteObject(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing NVM3 delete object \n");
  sl_status_t status;

  UnityPrintf("Testing with null parameter \n");
  status = nvm3_deleteObject(NULL, 1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with null parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 0;
  UnityPrintf("Testing with invalid parameter \n");
  status = nvm3_deleteObject(TEST_NVM3_DEFAULT_HANDLE, 1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NOT_INITIALIZED, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 1;
  UnityPrintf("Testing with invalid parameter \n");
  status = nvm3_deleteObject(TEST_NVM3_DEFAULT_HANDLE, NVM3_KEY_MASK + 1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_KEY, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  //  UnityPrintf("Testing with correct parameters \n");
  //  status = nvm3_deleteObject(TEST_NVM3_DEFAULT_HANDLE, 1);
  //  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  //  UnityPrintf("Status of API is correct, NVM3 delete object successfully \n");

  UnityPrintf("NVM3 delete object test completed \n");
}

/*******************************************************************************
 * Function to test increment the counter.
 ******************************************************************************/
void test_nvm3_incrementCounter(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing NVM3 increment counter \n");
  sl_status_t status;
  uint32_t count = 0;

  UnityPrintf("Testing with null parameter \n");
  status = nvm3_incrementCounter(NULL, 1, &count);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with null parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 0;
  UnityPrintf("Testing with invalid parameter \n");
  status = nvm3_incrementCounter(TEST_NVM3_DEFAULT_HANDLE, 1, &count);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NOT_INITIALIZED, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 1;
  UnityPrintf("Testing with invalid parameter \n");
  status = nvm3_incrementCounter(TEST_NVM3_DEFAULT_HANDLE, NVM3_KEY_MASK + 1, &count);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_KEY, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  //  UnityPrintf("Testing with correct parameters \n");
  //  status = nvm3_incrementCounter(TEST_NVM3_DEFAULT_HANDLE, TEST_DELETE_COUNTER_KEY, &count);
  //  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  //  UnityPrintf("Status of API is correct, NVM3 increment counter successfully \n");

  UnityPrintf("NVM3 increment counter test completed \n");
}

/*******************************************************************************
 * Function to test erase all data.
 ******************************************************************************/
void test_nvm3_eraseAll(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing NVM3 erase all data \n");
  sl_status_t status;

  UnityPrintf("Testing with null parameter \n");
  status = nvm3_eraseAll(NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with null parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 0;
  UnityPrintf("Testing with invalid parameter \n");
  status = nvm3_eraseAll(TEST_NVM3_DEFAULT_HANDLE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NOT_INITIALIZED, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 1;
  //  UnityPrintf("Testing with correct parameters \n");
  //  status = nvm3_eraseAll(TEST_NVM3_DEFAULT_HANDLE);
  //  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  //  UnityPrintf("Status of API is correct, NVM3 erase all data successfully \n");

  UnityPrintf("NVM3 erase all data test completed \n");
}

/*******************************************************************************
 * Function to test set erase count.
 ******************************************************************************/
void test_nvm3_setEraseCount(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing NVM3 set erase count \n");
  uint32_t set_count = 0;

  nvm3_setEraseCount(set_count);
  UnityPrintf("Status of API is correct, NVM3 set erase count successfully \n");

  UnityPrintf("NVM3 set erase count test completed \n");
}

/*******************************************************************************
 * Function to test get erase count.
 ******************************************************************************/
void test_nvm3_getEraseCount(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing NVM3 get erase count \n");
  uint32_t get_count = 0;
  sl_status_t status;

  UnityPrintf("Testing with null parameter \n");
  status = nvm3_getEraseCount(NULL, &get_count);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with null parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 0;
  UnityPrintf("Testing with invalid parameter \n");
  status = nvm3_getEraseCount(TEST_NVM3_DEFAULT_HANDLE, &get_count);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NOT_INITIALIZED, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 1;
  status                                  = nvm3_getEraseCount(TEST_NVM3_DEFAULT_HANDLE, &get_count);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, NVM3 get erase count successfully \n");

  UnityPrintf("NVM3 get erase count test completed \n");
}

/*******************************************************************************
 * Function to test repack NVM3.
 ******************************************************************************/
void test_nvm3_repack(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing NVM3 repack \n");
  sl_status_t status;

  UnityPrintf("Testing with null parameter \n");
  status = nvm3_repack(NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with null parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 0;
  UnityPrintf("Testing with invalid parameter \n");
  status = nvm3_repack(TEST_NVM3_DEFAULT_HANDLE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NOT_INITIALIZED, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 1;
  status                                  = nvm3_repack(TEST_NVM3_DEFAULT_HANDLE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, NVM3 repack successfully \n");

  UnityPrintf("NVM3 repack test completed \n");
}

/*******************************************************************************
 * Function to test repack needed NVM3.
 ******************************************************************************/
void test_nvm3_repackNeeded(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing NVM3 repack needed \n");
  bool status;

  UnityPrintf("Testing with null parameter \n");
  status = nvm3_repackNeeded(NULL);
  TEST_ASSERT_EQUAL_HEX(false, status);
  UnityPrintf("Testing with null parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 0;
  UnityPrintf("Testing with invalid parameter \n");
  status = nvm3_repackNeeded(TEST_NVM3_DEFAULT_HANDLE);
  TEST_ASSERT_EQUAL_HEX(false, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 1;
  status                                  = nvm3_repackNeeded(TEST_NVM3_DEFAULT_HANDLE);
  TEST_ASSERT_EQUAL_HEX(false, status);
  UnityPrintf("Status of API is correct, NVM3 repack needed successfully \n");

  UnityPrintf("NVM3 repack needed test completed \n");
}

/*******************************************************************************
 * Function to test resize NVM3.
 ******************************************************************************/
void test_nvm3_resize(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing NVM3 resize \n");
  sl_status_t status;
  nvm3_HalPtr_t newaddr = (int *)4096;
  size_t new_size       = 4096;

  UnityPrintf("Testing with null parameter \n");
  status = nvm3_resize(NULL, newaddr, new_size);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with null parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 0;
  UnityPrintf("Testing with invalid parameter \n");
  status = nvm3_resize(TEST_NVM3_DEFAULT_HANDLE, newaddr, new_size);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NOT_INITIALIZED, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  TEST_NVM3_DEFAULT_HANDLE->hasBeenOpened = 1;
  UnityPrintf("Testing with invalid parameter \n");
  status = nvm3_resize(TEST_NVM3_DEFAULT_HANDLE, newaddr + 1, new_size);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NVM3_ALIGNMENT_INVALID, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  UnityPrintf("Testing with invalid parameter \n");
  status = nvm3_resize(TEST_NVM3_DEFAULT_HANDLE, newaddr, new_size + 1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NVM3_ALIGNMENT_INVALID, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  //  newaddr = (int*)0;
  //  new_size = 0;
  //  UnityPrintf("Testing with correct parameters \n");
  //  status = nvm3_resize(TEST_NVM3_DEFAULT_HANDLE, newaddr, new_size);
  //  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  //  UnityPrintf("Status of API is correct, NVM3 resize successfully \n");

  UnityPrintf("NVM3 resize test completed \n");
}

/*******************************************************************************
 * Function to test count objects NVM3.
 ******************************************************************************/
void test_nvm3_countObjects(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing NVM3 count objects \n");
  size_t count;

  count = nvm3_countObjects(TEST_NVM3_DEFAULT_HANDLE);
  UnityPrintf("Deleted object count = %ld \n", count);
  UnityPrintf("Status of API is correct, NVM3 count objects successfully \n");

  UnityPrintf("NVM3 count objects test completed \n");
}

/*******************************************************************************
 * Function to test count deleted objects NVM3.
 ******************************************************************************/
void test_nvm3_countDeletedObjects(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing NVM3 count deleted objects \n");
  size_t count;

  count = nvm3_countDeletedObjects(TEST_NVM3_DEFAULT_HANDLE);
  UnityPrintf("Deleted object count = %ld \n", count);
  UnityPrintf("Status of API is correct, NVM3 count deleted objects successfully \n");

  UnityPrintf("NVM3 count deleted objects test completed \n");
}
