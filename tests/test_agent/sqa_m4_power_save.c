#include <string.h>
#include "cmsis_os2.h"
#include "sl_status.h"
#include "sl_wifi.h"
#include "sl_wifi_device.h"
#include "sl_net_wifi_types.h"
#include "sl_si91x_driver.h"
#include "rsi_rtc.h"
#include "rsi_chip.h"
#include "rsi_wisemcu_hardware_setup.h"
#include "rsi_power_save.h"
#include "sli_siwx917_soc.h"
#include "command_identifiers.h"
#include "console.h"
#include "rsi_debug.h"
#include "rsi_m4.h"
#include "sl_si91x_m4_ps.h"
#include "sl_si91x_host_interface.h"

/******************************************************
*               Function Declarations
******************************************************/

/******************************************************
*               Variable Definitions
******************************************************/

/******************************************************
*               Function Definitions
******************************************************/
sl_status_t m4_ta_secure_handshake_command_handler(console_args_t *arguments)
{
  uint8_t sub_cmd_type = (uint8_t)arguments->arg[0];
  uint8_t xtal_enable  = (uint8_t)arguments->arg[1];

  sl_status_t status = sl_si91x_m4_ta_secure_handshake(sub_cmd_type, 1, &xtal_enable, 0, NULL);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to bring m4_ta_secure_handshake: 0x%lx\r\n", status);
    return status;
  }
  printf("\r\nM4-NWP secure handshake is successful\r\n");

  return status;
}

sl_status_t m4_powersave_command_handler(void)
{
#if SL_SI91X_MCU_ALARM_BASED_WAKEUP
  initialize_m4_alarm();
#endif

#ifdef SLI_SI91X_MCU_INTERFACE
  printf("\r\nM4 in sleep\r\n");
  sl_si91x_m4_sleep_wakeup();
  printf("\r\nM4 wake up\r\n");
#endif

  return SL_STATUS_OK;
}
