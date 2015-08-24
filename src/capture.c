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

void dev_list(const char *regex) {
  int err, idx;
  pcap_if_t *devs = NULL, *cur;
  char errbuf[PCAP_ERRBUF_SIZE] = {0};
  bool dev_found = false;

  err = pcap_findalldevs(&devs, errbuf);
  if(err)
    die(0, "%s", errbuf);

  for(idx = 0, cur = devs; cur; cur = cur->next, ++idx)
    if(regex_matches_or_is_null(regex, cur->name)) {
      printf("%3d: ", idx);
      printf("%-20s%s%s%s\n", cur->name
                            , cur->flags & PCAP_IF_LOOPBACK ? "[loopback]" : ""
                            , cur->flags & PCAP_IF_UP ? "[up]" : "[down]"
                            , cur->flags & PCAP_IF_RUNNING ? "[running]" : "");
      dev_found = true;
    }

  if(!dev_found)
    fprintf(stderr, "No matching devices found\n");

  pcap_freealldevs(devs);
}

void dev_info(const char *regex) {
  int err, idx;
  pcap_if_t *devs = NULL, *cur;
  pcap_addr_t *addr;
  char errbuf[PCAP_ERRBUF_SIZE], addrbuf[1024];
  bool dev_found = false;

  err = pcap_findalldevs(&devs, errbuf);
  if(err)
    die(0, "%s", errbuf);

  for(idx = 0, cur = devs; cur; cur = cur->next, ++idx)
    if(regex_matches_or_is_null(regex, cur->name)) {
      dev_found = true;
      printf("%3d: ", idx);
      printf("%-20s%s%s%s\n", cur->name
                            , cur->flags & PCAP_IF_LOOPBACK ? "[loopback]" : ""
                            , cur->flags & PCAP_IF_UP ? "[up]" : "[down]"
                            , cur->flags & PCAP_IF_RUNNING ? "[running]" : "");

      if(cur->description)
        printf("     %s\n", cur->description);

      for(addr = cur->addresses; addr; addr = addr->next) {
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

  if(!dev_found)
    fprintf(stderr, "No matching devices found\n");

  pcap_freealldevs(devs);
}

void dev_datalinks(const char *regex) {
  int err, idx, nlinktypes, i, *linktypes;
  pcap_if_t *devs = NULL, *cur;
  char errbuf[PCAP_ERRBUF_SIZE];
  bool dev_found = false;
  pcap_t *pcap;

  err = pcap_findalldevs(&devs, errbuf);
  if(err)
    die(0, "%s", errbuf);

  for(idx = 0, cur = devs; cur; cur = cur->next, ++idx)
    if(regex_matches_or_is_null(regex, cur->name)) {
      dev_found = true;
      printf("%3d: ", idx);
      printf("%-20s\n", cur->name);

      pcap = pcap_create(cur->name, errbuf);
      if(!pcap)
        continue;

      err = pcap_activate(pcap);
      if(err)
        continue;

      nlinktypes = pcap_list_datalinks(pcap, &linktypes);
      for(i = 0; i < nlinktypes; ++i)
        printf("     %-30s %s\n"
               , pcap_datalink_val_to_name(linktypes[i])
               , pcap_datalink_val_to_description(linktypes[i]));

      pcap_free_datalinks(linktypes);
      pcap_close(pcap);
    }

  if(!dev_found)
    fprintf(stderr, "No matching devices found\n");

  pcap_freealldevs(devs);
}
