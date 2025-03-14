/***************************************************************************/
/**
 * @file sl_ulp_dac_example.c
 * @brief ULP DAC example
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_ulp_dac_example.h"
#include "rsi_debug.h"
#include "sl_si91x_dac.h"
#include "rsi_debug.h"

#include "sl_si91x_dac_config.h"
#include "sl_si91x_power_manager.h"
#include "sl_si91x_wireless_shutdown.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define STATIC_MODE 1 // For static mode set this macro value as '1'
/* Core clock modification Macros */
#define PS4_SOC_FREQ          180000000 /*<! PLL out clock 180MHz            */
#define SOC_PLL_REF_FREQUENCY 40000000  /*<! PLL input REFERENCE clock 40MHZ */
#define DIVISION_FACTOR       0         // Division factor
#if (STATIC_MODE != 1)
#define BUFFER_SIZE 1024 // For FIFO mode, buffer size can configure.
#else
#define BUFFER_SIZE 1 // For static mode.
#endif
#define VREF_VALUE        3.3f // Reference voltage
#define MS_DELAY_COUNTER  4600 // Delay count
#define FIVE_SECOND_DELAY 5000 // giving the 5 second delay in between state changes

/*******************************************************************************
 ******************************  Data Types  ***********************************
 ******************************************************************************/
// Enum for different transmission scenarios
typedef enum {
  ULP_DAC_PROCESS_ACTION,            // process action mode
  SL_ULP_DAC_POWER_STATE_TRANSITION, // dac power state transitioning mode
  ULP_DAC_TRANSMISSION_COMPLETED,    // Transmission completed mode
} ulp_dac_enum_t;

