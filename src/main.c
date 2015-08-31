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
#include "options.h"

enum acttypes {
  ACT_NONE,
  ACT_HELP,
  ACT_ERR,
  ACT_VERBOSE,
  ACT_DEV,
  ACT_CAPFILE,
  ACT_INFO,
  ACT_CAPTURE,
  ACT_REPLAY,
};

struct opts opts = {
  .action = 0,
  .dev = NULL,
  .capfile = NULL,
  .verbose = false,
};

#define FLAGCOUNT (sizeof(flaglist) / sizeof(*flaglist))
struct flag flaglist[] = {
  { .name = 'I',
    .description = "Print information about available devices",
    .arg = ARG_NONE,
    .mode = true,
    .mode_blacklist = "f",
    .action = ACT_INFO
  },
  { .name = 'C',
    .description = "Capture packets",
    .arg = ARG_NONE,
    .mode = true,
    .mode_blacklist = NULL,
    .action = ACT_CAPTURE
  },
  { .name = 'R',
    .description = "Replay packets",
    .arg = ARG_NONE,
    .mode = true,
    .mode_blacklist = NULL,
    .action = ACT_REPLAY
  },
  { .name = 'f',
    .description = "Specify capture file to capture to / replay from",
    .arg = ARG_STRING,
    .mode = false,
    .action = ACT_CAPFILE
  },
  { .name = 'd',
    .description = "Specify the device to capture/replay on by regex",
    .arg = ARG_REGEX,
    .mode = false,
    .action = ACT_DEV
  },
  { .name = 'h',
    .description = "Print this message",
    .arg = ARG_NONE,
    .mode = false,
    .action = ACT_HELP
  },
  { .name = 'v',
    .description = "Increase the verbosity of the information printed",
    .arg = ARG_NONE,
    .mode = false,
    .action = ACT_VERBOSE
  },
};

int main(int argc, char **argv) {
  char *arg = NULL, *optstr = make_optstr(flaglist, FLAGCOUNT);
  int a;

  if(!argv[1]) {
    print_flag_usage(stdout, flaglist, FLAGCOUNT);
    return EXIT_FAILURE;
  }

  while((a = getflag(argc, argv, flaglist, FLAGCOUNT, optstr, &arg)) >= 0)
    switch((enum acttypes)a) {
      case ACT_NONE:
      case ACT_HELP:
        print_flag_usage(stdout, flaglist, FLAGCOUNT);
        return EXIT_SUCCESS;
      case ACT_VERBOSE:
        ++opts.verbose;
        break;
      case ACT_DEV:
        opts.dev = arg;
        break;
      case ACT_CAPFILE:
        opts.capfile = arg;
        break;
      case ACT_INFO:
      case ACT_CAPTURE: /* Fallthrough */
      case ACT_REPLAY:  /* Fallthrough */
        opts.action = a;
        break;
      default:
        die(0, "DEBUG: getflag() returned an unknown value: '%c'", a);
    }

  switch((enum acttypes)opts.action) {
    case ACT_INFO:
      dev_info(opts.dev);
      break;
    case ACT_CAPTURE:
      die(0, "Capture not yet implemented");
      break;
    case ACT_REPLAY:
      die(0, "Replay not yet implemented");
      break;
    default:
      print_flag_usage(stderr, flaglist, FLAGCOUNT);
      return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
