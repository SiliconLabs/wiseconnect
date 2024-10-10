/*	$OpenBSD: socket.h,v 1.105 2022/09/03 21:13:48 mbuhl Exp $	*/
/*	$NetBSD: socket.h,v 1.14 1996/02/09 18:25:36 christos Exp $	*/

/*
 * Copyright (c) 1982, 1985, 1986, 1988, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)socket.h	8.4 (Berkeley) 2/21/94
 */

#ifndef COMPONENTS_BSD_SOCKET_INC_SOCKET_H_
#define	COMPONENTS_BSD_SOCKET_INC_SOCKET_H_

/* get the definitions for struct iovec, size_t, ssize_t, and <sys/cdefs.h> */
#include "uio.h"
#include "netinet_in.h"

#ifndef	_SOCKLEN_T_DEFINED_
#define	_SOCKLEN_T_DEFINED_
typedef	__socklen_t	socklen_t;	/* length type for network syscalls */
#endif

#ifndef	_SA_FAMILY_T_DEFINED_
#define	_SA_FAMILY_T_DEFINED_
typedef	__sa_family_t	sa_family_t;	/* sockaddr address family type */
#endif

typedef long off_t;
/*
 * Definitions related to sockets: types, address families, options.
 */

/*
 * Types
 */
/**
 * @addtogroup BSD_SOCKET_TYPES Socket Type
 * @ingroup BSD_SOCKET_FUNCTIONS
 * @{ 
 */
#define	SOCK_STREAM	1		///< Stream socket. Provides sequenced, reliable, two-way, connection-based byte streams.
#define	SOCK_DGRAM	2		///< Datagram socket. Supports datagrams (connectionless, unreliable messages of a fixed maximum length).
#define	SOCK_RAW	3		///< Raw socket. Provides raw network protocol access.
#define	SOCK_RDM	4		///< Reliably-delivered message. Provides a reliable datagram layer that does not guarantee ordering.
#define	SOCK_SEQPACKET	5   ///< Sequenced packet stream. Provides a sequenced, reliable, two-way, connection-based data transmission path for datagrams of fixed maximum length.
#ifdef _KERNEL
#define	SOCK_TYPE_MASK	0x000F		///< Mask that covers all the above socket types.
#endif
/** @} */

/*
 * Socket creation flags
 */
#if __BSD_VISIBLE
#define	SOCK_CLOEXEC		0x8000	/* set FD_CLOEXEC */
#define	SOCK_NONBLOCK		0x4000	/* set O_NONBLOCK */
#ifdef _KERNEL
#define	SOCK_NONBLOCK_INHERIT	0x2000	/* inherit O_NONBLOCK from listener */
#endif
#define	SOCK_DNS		0x1000	/* set SS_DNS */
#endif /* __BSD_VISIBLE */

/*
 * Option flags per-socket.
 */
/**
 * @addtogroup BSD_SOCKET_OPTION_NAME Socket Option Name
 * @ingroup BSD_SOCKET_FUNCTIONS
 * @{ 
 */
#define	SO_DEBUG	0x0001		///< Enables recording of debugging information.
#define	SO_ACCEPTCONN	0x0002		///< Indicates that the socket has had listen().
#define	SO_REUSEADDR	0x0004		///< Allows local address reuse.
#define	SO_KEEPALIVE	0x0008		///< Keeps connections alive.
#define	SO_DONTROUTE	0x0010		///< Uses interface addresses only.
#define	SO_BROADCAST	0x0020		///< Permits sending of broadcast messages.
#define	SO_USELOOPBACK	0x0040		///< Bypasses hardware when possible.
#define	SO_LINGER	0x0080		///< Lingers on close if data is present.
#define	SO_OOBINLINE	0x0100		///< Leaves received out-of-band data in line.
#define	SO_REUSEPORT	0x0200		///< Allows local address and port reuse.
#define SO_TIMESTAMP	0x0800		///< Timestamps received datagram traffic.
#define SO_BINDANY	0x1000		///< Allows binding to any address.
#define SO_ZEROIZE	0x2000		///< Zeroes out all mbufs sent over the socket.
#define SO_MAX_RETRANSMISSION_TIMEOUT_VALUE 0x3012 ///< Configures max retransmission timeout value. The option value associated with this option name should be a power of 2 between 1 and 32.
/*
 * Additional options, not kept in so_options.
 */
#define	SO_SNDBUF	0x1001		///< Sets send buffer size.
#define	SO_RCVBUF	0x1002		///< Sets receive buffer size.
#define	SO_SNDLOWAT	0x1003		///< Sets send low-water mark.
#define	SO_RCVLOWAT	0x1004		///< Sets receive low-water mark.
#define	SO_SNDTIMEO	0x1005		///< Sets send timeout.
#define	SO_RCVTIMEO	0x1006		///< Sets receive timeout.
#define	SO_ERROR	0x1007		///< Gets error status and clears.
#define	SO_TYPE		0x1008		///< Gets socket type.
#define	SO_NETPROC	0x1020		///< Multiplexes; network processing.
#define	SO_RTABLE	0x1021		///< Sets routing table to be used.
#define	SO_PEERCRED	0x1022		///< Gets connect-time credentials.
#define	SO_SPLICE	0x1023		///< Splices data to another socket.
#define	SO_DOMAIN	0x1024		///< Gets socket domain.
#define	SO_PROTOCOL	0x1025		///< Gets socket protocol.
/*
 * si91x socket options
 */
#define	SO_CERT_INDEX				0x1026	///< Sets certificate index for SSL socket.
#define	SO_HIGH_PERFORMANCE_SOCKET	0x1027	///< Enables high-performance socket.
#define SO_TLS_SNI                  0x1028  ///< Passes SNI extension for SSL socket.
#define SO_TLS_ALPN                 0x1029  ///< Passes ALPN extension for SSL socket.
/** @} */

// From Linux include/uapi/linux/tcp.h
// See https://www.kernel.org/doc/Documentation/networking/tls.txt for more details.
#define TCP_ULP	0x001f		/* Attach a ULP to a TCP connection */