static ulp_dac_enum_t ulp_dac_current_mode  = ULP_DAC_PROCESS_ACTION;
static sl_power_state_t current_power_state = SL_SI91X_POWER_MANAGER_PS4;

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
static float vref_value = (float)VREF_VALUE;
#if (STATIC_MODE == 1)
static int16_t dac_input_sample_data[1] = { 0x3FF };
#else
static int16_t dac_input_sample_data[BUFFER_SIZE] = {
  0x200, 0x209, 0x213, 0x21C, 0x226, 0x22F, 0x238, 0x242, 0x24B, 0x254, 0x25E, 0x267, 0x270, 0x279, 0x282, 0x28B, 0x294,
  0x29D, 0x2A6, 0x2AF, 0x2B8, 0x2C1, 0x2C9, 0x2D2, 0x2DA, 0x2E3, 0x2EB, 0x2F4, 0x2FC, 0x304, 0x30C, 0x314, 0x31C, 0x324,
  0x32B, 0x333, 0x33A, 0x342, 0x349, 0x350, 0x357, 0x35E, 0x365, 0x36C, 0x372, 0x379, 0x37F, 0x385, 0x38B, 0x391, 0x397,
  0x39C, 0x3A2, 0x3A7, 0x3AC, 0x3B1, 0x3B6, 0x3BB, 0x3C0, 0x3C4, 0x3C8, 0x3CD, 0x3D1, 0x3D4, 0x3D8, 0x3DC, 0x3DF, 0x3E2,
  0x3E5, 0x3E8, 0x3EB, 0x3ED, 0x3F0, 0x3F2, 0x3F4, 0x3F6, 0x3F7, 0x3F9, 0x3FA, 0x3FC, 0x3FD, 0x3FD, 0x3FE, 0x3FF, 0x3FF,
  0x3FF, 0x3FF, 0x3FF, 0x3FE, 0x3FE, 0x3FD, 0x3FC, 0x3FB, 0x3FA, 0x3F9, 0x3F7, 0x3F5, 0x3F3, 0x3F1, 0x3EF, 0x3EC, 0x3EA,
  0x3E7, 0x3E4, 0x3E1, 0x3DE, 0x3DA, 0x3D7, 0x3D3, 0x3CF, 0x3CB, 0x3C7, 0x3C3, 0x3BE, 0x3B9, 0x3B5, 0x3B0, 0x3AB, 0x3A5,
  0x3A0, 0x39A, 0x395, 0x38F, 0x389, 0x383, 0x37D, 0x376, 0x370, 0x369, 0x363, 0x35C, 0x355, 0x34E, 0x347, 0x33F, 0x338,
  0x330, 0x329, 0x321, 0x319, 0x311, 0x309, 0x301, 0x2F9, 0x2F1, 0x2E9, 0x2E0, 0x2D8, 0x2CF, 0x2C6, 0x2BE, 0x2B5, 0x2AC,
  0x2A3, 0x29A, 0x291, 0x288, 0x27F, 0x276, 0x26D, 0x264, 0x25A, 0x251, 0x248, 0x23F, 0x235, 0x22C, 0x222, 0x219, 0x210,
  0x206, 0x1FD, 0x1F3, 0x1EA, 0x1E1, 0x1D7, 0x1CE, 0x1C5, 0x1BB, 0x1B2, 0x1A9, 0x19F, 0x196, 0x18D, 0x184, 0x17B, 0x172,
  0x169, 0x160, 0x157, 0x14E, 0x145, 0x13C, 0x134, 0x12B, 0x123, 0x11A, 0x112, 0x10A, 0x101, 0x0F9, 0x0F1, 0x0E9, 0x0E2,
  0x0DA, 0x0D2, 0x0CB, 0x0C3, 0x0BC, 0x0B5, 0x0AE, 0x0A7, 0x0A0, 0x099, 0x092, 0x08C, 0x085, 0x07F, 0x079, 0x073, 0x06D,
  0x067, 0x062, 0x05C, 0x057, 0x052, 0x04D, 0x048, 0x043, 0x03F, 0x03A, 0x036, 0x032, 0x02E, 0x02A, 0x027, 0x023, 0x020,
  0x01D, 0x01A, 0x017, 0x014, 0x012, 0x010, 0x00D, 0x00B, 0x00A, 0x008, 0x007, 0x005, 0x004, 0x003, 0x002, 0x002, 0x001,
  0x001, 0x001, 0x001, 0x001, 0x002, 0x002, 0x003, 0x004, 0x005, 0x007, 0x008, 0x00A, 0x00B, 0x00D, 0x010, 0x012, 0x014,
  0x017, 0x01A, 0x01D, 0x020, 0x023, 0x027, 0x02A, 0x02E, 0x032, 0x036, 0x03A, 0x03F, 0x043, 0x048, 0x04D, 0x052, 0x057,
  0x05C, 0x062, 0x067, 0x06D, 0x073, 0x079, 0x07F, 0x085, 0x08C, 0x092, 0x099, 0x0A0, 0x0A7, 0x0AE, 0x0B5, 0x0BC, 0x0C3,
  0x0CB, 0x0D2, 0x0DA, 0x0E2, 0x0E9, 0x0F1, 0x0F9, 0x101, 0x10A, 0x112, 0x11A, 0x123, 0x12B, 0x134, 0x13C, 0x145, 0x14E,
  0x157, 0x160, 0x169, 0x172, 0x17B, 0x184, 0x18D, 0x196, 0x19F, 0x1A9, 0x1B2, 0x1BB, 0x1C5, 0x1CE, 0x1D7, 0x1E1, 0x1EA,
  0x1F3, 0x1FD, 0x206, 0x210, 0x219, 0x222, 0x22C, 0x235, 0x23F, 0x248, 0x251, 0x25A, 0x264, 0x26D, 0x276, 0x27F, 0x288,
  0x291, 0x29A, 0x2A3, 0x2AC, 0x2B5, 0x2BE, 0x2C6, 0x2CF, 0x2D8, 0x2E0, 0x2E9, 0x2F1, 0x2F9, 0x301, 0x309, 0x311, 0x319,
  0x321, 0x329, 0x330, 0x338, 0x33F, 0x347, 0x34E, 0x355, 0x35C, 0x363, 0x369, 0x370, 0x376, 0x37D, 0x383, 0x389, 0x38F,
  0x395, 0x39A, 0x3A0, 0x3A5, 0x3AB, 0x3B0, 0x3B5, 0x3B9, 0x3BE, 0x3C3, 0x3C7, 0x3CB, 0x3CF, 0x3D3, 0x3D7, 0x3DA, 0x3DE,
  0x3E1, 0x3E4, 0x3E7, 0x3EA, 0x3EC, 0x3EF, 0x3F1, 0x3F3, 0x3F5, 0x3F7, 0x3F9, 0x3FA, 0x3FB, 0x3FC, 0x3FD, 0x3FE, 0x3FE,
  0x3FF, 0x3FF, 0x3FF, 0x3FF, 0x3FF, 0x3FE, 0x3FD, 0x3FD, 0x3FC, 0x3FA, 0x3F9, 0x3F7, 0x3F6, 0x3F4, 0x3F2, 0x3F0, 0x3ED,
  0x3EB, 0x3E8, 0x3E5, 0x3E2, 0x3DF, 0x3DC, 0x3D8, 0x3D4, 0x3D1, 0x3CD, 0x3C8, 0x3C4, 0x3C0, 0x3BB, 0x3B6, 0x3B1, 0x3AC,
  0x3A7, 0x3A2, 0x39C, 0x397, 0x391, 0x38B, 0x385, 0x37F, 0x379, 0x372, 0x36C, 0x365, 0x35E, 0x357, 0x350, 0x349, 0x342,
  0x33A, 0x333, 0x32B, 0x324, 0x31C, 0x314, 0x30C, 0x304, 0x2FC, 0x2F4, 0x2EB, 0x2E3, 0x2DA, 0x2D2, 0x2C9, 0x2C1, 0x2B8,
  0x2AF, 0x2A6, 0x29D, 0x294, 0x28B, 0x282, 0x279, 0x270, 0x267, 0x25E, 0x254, 0x24B, 0x242, 0x238, 0x22F, 0x226, 0x21C,
  0x213, 0x209, 0x200, 0x1F7, 0x1ED, 0x1E4, 0x1DA, 0x1D1, 0x1C8, 0x1BE, 0x1B5, 0x1AC, 0x1A2, 0x199, 0x190, 0x187, 0x17E,
  0x175, 0x16C, 0x163, 0x15A, 0x151, 0x148, 0x13F, 0x137, 0x12E, 0x126, 0x11D, 0x115, 0x10C, 0x104, 0x0FC, 0x0F4, 0x0EC,
  0x0E4, 0x0DC, 0x0D5, 0x0CD, 0x0C6, 0x0BE, 0x0B7, 0x0B0, 0x0A9, 0x0A2, 0x09B, 0x094, 0x08E, 0x087, 0x081, 0x07B, 0x075,
  0x06F, 0x069, 0x064, 0x05E, 0x059, 0x054, 0x04F, 0x04A, 0x045, 0x040, 0x03C, 0x038, 0x033, 0x02F, 0x02C, 0x028, 0x024,
  0x021, 0x01E, 0x01B, 0x018, 0x015, 0x013, 0x010, 0x00E, 0x00C, 0x00A, 0x009, 0x007, 0x006, 0x004, 0x003, 0x003, 0x002,
  0x001, 0x001, 0x001, 0x001, 0x001, 0x002, 0x002, 0x003, 0x004, 0x005, 0x006, 0x007, 0x009, 0x00B, 0x00D, 0x00F, 0x011,
  0x014, 0x016, 0x019, 0x01C, 0x01F, 0x022, 0x026, 0x029, 0x02D, 0x031, 0x035, 0x039, 0x03D, 0x042, 0x047, 0x04B, 0x050,
  0x055, 0x05B, 0x060, 0x066, 0x06B, 0x071, 0x077, 0x07D, 0x083, 0x08A, 0x090, 0x097, 0x09D, 0x0A4, 0x0AB, 0x0B2, 0x0B9,
  0x0C1, 0x0C8, 0x0D0, 0x0D7, 0x0DF, 0x0E7, 0x0EF, 0x0F7, 0x0FF, 0x107, 0x10F, 0x117, 0x120, 0x128, 0x131, 0x13A, 0x142,
  0x14B, 0x154, 0x15D, 0x166, 0x16F, 0x178, 0x181, 0x18A, 0x193, 0x19C, 0x1A6, 0x1AF, 0x1B8, 0x1C1, 0x1CB, 0x1D4, 0x1DE,
  0x1E7, 0x1F0, 0x1FA, 0x203, 0x20D, 0x216, 0x21F, 0x229, 0x232, 0x23B, 0x245, 0x24E, 0x257, 0x261, 0x26A, 0x273, 0x27C,
  0x285, 0x28E, 0x297, 0x2A0, 0x2A9, 0x2B2, 0x2BB, 0x2C4, 0x2CC, 0x2D5, 0x2DD, 0x2E6, 0x2EE, 0x2F6, 0x2FF, 0x307, 0x30F,
  0x317, 0x31E, 0x326, 0x32E, 0x335, 0x33D, 0x344, 0x34B, 0x352, 0x359, 0x360, 0x367, 0x36E, 0x374, 0x37B, 0x381, 0x387,
  0x38D, 0x393, 0x399, 0x39E, 0x3A4, 0x3A9, 0x3AE, 0x3B3, 0x3B8, 0x3BD, 0x3C1, 0x3C6, 0x3CA, 0x3CE, 0x3D2, 0x3D6, 0x3D9,
  0x3DD, 0x3E0, 0x3E3, 0x3E6, 0x3E9, 0x3EC, 0x3EE, 0x3F0, 0x3F3, 0x3F5, 0x3F6, 0x3F8, 0x3F9, 0x3FB, 0x3FC, 0x3FD, 0x3FE,
  0x3FE, 0x3FF, 0x3FF, 0x3FF, 0x3FF, 0x3FF, 0x3FE, 0x3FE, 0x3FD, 0x3FC, 0x3FB, 0x3F9, 0x3F8, 0x3F6, 0x3F5, 0x3F3, 0x3F0,
  0x3EE, 0x3EC, 0x3E9, 0x3E6, 0x3E3, 0x3E0, 0x3DD, 0x3D9, 0x3D6, 0x3D2, 0x3CE, 0x3CA, 0x3C6, 0x3C1, 0x3BD, 0x3B8, 0x3B3,
  0x3AE, 0x3A9, 0x3A4, 0x39E, 0x399, 0x393, 0x38D, 0x387, 0x381, 0x37B, 0x374, 0x36E, 0x367, 0x360, 0x359, 0x352, 0x34B,
  0x344, 0x33D, 0x335, 0x32E, 0x326, 0x31E, 0x317, 0x30F, 0x307, 0x2FF, 0x2F6, 0x2EE, 0x2E6, 0x2DD, 0x2D5, 0x2CC, 0x2C4,
  0x2BB, 0x2B2, 0x2A9, 0x2A0, 0x297, 0x28E, 0x285, 0x27C, 0x273, 0x26A, 0x261, 0x257, 0x24E, 0x245, 0x23B, 0x232, 0x229,
  0x21F, 0x216, 0x20D, 0x203, 0x1FA, 0x1F0, 0x1E7, 0x1DE, 0x1D4, 0x1CB, 0x1C1, 0x1B8, 0x1AF, 0x1A6, 0x19C, 0x193, 0x18A,
  0x181, 0x178, 0x16F, 0x166, 0x15D, 0x154, 0x14B, 0x142, 0x13A, 0x131, 0x128, 0x120, 0x117, 0x10F, 0x107, 0x0FF, 0x0F7,
  0x0EF, 0x0E7, 0x0DF, 0x0D7, 0x0D0, 0x0C8, 0x0C1, 0x0B9, 0x0B2, 0x0AB, 0x0A4, 0x09D, 0x097, 0x090, 0x08A, 0x083, 0x07D,
  0x077, 0x071, 0x06B, 0x066, 0x060, 0x05B, 0x055, 0x050, 0x04B, 0x047, 0x042, 0x03D, 0x039, 0x035, 0x031, 0x02D, 0x029,
  0x026, 0x022, 0x01F, 0x01C, 0x019, 0x016, 0x014, 0x011, 0x00F, 0x00D, 0x00B, 0x009, 0x007, 0x006, 0x005, 0x004, 0x003,
  0x002, 0x002, 0x001, 0x001, 0x001, 0x001, 0x001, 0x002, 0x003, 0x003, 0x004, 0x006, 0x007, 0x009, 0x00A, 0x00C, 0x00E,
  0x010, 0x013, 0x015, 0x018, 0x01B, 0x01E, 0x021, 0x024, 0x028, 0x02C, 0x02F, 0x033, 0x038, 0x03C, 0x040, 0x045, 0x04A,
  0x04F, 0x054, 0x059, 0x05E, 0x064, 0x069, 0x06F, 0x075, 0x07B, 0x081, 0x087, 0x08E, 0x094, 0x09B, 0x0A2, 0x0A9, 0x0B0,
  0x0B7, 0x0BE, 0x0C6, 0x0CD, 0x0D5, 0x0DC, 0x0E4, 0x0EC, 0x0F4, 0x0FC, 0x104, 0x10C, 0x115, 0x11D, 0x126, 0x12E, 0x137,
  0x13F, 0x148, 0x151, 0x15A, 0x163, 0x16C, 0x175, 0x17E, 0x187, 0x190, 0x199, 0x1A2, 0x1AC, 0x1B5, 0x1BE, 0x1C8, 0x1D1,
  0x1DA, 0x1E4, 0x1ED, 0x1F7
};
#endif
static boolean_t dac_static_intr_flag = false;
static boolean_t dac_fifo_intr_flag   = false;
sl_dac_clock_config_t dac_clock_config;
sl_status_t status;
// Define a flag to track the power state transition
static bool dac_ps4_to_ps2_transition_done = false;
static bool dac_ps2_to_ps4_transition_done = false;
/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void dac_callback_event(uint8_t event);
static void delay(uint32_t idelay);
static void configure_ps2_power_state(void);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/*******************************************************************************
 * DAC example initialization function
 ******************************************************************************/
