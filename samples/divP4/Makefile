CC = gcc  -Wall -g -fPIC
CFLAGS = -I${PVM_ROOT}/include -I../../FilterDev -I../.. -I../../Hash
CLIBS = -L../.. -L${PVM_ROOT}/lib/LINUX${PROCESSOR_TYPE} -lds -lpvm3 -lexpat -ldl 

target: divP4.so imp.so leNum.so main

divP4.so: divP4.c
	${CC} ${CFLAGS} ${CLIBS} -shared -o divP4.so divP4.c 

imp.so: imp.c 
	${CC} ${CFLAGS} ${CLIBS} -shared -o imp.so imp.c

leNum.so: leNum.c
	${CC} ${CFLAGS} ${CLIBS} -shared -o leNum.so leNum.c

main: main.c
	${CC} ${CFLAGS} ${CLIBS} main.c -o main

clean:
	rm -f *.o divP4.so imp.so leNum.so main
