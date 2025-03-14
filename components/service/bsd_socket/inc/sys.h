/*******************************************************************************
* @file  sys.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef _SYS_H
#define _SYS_H

#ifndef _SIZE_T_DEFINED
#define _SIZE_T_DEFINED
#undef size_t
typedef unsigned int size_t;
#endif /* _SIZE_T_DEFINED */

#ifndef _SSIZE_T_DEFINED
#define _SSIZE_T_DEFINED
#undef ssize_t
typedef int ssize_t;
#endif /* _SSIZE_T_DEFINED */

#ifndef _SYS__SIGSET_H_
#define	_SYS__SIGSET_H_
typedef unsigned long __sigset_t;
#endif /* !_SYS__SIGSET_H_ */

#ifndef _SUSECONDS_T_DECLARED
typedef	long suseconds_t;
#define	_SUSECONDS_T_DECLARED
#endif

#if !defined(__time_t_defined) && !defined(_TIME_T_DECLARED) && !defined(time_t)
typedef	long int time_t;
#define __time_h
#define	__time_t_defined
#define	_TIME_T_DECLARED
#endif

/* This define is also used outside of Newlib, e.g. in MinGW-w64 */
#ifndef _TIMEVAL_DEFINED
#define _TIMEVAL_DEFINED

/*
 * Structure returned by gettimeofday(2) system call, and used in other calls.
 */
struct timeval {
	time_t		tv_sec;		/* seconds */
	suseconds_t	tv_usec;	/* and microseconds */
};
#endif /* _TIMEVAL_DEFINED */

#ifndef _STRUCT_TIMESPEC
#define _STRUCT_TIMESPEC
/** Represents a time. */
struct timespec {
  /** Number of seconds. */
  time_t tv_sec;
  /** Number of nanoseconds. Must be less than 1,000,000,000. */
  long tv_nsec;
};
#endif

#endif //_SYS_H