void sl_ulp_dac_example_init(void)
{
  sl_dac_version_t dac_version;
  sl_status_t status;
  sl_dac_clock_config_t dac_clock_config;
  dac_clock_config.soc_pll_clock           = PS4_SOC_FREQ;
  dac_clock_config.soc_pll_reference_clock = SOC_PLL_REF_FREQUENCY;
  dac_clock_config.division_factor         = DIVISION_FACTOR;
  do {
    // Version information of DAC driver
    dac_version = sl_si91x_dac_get_version();
    DEBUGOUT("DAC version is fetched successfully \n");
    DEBUGOUT("SL_DAC API version is %d.%d.%d \n", dac_version.release, dac_version.major, dac_version.minor);
    // Initializing DAC peripheral
    status = sl_si91x_dac_init(&dac_clock_config);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_dac_init: Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("SL_DAC initialization is successful \n");
    DEBUGOUT("current power state : %d\n", sl_si91x_power_manager_get_current_state());
    //   DAC configuration
    status = sl_si91x_dac_set_configuration(sl_dac_config, vref_value);
    /* Due to calling trim_efuse API on DAC configuration in driver it will
      change the clock frequency, if we are not initialize the debug again
      it will print the garbage data in console output. */
    DEBUGINIT();
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_dac_set_configuration: Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("SL_DAC set configuration is successful \n");
    // Register user callback function
    status = sl_si91x_dac_register_event_callback(dac_callback_event);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_dac_register_event_callback: Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("SL_DAC register event callback is successful \n");
    // DAC input sample data writing
    status = sl_si91x_dac_write_data((int16_t *)dac_input_sample_data, BUFFER_SIZE);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_dac_write_data: Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("SL_DAC data write is successful\n");
    DEBUGOUT("SL_DAC start \n");
    // Start DAC peripheral
    status = sl_si91x_dac_start();
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_dac_start: Error Code : %lu \n", status);
      break;
    }
  } while (false);
}

