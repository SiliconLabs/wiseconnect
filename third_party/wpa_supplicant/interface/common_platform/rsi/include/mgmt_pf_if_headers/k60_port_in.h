/* Copyright (C) 1991-2001, 2003, 2004, 2006 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#ifndef _NETINET_IN_H
#define _NETINET_IN_H 1

/* Standard well-defined IP protocols.  */
enum {
  IPPROTO_IP = 0, /* Dummy protocol for TCP.  */
#define IPPROTO_IP IPPROTO_IP
  IPPROTO_HOPOPTS = 0, /* IPv6 Hop-by-Hop options.  */
#define IPPROTO_HOPOPTS IPPROTO_HOPOPTS
  IPPROTO_ICMP = 1, /* Internet Control Message Protocol.  */
#define IPPROTO_ICMP IPPROTO_ICMP
  IPPROTO_IGMP = 2, /* Internet Group Management Protocol. */
#define IPPROTO_IGMP IPPROTO_IGMP
  IPPROTO_IPIP = 4, /* IPIP tunnels (older KA9Q tunnels use 94).  */
#define IPPROTO_IPIP IPPROTO_IPIP
  IPPROTO_TCP = 6, /* Transmission Control Protocol.  */
#define IPPROTO_TCP IPPROTO_TCP
  IPPROTO_EGP = 8, /* Exterior Gateway Protocol.  */
#define IPPROTO_EGP IPPROTO_EGP
  IPPROTO_PUP = 12, /* PUP protocol.  */
#define IPPROTO_PUP IPPROTO_PUP
  IPPROTO_UDP = 17, /* User Datagram Protocol.  */
#define IPPROTO_UDP IPPROTO_UDP
  IPPROTO_IDP = 22, /* XNS IDP protocol.  */
#define IPPROTO_IDP IPPROTO_IDP
  IPPROTO_TP = 29, /* SO Transport Protocol Class 4.  */
#define IPPROTO_TP IPPROTO_TP
  IPPROTO_IPV6 = 41, /* IPv6 header.  */
#define IPPROTO_IPV6 IPPROTO_IPV6
  IPPROTO_ROUTING = 43, /* IPv6 routing header.  */
#define IPPROTO_ROUTING IPPROTO_ROUTING
  IPPROTO_FRAGMENT = 44, /* IPv6 fragmentation header.  */
#define IPPROTO_FRAGMENT IPPROTO_FRAGMENT
  IPPROTO_RSVP = 46, /* Reservation Protocol.  */
#define IPPROTO_RSVP IPPROTO_RSVP
  IPPROTO_GRE = 47, /* General Routing Encapsulation.  */
#define IPPROTO_GRE IPPROTO_GRE
  IPPROTO_ESP = 50, /* encapsulating security payload.  */
#define IPPROTO_ESP IPPROTO_ESP
  IPPROTO_AH = 51, /* authentication header.  */
#define IPPROTO_AH IPPROTO_AH
  IPPROTO_ICMPV6 = 58, /* ICMPv6.  */
#define IPPROTO_ICMPV6 IPPROTO_ICMPV6
  IPPROTO_NONE = 59, /* IPv6 no next header.  */
#define IPPROTO_NONE IPPROTO_NONE
  IPPROTO_DSTOPTS = 60, /* IPv6 destination options.  */
#define IPPROTO_DSTOPTS IPPROTO_DSTOPTS
  IPPROTO_MTP = 92, /* Multicast Transport Protocol.  */
#define IPPROTO_MTP IPPROTO_MTP
  IPPROTO_ENCAP = 98, /* Encapsulation Header.  */
#define IPPROTO_ENCAP IPPROTO_ENCAP
  IPPROTO_PIM = 103, /* Protocol Independent Multicast.  */
#define IPPROTO_PIM IPPROTO_PIM
  IPPROTO_COMP = 108, /* Compression Header Protocol.  */
#define IPPROTO_COMP IPPROTO_COMP
  IPPROTO_SCTP = 132, /* Stream Control Transmission Protocol.  */
#define IPPROTO_SCTP IPPROTO_SCTP
  IPPROTO_RAW = 255, /* Raw IP packets.  */
#define IPPROTO_RAW IPPROTO_RAW
  IPPROTO_MAX
};

/* Internet address.  */
typedef uint32 in_addr_t;
struct in_addr {
  in_addr_t s_addr;
};

/* IPv6 address */
struct in6_addr {
  union {
    uint8 u6_addr8[16];
    uint16 u6_addr16[8];
    uint32 u6_addr32[4];
  } in6_u;
#define s6_addr   in6_u.u6_addr8
#define s6_addr16 in6_u.u6_addr16
#define s6_addr32 in6_u.u6_addr32
};

extern const struct in6_addr in6addr_any;      /* :: */
extern const struct in6_addr in6addr_loopback; /* ::1 */

#define INET_ADDRSTRLEN  16
#define INET6_ADDRSTRLEN 46

/* IPv4 multicast request.  */
struct ip_mreq {
  /* IP multicast address of group.  */
  struct in_addr imr_multiaddr;

  /* Local IP address of interface.  */
  struct in_addr imr_interface;
};

#endif /* netinet/in.h */

/**
*$Log: k60_port_in.h,v $
*Revision 1.1.1.1  2012/02/28 07:14:12  kiran.gunda
*WiSe Connet initial import
*
**/
