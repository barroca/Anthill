TOPDIR = ../..
include ${TOPDIR}/Makefile.conf


all: Cache.o


test_cache: test_cache.c Cache.o
	${CC} ${CFLAGS} ${LDFLAGS} -lefence \
	../prod_cons/prod_cons.o \
	../Task/Task.o \
	../DataSpace/DataSpace.o \
	../DataSpace/TaskData.o \
	../TaskIdList/TaskIdList.o \
	../cser/cser.o  \
	../Hash/hashstr.o \
	../Hash/str.o \
	../Hash/hashDSIVoid.o \
	../Hash/hashIntInt.o \
	../Hash/hashint.o \
	Cache.o test_cache.c -o test_cache

Cache.o: Cache.c Cache.h
	${CC} ${CFLAGS} -c Cache.c

.PHONY: clean install
clean:
	rm -f *.o *.so test_cache

install: all
	\cp -f Cache.o ${TOPDIR}
