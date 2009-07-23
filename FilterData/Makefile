TOPDIR= ..
include ${TOPDIR}/Makefile.conf

LDFLAGS = -ldl


all: FilterData.o hashDefault.o Policies.o Ports.o Instrumentation.o Termination.o

a.out: Ports.o hashDefault.o
	$(CC) $(CFLAGS) $(LDFLAGS) Ports.o hashDefault.o -o a.out 

Ports.o: Ports.c Ports.h hashDefault.h
	$(CC) $(CFLAGS) -c Ports.c -o Ports.o

hashDefault.o: hashDefault.c hashDefault.h
	$(CC) $(CFLAGS) -c hashDefault.c -o hashDefault.o
	
FilterData.o: FilterData.c FilterData.h
	$(CC) $(CFLAGS) -c FilterData.c -o FilterData.o

Policies.o: Policies.c Policies.h
	${CC} ${CFLAGS} -c Policies.c

#Task.o: Task.c Task.h
#	${CC} ${CFLAGS} -c Task.c

Termination.o: Termination.c Termination.h FilterData.h
	${CC} ${CFLAGS} -c Termination.c

Instrumentation.o: Instrumentation.c Instrumentation.h
	${CC} ${CFLAGS} -c Instrumentation.c

clean:
	rm -f *.o a.out
