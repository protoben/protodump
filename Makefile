CC      = gcc
CFLAGS  = -g -Wall --std=c99
LDFLAGS =

PARTS   = common data
OBJS    = $(PARTS:%=%.o)

NAME    =

all:

libcalg/%:
	@${MAKE} -C libcalg $*

test/%: test/%.c
	@${MAKE} -C test $*

%.o: %.c %.h
	@echo cc $@
	@${CC} -c ${CFLAGS} ${LDFLAGS} $<

clean:
	rm -f ${OBJS} ${NAME}
	${MAKE} -C libcalg clean
	${MAKE} -C test clean