#define TLS		"tls"
#define TLS_1_0	"tls_1_0"
#define TLS_1_1	"tls_1_1"
#define TLS_1_2	"tls_1_2"
#define TLS_1_3 "tls_1_3"

/**
 * Structure used for manipulating linger option.
 */
struct	linger {
	int	l_onoff;		///< Option on/off 
	int	l_linger;		///< Linger time 
};

#if __BSD_VISIBLE

//#ifndef _TIMEVAL_DECLARED
//#define _TIMEVAL_DECLARED
//struct timeval {
//	time_t		tv_sec;		/* seconds */
//	suseconds_t	tv_usec;	/* and microseconds */
//};
//#endif

/*
 * Structure used for manipulating splice option.
 */

struct	splice {
	int	sp_fd;			/* drain socket file descriptor */
	off_t	sp_max;			/* if set, maximum bytes to splice */
	struct	timeval	sp_idle;	/* idle timeout */
};

/*
 * Maximum number of alternate routing tables
 */
#define	RT_TABLEID_MAX		255
#define	RT_TABLEID_BITS		8
#define	RT_TABLEID_MASK		0xff

#endif /* __BSD_VISIBLE */

/**
 * @addtogroup BSD_SOCKET_OPTION_LEVEL Socket Option Level
 * @ingroup BSD_SOCKET_FUNCTIONS
 * @{ 
 */
/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
#define	SOL_SOCKET	0xffff		///< This is used to denote that the options are applicable at the socket level.

//From Linux include/linux/socket.h
/* Setsockoptions(2) level. Thanks to BSD these must match IPPROTO_xxx */
#define SOL_TCP 0x0006  ///< This is used to denote that the options are applicable at the TCP level.
/** @} */

/*
 * Address families.
 */
/**
 * @addtogroup BSD_SOCKET_FAMILIY Socket Address Family
 * @ingroup BSD_SOCKET_FUNCTIONS
 * @{ 
 */
#define	AF_UNSPEC	0		///< Unspecified address family.
#define	AF_UNIX		1		///< Local to host (UNIX domain) address family.
#define	AF_LOCAL	AF_UNIX		///< POSIX compatibility alias for AF_UNIX.
#define	AF_INET		2		///< Internet IP Protocol address family.
#define	AF_IMPLINK	3		///< ARPANET IMP address family.
#define	AF_PUP		4		///< PUP protocols (e.g., BSP) address family.
#define	AF_CHAOS	5		///< MIT CHAOS protocols address family.
#define	AF_NS		6		///< XEROX NS protocols address family.
#define	AF_ISO		7		///< ISO protocols address family.
#define	AF_OSI		AF_ISO  ///< OSI protocol family.
#define	AF_ECMA		8		///< European Computer Manufacturers Association (ECMA) protocols address family.
#define	AF_DATAKIT	9		///< Datakit protocols address family.
#define	AF_CCITT	10		///< CCITT protocols (e.g., X.25) address family.
#define	AF_SNA		11		///< IBM SNA address family.
#define AF_DECnet	12		///< DECnet address family.
#define AF_DLI		13		///< DEC Direct data link interface address family.
#define AF_LAT		14		///< LAT address family.
#define	AF_HYLINK	15		///< NSC Hyperchannel address family.
#define	AF_APPLETALK	16		///< AppleTalk address family.
#define	AF_ROUTE	17		///< Internal Routing Protocol address family.
#define	AF_LINK		18		///< Link layer interface address family.
#define	pseudo_AF_XTP	19		///< eXpress Transfer Protocol (no AF).
#define	AF_COIP		20		///< Connection-oriented IP, aka ST II address family.
#define	AF_CNT		21		///< Computer Network Technology address family.
#define pseudo_AF_RTIP	22		///< Identifier for RTIP packets.
#define	AF_IPX		23		///< Novell Internet Protocol address family.
#define	AF_INET6	24		///< IPv6 address family.
#define pseudo_AF_PIP	25		///< Identifier for PIP packets.
#define AF_ISDN		26		///< Integrated Services Digital Network address family.
#define AF_E164		AF_ISDN		///< CCITT E.164 recommendation address family.
#define AF_NATM		27		///< Native ATM access address family.
#define	AF_ENCAP	28      ///< Encapsulation address family.
#define	AF_SIP		29		///< Simple Internet Protocol address family.
#define AF_KEY		30      ///< Key management protocol address family (originally developed for usage with IPsec).
#define pseudo_AF_HDRCMPLT 31		///< Used by BPF to not rewrite headers in interface output routine.
#define	AF_BLUETOOTH	32		///< Bluetooth address family.
#define AF_MPLS         33              ///< MPLS address family.
#define pseudo_AF_PFLOW 34		///< Identifier for pflow packets.
#define pseudo_AF_PIPEX 35		///< Identifier for PIPEX packets.
#define AF_MAX          36      ///< Maximum address family value.
/** @} */

/*
 * Structure used by kernel to store most
 * addresses.
 */
/**
 * @addtogroup BSD_SOCKET_FUNCTIONS
 * @{ 
 */
/**
 * @struct sockaddr
 * @brief 
 *     The structure stores generic socket address.
 * 
 * @details
 *     The @ref sockaddr structure represents addresses in a protocol-independent way. It includes the address family, and the address data. The actual format of the address data is determined by the address family specified in `sa_family`.
 * 
 */
struct sockaddr {
	uint8_t    sa_len;		///< Total length of the structure.
	sa_family_t sa_family;		///< Address family (e.g., AF_INET).
	char	    sa_data[14];	///< Socket address data. The actual format is determined by the address family.
};
/** @} */

