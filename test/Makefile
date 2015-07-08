include ../Defs.mk

CPPFLAGS+= -I../src
OBJS	:= $(patsubst %,../%,${OBJS})
TESTS	= $(patsubst ./%.c,%,$(shell find . -name '*.c'))
ifndef VERBOSE
TESTARGS= 2>&1 > /dev/null
endif

.PHONY: all clean run_tests

all: run_tests

run_tests: ${TESTS}
	prove ${TESTS}

clean:
	@for test in ${TESTS}; do \
	  echo RM $(patsubst %,test/%,$$test); \
	  ${RM} $$test; \
	done

%: %.c
	@echo CC test/$@
	@${CC} ${CPPFLAGS} ${CFLAGS} ${OBJS} $< -o $@ ${LDFLAGS}
