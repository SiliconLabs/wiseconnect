# Simple SMP

## 1 Purpose / Scope

This application demonstrates how to configure Silicon Labs device in Central mode , connect with remote slave device and how to enable SMP(Security Manager Protocol) pairing.

## 2 Prerequisites / Setup Requirements

### 2.1 Hardware Requirements

- Windows PC.
- SiWx91x Wi-Fi Evaluation Kit. The SiWx91x supports multiple operating modes. See [Operating Modes]() for details.
- **SoC Mode**: 
  - Silicon Labs [BRD4325A, BRD4325B, BRD4325G](https://www.silabs.com/)
- **NCP Mode**:
  - Silicon Labs [(BRD4180A, BRD4280B)](https://www.silabs.com/); **AND**
  - Host MCU Eval Kit. This example has been tested with:
  - Silicon Labs [WSTK + EFR32MG21](https://www.silabs.com/development-tools/wireless/efr32xg21-bluetooth-starter-kit)
- BLE peripheral device

### 2.2 Software Requirements
    
- Embedded Development Environment
- Simplicity Studio IDE 
  - Download the [Simplicity Studio IDE](https://www.silabs.com/developers/simplicity-studio).
  - Follow the [Simplicity Studio user guide](https://docs.silabs.com/simplicity-studio-5-users-guide/1.1.0/ss-5-users-guide-getting-started/install-ss-5-and-software#install-ssv5) to install Simplicity Studio IDE.
- Download and install the Silicon Labs [EFR Connect App](https://www.silabs.com/developers/efr-connect-mobile-app) in the android smart phones for testing BLE applications. Users can also use their choice of BLE apps available in Android/iOS smart phones.

### 2.3 Setup Diagram
   
**SoC Mode :**

![Figure: Setup Diagram SoC Mode for Simple SMP Example](resources/readme/simplesmpsoc.png)

Follow the [Getting Started with SiWx91x SoC](https://docs.silabs.com/) guide to setup the hardware connections and Simplicity Studio IDE.
  
**NCP Mode :**  

![Figure: Setup Diagram NCP Mode for Simple SMP Example](resources/readme/simplesmpncp.png) 

Follow the [Getting Started with EFx32](https://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-getting-started-with-efx32/) guide to setup the hardware connections and Simplicity Studio IDE.
 
## 3. Project Environment

### 3.1 Project Creation

#### 3.1.1 SoC Mode

1. Ensure the SiWx91x setup is connected to your PC.

- In the Simplicity Studio IDE, the SiWx91x SoC board will be detected under **Debug Adapters** pane as shown below.

   ![Soc Board detection](resources/readme/socboarddetection111.png)
   
- Studio should detect your board. Your board will be shown here. Click on the board detected and go to **EXAMPLE PROJECTS & DEMOS** section.  

- Filter for Bluetooth examples from the Gecko SDK added. For this, check the *Bluetooth* checkbox under **Wireless Technology** and select *BLE - Simple SMP* application.

   ![projct_selection](resources/readme/create_project1.png)

- Click 'Create'. The "New Project Wizard" window appears. Click 'Finish'

  ![creation_final](resources/readme/create_project2.png)

#### 3.1.2 NCP Mode

1. Ensure the EFx32 and SiWx91x setup is connected to your PC.

- In the Simplicity Studio IDE, the EFR32 board will be detected under **Debug Adapters** pane as shown below.
   
   ![EFR32 Board detection](resources/readme/efr32.png)

- Ensure the latest Gecko SDK along with the WiSeConnect 3 extension is added to Simplicity Studio.

- Go to the 'EXAMPLE PROJECT & DEMOS' tab and select *BLE - Simple SMP* application.

- Click 'Create'. The "New Project Wizard" window appears. Click 'Finish'.

  ![creation_final](resources/readme/create_project2.png)
  
### 3.2 Setup for Application Prints

#### 3.2.1 SoC Mode

  You can use either of the below USB to UART converters for application prints.

1. Setup using USB to UART converter board.

 - Connect Tx (Pin-6) to P27 on WSTK
   - Connect GND (Pin 8 or 10) to GND on WSTK

   **![FTDI_prints](resources/readme/usb_to_uart_1.png)**

2. Setup using USB to UART converter cable.

 - Connect RX (Pin 5) of TTL convertor to P27 on WSTK
   - Connect GND (Pin1) of TTL convertor to GND on WSTK
    
   **![FTDI_prints](resources/readme/usb_to_uart_2.png)**

**Tera Term setup - for NCP and SoC modes**

1. Open the Tera Term tool.

 - For SoC mode, choose the serial port to which USB to UART converter is connected and click on **OK**. 

   **![UART - SoC](resources/readme/port_selection_soc.png)**

 - For NCP mode, choose the J-Link port and click on **OK**.
    
   **![J-link - NCP](resources/readme/port_selection.png)**

2. Navigate to the Setup → Serial port and update the baud rate to **115200** and click on **OK**.

  **![Serial port](resources/readme/serial_port_setup.png)**
 
  **![Baud rate](resources/readme/serial_port.png)**

## 4 Application Build Environment

The application can be configured to suit your requirements and development environment. Read through the following sections and make any changes needed. 

### 4.1 Configure the application

**4.1.1** Open `ble_smp.c` file

**4.1.1.1** User must update the below parameters 

         #define MASTER                                            1
         #define SLAVE                                             0
         #define ROLE                                              SLAVE

`RSI_BLE_LOCAL_NAME` refers the name of the Silicon Labs device to appear during scanning by remote devices.

         #define RSI_BLE_DEVICE_NAME                              "BLE_SIMPLE_SMP"

`RSI_BLE_DEV_ADDR_TYPE` refers address type of the remote device to connect.

         #define RSI_BLE_DEV_ADDR_TYPE                             LE_PUBLIC_ADDRESS

**Note:** Depending on the remote device, address type will change.

Valid configurations are

         LE_RANDOM_ADDRESS
         LE_PUBLIC_ADDRESS
`RSI_BLE_DEV_ADDR` refers address of the remote device to connect.

         #define RSI_BLE_DEV_ADDR                                  "00:1A:7D:DA:71:13" 

`RSI_REMOTE_DEVICE_NAME` refers the name of remote device to which Silicon Labs device has to connect

         #define RSI_REMOTE_DEVICE_NAME                            "REDPINE_DEV"

**Note:** Silicon Labs module can connect to remote device by referring either `RSI_BLE_DEV_ADDR` or `RSI_REMOTE_DEVICE_NAME` of the remote device.

`RSI_BLE_SMP_IO_CAPABILITY` refers IO capability.

         #define RSI_BLE_SMP_IO_CAPABILITY                         0x03
`RSI_BLE_SMP_PASSKEY` refers address type of the remote device to connect.

         #define RSI_BLE_SMP_PASSKEY                               0

**4.1.1.2** Following are the non-configurable macros in the application.
         #define RSI_BLE_CONN_EVENT                                0x01
         #define RSI_BLE_DISCONN_EVENT                             0x02
         #define RSI_BLE_SMP_REQ_EVENT                             0x03
         #define RSI_BLE_SMP_RESP_EVENT                            0x04
         #define RSI_BLE_SMP_PASSKEY_EVENT                         0x05
         #define RSI_BLE_SMP_FAILED_EVENT                          0x06
         #define RSI_BLE_ENCRYPT_STARTED_EVENT                     0x07

`BT_GLOBAL_BUFF_LEN` refers Number of bytes required by the application and the driver

         #define BT_GLOBAL_BUFF_LEN                                15000

**4.2.1** Open ble_config.h file and update/modify following macros,

         #define CONCURRENT_MODE                                   RSI_DISABLE
         #define RSI_FEATURE_BIT_MAP                               FEAT_SECURITY_OPEN
         #define RSI_TCP_IP_BYPASS                                 RSI_DISABLE
         #define RSI_TCP_IP_FEATURE_BIT_MAP                        TCP_IP_FEAT_DHCPV4_CLIENT 
         #define RSI_CUSTOM_FEATURE_BIT_MAP                        FEAT_CUSTOM_FEAT_EXTENTION_VALID
         #define RSI_EXT_CUSTOM_FEATURE_BIT_MAP                    EXT_FEAT_384K_MODE
         #define RSI_BAND                                          RSI_BAND_2P4GHZ

         #define RSI_BLE_PWR_INX                                   30
         #define RSI_BLE_PWR_SAVE_OPTIONS                          0

**Note:** ble_config.h file is already set with desired configuration in respective example folders user need not change for each example.

## 5. Build and Test the Application

- Follow the below steps for the successful execution of the application.

### 5.1 Build the Application

- Follow the below steps for the successful execution of the application.

#### SoC Mode

- Once the project is created, click on the build icon (hammer) to build the project (or) right click on project and click on Build Project.

   ![build_project](resources/readme/build_example.png)
   
- Successful build output will show as below.
   
#### NCP Mode

   ![build_project](resources/readme/build_example.png)
   
- Successful build output will show as below.

### 5.2 Loading the Application Image

1. Click on Tools and Simplicity Commander as shown below.

   ![](resources/readme/load_image1.png)
   
2. Load the firmware image
 - Select the board. 
 - Browse the application image (.hex) and click on Flash button.

   ![](resources/readme/load_image2.png)

### 5.3 Common Steps

Following the execution process in case of Silicon Labs device as master

1. If user select the MASTER role, After the program gets executed, Silicon Labs device will be trying to connect to remote BT device, advertise the third party device.
If user select the SLAVE role, After the program gets executed, Silicon Labs device will be advertising, then connect from remote device.
2. After successful connection, flow of commands is as below:
Master device will initiate SMP pairing
Slave device give SMP response.
Both devices will exchange SMP passkey as zero
3. If SMP succeed, host receives SMP encrypt enabled event. If not success, Device sends SMP failure event to host.
4. In encryption enabled event LocalEDIV, Local Rand, LocalLTK parameters will be indicated.
5. Again, after disconnection, if Master want to connect, master ask for LE LTK Request event to slave by giving LocalEDIV and LocalRand, and if same, this example give LocalLTK with positive reply using ltk request reply command.

**Note:**
We can also send negative reply but remote device may or may not initiate pairing again.
Currently, in encryption enabled event EDIV, RAND, LTK are of local device so that if master initiate connection he will ask for LTK request by giving slave's (in this example) EDIV and RAND.
