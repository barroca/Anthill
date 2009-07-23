TOPDIR = ../..
include ${TOPDIR}/Makefile.conf


all: TaskData.o DataSpace.o

testeTaskData: testeTaskData.c TaskData.o
	${CC} ${CFLAGS} ${LDFLAGS} -g testeTaskData.c TaskData.o ../cser/cser.o -o testeTaskData

testeDataSpace: testeDataSpace.c DataSpace.o TaskData.o
	${CC} ${CFLAGS} ${LDFLAGS} -g testeDataSpace.c DataSpace.o TaskData.o ../cser/cser.o ../Hash/hashstr.o ../Hash/util.o ../Hash/str.o -o testeDataSpace

TaskData.o: TaskData.c TaskData.h
	${CC} ${CFLAGS} -c TaskData.c

DataSpace.o: DataSpace.c DataSpace.h
	${CC} ${CFLAGS} -c DataSpace.c

.PHONY: clean install
clean:
	rm -f *.o *.so testeTaskData testeDataSpace

install: all
	\cp -f DataSpace.o TaskData.o ${TOPDIR}
