AH_API_CPP = ${ANTHILL_ROOT}/api_c++
AH_SRC = ${ANTHILL_ROOT}/void_3.1
AH_MT_LIBS = ${ANTHILL_ROOT}/matLab_libs

CC = g++ -g -Wall
CFLAGS = -Wno-deprecated -I${AH_API_CPP} -I${AH_SRC} -I${AH_SRC}/FilterDev -I${PVM_ROOT}/include
CLIBS = -L${AH_API_CPP} -ldscpp -L${AH_SRC} -lds -lexpat -L${AH_MT_LIBS} -L. 

all: libUtil.so 

libUtil.so: Util.cpp
	cp ${AH_API_CPP}/DCBuffer.o .
	cp ${AH_API_CPP}/api_util.o .
	${CC} ${CFLAGS} ${CLIBS} -fPIC -shared -o libUtil.so Util.cpp DCBuffer.o api_util.o


clean:
	rm -f *.o main libUtil.so
