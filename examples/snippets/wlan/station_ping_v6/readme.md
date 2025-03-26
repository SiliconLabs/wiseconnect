# Wi-Fi - Station Ping V6

## Table of Contents

- [Purpose/Scope](#purposescope)
- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [Setup Diagram](#setup-diagram)
- [Getting Started](#getting-started)
- [Application Build Environment](#application-build-environment)
- [Test the Application](#test-the-application)

## Purpose/Scope

Ping is used diagnostically to ensure that the host computer the user is trying to reach is actually operating. Ping works by sending an Internet Control Message Protocol (ICMP) Echo Request to a specified interface on the network and waiting for a reply. The application demonstrates how to configure the SiWx91x module in client mode to send a ping request to the target IP address.

## Prerequisites/Setup Requirements

### Hardware Requirements

- A Windows PC.
- A Wi-Fi Access Point.
- SoC Mode:
  - Silicon Labs [BRD4338A](https://www.silabs.com/development-tools/wireless/wi-fi/siwx917-rb4338a-wifi-6-bluetooth-le-soc-radio-board?tab=overview)
- NCP Mode:
  - Silicon Labs [BRD4180B](https://www.silabs.com/wireless/zigbee/efr32mg21-series-2-socs/device.efr32mg21a020f1024im32)
  - Host MCU Eval Kit. This example has been tested with:
    - Silicon Labs [WSTK + EFR32MG21](https://www.silabs.com/development-tools/wireless/efr32xg21-bluetooth-starter-kit)

### Software Requirements

- Simplicity Studio IDE

### Setup Diagram

  ![Figure: Setup Diagram SoC and NCP Mode for Station Ping Example](resources/readme/stationpingsetupsoc_ncp.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- [Install Simplicity Studio](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#install-simplicity-studio)
- [Install WiSeConnect 3 extension](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#install-the-wi-se-connect-3-extension)
- [Connect your device to the computer](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#connect-si-wx91x-to-computer)
- [Upgrade your connectivity firmware](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#update-si-wx91x-connectivity-firmware)
- [Create a Studio project](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#create-a-project)

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

## Application Build Environment

The application can be configured to suit user requirements and development environment. Read through the following sections and make any changes needed.

- In the Project Explorer pane, expand the **config** folder and open the ``sl_net_default_values.h`` file. Configure the following parameters to enable your Silicon Labs Wi-Fi device to connect to your Wi-Fi network.

- STA instance related parameters

  - DEFAULT_WIFI_CLIENT_PROFILE_SSID refers to the name with which Wi-Fi network that shall be advertised and Si91X module is connected to it.
  
       ```c
       #define DEFAULT_WIFI_CLIENT_PROFILE_SSID               "YOUR_AP_SSID"      
       ```

  - DEFAULT_WIFI_CLIENT_CREDENTIAL refers to the secret key if the access point is configured in WPA-PSK/WPA2-PSK security modes.

       ```c
       #define DEFAULT_WIFI_CLIENT_CREDENTIAL                 "YOUR_AP_PASSPHRASE" 
       ```

  - DEFAULT_WIFI_CLIENT_SECURITY_TYPE refers to the security type if the access point is configured in WPA/WPA2 or mixed security modes.
 
     ```c
     #define DEFAULT_WIFI_CLIENT_SECURITY_TYPE                            SL_WIFI_WPA2 
     ```
  
  - Other STA instance configurations can be modified if required in `default_wifi_client_profile` configuration structure.

> Note: 
> Users can configure default region-specific regulatory information using `sl_wifi_region_db_config.h`

- Configure the following parameters in ``app.c`` to test station ping application

  - Remote peer configurations

      ```c
      #define REMOTE_IP_ADDRESS   "2409:40f0:18:ffa3:890d:1212:170:293e"    // Remote/Target Ipv6 address to ping
      #define PING_PACKET_SIZE    64                 // Size of ping request packet
      ```

## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Build the application.
- Flash, run and debug the application.
- Upon successfully connecting to the Access Point, the device initiates ping requests to the specified REMOTE_IP_ADDRESS with the configured PING_PACKET_SIZE to verify the availability of the target device.

  ![Station_Ping_Output](resources/readme/station_ping_output.png)
