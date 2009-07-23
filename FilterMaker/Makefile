TOPDIR= ..
include ${TOPDIR}/Makefile.conf

all: MatLabUtil.o MatLabFilter.o MatLabMain.o MatLabMakefile.o ConfMaker.o

MatLabUtil.o: MatLabUtil.c MatLabUtil.h ../constants.h ../structs.h
	${CC} ${CFLAGS} -DMATLAB -c MatLabUtil.c

MatLabFilter.o: MatLabFilter.c MatLabFilter.h ../constants.h ../structs.h
	${CC} ${CFLAGS} -DMATLAB -c MatLabFilter.c

MatLabMain.o: MatLabMain.c MatLabMain.h ../constants.h ../structs.h
	${CC} ${CFLAGS} -DMATLAB -c MatLabMain.c
	
MatLabMakefile.o: MatLabMakefile.c MatLabMakefile.h ../constants.h ../structs.h
	${CC} ${CFLAGS} -DMATLAB -c MatLabMakefile.c 

ConfMaker.o: ConfMaker.c ConfMaker.h ../constants.h ../structs.h
	${CC} ${CFLAGS} -DMATLAB -c ConfMaker.c 


#main: main.c MatLabUtil.c MatLabUtil.h MatLabFilter.c MatLabFilter.h MatLabMain.c MatLabMain.h MatLabMakefile.c MatLabMakefile.h ../constants.h ../structs.h
#	cp ../parser.o .
#	cp ../Hosts.o .
#	cp ../FilterSpec.o .
#	cp ../Layout.o .
#	cp ../StreamSpec.o .
#	cp ../MatLabDesc.o .
#	cp ../FilterData/Policies.o .
#	${CC} ${CFLAGS} ${CLIBSDIR} ${CLIBS} main.c Policies.o MatLabDesc.o Layout.o StreamSpec.o FilterSpec.o parser.o Hosts.o MatLabFilter.o MatLabUtil.o MatLabMain.o MatLabMakefile.o -o main -lpvm3
	
clean:
	rm -f *.o main
