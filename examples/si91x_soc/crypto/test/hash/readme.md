# Test PSA Hash algorithms for SI91X devices

## Introduction 
- This folder contains Application that are used to for Unit test code to test the Crypto functionality


## Setting Up 
 - To use this application following Hardware, Software and the Project Setup is required

### Hardware Requirements	
  - Windows PC
  - SiWx917 Radio Board

### Software Requirements
  - Simplicity SDK version: 2024.6.0
  - Si91x SDK
  - Embedded Development Environment
    - For Silicon Labs Si91x, use the latest version of Simplicity Studio (refer **"Download and Install Simplicity Studio"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html**)
 
## Project Setup
- **Silicon Labs Si91x** refer **"Download SDKs"**, **"Add SDK to Simplicity Studio"**, **"Connect SiWx917"**, **"Open Example Project in Simplicity Studio"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html** to work with Si91x and Simplicity Studio

## Build 
- Compile the application in Simplicity Studio using build icon 

![Figure: Build run and Debug](resources/readme/image508c.png)

## Device Programming
- To program the device ,refer **"Burn M4 Binary"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html** to work with Si91x and Simplicity Studio

## Executing the Application
-  Enable any one of the SHA mode at a time.
 **Note!** Application tests the Following SHA modes: 
  - SHA-1
  - SHA-224
  - SHA-256
  - SHA-384
  - SHA-512
  
  To enable the test case for SHA-1 uncomment the macros TEST_SL_SHA1_SHORT and TEST_SL_SHA1_LONG present in **psa_driver_hash_test.h**.
  To enable the test case for SHA-224 uncomment the macros TEST_SL_SHA224_SHORT and TEST_SL_SHA224_LONG present in **psa_driver_hash_test.h**.
  To enable the test case for SHA-256 uncomment the macros TEST_SL_SHA256_SHORT and TEST_SL_SHA256_LONG present in **psa_driver_hash_test.h**.
  To enable the test case for SHA-384 uncomment the macros TEST_SL_SI91X_SHA384_SHORT and TEST_SL_SI91X_SHA384_LONG in **psa_driver_hash_test.h**.
  To enable the test case for SHA-512 uncomment the macros TEST_SL_SI91X_SHA512_SHORT and TEST_SL_SI91X_SHA512_LONG in **psa_driver_hash_test.h**.
  Multiple test cases can also be enabled at the same time, eg. SHA-1, SHA-224 and SHA256 can be tested at once and SHA-384, SHA512 can be tested at once.

## Expected Results 
- Every Test cases should PASS. 