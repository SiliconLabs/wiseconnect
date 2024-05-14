# Wi-Fi - HTTP Client

## Table of Contents

- [Purpose/Scope](#purposescope)
- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [Setup Diagram](#setup-diagram)
- [Getting Started](#getting-started)
- [Application Build Environment](#application-build-environment)
- [Test the Application](#test-the-application)
- [Steps to set up HTTP server](#steps-to-set-up-http-server)

## Purpose/Scope

This application demonstrates how to configure SiWx91x device as an HTTP client and perform HTTP PUT, GET and POST operations with the HTTP server opened on remote peer. In this application, the SiWx91x is configured as a Wi-Fi station and connects to an Access point. The application then performs HTTP PUT, GET and POST operations with HTTP server opened on remote peer.

## Prerequisites/Setup Requirements

### Hardware Requirements

- A Windows PC.
- SoC Mode:
  - Silicon Labs [BRD4325A, BRD4325B, BRD4325C, BRD4325G, BRD4388A, BRD4339B](https://www.silabs.com/)
- NCP Mode:
  - Standalone
    - BRD4002A Wireless pro kit mainboard [SI-MB4002A]
    - EFR32xG24 Wireless 2.4 GHz +10 dBm Radio Board [xG24-RB4186C](https://www.silabs.com/development-tools/wireless/xg24-rb4186c-efr32xg24-wireless-gecko-radio-board?tab=overview)
    - NCP EFR Expansion Kit with NCP Radio board (BRD4346A + BRD8045A) [SiWx917-EB4346A]

### Software Requirements

- Simplicity Studio
- Windows PC (Remote PC) with HTTP server

### Setup Diagram

  ![Figure: Setup Diagram SoC and NCP Mode for HTTP Client Example](resources/readme/http_client_soc_ncp.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

## Application Build Environment

The application can be configured to suit user requirements and development environment. Read through the following sections and make any changes needed.

- The application uses the default configurations as provided in the **default_wifi_ap_profile** in ``sl_net_default_values.h`` and user can choose to configure these parameters as needed.
- In the Project Explorer pane, expand the **config** folder and open the **sl_net_default_values.h** file. Configure the following parameters to enable your Silicon Labs Wi-Fi device to connect to your Wi-Fi network.

  - STA instance related parameters

    - DEFAULT_WIFI_CLIENT_PROFILE_SSID refers to the name with which Wi-Fi network that shall be advertised and Si91X module is connected to it.

      ```c
      #define DEFAULT_WIFI_CLIENT_PROFILE_SSID               "YOUR_AP_SSID"      
      ```

    - DEFAULT_WIFI_CLIENT_CREDENTIAL refers to the secret key if the Access point is configured in WPA-PSK/WPA2-PSK security modes.

      ```c
      #define DEFAULT_WIFI_CLIENT_CREDENTIAL                 "YOUR_AP_PASSPHRASE" 
      ```

    - DEFAULT_WIFI_CLIENT_SECURITY_TYPE refers to the security type if the Access point is configured in WPA/WPA2 or mixed security modes.

      ```c
      #define DEFAULT_WIFI_CLIENT_SECURITY_TYPE              SL_WIFI_WPA2 
      ```
  
  - Other STA instance configurations can be modified if required in `default_wifi_client_profile` configuration structure.

  - Configure the following parameters in ``app.c`` to test HTTP Client app as per requirements

  - HTTP Server Port and IP Settings

    ```c
    #define HTTP_PORT           80                 // Remote http server port
    #define HTTP_SERVER_IP      "192.168.0.100"    // Remote http server IP address
    ```

- `sl_wifi_device_configuration_t` from `app.c` should be modified as per below requirements

  **[sl_net_set_credential()](https://docs.silabs.com/wiseconnect/3.0.13/wiseconnect-api-reference-guide-nwk-mgmt/net-credential-functions#sl-net-set-credential)** API expects the certificate in the form of linear array. Convert the pem certificate into linear array form using python script provided in the SDK `<SDK>/resources/scripts/certificate_script.py`.

   For example : If the certificate is ca-certificate.pem, enter the command in the following way:
   python certificate_script.py ca-certificate.pem
   The script will generate ca-certificate.pem in which one linear array named ca-certificate contains the certificate.

  Root CA certificate needs to be converted as mentioned above.

  After the conversion, place the converted file in `<SDK>/resources/certificates/` path and include the certificate file in app.c

  ```c
  // Certificate includes
  #include "ca-certificate.pem.h"
  
  // Load Security Certificates
  status = sl_net_set_credential(SL_NET_TLS_SERVER_CREDENTIAL_ID(0), SL_NET_SIGNING_CERTIFICATE, ca-certificate, sizeof(ca-certificate) - 1);
  ```

## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Build the application
- Flash, run and debug the application

- Before running the application, set up the HTTP server on a remote PC.

- Run the application. After the program gets executed, the SiWx91x module connects to AP and get an IP address.

- The application then requests for HTTP PUT to PUT/Create the file on to the server, which is given in index.txt file and waits until put file operation is complete.

- The remote HTTP server accepts the PUT request and writes the received data to a file. User can find the created new file **index.html** at the following path **resources → scripts**.

- After successful creation of file using HTTP PUT, the application requests for the file **index.html** from the HTTP server using HTTP GET method and waits until complete response is received from the server.

- After fetching **index.html**, the application posts the given data in **HTTP_DATA** to the HTTP server using HTTP POST method.

- User can see the log messages (success responses for HTTP PUT, HTTP GET and HTTP POST) at HTTP server as follows:
  ![http_client_Output](resources/readme/http_client_output.png)

## Steps to set up HTTP server

1. In a Windows PC, make sure python is installed.
2. Navigate to **`/<SDK>/resources/scripts/`**. Run **simple_http_server.py** script on port number 80 using the following command.

   `python simple_http_server.py 80`

   **NOTE:** If python command is not working replace the python with py.
   
## Trouble shooting

- If any permission errors are observed while setting up the Python HTTP server:
  - Try running the python HTTP server script with admin privileges (Open command prompt or terminal in admin mode)
  - Try using a different port like 8080 for the HTTP connection
