include Defs.mk

NAME    = protodump
PARTS   = common json
OBJS    = $(PARTS:%=src/%.o)

.PHONY: all test clean

all:
	@echo ${CFLAGS}

test:
	${MAKE} -C test

%.o: %.c %.h
	@echo cc $@
	@${CC} -c ${CFLAGS} ${CPPFLAGS} $< ${LDFLAGS}

clean:
	rm -f ${OBJS} ${NAME}
	${MAKE} -C test clean
