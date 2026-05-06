/*******************************************************************************
* @file  select.h
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

#ifndef _SYS_SELECT_H
#define _SYS_SELECT_H

/* We don't define fd_set and friends if we are compiling POSIX
   source, or if we have included (or may include as indicated
   by __USE_W32_SOCKETS) the W32api winsock[2].h header which
   defines Windows versions of them.   Note that a program which
   includes the W32api winsock[2].h header must know what it is doing;
   it must not call the Cygwin select function.
*/
# if !(defined (_WINSOCK_H) || defined (_WINSOCKAPI_) || defined (__USE_W32_SOCKETS))

#include "sys.h"

//typedef unsigned long long int __size_t ;

typedef struct timeval_t {
	long		tv_sec;		/* seconds */
	long long	tv_usec;	/* and microseconds */
}timeval;


typedef struct timespec_t {
	long long	tv_sec;		/* seconds */
	long	tv_nsec;	/* and nanoseconds */
}timespec;


#if !defined(_SIGSET_T_DECLARED)
#define	_SIGSET_T_DECLARED
typedef	__sigset_t	sigset_t;
#endif

#  define _SYS_TYPES_FD_SET
/*
 * Select uses bit masks of file descriptors in longs.
 * These macros manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here
 * should be >= NOFILE (param.h).
 */
#ifndef	FD_SETSIZE
#define	FD_SETSIZE	32 //64 to 32 changed
#endif

typedef unsigned int	__fd_mask;  //long to int changed
#if __BSD_VISIBLE
typedef __fd_mask	fd_mask;
#endif

#define _NFDBITS	((int)sizeof(__fd_mask) * 8) /* bits per mask */
#if __BSD_VISIBLE
#define NFDBITS		_NFDBITS
#endif

#ifndef	_howmany
#define	_howmany(x,y)	(((x) + ((y) - 1)) / (y))
#endif

typedef	struct fd_set {
	__fd_mask	__fds_bits[_howmany(FD_SETSIZE, _NFDBITS)];
} fd_set;
#if __BSD_VISIBLE
#define fds_bits	__fds_bits
#endif

#define __fdset_mask(n)	((__fd_mask)1 << ((n) % _NFDBITS))
#define FD_CLR(n, p)	((p)->__fds_bits[(n)/_NFDBITS] &= ~__fdset_mask(n))
#if __BSD_VISIBLE
#define FD_COPY(f, t)	(void)(*(t) = *(f))
#endif
#define FD_ISSET(n, p)	(((p)->__fds_bits[(n)/_NFDBITS] & __fdset_mask(n)) != 0)
#define FD_SET(n, p)	((p)->__fds_bits[(n)/_NFDBITS] |= __fdset_mask(n))
#define FD_ZERO(p) do {				\
        fd_set *_p;					\
        size_t _n;					\
							\
        _p = (p);					\
        _n = _howmany(FD_SETSIZE, _NFDBITS);		\
        while (_n > 0)					\
                _p->__fds_bits[--_n] = 0;		\
} while (0)

#if !defined (__INSIDE_CYGWIN_NET__)

/**
 * @brief
 * The select function is used to monitor multiple file descriptors for readiness to
 * perform I/O operations.  The file descriptors in the sets are monitored to
 * see if they are ready for reading, ready for writing, or have an error
 * condition pending.  The timeout parameter specifies the maximum time to
 * wait for the file descriptors to become ready.  If the timeout parameter
 * is NULL, select will block until a file descriptor becomes ready.  If the
 * timeout parameter is not NULL, select will return after the specified time
 * has elapsed, even if no file descriptors are ready.
 * @param __n
 *   The highest file descriptor number in any of the sets plus one.
 * @param __readfds
 *      The set of file descriptors to check for being ready to read.
 * @param __writefds
 *     The set of file descriptors to check for being ready to write.
 * @param __exceptfds
 *    The set of file descriptors to check for error conditions pending.
 * @param __timeout
 *   The maximum time to wait for the file descriptors to become ready.
 * @return
 *   On succcess returns total number of file descriptors contained in the three returned descriptor sets, which may be zero if the timeout expires.
 *   On error, -1 is returned, and errno is set appropriately.
 * @note
 * The select function modifies the sets passed to it, so if the function
 * is to be called again, the sets must be reinitialized.
 * The exceptfds parameter is not currently supported.
 */

int select(int __n, fd_set *__readfds, fd_set *__writefds,
		 fd_set *__exceptfds, const struct timeval *__timeout);


#endif /* !__INSIDE_CYGWIN_NET__ */

#endif /* !(_WINSOCK_H || _WINSOCKAPI_ || __USE_W32_SOCKETS) */

#endif /* sys/select.h */
