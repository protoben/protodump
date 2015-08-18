/*
 * main.c
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

#include <stdio.h>
#include <stdlib.h>

#include "arg.h"
#include "capture.h"
#include "common.h"
#include "netutil.h"

enum acttypes {
  ACT_NONE,
  ACT_HELP,
  ACT_ERR,
  ACT_DEV_LIST,
  ACT_DEV_INFO
};

#define FLAGCOUNT (sizeof(flaglist) / sizeof(*flaglist))
struct flag flaglist[] = {
  { .name = 'L',
    .description = "List devices available for capture",
    .arg = ARG_REGEX,
    .arg_optional = true,
    .action = ACT_DEV_LIST
  },
  { .name = 'I',
    .description = "Print verbose information about available devices",
    .arg = ARG_REGEX,
    .arg_optional = true,
    .action = ACT_DEV_INFO
  },
  { .name = 'h',
    .description = "Print this message",
    .arg = ARG_NONE,
    .arg_optional = false,
    .action = ACT_HELP
  },
};

struct {
  enum acttypes action;
  char *dev;
} opts = {
  .action = ACT_NONE,
  .dev = NULL,
};

int main(int argc, char **argv) {
  char *arg = NULL, *optstr = make_optstr(flaglist, FLAGCOUNT);
  int a;

  while((a = getflag(argc, argv, flaglist, FLAGCOUNT, optstr, &arg)) >= 0)
    switch(a) {
      case ACT_NONE:
      case ACT_HELP:
        print_flag_usage(stdout, flaglist, FLAGCOUNT);
        return EXIT_SUCCESS;
      case ACT_DEV_LIST:
      case ACT_DEV_INFO:
        opts.action = a;
        opts.dev = arg;
        break;
      default:
        die(0, "DEBUG: getflag() returned an unknown value: '%c'", a);
    }

  switch(opts.action) {
    case ACT_DEV_LIST:
      dev_list(opts.dev);
      break;
    case ACT_DEV_INFO:
      dev_info(opts.dev);
      break;
    default:
      print_flag_usage(stderr, flaglist, FLAGCOUNT);
      return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
