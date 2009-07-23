TOPDIR = ../..
include ${TOPDIR}/Makefile.conf

all: hashint.o hashstr.o hashDSIVoid.o hashIntInt.o

test_hash: hashstr.o hashIntInt.o hashIntFloat.o hashDSIVoid.o str.o test_hash.c
	${CC} ${CFLAGS} ${LDFLAGS} test_hash.c hashstr.o hashIntInt.o hashIntFloat.o hashDSIVoid.o str.o ../cser/cser.o ../TaskIdList/TaskIdList.o -o test_hash

hashIntInt.o: hash.c hash.h util.o
	${CC} ${CFLAGS} -c -DKEY_INT -DVAL_INT hash.c -o hashIntInt.o

hashIntFloat.o: hash.c hash.h util.o
	${CC} ${CFLAGS} -c -DKEY_INT -DVAL_FLOAT hash.c -o hashIntFloat.o

hashDSIVoid.o: hash.c hash.h util.o
	${CC} ${CFLAGS} -c -DKEY_DSI -DVAL_VOID hash.c -o hashDSIVoid.o

hashint.o: hash.c hash.h util.o
	${CC} ${CFLAGS} -c -DKEY_INT -DVAL_VOID hash.c -o hashint.o

hashstr.o: hash.c hash.h util.o str.o
	${CC} ${CFLAGS} -c -DKEY_STRING -DVAL_VOID hash.c -o hashstr.o

str.o: str.c str.h
	${CC} ${CFLAGS} -c str.c -o str.o	

util.o: util.h util.c
	${CC} ${CFLAGS} -c util.c -o util.o

.PHONY: clean install
clean:
	rm -f *.o *.so test_hash

install:
	cp *.o ${TOPDIR}
