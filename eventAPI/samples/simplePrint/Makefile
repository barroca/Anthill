CC = g++ -g -Wall -fPIC
CFLAGS = -I../../ -I../../../ -I${PVM_ROOT}/include -I../../../FilterDev -I../../../AbstractDataTypes/Hash/ -I../../../AbstractDataTypes/AppParameters/ -I../../../ahBuffer/
CLIBS = -L../../ -lahevent -L../../../ -lds -lexpat #-lefence 
CLIBSDIR =


all: filterA.so filterB.so main.o

filterA.so: filterA.cpp filterA.h
	${CC} ${CFLAGS} -c filterA.cpp
	${CC} ${CFLAGS} ${CLIBS} -fPIC -shared filterA.o -o filterA.so 

filterB.so: filterB.cpp filterB.h
	${CC} ${CFLAGS} -c filterB.cpp
	${CC} ${CFLAGS} ${CLIBS} -fPIC -shared filterB.o -o filterB.so


main.o : main.c
	${CC} ${CFLAGS} -L../../../ -lds main.c -o main 

clean:
	rm -f *.o filterA.so filterB.so main *.atc
