TOPDIR = ../..
include ${TOPDIR}/Makefile.conf


all: Task.o

teste: teste.c Task.o
	${CC} ${CFLAGS} ${LDFLAGS} teste.c Task.o ../DataSpace/DataSpace.o ../Hash/hashstr.o ../DataSpace/TaskData.o ../Hash/util.o ../Hash/str.o ../TaskIdList/TaskIdList.o ../cser/cser.o ../Hash/hashint.o -o teste

Task.o: Task.c Task.h
	${CC} ${CFLAGS} -c Task.c

.PHONY: clean install
clean:
	rm -f *.o *.so

install:
	cp Task.o ${TOPDIR}