/**
 * Sockaddr type which can hold any sockaddr type available
 * in the system.
 *
 * Note: __ss_{len,family} is defined in RFC2553.  During RFC2553 discussion
 * the field name went back and forth between ss_len and __ss_len,
 * and RFC2553 specifies it to be __ss_len.  openbsd picked ss_len.
 * For maximum portability, userland programmer would need to
 * (1) make the code never touch ss_len portion (cast it into sockaddr and
 * touch sa_len), or (2) add "-Dss_len=__ss_len" into CFLAGS to unify all
 * occurrences (including header file) to __ss_len.
 */
struct sockaddr_storage {
	uint8_t	ss_len;		///< Total length. 
	sa_family_t	ss_family;	///< Address family.
	unsigned char	__ss_pad1[6];	///< Align to quad. 
	uint64_t	__ss_pad2;	///< Force alignment for stupid compilers. 
	unsigned char	__ss_pad3[240];	///< Pad to a total of 256 bytes. 
};

#ifdef _KERNEL
/*
 * Structure used by kernel to pass protocol
 * information in raw sockets.
 */
struct sockproto {
	unsigned short	sp_family;	/* address family */
	unsigned short	sp_protocol;	/* protocol */
};
#endif /* _KERNEL */

/*
 * Protocol families, same as address families for now.
 */
#define	PF_UNSPEC	AF_UNSPEC
#define	PF_LOCAL	AF_LOCAL
#define	PF_UNIX		AF_UNIX
#define	PF_INET		AF_INET
#define	PF_IMPLINK	AF_IMPLINK
#define	PF_PUP		AF_PUP
#define	PF_CHAOS	AF_CHAOS
#define	PF_NS		AF_NS
#define	PF_ISO		AF_ISO
#define	PF_OSI		AF_ISO
#define	PF_ECMA		AF_ECMA
#define	PF_DATAKIT	AF_DATAKIT
#define	PF_CCITT	AF_CCITT
#define	PF_SNA		AF_SNA
#define PF_DECnet	AF_DECnet
#define PF_DLI		AF_DLI
#define PF_LAT		AF_LAT
#define	PF_HYLINK	AF_HYLINK
#define	PF_APPLETALK	AF_APPLETALK
#define	PF_ROUTE	AF_ROUTE
#define	PF_LINK		AF_LINK
#define	PF_XTP		pseudo_AF_XTP	/* really just proto family, no AF */
#define	PF_COIP		AF_COIP
#define	PF_CNT		AF_CNT
#define	PF_IPX		AF_IPX		/* same format as AF_NS */
#define PF_INET6	AF_INET6
#define PF_RTIP		pseudo_AF_RTIP	/* same format as AF_INET */
#define PF_PIP		pseudo_AF_PIP
#define PF_ISDN		AF_ISDN
#define PF_NATM		AF_NATM
#define PF_ENCAP	AF_ENCAP
#define	PF_SIP		AF_SIP
#define PF_KEY		AF_KEY
#define PF_BPF		pseudo_AF_HDRCMPLT
#define	PF_BLUETOOTH	AF_BLUETOOTH
#define PF_MPLS		AF_MPLS
#define PF_PFLOW	pseudo_AF_PFLOW
#define PF_PIPEX	pseudo_AF_PIPEX
#define	PF_MAX		AF_MAX

/*
 * These are the valid values for the "how" field used by shutdown(2).
 */
#define	SHUT_RD		0
#define	SHUT_WR		1
#define	SHUT_RDWR	2

#if __BSD_VISIBLE
#define SA_LEN(x) ((x)->sa_len)

/*
 * Definitions for network related sysctl, CTL_NET.
 *
 * Second level is protocol family.
 * Third level is protocol number.
 *
 * Further levels are defined by the following individual families.
 */
#define NET_MAXID	AF_MAX

#define CTL_NET_NAMES { \
	{ 0, 0 }, \
	{ "unix", CTLTYPE_NODE }, \
	{ "inet", CTLTYPE_NODE }, \
	{ "implink", CTLTYPE_NODE }, \
	{ "pup", CTLTYPE_NODE }, \
	{ "chaos", CTLTYPE_NODE }, \
	{ "xerox_ns", CTLTYPE_NODE }, \
	{ "iso", CTLTYPE_NODE }, \
	{ "ecma", CTLTYPE_NODE }, \
	{ "datakit", CTLTYPE_NODE }, \
	{ "ccitt", CTLTYPE_NODE }, \
	{ "ibm_sna", CTLTYPE_NODE }, \
	{ "decnet", CTLTYPE_NODE }, \
	{ "dec_dli", CTLTYPE_NODE }, \
	{ "lat", CTLTYPE_NODE }, \
	{ "hylink", CTLTYPE_NODE }, \
	{ "appletalk", CTLTYPE_NODE }, \
	{ "route", CTLTYPE_NODE }, \
	{ "link", CTLTYPE_NODE }, \
	{ "xtp", CTLTYPE_NODE }, \
	{ "coip", CTLTYPE_NODE }, \
	{ "cnt", CTLTYPE_NODE }, \
	{ "rtip", CTLTYPE_NODE }, \
	{ "ipx", CTLTYPE_NODE }, \
	{ "inet6", CTLTYPE_NODE }, \
	{ "pip", CTLTYPE_NODE }, \
	{ "isdn", CTLTYPE_NODE }, \
	{ "natm", CTLTYPE_NODE }, \
	{ "encap", CTLTYPE_NODE }, \
	{ "sip", CTLTYPE_NODE }, \
	{ "key", CTLTYPE_NODE }, \
	{ "bpf", CTLTYPE_NODE }, \
	{ "bluetooth", CTLTYPE_NODE }, \
	{ "mpls", CTLTYPE_NODE }, \
	{ "pflow", CTLTYPE_NODE }, \
	{ "pipex", CTLTYPE_NODE }, \
}

/*
 * PF_ROUTE - Routing table
 *
 * Four additional levels are defined:
 *	Fourth: address family, 0 is wildcard
 *	Fifth: type of info, defined below
 *	Sixth: flag(s) to mask with for NET_RT_FLAGS
 *	Seventh: routing table to use (facultative, defaults to 0)
 *		 NET_RT_TABLE has the table id as sixth element.
 */
