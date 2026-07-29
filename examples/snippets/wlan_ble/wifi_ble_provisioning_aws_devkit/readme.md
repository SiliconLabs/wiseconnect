# Sensor Data Publishing and LED Control with AWS IoT MQTT for SiWG917 Dev Kit

## High-Level Overview

SiWG917 dev kit AWS example: Provision Wi-Fi over BLE using the Si Connect app, connect to AWS IoT Core via MQTT, and publish sensor data in SoC mode.

## Table of Contents

- [Sensor Data Publishing and LED Control with AWS IoT MQTT for SiWG917 Dev Kit](#sensor-data-publishing-and-led-control-with-aws-iot-mqtt-for-siwg917-dev-kit)
  - [Table of Contents](#table-of-contents)
  - [High-Level Overview](#high-level-overview)
  - [Purpose / Scope](#purpose--scope)
  - [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
    - [Hardware Requirements](#hardware-requirements)
    - [Software Requirements](#software-requirements)
    - [Setup Diagram](#setup-diagram)
  - [Getting Started](#getting-started)
    - [Create an application](#create-an-application)
    - [Configure the following parameters in `aws_iot_config.h` file present at `<project>/config`](#configure-the-following-parameters-in-aws_iot_configh-file-present-at-projectconfig)
    - [Prepare the AWS Certificate for the Si Connect Mobile App](#prepare-the-aws-certificate-for-the-si-connect-mobile-app)
  - [Test the Application](#test-the-application)
    - [Application Output](#application-output)
    - [MQTT Connection](#mqtt-connection)
  - [Additional Information](#additional-information)
    - [Setting up Security Certificates](#setting-up-security-certificates)
    - [Create an AWS Thing](#create-an-aws-thing)
  
  - [Troubleshooting](#troubleshooting)
  - [Resources](#resources)
  - [Report Bugs and Get Support](#report-bugs-and-get-support)

## Purpose / Scope

In this application, the Bluetooth Low Energy (BLE) and Si Connect App (formerly Simplicity Connect / EFR Connect App) are used for provisioning the SiWx917 to a Wi-Fi Network. Once provisioned, SiWx917 acts as a Wi-Fi station and connects to the AWS cloud via MQTT.

The user can enable the required LED from the mobile app. The mobile app will publish the required LED state to MQTT_TOPIC1. SiWx917 subscribes to MQTT_TOPIC1 and enables the requested LED.

The application also publishes the real-time sensor data (LUX, Temperature, Humidity, Gyro, and Accelerometer) to the cloud on MQTT_TOPIC2. This can be viewed from the mobile app dashboard.

## Prerequisites/Setup Requirements

 Before running the application, the user will need the following things to setup.

### Hardware Requirements

- Windows PC
- Wireless Access Point
- **SoC Mode**:
  - Kits
    - [BRD2605A](https://www.silabs.com/development-tools/wireless/wi-fi/siwx917-dk2605a-wifi-6-bluetooth-le-soc-dev-kit?tab=overview)/[BRD2605B](https://www.silabs.com/development-tools/wireless/wi-fi/siwx917-dk2605a-wifi-6-bluetooth-le-soc-dev-kit?tab=overview) (SiWG917 Dev Kit Board)
- Android Phone or iPhone with Si Connect App (formerly Simplicity Connect / EFR Connect App), which is available in Play Store and App Store (or) Windows PC with Windows Silicon Labs connect application.

### Software Requirements

- [Simplicity Studio](https://www.silabs.com/developers/simplicity-studio)
- Silicon Labs [Si Connect App (formerly Simplicity Connect / EFR Connect App)](https://www.silabs.com/developers/simplicity-connect-mobile-app?tab=downloads), the app can be downloaded from Google Play store/Apple App store.
  > IMPORTANT: This example requires Si Connect App version 3.2.0 or later. Earlier versions do not prompt for the AWS certificate and endpoint configuration required by this application.
- **OpenSSL** command-line tool (required on the development PC to create the AWS device `.p12` certificate bundle for the Si Connect App):
  - **Windows:** Install from [OpenSSL for Windows](https://slproweb.com/products/Win32OpenSSL.html) (64-bit installer recommended).
  - **Linux / macOS:** Install the OpenSSL package from your OS distribution (for example, the `openssl` package on Ubuntu).
  - **Version:** OpenSSL with `pkcs12` support. OpenSSL 3.0 and later may require the `-legacy` flag when exporting the `.p12` file (see [Prepare the AWS Certificate for the Si Connect Mobile App](#prepare-the-aws-certificate-for-the-si-connect-mobile-app)).

### Setup Diagram

![Figure: Setup Diagram for SiWG917 Dev Kit](resources/readme/embedded-sensor-demo-setup.png)

## Getting Started

### Create an application

- You can follow the instructions on the [Developing with Radio Boards in SoC Mode page](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#developing-with-wi-se-connect-sdk-v3-x-with-si-wx91x-radio-boards) to:
  - [Create a project](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#create-a-project)
  - [Build the application](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#build-an-application)
  - [Flash the application](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/#flash-an-application)

![Create a Project](resources/readme/create-project.png)

### Configure the following parameters in `aws_iot_config.h` file present at `<project>/config`

Before configuring the parameters in `aws_iot_config.h`, register the SiWx917 device in the AWS IoT registry by following the steps mentioned in the [Create an AWS Thing](#create-an-aws-thing) section.

Configure AWS_IOT_MQTT_HOST macro with the device data endpoint to connect to AWS. To get the device data endpoint in the AWS IoT Console, navigate to Settings, copy the Endpoint, and define the AWS_IOT_MQTT_HOST macro with this value.

![AWS_IOT_MQTT_HOST_NAME](resources/readme/aws_iot_mqtt_host_url.png)

 ```c
   //AWS Host name 
   #define AWS_IOT_MQTT_HOST          "a2m21kovu9tcsh-ats.iot.us-east-2.amazonaws.com"  

   //default port for MQTT
   #define AWS_IOT_MQTT_PORT          "8883"
   
   #define AWS_IOT_MQTT_CLIENT_ID     "silicon_labs_thing"
   
   // Thing Name of the Shadow this device is associated with 
   #define AWS_IOT_MY_THING_NAME      "silicon_labs_thing"    
```

To authenticate and securely connect with AWS, the SiWx917 device requires a unique x.509 security certificate and private key, as well as a CA certificate. At this point, you must have a device certificate, private key, and CA certificate, which are downloaded during the creation/registration of AWS Thing.

By default, the device certificate and private key that are downloaded from the AWS are in [.pem format](https://en.wikipedia.org/wiki/Privacy-Enhanced_Mail). To load the device certificate and private key to the SiWx917, the device certificate and private key should be converted into a C-array. For converting the certificates and private key into a C-array, refer to [Setting up Security Certificates](#setting-up-security-certificates).

By default, the WiSeConnect SDK contains the Starfield Root CA Certificate in C-array format.

> **Note** :
 The included Cloud connectivity certificates are for reference only. If using default certificates in the release, the cloud connection will not work. You must replace the default certificates with valid certificates while connecting to the appropriate Cloud/OpenSSL Server.

> **Note**: For recommended settings, please refer the [recommendations guide](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-prog-recommended-settings/).

### Prepare the AWS Certificate for the Si Connect Mobile App

Starting with **Si Connect App version 3.2.0**, the mobile app prompts you to provide AWS certificate and endpoint details during the provisioning flow. You must prepare the following items before running the application:

1. **Create a PKCS#12 (.p12) certificate bundle**

   The Si Connect app requires the device certificate and private key in PKCS#12 format. Use the device certificate and private key files that were downloaded when you [created the AWS Thing](#create-an-aws-thing).

   Run the following command using OpenSSL to create the `.p12` file (the device certificate and key alone are sufficient, the Si Connect app does not require the root CA inside the bundle):

   ```sh
   openssl pkcs12 -export -out aws_device_cert.p12 \
     -inkey <device-private-key>.pem.key \
     -in <device-certificate>.pem.crt
   ```

   You will be prompted to set an **export password**. Remember this password, you will need to enter it in the Si Connect app.

   > **Note (OpenSSL 3.x):** OpenSSL 3.0 and later use modern PBE/MAC defaults that some mobile keystores (notably older Android versions and some iOS releases) cannot import. If the Si Connect app rejects the generated `.p12`, regenerate it with the `-legacy` flag, or specify legacy algorithms explicitly:
   >
   > ```sh
   > openssl pkcs12 -export -legacy -out aws_device_cert.p12 \
   >   -inkey <device-private-key>.pem.key \
   >   -in <device-certificate>.pem.crt
   > ```

   > **Tip:** If you need to bundle the AWS root CA into the `.p12` (not required by Si Connect, but useful for testing with other tools), download the Starfield / Amazon root CA from Amazon Trust Services and add `-certfile <downloaded-ca>.pem`:
   >
   > - [https://www.amazontrust.com/repository/SFSRootCAG2.pem](https://www.amazontrust.com/repository/SFSRootCAG2.pem) (Starfield Services Root CA G2)
   > - [https://www.amazontrust.com/repository/AmazonRootCA1.pem](https://www.amazontrust.com/repository/AmazonRootCA1.pem) (Amazon Root CA 1)
   >
   > The SDK ships the Starfield CA only as a C-array header (`<SDK>/resources/certificates/aws_starfield_ca.pem.h`), which OpenSSL cannot consume directly.

2. **Transfer the .p12 file to your mobile device**

   Copy the generated `aws_device_cert.p12` file to your Android or iOS device (e.g., via email, cloud storage, or USB transfer).

3. **Note down the AWS IoT Endpoint URL**

   This is the same endpoint configured in `AWS_IOT_MQTT_HOST` in `aws_iot_config.h`. You can find it in the [AWS IoT Console](https://console.aws.amazon.com/iot/home) under **Settings > Device data endpoint**. It has the following format:

   ```text
   <unique-id>.iot.<region>.amazonaws.com
   ```

During the mobile app provisioning flow, you will be prompted to provide the following:

| Mobile App Field       | Value to Enter                                                                 |
|------------------------|--------------------------------------------------------------------------------|
| **Certificate file**   | Select the `aws_device_cert.p12` file transferred to your device               |
| **Certificate password** | The export password you set when creating the `.p12` file                    |
| **Endpoint URL**       | Your AWS IoT device data endpoint (e.g., `a2m21kovu9tcsh-ats.iot.us-east-2.amazonaws.com`) |
| **Subscriber topic**   | The MQTT topic to subscribe to for sensor data (e.g., `MQTT_TOPIC2`)           |
| **Publisher topic**    | The MQTT topic to publish LED control commands to (e.g., `MQTT_TOPIC1`)        |

> **Note**: The Subscriber topic and Publisher topic field labels above are from the mobile app's perspective — the topic the mobile app subscribes to (to receive sensor data) and the topic the mobile app publishes to (to send LED commands). From the device firmware's perspective these are reversed: in `wifi_app.c`, the `MQTT_TOPIC1` is the topic to which the SiWx917 subscribes (to receive LED commands) and the `MQTT_TOPIC2` is the topic to which the SiWx917 publishes (to send sensor data).

## Test the Application

The following instructions are provided in [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/) to:

- Build the application.
- Flash, run, and debug the application.

Complete the following steps for successful execution of the application:

1. Connect any serial console for prints.

2. When the SiWx917 EVK enters BLE advertising mode, launch the **Si Connect App** (formerly Simplicity Connect / EFR Connect App).

3. Click on **Demo** and select **AWS IoT**.

   ![](resources/readme/Mobile_app_ui_1.png)

4. It will scan for the module, and it appears as `BLE_CONFIGURATOR` on the UI. Select as shown below.

   ![](resources/readme/dut_scan_result.png)

5. Now, SiWx917 module will start as a station and scan for the access points (AP) nearby.

6. Once the list of AP scan results is displayed on the screen, you can select the SSID of the AP to which you want to connect.

   ![](resources/readme/remote_screen2.png)

7. Click on the SSID of the AP, then enter a password if the AP is in security mode. Click **Connect** to associate with the access point.

   ![](resources/readme/remote_screen3.png)

8. Once the Silicon Labs module is successfully connected to the Wi-Fi network, the BLE connection is automatically disconnected from the application side since it is no longer needed for provisioning.

9. After the Wi-Fi connection is established, the Si Connect app navigates to the AWS configuration screen. You will be prompted to provide the following details (see [Prepare the AWS Certificate for the Si Connect Mobile App](#prepare-the-aws-certificate-for-the-si-connect-mobile-app) for how to obtain these values):

   - **Certificate file**: Tap to browse and select the `.p12` certificate file you transferred to your device.
   - **Certificate password**: Enter the export password you set when creating the `.p12` file.
   - **Endpoint URL**: Enter your AWS IoT device data endpoint (e.g., `a2m21kovu9tcsh-ats.iot.us-east-2.amazonaws.com`).
   - **Subscriber topic**: Enter the MQTT topic to subscribe to for sensor data (e.g., `MQTT_TOPIC2`) — temperature, humidity, light (lux), and accelerometer/gyroscope readings from the Dev Kit sensors.
   - **Publisher topic**: Enter the MQTT topic to publish LED control commands to (e.g., `MQTT_TOPIC1`).

   ![](resources/readme/Mobile_app_ui_2.png)

   > **Note:** In Si Connect App versions prior to 3.2.0, only the subscriber and publisher topics were required. Starting with version 3.2.0, the certificate and endpoint fields are mandatory.

10. The application starts publishing sensor data to AWS via MQTT, and this information will be displayed on the mobile app dashboard.

   ![](resources/readme/Mobile_app_ui_3.png)

   ![](resources/readme/Mobile_app_ui_4.png)

11. The mobile app publishes the required LED state to AWS. The application, which is already subscribed to this topic, will turn on the respective LED on the SiWx917.

    ![](resources/readme/Mobile_app_ui_5.png)

### Application Output

  ![](resources/readme/output1.png)

  ![](resources/readme/output2.png)

**Note:**

- To learn more about aws mqtt apis error codes, refer to the `aws_iot_error.h` file present in the `<SDK>\third_party\aws_sdk\include\`.
- If the user is calling select and experiencing long wait times, and if no data is received, it is the user's responsibility to manage sending the keepalive packets to maintain the connection.

### MQTT Connection

After successfully connecting to Wi-Fi, the application establishes a connection to AWS IoT Core. It subscribes to a topic (`MQTT_TOPIC1`) to receive LED control commands and publishes sensor data on another topic (`MQTT_TOPIC2`). The application then waits to receive data published on the subscribed topic from the cloud.

1. Go to the mobile app and enable the required LED.

2. Realtime sensor data will be displayed on the mobile app dashboard. You can also see the published sensor data by subscribing to the topic (MQTT_TOPIC2) in AWS console.

   ![](resources/readme/publish_status_from_AWS.png)

## Additional Information

### Setting up Security Certificates

The WiSeConnect SDK provides a conversion script (written in Python 3) to make the conversion straightforward. The script is provided in the SDK `<SDK>/resources/scripts` directory and is called [certificate_to_array.py](https://github.com/SiliconLabs/wiseconnect/tree/v4.1.1-content-for-docs/resources/scripts/).

1. Copy the downloaded device certificate, private key from AWS, and the certificate_to_array.py to `<SDK>/resources/certificates`.

2. To convert the device certificate and private key to C arrays, open a system command prompt in the same path and give the following commands.

   ```sh
   $> python3 certificate_to_array.py <input filename> <output arrayname>

   For example:
   $> python3 certificate_to_array.py d8f3a44d3f.pem.crt aws_client_certificate.pem
   $> python3 certificate_to_array.py d8f3a44d3f.pem.key aws_client_private_key.pem
   ```

   After running the above commands, two new files are created as below:

   ```sh
   aws_client_certificate.pem.crt.h
   aws_client_private_key.pem.key.h
   ```

3. After converting the device certificate and private key to C - array, it is essential to include the device certificate: `aws_client_certificate.pem.crt.h` and private key: `aws_client_private_key.pem.key.h` in the `<SDK>/resources/certificates` folder.

4. Ensure to load the device certificate and private key to SiWx917 using the [sl_net_set_credential()](https://docs.silabs.com/wiseconnect/latest/wiseconnect-api-reference-guide-nwk-mgmt/net-credential-functions#sl-net-set-credential) API.

   ```c
   status = sl_net_set_credential(SL_NET_TLS_SERVER_CREDENTIAL_ID(0), SL_NET_CERTIFICATE, aws_client_certificate, (sizeof(aws_client_certificate) - 1));
  
   status = sl_net_set_credential(SL_NET_TLS_SERVER_CREDENTIAL_ID(0), SL_NET_PRIVATE_KEY, aws_client_private_key, (sizeof(aws_client_private_key) - 1));
   ```

5. Ensure to update the certificate names in the **IoT_Client_Init_Params** structure before calling the **aws_iot_mqtt_init()** API.

The Starfield Root CA certificate used by your Wi-Fi device to verify the AWS server is already included in the WiSeConnect SDK at `<SDK>/resources/certificates`; no additional setup is required.

  > **NOTE :**
  > Support for the SNI extension has been added to the AWS SDK, ensuring it is set by the client when connecting to an AWS server using TLS 1.3. This is handled internally by the AWS SDK and does not affect compatibility with other TLS versions.

  > **NOTE :**
  > Amazon uses [Starfield Technologies](https://www.starfieldtech.com/) to secure the AWS website, the WiSeConnect SDK includes the [Starfield CA Certificate](https://github.com/SiliconLabs/wiseconnect/tree/v4.1.1-content-for-docs/resources/certificates/aws_starfield_ca.pem.h).
  >
  > AWS has announced that there will be changes in their root CA chain. More details can be found in the reference link: [here](https://aws.amazon.com/blogs/security/acm-will-no-longer-cross-sign-certificates-with-starfield-class-2-starting-august-2025/)
  >
  > We are providing both root CAs (Starfield class-2 and Starfield G2) in aws_starfield_ca.pem.h, which is located in the WiSeConnect directory `<SDK>/resources/certificates/aws_starfield_ca.pem.h`
  >
  > For AWS connectivity, StarField Root CA certificate has the highest authority being at the top of the signing hierarchy.
  >
  > The StarField Root CA certificate is an expected/required certificate which usually comes pre-installed in the operating systems and plays a key part in certificate chain verification when a device is performing TLS authentication with the IoT endpoint.
  >
  > On a SiWx91x device, we do not maintain the root CA trust repository due to memory constraints, so it is mandatory to load Starfield Root CA certificate for successful mutual authentication to the AWS server.
  >
  > The certificate chain sent by AWS server is as below:
  > **Starfield Class 2**:
  > id-at-commonName=Amazon,RSA 2048 M01,id-at-organizationName=Amazon,id-at-countryName=US
  > id-at-commonName=Amazon Root CA 1,id-at-organizationName=Amazon,id-at-countryName=US
  > id-at-commonName=Starfield Services Root Certificate Authority - G2,id-at-organizationName=Starfield Technologies, Inc.,id-at-localityName=Scottsdale,id-at- stateOrProvinceName=Arizona,id-at-countryName=US
  >id-at-organizationalUnitName=Starfield Class 2 Certification Authority,id-at-organizationName=Starfield Technologies, Inc.,id-at-countryName=US
  >
  > **Starfield G2**:
  > id-at-commonName=Amazon RSA 2048 M01,id-at-organizationName=Amazon,id-at-countryName=US
  > id-at-commonName=Amazon Root CA 1,id-at-organizationName=Amazon,id-at-countryName=US
  > id-at-commonName=Starfield Services Root Certificate Authority - G2,id-at-organizationName=Starfield Technologies, Inc.,id-at-localityName=Scottsdale,id-at-stateOrProvinceName=Arizona,id-at-countryName=US
  >
  > To authenticate the AWS server on SiWx91x, first validate the Root CA (validate the Root CA received with the Root CA loaded on the device). Once the Root CA validation is successful, other certificates sent from the AWS server are validated.
  > Alternate certification chains support is added. With this, as opposed to requiring full chain validation, only the peer certificate must validate to a trusted certificate. This allows loading intermediate root CA's as trusted.
  > The default CA certificate is the Starfield Combined CA certificate. To use the Intermediate Amazon Root CA 1 certificate, define the `SL_SI91X_AWS_IOT_ROOT_CA1` macro in the application.

### Create an AWS Thing

Create a thing in the AWS IoT registry to represent your IoT device.

1. In the [AWS IoT console](https://console.aws.amazon.com/iot/home), in the navigation pane, under **Manage**, choose **All devices**, and then choose **Things**.

   ![AWS console](resources/readme/aws_create_thing_step1.png)

2. If **No things** message is displayed, click on **Create things**.

   ![AWS thing](resources/readme/aws_create_thing_step2.png)

3. On the **Create things** page, choose **Create single thing** and click next.

   ![AWS thing creation](resources/readme/aws_create_thing_step3.png)

4. On the **Specify thing properties** page, enter a name for your IoT thing (for example, **Test_IoT**), and choose **Unnamed shadow (classic)** in the Device Shadow section, then choose **Next**. You cannot change the name of a thing after you create it. To change a thing's name, you must create a new thing, give it a new name, and then delete the old thing.

   ![Add Device 1](resources/readme/aws_create_thing_step4.png)

5. During **Configure device certificate** step, choose **Auto-generate a new certificate (recommended)** option and click **Next**.

   ![Add Device 2](resources/readme/aws_create_thing_step5.png)

6. Attach the policy to the thing created.

   - If you have an existing policy, attach it and click **Create thing**.

     ![Attach policy](resources/readme/aws_choosing_policy.png)

   - If policy is not yet created, follow the steps below.
  
     1. Choose **Create policy** and fill the fields as per your requirements.

         ![Create policy](resources/readme/aws_create_thing_attach_policy.png)

     2. Give the **Name** to your Policy. Fill in the **Action** and **Resource ARN** fields as shown in the image below. Click on **Allow** under **Effect** and click **Create**.

        ![Filling fields for policy](resources/readme/aws_create_thing_policy_create.png)

     3. Choose the created policy and click on **Create thing**.

7. Choose the **Download** links to download the device certificate and private key. Note that Root CA certificate is already present in the SDK (aws_starfield_ca.pem.h), and can be directly used.
  
    >**Warning:** This is the only instance you can download your device certificate and private key. Make sure to save them securely.

    ![Downloading certificates](resources/readme/aws_thing_certificates_download.png)

8. Click **Done**.

   The created thing should now be visible on the AWS console (**Manage > All devices > Things**).

## Troubleshooting

If you encounter issues while running this example, check the following:

- Prepare AWS certificates and configure `aws_iot_config.h` before you build the application.
- Use the Si Connect mobile app for BLE provisioning and verify the target AP credentials.
- Review [Setting up Security Certificates](#setting-up-security-certificates) and [Create an AWS Thing](#create-an-aws-thing) for cloud setup.
- Confirm MQTT connection status in the [MQTT Connection](#mqtt-connection) console output.

## Resources

- [WiSeConnect Getting Started Guide](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/)
- [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure)
- [WiSeConnect Recommended Settings Guide](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-prog-recommended-settings/)
- [AWS IoT Core Documentation](https://docs.aws.amazon.com/iot/)

## Report Bugs and Get Support

Report issues and get help from the Silicon Labs community:

- [Silicon Labs Community](https://www.silabs.com/community)

