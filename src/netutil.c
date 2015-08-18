/*
 * netutil.c
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

#include "netutil.h"

static unsigned count_1_bits(void *buf, unsigned buflen) {
  uint8_t *bits = buf, mask;
  unsigned i, res = 0;

  for(mask = 0x80, i = 0; i < buflen; mask >>= 1) {
    if(!mask) {
      ++i;
      mask = 0x80;
    }

    if(bits[i] & mask)
      ++res;
    else
      break;
  }

  return res;
}

char *addr_to_string(struct sockaddr *saddr, char *buf, size_t buflen) {
  long err = 0;
  unsigned i;
  union addr *paddr = (union addr*)saddr;

  if(!saddr)
    strncpy(buf, "none", buflen);
  else {
    errno = 0;
    switch(saddr->sa_family) {
      case AF_INET:
        err = !inet_ntop(AF_INET, &paddr->sin.sin_addr, buf, buflen);
        break;
      case AF_INET6:
        err = !inet_ntop(AF_INET6, paddr->sin6.sin6_addr.s6_addr, buf, buflen);
        break;
      case AF_PACKET:
        for(i = 0; i < paddr->sll.sll_halen; ++i) {
          if(3 * i + 4 >= buflen) { /* if we would overrun the buffer */
            err = errno = ENOSPC;
            break;
          }
          snprintf(&buf[3 * i], 4, "%02x:", paddr->sll.sll_addr[i]);
        }
        buf[(3 * i > 0) ? (3 * i) - 1: 0] = '\0';
        break;
      default:
        snprintf(buf, buflen, "unknown address type %d", saddr->sa_family);
    }
  }

  if(err)
    snprintf(buf, buflen, "unable to print: %s",
      (errno == EAFNOSUPPORT) ? "address family not supported" :
      (errno == ENOSPC) ? "supplied buffer too small" :
                          "unknown error");
  return buf;
}

int netmask_to_string(struct sockaddr *saddr) {
  union addr *paddr = (union addr*)saddr;
  int res;

  switch(paddr->sa.sa_family) {
    case AF_INET:
      res = count_1_bits(&paddr->sin.sin_addr, sizeof paddr->sin.sin_addr);
      break;
    case AF_INET6:
      res = count_1_bits(paddr->sin6.sin6_addr.s6_addr, sizeof paddr->sin6.sin6_addr.s6_addr);
      break;
    case AF_PACKET:
      res = count_1_bits(paddr->sll.sll_addr, paddr->sll.sll_halen);
      break;
    default:
      res = -1;
  }

  return res;
}
