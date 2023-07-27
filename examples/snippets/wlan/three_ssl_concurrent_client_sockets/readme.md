# Three SSL Client Sockets 

## 1 Purpose/Scope

This application demonstrates how SiWx91x will connect to three different SSL servers with three different set of SSL certificates and loading certificates into the FLASH.

## 2 Prerequisites/Setup Requirements

### 2.1 Hardware Requirements 

- Windows PC
- AWS server information like domain name running in the cloud which supports SSL connection.
- Wireless Access Point
- TCP server over SSL running in Windows PC (This application uses OpenSSL to create TCP server over SSL)
- SiWx91x Wi-Fi Evaluation Kit
- **SoC Mode**: 
  - Silicon Labs [(BRD4325A,BRD4325B, BRD4325G)](https://www.silabs.com/)
- **NCP Mode**:
  - Silicon Labs [(BRD4180A, BRD4280B)](https://www.silabs.com/)
  - Host MCU Eval Kit. This example has been tested with:
    - Silicon Labs [WSTK + EFR32MG21](https://www.silabs.com/development-tools/wireless/efr32xg21-bluetooth-starter-kit)

### 2.2 Software Requirements

- Simplicity Studio IDE 

  - Download the latest [Simplicity Studio IDE](https://www.silabs.com/developers/simplicity-studio)
  - Follow the [Simplicity Studio user guide](https://docs.silabs.com/simplicity-studio-5-users-guide/1.1.0/ss-5-users-guide-getting-started/install-ss-5-and-software#install-ssv5) to install Simplicity Studio IDE

### 2.3 Setup Diagram

#### SoC Mode 

![Figure: Setup Diagram SoC Mode for three_ssl_client_sockets Example](resources/readme/threesslsocketsoc.png)

Follow the [Getting Started with SiWx91x SoC](https://docs.silabs.com/) guide to set up the hardware connections and Simplicity Studio IDE.
  
#### NCP Mode  

![Figure: Setup Diagram NCP Mode for three_ssl_client_sockets Example](resources/readme/threesslsocketncp.png)

Follow the [Getting Started with SiWx91x SoC](https://docs.silabs.com/) guide to set up the hardware connections and Simplicity Studio IDE.

### 3 Project Environment

- Ensure the SiWx91x loaded with the latest firmware following the [Getting started with a PC](https://docs.silabs.com/rs9116/latest/wiseconnect-getting-started)

### 3.1 Creating the project

#### 3.1.1 SoC mode

- In the Simplicity Studio IDE, the SiWx91x SoC board will be detected under **Debug Adapters** pane as shown below.

    **![Soc Board detection](resources/readme/socboarddetection.png)**

- Ensure the latest Gecko SDK along with the  WiSeConnect3 extension is added to Simplicity Studio.

- Studio should detect your board. Your board will be shown here. Click on the board detected and go to **EXAMPLE PROJECTS & DEMOS** section.

- Filter for Wi-Fi examples from the Gecko SDK added. For this, check the *Wi-Fi* checkbox under **Wireless Technology**

    **![Multiple TLS project](resources/readme/three_ssl.png)**

- Click 'Create'. The "New Project Wizard" window appears. Click 'Finish'

    **![Create Multiple TLS project](resources/readme/create_project.png)**

#### 3.1.2 NCP mode

- In the Simplicity Studio IDE, the EFR32 board will be detected under **Debug Adapters** pane as shown below.

  **![EFR32 Board detection](resources/readme/efr32.png)**

- Ensure the latest Gecko SDK along with the  WiSeConnect3 extension is added to  Simplicity Studio.

- Go to the 'EXAMPLE PROJECT & DEMOS' tab and select Wi-Fi - Three TLS Concurrent Client Sockets example.

  **![Multiple TLS project](resources/readme/three_ssl_ncp.png)**

- Click 'Create'. The "New Project Wizard" window appears. Click 'Finish'

   **![Create Multiple TLS project](resources/readme/create_project.png)**

### 3.2 Set up for application prints

#### 3.2.1 SoC mode

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

  **![Baud rate](resources/readme/serial_port.png)**

## 4 Application Build Environment

The application can be configured to suit user requirements and development environment.
Read through the following sections and make any changes needed. 

### 4.1 Configure the application  

The application can be configured to suit user requirements and development environment. Read through the following sections and make any changes needed.

#### 4.1.1 In the Project explorer pane, expand the **config** folder and open the **sl_net_default_values.h** file. Configure the following parameters to enable your Silicon Labs Wi-Fi device to connect to your Wi-Fi network

**STA instance related parameters**

- DEFAULT_WIFI_CLIENT_PROFILE_SSID refers to the name with which the SiWx91x SoftAP's Wi-Fi network shall be advertised.

  ```c
  #define DEFAULT_WIFI_CLIENT_PROFILE_SSID               "YOUR_AP_SSID"      
  ```

- DEFAULT_WIFI_CLIENT_CREDENTIAL refers to the secret key if the Access point is configured in WPA-PSK/WPA2-PSK security modes.

  ```c 
  #define DEFAULT_WIFI_CLIENT_CREDENTIAL                 "YOUR_AP_PASSPHRASE" 
  ```

- Other STA instance configurations can be modified if required in `default_wifi_client_profile` configuration structure.

2. Configure the following parameters in **app.c** to test three_ssl_client_sockets app as per requirements  

```c
   
   #define SERVER_PORT1   <remote port>       // Remote server port
   #define SERVER_PORT2   <remote port>       // Remote server port cloud.
   #define SERVER_PORT3   <remote port>       //  remote peer/ which is running on cloud.   
   #define SERVER_ADDR    "192.168.1.108"     // Remote server IP address
```

If certificates are not there in flash, then ssl handshake will fail.
  
AWS_DOMAIN_NAME refers to domain name of the AWS server

```c   
   #define AWS_DOMAIN_NAME   "a25jwtlmds8eip-ats.iot.us-east-2.amazonaws.com"
```

### 4.2 Build the application

Follow the below steps for the successful execution of the application.

- SoC mode: Build as  TLS Client Example

  **![Build as](resources/readme/build_project.png)**

- NCP mode: 

### 4.3 Run and Test the application

After making any custom configuration changes required, build, download and run the application as below.

- Copy the certificates server-cert and server-key into Openssl/bin folder in the Windows PC (Remote PC).

**Note!**
 > All the certificates are given in the SDK. Path: `<SDK>/resources/certificates`
 
- In Windows PC (Remote PC) which is connected to AP, run the Openssl server by giving the following command

```sh
    > Openssl.exe s_server -accept<SERVER_PORT> -cert <server_certificate_file_path> -key <server_key_file_path> -tls<tls_version>

   Example: openssl.exe s_server -accept 5001 -cert server-cert.pem -key server-key.pem -tls1
   ```

![Run the Openssl server](resources/readme/ssl_server.png)
 
- Make sure the SSL server is running in the cloud (check with the domain name)

- After the program gets executed, SiWx91x would be connected to Access point having the configuration same that of in the application and get IP.

- The Device which is configured as SSL client will connect to three different remote SSL servers.

### 4.4 Application Output

**![Multiple TLS_Output](resources/readme/three_ssl_socket_output_1.png)**
**![Multiple TLS_Output](resources/readme/three_ssl_socket_output_2.png)**