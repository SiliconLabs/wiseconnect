#include "console.h"
#include "sl_si91x_driver.h"

sl_status_t sl_wifi_assert_command_handler()
{
  return sl_si91x_assert();
}

sl_status_t sl_si91x_get_ram_log_command_handler(console_args_t *arguments)
{
  uint32_t address = (uint32_t)GET_COMMAND_ARG(arguments, 0);
  uint32_t length  = (uint32_t)GET_COMMAND_ARG(arguments, 1);
  if (length == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  return sl_si91x_get_ram_log(address, length);
}
