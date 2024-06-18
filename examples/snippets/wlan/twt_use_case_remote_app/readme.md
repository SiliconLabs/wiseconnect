# Wi-Fi - TWT Use Case Remote app

- [Purpose/Scope](#purposescope) 
- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [Setup Diagram](#setup-diagram)
- [Getting Started](#getting-started)
- [Application Build Environment](#application-build-environment)
  - [Configure the Application](#configure-the-application)
  - [Application Configuration](#application-configuration)
- [Test the Application](#test-the-application)

## Purpose/Scope

The **TWT Use Case Remote App** works in conjunction with the **TWT Use Case Demo App** (Path: SDK\examples\snippets\wlan\wifi6_twt_use_case_demo) to demonstrate the TWT feature for scenarios similar to door locks and cameras. This application sends a trigger over a TCP socket to the TWT Use Case Demo App at specific intervals after connection. In response, it receives TCP data in the door lock scenario and UDP data in the camera scenario. 

This application does **not** include TWT sleep.


## Prerequisites/Setup Requirements

### Hardware Requirements  

- PC or Mac.
- Wi-Fi Access point with 11ax and TWT responder mode support.
- **SoC Mode**:
  - Standalone
    - BRD4002A Wireless pro kit mainboard [SI-MB4002A]
    - Radio Boards 
  	  - BRD4338A [SiWx917-RB4338A]
  - Kits
  	- SiWx917 Pro Kit [Si917-PK6031A](https://www.silabs.com/development-tools/wireless/wi-fi/siwx917-pro-kit?tab=overview)
  	- SiWx917 Pro Kit [Si917-PK6032A]
  	
- **NCP Mode**:
  - Standalone
    - BRD4002A Wireless pro kit mainboard [SI-MB4002A]
    - EFR32xG24 Wireless 2.4 GHz +10 dBm Radio Board [xG24-RB4186C](https://www.silabs.com/development-tools/wireless/xg24-rb4186c-efr32xg24-wireless-gecko-radio-board?tab=overview)
    - NCP EFR Expansion Kit with NCP Radio board (BRD4346A + BRD8045A) [SiWx917-EB4346A]
  - Kits
  	- EFR32xG24 Pro Kit +10 dBm [xG24-PK6009A](https://www.silabs.com/development-tools/wireless/efr32xg24-pro-kit-10-dbm?tab=overview)

### Software Requirements

- Simplicity Studio 
- [Iperf Application](https://iperf.fr/iperf-download.php)

### Setup Diagram

 **SoC**
![Figure: Setup Diagram for TWT Use Case Remote AppExample: SoC](resources/readme/twt_tcpandudpclient_soc.png) 
 
 **NCP**
![Figure: Setup Diagram for TWT Use Case Remote AppExample: SoC](resources/readme/twt_tcpclient_udp_client_ncp.png) 

**NOTE**: 
- The Host MCU platform (EFR32xG21) and the SiWx91x interact with each other through the SPI interface. 

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

## Application Build Environment

The application can be configured to suit your requirements and development environment.

### Configure the Application

In the Project Explorer pane, expand the **config** folder and open the **sl_net_default_values.h** file. Configure the following parameters to enable your Silicon Labs Wi-Fi device to connect to your Wi-Fi network.

This application, used with the TWT Use Case Demo Application, simulates door lock and camera scenarios.

In the camera scenario, the remote app (Device A) sends a command to the Device Under Test (DUT, Device B). Device B responds with UDP data, simulating camera streaming.

In the door lock scenario, upon receiving a command from Device A, Device B sends a TCP response, simulating a door lock status response.

The app operates a TCP server, periodically sending commands to Device B and awaiting a TCP/UDP response, depending on the scenario.

Device B functions as a TCP client and, if the DOOR_LOCK_SIMULATION macro is disabled for the camera scenario, also as a UDP client.

The application measures Round Trip Time (RTT) in milliseconds, calculated from when the command is sent to when the first response packet is received.

- **STA instance related parameters**

  - DEFAULT_WIFI_CLIENT_PROFILE_SSID refers to the name with which the SiWx91x SoftAP's Wi-Fi network shall be advertised.

     ```c
     #define DEFAULT_WIFI_CLIENT_PROFILE_SSID               "YOUR_AP_SSID"      
     ```

  - DEFAULT_WIFI_CLIENT_CREDENTIAL refers to the secret key if the access point is configured in WPA-PSK/WPA2-PSK security modes.

     ```c 
     #define DEFAULT_WIFI_CLIENT_CREDENTIAL                 "YOUR_AP_PASSPHRASE" 
     ```

  - For Doorlock use case, TCP_RECEIVE should be 1 and for camera streaming scenario TCP_RECEIVE should be 0.

- **Configure the following parameters in app.c**
  - Configure RECEIVE_DATA_TIMEOUT. It is the interval between two successive trigger commands from the application. It is in milli seconds. It should be greater than [rx_latency](https://docs.silabs.com/wiseconnect/latest/wiseconnect-api-reference-guide-wi-fi/sl-wifi-twt-selection-t#rx-latency) configured in **TWT Use Case Demo App** 

  - Other STA instance configurations can be modified if required in `default_wifi_client_profile` configuration structure.

  - Client/Server IP Settings

      ```c
      #define TCP_RECEIVE        1
      #define TCP_LISTENING_PORT 5001
      #define UDP_LISTENING_PORT 5002            
      ```

## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Build the application.
- Flash, run and debug the application.
- After successful connection, module will be brought up as a TCP server socket, listens for TCP client and sends configured NUMBER_OF_PACKETS to TCP or UDP client (Si91x module with TWT enabled).

    ![Application prints](resources/readme/remoteapplicationprintssoc.png)
