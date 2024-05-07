# PSA ECDSA for SI91X

## Table of Contents

- [Purpose/Scope](#purposescope)
- [Prerequisites/Setup Requirements](#prerequisitessetup-requirements)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
  - [Setup Diagram](#setup-diagram)
- [Getting Started](#getting-started)
- [Application Build Environment](#application-build-environment)
  - [Application Configuration Parameters](#application-configuration-parameters)
- [Test the Application](#test-the-application)

## Purpose/Scope

- This application contains an example code to demonstrate the PSA ECDSA functionality
- This application uses hardware accelerators for key generation of SECPR1 curves and TA execution for signature generation / verification
- Key generation : HW accelerators for P192R1 and P256R1 ; SW implementation for P224R1
- Sign and Verify Plain message : TA execution for P224R1 and P256R1 ; SW implementation for P192R1
- Sign and Verify with Wrapped message : TA execution for P224R1 and P256R1 ; P192R1 not supported

## Prerequisites/Setup Requirements

- The board MBR should have "ta_secure_boot_enable" set to 1 for wrap functionality

### Hardware Requirements	

  - Windows PC 
  - Silicon Labs [Si917 Evaluation Kit WSTK + BRD4325A]

### Software Requirements

- Simplicity Studio

### Setup Diagram 

  ![Figure: Introduction](resources/readme/image508a.png)

## Getting Started

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/) to:

- Install Studio and WiSeConnect 3 extension
- Connect your device to the computer
- Upgrade your connectivity firmware
- Create a Studio project

For details on the project folder structure, see the [WiSeConnect Examples](https://docs.silabs.com/wiseconnect/latest/wiseconnect-examples/#example-folder-structure) page.

## Application Build Environment

- To program the device ,refer **"Burn M4 Binary"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html** to work with Si91x and Simplicity Studio.

### Application Configuration Parameters

curve : SL_SI91X_ECC_SECP192R1

| Macro | Size |
|---------------| ------------------|
| SL_SI91X_PRIVATE_KEY_SIZE | 24 |
| SL_SI91X_PUBLIC_KEY_SIZE | 49 |

curve : SL_SI91X_ECC_SECP224R1

| Macro | Size |
|---------------| ------------------|
| SL_SI91X_PRIVATE_KEY_SIZE | 28 |
| SL_SI91X_PUBLIC_KEY_SIZE | 57 |

curve : SL_SI91X_ECC_SECP256R1

| Macro | Size |
|---------------| ------------------|
| SL_SI91X_PRIVATE_KEY_SIZE | 32 |
| SL_SI91X_PUBLIC_KEY_SIZE | 65 |

- Default ecc curve is SEP256R1, can be changed to SECP192R1 or SECP224R1 by changing private key size in psa_ecdsa_app.c : psa_set_key_bits();
- Default private key size is 256 bits, can be changed to 192 bits or 224 bits
- Default public key size is corresponding to 256 bits private key size
- Default SHA hashing used is SHA256, can be changed to SHA384 or SHA512
- To change the SHA algorithm, set the psa_key_algorithm to PSA_ALG_ECDSA(PSA_ALG_SHA_XXX) in psa_ecdsa_app.c
- To use wrapped key instead of plain for private key, enable USE_WRAPPED_KEYS macro

## Test the Application

Refer to the instructions [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-developing-for-silabs-hosts/) to:

- Build the application.
- Flash, run and debug the application.

Follow the steps as mentioned for the successful execution of the application:

[AN1311: Integrating Crypto Functionality Using PSA Crypto Compared to Mbed TLS Guide](https://www.silabs.com/documents/public/application-notes/an1311-mbedtls-psa-crypto-porting-guide.pdf)

## Application Output

  ![ECDSA Output](resources/readme/output.png)