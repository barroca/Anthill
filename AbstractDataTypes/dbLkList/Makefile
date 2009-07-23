TOPDIR = ../..
include ${TOPDIR}/Makefile.conf


SRC=doublelinkedlist
OBJNAME=${SRC}.o

target: ${OBJNAME}

${OBJNAME}: dbLkList.c dbLkList.h
	${CC} ${CFLAGS} -c -o ${OBJNAME} dbLkList.c

#install rule
.PHONY: install
install: 
	cp ${OBJNAME} ${TOPDIR} 

# this is the clean rule, leave as it is
clean:
	rm -f *.o 
