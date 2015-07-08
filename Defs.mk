CC	= gcc
CFLAGS	= -Wall --std=c99
CPPFLAGS=
LDFLAGS	=

NAME    = protodump
PARTS   = common ccan/json/json ccan/tap/tap
OBJS    = $(PARTS:%=src/%.o)

DEBUG	= 1
ifdef DEBUG
CFLAGS		+= -g -D_DEBUG
endif
