CC	= gcc
CFLAGS	= -Wall --std=c99 -g
CPPFLAGS= -D_DEFAULT_SOURCE
LDFLAGS	= -lpcap

NAME    = protodump

OBJS    = $(PARTS:%=src/%.o)
PARTS   = \
          arg \
          capture \
          ccan/json/json \
          ccan/tap/tap \
          common \
          netutil \

DEBUG	= 1
ifdef DEBUG
CFLAGS		+= -g -D_DEBUG
endif