/*******************************************************************************
 * Function will run continuously and will wait for trigger
 ******************************************************************************/
void sl_ulp_dac_example_process_action(void)
{
  /*************************************************************************************************
   * This section manages power state transitions within the system, optimizing
   *power consumption while maintaining essential functionality. It transitions
   *the system from a higher power state (PS4) to a lower one (PS2) during
   *specific operations to conserve power. This involves adjusting clock
   * references and shutting down unnecessary power supplies. After completing
   *the operation, the code transitions back to the higher power state (PS4) to
   *ensure adequate resources for subsequent tasks. This approach balances power
   *efficiency with operational requirements across various system functions.
   ***************************************************************************************************/
  sl_status_t status;
  switch (ulp_dac_current_mode) {
    case ULP_DAC_PROCESS_ACTION:
      if (dac_static_intr_flag == true) {
        DEBUGOUT("Data successfully sampled \n");
      }

      else if (dac_fifo_intr_flag == true) {
        sl_si91x_dac_stop();
        sl_si91x_dac_deinit();
      }
      if ((dac_static_intr_flag == true || dac_fifo_intr_flag == true)) {
        dac_static_intr_flag = false;
        dac_fifo_intr_flag   = false;
        sl_si91x_dac_deinit();
        ulp_dac_current_mode = SL_ULP_DAC_POWER_STATE_TRANSITION;
      }
      break;

    case SL_ULP_DAC_POWER_STATE_TRANSITION:
      if (dac_ps4_to_ps2_transition_done && dac_ps2_to_ps4_transition_done) {
        dac_ps4_to_ps2_transition_done = false;
        dac_ps2_to_ps4_transition_done = false;
        status                         = sl_si91x_dac_deinit();
        if (status != SL_STATUS_OK) {
          DEBUGOUT("sl_si91x_dac_deinit: Error Code : %lu \n", status);
        }
        DEBUGOUT("Data deinit \n");
        ulp_dac_current_mode = ULP_DAC_TRANSMISSION_COMPLETED;
      } else if (current_power_state == SL_SI91X_POWER_MANAGER_PS4) {
        DEBUGOUT("Switching the adc from PS4 -> PS2 state\n");
        // Control power management by adjusting clock references and shutting
        // down the power supply
        sl_si91x_wireless_shutdown();
        status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);
        /* Due to calling trim_efuse API om power manager it will change the clock
       frequency, if we are not initialize the debug again it will print the
       garbage data or no data in console output. */
        DEBUGINIT();
        if (status != SL_STATUS_OK) {
          DEBUGOUT("sl_si91x_power_manager_add_ps_requirement: Error Code : %lu \n", status);
          break;
        }
        // Configuring the ps2 power state by configuring
        // the ram retention and removing the unused peripherals
        configure_ps2_power_state();
        current_power_state = SL_SI91X_POWER_MANAGER_PS2;
        // Adding 5 seconds delay for verifying the current consumption
        delay(FIVE_SECOND_DELAY);
        sl_ulp_dac_example_init();
        ulp_dac_current_mode           = ULP_DAC_PROCESS_ACTION;
        dac_ps4_to_ps2_transition_done = true;
      } else if (current_power_state == SL_SI91X_POWER_MANAGER_PS2) {
        DEBUGOUT("Switching the adc from PS2 -> PS4 state\n");
        status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
        if (status != SL_STATUS_OK) {
          DEBUGOUT("sl_si91x_power_manager_add_ps_requirement: Error Code : %lu \n", status);
          break;
        }
        DEBUGINIT();
        current_power_state = SL_SI91X_POWER_MANAGER_PS4;
        // Adding 5 seconds delay for verifying the current consumption
        delay(FIVE_SECOND_DELAY);
        sl_ulp_dac_example_init();
        ulp_dac_current_mode           = ULP_DAC_PROCESS_ACTION;
        dac_ps2_to_ps4_transition_done = true;
      }
      break;
    case ULP_DAC_TRANSMISSION_COMPLETED:
      current_power_state = LAST_ENUM_POWER_STATE;
      break;

    default:
      break;
  }
}
/*******************************************************************************
 * Callback event function
 * It is responsible for the event which are triggered by DAC interface
 * @param  event       : SL_DAC_FIFO_MODE_EVENT => DAC input sample for
 *                                 standard sine wave data acquisition done.
 *                       SL_DAC_STATIC_MODE_EVENT => Static mode DAC data
 *                                                      acquisition done.
 ******************************************************************************/
