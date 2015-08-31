/*
 * capture.c
 *
 * The MIT License (MIT)
 * 
 * Copyright (c) 2014 Ben Hamlin
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
 */

#include "capture.h"

static bool regex_matches_or_is_null(const char *regex, const char *test) {
  bool match;
  int err;
  regex_t reg;
  char errbuf[1024];

  if(!regex)
    return true;

  err = regcomp(&reg, regex, REG_EXTENDED | REG_NOSUB);
  if(err) {
    regerror(err, &reg, errbuf, sizeof errbuf);
    die(0, "regcomp(): %s", errbuf);
  }

  match = !regexec(&reg, test, 0, NULL, 0);

  regfree(&reg);
  return match;
}

static bool check_capture(char *errbuf, const char *dev) {
  int err;
  pcap_t *handle;

  handle = pcap_create(dev, errbuf);
  if(!handle)
    goto fail;

  err = pcap_activate(handle);
  if(err)
    strncpy(errbuf, pcap_geterr(handle), PCAP_ERRBUF_SIZE);
  if(err && err != PCAP_WARNING)
    goto fail;

  pcap_close(handle);
  return true;

fail:
  pcap_close(handle);
  return false;
}

static bool check_promisc(const char *dev) {
  char junk[PCAP_ERRBUF_SIZE];
  int err;
  pcap_t *handle;

  handle = pcap_create(dev, junk);
  if(!handle)
    goto fail;

  err = pcap_set_promisc(handle, 1);
  if(err)
    /* Should never error: pcap_set_promisc() only errors if handle is active. */
    die(0, "DEBUG: Reached unreachable condition at %s:%lu\n", __FILE__, __LINE__);

  err = pcap_activate(handle);
  if(err && err != PCAP_WARNING)
    goto fail;

  pcap_close(handle);
  return true;

fail:
  pcap_close(handle);
  return false;
}

static bool check_rfmon(const char *dev) {
  char junk[PCAP_ERRBUF_SIZE];
  pcap_t *handle;

  handle = pcap_create(dev, junk);
  if(!handle)
    goto fail;

  if(pcap_can_set_rfmon(handle) != 1)
    goto fail;

  pcap_close(handle);
  return true;

fail:
  pcap_close(handle);
  return false;
}

static void print_pcap_addrs(pcap_addr_t *addr) {
  char addrbuf[1024];

  for(; addr; addr = addr->next) {
    if(addr->netmask)
      printf("     %s/%d ",
             addr_to_string(addr->addr, addrbuf, sizeof addrbuf),
             netmask_to_string(addr->netmask));
    else
      printf("     %s ",
             addr_to_string(addr->addr, addrbuf, sizeof addrbuf));
    if(addr->broadaddr)
      printf("brd %s",
             addr_to_string(addr->broadaddr, addrbuf, sizeof addrbuf));
    if(addr->dstaddr)
      printf("dst %s",
             addr_to_string(addr->dstaddr, addrbuf, sizeof addrbuf));
    puts("");
  }
}

static void print_datalinks(const char *dev) {
  int *linktypes;
  int err, i, nlinktypes;
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_t *handle;

  handle = pcap_create(dev, errbuf);
  if(!handle)
    return;

  err = pcap_activate(handle);
  if(err)
    return;

  nlinktypes = pcap_list_datalinks(handle, &linktypes);
  if(nlinktypes > 0)
    puts("     Linktypes:");
  for(i = 0; i < nlinktypes; ++i)
    printf("       %-20s %s\n"
           , pcap_datalink_val_to_name(linktypes[i])
           , pcap_datalink_val_to_description(linktypes[i]));

  pcap_free_datalinks(linktypes);
  pcap_close(handle);
}

static void print_timestamp_types(const char *dev) {
  int i, ntstypes;
  char junk[PCAP_ERRBUF_SIZE];
  int *tstypes;
  pcap_t *handle;

  handle = pcap_create(dev, junk);
  if(!handle)
    return;

  ntstypes = pcap_list_tstamp_types(handle, &tstypes);
  if(ntstypes > 0)
    puts("     Timestamp types:");
  for(i = 0; i < ntstypes; ++i)
    printf("       %-20s %s\n", pcap_tstamp_type_val_to_name(tstypes[i]),
                                pcap_tstamp_type_val_to_description(tstypes[i]));

  pcap_free_tstamp_types(tstypes);
  pcap_close(handle);
}

void dev_info(const char *regex) {
  int err, idx;
  pcap_if_t *devs = NULL, *cur;
  char errbuf[PCAP_ERRBUF_SIZE];
  bool dev_found = false;
  bool rfmon_ok, promisc_ok, capture_ok;

  err = pcap_findalldevs(&devs, errbuf);
  if(err)
    die(0, "%s", errbuf);

  for(idx = 0, cur = devs; cur; cur = cur->next, ++idx)
    if(regex_matches_or_is_null(regex, cur->name)) {
      dev_found = true;

      errbuf[0] = '\0';
      capture_ok = check_capture(errbuf, cur->name);
      rfmon_ok = capture_ok ? check_rfmon(cur->name) : false;
      promisc_ok = capture_ok ? check_promisc(cur->name) : false;

      printf("%3d: ", idx);
      printf("%-22s %s%s%s%s%s%s\n", cur->name
             , cur->flags & PCAP_IF_LOOPBACK ? "[loopback]"   : ""
             , cur->flags & PCAP_IF_UP       ? "[up]"         : "[down]"
             , cur->flags & PCAP_IF_RUNNING  ? "[running]"    : ""
             , promisc_ok                    ? "[promisc_ok]" : ""
             , rfmon_ok                      ? "[rfmon_ok]"   : ""
             , capture_ok                    ? "[usable]"     : "[unusable]");

      if(opts.verbose) {
        if(cur->description)
          printf("     %s\n", cur->description);
        if(errbuf[0])
          printf("     %s\n", errbuf);

        print_pcap_addrs(cur->addresses);
        print_datalinks(cur->name);
        print_timestamp_types(cur->name);
      }
    }

  if(!dev_found)
    fprintf(stderr, "No matching devices found\n");

  pcap_freealldevs(devs);
}
