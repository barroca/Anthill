TOPDIR=../..
include ${TOPDIR}/Makefile.conf

SRC=stack


OBJNAME=${SRC}.o

target: ${OBJNAME}

${OBJNAME}: ${SRC}.c ${SRC}.h
	${CC} ${CFLAGS} -c -o ${OBJNAME} ${SRC}.c

#install rule
.PHONY: install
install: 
	cp ${OBJNAME} ${TOPDIR}

# this is the clean rule, leave as it is
clean:
	rm -f *.o 
