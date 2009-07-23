CC = g++ -g -Wall -fPIC
CFLAGS = -Wno-deprecated -I${HOME}/MCR/v72/extern/include/ -I./..
CLIBS = -L. -L../ -ltest -ldl 
CLIBSDIR =

all: deserialize.so

deserialize.so: deserialize.cpp deserialize.h 
	${CC} ${CFLAGS} -c deserialize.cpp
	${CC} ${CFLAGS} ${CLIBS} -fPIC -shared deserialize.o -o libdeserialize.so

clean:
	rm -f *.o 
