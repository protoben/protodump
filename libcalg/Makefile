PARTS = trie
OBJS  = $(PARTS:%=%.o)

all:

%.o: %.c %.h
	@echo "cc $<"
	@${CC} -c ${CFLAGS} ${LDFLAGS} $<

clean:
	rm -f ${OBJS}
