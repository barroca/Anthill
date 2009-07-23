TOPDIR = ../..
include ${TOPDIR}/Makefile.conf


all: TaskIdList.o TaskIdListSend.o

teste: teste.c TaskIdList.o
	${CC} ${CFLAGS} ${LDFLAGS} -lefence teste.c TaskIdList.o ../cser/cser.o ../Hash/hashIntInt.o ../Hash/str.o ../Hash/util.o -o teste

TaskIdList.o: TaskIdList.c TaskIdList.h
	${CC} ${CFLAGS} -c TaskIdList.c -o TaskIdList.o

TaskIdListSend.o: TaskIdList.c TaskIdList.h
	${CC} ${CFLAGS} -c -DTASK_ID_LIST_SEND TaskIdList.c -o TaskIdListSend.o


.PHONY: clean install
clean:
	rm -f *.o *.so teste

install:
	cp TaskIdList.o TaskIdListSend.o ${TOPDIR}
