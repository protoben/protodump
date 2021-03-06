/*
 * capture.h
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

#ifndef PROTODUMP_CAPTURE_H
#define PROTODUMP_CAPTURE_H

#include <pcap/pcap.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "options.h"
#include "netutil.h"

/* Print information about devices available for capture. If opts.verbose is
 * false, just print device indices, device names, and a list of attributes.
 * Otherwise, print verbose information (addresses, linktypes, timestamp types).
 * 
 * regex: If this is non-null, only list devices that match the given regex. 
 */
void dev_info(const char *regex);

/* Capture packets based on the given filter. If filter is NULL, capture all
 * packets. Return the number of packets captured.
 *
 * filter: If non-NULL, specifies an in-kernel filter ala pcap-filter(7)
 */
int capture_live(const char *filter);

/* Capture packets from file based on the given filter. If filter is NULL,
 * capture all packets. Return the number of packets captured.
 *
 * file:   File to capture from, which should not be NULL
 * filter: If non-NULL, specifies an in-kernel filter ala pcap-filter(7)
 */
int capture_from_file(const char *filter, const char *file);

#endif
