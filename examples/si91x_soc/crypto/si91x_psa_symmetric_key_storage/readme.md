# PSA symmetric Key storage for SI91X

## Table of Contents

- [Purpose/Scope](#purposescope)
- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [Setup Diagram](#setup-diagram)
- [Getting Started](#getting-started)
- [Application Build Environment](#application-build-environment)
- [Test the Application](#test-the-application)

## Purpose/Scope

- This application contains an example code to demonstrate the PSA asymmetric key storage functionality.
- This application uses mbedtls fallback for key generation using ecc curve.
- Only TRNG generation is done using hardware accelerator.

## Prerequisites/Setup Requirements

Before running the application, the user will need the following things to setup.

### Hardware Requirements	

  - Windows PC 
  - Silicon Labs [Si917 Evaluation Kit WSTK + BRD4338A]

### Software Requirements

- Simplicity Studio

### Setup Diagram
 
  ![Figure: Introduction](resources/readme/image508a.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

## Application Build Environment

### Application Configuration Parameters

- The user can run the application to verify import, generate and export of Volatile and Persistent plain keys.

 * PERSISTENT_KEY_ID in psa_asymmetric_key_storage_app.c file denotes key uid which will be stored in NVM3.
 * KEY_SIZE_BITS macro in psa_asymmetric_key_storage_app.c file is size of plain private key in bits.
 * KEY_MAX_SIZE is the max size of the key buffer used to store the generated public key. 
 * Private_key in psa_asymmetric_key_storage_app.c file is the private key to be imported.
 * The public key generated will replace the private key in key_attributes.
 * Public_key in psa_asymmetric_key_storage_app.c is the public key generated and exported.
 * Default algo for asymmetric key generation is ECDH.
 * Default ecc curve is SEP256R1 - can choose between SECPxxxR1 and MONTGOMERY
 * Default key size is 256 bits - can choose among 192, 256, 384 or 521 bits for SECPxxxR1 and 255 or 448 bits for MONTGOMERY
 * To change the ecc curve and key size, change the corresponding macro in psa_asymmetric_key_storage_app.c - psa_set_key_type() and change the KEY_MAX_SIZE to the corresponding length of public key generated. 


## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-getting-started/) to:

- Build the application.
- Flash, run and debug the application.

Follow the steps as mentioned for the successful execution of the application:

[AN1311: Integrating Crypto Functionality Using PSA Crypto Compared to Mbed TLS Guide](https://www.silabs.com/documents/public/application-notes/an1311-mbedtls-psa-crypto-porting-guide.pdf)