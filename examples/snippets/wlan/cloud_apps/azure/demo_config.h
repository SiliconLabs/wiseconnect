/* Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License. */

#ifndef DEMO_CONFIG_H
#define DEMO_CONFIG_H

/* FreeRTOS config include. */
#include "FreeRTOSConfig.h"

/*
 * This plug-and-play model can be found at:
 * https://github.com/Azure/iot-plugandplay-models/blob/main/dtmi/com/example/thermostat-1.json
 * This Model ID is tightly tied to the code implementation in `sample_azure_iot_pnp_simulated_device.c`
 * If you intend to test a different Model ID, please provide the implementation of the model on your application.
 */
#define sampleazureiotMODEL_ID "dtmi:com:example:Thermostat;1"

/**************************************************/
/******* DO NOT CHANGE the following order ********/
/**************************************************/

/* Include logging header files and define logging macros in the following order:
 * 1. Include the header file "logging_levels.h".
 * 2. Define the LIBRARY_LOG_NAME and LIBRARY_LOG_LEVEL macros depending on
 * the logging configuration for DEMO.
 * 3. Include the header file "logging_stack.h", if logging is enabled for DEMO.
 */

//#include "logging_levels.h"

/* Logging configuration for the Demo. */
#ifndef LIBRARY_LOG_NAME
#define LIBRARY_LOG_NAME "AzureIoTDemo"
#endif

#ifndef LIBRARY_LOG_LEVEL
#define LIBRARY_LOG_LEVEL LOG_INFO
#endif

/*
 * The function prints to the console before the network is connected;
 * then a UDP port after the network has connected. */
extern void vLoggingPrintf(const char *pcFormatString, ...);

/* Map the SdkLog macro to the logging function to enable logging */
#ifndef SdkLog
#define SdkLog(message) vLoggingPrintf message
#endif

//#include "logging_stack.h"

/************ End of logging configuration ****************/

/**
 * @brief Enable Device Provisioning
 *
 * @note To disable Device Provisioning undef this macro
 *
 */
/* #define democonfigENABLE_DPS_SAMPLE   */

#ifdef democonfigENABLE_DPS_SAMPLE

/**
 * @brief Provisioning service endpoint.
 *
 * @note https://docs.microsoft.com/azure/iot-dps/concepts-service#service-operations-endpoint
 *
 */
#define democonfigENDPOINT "global.azure-devices-provisioning.net"

/**
 * @brief Id scope of provisioning service.
 *
 * @note https://docs.microsoft.com/azure/iot-dps/concepts-service#id-scope
 *
 */
#define democonfigID_SCOPE "<YOUR ID SCOPE HERE>"

/**
 * @brief Registration Id of provisioning service
 *
 * @warning If using X509 authentication, this MUST match the Common Name of the cert.
 *
 *  @note https://docs.microsoft.com/azure/iot-dps/concepts-service#registration-id
 */
#define democonfigREGISTRATION_ID "<YOUR REGISTRATION ID HERE>"

#endif /* democonfigENABLE_DPS_SAMPLE */

/**
 * @brief IoTHub module Id.
 *
 * @note This is optional argument for IoTHub
 */
#define democonfigMODULE_ID ""

/**
 * @brief IoTHub hostname.
 *
 */
#define democonfigHOSTNAME "<YOUR IOT HUB HOSTNAME HERE>"

/**
 * @brief IoTHub device Id.
 *
 */
#define democonfigDEVICE_ID "<YOUR DEVICE ID HERE>"

//To use Symmetric Key set macro USE_SYMMETRIC_KEY value to 1 or else set as 0 to use x509 Key
#define USE_SYMMETRIC_KEY 1

#if USE_SYMMETRIC_KEY
/**
* @brief Device symmetric key
*
*/
#define democonfigDEVICE_SYMMETRIC_KEY "<Symmetric key>"

#else
/**
* @brief Client's X509 Certificate.
*
*/
#define democonfigCLIENT_CERTIFICATE_PEM "<YOUR DEVICE CERT HERE>"

/**
* @brief Client's private key.
*
*/
#define democonfigCLIENT_PRIVATE_KEY_PEM "<YOUR DEVICE PRIVATE KEY HERE>"
#endif

/**
 * @brief Load the required certificates:
 *  - Baltimore Trusted Root CA
 *  - DigiCert Global Root G2
 *  - Microsoft RSA Root Certificate Authority 2017
 *
 * @warning Hard coding certificates is not recommended by Microsoft as a best
 * practice for production scenarios. Please see our document here for notes on best practices.
 * https://github.com/Azure-Samples/iot-middleware-freertos-samples/blob/main/docs/certificate-notice.md
 *
 */
#define democonfigROOT_CA_PEM                                            \
  "-----BEGIN CERTIFICATE-----\r\n"                                      \
  "MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh\r\n" \
  "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\r\n" \
  "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\r\n" \
  "MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT\r\n" \
  "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\r\n" \
  "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG\r\n" \
  "9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI\r\n" \
  "2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx\r\n" \
  "1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ\r\n" \
  "q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz\r\n" \
  "tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ\r\n" \
  "vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP\r\n" \
  "BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV\r\n" \
  "5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY\r\n" \
  "1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4\r\n" \
  "NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG\r\n" \
  "Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91\r\n" \
  "8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe\r\n" \
  "pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl\r\n" \
  "MrY=\r\n"                                                             \
  "-----END CERTIFICATE-----\r\n"

/**
 * @brief Size of the network buffer for MQTT packets.
 */
#define democonfigNETWORK_BUFFER_SIZE (5 * 1024U)

/**
 * @brief IoTHub endpoint port.
 */
#define democonfigIOTHUB_PORT (4002)

#endif /* DEMO_CONFIG_H */
