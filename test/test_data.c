/*
 * test_data.c
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
 * ┏━┓┏━┓┏━┓╺┳╸┏━┓╺┳┓╻ ╻┏┳┓┏━┓
 * ┣━┛┣┳┛┃ ┃ ┃ ┃ ┃ ┃┃┃ ┃┃┃┃┣━┛
 * ╹  ╹┗╸┗━┛ ╹ ┗━┛╺┻┛┗━┛╹ ╹╹  
 *
 */

#include <stdlib.h>
#include <unistd.h>

#include "../data.h"
#include "../common.h"

char *name;

void usage() {
  die("Usage: %s [-d type=x,value=y]\n",
      name);
}

data_t make_data(char *optarg) {
  int i, optidx, b;
  char *type = NULL, *value = NULL, *arg;
  char *tokens[] = { "type", "value", NULL };
  data_t data = data_new();

  while((optidx = getsubopt(&optarg, tokens, &arg)) >= 0) {
    if(!arg) usage();

    switch(optidx) {
      case 0:
        if(type) usage();
        type = arg;
        break;
      case 1:
        if(value) usage();
        value = arg;
        break;
    }
  }

  if(!type) usage();

  i = datatype_string2type(type);
  if(i == DT_INVALID) usage();

  switch(i) {
    case DT_NULL:
      if(value) usage();
      data_set_null(data);
      break;
    case DT_BOOL:
      if(!value) usage();
      if(!strcmp(value, "true")) data_set_bool(data, true);
      else if(!strcmp(value, "false")) data_set_bool(data, false);
      else usage();
      break;
    case DT_INT:
      if(!value) usage();
      data_set_int(data, atoi(value));
      break;
    case DT_DOUBLE:
      if(!value) usage();
      data_set_double(data, atof(value));
      break;
    case DT_STRING:
      if(!value) usage();
      data_set_string(data, value);
      break;
    default:
      usage();
  }

  return data;
}

void print_data(data_t data) {
  switch(data->type) {
    case DT_NULL:
      printf("%s: empty\n", datatype_type2string(data->type));
      break;
    case DT_BOOL:
      printf("%s: %s\n", datatype_type2string(data->type),
                         data->b ? "true" : "false");
      break;
    case DT_INT:
      printf("%s: %d\n", datatype_type2string(data->type), data->i);
      break;
    case DT_DOUBLE:
      printf("%s: %f\n", datatype_type2string(data->type), data->d);
      break;
    case DT_STRING:
      printf("%s: %s\n", datatype_type2string(data->type), data->s);
      break;
    default:
      break;
  }
}

int main(int argc, char **argv) {
  int opt;
  data_t d;

  name = argv[0];

  while((opt = getopt(argc, argv, "d:")) >= 0)
    switch(opt) {
      case 'd':
        d = make_data(optarg);
        print_data(d);
        data_free(d);
        break;
      default:
        usage();
    }

    return 0;
}
