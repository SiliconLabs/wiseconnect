# Migrating from WiSeConnect™ SDK v3.3.4 to v3.4.0

## Table of Contents

- [Overview](#overview)
- [Migration Steps](#migration-steps)
  - [Update API Calls](#update-api-calls)
  - [Update Files](#update-files)
  - [Update Macros](#update-macros)

## Overview

This is a guide for updating an existing application using the WiSeConnect™ SDK v3.3.4 to a v3.4.0 application.

There are few naming and file changes in v3.4.0 as compared to v3.3.4, mostly in order to standardize the names and improve the overall usage experience of the application programming interface (API). Migration requires the names everywhere to be updated in the existing code of an application.

## Migration Steps

In order to convert a WiSeConnect SDK v3.3.4 application to a v3.4.0 application,

1. Open your existing application project in Simplicity Studio.

2. In each source file of the project, replace the v3.3.4 names or interfaces with v3.4.0 names or interfaces. 

Refer to the tables in each of the sections that follow which map the v3.3.4 API elements to v3.4.0. In some instances, the differences between v3.3.4 and v3.4.0 are highlighted in **bold** text.
- [Update API Calls](#update-api-calls)
- [Update Files](#update-files)
- [Update Macros](#update-macros)
- [Update Types](#update-types)

### Update API Calls

- For **sl_si91x_select()** API:
  - Until the 3.3.4 release, if the callback argument was not provided (i.e., NULL) in the `sl_si91x_select()` API, the function would return zero on success and -1 on failure. However, starting with the 3.4.0 release, when the callback is not provided, the function now returns the total number of file descriptors in the three descriptor sets upon successful execution. It may return zero if the timeout expires, or -1 in case of errors.

- For **sl_si91x_fwup_load()** API:
  - Before WC-3.4.0, the firmware update process returned a success code of 0x10003 upon successful completion. However, this code had an overlap with an existing error code, specifically the “No AP Found” error. To resolve this issue, the firmware update return codes have been restructured. Starting with WC-3.4.0, a prefix of 0xdd has been added to all firmware update return codes. This ensures a unique and easily identifiable range of return values for firmware updates.

- For **sl_si91x_setsockopt_async** API:
  - The term "async" has been removed from the API as setting an option on a socket is not related to the socket being synchronous/asynchronous. Hence, the name of the API has been updated to `sl_si91x_setsockopt`.

- Removed **sl_si91x_set_custom_sync_sockopt** API:
  - Starting from WC-3.4.0, the `setsockopt` API of BSD can be used to set SiWx91x options that were previously supported by `sl_si91x_set_custom_sync_sockopt`.

- Removed **sl_si91x_get_custom_sync_sockopt** API:
  - Starting from WC-3.4.0, the `getsockopt` API of BSD can be used to get SiWx91x options values that were previously supported by `sl_si91x_get_custom_sync_sockopt`.

- Removed **sl_si91x_m4_sleep_wakeup** API:
  - Starting from WC-3.4.0, the `sl_si91x_power_manager_sleep` API of Power Manager should be used in power-save applications without tickless idle mode that were previously supported by `sl_si91x_m4_sleep_wakeup`.
  - The `sl_si91x_power_manager_sleep` API is defined in the `sl_si91x_power_manager.h` file. Therefore, include this header file in power-save applications.

- Removed `sl_si91x_soc_soft_reset()` API references from the SDK

### Update Files

- NA

### Update Macros

| **Module**  |     **v3.3.4**                |         **v3.4.0**             |
|-------------| ------------------------------| -------------------------------|
| Wi-Fi       | `SO_CERT_INDEX`               | `SL_SO_CERT_INDEX`             |
| Wi-Fi       | `SO_HIGH_PERFORMANCE_SOCKET`  | `SL_SO_HIGH_PERFORMANCE_SOCKET`|
| Wi-Fi       | `SO_TLS_SNI`                  | `SL_SO_TLS_SN`                 |
| Wi-Fi       | `SO_TLS_ALPN`                 | `SL_SO_TLS_ALPN`               |
- Note: The custom socket option macros have been renamed with the prefix "SL" to enhance readability.


### Update Typedefs
| **Module**  |     **v3.3.4**                      |         **v3.4.0**                                 |
|-------------| ------------------------------------| ---------------------------------------------------|
| Wi-Fi       | `receive_data_callback`             | `sl_si91x_socket_receive_data_callback_t`          |
| Wi-Fi       | `accept_callback`                   | `sl_si91x_socket_accept_callback_t`                |
| Wi-Fi       | `data_transfer_complete_handler`    | `sl_si91x_socket_data_transfer_complete_handler_t` |
| Wi-Fi       | `select_callback`                   | `sl_si91x_socket_select_callback_t`                |
| Wi-Fi       | `remote_socket_termination_callback`| `sl_si91x_socket_remote_termination_callback_t`    |
- Note: The above typedefs were updated in compliance with Silicon Labs coding standards.

### Update Types

| **Module** | **v3.3.4** | **v3.4.0** |
|------------|------------|------------|
| Wi-Fi   | `sl_si91x_performance_profile_t` | The `sl_si91x_performance_profile_t` enum variables have been renamed for clarity, with `STANDBY_POWER_SAVE` updated to `DEEP_SLEEP_WITHOUT_RAM_RETENTION` and `STANDBY_POWER_SAVE_WITH_RAM_RETENTION` updated to `DEEP_SLEEP_WITH_RAM_RETENTION`.   |
| Wi-Fi  | `SL_WIFI_STATS_AYSNC_EVENT` enum variable in `sl_wifi_event_t` | `SL_WIFI_STATS_ASYNC_EVENT` enum variable in `sl_wifi_event_t`|
|Memory configuration components |si917_memory_default_config|si91x_memory_default_config|
|Memory configuration components |si917_mem_config_1|si91x_mem_config_1|
|Memory configuration components |si917_mem_config_2|si91x_mem_config_2|
|Memory configuration components |si917_mem_config_3|si91x_mem_config_3|
