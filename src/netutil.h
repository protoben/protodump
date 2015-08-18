/*
 * netutil.h
 *
 * Copyright (c) 2014 Ben Hamlin <protob3n@gmail.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *                       __            __                    
 *     ____  _________  / /_____  ____/ /_  ______ ___  ____ 
 *    / __ \/ ___/ __ \/ __/ __ \/ __  / / / / __ `__ \/ __ \
 *   / /_/ / /  / /_/ / /_/ /_/ / /_/ / /_/ / / / / / / /_/ /
 *  / .___/_/   \____/\__/\____/\__,_/\__,_/_/ /_/ /_/ .___/ 
 * /_/                                              /_/      
 *
 */

#ifndef PROTODUMP_NETUTIL_H
#define PROTODUMP_NETUTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netpacket/packet.h>

typedef union addr pd_addr_t;
union addr {
  struct sockaddr sa;
  struct sockaddr_storage ss;
  struct sockaddr_in sin;     /* ipv4 socket */
  struct sockaddr_in6 sin6;   /* ipv6 socket */
  struct sockaddr_ll sll;     /* packet socket */
};

/**
 * Take a struct sockaddr, parse its address family, and produce a string.
 *
 * saddr:  The struct sockaddr to parse
 * buf:    Buffer to write the string to
 * buflen: Length of buf
 */
char *addr_to_string(struct sockaddr *saddr, char *buf, size_t buflen);

/**
 * Taket a struct sockaddr, interpret it as a netmask, and return the
 * corresponding integer in the range [0, address_bit_width]. On error,
 * return -1.
 *
 * saddr: The struct sockaddr to parse
 */
int netmask_to_string(struct sockaddr *saddr);

#endif
