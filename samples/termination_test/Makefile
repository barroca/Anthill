# This is a sample makefile for compiling filters for void enviroment

#enter void directory here
VOID_DIR=/home/speed/coutinho/datacutter/void_3.1/
#enter application directory here
APP_DIR=/home/speed/coutinho/datacutter/void_3.1/samples/labeled_stream_test/
# c compiler
CC = gcc  -Wall -g 
# includes
CFLAGS = -I${PVM_ROOT}/include -I${VOID_DIR} -I${VOID_DIR}/FilterDev -I${VOID_DIR}/Hash
# libs
CLIBS = -L${VOID_DIR} -L${APP_DIR} -L${PVM_ROOT}/lib/LINUX -lds -lpvm3 -lexpat -ldl 

# put all filters shared libraries here, and main application
# example: target: divP4.so imp.so leNum.so main
target: main EnMsg.so acc.so label.so

#for each filter above, put the source here, dont use -shared for main
#example:
#divP4.so: divP4.c
#	${CC} ${CFLAGS} ${CLIBS} -shared -o divP4.so divP4.c 

EnMsg.so: EnMsg.c
	${CC} ${CFLAGS} ${CLIBS} -shared -o EnMsg.so EnMsg.c 

label.so: labelfunc.c
	${CC} ${CFLAGS} ${CLIBS} -shared -o label.so labelfunc.c 

acc.so: acc.c
	${CC} ${CFLAGS} ${CLIBS} -shared -o acc.so acc.c 


main: main.c
	${CC} ${CFLAGS} ${CLIBS} -o main main.c

 
# this is the clean rule, leave as it is
clean:
	rm -f *.o EnMsg.so acc.so label.so main