static void dac_callback_event(uint8_t event)
{
  if (event == SL_DAC_STATIC_MODE_EVENT) {
    dac_static_intr_flag = true;
  }
  if (event == SL_DAC_FIFO_MODE_EVENT) {
    dac_fifo_intr_flag = true;
  }
}

/*******************************************************************************
 * Delay function for 1ms
 ******************************************************************************/
static void delay(uint32_t idelay)
{
  for (uint32_t x = 0; x < MS_DELAY_COUNTER * idelay; x++) {
    __NOP();
  }
}
/*******************************************************************************
 * powering off the peripherals not in use,
 * Configuring power manager ram-retention
 ******************************************************************************/
static void configure_ps2_power_state(void)
{
  sl_power_peripheral_t peri;
  sl_power_ram_retention_config_t config;
  peri.m4ss_peripheral = 0;
  // Configure RAM banks for retention during power management
  config.configure_ram_banks = true; // Enable RAM bank configuration
  config.m4ss_ram_banks      = SL_SI91X_POWER_MANAGER_M4SS_RAM_BANK_8 | SL_SI91X_POWER_MANAGER_M4SS_RAM_BANK_9
                          | SL_SI91X_POWER_MANAGER_M4SS_RAM_BANK_10; // Specify the RAM banks to be
                                                                     // retained during power
                                                                     // management
  config.ulpss_ram_banks = SL_SI91X_POWER_MANAGER_ULPSS_RAM_BANK_2 | SL_SI91X_POWER_MANAGER_ULPSS_RAM_BANK_3;
  // Ored value for ulpss peripheral.
  peri.ulpss_peripheral = SL_SI91X_POWER_MANAGER_ULPSS_PG_MISC | SL_SI91X_POWER_MANAGER_ULPSS_PG_CAP
                          | SL_SI91X_POWER_MANAGER_ULPSS_PG_SSI | SL_SI91X_POWER_MANAGER_ULPSS_PG_I2S
                          | SL_SI91X_POWER_MANAGER_ULPSS_PG_I2C | SL_SI91X_POWER_MANAGER_ULPSS_PG_IR
                          | SL_SI91X_POWER_MANAGER_ULPSS_PG_FIM;
  // Ored value for npss peripheral.
  peri.npss_peripheral = SL_SI91X_POWER_MANAGER_NPSS_PG_MCUWDT | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUPS
                         | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUTS | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUSTORE2
                         | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUSTORE3;
  do {
    // Peripherals passed in this API are powered off.
    status = sl_si91x_power_manager_remove_peripheral_requirement(&peri);
    if (status != SL_STATUS_OK) {
      // If status is not OK, return with the error code.
      DEBUGOUT("sl_si91x_power_manager_remove_peripheral_requirement failed, "
               "Error Code: 0x%lX",
               status);
      break;
    }
    // RAM retention modes are configured and passed into this API.
    status = sl_si91x_power_manager_configure_ram_retention(&config);
    if (status != SL_STATUS_OK) {
      // If status is not OK, return with the error code.
      DEBUGOUT("sl_si91x_power_manager_configure_ram_retention failed, Error "
               "Code: 0x%lX",
               status);
      break;
    }
  } while (false);
}
