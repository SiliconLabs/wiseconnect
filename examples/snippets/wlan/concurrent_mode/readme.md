# Concurrent Mode

## 1 Purpose/Scope

This application demonstrates how to configure the SiWx91x in concurrent mode i.e., in both Wi-Fi Station mode (STA instance) and Access Point mode (SoftAP instance).
In this example application, the SiWx91x's STA instance gets connected to a wireless Access Point and gets an IP address.

## 2 Prerequisites/Setup Requirements

### 2.1 Hardware Requirements

- Windows PC
- Wireless Access Point
- **SoC Mode**: 
  - Silicon Labs [BRD4325A,BRD4325B, BRD4325G](https://www.silabs.com/)
- **NCP Mode**: 
  - Silicon Labs [(BRD4180A, BRD4280B)](https://www.silabs.com/); **AND**
  - Host MCU Eval Kit. This example has been tested with:
    - Silicon Labs [WSTK + EFR32MG21](https://www.silabs.com/development-tools/wireless/efr32xg21-bluetooth-starter-kit)

### 2.2 Software Requirements

- Simplicity Studio IDE 
  - Download the [Simplicity Studio IDE](https://www.silabs.com/developers/simplicity-studio).
  - Follow the [Simplicity Studio user guide](https://docs.silabs.com/simplicity-studio-5-users-guide/1.1.0/ss-5-users-guide-getting-started/install-ss-5-and-software#install-ssv5) to install Simplicity Studio IDE.

### 2.3 Set up Diagram

#### SoC Mode 

Set up diagram for SoC mode:

![Figure: Setup Diagram SoC Mode for Access point Example](resources/readme/concurrentsoc.png)

Follow the [Getting Started with SiWx91x SoC](https://docs.silabs.com/) guide to set up the hardware connections and Simplicity Studio IDE.

#### NCP Mode  

Set up diagram for NCP mode:

![Figure: Setup Diagram SoC Mode for Access point Example](resources/readme/concurrentncp.png)

Follow the [Getting Started with EFx32](https://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-getting-started-with-efx32/) guide to setup the hardware connections and Simplicity Studio IDE.

**NOTE**: 

- The Host MCU platform (EFR32MG21) and the SiWx91x interact with each other through the SPI interface. 

## 3 Project Environment

1. Ensure the SiWx91x loaded with the latest firmware following the [Getting started with a PC](https://docs.silabs.com/rs9116/latest/wiseconnect-getting-started)

2. Ensure the EFx32 and SiWx91x set up is connected to your PC.

3. Ensure the latest Gecko SDK along with the extension WiSeConnect3 is added to Simplicity Studio.

### 3.1 Creating the project

#### 3.1.1 SoC mode

- In the Simplicity Studio IDE, the SiWx91x SoC board will be detected under Debug Adapters pane as shown below.

   **![Soc Board detection](resources/readme/soc_board_detection.png)**

- Click on the board detected and go to **EXAMPLE PROJECTS & DEMOS** section.
   
   **![Examples and Demos](resources/readme/examples_demos.png)**

- Select Wi-Fi - SoC Concurrent Wi-Fi client and SoftAP

   **![Concurrent Mode project](resources/readme/example_soc.png)**

#### 3.1.2 NCP mode

- In the Simplicity Studio IDE, the EFR32 board will be detected under Debug Adapters pane as shown below.

   **![Soc Board detection](resources/readme/efr32.png)**

- Now choose **Wi-Fi- NCP Concurrent Wi-Fi client and SoftAP** example for NCP mode or choose **Wi-Fi- SoC Concurrent Wi-Fi client and SoftAP** example for SoC mode and click on **Create**.
   
   **![Concurrent Mode project](resources/readme/concurrent_example_ncp.png)** 

- Give the desired name to your project and cick on **Finish**.

   **![Concurrent Mode project](resources/readme/create_project.png)**

### 3.3 Set up for application prints

**SoC mode**: 
You can use either of the below USB to UART converters for application prints.

1. Set up using USB to UART converter board.

   - Connect Tx (Pin-6) to P27 on WSTK
   - Connect GND (Pin 8 or 10) to GND on WSTK

    **![FTDI_prints](resources/readme/usb_to_uart_1.png)**

2. Set up using USB to UART converter cable.

   - Connect RX (Pin 5) of TTL convertor to P27 on WSTK
   - Connect GND (Pin1) of TTL convertor to GND on WSTK

    **![FTDI_prints](resources/readme/usb_to_uart_2.png)**

**Tera Term set up - for NCP and SoC modes**

1. Open the Tera Term tool.

 - For SoC mode, choose the serial port to which USB to UART converter is connected and click on **OK**. 
  
    **![UART - SoC](resources/readme/port_selection_soc.png)**

 - For NCP mode, choose the J-Link port and click on **OK**.

      **![J-link - NCP](resources/readme/port_selection.png)**

2. Navigate to the Setup → Serial port and update the baud rate to **115200** and click on **OK**.

 **![Serial port](resources/readme/serial_port_setup.png)**

 **![Baud rate](resources/readme/baud_rate.png)**

## 4 Application Build Environment

### 4.1 Configure the Application

The application can be configured to suit user requirements and development environment. Read through the following sections and make any changes needed.

#### 4.1.1 In the Project explorer pane, open the **app.c** file. Configure the following parameters based on your requirements

- **STA instance related parameters**

- WIFI_CLIENT_PROFILE_SSID refers to the name with which the SiWx91x SoftAP's Wi-Fi network shall be advertised.

  ```c
  #define WIFI_CLIENT_PROFILE_SSID               "YOUR_AP_SSID"
  ```

- DEFAULT_WIFI_CLIENT_CREDENTIAL refers to the secret key if the Access point is configured in WPA-PSK/WPA2-PSK security modes.

  ```c
  #define WIFI_CLIENT_CREDENTIAL                 "YOUR_AP_PASSPHRASE"
  ```

- Other STA instance configurations can be modified if required in `wifi_client_profile` configuration structure.

- **AP instance related parameters**

- WIFI_AP_PROFILE_SSID refers to the name of the WiSeConnect Access point would be created.

  ```c
  #define WIFI_AP_PROFILE_SSID                   "MY_AP_SSID"
  ```

- WIFI_AP_CREDENTIAL refers to the secret key of the WiSeConnect Access point would be created.

  ```c
  #define WIFI_AP_CREDENTIAL                     "MY_AP_PASSPHRASE"
    ```

> Note: 
>
> 1. In concurrent mode, STA and AP should be configured on the same channel. The STA instance shall first scan for the specified external AP, wherein the channel number of AP is fetched and passed as an argument during SoftAP creation.
> 2. Valid values for CHANNEL_NO are 1 to 11 in 2.4GHz band and 36 to 48 & 149 to 165 in 5GHz. In this example, default configured band is 2.4GHz.

- Other STA instance configurations can be modified if required in `default_wifi_ap_profile` configuration structure.

> Note:
>
> 1. This application is not providing the facility to configure the Access Point’s IP Parameters. Default IP address of the Silicon Labs Access point is **192.168.100.76**
> 2. In concurrent mode, the IP networks of Silicon Labs STA and Silicon Labs Access Point both should be different. Configure Wireless Access Point IP network(Ex: 192.168.0.1) other than Silicon Labs Access point IP network.

#### 4.1.2 Open **sl_wifi_device.h** file. User can also refer the `sl_wifi_default_concurrent_configuration` and can modify/create configurations as per their needs and requirements
>
> Note:
>
> - In `sl_wifi_default_concurrent_configuration`, `oper_mode` must be `SL_SI91X_CONCURRENT_MODE` for this example.

### 4.2 Build the Application

#### - SoC Mode : Build as Concurrent_mode Example

   **![Build as ](resources/readme/build_concurrent.png)**

#### - NCP Mode

### 4.3 Run and Test the application

1. Once the build was successful, right click on project and select **Debug As → Silicon Labs ARM Program** to program the device as shown in below image.

   **![debug_mode_NCP](resources/readme/program_device.png)**

2. As soon as the debug process is completed, the application control branches to the main().

3. Click on the **Resume** icon in the Simplicity Studio IDE toolbar to run the application.

   **![Run](resources/readme/run.png)**

4. After the program gets executed, SiWx91x connects to the access point as a Station. On other hand, SiWx91x advertizes itself as an access point with provided configurations mentioned above.

### 4.4 Application Output 

- SoC mode:

   **![Application prints](resources/readme/output_soc.png)**

- NCP mode:

   **![Application prints](resources/readme/application_prints_ncp.png)**
