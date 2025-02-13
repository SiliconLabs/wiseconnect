# Migrating from WiSeConnect™ SDK v3.4.2 to v3.5.0

## Table of Contents

- [Overview](#overview)
- [Migration Steps](#migration-steps)
  - [Updated default TCP/IP thread priority in LWIP stack](#updated-default-tcpip-thread-priority-in-lwip-stack)

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

### Update Types

| **Module** | **v3.4.2** | **v3.5.0** |
|------------|------------|------------|
| Wi-Fi | **sl_si91x_network_params_response_t** | typo error corrected as **sli_si91x_network_params_response_t** |
| Wi-Fi | **sl_si91x_rsp_wireless_info_t** | added new member variable named bssid to store the address of connected AP|
