# Crypto - PUF

## Table of Contents

- [Purpose/Scope](#purposescope)
- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [Setup Diagram](#setup-diagram)
- [Getting Started](#getting-started)
- [Application Build Environment](#application-build-environment)
- [Test the Application](#test-the-application)
- [Application Output](#application-output)

## Purpose/Scope

This application demonstrates how to configure and use the Physical Unclonable Function (PUF) API for AES encryption and decryption. It illustrates how to leverage PUF for secure key management and cryptographic operations using AES in both ECB and CBC modes.

## Prerequisites/Setup Requirements

### Hardware Requirements

- Windows PC
- SoC Mode:
  - Silicon Labs [BRD4338A](https://www.silabs.com/)

### Software Requirements

- Simplicity Studio IDE

### Setup Diagram

#### SoC Mode 

  ![Figure: Setup Diagram SoC Mode for Crypto PUF Example](resources/readme/setup_dgm.png)

## Getting Started

Follow the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Install Simplicity Studio and the WiSeConnect 3 extension.
- Connect your device to the computer.
- Upgrade your connectivity firmware.
- Create a Studio project.

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

## Application Build Environment

The application can be configured to suit your requirements and development environment. Adjust the following parameters in the `app.c` file:

- **AES Key**: Used for encryption and decryption. Modify the `RSI_AES_KEY` constant if a different key is needed.
- **Plain Text**: Data to be encrypted/decrypted. Adjust the `RSI_AES_PLAIN_TXT` constant as required.
- **IV (Initialization Vector)**: For AES CBC mode, adjust the `RSI_AES_CBC_IV` constant if a different IV is required.

## Test the Application

To test the application:

1. Build the application.
2. Flash the firmware to the device.
3. Run and debug the application.
4. Observe the encryption and decryption results for AES ECB and AES CBC modes.

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) for detailed steps.

## Application Output

  ![PUF Output](resources/readme/output.png)

The output will show the results of AES encryption and decryption operations using the PUF API. 

**Key Actions Performed by the Application:**

1. **Start PUF:** Initializes the PUF module for cryptographic operations.
2. **Set Key:** Configures the PUF with the specified AES key.
3. **Get Key:** Retrieves and validates the key from PUF.
4. **Encrypt:** Performs AES encryption in ECB and CBC modes.
5. **Decrypt:** Performs AES decryption and validates the results.

For a detailed view of the implementation, refer to the `app.c` file and make any necessary adjustments to suit your specific requirements.

