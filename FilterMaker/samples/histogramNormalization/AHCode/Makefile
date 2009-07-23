AH_API_CPP = ${ANTHILL_ROOT}/api_c++
AH_SRC = ${ANTHILL_ROOT}/void_3.1

MT_LIB = ${COMPILED_MATLAB_ROOT}/lib
MT_LIBINLUCDE = ${COMPILED_MATLAB_ROOT}/libinclude
MT_INCLUDE = ${COMPILED_MATLAB_ROOT}/matlabinclude

CC = g++ -g -Wall
CFLAGS = -Wno-deprecated -I${AH_API_CPP} -I${AH_SRC} -I${AH_SRC}/FilterDev -I${PVM_ROOT}/include -I${MT_LIBINLUCDE} -I${MT_INCLUDE} -I${CACHE_ROOT}
CLIBS = -L${AH_API_CPP} -ldscpp -L${AH_SRC} -lds -lexpat -L. -L${MT_LIB} -lTKHistogramNormalization -lserialize -ldeserialize -lTKHistogramNormalization1

all: main filterA.so filterB.so

filterA.so: filterA.cpp
	${CC} ${CFLAGS} ${CLIBS} -fPIC -shared -o filterA.so filterA.cpp

filterB.so: filterB.cpp
	${CC} ${CFLAGS} ${CLIBS} -fPIC -shared -o filterB.so filterB.cpp

main: matlab_filters_main.cpp
	cp ${AH_API_CPP}/DCBuffer.o .
	cp ${AH_API_CPP}/api_util.o .
	${CC} ${CFLAGS} -L${AH_SRC} -lds DCBuffer.o api_util.o matlab_filters_main.cpp -o main

clean:
	rm -f *.o main filterA.so filterB.so