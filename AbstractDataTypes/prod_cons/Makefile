TOPDIR = ../..
include ${TOPDIR}/Makefile.conf


all: prod_cons.o


prod_cons.o: prod_cons.c prod_cons.h
	${CC} ${CFLAGS} -c prod_cons.c

.PHONY: clean install
clean:
	rm -f *.o *.so

install: all
	\cp -f prod_cons.o ${TOPDIR}
