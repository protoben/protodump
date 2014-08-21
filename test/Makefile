PARTS = test_data

all:

test_data: test_data.c ../common.o ../data.o ../libcalg/trie.o
	@echo "cc test_data"
	@${CC} -o $@ ${CFLAGS} $^ ${LDFLAGS}

../%.o:
	@make -C .. $*.o

clean:
	rm -f ${PARTS}