#define NET_RT_DUMP	1		/* dump; may limit to a.f. */
#define NET_RT_FLAGS	2		/* by flags, e.g. RESOLVING */
#define NET_RT_IFLIST	3		/* survey interface list */
#define	NET_RT_STATS	4		/* routing table statistics */
#define	NET_RT_TABLE	5
#define	NET_RT_IFNAMES	6
#define	NET_RT_SOURCE	7
#define	NET_RT_MAXID	8

#define CTL_NET_RT_NAMES { \
	{ 0, 0 }, \
	{ "dump", CTLTYPE_STRUCT }, \
	{ "flags", CTLTYPE_STRUCT }, \
	{ "iflist", CTLTYPE_STRUCT }, \
	{ "stats", CTLTYPE_STRUCT }, \
	{ "table", CTLTYPE_STRUCT }, \
	{ "ifnames", CTLTYPE_STRUCT }, \
	{ "source", CTLTYPE_STRUCT }, \
}

/*
 * PF_UNIX - unix socket tunables
 */
#define NET_UNIX_INFLIGHT	6
#define NET_UNIX_DEFERRED	7
#define NET_UNIX_MAXID		8

#define CTL_NET_UNIX_NAMES { \
	{ 0, 0 }, \
	{ "stream", CTLTYPE_NODE }, \
	{ "dgram", CTLTYPE_NODE }, \
	{ 0, 0 }, \
	{ 0, 0 }, \
	{ "seqpacket", CTLTYPE_NODE }, \
	{ "inflight", CTLTYPE_INT }, \
	{ "deferred", CTLTYPE_INT }, \
}

#define UNPCTL_RECVSPACE	1
#define UNPCTL_SENDSPACE	2
#define NET_UNIX_PROTO_MAXID	3

#define CTL_NET_UNIX_PROTO_NAMES { \
	{ 0, 0 }, \
	{ "recvspace", CTLTYPE_INT }, \
	{ "sendspace", CTLTYPE_INT }, \
}

/*
 * PF_LINK - link layer or device tunables
 */
#define NET_LINK_IFRXQ		1	/* net.link.ifrxq */
#define NET_LINK_MAXID		2

#define CTL_NET_LINK_NAMES { \
	{ 0, 0 }, \
	{ "ifrxq", CTLTYPE_NODE }, \
}

#define NET_LINK_IFRXQ_PRESSURE_RETURN \
				1	/* net.link.ifrxq.pressure_return */
#define NET_LINK_IFRXQ_PRESSURE_DROP \
				2	/* net.link.ifrxq.pressure_drop */
#define NET_LINK_IFRXQ_MAXID	3

#define CTL_NET_LINK_IFRXQ_NAMES { \
	{ 0, 0 }, \
	{ "pressure_return", CTLTYPE_INT }, \
	{ "pressure_drop", CTLTYPE_INT }, \
}

/*
 * PF_KEY - Key Management
 */
#define NET_KEY_SADB_DUMP	1	/* return SADB */
#define NET_KEY_SPD_DUMP	2	/* return SPD */
#define NET_KEY_MAXID		3

#define CTL_NET_KEY_NAMES { \
	{ 0, 0 }, \
	{ "sadb_dump", CTLTYPE_STRUCT }, \
	{ "spd_dump", CTLTYPE_STRUCT }, \
}

/*
 * PF_BPF  not really a family, but connected under CTL_NET
 */
#define NET_BPF_BUFSIZE		1		/* default buffer size */
#define NET_BPF_MAXBUFSIZE	2		/* maximum buffer size */
#define NET_BPF_MAXID		3

#define CTL_NET_BPF_NAMES { \
	{ 0, 0 }, \
	{ "bufsize", CTLTYPE_INT }, \
	{ "maxbufsize", CTLTYPE_INT }, \
}

/*
 * PF_PFLOW not really a family, but connected under CTL_NET
 */
#define NET_PFLOW_STATS		1		/* statistics */
#define NET_PFLOW_MAXID		2

#define CTL_NET_PFLOW_NAMES { \
	{ 0, 0 }, \
	{ "stats", CTLTYPE_STRUCT }, \
}
#endif /* __BSD_VISIBLE */

/*
 * Maximum queue length specifiable by listen(2).
 */
#define	SOMAXCONN	128

/**
 * Message header for recvmsg and sendmsg calls.
 * Used value-result for recvmsg, value only for sendmsg.
 */
struct msghdr {
	void		*msg_name;	///< Optional address. 
	socklen_t	msg_namelen;	///< Size of address.
	struct		iovec *msg_iov;	///< Scatter/gather array. 
	unsigned int	msg_iovlen;	///< # elements in msg_iov. 
	void		*msg_control;	///< Ancillary data, see below. 
	socklen_t	msg_controllen;	///< Ancillary data buffer len. 
	int		msg_flags;	///< Flags on received message. 
};
/// Message Header 
struct mmsghdr {
	struct msghdr msg_hdr; ///< Message header
	unsigned int msg_len; ///< Message len
};

struct timespec;

#define	MSG_OOB			0x1	/* process out-of-band data */
#define	MSG_PEEK		0x2	/* peek at incoming message */
#define	MSG_DONTROUTE		0x4	/* send without using routing tables */
#define	MSG_EOR			0x8	/* data completes record */
#define	MSG_TRUNC		0x10	/* data discarded before delivery */
#define	MSG_CTRUNC		0x20	/* control data lost before delivery */
#define	MSG_WAITALL		0x40	/* wait for full request or error */
#define	MSG_DONTWAIT		0x80	/* this message should be nonblocking */
#define	MSG_BCAST		0x100	/* this message rec'd as broadcast */
#define	MSG_MCAST		0x200	/* this message rec'd as multicast */
#define	MSG_NOSIGNAL		0x400	/* do not send SIGPIPE */
#define	MSG_CMSG_CLOEXEC	0x800	/* set FD_CLOEXEC on received fds */
#define	MSG_WAITFORONE		0x1000	/* nonblocking but wait for one msg */

