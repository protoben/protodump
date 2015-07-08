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
	@for test in ${TESTS}; do \
	  echo -n TEST $(patsubst %,test/%,$$test); \
	  if ./$$test ${TESTARGS}; then \
	    echo -e " \e[1;34mPASSED\e[0m"; \
	  else \
	    echo -e " \e[1;31mFAILED\e[0m"; \
	  fi \
	done

clean:
	@for test in ${TESTS}; do \
	  echo RM $(patsubst %,test/%,$$test); \
	  ${RM} $$test; \
	done

%: %.c
	@echo CC test/$@
	@${CC} ${CPPFLAGS} ${CFLAGS} ${OBJS} $< -o $@ ${LDFLAGS}
