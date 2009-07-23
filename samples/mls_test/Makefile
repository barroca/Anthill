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

# put all filters shared libraries here, and main application
# example: target: divP4.so imp.so leNum.so main
target: main rn.so print.so mls_label.so

#for each filter above, put the source here, dont use -shared for main
rn.so: readNumber.c
	${CC} ${CFLAGS} ${CLIBS} -shared -o rn.so readNumber.c 

mls_label.so: mls_label.c
	${CC} ${CFLAGS} ${CLIBS} -shared -o mls_label.so mls_label.c 

print.so: print.c
	${CC} ${CFLAGS} ${CLIBS} -shared -o print.so print.c 

main: main.c
	${CC} ${CFLAGS} ${CLIBS} -o main main.c

 
# this is the clean rule, leave as it is
clean:
	rm -f *.o *.so main
