/*
 * common.h
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

#ifndef PROTODUMP_COMMON_H
#define PROTODUMP_COMMON_H

#include <errno.h>
#include <regex.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "options.h"

#define NAME   "protodump"
#define VER    "0.1"
#define AUTHOR "Ben Hamlin"

/* External functions */
void die(int err, char *fmt, ...);
void *malloc_or_die(size_t sz);
void *realloc_or_die(void *p, size_t sz);
FILE *fopen_or_die(const char *filename, const char *mode);
void regcomp_or_die(regex_t *preg, const char *regex, int cflags);
void plog(int required_verbosity, char *fmt, ...);

#endif