/**
 * Header for ancillary data objects in msg_control buffer.
 * Used for additional information with/about a datagram
 * not expressible by flags.  The format is a sequence
 * of message elements headed by cmsghdr structures.
 */
struct cmsghdr {
	socklen_t	cmsg_len;	///< Data byte count, including hdr. 
	int		cmsg_level;	///< Originating protocol. 
	int		cmsg_type;	///< Protocol-specific type. 
/* followed by	u_char  cmsg_data[]; */
};

/* given pointer to struct cmsghdr, return pointer to data */
#define	CMSG_DATA(cmsg) \
	((unsigned char *)(cmsg) + _ALIGN(sizeof(struct cmsghdr)))

/* given pointer to struct cmsghdr, return pointer to next cmsghdr */
#define	CMSG_NXTHDR(mhdr, cmsg)	\
	(((char *)(cmsg) + _ALIGN((cmsg)->cmsg_len) + \
			    _ALIGN(sizeof(struct cmsghdr)) > \
	    ((char *)(mhdr)->msg_control) + (mhdr)->msg_controllen) ? \
	    (struct cmsghdr *)NULL : \
	    (struct cmsghdr *)((char *)(cmsg) + _ALIGN((cmsg)->cmsg_len)))

/*
 * RFC 2292 requires to check msg_controllen, in case that the kernel returns
 * an empty list for some reasons.
 */
#define	CMSG_FIRSTHDR(mhdr) \
	((mhdr)->msg_controllen >= sizeof(struct cmsghdr) ? \
	 (struct cmsghdr *)(mhdr)->msg_control : \
	 (struct cmsghdr *)NULL)

/* Round len up to next alignment boundary */
#ifdef _KERNEL
#define CMSG_ALIGN(n)		_ALIGN(n)
#endif

/* Length of the contents of a control message of length len */
#define	CMSG_LEN(len)	(_ALIGN(sizeof(struct cmsghdr)) + (len))

/* Length of the space taken up by a padded control message of length len */
#define	CMSG_SPACE(len)	(_ALIGN(sizeof(struct cmsghdr)) + _ALIGN(len))

/* "Socket"-level control message types: */
#define	SCM_RIGHTS	0x01		/* access rights (array of int) */
#define	SCM_TIMESTAMP	0x04		/* timestamp (struct timeval) */

#ifndef _KERNEL

/* __BEGIN_DECLS */

/**
 * @addtogroup BSD_SOCKET_FUNCTIONS
 * @{ 
 */

/***************************************************************************/ 
/**
 * @brief
 *   Create an endpoint for communication.
 * 
 * @details
 *   The `socket()` function creates an endpoint for communication and returns a file descriptor that refers to that endpoint. 
 *   The file descriptor returned by a successful call would be the lowest-numbered file descriptor not currently open for the process.
 * 
 * @param[in] domain
 *   Specifies a communication domain, selecting the protocol family to be used for communication. 
 *   Must be one of the values from @ref BSD_SOCKET_FAMILIY.
 * 
 * @param[in] type
 *   Specifies the semantics of communication for the socket. 
 *   Must be one of the values from BSD_SOCKET_TYPES. 
 *   Currently defined types are SOCK_STREAM, SOCK_DGRAM, SOCK_RAW, SOCK_RDM, and SOCK_SEQPACKET.
 * 
 * @param[in] protocol
 *   Specifies a particular protocol to be used with the socket. Normally, only a single protocol exists to support a particular socket type 
 *   within a given protocol family. However, if multiple protocols exist, a specific protocol must be specified. 
 *   The protocol number to use is specific to the communication domain. Must be one of the values from @ref BSD_SOCKET_PROTOCOL.
 * 
 * @return 
 *   Returns a descriptor referencing the socket if successful. 
 *   Returns -1 if an error occurs.
 ******************************************************************************/
int socket(int domain, int type, int protocol);

/***************************************************************************/ 
/**
 * @brief
 *   Bind a name to a socket.
 * 
 * @details
 *   When a socket is created with `socket()`, it exists in a namespace (address family) but, has no address assigned to it. 
 *   The `bind()` function assigns the address specified by `addr` to the socket referred to by the file descriptor `socket_id`.  
 *   Traditionally, this operation is called "assigning a name to a socket".
 * 
 *   It is normally necessary to assign a local address using `bind()` before a `SOCK_STREAM` socket may receive connections.
 * 
 * @param[in] socket_id
 *   Socket identification number.
 * 
 * @param[in] addr 
 *   Pointer to a @ref sockaddr structure containing the address to be assigned to the socket.
 * 
 * @param[in] addr_len 
 *   The `addr_len` parameter specifies the size, in bytes, of the address structure pointed to by `addr`.
 * 
 * @return
 *   Returns 0 if successful. Otherwise, returns -1 and sets the global variable `errno` to indicate the error.
 ******************************************************************************/
int bind(int socket_id, const struct sockaddr *addr, socklen_t addr_len);

/***************************************************************************/ 
/**
 * @brief
 *   Listen for connections on a socket.
 * 
 * @details
 *   The `listen()` function marks the socket referred to by `socket_id` as a passive socket, 
 *   that is, as a socket that would be used to accept incoming connection requests using `accept()`.
 *   This function applies only to sockets of type `SOCK_STREAM` or `SOCK_SEQPACKET`.
 * 
 * @pre
 *   To accept connections, a socket must first be created with @ref socket(). 
 *   A willingness to accept incoming connections and a queue limit for incoming connections 
 *   must be specified with @ref listen(), and then the connections can be accepted with @ref accept().
 * 
 * @param[in] socket_id
 *   Socket identification number.
 * 
 * @param[in] backlog
 *   The backlog argument defines the maximum number of pending connections that can be queued.
 * 
 * @return
 *   Returns 0 if successful. Otherwise, returns -1 and sets the global variable `errno` to indicate the error.
 ******************************************************************************/
