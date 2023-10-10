# WLAN HTTPS BLE Dual Role

## 1. Purpose / Scope

This example demonstrates the ability of Si91x EVK connecting and transferring the data simultaneously in BLE/WiFi wireless interfaces.

In the Coex mode (WiFi+BLE), the module will connect to AP and then download the fixed file from PC acting as Server and supports BLE connection/data transfers simultaneously.

The Application can be configured for the individual protocol execution as well the combination of protocols execution ( WiFi+BLE). Two BLE connections (Master and Slave) are supported.

## 2. Prerequisites / Setup Requirements

Before running the application, the user will need the following things to setup.

### 2.1 Hardware Requirements

- Windows PC with Host interface(UART/ SPI) in case of WiSeConnect.
 **SoC Mode**:
  - Silicon Labs [BRD4325A, BRD4325B, BRD4325C, BRD4325G, BRD4338A](https://www.silabs.com/)
- **NCP Mode**:
  - Silicon Labs [BRD4180B](https://www.silabs.com/) **AND**
  - Host MCU Eval Kit. This example has been tested with:
    - Silicon Labs [WSTK + EFR32MG21](https://www.silabs.com/development-tools/wireless/efr32xg21-bluetooth-starter-kit)
- Wireless Access Point
- Smart phone/tablet with BLE Application (Ex: )
- Windows PC2 (Remote PC) with HTTP/HTTPS server running.

  ![Setup Diagram for WLAN HTTP/HTTPs  BLE Dual Role Example](resources/readme/image300.png)

Follow the [Getting Started with Wiseconnect3 SDK](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) guide to set up the hardware connections and Simplicity Studio IDE.

### 2.2 Software Requirements

SiWx917_WiSeConnect_SDK.X.X

- Simplicity Studio IDE
  - Download the latest [Simplicity Studio IDE](https://www.silabs.com/developers/simplicity-studio)
  - Follow the [Simplicity Studio user guide](https://docs.silabs.com/simplicity-studio-5-users-guide/1.1.0/ss-5-users-guide-getting-started/install-ss-5-and-software#install-ssv5) to install Simplicity Studio IDE

- Download and install the Silicon Labs [EFR Connect App](https://www.silabs.com/developers/efr-connect-mobile-app) in the android smart phones for testing BLE applications. Users can also use their choice of BLE apps available in Android/iOS smart phones.
- Tera Term software or any other serial terminal software - for viewing application prints

## 3 Project Environment

- Ensure the SiWx91x loaded with the latest firmware following the [Upgrade Si91x firmware](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/getting-started-with-soc-mode#upgrade-si-wx91x-connectivity-firmware)

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

- Select **Wi-Fi Coex - Wi-Fi Client HTTPS BLE Dual Role** test application

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

## 4. Application Configuration Parameters

The application can be configured to suit your requirements and development environment. Read through the following sections and make any changes needed.

**4.1** Open `app_common_config.h` file and configure below macros.

Set below macro to 1 to run **BLE** application

```c
#define RSI_ENABLE_BLE_TEST                          1 //Set this to 0 to disable BLE
```

Set below macro to 1 to run **WLAN** application

```c
#define RSI_ENABLE_WLAN_TEST                         1 //Set this to 0 to disable WLAN
```

**4.2** Select **BLE** configurations in `rsi_ble_common_config.h` file

To select number of BLE connections, configure below macros.

   Set below macro to required slave connections.

```c
#define RSI_BLE_MAX_NBR_PERIPHERALS                       1
```

   Set below macro to required master connections.

```c
#define RSI_BLE_MAX_NBR_CENTRALS                      1
```

> Note:
> Maximum no. of  RSI_BLE_MAX_NBR_CENTRALS can be configured to '1' and RSI_BLE_MAX_NBR_PERIPHERALS to '1'.

**4.3** Select **BLE** configurations in `ble_config.h` file

If CONNECT_OPTION is set to CONN_BY_NAME, configure below macros.

```c
#define CONNECT_OPTION                               CONN_BY_NAME //CONN_BY_NAME or CONN_BY_ADDR
```

To identify remote device with BD Address/device name.

   Add the remote BLE device name to connect

```c
#define RSI_REMOTE_DEVICE_NAME1                      "slave1"
#define RSI_REMOTE_DEVICE_NAME2                      "slave2"
#define RSI_REMOTE_DEVICE_NAME3                      "slave3"
```

If CONNECT_OPTION is set to CONN_BY_ADDR, configure the below macros.

Configure the address type of remote device as either Public Address or Random Address

```c
#define RSI_BLE_DEV_ADDR_TYPE                        LE_PUBLIC_ADDRESS //!LE_PUBLIC_ADDRESS or LE_RANDOM_ADDRESS
```

Add the BD Address of remote BLE device to connect

```c
#define RSI_BLE_DEV_1_ADDR                           "88:DA:1A:FE:2A:2C"
#define RSI_BLE_DEV_2_ADDR                           "7E:E6:5E:30:77:6F"
#define RSI_BLE_DEV_3_ADDR                           "70:1A:69:32:7C:8E
```

Configure below macros to select the profile characteristics uuid for data transfer.

```c
#define RSI_BLE_CLIENT_WRITE_SERVICE_UUID_M1             0x180D //! Heart Rate service uuid
#define RSI_BLE_CLIENT_WRITE_CHAR_UUID_M1                0x2A39 //! Heart Rate control Point
#define RSI_BLE_CLIENT_WRITE_NO_RESP_SERVICE_UUID_M1     0x1802 //! Immediate Alert service uuid
#define RSI_BLE_CLIENT_WRITE_NO_RESP_CHAR_UUID_M1        0x2A06 //! Alert level char uuid
#define RSI_BLE_CLIENT_INIDCATIONS_SERVICE_UUID_M1       0x1809 //! Health thermometer Alert service uuid
#define RSI_BLE_CLIENT_INIDCATIONS_CHAR_UUID_M1          0x2A1C //! Temperature measurement
#define RSI_BLE_CLIENT_NOTIFICATIONS_SERVICE_UUID_M1     0x180D //! Heart Rate service uuid
#define RSI_BLE_CLIENT_NOTIFICATIONS_CHAR_UUID_M1        0x2A37 //! Heart Rate measurement 
```

Configure below macros to select each connection configurations

   `Master1 configurations: (where XX=C1)`

Set below macro to enable secure connection between Silicon Labs device(peripheral) and remote ble device(central)

```c
#define SMP_ENABLE_XX               0  //By default, this macro is set to '0' 
```

Set below macro to add remote device to acceptlist

```c
#define ADD_TO_ACCEPTLIST_XX         0  //By default, this macro is set to '0' 
```

Set below macro to discover remote profiles.

```c
#define PROFILE_QUERY_XX            1  //By default, this macro is set to '1'
```

Set below macro to enable data transfer between devices

```c
#define DATA_TRANSFER_XX            1  //By default, this macro is set to '1'
```

To select the type of data transfer configure below macros

   Set below macro to receive 'gatt notifications' from remote device.

```c
#define RX_NOTIFICATIONS_FROM_XX    0  //By default, this macro is set to '1'
```

> Note:
> Make sure to set below macros to 0.

Set below macro to receive 'gatt indications' from remote device.

```c
#define RX_INDICATIONS_FROM_XX      0  //By default, this macro is set to '0'
```

Set below macro to Transmit 'gatt notifications' to remote device.

```c
#define TX_NOTIFICATIONS_TO_XX      1  //By default, this macro is set to '1'
```

> Note:
> Make sure to set below macros to 0

```c
#define  TX_WRITES_TO_XX            0  //Set this to 0 
#define  TX_WRITES_NO_RESP_TO_XX    0  //Set this to 0 
#define  TX_INDICATIONS_TO_XX       0  //Set this to 0 
```

Set below macro to Transmit 'gatt write with response' to remote device.

```c
#define TX_WRITES_TO_XX             0  //By default, this macro is set to '0' 
```

Set below macro to Transmit 'gatt write without response' to remote device.

```c
#define TX_WRITES_NO_RESP_TO_XX     0  //By default, this macro is set to '0' 
```

Set below macro to Transmit 'gatt indications to remote device.

```c
#define TX_INDICATIONS_TO_XX        0  //By default, this macro is set to '0' 
```

To select data length extension for each connection configure below macro

   Set below macro to enable data length extension

```c
#define DLE_ON_XX                   0  //By default, this macro is set to '0' 
```

Configure below macros to set connection interval, connection latency and connection supervision timeout.

Below configuration is for connection interval of 45ms, latency 0 and timeout as 400ms

```c
#define CONN_INTERVAL_XX            36
#define CONN_LATENCY_XX             0
#define CONN_SUPERVISION_TIMEOUT_XX  400 
```

> Note:
> Follow the above instructions to configure for remaining connections (slave1(XX = P1), slave2 (XX =P2), slave3(XX=P3) and master2(XX=C2))

**4.4** Select WLAN configurations in `wifi_app_config.h` file

Enter the AP Connectivity essential configs as the value to SSID, SECURITY_TYPE and PSK

```c
#define     SSID                    "SILABS_AP"
#define     SECURITY_TYPE           SL_WIFI_WPA2 
#define     PSK                     "1234567890"
```

Configure below macro to make Use of Local HTTP server to download the files.

```c
#define   HTTPS_DOWNLOAD            0           // set to '0' to choose HTTP download
#define   SERVER_PORT               80          // by default http runs on port 80
#define   SERVER_IP_ADDRESS         "192.168.0.103"   // Local server ip address
#define   DOWNLOAD_FILENAME         "5MB.txt"   // File to download
#define   CONTINUOUS_HTTP_DOWNLOAD  1           // set to '1' to download continuously, if reset download happens only once.
```

Configure below macros to make Use of Local HTTPS server to download the files.

```c
#define   HTTPS_DOWNLOAD            1           // set to '1' to choose HTTPs download
#define   SERVER_PORT               443         // by default https runs on port 443
#define   SERVER_IP_ADDRESS         "192.168.0.101"   // Local server ip address
#define   DOWNLOAD_FILENAME         "5MB.txt"   // File to download, by default this file is provided in the demo
#define   CONTINUOUS_HTTP_DOWNLOAD  1           // set to '1' to download continuously, if reset download happens only once.
```

> Note:
> BY default, when 'HTTPS_DOWNLOAD' is set, SSL and LOAD_CERTIFICATE will be set to '1' as it is required for HTTPS download.

### 4.2 Build the application

- SoC mode:  Build as  Https Ble Dual Role  Example

    **![Build as](resources/readme/htttp_dualrole_build.png)**

- NCP mode:

### 4.3 Run and Test the application

- Once the build was successful, right click on project and click on Debug As->Silicon Labs ARM Program as shown in below image.

  - SoC

    ![debug_mode_soc](resources/readme/htttp_dualrole_run.png)

  - NCP

    ![debug_mode_NCP](resources/readme/htttp_dualrole_run.png)

### 5.3 Common Steps

1. Compile the project and flash the binary.

2. Before running below commands, make sure the file configured in 'DOWNLOAD_FILENAME' is present under below path

   [File path: `/<SDK>/resources/scripts/`]

3. To download the files from local http server, navigate to below folder and run below command.

   [File path: `/<SDK>/resources/scripts/`]

   python simple_http_server.py 80

4. To download the files from local https server, copy ssl certificates 'server-cert.pem' , 'server-key.pem' from below 'source path' and paste in to 'destination path'.

   [source path: `/<SDK>/resources/certificates/`]

   [destination path: `/<SDK>/resources/scripts/`]

     Open command prompt, navigate to above destination path and run below command.

   openssl s_server -accept 443 -cert server-cert.pem -key server-key.pem -tls1_2 -WWW

5. After the program gets executed, module scans for the configured Access point, connects to it and acquires the ip address

6. After acquiring ip address, initiates connection to remote server.(ex: simple_http_server.py running in same network where Module is also connected)

7. If connection is successful,

   - Si917 EVK starts advertising and scanning BLE

   - Advertises BLE and simultaneously downloads http packets sent from remote server

8. If connection is not successful, step 5 is repeated until connection is success

9. While downloading is happening, user can initiate  BLE connections (both peripheral and central).

10. To check BLE peripheral connection, scan and initiate connection from nRF connect/dongles.

11. Si91x EVK accepts the BLE connections if initiated by remote BLE device(max 2 peripheral connections are accepted) and starts data transfer based on the user configuration.

12. To check data transfer, enable Gatt notifications of Module on service characteristic RSI_BLE_ATTRIBUTE_1_UUID

13. If enabled Si91x EVK continuously transmits 20 notifications per connection interval of size 20bytes.

14. To check BLE central connection, advertise the remote ble devices using phone/dongles.

15. Module scans for advertised devices, crosschecks the ble device names/ble device address as configured in application, if matches initiate connection.

16. If BLE connection is successful, Si91x EVK enables the Gatt notifications of remote device for RSI_BLE_CLIENT_NOTIFICATIONS_CHAR_UUID_M1 (Heart Rate measurement) and receives notifications/connection interval.

> Note:
> Steps 9 to 12 can be repeated for 2 peripheral connection and steps 13 to 15 can be repeated for 3 central connections based on the RSI_BLE_MAX_NBR_CENTRALS and RSI_BLE_MAX_NBR_PERIPHERALS. Verify that all connections are stable and simultaneous data transfer is happening from all the radios of RS9116W EVK.
