# BLE - iBeacon

## Table of Contents

- [Purpose/Scope](#purposescope) 
- [iBeacon Overview](#ibeacon-overview)
- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [Setup Diagram](#setup-diagram)
- [Getting Started](#getting-started)
- [Application Build Environment](#application-build-environment)
- [Test the Application](#test-the-application)

## Purpose/Scope

This application demonstrates how to set the iBeacon data format in advertising parameters in  BLE peripheral mode.

## iBeacon Overview

The iBeacon advertise data format is as follows:

  | iBeacon prefix |UUID |Major Number |Minor |Number |TX Power |
  |------|---|------|------|------|------|
  |1|9Bytes|16bytes |2Bytes |2 Bytes |1bytes|

- iBeacon Prefix:
   Vendor specific fixed value.
   Default iBeacon prefix values setting by application is:
   ``Prefix = {0x02, 0x01, 0x02, 0x1A, 0xFF, 0x4C, 0x00, 0x02, 0x15}``

- UUID:
  - User generated proximity UUID.
  - Remote devices recognize which beacon they approach on the basis of UUID, major and minor numbers.
  - Default UUID, Major and Minor values setting by application is:
   ``UUID = {0xFB , 0x0B , 0x57 , 0xA2 , 0x82 , 0x28 , 0x44 , 0xCD , 0x91 ,
   0x3A , 0x94 , 0xA1 , 0x22 , 0xBA , 0x12 , 0x06}``
   major_num = {0x11, 0x22}
   minor_num = {0x33, 0x44}

  - TX Power is used to calculate distance from iBeacon.
   Default TX power value setting by application is:
   `TX Power = 0x33`
  > **Note:**
  >If the user wants to change the prefix, UUID, Major number, Minor number and TX Power values, change the following values in rsi_ble_ibeacon.c_ file.
  >
  > **For Prefix:**
  > adv[31] = {0x02, 0x01, 0x02, 0x1A, 0xFF, 0x4C, 0x00, 0x02, 0x15}; //prefix(9bytes)`
  >
  > **For UUID:**
  >
  > uint8_t uuid[16] = {0xFB , 0x0B , 0x57 , 0xA2 , 0x82 , 0x28 , 0x44 , 0xCD , 0x91 , 0x3A , 0x94 , 0xA1 , 0x22 , 0xBA , 0x12 , 0x06};
  >
  > For Major Number:
  >
  > - uint8_t major_num[2] = {0x11, 0x22};
  >
  > - For Minor Number:
  > - uint8_t minor_num[2] = {0x33, 0x44};
  >
  > - For TX Power:
  > - uint8_t tx_power = 0x33;

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC with Host interface(UART/ SPI/ SDIO).
- SiWx91x Wi-Fi Evaluation Kit. The SiWx91x supports multiple operating modes. See [Operating Modes]() for details.
- **SoC Mode**:
  - Standalone
    - BRD4002A Wireless pro kit mainboard [SI-MB4002A]
    - Radio Boards 
  	  - BRD4338A [SiWx917-RB4338A]
      - BRD4339B [SiWx917-RB4339B]
  	  - BRD4340A [SiWx917-RB4340A]
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

- Smart phone with ibeacon detector application

### Software Requirements

- Simplicity Studio
- Smartphone configured as BLE Peripheral

- Download and install the Silicon Labs [EFR Connect App or other BLE Central/Peripheral app.](https://www.silabs.com/developers/efr-connect-mobile-app) in the android smart phones for testing BLE applications. Users can also use their choice of BLE apps available in Android/iOS smart phones.

### Setup Diagram

![](resources/readme/ble_ibeacon_soc_ncp.png)
  
## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

## Application Build Environment

The application can be configured to suit your requirements and development environment. Read through the following sections and make any changes needed.

- Open `ble_ibeacon.c` file and update/modify following macros:

  - `RSI_BLE_LOCAL_NAME` refers name of the Silicon Labs device to appear during scanning by remote devices.

     ```c
    #define RSI_BLE_LOCAL_NAME                     "ibeacon"
    ```

  - Following are the event numbers for connection and Disconnection events,

    ```c
    #define RSI_APP_EVENT_CONNECTED                1
    #define RSI_APP_EVENT_DISCONNECTED             2
    ```

- Following are the **non-configurable** macros in the application.

  - BT_GLOBAL_BUFF_LEN refers Number of bytes required by the application and the driver

    ```c
    #define BT_GLOBAL_BUFF_LEN                     15000
    ```

- Open `ble_config.h` file and update/modify following macros,

  ```c
  #define RSI_BLE_PWR_INX                        30
  #define RSI_BLE_PWR_SAVE_OPTIONS               0
  ```  

  > **Note:** `rsi_ble_config.h` files are already set with desired configuration in respective example folders user need not change for each example.

## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Build the application in Studio.
- Flash, run and debug the application.

Follow the steps for the successful execution of the program:

1. After the program gets executed, Silicon Labs module would be in advertising state.

2. Connect any serial console for prints.

3. Open iBeaconDetector app in the smartphone and do scan.

4. In the App, Silicon Labs module device would appear with the name configured in the macro **RSI\_BLE\_LOCAL\_NAME** (Ex: "ibeacon") or sometimes observed as "SimpleBLEPeripheral".

5. After successful scan, user can see the Silicon Labs device advertised data i.e UUID, Maximum Number, Minimum Number and TX Power in iBeaconDetector application.  

6. Refer the following images for console prints:

    ![](resources/readme/output1.png)
