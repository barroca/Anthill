CC = g++ -g -Wall
CFLAGS = -Wno-deprecated -I${COMPILED_MATLAB_ROOT}/matlabinclude -I${COMPILED_MATLAB_ROOT}/test/sharedlib -I${ANTHILL_ROOT}/api_c++ -DDEBUG
CLIBS = -L. -L${COMPILED_MATLAB_ROOT}/test/sharedlib -ltest  
CLIBSDIR =

all: copylbs main deserialize.so serialize.so

copylbs:
	cp ${HOME}/void/api_c++/DCBuffer.o .
	cp ${HOME}/void/api_c++/api_util.o .

deserialize.so: deserialize.cpp deserialize.h 
	${CC} ${CFLAGS} -c deserialize.cpp
	${CC} ${CFLAGS} ${CLIBS} -fPIC -shared  DCBuffer.o api_util.o deserialize.o -o libdeserialize.so

serialize.so: serialize.cpp serialize.h 
	${CC} ${CFLAGS} -c serialize.cpp
	${CC} ${CFLAGS} ${CLIBS} -fPIC -shared  DCBuffer.o api_util.o serialize.o -o libserialize.so

main: deserialize.so serialize.so main.cpp 
	g++ -g -Wall ${CFLAGS} ${CLIBS} -ldeserialize -lserialize main.cpp -o main

clean:
	rm -f filterA.so main 
