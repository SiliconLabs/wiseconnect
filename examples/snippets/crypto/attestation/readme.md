# SECURE ATTESTATION

## Introduction

- This application demonstrates how to send a token request to TA and receive the response token.
- This application can be used by the user for checking the authenticity of the device.
- This application sends a nonce of 32 byte. This nonce is generated using TRNG.
- This application receives the token which is signed using Attestation private key.
- This application then decodes the token using t_cose and QCBOR libraries.

## Setting Up 
- To use this application following Hardware, Software and the Project Setup is required

### Hardware Requirements	
  - Windows PC 
  - Silicon Labs [Si917 Evaluation Kit WSTK]
 
![Figure: Introduction](resources/readme/image502a.png)

### Software Requirements
  - Si91x SDK
  - Embedded Development Environment
    - For Silicon Labs Si91x, use Keil Version 5

## Configuration and Steps for Execution

- Configure the following parameters in rsi_attestation_app.h file and update/modify following macros if required

   ```c
   #define LENGTH_OF_RANDOM_DATA        // Length of nonce to be sent along with the attestation request
   #define LENGTH_OF_TOKEN              // Length of token to be received
   ```

## Build 
1. Compile the application in Keil using build icon

## Executing the Application
1. Compile and run the application.
2. When the application runs,It sends a token request along with 32 byte nonce and receives the token.

## Expected Results 
 - User will get the token along with the nonce that was sent in verify_data buffer present in t_cose_verify.c file and same data get printed with payload and signature data on the UART console.
 - Payload data and signature data should to copied in two separate files using hex editor and run the below command for signature verification
   openssl dgst -sha256 -verify <Attestation_pubkey.pem> -signature <signature file> <payload file>
