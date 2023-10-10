# PSA Asymmetric Key storage for SI91X

## Introduction 
- This application contains an example code to demonstrate the PSA asymmetric key storage functionality.
- This application uses mbedtls fallback for key generation using ecc curve.
- Only TRNG generation is done using hardware accelerator.

## Setting Up 
 - To use this application following Hardware, Software and the Project Setup is required.

### Hardware Requirements	
  - Windows PC 
  - Silicon Labs [Si917 Evaluation Kit WSTK + BRD4338A]
 
![Figure: Introduction](resources/readme/image508a.png)

### Software Requirements
  - GSDK version 4.3.2
  - Si91x SDK
  - Embedded Development Environment
    - For Silicon Labs Si91x, use the latest version of Simplicity Studio (refer **"Download and Install Simplicity Studio"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html**)
 
## Project Setup
- **Silicon Labs Si91x** refer **"Download SDKs"**, **"Add SDK to Simplicity Studio"**, **"Connect SiWx917"**, **"Open Example Project in Simplicity Studio"** section in **getting-started-with-siwx917-soc** guide at **release_package/docs/index.html** to work with Si91x and Simplicity Studio

## Build 
- Compile the application in Simplicity Studio using build icon 

![Figure: Build run and Debug](resources/readme/image508c.png)

## Device Programming
- To program the device ,refer **"Burn M4 Binary"** section in **getting-started-with-siwx917-soc** guide at 
**release_package/docs/index.html** to work with Si91x and Simplicity Studio

## Executing the Application
- The user can run the application to verify import, generate and export of Volatile and Persistent plain keys.

### Configuration
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

## Key Storage
The following key storages are supported in this example:

* Volatile public key in RAM
* Persistent public key in NVM3

## Expected Results 
- The private key should be successfully generated and imported in plain format.
- The public key is generated using private key and exported in plain format.
- The public key replaces the private key in key buffer and attributes are replaced accordingly.
- Public key should be stored in NVM3 in case of persistent key.
 
## Resources

[AN1311: Integrating Crypto Functionality Using PSA Crypto Compared to Mbed TLS Guide](https://www.silabs.com/documents/public/application-notes/an1311-mbedtls-psa-crypto-porting-guide.pdf)