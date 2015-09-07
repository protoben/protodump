/*
 * common.c
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

#include "common.h"

void die(int err, char *fmt, ...) {
  va_list ap;
  char buf[BUFSIZ];

  va_start(ap, fmt);
  vsnprintf(buf, sizeof buf, fmt, ap);
  va_end(ap);

  if(err) {
    errno = err;
    perror(buf);
  } else {
    fprintf(stderr, "%s\n", buf);
  }

  exit(EXIT_FAILURE);
}

void *malloc_or_die(size_t sz) {
  void *ret;

  errno = 0;
  ret = malloc(sz);
  if(!ret)
    die(errno, "malloc(%lu)", sz);

  return ret;
}

void *realloc_or_die(void *p, size_t sz) {
  void *ret;

  errno = 0;
  ret = realloc(p, sz);
  if(!ret)
    die(errno, "realloc(%#lx, %lu)", p, sz);

  return ret;
}

FILE *fopen_or_die(const char *filename, const char *mode) {
  FILE *fp;

  errno = 0;
  fp = fopen(filename, mode);
  if(!fp)
    die(errno, "fopen(\"%s\", \"%s\")", filename, mode);

  return fp;
}

void regcomp_or_die(regex_t *preg, const char *regex, int cflags) {
  char errbuf[BUFSIZ];
  int err;

  cflags |= REG_EXTENDED;
  err = regcomp(preg, regex, cflags);
  if(err) {
    regerror(err, preg, errbuf, sizeof errbuf);
    die(0, "regcomp(\"%s\", %#x): %s", regex, cflags, errbuf);
  }
}

void plog(int required_verbosity, char *fmt, ...) {
  va_list ap;

  if(options.verbose >= required_verbosity) {
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
  }

  fputc('\n', stderr);
}
