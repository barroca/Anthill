TOPDIR= ..
include ${TOPDIR}/Makefile.conf



all: dcbuffer.o api_util.o api_cpp.o
	${CC} ${CFLAGS} ${CLIBS} -fPIC -shared  api_cpp.o DCBuffer.o api_util.o -o ${APICPPLIBNAME} 

api_cpp.o: api_cpp.cpp api_cpp.h
	${CC} ${CFLAGS} -c api_cpp.cpp

dcbuffer.o: DCBuffer.cpp DCBuffer.h
	${CC} ${CFLAGS} -c DCBuffer.cpp

api_util.o: api_util.cpp api_util.h
	${CC} ${CFLAGS} -c api_util.cpp

clean:
	rm -f *.o libdscpp.so main a.out
