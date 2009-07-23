TOPDIR= ..
include ${TOPDIR}/Makefile.conf
CC1 =  g++ -Wall -fPIC -DREENTRANT


all: eventAPI.o
	${CC1} ${CFLAGS} ${CLIBSDIR} ${CLIBS} -fPIC -shared eventAPI.o -o ${EVENTAPILIBNAME} 
	
main: eventAPI.o
	${CC1} ${CFLAGS} ${CLIBSDIR} ${CLIBS} -fPIC eventAPI.o -o main 

eventAPI.o: eventAPI.cpp eventAPI.h eventExceptions.h
	${CC1} ${CFLAGS} -c eventAPI.cpp

clean:
	rm -f *.o ${EVENTAPILIBNAME} main
