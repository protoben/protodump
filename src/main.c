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

/* Option defaults */
struct options options = {
  .action = 0,
  .dev = NULL,
  .capwrite = NULL,
  .capread = NULL,
  .jsonfile = NULL,
  .verbose = false,
  .rfmon = false,
  .promisc = false,
  .snaplen = 65536,
  .read_timeout = 100,
  .buffer_size = 0,
  .tstamp_type = PCAP_ERROR,
  .tstamp_nano = false,
  .linktype = PCAP_ERROR,
};

enum acttypes {
  ACT_NONE,
  ACT_HELP,
  ACT_ERR,
  ACT_VERBOSE,
  ACT_DEV,
  ACT_CAPWRITE,
  ACT_CAPREAD,
  ACT_JSON,
  ACT_PROMISC,
  ACT_RFMON,
  ACT_SNAPLEN,
  ACT_TIMEOUT,
  ACT_BUFSIZE,
  ACT_TIMESTAMP,
  ACT_NANORES,
  ACT_LINKTYPE,
  ACT_INFO,
  ACT_CAPTURE,
  ACT_REPLAY,
};

/* Flags come in two types: modes and options. Only one mode flag may be
 * used, since they dictate what the program does overall. The convention
 * is that mode names are capital, whereas option names are lower case.
 *
 * name:           The flag letter
 * description:    Short description that appears in the usage
 * arg:            Argument type from arg.h for syntax checking
 * mode:           Should be true if this is a mode, false otherwise
 * mode_blacklist: Options incompatible with this mode
 * action:         Value for the switch statements in main
 */
#define FLAGCOUNT (sizeof(flaglist) / sizeof(*flaglist))
struct flag flaglist[] = {
  { .name = 'I',
    .description = "Print information about available devices",
    .arg = ARG_NONE,
    .mode = true,
    .mode_blacklist = "bjlnprstuw",
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
  { .name = 'b',
    .description = "Try to set the size of pcap's packet buffer",
    .arg = ARG_POSINTEGER,
    .mode = false,
    .action = ACT_BUFSIZE
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
  { .name = 'j',
    .description = "Specify JSON file to use instead of stdin/stdout",
    .arg = ARG_STRING,
    .mode = false,
    .action = ACT_JSON
  },
  { .name = 'l',
    .description = "Tell pcap which link type to use, ala pcap-linktype(7)",
    .arg = ARG_STRING,
    .mode = false,
    .action = ACT_LINKTYPE
  },
  { .name = 'm',
    .description = "Try to put the interface into 802.11 monitor mode",
    .arg = ARG_NONE,
    .mode = false,
    .action = ACT_RFMON
  },
  { .name = 'n',
    .description = "Set timestamps to use ns rather than ms resolution",
    .arg = ARG_NONE,
    .mode = false,
    .action = ACT_NANORES
  },
  { .name = 'p',
    .description = "Try to put the interface into promiscuous mode",
    .arg = ARG_NONE,
    .mode = false,
    .action = ACT_PROMISC
  },
  { .name = 'r',
    .description = "Pcap capture file to capture / replay from",
    .arg = ARG_STRING,
    .mode = false,
    .action = ACT_CAPREAD
  },
  { .name = 's',
    .description = "Number of bytes to truncate packets to (def. 65536)",
    .arg = ARG_POSINTEGER,
    .mode = false,
    .action = ACT_SNAPLEN
  },
  { .name = 't',
    .description = "Time to wait for the buffer to fill up in ms (def. 100)",
    .arg = ARG_POSINTEGER,
    .mode = false,
    .action = ACT_TIMEOUT
  },
  { .name = 'u',
    .description = "Set the timestamp type, ala pcap-tstamp(7)",
    .arg = ARG_STRING,
    .mode = false,
    .action = ACT_TIMESTAMP
  },
  { .name = 'v',
    .description = "Increase the verbosity of the information printed",
    .arg = ARG_NONE,
    .mode = false,
    .action = ACT_VERBOSE
  },
  { .name = 'w',
    .description = "Pcap capture file to capture / replay to",
    .arg = ARG_STRING,
    .mode = false,
    .action = ACT_CAPWRITE
  },
};

int main(int argc, char **argv) {
  char *arg = NULL, *optstr = make_optstr(flaglist, FLAGCOUNT);
  int a;
  char *filter = NULL;

  if(!argv[1]) {
    print_flag_usage(stdout, flaglist, FLAGCOUNT);
    return EXIT_FAILURE;
  }

  /* Handle option flags */
  while((a = getflag(argc, argv, flaglist, FLAGCOUNT, optstr, &arg)) >= 0)
    switch((enum acttypes)a) {
      case ACT_NONE:
      case ACT_HELP: /* Fallthrough */
        print_flag_usage(stdout, flaglist, FLAGCOUNT);
        return EXIT_SUCCESS;
      case ACT_ERR:
        print_flag_usage(stdout, flaglist, FLAGCOUNT);
        return EXIT_FAILURE;
      case ACT_VERBOSE:
        ++options.verbose;
        break;
      case ACT_DEV:
        options.dev = arg;
        break;
      case ACT_CAPWRITE:
        options.capwrite = arg;
        break;
      case ACT_CAPREAD:
        options.capread = arg;
        break;
      case ACT_JSON:
        options.jsonfile = arg;
        break;
      case ACT_PROMISC:
        ++options.promisc;
        break;
      case ACT_RFMON:
        ++options.rfmon;
        break;
      case ACT_SNAPLEN:
        options.snaplen = (int)strtoul(arg, NULL, 0);
        break;
      case ACT_TIMEOUT:
        options.read_timeout = (int)strtoul(arg, NULL, 0);
        break;
      case ACT_BUFSIZE:
        options.buffer_size = (int)strtoul(arg, NULL, 0);
        break;
      case ACT_TIMESTAMP:
        options.tstamp_type = pcap_tstamp_type_name_to_val(arg);
        if(options.tstamp_type == PCAP_ERROR)
          die(0, "Not a valid timestamp type: %s\nSee pcap-tstamp(7)", arg);
        break;
      case ACT_NANORES:
        ++options.tstamp_nano;
        break;
      case ACT_LINKTYPE:
        options.linktype = pcap_datalink_name_to_val(arg);
        if(options.linktype == PCAP_ERROR)
          die(0, "Not a valid pcap linktype: %s\nSee pcap-linktype(7)", arg);
        break;

      /* Pass modes on to the next switch */
      case ACT_INFO:
      case ACT_CAPTURE: /* Fallthrough */
      case ACT_REPLAY:  /* Fallthrough */
        options.action = a;
        break;
    }

  /* Handle mode flags */
  switch((enum acttypes)options.action) {
    case ACT_INFO:
      dev_info(options.dev);
      break;
    case ACT_CAPTURE:
      if(options.capread)
        capture_from_file(filter, options.capread);
      else
        capture_live(filter);
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
