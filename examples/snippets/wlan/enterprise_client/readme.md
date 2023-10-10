# Enterprise client

## Table of Contents

- [Purpose/Scope](#purposescope)
- [Prerequisites/Set up Requirements](#prerequisitesset-up-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [Setup Diagram](#setup-diagram)
- [Getting Started](#getting-started)
- [Application Build Environment](#application-build-environment)
- [Test the Application](#test-the-application)
- [Additional Information](#additional-information)
  - [Configure Access Point in WAP2-EAP](#configure-access-point-in-wap2-eap)
  - [FreeRADIUS server setup](#freeradius-server-setup)

## Purpose/Scope

This application demonstrates how to configure SiWx91x in enterprise client mode, connect SiWx91x to an enterprise secured Access point using any of the EAP-TLS, EAP-TTLS, EAP-FAST, and PEAP methods.

##  Prerequisites/Set up Requirements

Before running the application, the user will need the following things to setup.

###  Hardware Requirements

- Windows PC
- A Wireless Access Point with WPA2-Enterprise configuration
- **SoC Mode**:
  - [Silicon Labs BRD4325A/BRD4325B/BRD4325C/BRD4325G/BRD4338A Radio boards](https://www.silabs.com/)
  - [BRD4002A Wireless Pro Kit Mainboard (WPK)](https://www.silabs.com/)
- **NCP Mode**:
  - [Silicon Labs SLWSTK6006A EFR32xG21 Wireless Starter Kit](https://www.silabs.com/development-tools/wireless/efr32xg21-wireless-starter-kit) which includes
      - BRD4001A Wireless Starter Kit Mainboard (WSTK)/BRD4002A Wireless Pro Kit Mainboard (WPK)
      - BRD4180A/BRD4180B Radio Board


###  Software Requirements

- Simplicity Studio

- [Silicon Labs Gecko SDK](https://github.com/SiliconLabs/gecko_sdk)

- [Si91x COMBO SDK](https://github.com/SiliconLabs/wiseconnect-wifi-bt-sdk/)

- [FreeRADIUS Server](https://freeradius.org/)

**Note:**

> This example application supports Bare metal and FreeRTOS configurations.

###  **Setup Diagram**

The picture below illustrates the EAP frame exchanges between SiWx91x, Access Point and FreeRADIUS Server.

**![Figure: Setup Diagram for Enterprise Client soc Example](resources/readme/eap_exchanges.png)**

**![Figure: Setup Diagram for Enterprise Client soc Example](resources/readme/setup_soc_ncp.png)**

**Note:**

- The Host MCU platform (EFR32MG21) and the SiWx91x interact with each other through the SPI interface.

Follow the [Getting Started with Wiseconnect3 SDK](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) guide to set up the hardware connections and Simplicity Studio IDE.

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/)  to:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

##  Application Build Environment

The application can be configured to suit your requirements and development environment.

- In the Project explorer pane, expand the **enterprise_client** folder and open the `app.c` file. Configure the following parameters based on your requirements.

 **![Application configuration](resources/readme/application_configuration.png)**

   - WIFI_ACCESS_POINT_SSID refers to the name of the Access point configured in WAP/WPA2-EAP security mode.

   ```c
    #define WIFI_ACCESS_POINT_SSID                              "enterprise"
   ```
  
   - All the configuration parameters are present in **wifi_client_enterprise_eap_profile**, **wifi_client_enterprise_eap_profile.config.security** refers to the type of security. In this example, SiWx91x supports WPA-EAP, WPA2-EAP security types.

   ```c
    .config.security                                            SL_WIFI_WPA2_ENTERPRISE
   ```

   - In most of the cases, the EAP-TLS method uses root CA certificate and public-private key pairs for authentication.
   - Encryption type can be switched using following.

   ```c
    .config.encryption                                          SL_WIFI_EAP_TLS_ENCRYPTION
   ```

>  **Note:**
>
> 1. Application shall load the certificates using **sl_wifi_set_certificate** API.
> 2. By default, the application loads **wifiuser.pem** certificate present at **<WiFi SDK> → resources → certificates** 
      
   - In order to load your own certificate, follow the below steps:

   - The certificate has to be passed as a parameter to **rsi_wlan_set_certificate** API in linear array format. Convert the **.pem** format certificate into linear array form using python script provided in the SDK **SiWx91x COMBO SDK → resources → certificates → certificate_to_array.py**

   - You can load the certificate in two ways as mentioned below. 
     - Aggregate the certificates in to one file in a fixed order of private key, public key, intermediate CA/dummy certificate, and CA certificate and load the certificate with certificate type **1**. Place the certificate at **<SiWx91x COMBO SDK> → resources → certificates**. Convert the single certificate file into linear array using the following command.

         `python certificate_to_array.py wifi-user.pem`

      - Load the EAP certificates - private key, public key, and CA certificates individually with certificate type as 17,33 and 49 respectively. Maximum certificate length for each individual certificate is 4088 bytes. Place the certificate at **<SiWx91x COMBO SDK> → resources → certificates**. Convert the certificates into linear array using the following commands.

         `python certificate_to_array.py <private key file name>`

         `python certificate_to_array.py <public key file name>`

         `python certificate_to_array.py <CA certificate file name>`

      - The above commands shall generate .h files which contains certificate as a character array. Include these files in `rsi_enterprise_client.c` file.

   - Credentials for EAP authentication can be passed in through **wifi_client_enterprise_eap_credential** structure
   - **wifi_client_enterprise_eap_credential.data.username** refers to user ID which is configured in the user configuration file of the radius server. In this example, user identity is **user1**. 

      ```c
      .data.username                                              "user1"
      ```

   - **wifi_client_enterprise_eap_credential.data.password** refers to the password which is configured in the user configuration file of the Radius Server for that User Identity. In this example, password is **password1**.

      ```c
      .data.password                                              "password1"
      ```

   - DHCP_MODE refers to whether the IP assignment of SiWx91x is done statically or through DHCP.

      1  - DHCP_MODE
      
      0  - Static IP assignment

   - IP related configuration can be configured in **wifi_client_enterprise_eap_profile.ip** structure

      ```c
      .ip = {
            .mode = SL_IP_MANAGEMENT_DHCP,
            .type = SL_IPV4,
            .host_name = NULL,
            .ip = {{{0}}},
         }
      ```

   - Enterprise configuration parameters. By default, the EAP method is set to **TLS**.

**Important Notes**: 

- Ensure the Access Point is configured in WPA2-Enterprise security mode. For more details, please refer to [Configure Access Point in WAP2-EAP](#configure-access-point-in-wap2-eap) in Appendix section.
- Ensure the FreeRADIUS set up is ready before running the application project. For more details, please refer to [FreeRADIUS Server set up](#freeradius-server-set-up) in Appendix section.

## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Build the application
- Flash, run and debug the application

    **![Application prints](resources/readme/application_prints.png)**


## Additional Information

### Configure Access Point in WAP2-EAP

- Navigate to the Wireless Security section and enable the **WPA/WPA2 - Enterprise** option, as shown in the figure below. The image below is for a TP-Link Access Point.

   **![Enable the WPA/WPA2 - Enterprise option](resources/readme/wpa2_eap.png)**

- Enter the IP address of the Radius Server in the field labeled, **Radius Server IP**. In the above figure, it is **192.168.0.139**.

   **Note**: Give **Radius Server IP** as the IP address of the PC on which you want to run the FreeRADIUS server.

- Enter the Radius Password as **12345678**. This password should be given as a **secret** in FreeRADIUS server.

### FreeRADIUS server set up

The configuration explained below is for Linux OS, similar process may be followed for other OS.

- FreeRADIUS Server installation links:

   [https://freeradius.org/](https://freeradius.org/)

   [http://xperiencetech.com/download/radius-free-download.asp](http://xperiencetech.com/download/radius-free-download.asp)

   **Note**: This example application has been tested with  FreeRADIUS-server-3.2.3.
   
- Once installed, go to the **C: → FreeRADIUS → etc → raddb** folder. Open the **clients.conf** file and add the following lines at the end of the file.

   ```c
   client 192.168.0.1/24 {
   secret = 12345678
   shortname = private-network-1
   }
   ```

- The IP address in the above lines (**192.168.0.1**) is the gateway IP address of the Access Point to which SiWx91x is going to connect. The **secret = 12345678** is the key that is given in the Access Point's radius server configurations to authenticate it with the FreeRADIUS Server.

The gateway ip address of TP_Link Access Point is,

   **![Change the input for the default_eap_type field ](resources/readme/lan_ip.png)**

- Open the **eap.conf** file  by following the path, **FreeRADIUS → etc → raddb → mods-available** and make the following changes:
  - Change the **default_eap_type** field under the **eap** section to  **tls**, as shown in the figure below.

      **![Change the input for the default_eap_type field ](resources/readme/eap_type.png)**

  - Change the paths for **private_key_file**, **certificate_file** and **CA_file** under the **tls** section to **${certdir}/wifi-user.pem**, as shown in the figure below.

      **![Change the inputs](resources/readme/certs_path1.png)**

      **![Change the inputs](resources/readme/certs_path2.png)**
  
  - Uncomment the **fragment_size** and **include_length** lines under the **tls** section, as shown in the figure below.

      **![Uncomment the fragment_size and include_length lines](resources/readme/eap_fragment_size.png)**
  
  - Open the **users** file present at **C: → FreeRADIUS → etc → raddb** and add the lines shown below. This adds/registers a user with username **user1** and password **test123**. These are given in the `app.c`     at  **sl_net_wifi_eap_credential_entry_t** api.

     **![Open the users file and add the lines](resources/readme/eap_credentials.png)**

  - Copy the **wifi-user.pem** file from **WiSeConnect3 → resources → certificates** folder to **C: → FreeRADIUS → etc → raddb → certs** folder.

  - Open the terminal in linux, go to the radius server path and give command below to start radius server:
   
      ```c
      radiusd -X
      ```

  - Then Radius server has started successfully you will see a print at the end which says, **Ready to process requests**.

     **![Run Radius server in Windows PC2](resources/readme/radius_server.png)**

**Note:**
 The radius server has to run before the application is executed. You will observe some transactions when the module is trying to connect to the radius server. Restart the Radius server when you execute the application every time.
 
 Make sure that, the radius server and module is on the same network. 
