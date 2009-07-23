# This is a sample makefile for compiling filters for void enviroment

#enter void directory here
VOID_DIR=../..
#enter application directory here
APP_DIR=./
# c compiler
CC = gcc  -Wall -g 
# includes
CFLAGS = -I${PVM_ROOT}/include -I${VOID_DIR} -I${VOID_DIR}/FilterDev -I${VOID_DIR}/Hash -I${VOID_DIR}/FilterData -DVOID_FT 
# libs
CLIBS = -L${VOID_DIR} -L${APP_DIR} -L${PVM_ROOT}/lib/LINUX -lds -lpvm3 -lexpat -ldl -lefence


target: main libcounter.so libadder.so

adder.o : adder.c adder.h
	${CC} ${CFLAGS} -c adder.c

libadder.so : adder.o
	${CC} ${CFLAGS} ${CLIBS} -shared adder.o -o libadder.so

counter.o : counter.c counter.h
	${CC} ${CFLAGS} -c counter.c

libcounter.so : counter.o
	${CC} ${CFLAGS} ${CLIBS} -shared counter.o -o libcounter.so

main.o : main.c adder.h counter.h
	${CC} ${CFLAGS} -c main.c

main : main.o
	${CC} ${CFLAGS} ${CLIBS} main.o -o main

clean:
	rm -f *.o main libcounter.so libadder.so start.gdb
