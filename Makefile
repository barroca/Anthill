TOPDIR= .

include ${TOPDIR}/Makefile.conf

# defaul action, build the library
all: ${REALNAME} ${FILTERMAKERMAIN}

${REALNAME}: Layout.o FilterSpec.o StreamSpec.o Manager.o parser.o Hosts.o MatLabDesc.o ${SUBDIRS}
	$(CC) -fPIC -shared ${CLIBSDIR} ${CLIBS} Layout.o StreamSpec.o Manager.o parser.o FilterSpec.o Hosts.o FilterData/*.o FilterDev/*.o util.o str.o hashint.o hashstr.o TaskIdList.o TaskIdListSend.o cser.o DataSpace.o TaskData.o prod_cons.o Cache.o Task.o stack.o doublelinkedlist.o hashDSIVoid.o hashIntInt.o Tracer/tracer.o MatLabDesc.o -o ${REALNAME} -lpvm3
	 ln -sf ${REALNAME} ${SONAME}
	 ln -sf ${REALNAME} ${LINKERNAME}

${FILTERMAKERMAIN}: filterMaker.c MatLabDesc.o Layout.o StreamSpec.o FilterSpec.o parser.o Hosts.o ${SUBDIRS}
	${CC} ${CFLAGS} ${CLIBSDIR} ${CLIBS} filterMaker.c FilterMaker/*.o MatLabDesc.o Layout.o StreamSpec.o FilterSpec.o parser.o Hosts.o FilterData/Policies.o -o ${FILTERMAKERMAIN} -lpvm3

Manager.o : Manager.c Manager.h parser.h Messages.h
	${CC} ${CFLAGS} -c Manager.c

FilterSpec.o: FilterSpec.c FilterSpec.h constants.h structs.h 
	${CC} ${CFLAGS} -c FilterSpec.c

Hosts.o: Hosts.c Hosts.h constants.h
	${CC} ${CFLAGS} -c Hosts.c

Layout.o : Layout.c Layout.h constants.h structs.h 
	${CC} ${CFLAGS} -c Layout.c

StreamSpec.o : StreamSpec.c StreamSpec.h FilterSpec.h FilterData/Policies.h 
	${CC} ${CFLAGS} -c StreamSpec.c

parser.o: parser.c parser.h constants.h
	${CC} ${CFLAGS} -c parser.c

MatLabDesc.o: MatLabDesc.c MatLabDesc.h constants.h structs.h 
	${CC} ${CFLAGS} -c MatLabDesc.c

structs.h: Hosts.h constants.h

# phony rules
.PHONY: ${SUBDIRS} clean install

#call subdirectories
${SUBDIRS}:
	make -C $@

#clean rule
clean:
	make -C FilterDev clean
	make -C FilterData clean
	make -C AbstractDataTypes clean
	make -C Tracer clean
	make -C FilterMaker clean
	make -C api_c++ clean
	rm -f *.so *.so.* *.o filtermaker

# install rule: install the lib and the header files
install:
	mkdir -p ${PREFIX}/lib
	cp ${REALNAME} ${SONAME} ${LINKERNAME} api_c++/${APICPPLIBNAME} ${PREFIX}/lib
	mkdir -p ${PREFIX}/include/FilterDev
	cp FilterDev/FilterDev.h  ${PREFIX}/include/FilterDev
	cp *.h api_c++/api_cpp.h ${PREFIX}/include
	mkdir -p ${PREFIX}/bin
	cp ${FILTERMAKERMAIN} ${PREFIX}/bin
