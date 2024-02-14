# BLE - Power Save

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

This application demonstrates how to configure SiWx91x in power save profile in Advertising mode & Connected mode.

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC.
- SiWx91x Wi-Fi Evaluation Kit.
- SoC Mode
  - Silicon Labs [BRD4325A, BRD4325B, BRD4325C, BRD4338A, BRD4339B ](https://www.silabs.com/)
- NCP Mode:
  - Silicon Labs [BRD4180B](https://www.silabs.com/); **AND**
  - Host MCU Eval Kit. This example has been tested with:
  - Silicon Labs [WSTK + EFR32MG21](https://www.silabs.com/development-tools/wireless/efr32xg21-bluetooth-starter-kit)
- **PSRAM Mode**:
    - Silicon Labs [BRD4340A](https://www.silabs.com/)
- Power Analyzer

### Software Requirements

- Simplicity Studio
- Download and install the Silicon Labs [EFR Connect App or other BLE Central/Peripheral app.](https://www.silabs.com/developers/efr-connect-mobile-app) in the android smart phones for testing BLE applications. Users can also use their choice of BLE apps available in Android/iOS smart phones.

### Setup Diagram

  ![Figure: Setup Diagram for Power Save example](resources/readme/ble_power_save_soc_ncp.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

## Application Build Environment

The application can be configured to suit your requirements and development environment. Read through the following sections and make any changes needed.

- Open `ble_config.h` file and update/modify following macros

   
  - `BLE_ROLE` refers to the SiWx91x device role, valid configurations are `PERIPHERAL_ROLE`, `CENTRAL_ROLE & DUAL_ROLE`.
    ```c
    #define BLE_ROLE    PERIPHERAL_ROLE
    ```
   
  - Remote Peripheral device paramters:
    - `RSI_BLE_DEV_ADDR_TYPE` refers to the address type of the remote device to connect.
    - Based on address type of remote device, valid configurations are `LE_RANDOM_ADDRESS` and `LE_PUBLIC_ADDRESS`
      ```c
	    #define RSI_BLE_DEV_ADDR_TYPE             LE_PUBLIC_ADDRESS 
      ```
    - `RSI_BLE_DEV_ADDR` refers to the address of the remote device to connect.
      ```c
      #define RSI_BLE_DEV_ADDR                               "00:1E:7C:25:E9:4D" 
      ```
    - `RSI_REMOTE_DEVICE_NAME` refers to the name of remote device to which Silicon Labs device has to connect.
      ```c
      #define RSI_REMOTE_DEVICE_NAME                         "SILABS_DEV" 
      ```
  
    > **Note:** you required to configure either the `RSI_BLE_DEV_ADDR` or `RSI_REMOTE_DEVICE_NAME` of the remote device.
### Power save configuration
  - Configure `ENABLE_POWER_SAVE` parameter to enable power save mode.  
  
    ```c
    #define ENABLE_POWER_SAVE              1
    ```
   
    > **Note:** If you are using the NCP-EXP-Board, refer the "**Powersave functionality with NCP expansion board**" section  the ***Getting started with SiWx91x NCP*** guide.

  - `PSP_TYPE` refers power save profile type.
    ```c
    #define PSP_TYPE                             RSI_MAX_PSP
    ```
    The Silicon Labs device supports following power save profile types in BTLE mode,
    - **RSI_MAX_PSP (0):** In this mode, the Silicon Labs device will be in Maximum power save mode. i.e Device will wake up for every DTIM beacon and do data Tx and Rx.

  - `PSP_MODE` refers power save profile mode. 
    ```c
    #define PSP_MODE                             RSI_SLEEP_MODE_2
    ```
    The Silicon Labs device supports following power modes in BTLE,
   
    - **RSI_ACTIVE (0):** In this mode, the Silicon Labs device is active and power save is disabled.
   
    - **RSI_SLEEP_MODE_2 :** This mode is applicable when the Silicon Labs device is in Advertising state as well as in connected state. In this sleep mode, SoC will go to sleep based on GPIO handshake, therefore handshake is required before sending data to the Silicon Labs device.
   
    - **RSI_SLEEP_MODE_8 :** In this power mode, the Silicon Labs device goes to power save when it is in the unassociated state with the remote device. In this sleep mode, SoC will go to sleep based on Message exchange, therefore handshake is required before sending the command to the Silicon Labs device.     
   
    > **Note:** Default `RSI_SLEEP_MODE_2` with `MAX_PSP` is configured in the application.

> **Note:** "Opermode command parameters" are set with desired configuration in app.c.	

> **Note:** `ble_config.h` and `app.c` files are already set with desired configuration in respective example folders you need not change for each example.

## Test the application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Build the application in Studio.
- Flash, run and debug the application.

Follow the steps for successful execution of the program:

> **Note:** The provided mobile screenshots are from the 2.5.2 version of the EFR Connect app, it is recommended to use the latest version.

1. After the program gets executed, the Silicon Labs device  would be in Advertising state with configured power save the profile.

2. Connect any serial console for prints.

3. The Silicon Labs device will go to sleep and wakes up for every advertising interval and goes back to sleep after advertising. Refer the given following image for power save cycle in advertising mode.

    ![Power Save Cycle in Advertising mode](resources/readme/blepowersavencpmeasurement.png)       

4. Open an EFR Connect App in the Smartphone and do Scan.

5. In the App, Silicon Labs device will appear with the name configured in the macro **RSI_BLE_LOCAL_NAME (Ex: "WLAN_BLE_SIMPLE")** or sometimes observed as the Silicon Labs device as the internal name **"SILABS_DEVICE".**

    ![](resources/readme/blepowersaveadvertising.png) 

6. Initiate connection from the mobile App.

7. After successful connection, user can see the connected state in EFR connect app and also check the supported services by the Silicon Labs device. 

    ![](resources/readme/blepowersavedeviceconnection.png) 

8. After successful connection, Silicon Labs device goes to sleep and wakes up for every connection interval. Check the following image for power save cycle after connection.


   ![](resources/readme/blepwlog.png)    

9. After successful program execution, if the Silicon Labs device is configured in PERIPHERAL_ROLE , the prints in teraterm looks as shown below.

> **Note:**
>- Default configuration of connection interval of Central device (smartphone) is 18 ms. So, the Silicon Labs device  will wake up for every 18ms sec and goes back to sleep after advertise.
>- Above power save profile image capture when it is in the idle state after successful connection. So, the user may not get same profile as shown above image.
It will vary based on the traffic.

10. Refer the following image for console prints:

    ![Application Prints Soc](resources/readme/output1.png)

## **Current consumption measurement**

- **NCP**      
   - A Power meter can be used to measure the current consumption of SiWx91x NCP module.    

      ![Figure: Current consumption for NCP mode Power Save Standby Example](resources/readme/blepowersavecurrentmeasurementpins.png)     
   - Negative probe of power meter should be connected to J2.1 pin and the positive probe should be connected to J2.2 pin.    
   -  Average current consumption of SiWx91x NCP module as measured with a power-meter.  

      ![NCP current consumption as measured with power meter](resources/readme/blepowersavencpmeasurement.png)

- **SoC**

   - Refer [AEM measurement](https://docs.silabs.com/) section in ***Getting Started with SiWx91x SoC*** guide for measuring current consumption of SiWx91x SoC module. 

      ![SoC current consumption measured using energy profiler](resources/readme/blepwsocadv.png) 

- **PSRAM**

   - Refer [AEM measurement](https://docs.silabs.com/) section in ***Getting Started with SiWx91x PSRAM*** guide for measuring current consumption of SiWx91x PSRAM module.    

      ![PSRAM current consumption measured using energy profiler](resources/readme/blepwsocadv.png) 
 
> **NOTE:** 
> - The measured current may vary if the scenario is performed in open environment. 