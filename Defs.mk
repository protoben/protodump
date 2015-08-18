CC	= gcc
CFLAGS	= -Wall --std=c99 -g
CPPFLAGS= -D_DEFAULT_SOURCE
LDFLAGS	= -lpcap

NAME    = protodump
PARTS   = common capture netutil arg ccan/json/json ccan/tap/tap
OBJS    = $(PARTS:%=src/%.o)

DEBUG	= 1
ifdef DEBUG
CFLAGS		+= -g -D_DEBUG
endif
