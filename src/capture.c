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

  char regline[strlen(regex) + 3];
  sprintf(regline, "^%s$", regex);

  err = regcomp(&reg, regline, REG_EXTENDED | REG_NOSUB);
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
    printf("     Linktypes:%s", options.verbose > 1 ? "\n" : " ");
  if(options.verbose > 1)
    for(i = 0; i < nlinktypes; ++i)
      printf("       %-20s %s\n"
             , pcap_datalink_val_to_name(linktypes[i])
             , pcap_datalink_val_to_description(linktypes[i]));
  else
    for(i = 0; i < nlinktypes; ++i)
      printf("%s%s", pcap_datalink_val_to_name(linktypes[i])
                   , i + 1 > nlinktypes ? ", " : "\n");

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
    printf("     Timestamp types:%s", options.verbose > 1 ? "\n" : " ");
  if(options.verbose > 1)
    for(i = 0; i < ntstypes; ++i)
      printf("       %-20s %s\n", pcap_tstamp_type_val_to_name(tstypes[i]),
                                  pcap_tstamp_type_val_to_description(tstypes[i]));
  else
    for(i = 0; i < ntstypes; ++i)
      printf("%s%s", pcap_tstamp_type_val_to_name(tstypes[i])
                   , i + 1 > ntstypes ? ", " : "\n");

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

      if(options.verbose) {
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

static char *match_dev_regex_or_die(const char *regstr) {
  static char dev[1024];
  int matches = 0, devlen = 0;
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_if_t *interfaces, *cur;
  int err;

  err = pcap_findalldevs(&interfaces, errbuf);
  if(err)
    die(0, "pcap_findalldevs(): %s", errbuf);

  for(cur = interfaces; cur; cur = cur->next)
    if(regex_matches_or_is_null(regstr, cur->name))
      devlen = snprintf(&dev[devlen], sizeof(dev) - devlen
                        , "%s%s"
                        , matches++ ? ", " : ""
                        , cur->name);

  if(!matches)
    die(0, "No device matching regex: %s", regstr);
  else if(matches > 1)
    die(0, "Ambiguous device regex: %s\nDid you mean one of these: %s", regstr, dev);

  pcap_freealldevs(interfaces);
  return dev;
}

static void prep_pcap_handle(pcap_t *handle) {
  int err;

  err = pcap_set_rfmon(handle, options.rfmon);
  if(err)
    die(0, "DEBUG: pcap handle should not be activated at %s:%d", __FILE__, __LINE__);

  err = pcap_set_promisc(handle, options.promisc);
  if(err)
    die(0, "DEBUG: pcap handle should not be activated at %s:%d", __FILE__, __LINE__);

  err = pcap_set_snaplen(handle, options.snaplen);
  if(err)
    die(0, "DEBUG: pcap handle should not be activated at %s:%d", __FILE__, __LINE__);

  err = pcap_set_timeout(handle, options.read_timeout);
  if(err)
    die(0, "DEBUG: pcap handle should not be activated at %s:%d", __FILE__, __LINE__);

  if(options.buffer_size > 0) {
    err = pcap_set_buffer_size(handle, options.buffer_size);
    if(err)
      die(0, "DEBUG: pcap handle should not be activated at %s:%d", __FILE__, __LINE__);
  }

  if(options.tstamp_type != PCAP_ERROR) {
    err = pcap_set_tstamp_type(handle, options.tstamp_type);
    if(err == PCAP_ERROR_ACTIVATED)
      die(0, "DEBUG: pcap handle should not be activated at %s:%d", __FILE__, __LINE__);
    else if(err == PCAP_ERROR_CANTSET_TSTAMP_TYPE)
      die(0, "pcap_set_tstamp_type(): Device does not support setting the timestamp");
    else if(err == PCAP_WARNING_TSTAMP_TYPE_NOTSUP)
      plog(0, "pcap_set_tstamp_type(): Device does not support specified tstamp type");
  }

  if(options.tstamp_nano) {
    err = pcap_set_tstamp_precision(handle, PCAP_TSTAMP_PRECISION_NANO);
    if(err == PCAP_ERROR_ACTIVATED)
      die(0, "DEBUG: pcap handle should not be activated at %s:%d", __FILE__, __LINE__);
    else if(err == PCAP_ERROR_TSTAMP_PRECISION_NOTSUP)
      die(0, "pcap_set_tstamp_precision(): Device does not support nanosecond precision");
  }

  if(options.linktype != PCAP_ERROR) {
    err = pcap_set_datalink(handle, options.linktype);
    if(err)
      die(0, "pcap_set_datalink(): %s", pcap_geterr(handle));
  }
}

int capture_live(const char *filter) {
  int err;
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_t *handle;
  char *dev = options.dev ? match_dev_regex_or_die(options.dev) : "all";

  plog(1, "Capturing on device: %s", dev);

  handle = pcap_create(dev, errbuf);
  prep_pcap_handle(handle);

  err = pcap_activate(handle);
  if(err == PCAP_WARNING)
    plog(0, "pcap_activate(): %s", pcap_geterr(handle));
  else if(err == PCAP_WARNING_PROMISC_NOTSUP || err == PCAP_WARNING_TSTAMP_TYPE_NOTSUP)
    plog(0, "pcap_activate(): %s", pcap_geterr(handle));
  else if(err)
    die(0, "pcap_activate(): %s", pcap_geterr(handle));

  pcap_close(handle);
  return 0;
}

int capture_from_file(const char *filter, const char *file) {
  if(!file)
    die(0, "DEBUG: \"file\" should not be NULL at %s:%lu", __FILE__, __LINE__);

  die(0, "Capture from file not yet implemented");

  return 0;
}
