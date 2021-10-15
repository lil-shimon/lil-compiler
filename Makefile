CFLAGS=-std=c11 -g -static

lilcc: lilcc.c

test: lilcc
				./test.sh

clean:
				rm -f lilcc *.o *~ tmp*

.PHONY: test clean
