# PSA MULTITHREAD for SI91X

## Introduction 
- This application contains an example code to demonstrate the use of thread-safe mechanism while using crypto hardware accelerators.

## Setting Up 
 - To use this application following Hardware, Software and the Project Setup is required

### Hardware Requirements	
  - Windows PC 
  - Silicon Labs [Si917 Evaluation Kit WSTK + BRD4325A]
 
![Figure: Introduction](resources/readme/image508a.png)

### Software Requirements
  - GSDK version 4.4.0
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
- The current implementation executes AES-ECB single-part encryption and decryption for AES.
- The current implementation executes the SHA 256 hashing.

## Configuration
- With default configuration, the application executes SHA and AES in two threads one after another.
- Uncomment SAME_CRYPTO_ACCELERATORS and comment DIFFERENT_CRYPTO_ACCELERATORS in app.c to execute sha operation in both threads.
- Set the priority of threads (thread0, thread1, thread2) to low, low4, low4 respectively to execute sha and aes applications one after another.
- Set the priority of threads (thread0, thread1, thread2) to low4, low, low respectively to execute the sha and aes application threads parallely in round-robin mode.

## Expected Results 
- The Encryption/Decryption function should produce the expected results. 
- The SHA hashing should produce the expected result.
- The two threads should use semaphores for crypto accelerator functionalities.

## Resources

* [AN1311: Integrating Crypto Functionality Using PSA Crypto Compared to Mbed TLS Guide](https://www.silabs.com/documents/public/application-notes/an1311-mbedtls-psa-crypto-porting-guide.pdf)

* [AN1135: Using Third Generation Non-Volatile Memory (NVM3) Data Storage](https://www.silabs.com/documents/public/application-notes/an1135-using-third-generation-nonvolatile-memory.pdf)