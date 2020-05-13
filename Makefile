CFLAGS=-std=c11 -g -static
chibicc: chibicc.c

test:chibicc
	./test.sh

clean:
	rm -rf chibicc *.o *~ tmp*

.PHONY: test clean
