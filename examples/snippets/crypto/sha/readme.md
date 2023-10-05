# SHA Engine

## About 
In cryptography, SHA (Secure Hash Algorithm) is a cryptographic hash function designed by the United States National Security Agency and is a U.S. Federal Information Processing Standard published by the United States NIST

SHA produces a message digest based on principles similar to those used by Ronald L. Rivest of MIT in the design of the MD4 and MD5 message digest algorithms, but has a more conservative design.

SHA forms part of several widely used security applications and protocols, including TLS and SSL, PGP, SSH, S/MIME, and IPsec. Those applications can also use MD5; both MD5 and SHA are descended from MD4.

## Introduction

- This application explains how to configure and use the SHA crypto engine using Silabs SDK.
- This Application demonstrates the various SHA operations like
  - SHA 160
  - SHA 224
  - SHA 256
  - SHA 384
  - SHA 512
- This application is used to compute digest using SHA crypto engine
- This application is provided with single NIST test vector to verify funtionality.

## Setting Up 
- To use this application following Hardware, Software and the Project Setup is required

### Hardware Requirements	
  - Windows PC 
  - Silicon Labs [Si917 Evaluation Kit WSTK]
 
![Figure: Introduction](resources/readme/image502a.png)

### Software Requirements
  - Si91x SDK
  - Embedded Development Environment
    - For Silicon Labs Si91x, use Simplicity Studio

## Configuration and Steps for Execution

- Configure the following parameters in app.c file and update/modify following macros if required
- In calling function sl_si91x_sha, change SL_SI91x_SHA_512 to required sha mode
- From given configuration,
“SHA” refers to data which is given as input to SHA for computing digest.


## Build 
1. Compile the application in Studio using build icon

## Executing the Application
1. Compile and run the application.
2. Depending on SHA mode digest of respective size is computed for the given input data.

## Expected Results 
 - User will get the digest value.