int listen(int socket_id, int backlog);

/***************************************************************************/ 
/**
 * @brief
 *   Accept a connection on a socket.
 * 
 * @details
 *   The `accept()` system call is used with connection-based socket types (for example, `SOCK_STREAM`, `SOCK_SEQPACKET`). 
 *   It extracts the first connection request on the queue of pending connections for the listening socket (`socket_id`), 
 *   creates a new connected socket, and returns a new file descriptor referring to that socket. 
 *   The newly created socket is not in the listening state. The original socket (`socket_id`) is unaffected by this call.
 * 
 *   If no pending connections are present on the queue and the original socket is not marked as non-blocking, 
 *   this function blocks the caller until a connection is present. If the original socket is marked non-blocking and 
 *   no pending connections are present on the queue, @ref accept() returns an error. The accepted socket may not be 
 *   used to accept more connections. The original socket (`socket_id`) remains open.
 * 
 * @param[in] socket_id
 *   Socket identification number of the socket to accept. This socket must have been created with @ref socket(), 
 *   bound to an address with @ref bind(), and set to listen for connections with @ref listen().
 * 
 * @param[out] addr 
 *   Pointer to a `sockaddr` structure that will be filled with the address of the connecting entity, as known
 *   to the communications layer. The exact format of the `addr` argument is determined by the domain in which the 
 *   communication is occurring. A null pointer may be specified for `addr` if the address information is not desired; 
 *   in this case, `addr_len` is not used and should also be null.
 * 
 * @param[in, out] addr_len 
 *   Pointer to a `socklen_t` variable. Initially, it should contain the amount of space pointed to by `addr`. 
 *   On return, it will contain the actual length (in bytes) of the address returned. This call is used with 
 *   connection-based socket types, currently with `SOCK_STREAM`.
 * 
 * @return
 *   Returns a non-negative integer that is a descriptor for the accepted socket if successful. 
 *   Returns -1 on error and sets the global variable `errno` to indicate the error.
 * 
 * @note 
 *   The @ref accept() system call only supports blocking mode.
 ******************************************************************************/
int accept(int socket_id, struct sockaddr *addr, socklen_t *addr_len);

/***************************************************************************/ 
/**
 * @brief
 *   Initiate a connection on a socket.
 * 
 * @details
 *   The @ref connect() system call connects the socket referred to by the file descriptor `socket_id` to the address specified by `addr`.  
 *   The `addr_len` argument specifies the size of `addr`. The format of the address in `addr` is determined by 
 *   the address space of the socket `socket_id`; see @ref socket() for further details.
 * 
 *   If `socket_id` is of type `SOCK_DGRAM`, the @ref connect() system call specifies the peer with which the socket is to be associated.
 *   If the socket is of type `SOCK_STREAM`, the @ref connect() system call attempts to make a connection to another socket. The other 
 *   socket is specified by `addr`, which is an address in the communications space of the socket. Each communications space interprets
 *   the `addr` argument in its own way. Generally, stream sockets may successfully @ref connect() only once; datagram sockets can use 
 *   @ref connect() multiple times to change their association.
 * 
 * @param[in] socket_id
 *   The socket identification number.
 * 
 * @param[in] addr 
 *   The addr argument of type @ref sockaddr is the address is that to which datagrams are to be sent.
 * 
 * @param[in] addr_len 
 *   The addr_len argument of type @ref socklen_t indicates the amount of space pointed to by addr, in bytes.
 * 
 * @return
 *   Returns 0 if successful. Otherwise, returns -1 and sets the global variable `errno` to indicate the error.
 * 
 * @note
 *   Connecting to an invalid address, such as a null address, will result in an `EFAULT` error.
 ******************************************************************************/
int connect(int socket_id, const struct sockaddr *addr, socklen_t addr_len);

/***************************************************************************/ 
/**
 * @brief
 *   Receive a message from a socket.
 *  
 * @details
 *   The @ref recv() function is normally used only on a connected socket. It receives messages from a socket and returns the length of the message on successful completion. 
 *   If a message is too long to fit in the supplied buffer, excess bytes may be discarded depending on the type of socket the message is received from.
 *   By default, @ref recv() is a blocking API. To use it in a non-blocking manner, you can set the socket to non-blocking mode using `setsockopt()`.
 * 
 * @param[in] socket_id
 *   Socket identification number.
 * 
 * @param[out] buf
 *   Pointer to the buffer that receives the data.
 * 
 * @param[in] buf_len
 *   Length of the buffer pointed to by the `buf` parameter.
 * 
 * @param[in] flags
 *   Controls the reception of the data. Currently, no flags are supported and this parameter should be set to 0.
 * 
 * @return
 *   ssize_t
 * 
 * @note 
 *   The @ref recv() system call does not support any flags.
 ******************************************************************************/
ssize_t recv(int socket_id, void *buf, size_t buf_len, int flags);

/***************************************************************************/ 
/**
 * @brief
 *   Receive a message from a socket.
 * 
 * @details
 *   The @ref recvfrom() system call is used to receive messages from a socket and can be used to receive data on a socket 
 *   whether or not it is connection-oriented. If `from_addr` is not a null pointer and the socket is not connection-oriented, 
 *   the source address of the message is filled in.
 * 
 * @param[in] socket_id
 *   The socket identification number.
 * 
 * @param[out] buf
 *   Pointer to the buffer that receives the data.
 * 
 * @param[in] buf_len
 *   The length of the buffer pointed to by the `buf` parameter, in bytes.
 * 
 * @param[in] flags
 *   Controls the reception of the data. Currently, no flags are supported and this parameter should be set to 0.
 * 
 * @param[out] from_addr 
 *   Pointer to a socket address structure of type @ref sockaddr that will be filled with the source address of the received message. 
 *   If the source address is not required, this parameter can be NULL.
 * 
 * @param[in, out] from_addr_len 
 *   A value-result argument of type @ref socklen_t. Initially, it should contain the size of the buffer associated with `from_addr`. 
 *   On return, it will be modified to indicate the actual size of the address stored there. If `from_addr` is NULL, this parameter is ignored.
 * 
 * @return
 *   Returns the length of the message on successful completion. Returns -1 on error and sets the global variable `errno` to indicate the error.
 * 
 * @note 
 *   The @ref recvfrom() system call does not support any flags.
 ******************************************************************************/
