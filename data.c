/*
 * data.c
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

#include "data.h"

char *dt_strings[] = { "null", "bool", "int", "double", "string", "map", "array", NULL };

struct data *data_new() {
  struct data *res = malloc_or_die(sizeof *res);

  res->type = DT_INVALID;

  return res;
}

void data_free(struct data *dp) {
  if(!dp) return;

  switch(dp->type) {
    case DT_STRING:
      free(dp->s);
      break;
    case DT_MAP:
      datamap_free(dp->m);
      break;
    case DT_ARRAY:
      datalist_free(dp->a);
      break;
    default:
      break;
  }

  free(dp);
}

struct data *data_copy(struct data *src) {
  struct data *res;

  if(!src) return NULL;

  switch(src->type) {
    case DT_NULL:
      res = data_new(NULL, DT_NULL);
      break;
    case DT_BOOL:
      res = data_new(&src->b, DT_BOOL);
      break;
    case DT_INT:
      res = data_new(&src->i, DT_INT);
      break;
    case DT_DOUBLE:
      res = data_new(&src->d, DT_DOUBLE);
      break;
    case DT_STRING:
      res = data_new(src->s, DT_STRING);
      break;
    case DT_MAP:
      res = data_new(&src->m, DT_MAP);
      break;
    case DT_ARRAY:
      res = data_new(&src->a, DT_ARRAY);
      break;
    default:
      break;
  }

  return res;
}

void data_set(struct data *dp, void *valuep, enum datatype type) {
  dp->type = type;

  switch(type) {
    case DT_BOOL:
      dp->b = *(bool*)valuep;
      break;
    case DT_INT:
      dp->i = *(int*)valuep;
      break;
    case DT_DOUBLE:
      dp->d = *(double*)valuep;
      break;
    case DT_STRING:
      dp->s = newstr((char*)valuep);
      break;
    case DT_MAP:
      dp->m = datamap_copy(*(datamap_t*)valuep);
      break;
    case DT_ARRAY:
      dp->a = datalist_copy(*(datalist_t*)valuep);
    default:
      break;
  }
}

Trie *datamap_new() {
  Trie *res = trie_new();

  if(!res) die("datamap_new(): Out of memory?!");

  return res;
}

void datamap_free(Trie *map) {
  trie_free(map);
}

Trie *datamap_copy(Trie *src) { /* FIXME: Shallow copy for now. */
  return src;
}

void datamap_insert(Trie *map, char *key, data_t *value) {
  int err;
  
  err = trie_insert(map, key, value);
  if(!err) die("datamap_insert(%s): Out of memory?!", key);
}

struct data *datamap_lookup(Trie *map, char *key) {
  return (struct data*)trie_lookup(map, key);
}

struct datalist *datalist_new() {
  struct datalist *res = malloc_or_die(sizeof *res);

  res->list = NULL;
  res->size = 0;
  res->alloc = 0;

  return res;
}

void datalist_free(struct datalist *dlp) {
  int i = 0;
  struct data **list = dlp->list;

  if(!dlp) return;

  if(list)
    for(; list[i]; ++i)
      data_free(list[i]);

  assert(i == dlp->size);

  free(list);
  free(dlp);
}

struct datalist *datalist_copy(struct datalist *src) {
  int i = 0;
  struct datalist *res = malloc_or_die(sizeof *res);
  struct data **list = src->list;

  if(!src) return NULL;

  if(!src->list)
    res->list = NULL;
  else
  {
    res->list[i] = malloc_or_die(src->alloc);

    for(; list[i]; ++i)
      res->list[i] = data_copy(list[i]);
  }

  assert(i == src->size);

  res->size = src->size;
  res->alloc = src->alloc;

  return res;
}

static void datalist_grow_internal(struct datalist *dlp) {
  assert(dlp != NULL);

  dlp->alloc += DATALIST_ALLOC;
  dlp->list = realloc_or_die(dlp->list, dlp->alloc);
}

void datalist_append(struct datalist *dlp, struct data *dp) {
  size_t space_needed = (dlp->size + 2) + sizeof(struct data*);

  if(dlp->alloc < space_needed)
    datalist_grow_internal(dlp);

  dlp->list[dlp->size++] = data_copy(dp);
  dlp->list[dlp->size] = NULL;
}

const char *datatype_type2string(datatype_t type) {
  return dt_strings[type];
}

datatype_t datatype_string2type(const char *string) {
  int i;

  for(i = 0; dt_strings[i]; ++i)
    if(!strcmp(string, dt_strings[i]))
      break;

  return i;
}
