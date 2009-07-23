TOPDIR= ..
include ${TOPDIR}/Makefile.conf


all: FilterDev.o

FilterDev.o: FilterDev.c FilterDev.h ../FilterData/FilterData.h ../constants.h
	${CC} ${CFLAGS} -c FilterDev.c 

clean:
	rm -f *.o
