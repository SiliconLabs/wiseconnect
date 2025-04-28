#Migrating from WiSeConnect™ SDK v3 .4.2 to v3 .5.0

## Table of Contents

- [Table of Contents](#table-of-contents)
- [Overview](#overview)
- [Migration Steps](#migration-steps)
  - [Updated default TCP/IP thread priority in LWIP stack:](#updated-default-tcpip-thread-priority-in-lwip-stack)
  - [Updated-user-gain-table-API](#updated-user-gain-table-api)
  - [Condition Introduced for Keeping Files in .data Section Based on pm\_ps2\_component](#condition-introduced-for-keeping-files-in-data-section-based-on-pm_ps2_component)
  - [API Naming Corrections](#api-naming-corrections)
  - [Changes to Reserved Fields](#changes-to-reserved-fields)
  - [Updated Structures](#updated-structures)
  - [Deprecated APIs](#deprecated-apis)
  - [Deprecated Macros](#deprecated-macros)
  - [Deprecated Enumerators](#deprecated-enumerators)
  - [Deprecated Structures/API's](#deprecated-structuresapis)

## Overview

This is a guide for updating an existing application using the WiSeConnect™ SDK v3.4.2 to a v3.5.0 application.

There are few naming and file changes in v3.5.0 as compared to v3.4.2, mostly in order to standardize the names and improve the overall usage experience of the application programming interface (API). Migration requires the names everywhere to be updated in the existing application.

## Migration Steps

In order to convert a WiSeConnect SDK v3.4.2 application to a v3.5.0 application,

1. Open your existing application project in Simplicity Studio.

2. In each source file of the project, replace the v3.4.2 names or interfaces with v3.5.0 names or interfaces. 

Refer to the tables in each of the sections that follow which map the v3.4.2 API elements to v3.5.0 In some instances, the differences between v3.4.2 and v3.5.0 are highlighted in **bold** text.


### Updated default TCP/IP thread priority in LWIP stack:

As part of the migration from WiSeConnect™ SDK v3.4.2 to v3.5.0, there have been changes to the TCP/IP thread priority in the LWIP stack to ensure faster and more responsive handling of network operations.

**Reason for Change**

Currently, the TCP/IP thread priority is at osPriorityIdle. The TCP/IP thread's idle priority is causing issues with UDP data transfer, as it is unable to process the ARP response. This occurs because the application thread, with its higher priority, does not allow the TCP/IP thread sufficient processing time.

The ideal priority for the TCP/IP thread in the LWIP stack should be higher than application threads but lower than critical system threads such as the bus and event threads.
To address this, we have updated the TCP/IP thread in LWIP priority to osPriorityNormal.

**User Impact**

Increasing the LWIP TCP/IP thread priority can affect the functionality of application threads, as their priorities are lower compared to the TCP/IP thread.

### Updated-user-gain-table-API

As part of the migration from WiSeConnect™ SDK v3.4.2 to v3.5.0, A new API `sl_wifi_update_su_gain_table` is added for loading Wi-Fi user gain table values to get more gain in 11ax SU operation. 

**Reason for Change**
This request aims to support at least two 11ax Power tables (SU and TB), as currently only one power table is supported. Due to this limitation, we are using lower TB-based power levels in 11ax, which reduces overall 11ax performance.
 
With current tests:
 
- ETSI/MIC TX powers are reduced by about ~6dB for 11ax (limited by 26Tone RU - PSD).
- FCC Band edge for 26Tone RU is worse by up to ~2dB compared to other 11ax cases.
  -  With the new power table, we will be able to support both TB and SU side by side with two different power levels, without compromising the SU power table.
  - We should be able to support different levels or backoffs for various tones (26, 52, 106, etc.).
  - Supporting different backoffs based on the placement of 26-tone or 52-tone (whether center, edge of 20MHz, etc.) is optional.
**User Impact**

Increased gain values as per mentioned above.

### Condition Introduced for Keeping Files in .data Section Based on pm_ps2_component
   A condition has been introduced in release **3.5.0** to keep specific files in the `.data` section (RAM) **only if** the `pm_ps2_component` is installed.

- **Component Requirement:**  
  The `pm_ps2_component` must be installed when the application is designed to switch or manage PS2 power states.

- **Reason for Change:**  
  In previous releases,when the ULP wakeup sources components were used or installed, files related to PS2 functionality were unnecessarily placed in the `.data` section, causing **increased RAM usage** in applications that did not require PS2 features. This change optimizes RAM usage by including these files only when necessary.

- **Action Required:**  
  If your application uses PS2 power management:
  - Install the `pm_ps2_component` in **Simplicity Studio**,  
    **OR**
  - Add it to the `components` section of your `.slcp` file manually.
  
- **Note:**  
  Without this component, related files will no longer be moved to `.data` section, helping reduce RAM footprint for applications that do not use PS2 features.

### API Naming Corrections

| **Module** | **v3.4.2** | **v3.5.0** |
|------------|------------|------------|
| Wi-Fi | **sl_si91x_network_params_response_t** | typo error corrected as **sli_si91x_network_params_response_t** |

### Changes to Reserved Fields

| **Module** | **v3.4.2** | **v3.5.0** |
|------------|------------|------------|
| Wi-Fi | reserved **BIT 6** in **ctrl_flags** member variable of **sl_wifi_transceiver_tx_data_control_t** | used for **Extended Information** |
| Wi-Fi | reserved **BIT 7** in **ctrl_flags** member variable of **sl_wifi_transceiver_tx_data_control_t** | used for **Immediate Transfer** |
| Wi-Fi | **reserved1** member variable of **sl_wifi_transceiver_tx_data_control_t** | renamed to **ctrl_flags1** |

### Updated Structures

| **Module** | **v3.4.2** | **v3.5.0** |
|------------|------------|------------|
| Wi-Fi | The **status** member variable in **sl_wifi_transceiver_tx_data_confirmation_t** is of type **sl_status_t** | The **status** member variable in **sl_wifi_transceiver_tx_data_confirmation_t** has been changed to **uint32_t** |
| Wi-Fi | The **status** member variable in **sl_wifi_transceiver_rx_data_t** is of type **sl_status_t** | The **status** member variable in **sl_wifi_transceiver_rx_data_t** has been changed to **uint32_t** |

### Deprecated APIs

**Note:** The following elements are applicable to both NCP and SoC modes.

- Deprecated **sl_wifi_update_gain_table** API.
  > **Note**: Starting from WC-3.5.0, the sl_wifi_update_gain_table API is deprecated. It is recommended to use the updated API to ensure compatibility with future releases.

  | **Module**| **v3.4.2**                  | **v3.5.0**                     |
  |-----------|-----------------------------|--------------------------------|
  | Wi-Fi     | `sl_wifi_update_gain_table` | `sl_wifi_update_su_gain_table` |

### Deprecated Macros

**Note:** The following elements are applicable to both NCP and SoC modes.

- Deprecated **SL_SI91X_TCP_IP_FEAT_POP3_CLIENT** macro.
  > **Note**: Starting from WC-3.5.0, the SL_SI91X_TCP_IP_FEAT_POP3_CLIENT macro is deprecated. It is recommended to use the updated macros to ensure compatibility with future releases.

  | **Module**| **v3.4.2**                                       | **v3.5.0**                                           |
  |-----------|--------------------------------------------------|------------------------------------------------------|
  | Wi-Fi     | `SL_SI91X_TCP_IP_FEAT_POP3_CLIENT`               | `SL_SI91X_TCP_IP_FEAT_DTLS_THREE_SOCKETS`            |

### Deprecated Enumerators

**Note:** The following elements are applicable to both NCP and SoC modes.

- Deprecated **SL_NET_INVALID_CREDENTIAL_ID** enumerator.
  > **Note**: Starting from WC-3.5.0, the enumerator SL_NET_INVALID_CREDENTIAL_ID is deprecated. It is recommended to use the updated enumerator to ensure compatibility with future releases.

  | **Module**| **v3.4.2**                                       | **v3.5.0**                                           |
  |-----------|--------------------------------------------------|------------------------------------------------------|
  | Network Manager     | `SL_NET_INVALID_CREDENTIAL_ID`                   | `SL_NET_NO_CREDENTIAL_ID`            |



### Deprecated Structures/API's

**Note:** The following elements are applicable to both NCP and SoC modes.

- Deprecated few APIs and Structures.
  > **Note**: Starting from WC-3.5.0, It is recommended to use the v2 versions of APIs/Structures to ensure compatibility with future releases.

  | **Module**| **v3.4.2**                                       | **v3.5.0**                                           |
  |-----------|--------------------------------------------------|------------------------------------------------------|
  | Wi-Fi     | `sl_wifi_performance_profile_t`                  | `sl_wifi_performance_profile_v2_t`                   |
  | Wi-Fi     | `sl_wifi_listen_interval_t`                      | `sl_wifi_listen_interval_v2_t`                       |
  | Wi-Fi     | `sl_wifi_set_performance_profile`                | `sl_wifi_set_performance_profile_v2`                 |
  | Wi-Fi     | `sl_wifi_get_performance_profile`                | `sl_wifi_get_performance_profile_v2`                 |
  | Wi-Fi     | `sl_wifi_set_listen_interval`                    | `sl_wifi_set_listen_interval_v2`                     |
  | Wi-Fi     | `sl_wifi_get_listen_interval`                    | `sl_wifi_get_listen_interval_v2`                     |
  | Wi-Fi     | `sl_si91x_set_listen_interval`                   | `sl_wifi_set_listen_interval_v2`                     |