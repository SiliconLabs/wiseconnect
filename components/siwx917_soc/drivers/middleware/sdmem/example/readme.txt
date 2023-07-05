   
===========================================================================================================
                               SD file system Example Program
===========================================================================================================

Copyright (c) 2019 Silicon Labs Incorporated. All Rights Reserved.
 
NOTICE:  All  information  contained  herein is, and  remains  the  property of 
Silicon Labs Incorporated. The intellectual and technical concepts contained
herein  are  proprietary to  Silicon Labs Incorporated and may be covered by 
U.S. and Foreign Patents, patents in process, and are protected by trade secret
or copyright law. Dissemination  of this  information or reproduction  of  this
material is strictly forbidden unless prior written permission is obtained from
Silicon Labs Incorporated.
 

============================================================================================================
============================================================================================================                                                       
Function description:
============================================================================================================
The SD fatfs example shows how to create a folder,create a file,write data into that file,read data from file,
and close that file. 
============================================================================================================
Assistance tool:
---------------------
Smart EVK

Assistance software:
---------------------
None

============================================================================================================
Usage Procedure:
============================================================================================================
->Insert SD card into the sd slot,and run the program.
============================================================================================================ 
Pass criteria
===========================================================================================================
Both Write_Buffer and Read_Buffer data should be same.
============================================================================================================
Note
============================================================================================================
1.Bydefault SD card is enable in this example.
2.For EMMC cards enable MMC_CARDS macro in rsi_sdmem.h file.
3.For 8 bit mode, enable _8BIT_MODE this macro in rsi_sdmem.h file.
4.For 1 bit mode, enable _1BIT_MODE this macro in rsi_sdmem.h file
5.Bydefault 4 bit mode is enable in this example.
============================================================================================================