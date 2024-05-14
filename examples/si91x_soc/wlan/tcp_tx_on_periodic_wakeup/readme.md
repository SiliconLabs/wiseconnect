# WLAN TCP TX on periodic wakeup

## Table of Contents

- [Purpose/Scope](#purposescope)
- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [Setup Diagram](#setup-diagram)
- [Getting Started](#getting-started)
- [Application Build Environment](#application-build-environment)
- [Test the application](#test-the-application)

## Purpose/Scope
This application demonstrates the process for configuring SiWx91x in connected sleep mode while having a TCP client socket in open state.

The application creates TCP client socket and then connects to a remote server to send 64MB data to TCP Server. After completing data transfer, NWP/TA enters connected sleep and M4 goes to sleep with configurable alarm time. After the alarm times out, M4 wakes up NWP/TA by sending a packet and the data transmission repeats.

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC
- Silicon Labs SiWx917 Evaluation Kit [WPK/WSTK + BRD4325A]
- Wireless Access Point
- Windows PC with iperf and similar TCP Server application


### Software Requirements

- Simplicity Studio
- [iPerf Application](https://iperf.fr/iperf-download.php) to run TCP Server
- VCOM Setup
  - The Docklight tool's setup instructions are provided below.

    ![Figure: VCOM_setup](resources/readme/vcom.png)

## Setup Diagram

![Setup Diagram for WLAN TCP TX Periodic Wakeup](resources/readme/setup_soc.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Install Simplicity Studio and WiSeConnect 3 extension
- Connect SiWx917 device to the computer
- Upgrade the device connectivity firmware
- Create the Studio project

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

## Application Build Environment

The application can be configured to suit your requirements and development environment. 

**STA instance related parameters**

- In the Project Explorer pane, expand the **config** folder and open the **sl_net_default_values.h** file. Configure the following parameters to enable your Silicon Labs Wi-Fi device to connect to your Wi-Fi network

- DEFAULT_WIFI_CLIENT_PROFILE_SSID refers to the name with which Wi-Fi network that shall be advertised and Si91X module is connected to it.

  ```c
  #define DEFAULT_WIFI_CLIENT_PROFILE_SSID               "YOUR_AP_SSID"      
  ```

- DEFAULT_WIFI_CLIENT_CREDENTIAL refers to the secret key if the Access point is configured in WPA-PSK/WPA2-PSK security modes.

  ```c
  #define DEFAULT_WIFI_CLIENT_CREDENTIAL                 "YOUR_AP_PASSPHRASE" 
  ```

- DEFAULT_WIFI_CLIENT_SECURITY_TYPE refers to the security type of the Access point. The supported security modes are mentioned in `sl_wifi_security_t`.

  ```c
  #define DEFAULT_WIFI_CLIENT_SECURITY_TYPE               SL_WIFI_WPA2 
  ```

- Other STA instance configurations can be modified if required in `default_wifi_client_profile` configuration structure.

**TCP Configuration**

- In the Project Explorer pane, open the **app.c** file. 

  ```c
  #define SERVER_IP_ADDRESS  "192.168.0.100"  // Server IP address 
  #define SERVER_PORT        5000             // TCP server port of the remote TCP server
  ```

## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Build the application
- Flash, run and debug the application

To establish TCP Server on remote PC, download [iPerf Application](https://iperf.fr/iperf-download.php) and run the below command from the installed folder.
		
  ```c
  C:\> iperf.exe -s -p <SERVER_PORT> -i 1
   For example ...
  C:\> iperf.exe -s -p 5001 -i 1
  ```

The SiWx91x SoC, which is configured as a TCP client, connects to the iperf server. Upon successsful TCP connection, the Si917 NWP configured in High Performance mode, sends 512MB data to server. After data transfer, NWP is set to Associated Powersave mode, M4 is set to sleep. Once the configured ALARM_PERIODIC_TIME expires, NWP is set back to high performance mode, M4 is triggered out of sleep, performs data transfer and goes back to sleep.

![Figure: Console logs](resources/readme/console_logs.png)