ssize_t recvfrom(int socket_id, void *buf, size_t buf_len, int flags, struct sockaddr *from_addr, socklen_t *from_addr_len);

/***************************************************************************/ 
/**
 * @brief
 *   Send a message on a socket.
 * 
 * @details
 *   The @ref send() function is used to transmit a message on a socket. This function can only be used when the socket is in a connected state. 
 *   If the socket is connection-oriented, the protocol must support implied connect or the socket must be explicitly connected before use. 
 *   No indication of failure to deliver is implicit in a @ref send(). Locally detected errors are indicated by a return value of -1. 
 *   If no message space is available at the socket to hold the message to be transmitted, then @ref send() normally blocks, unless the socket 
 *   has been placed in non-blocking I/O mode.
 * 
 * @param[in] socket_id
 *   The socket identification number.
 * 
 * @param[in] buf
 *   Pointer to the buffer containing the message to transmit.
 * 
 * @param[in] buf_len
 *   The length of the message in bytes.
 * 
 * @param[in] flags
 *   Controls the transmission of the data. Currently, no flags are supported and this parameter should be set to 0.
 * 
 * @return
 *   Returns the number of bytes sent on success. Returns -1 on error and sets the global variable `errno` to indicate the error.
 * 
 * @note 
 *   - The @ref send() system call currently supports only blocking mode.
 *   - The @ref send() system call does not guarantee that the packets are transmitted to the remote node; they are enqueued in the local queue.
 *   - The @ref send() system call does not support any flags.
 *   - The @ref send() system call can send a maximum of 1460 bytes in the case of plain TCP/UDP. For TLS, the maximum buffer length is 1370 bytes.
 ******************************************************************************/
ssize_t send(int socket_id, const void *buf, size_t buf_len, int flags);

/***************************************************************************/ 
/**
 * @brief
 *   Send a message on a socket.
 * 
 * @details
 *   The @ref sendto() function is used to transmit a message on a socket. This function can be used at any time if the socket is in connectionless mode. 
 *   If the socket is in connection mode, the protocol must support implied connect or the socket must be in a connected state before use. 
 *   The address of the target is specified by `to_addr` with `to_addr_len` indicating its size. If the socket is in a connected state, 
 *   the target address passed to @ref sendto() is ignored. If the message is too long to pass atomically through the protocol, 
 *   the error `EMSGSIZE` is returned, and the message is not transmitted.
 * 
 * @param[in] socket_id
 *   The socket identification number.
 * 
 * @param[in] buf
 *   Pointer to the buffer containing the message to transmit.
 * 
 * @param[in] buf_len
 *   The length of the message in bytes.
 * 
 * @param[in] flags
 *   Controls the transmission of the data. Due to firmware limitations, @ref sendto() does not support any flags and this parameter should be set to 0.
 * 
 * @param[in] to_addr 
 *   Pointer to a `sockaddr` structure containing the address of the target.
 * 
 * @param[in] to_addr_len 
 *   Size of the address structure pointed to by `to_addr`, in bytes.
 * 
 * @return
 *   The number of octets sent. If an error occurred, a value of -1 is returned.
 * 
 * @note 
 *   - The @ref sendto() system call can only send a maximum of 1460 bytes in the case of plain TCP/UDP. For TLS, the maximum buffer length is 1370 bytes.
 *   - The @ref sendto() system call does not support any flags.
 ******************************************************************************/
ssize_t sendto(int socket_id, const void *buf, size_t buf_len, int flags, const struct sockaddr *to_addr, socklen_t to_addr_len);

/***************************************************************************/ 
/**
 * @brief
 *   Set options on a socket.
 * 
 * @details
 *   The @ref setsockopt() system call manipulates the options associated with a socket. Options may exist at multiple protocol levels; 
 *   they are always present at the uppermost "socket" level. When manipulating socket options, the level at which the option resides 
 *   and the name of the option must be specified. To manipulate options at the socket level, `option_level` is specified as `SOL_SOCKET`. 
 *   To manipulate options at any other level, the protocol number of the appropriate protocol controlling the option is supplied. 
 *   For example, to indicate that an option is to be interpreted by the TCP protocol, `option_level` should be set to the protocol number of TCP. 
 *   The `option_value` and `option_length` arguments are used to access option values for @ref setsockopt().
 * 
 * @param[in] socket_id
 *   The socket identification number.
 * 
 * @param[in] option_level
 *   The level at which the option is being set. One of the values from @ref BSD_SOCKET_OPTION_LEVEL.
 * 
 * @param[in] option_name
 *   The name of the option to set. One of the values from @ref BSD_SOCKET_OPTION_NAME. The `option_name` argument and any specified options 
 *   are passed uninterpreted to the appropriate protocol module for interpretation.
 * 
 * @param[in] option_value
 *   A pointer to the buffer containing the value for the option. Most socket-level options utilize an `int` argument for `option_value`. 
 *   For @ref setsockopt(), the argument should be non-zero to enable a boolean option, or zero if the option is to be disabled.
 * 
 * @param[in] option_length
 *   The length of the option data, in bytes, pointed to by `option_value`.
 * 
 * @return
 *   Returns 0 on successful completion. Returns -1 on error and sets the global variable `errno` to indicate the error.
 * 
 * @note 
 *   The following options are currently supported:
 *   - `SO_RCVTIMEO`: Receive timeout.
 *   - `SO_KEEPALIVE`: Keep connections alive.
 *   - `TCP_ULP`: TCP upper layer protocol.
 *   - `SO_MAX_RETRANSMISSION_TIMEOUT_VALUE`: Maximum retransmission timeout value.
 *   - `IP_TOS`: Type of service.
 ******************************************************************************/
