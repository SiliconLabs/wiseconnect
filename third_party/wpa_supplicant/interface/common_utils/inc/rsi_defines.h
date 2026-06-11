/*******************************************************************************
* @file  rsi_defines.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/

/*HEADER*************************************************************
 *********************************************************************
 ***
 ***
 *** File Name  : rsi_defines.h
 ***
 *** File Description: 
 ***   This file contains general definitions.
 ***
 ***
 *** $Rcsfile $
 ***
 *** $Revision $
 ***
 *** Rev History:
 *** Sl    By                 Date            Change Details 
 *** -----------------------------------------------------------------
 *** 01.  
 *** 02.  
 *** -----------------------------------------------------------------
 ***
 ********************************************************************
 *END****************************************************************/

#ifndef _RSIDEFINES_H
#define _RSIDEFINES_H

/*
 * @ Type Definitions
 */
#ifndef __PE_Types_H
#include "sl_wlan_data_types.h"
#endif

typedef unsigned long LDD_TDeviceData;
typedef unsigned long LDD_TUserData;
typedef unsigned long LDD_RTC_TTime;

#ifndef BIT
#define BIT(x) ((uint32)1 << (x))
#endif
#define BIT_16(x) ((uint16)1 << (x))

#ifndef NULL
#define NULL 0
#endif

/****************** True and False Definitions ******************/
#ifdef FALSE
#undef FALSE
#endif
#define FALSE (0)

#ifdef TRUE
#undef TRUE
#endif
#define TRUE (1)

#endif
/* EOF */

/**
*$Log: rsi_defines.h,v $
*Revision 1.1.1.1  2012/02/28 07:14:12  kiran.gunda
*WiSe Connet initial import
*
**/
