CC	= gcc
CFLAGS	= -Wall --std=c99
CPPFLAGS=
LDFLAGS	=

DEBUG	= 1
ifdef DEBUG
CFLAGS		+= -g -D_DEBUG
endif
