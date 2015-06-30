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

void plog(char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  fputs(NAME ": ", stderr);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
}

void vlog(char *oldfmt, va_list oldap, char *prepend, ...)
{
  va_list ap;

  fputs(NAME ": ", stderr);
  if(prepend)
  {
    va_start(ap, prepend);
    vfprintf(stderr, prepend, ap);
    va_end(ap);
  }
  vfprintf(stderr, oldfmt, oldap);
}

void die(char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vlog(fmt, ap, "FATAL: ");
  va_end(ap);

  exit(EXIT_FAILURE);
}

void *malloc_or_die(size_t sz)
{
  void *ret;

  errno = 0;
  ret = malloc(sz);
  if(!ret) die("malloc(%lu): %s\n", sz, strerror(errno));

  return ret;
}

void *realloc_or_die(void *p, size_t sz)
{
  void *ret;

  errno = 0;
  ret = realloc(p, sz);
  if(!ret) die("realloc(%#lx, %lu): %s\n", p, sz, strerror(errno));

  return ret;
}

FILE *open_or_die(const char *filename, const char *mode)
{
  FILE *fp;

  errno = 0;
  fp = fopen(filename, mode);
  if(!fp)
    die("fopen(\"%s\", \"%s\"): %s\n", filename, mode, strerror(errno));

  return fp;
}

char *newnstr(const char *src, size_t len)
{
  char *dst;

  assert(len > 0);

  dst = malloc_or_die(len + 1);
  memcpy(dst, src, len);
  dst[len] = '\0';

  return dst;
}

char *newstr(const char *src)
{
  return newnstr(src, strlen(src));
}

char *newdstr(const char *src, char delim)
{
  char *end = strchr(src, delim);
  return (end) ? newnstr(src, end - src) : newstr(src);
}
