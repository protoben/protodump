/*
 * arg.c
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

#include "arg.h"

const char *argtype_names[] = {
  "NONE",
  "REGEX",
  "STRING",
  "INTEGER",
  "IPv4 ADDRESS",
  "IPv6 ADDRESS",
};

/* Regexes for argument matching */
#define DEC_NUMBER "(0|[1-9][0-9]*)"
#define OCT_NUMBER "(0[0-7]*)"
#define HEX_NUMBER "(0[xX][0-9a-fA-F]+)"
#define ANY_NUMBER "(" DEC_NUMBER "|" OCT_NUMBER "|" HEX_NUMBER ")"
#define IPV4_OCTET "([0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])"
#define IPV4_CIDR  "(/[0-2]?[0-9]|3[0-2])"
#define IPV4_ADDR  "(" "(" IPV4_OCTET ".){3}" IPV4_OCTET "(" IPV4_CIDR ")?" ")"
#define IPV6_OCTET "([0-9a-fA-F]{,4})" /* FIXME: This is wrong. */
#define IPV6_CIDR  "(/(0?[0-9]?[0-9]|1[0-1][0-9]|12[0-8]))"
#define IPV6_ADDR  "(" "(" IPV6_OCTET ":){,7}" IPV6_OCTET "(" IPV6_CIDR ")?" ")" /* FIXME: Totally wrong. */

#define ADD_TO_BUF(_buf, _c, _i, _len) do{_buf[(_i < _len) ? (_i++) : (_len - 1)] = _c;}while(0)
char *make_optstr(struct flag *flaglist, int nflags) {
  static char optstr[BUFSIZ];
  int i, c = 0;

  for(i = 0; i < nflags; ++i) {
    ADD_TO_BUF(optstr, flaglist[i].name, c, BUFSIZ);
    if(flaglist[i].arg != ARG_NONE)
      ADD_TO_BUF(optstr, ':', c, BUFSIZ);
  }
  ADD_TO_BUF(optstr, '\0', c, BUFSIZ);

  return optstr;
}

static bool reg_matches(const char *regex, const char *s) {
  regex_t reg;
  int res;

  regcomp_or_die(&reg, regex, REG_EXTENDED | REG_NOSUB);
  res = regexec(&reg, s, 0, NULL, 0);
  regfree(&reg);

  return (res != REG_NOMATCH);
}

static bool arg_is_valid(struct flag *pflag, char *arg) {
  switch(pflag->arg) {
    case ARG_NONE:
      return arg == NULL;
    case ARG_REGEX:
    case ARG_STRING:
      return true;
    case ARG_INTEGER:
      return reg_matches(ANY_NUMBER, arg);
    case ARG_IPV4:
      return reg_matches(IPV4_ADDR, arg);
    case ARG_IPV6:
      return reg_matches(IPV6_ADDR, arg);
  }

  return false;
}

int getflag(int argc, char **argv, struct flag *flaglist, int nflags,
            const char *optstr, char **parg) {
  struct  flag *cur;
  int i, c;
  static bool mode_set = false;

  optarg = NULL;
  c = getopt(argc, argv, optstr);
  if(c < 0) {
    if(!mode_set)
      die(0, "At least one mode argument must be specified");
    else
      return -1;
  }

  if(c == '?')
    exit(EXIT_FAILURE);

  for(i = 0; i < nflags; ++i)
    if(flaglist[i].name == c)
      cur = &flaglist[i];

  if(cur->mode) {
    if(mode_set)
      die(0, "Only one mode argument may be specified");
    else
      mode_set = true;
  }

  if(!arg_is_valid(cur, optarg))
    die(0, "Flag '-%c' requires an argument of type %s, not \"%s\"\n",
           cur->name, argtype_names[cur->arg], optarg);
  *parg = optarg;

  return cur->action;
}

void print_flag_usage(FILE *fp, struct flag *flaglist, int nflags) {
  int i, j, k, l;
  struct flag *modes[nflags + 1], *opts[nflags + 1], *opts_wargs[nflags + 1];
  char usage[] = "Usage:";

  for(i = j = k = l = 0; i < nflags; ++i)
    if(flaglist[i].mode)
      modes[j++] = &flaglist[i];
    else if(flaglist[i].arg == ARG_NONE)
      opts[k++] = &flaglist[i];
    else
      opts_wargs[l++] = &flaglist[i];
  modes[j] = opts[k] = opts_wargs[l] = NULL;

  for(i = 0; modes[i]; ++i) {
    fprintf(fp, "%-7s%s -%c ", usage, NAME, modes[i]->name);
    usage[0] = '\0';

    for(j = 0; opts_wargs[j]; ++j)
      if(!modes[i]->mode_blacklist
         || !strchr(modes[i]->mode_blacklist, opts_wargs[j]->name))
        fprintf(fp, "[-%c <%s>] ", opts_wargs[j]->name
                                   , argtype_names[opts_wargs[j]->arg]);

    if(opts[0]) {
      fputs("[-", fp);
      for(j = 0; opts[j]; ++j)
        if(!modes[i]->mode_blacklist
           || !strchr(modes[i]->mode_blacklist, opts[j]->name))
          putc(opts[j]->name, fp);
      putc(']', fp);
    }

    putc('\n', fp);
  }

  putc('\n', fp);
  for(i = 0; modes[i]; ++i)
    fprintf(fp, "\t-%c\t%s\n", modes[i]->name, modes[i]->description);
  for(i = 0; opts_wargs[i]; ++i)
    fprintf(fp, "\t-%c\t%s\n", opts_wargs[i]->name, opts_wargs[i]->description);
  for(i = 0; opts[i]; ++i)
    fprintf(fp, "\t-%c\t%s\n", opts[i]->name, opts[i]->description);
}
