/*
 * arg.h
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

#ifndef PROTODUMP_ARG_H
#define PROTODUMP_ARG_H

#include <sys/ioctl.h>
#include <limits.h>
#include <regex.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "common.h"

enum argtype {
  ARG_NONE,
  ARG_REGEX,
  ARG_STRING,
  ARG_POSINTEGER,
  ARG_IPV4,
  ARG_IPV6,
};

struct flag {
  char name;
  const char *description;
  enum argtype arg;
  bool mode;
  const char *mode_blacklist;
  int action;
};

char *make_optstr(struct flag *flaglist, int nflags);
int getflag(int argc, char **argv, struct flag *flaglist, int nflags,
            const char *optstr, char **parg);
void print_flag_usage(FILE *fp, struct flag *flaglist, int nflags);

#endif
