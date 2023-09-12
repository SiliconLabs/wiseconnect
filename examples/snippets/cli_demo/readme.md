# Cli Demo

## 1 Purpose/Scope

The CLI Demo application is a command-line interface (CLI) application designed to showcase various functionalities and capabilities of SiWx91x in different scenarios and configuration modes. It serves as a quick reference guide and a hands-on demonstration of SiWx91x core features for developers and users.

## 2 Prerequisites/Setup Requirements

### 2.1 Hardware Requirements

- A Windows PC.
- **SoC Mode**:
  - Silicon Labs [BRD4325A, BRD4325B, BRD4325C, BRD4325G, BRD4338A](https://www.silabs.com/)
- **NCP Mode**:
  - Silicon Labs [BRD4180B](https://www.silabs.com/) **AND**
  - Host MCU Eval Kit. This example has been tested with:
    - Silicon Labs [WSTK + EFR32MG21](https://www.silabs.com/development-tools/wireless/efr32xg21-bluetooth-starter-kit)

### 2.2 Software Requirements

- Simplicity Studio IDE

  - Download the latest [Simplicity Studio IDE](https://www.silabs.com/developers/simplicity-studio)
  - Follow the [Simplicity Studio user guide](https://docs.silabs.com/simplicity-studio-5-users-guide/1.1.0/ss-5-users-guide-getting-started/install-ss-5-and-software#install-ssv5) to install Simplicity Studio IDE

### 2.3 Setup Diagram

#### SoC Mode  

![Figure: Setup Diagram SoC Mode for cli_demo Example](resources/readme/clidemo_soc.png)

#### NCP Mode  

![Figure: Setup Diagram NCP Mode for cli_demo Example](resources/readme/clidemo_ncp.png)

**NOTE**:

- The Host MCU platform (EFR32MG21) and the SiWx91x interact with each other through the SPI interface.

## 3 Project Environment

- Ensure the SiWx91x loaded with the latest firmware following the [Upgrade Si91x firmware](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/getting-started-with-soc-mode#upgrade-si-wx91x-connectivity-firmware)

- Ensure the latest Gecko SDK along with the extension WiSeConnect3 is added to Simplicity Studio.

### 3.1 Creating the project

#### 3.1.1 SoC mode

- Ensure the SiWx91x set up is connected to your PC.

- In the Simplicity Studio IDE, the SiWx91x SoC board will be detected under **Debug Adapters** pane as shown below.

  **![Soc Board detection](resources/readme/soc_board_detection.png)**

#### 3.1.2 NCP mode

- Ensure the EFx32 and SiWx91x set up is connected to your PC.

- In the Simplicity Studio IDE, the EFR32 board will be detected under **Debug Adapters** pane as shown below.

  **![EFR32 Board detection](resources/readme/efr32.png)**

### 3.2 Importing the project

- Studio should detect your board. Your board will be shown here. Click on the board detected and go to **EXAMPLE PROJECTS & DEMOS** section 

#### SOC Mode

- Select **Cli Demo** test application

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

### 4.1 Configure the application

- The application uses the default configurations as provided in the **default_wifi_ap_profile** in **sl_net_default_values.h** and user can choose to configure these parameters as needed.

### 4.2 Build the application

- SoC mode: Build as cli_demo Example

  **![Build as](resources/readme/build_project.png)**

- NCP mode:

### 4.3 Run and Test the application

- Once the build was successful, right click on project and click on Debug As->Silicon Labs ARM Program as shown in below image.

  - SoC

    **![debug_mode_soc](resources/readme/run_project.png)**

  - NCP

    **![debug_mode_NCP](resources/readme/run_project.png)**

### 4.4 Application Output

- After the application gets execueted successfully, we get output like this

  **![cli_demo_Output](resources/readme/build_output.png)**

- In terminal screen We have so many commands by manually we can add those commands in Extension command in Serial debug assistant.we can add those commands to it.

 **Here are List of those commands:**

1. HELP
2. wifi_init
3. wifi_init -i client
4. wifi_init -i ap
5. wifi_init -i apsta
6. wifi_scan
7. wifi_scan -s <ssid> -i 2.4g
8. wifi_scan -s <ssid> -i ap
9. wifi_scan -s <ssid> -i client
10. wifi_scan -s <ssid> -i 5g
11. wifi_ap_up
12. wifi_connect  <ssid> <password>
13. wifi_deinit
14. wifi_disconnect -i 0
15. wifi_get_client_info
16. wifi_init -i client/ap
17. wifi_get_fw_version
18. wifi_get_mac_address -i 0
19. wifi_set_performance_profile high_performance
20. wifi_set_performance_profile power_saver
21. wifi_set_performance_profile power_saver_low_latency
22. wifi_set_performance_profile ultra_power_saver
23. wifi_get_signal_strength
24. wifi_get_statistics
25. wifi_get_performance_profile
26. wifi_ip_address
27. wifi_get_ip_address
28. sl_dns_host_get_by_name <url> -t <time_in_milliseconds> -i <ipv4/ipv6>
29. wifi_iot_socket_create -i 1 -j 2 -k 2
30. wifi_iot_socket_connect -i 0 -j <server_ip> -k 4 -l <server_port>
31. wifi_iot_socket_bind -i 0 -j <local_ip> -k 4 -l <local_port>
32. wifi_iot_socket_receive_from -i 0
33. wifi_iot_socket_create -i 1 -j -k 1
34. wifi_iot_socket_listen -i 0 -j 1
35. wifi_iot_socket_accept -i 0
36. wifi_iot_socket_send -i 0 -d <data_to be sent>
37. start_dhcp

And so on...

- Here I am given some commands how we can enter and use those commands in terminal screen..

**HELP COMMAND:-**

**![Help_command](resources/readme/help.png)**

- After clicking Help command in serial Debug assistant you will see so many scenario's in debug assistant like this

**![Prints](resources/readme/prints.png)**
**![Prints](resources/readme/prints7.png)**

- clean the TTL console using Empty Data

**![Prints](resources/readme/empty_data.png)**

**WIFI INIT COMMAND:-**

- Again click extension cmd and click WiFi Init command in TTL console

**![Prints](resources/readme/wifi_init.png)**

- After clicking WiFi init command in TTL console and click send command we get this prints

**![Prints](resources/readme/wifi_init-prints.png)**

**WIFI SCAN COMMAND:-**

- After clicking WiFi Scan command in TTL console and click send command we get this prints

**![Prints](resources/readme/scan_prints.png)**

**WIFI CONNECT COMMAND:-**

- After clicking WiFi Connect command in TTL console and click send command we get this prints

**![Prints](resources/readme/wifi_connect.png)**
**![Prints](resources/readme/connect.png)**

**WIFI DEINIT COMMAND:-**

- After clicking WiFi deinit command in TTL console and click send command we get this prints

**![Prints](resources/readme/deinit.png)**