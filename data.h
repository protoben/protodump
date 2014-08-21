/*
 * data.h
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

#ifndef _PROTODUMP_DATA_H
#define _PROTODUMP_DATA_H

#include <assert.h>
#include <stdbool.h>
#include <yajl/yajl_gen.h>
#include <yajl/yajl_parse.h>

#include "libcalg/trie.h"
#include "common.h"

typedef struct data *data_t;
typedef enum datatype datatype_t;
typedef Trie *datamap_t;
typedef struct datalist *datalist_t;

enum datatype { DT_NULL, DT_BOOL, DT_INT, DT_DOUBLE, DT_STRING, DT_MAP, DT_ARRAY, DT_INVALID };
struct data {
  enum datatype type;
  char *key;
  union {
    bool b;
    int i;
    double d;
    char *s;
    datamap_t m;
    datalist_t a;
  };
};

#define DATALIST_ALLOC (64 * sizeof(struct data*))
struct datalist {
  struct data **list;
  int size;
  size_t alloc;
};

/* External functions */

/*
 * Create or set data inside a data_t.
 */
data_t data_new();
void data_free(data_t data);
data_t data_copy(data_t src);
void data_set(data_t data, void *valuep, datatype_t type);

/*
 * Manipulate data in a datamap_t.
 */
datamap_t datamap_new(void);
void datamap_free(datamap_t map);
datamap_t datamap_copy(datamap_t src); /* FIXME: Shallow copy for now. */
void datamap_insert(datamap_t map, char *key, data_t *value);
data_t datamap_lookup(datamap_t map, char *key);

/*
 * Manipulate data in a datalist.
 */
datalist_t datalist_new(void);
void datalist_free(datalist_t list);
datalist_t datalist_copy(datalist_t src);
void datalist_append(datalist_t list, data_t data);
static inline data_t datalist_get_index(datalist_t list, int idx)
  { assert(idx < list->size); return list->list[idx]; }
static inline int datalist_size(datalist_t list) { return list->size; }

/*
 * Determine the type of a data_t.
 */
static inline bool data_isnull(data_t dp) { return dp->type == DT_NULL; }
static inline bool data_isbool(data_t dp) { return dp->type == DT_BOOL; }
static inline bool data_isint(data_t dp) { return dp->type == DT_INT; }
static inline bool data_isdouble(data_t dp) { return dp->type == DT_DOUBLE; }
static inline bool data_isstring(data_t dp) { return dp->type == DT_STRING; }
static inline bool data_ismap(data_t dp) { return dp->type == DT_MAP; }
static inline bool data_isarray(data_t dp) { return dp->type == DT_ARRAY; }
static inline datatype_t data_gettype(data_t dp) { return dp->type; }

/*
 * Retrieve data of the specified type from a data_t.
 */
static inline bool data_getbool(data_t dp) { assert(dp->type == DT_BOOL); return dp->b; }
static inline int data_getint(data_t dp) { assert(dp->type == DT_INT); return dp->i; }
static inline double data_getdouble(data_t dp) { assert(dp->type == DT_DOUBLE); return dp->d; }
static inline char *data_getstring(data_t dp) { assert(dp->type == DT_STRING); return dp->s; }
static inline datamap_t data_getmap(data_t dp) { assert(dp->type == DT_MAP); return dp->m; }
static inline datalist_t data_getarray(data_t dp) { assert(dp->type == DT_ARRAY); return dp->a; }

/*
 * Insert data of the specified type into a data_t.
 */
static inline void data_set_null(data_t dp) { dp->type = DT_NULL; }
static inline void data_set_bool(data_t dp, bool b) { dp->type = DT_BOOL; dp->b = b; }
static inline void data_set_int(data_t dp, int i) { dp->type = DT_INT; dp->i = i; }
static inline void data_set_double(data_t dp, double d) { dp->type = DT_DOUBLE; dp->d = d; }
static inline void data_set_string(data_t dp, char *s) { dp->type = DT_STRING; dp->s = newstr(s); }
static inline void data_set_map(data_t dp, datamap_t m) { dp->type = DT_MAP; dp->m = datamap_copy(m); }
static inline void data_set_array(data_t dp, datalist_t a) { dp->type = DT_ARRAY; dp->a = datalist_copy(a); }

/*
 * Convert a string to a type and back.
 */
const char *datatype_type2string(datatype_t type);
datatype_t datatype_string2type(const char *string);

#endif