int setsockopt(int socket_id, int option_level, int option_name, const void *option_value, socklen_t option_length);

/***************************************************************************/ 
/**
 * @brief
 *   Get options on a socket.
 * 
 * @details
 *   The @ref getsockopt() system call retrieves the options associated with a socket. Options may exist at multiple protocol levels; 
 *   they are always present at the uppermost "socket" level. When retrieving socket options, the level at which the option resides 
 *   and the name of the option must be specified. To retrieve options at the socket level, `option_level` is specified as `SOL_SOCKET`. 
 *   To retrieve options at any other level, the protocol number of the appropriate protocol controlling the option is supplied. 
 *   For example, to indicate that an option is to be interpreted by the TCP protocol, `option_level` should be set to the protocol number of TCP. 
 *   The `option_value` and `option_length` arguments identify a buffer in which the value for the requested options are to be returned.
 * 
 * @param[in] socket_id
 *   The socket identification number.
 * 
 * @param[in] option_level
 *   The level at which the option is defined. One of the values from @ref BSD_SOCKET_OPTION_LEVEL.
 * 
 * @param[in] option_name
 *   The name of the option to retrieve. One of the values from @ref BSD_SOCKET_OPTION_NAME. The `option_name` argument and any specified options 
 *   are passed uninterpreted to the appropriate protocol module for interpretation.
 * 
 * @param[out] option_value
 *   Pointer to a buffer where the retrieved option value will be stored.
 * 
 * @param[in, out] option_length
 *   A value-result argument of type @ref socklen_t. Initially, it should contain the size of the buffer pointed to by `option_value`. 
 *   On return, it will be modified to indicate the actual size of the value returned. If no option value is to be supplied or returned, 
 *   `option_value` may be NULL.
 * 
 * @return
 *   Returns 0 on successful completion. Returns -1 on error and sets the global variable `errno` to indicate the error.
 * 
 * @note 
 *   The following options are currently supported:
 *   - `SO_RCVTIMEO`: Receive timeout.
 *   - `SO_KEEPALIVE`: Keep connections alive.
 *   - `TCP_ULP`: TCP upper layer protocol.
 *   - `SO_MAX_RETRANSMISSION_TIMEOUT_VALUE`: Maximum retransmission timeout value.
 *   - `IP_TOS`: Type of service.
 ******************************************************************************/
int getsockopt(int socket_id, int option_level, int option_name, void *option_value, socklen_t *option_length);

/***************************************************************************/ 
/**
 * @brief
 *   Get the name of the connected peer socket.
 * 
 * @details
 *   The @ref getpeername() system call returns the address of the peer connected to the socket specified by `socket_id`. 
 *   The address is returned in the buffer pointed to by `name`, and the actual size of the address is returned in the variable 
 *   pointed to by `name_len`. If the buffer provided is too small, the address is truncated.
 * 
 * @param[in] socket_id
 *   The socket identification number.
 * 
 * @param[out] name 
 *   Pointer to a `sockaddr` structure that will be filled with the address of the peer socket.
 * 
 * @param[in, out] name_len
 *   A value-result argument of type @ref socklen_t. Initially, it should contain the size of the buffer pointed to by `name`. 
 *   On return, it will be modified to indicate the actual size of the address returned (in bytes).
 * 
 * @return
 *   Returns 0 on success. Returns -1 on error and sets the global variable `errno` to indicate the error.
 ******************************************************************************/
int getpeername(int socket_id, struct sockaddr *name, socklen_t *name_len);

/***************************************************************************/ 
/**
 * @brief
 *   Get the current address assigned to a socket.
 * 
 * @details
 *   The @ref getsockname() system call returns the current address to which the socket `socket_id` is bound. 
 *   The address is returned in the buffer pointed to by `name`, and the actual size of the address is returned in the variable 
 *   pointed to by `name_len`. If the buffer provided is too small, the address is truncated.
 * 
 * @param[in] socket_id
 *   The socket identification number.
 * 
 * @param[out] name 
 *   Pointer to a `sockaddr` structure that will be filled with the address of the socket.
 * 
 * @param[in, out] name_len
 *   A value-result argument of type @ref socklen_t. Initially, it should contain the size of the buffer pointed to by `name`. 
 *   On return, it will be modified to indicate the actual size of the address returned (in bytes).
 * 
 * @return
 *   Returns 0 on success. Returns -1 on error and sets the global variable `errno` to indicate the error.
 ******************************************************************************/
int getsockname(int socket_id, struct sockaddr *name, socklen_t *name_len);

/***************************************************************************/ 
/**
 * @brief 
 *   Close a socket.
 * 
 * @details
 *   The @ref close() function closes a socket identified by `socket_id`, so that it no longer refers to any file and can be reused. 
 *   This function releases the resources associated with the socket and makes the socket descriptor available for reuse.
 * 
 * @param[in] socket_id
 *   The socket identification number.
 * 
 * @return
 *   Returns 0 on success. Returns -1 on error and sets the global variable `errno` to indicate the error.
 * 
 * @note
 *   - Calling @ref close() on a server socket or the first client socket will close all associated sockets.
 *   - If @ref close() is called on a non-existing socket, the error `EBADF` (Bad file descriptor) will be set in `errno`.
 ******************************************************************************/
int close(int socket_id);

/** @} */

int	shutdown(int socket_id, int how);
//int	sockatmark(int);

//int	socketpair(int, int, int, int *);
int closesocket(int socket_id);

#if __BSD_VISIBLE
int	accept4(int, struct sockaddr *__restrict, socklen_t *__restrict, int);
#endif

/* __END_DECLS */

#else

static inline struct sockaddr *
sstosa(struct sockaddr_storage *ss)
{
	return ((struct sockaddr *)(ss));
}

#endif /* !_KERNEL */

#endif /* !COMPONENTS_BSD_SOCKET_INC_SOCKET_H_ */
