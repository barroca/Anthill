# This is a sample makefile for compiling filters for void enviroment

#enter void directory here
VOID_DIR=../..
#enter application directory here
APP_DIR=./
# c compiler
CC = gcc  -Wall -g 
# includes
CFLAGS = -I${PVM_ROOT}/include -I${VOID_DIR} -I${VOID_DIR}/FilterDev -I${VOID_DIR}/Hash -I${VOID_DIR}/FilterData
# libs
CLIBS = -L${VOID_DIR} -L${APP_DIR} -L${PVM_ROOT}/lib/LINUX -lds -lpvm3 -lexpat -ldl 


target: main libfilterA.so libfilterB.so

filterB.o : filterB.c
	${CC} ${CFLAGS} ${INCL} -c filterB.c

libfilterB.so : filterB.o
	${CC} ${CLIBS} -shared filterB.o -o libfilterB.so

filterA.o : filterA.c
	${CC} ${CFLAGS} ${INCL} -c filterA.c

libfilterA.so : filterA.o
	${CC} ${CLIBS} -shared filterA.o -o libfilterA.so

main.o : main.c libfilterA.so libfilterB.so
	${CC} ${CFLAGS} ${INCL} -c main.c

main : main.o
	${CC} ${CLIBS} main.o -o main
	
clean:
	rm -f *.o main libfilterA.so libfilterB.so
