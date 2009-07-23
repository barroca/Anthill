TOPDIR = ../..
include ${TOPDIR}/Makefile.conf


all:cser.o

cser.o: cser.c cser.h
	${CC} ${CFLAGS} -c cser.c -o cser.o

test: cser.c cser.h test.c
	$(CC) ${CFLAGS} -o test cser.c test.c

install: all
	\cp -f cser.o ${TOPDIR}

clean:
	rm -f test cser.o test.o
