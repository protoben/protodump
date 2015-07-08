include Defs.mk

.PHONY: all test clean

all: ${OBJS}

test: ${OBJS}
	${MAKE} -C test

%.o: %.c %.h
	@echo CC $@
	@${CC} -c ${CFLAGS} ${CPPFLAGS} -o $@ $< ${LDFLAGS}

clean:
	@for test in ${OBJS} ${NAME}; do \
	  echo RM $(patsubst %,test/%,$$test); \
	  rm -f $$test; \
	done
	${MAKE} -C test clean
