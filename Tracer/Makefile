TOPDIR= ..

include ${TOPDIR}/Makefile.conf

tracer.o: tracer.c tracer.h
	${CC} ${CFLAGS} -c tracer.c

test: tracer.c tracer.h
	${CC} ${CFLAGS} -c -DTRACER_MAINTEST tracer.c
	${CC} ${CFLAGS} ../AbstractDataTypes/stack/stack.o ../AbstractDataTypes/dbLkList/doublelinkedlist.o tracer.o -o test

clean:
	rm -rf tracer.o test bla
