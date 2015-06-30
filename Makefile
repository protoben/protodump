CC      = gcc
CFLAGS  = -g -Wall --std=c99
LDFLAGS =

PARTS   = common
OBJS    = $(PARTS:%=%.o)

NAME    = protodump

all:

test/%: test/%.c
	@${MAKE} -C test $*

%.o: %.c %.h
	@echo cc $@
	@${CC} -c ${CFLAGS} ${LDFLAGS} $<

clean:
	rm -f ${OBJS} ${NAME}
	${MAKE} -C test clean
