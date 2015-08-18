include Defs.mk

.PHONY: all test clean

all: ${NAME}

${NAME}: src/main.c ${OBJS}
	@echo CC $@
	@${CC} ${CFLAGS} ${CPPFLAGS} -o $@ $^ ${LDFLAGS}

test: ${OBJS}
	${MAKE} -C test

%.o: %.c %.h
	@echo CC $@
	@${CC} -c ${CFLAGS} ${CPPFLAGS} -o $@ $< ${LDFLAGS}

clean:
	@for test in ${OBJS} ${NAME}; do \
	  echo RM $(patsubst %,test/%,$$test); \
	  ${RM} $$test; \
	done
	${MAKE} -C test clean
