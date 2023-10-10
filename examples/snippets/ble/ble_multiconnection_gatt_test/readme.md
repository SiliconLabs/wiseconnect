# BLE Multiconnection GATT Test

## **1 Introduction**

The Application demonstrates how to configure SiWx91x EVK in central/peripheral modes and then connect with multiple peripherals/centrals.

It provides user to configure secure connection with central and peripheral remote devices and support data transfers.

This also displays all the remote GATT profiles and service characteristics logs of connected remote device.

## 2. Prerequisites/Setup Requirements

Before running the application, the user will need the following things to setup.

### 2.1 Hardware Requirements

- Windows PC
- **SoC Mode**:
  - Silicon Labs [BRD4325A, BRD4325B, BRD4325C, BRD4325G, BRD4338A](https://www.silabs.com/)
- **NCP Mode**:
  - Silicon Labs [BRD4180B](https://www.silabs.com/) **AND**
  - Host MCU Eval Kit. This example has been tested with:
    - Silicon Labs [WSTK + EFR32MG21](https://www.silabs.com/development-tools/wireless/efr32xg21-bluetooth-starter-kit)

### 2.2 Software Requirements

- SiWx917_WiSeConnect_SDK.X.X
- Simplicity Studio IDE
  - Download the latest [Simplicity Studio IDE](https://www.silabs.com/developers/simplicity-studio)
  - Follow the [Simplicity Studio user guide](https://docs.silabs.com/simplicity-studio-5-users-guide/1.1.0/ss-5-users-guide-getting-started/install-ss-5-and-software#install-ssv5) to install Simplicity Studio IDE

- Download and install the Silicon Labs [EFR Connect App](https://www.silabs.com/developers/efr-connect-mobile-app) in the android smart phones for testing BLE applications. Users can also use their choice of BLE apps available in Android/iOS smart phones.
- Tera Term software or any other serial terminal software - for viewing application prints

### 2.3 Setup Diagram

#### SoC Mode

![](resources/readme/blemultigattsoc.png)

#### NCP Mode

![](resources/readme/blemultigattncp.png)

**NOTE**:

- The Host MCU platform (EFR32xG21) and the SiWx91x interact with each other through the SPI interface.

Follow the [Getting Started with Wiseconnect3 SDK](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) guide to set up the hardware connections and Simplicity Studio IDE.

## 3 Project Environment

- Ensure the SiWx91x loaded with the latest firmware following the [Upgrade Si91x firmware](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/getting-started-with-soc-mode#upgrade-si-wx91x-connectivity-firmware)

- Ensure the latest Gecko SDK along with the extension WiSeConnect3 is added to Simplicity Studio.

### 3.1 Creating the project

#### 3.1.1 SoC mode

- Ensure the SiWx91x set up is connected to your PC.

- In the Simplicity Studio IDE, the SiWx91x SoC board will be detected under **Debug Adapters** pane as shown below.

  **![Soc Board detection](resources/readme/socboarddetection111.png)**

#### 3.1.2 NCP mode

- Ensure the EFx32 and SiWx91x set up is connected to your PC.

- In the Simplicity Studio IDE, the EFR32 board will be detected under **Debug Adapters** pane as shown below.

  **![EFR32 Board detection](resources/readme/efr32.png)**

### 3.2 Importing the project

- Studio should detect your board. Your board will be shown here. Click on the board detected and go to **EXAMPLE PROJECTS & DEMOS** section 

#### SOC Mode

- Select **BLE - Multiconnection GATT Test** test application

  **![project_selection](resources/readme/create_project1.png)**

- Click 'Create'. The "New Project Wizard" window appears. Click 'Finish'

  **![creation_final](resources/readme/create_project2.png)**

### 3.3 Set up for application prints

#### 3.3.1 Teraterm set up - for BRD4325A, BRD4325B, BRD4325C, BRD4325G

You can use either of the below USB to UART converters for application prints.

1. Set up using USB to UART converter board.

   - Connect Tx (Pin-6) to P27 on WSTK
   - Connect GND (Pin 8 or 10) to GND on WSTK

   **![FTDI_prints](resources/readme/usb_to_uart_1.png)**

2. Set up using USB to UART converter cable.

   - Connect RX (Pin 5) of TTL convertor to P27 on WSTK
   - Connect GND (Pin1) of TTL convertor to GND on WSTK

   **![FTDI_prints](resources/readme/usb_to_uart_2.png)**

3. Open the Teraterm tool.

   - For SoC mode, choose the serial port to which USB to UART converter is connected and click on **OK**.

     **![port_selection_soc](resources/readme/port_selection_soc.png)**

**Note:** For Other 917 SoC boards please refer section #3.3.2

#### 3.3.2 **Teraterm set up - for NCP and SoC modes**

1. Open the Teraterm tool.

- choose the J-Link port and click on **OK**.
    
    **![J-link - NCP](resources/readme/port_selection.png)**

2. Navigate to the Setup → Serial port and update the baud rate to **115200** and click on **OK**.

    **![serial_port_setup](resources/readme/serial_port_setup.png)**

    **![serial_port](resources/readme/serial_port.png)**

## 4 Application Build Environment

The application can be configured to suit your requirements and development environment.

### **4.1 Application configuration**

The application can be configured to suit you requirements and development environment. Go through the following sections and make any changes if needed.

1. In the Project explorer pane of the IDE, expand the **ble_multiconnection_gatt_test** folder and open the **ble_config.h** file. Configure the following parameters based on your requirements.

- **Multi device connection parameters**

   ```
   // RSI_BLE_APP_GATT_TEST refers to the Silicon Labs device name
   #define RSI_BLE_APP_GATT_TEST   (void *)"SI_COEX_MAX_DEMO"

   //RSI_BLE_MAX_NBR_PERIPHERALS refers to the number of remote device peripheral connections
   #define RSI_BLE_MAX_NBR_PERIPHERALS  1

   //RSI_BLE_MAX_NBR_CENTRALS refers to the number of remote device central connections
   #define RSI_BLE_MAX_NBR_CENTRALS   1
   ```

   **Note:** Maximum no. of  RSI_BLE_MAX_NBR_CENTRALS and RSI_BLE_MAX_NBR_PERIPHERALS can be configured to max '1'.
- **Remote Peripheral device parameters**

   ```
   //CONNECT_OPTION refers to the type of connection, valid configurations are CONN_BY_ADDR and CONN_BY_NAME
   #define CONNECT_OPTION CONN_BY_NAME 

   //RSI_REMOTE_DEVICE_NAMEX refers to the remote Peripheral device name
   #define RSI_REMOTE_DEVICE_NAME1 "SILABS_BLE_DEV"
   #define RSI_REMOTE_DEVICE_NAME2 "SILABS_BLE_DEV_1"
   #define RSI_REMOTE_DEVICE_NAME3 "SILABS_BLE_DEV_2"

   //RSI_BLE_DEV_ADDR_TYPE refers to the remote Peripheral device address type
   #define RSI_BLE_DEV_ADDR_TYPE LE_PUBLIC_ADDRESS

   //RSI_BLE_DEV_X_ADDR refers to the remote Peripheral device address
   #define RSI_BLE_DEV_1_ADDR "00:1A:7D:DA:71:16"
   #define RSI_BLE_DEV_2_ADDR "00:1A:7D:DA:71:74"
   #define RSI_BLE_DEV_3_ADDR "00:1A:7D:DA:73:13"

   ```

- **Specific Remote Periperal device configuration paramters**

**Note:** Where XX refers to C1 (or) C2

   ```c
   //set below macro to enable secure connection
   #define SMP_ENABLE_XX 0

   // Add remote device to acceptlist
   #define ADD_TO_ACCEPTLIST_XX 0
   
   // set below macro to discover remote profiles
 #define PROFILE_QUERY_XX 1
 
 // set below macro to perform data transfer
 #define DATA_TRANSFER_XX 1
 
 //configure below macros to select type of data transfer
 // set below macro to receive 'gatt notifications' from remote device
 #define RX_NOTIFICATIONS_FROM_XX 0
 
 // set below macro to receive 'gatt indications' from remote device
 #define RX_INDICATIONS_FROM_XX 0
 
 // set below macro to Transmit 'gatt notifications' to remote device
 #define TX_NOTIFICATIONS_TO_XX 1
 
 // set below macro to Transmit 'gatt write with response' to remote device
 #define TX_WRITES_TO_XX 0
 
 // set below macro to Transmit 'gatt write without response' to remote device
 #define TX_WRITES_NO_RESP_TO_XX 0
 
 // set below macro to Transmit 'gatt indications' to remote device
 #define TX_INDICATIONS_TO_XX 0
 
 // Configure below macro to select data length extension ON/OFF
 #define DLE_ON_XX 0

   // Configure below macros to select connection parameters while data transfer
 #define CONN_INTERVAL_XX            36 //! for conn interval of 45ms
 #define CONN_LATENCY_XX             0
 #define CONN_SUPERVISION_TIMEOUT_XX 400
   ```

- **Specific Remote Central device configuration parameters**

**Note:** Where YY refers to P1 (or) P2 (or) P3

   ```c
   // configure below macro to enable secure connection
 #define SMP_ENABLE_YY 0

 // Add remote device to acceptlist
 #define ADD_TO_ACCEPTLIST_YY 0

 // configure below macro to discover remote profiles
 #define PROFILE_QUERY_YY 1

 // configure below macro to perform data transfer
 #define DATA_TRANSFER_YY 1

 //configure below macros to select type of data transfer
 // set below macro to receive 'gatt notifications' from remote device
 #define RX_NOTIFICATIONS_FROM_YY 1

 // set below macro to receive 'gatt indications' from remote device
 #define RX_INDICATIONS_FROM_YY 0

 // set below macro to Transmit 'gatt notifications' to remote device
 #define TX_NOTIFICATIONS_TO_YY 0

 // set below macro to Transmit 'gatt write with response' to remote device
 #define TX_WRITES_TO_YY 0

 // set below macro to Transmit 'gatt write without response' to remote device
 #define TX_WRITES_NO_RESP_TO_YY 0

 // set below macro to Transmit 'gatt indications' to remote device
 #define TX_INDICATIONS_TO_YY 0

 // Configure below macros to select connection parameters while data transfer
 #define CONN_INTERVAL_YY            240 // for conn interval of 300ms
 #define CONN_LATENCY_YY             0
 #define CONN_SUPERVISION_TIMEOUT_YY 400
   ```

- **Power Save Configuration**

Configure "ENABLE_POWER_SAVE" parameter to enable power save mode.

   ```
      #define ENABLE_POWER_SAVE              1
   ```

   **Note:** If you are using the NCP-EXP-Board, refer the "**Powersave functionality with NCP expansion board**" section  the ***Getting started with SiWx91x NCP*** guide.


**Note:** "Opermode command parameters" are already set with desired configuration in app.c.

### 4.2 Build the Application

- Follow the below steps for the successful execution of the application.

#### Build Project - SoC Mode

- Once the project is created, click on the build icon (hammer) to build the project (or) right click on project and click on Build Project.

- Successful build output will be shown as below.

   ![build_project](resources/readme/build_example.png)

#### Build Project - NCP Mode

- Successful build output will shown as below.

   ![build_project](resources/readme/build_example.png)

## 5 Run and Test the Application

- Once the build was successfull, load the application image.

#### 5.1 Load the Application Image

1. Click on Tools and Simplicity Commander as shown below.

   ![](resources/readme/load_image1.png)

2. Load the application image

- Select the board.
- Browse the application image (.hex) and click on Flash button.

   ![](resources/readme/load_image2.png)

### **5.2 Application Execution Flow**

1. Configure the remote ble devices in peripheral and put it in advertising mode and/or in central and put in scanning mode.

   **Note:** Refer the [Creating New Advertisement Sets](https://docs.silabs.com/bluetooth/5.0/miscellaneous/mobile/efr-connect-mobile-app) for configuring the EFR connect mobile APP as advertiser.

2. Connect any serial console for prints.

3. Silicon Labs device is configured as GATT server with the below user-defined services.

   | S.No |UUID size |main service UUID | Characteristic service UUID | Property|
   |------|---|------|------|-----|
   |1|16-bit|0xAABB | 0x1AA1 | Read, Write & Notify|
   |2|128-bit|0x6A4E3304-667B-11E3-949A-0800200C9A66 | 0x6A4E3304-667B-11E3-949A-0800200C9A66 | Write with no response|
   |3|16-bit| 0x1AA1|0x1BB1 | Read & indicate|

4. Silicon Labs device is in scanning and advertising mode.

5. After the program gets executed, Silicon Labs device tries to connect with the remote device name specified in
   the macro **RSI_REMOTE_DEVICE_NAME1** or **RSI_BLE_DEV_1_ADDR**.

6. Observe that the connection is established between the desired device and Silicon Labs device.

7. When the connection is established with the remote device, the Silicon Labs device fetch the GATT profiles and characteristics and also prints the same on the serial terminal.

8. If user configures SMP pairing in the specified macro **SMP_ENABLE_XX**, after successful connection between Silicon Labs device and remote device flow of commands are as below:

   a. Central device will initiate SMP pairing
  
   b. Peripheral device gives SMP response.

   c. Based on the configured IO capability, the Silicon Labs device tries to establish the SMP connection.

9. When the SMP connection is established successfully, the host receives an SMP encrypt enabled event. If not, Silicon Labs Device sends an SMP failure event to the host.

10. When the Silicon Labs device as a central device, it initiates the scanning and tries to connects to a **remote BLE peripheral device**, which are specified in the `RSI_BLE_DEV_X_ADDR` or `RSI_REMOTE_DEVICE_NAMEX` macros. After the successful connection, the Silicon Labs device initiates the SMP connection too.

11. When the Silicon Labs device as a Peripheral device, it initiates the advertising. The **remote BLE peripheral device** requires to do a scan and initiates the connection. Once the connection is successful, the Silicon Labs device initiates the SMP pairing.

12. To check data transfer, enable Gatt notifications of Silicon Labs device on service characteristic having UUID 0x1AA1, where the Silicon Lab device as a peripheral device.

13. If enabled, Silicon Labs device continuously transmits notifications which can be seen on remote device.

- prints in teraterm looks as shown below.

![](resources/readme/output_1.png)
![](resources/readme/output_2.png)
![](resources/readme/output_3.png)
![](resources/readme/output_4.png)
