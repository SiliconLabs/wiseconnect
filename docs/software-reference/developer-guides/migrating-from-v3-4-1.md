# Migrating from WiSeConnect™ SDK v3.4.1 to v3.4.2

## Table of Contents

- [Overview](#overview)
- [Migration Steps](#migration-steps)
  - [Update Appliction name](#update-application-name)

## Overview

This is a guide for updating an existing application using the WiSeConnect™ SDK v3.4.1 to a v3.4.2 application.

There are few naming and file changes in v3.4.2 as compared to v3.4.1, mostly in order to standardize the names and improve the overall usage experience of the application programming interface (API). Migration requires the names everywhere to be updated in the existing application.

## Migration Steps

In order to convert a WiSeConnect SDK v3.4.1 application to a v3.4.2 application,

1. Open your existing application project in Simplicity Studio.

2. In each source file of the project, replace the v3.4.1 names or interfaces with v3.4.2 names or interfaces. 

Refer to the tables in each of the sections that follow which map the v3.4.1 API elements to v3.4.2 In some instances, the differences between v3.4.1 and v3.4.2 are highlighted in **bold** text.




### Update Application name

- Starting from the 3.4.2 release, the application **wifi_firmware_flashing_from_host_uart_xmodem_bootloader_ncp** has been renamed to **fw_update_bootloader_xmodem** to reduce the application name length and resolve studio build issues caused by long path names.