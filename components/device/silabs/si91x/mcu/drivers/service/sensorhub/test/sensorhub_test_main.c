/***************************************************************************/
/**
 * @file sensor_hub_test_main.c
 * @brief Sensor Hub Unity Test Functions
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
#include "sensor_hub.h"
#include "sensors_config.h"
#include "rsi_debug.h"
#include "unity.h"

/*******************************************************************************
 ***************************  Global Variables   *******************************
 ******************************************************************************/
uint32_t test_event_ack = 0; // Sensor event acknowledge
/*******************************************************************************
 ************************  Static Function Prototypes   ************************
 ******************************************************************************/
void test_sensor_event_handler(uint8_t sensor_id, uint8_t event); // application event handler
/*******************************************************************************
 ***************************  Local Variables   *******************************
 ******************************************************************************/

sl_sensor_id_t test_sensor_scan_info[SL_MAX_NUM_SENSORS];
static uint32_t test_sensor_scan_cnt = 0; // Sensor scan count

/*******************************************************************************
 ************************  Test Function Prototypes  ****************************
 ******************************************************************************/
void test_sensor_hub_init(void);
void test_sensorhub_detect_sensors(void);
void test_sensorhub_create_sensor(void);
void test_sensorhub_delete_sensor(void);
void test_sensorhub_start_sensor(void);
void test_sensorhub_stop_sensor(void);
void test_sensorhub_notify_cb_register(void);
void test_sensor_hub_start(void);

/******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  UnityBeginGroup("Sensor Hub");

  RUN_TEST(test_sensorhub_notify_cb_register, __LINE__);
  RUN_TEST(test_sensor_hub_init, __LINE__);
  RUN_TEST(test_sensorhub_detect_sensors, __LINE__);
  RUN_TEST(test_sensorhub_create_sensor, __LINE__);
  RUN_TEST(test_sensor_hub_start, __LINE__);
  RUN_TEST(test_sensorhub_start_sensor, __LINE__);
  RUN_TEST(test_sensorhub_delete_sensor, __LINE__);
  RUN_TEST(test_sensorhub_stop_sensor, __LINE__);

  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

/*******************************************************************************
 * Function to test initialization of Sensor Hub
 ******************************************************************************/
void test_sensor_hub_init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sensor Hub Init \n");
  sl_status_t status;

  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_sensorhub_init();
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Sensor Hub Init successfully \n");

  UnityPrintf("Sensor Hub Init test completed \n");
}

/*******************************************************************************
 * Function to test detect sensor from Sensor Hub
 ******************************************************************************/
void test_sensorhub_detect_sensors(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sensor Hub detect sensor \n");
  // Sensor should interface otherwise controller will stuck here
  //  UnityPrintf("Testing with correct parameters \n");
  //  test_sensor_scan_cnt = sl_si91x_sensorhub_detect_sensors((sl_sensor_id_t *)&test_sensor_scan_info, SL_MAX_NUM_SENSORS);
  //  UnityPrintf("Status of API is correct, Sensor Hub detect sensor successfully \n");

  UnityPrintf("Sensor Hub detect sensor test completed \n");
}

/*******************************************************************************
 * Function to test create sensor to Sensor Hub
 ******************************************************************************/
void test_sensorhub_create_sensor(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sensor Hub create sensor  \n");
  sl_status_t status;

  UnityPrintf("Testing with correct parameters \n");
  for (uint32_t i = 0; i < test_sensor_scan_cnt; i++) {
    status = sl_si91x_sensorhub_create_sensor(test_sensor_scan_info[i]);
    TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  }
  UnityPrintf("Status of API is correct, Sensor Hub create sensor successfully \n");

  UnityPrintf("Sensor Hub create sensor test completed \n");
}

/*******************************************************************************
 * Function to test start Sensor Hub
 ******************************************************************************/
void test_sensor_hub_start(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sensor Hub start \n");
  sl_status_t status;

  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_sensor_hub_start();
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Sensor Hub start successfully \n");

  UnityPrintf("Sensor Hub start test completed \n");
}

/*******************************************************************************
 * Function to test register callback to Sensor Hub
 ******************************************************************************/
void test_sensorhub_notify_cb_register(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sensor Hub register callback \n");
  sl_status_t status;

  UnityPrintf("Testing with null parameters \n");
  status = sl_si91x_sensorhub_notify_cb_register(NULL, (sl_sensor_id_t *)&test_event_ack);
  TEST_ASSERT_EQUAL_HEX(SL_SH_INVALID_PARAMETERS, status);
  UnityPrintf("Testing with null parameters successfully\n");

  UnityPrintf("Testing with null parameters \n");
  status = sl_si91x_sensorhub_notify_cb_register((void *)test_sensor_event_handler, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_SH_INVALID_PARAMETERS, status);
  UnityPrintf("Testing with null parameters successfully\n");

  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_sensorhub_notify_cb_register((void *)test_sensor_event_handler, (sl_sensor_id_t *)&test_event_ack);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Sensor Hub register callback successfully \n");

  UnityPrintf("Sensor Hub register callback test completed \n");
}

/*******************************************************************************
 * Function to test start sensor to Sensor Hub
 ******************************************************************************/
void test_sensorhub_start_sensor(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sensor Hub start sensor  \n");
  sl_status_t status;

  UnityPrintf("Testing with correct parameters \n");
  for (uint32_t i = 0; i < test_sensor_scan_cnt; i++) {
    status = sl_si91x_sensorhub_start_sensor(test_sensor_scan_info[i]);
    TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  }
  UnityPrintf("Status of API is correct, Sensor Hub start sensor successfully \n");

  UnityPrintf("Sensor Hub start sensor test completed \n");
}

/*******************************************************************************
 * Function to test delete sensor to Sensor Hub
 ******************************************************************************/
void test_sensorhub_delete_sensor(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sensor Hub delete sensor  \n");
  sl_status_t status;

  UnityPrintf("Testing with correct parameters \n");
  for (uint32_t i = 0; i < test_sensor_scan_cnt; i++) {
    status = sl_si91x_sensorhub_delete_sensor(test_sensor_scan_info[i]);
    TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  }
  UnityPrintf("Status of API is correct, Sensor Hub em post event successfully \n");

  UnityPrintf("Sensor Hub delete sensor test completed \n");
}

/*******************************************************************************
 * Function to test stop sensor to Sensor Hub
 ******************************************************************************/
void test_sensorhub_stop_sensor(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sensor Hub stop sensor  \n");
  sl_status_t status;

  UnityPrintf("Testing with correct parameters \n");
  for (uint32_t i = 0; i < test_sensor_scan_cnt; i++) {
    status = sl_si91x_sensorhub_stop_sensor(test_sensor_scan_info[i]);
    TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  }
  UnityPrintf("Status of API is correct, Sensor Hub stop sensor successfully \n");

  UnityPrintf("Sensor Hub stop sensor test completed \n");
}

/**************************************************************************/ /**
 * @fn           void test_sensor_event_handler()
 * @brief        This Sensor event handle to the Sensor HUB.
 *
 * @param[in]    sensor_id        :Id of the Sensor
 * @param[in]    events           :Sensor hub event
 * @param[out]   NULL
 *
******************************************************************************/
void test_sensor_event_handler(uint8_t sensor_id, uint8_t event)
{
  (void)sensor_id;
  (void)event;
}
