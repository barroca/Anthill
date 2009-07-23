CC = g++ -g -Wall -fPIC -DREENTRANT
CFLAGS = -I../../ -I../../../ -I${PVM_ROOT}/include -I../../../FilterDev -I../../../AbstractDataTypes/Hash/ -I../../../AbstractDataTypes/AppParameters/ -I../../../ahBuffer/ #-DAPP_DEBUG 
CLIBS = -L../../ -lahevent -L../../../ -lds -lexpat
CLIBSDIR =


all: reader.so merger.so adder.so genCandidates.so label.so main.o generator

reader.so: reader.cpp reader.h
	${CC} ${CFLAGS} -c reader.cpp `pkg-config --cflags --libs sigc++-2.0`
	${CC} ${CFLAGS} ${CLIBS} -fPIC -shared reader.o -o reader.so 

merger.so: merger.cpp merger.h invertedList.o
	${CC} ${CFLAGS} -c merger.cpp `pkg-config --cflags --libs sigc++-2.0`
	${CC} ${CFLAGS} ${CLIBS} -fPIC -shared invertedList.o merger.o -o merger.so

adder.so: adder.cpp adder.h
	${CC} ${CFLAGS} -c adder.cpp `pkg-config --cflags --libs sigc++-2.0`
	${CC} ${CFLAGS} ${CLIBS} -fPIC -shared adder.o -o adder.so

genCandidates.so: genCandidates.cpp genCandidates.h itemsets_to_calc.o
	${CC} ${CFLAGS} -c genCandidates.cpp `pkg-config --cflags --libs sigc++-2.0`
	${CC} ${CFLAGS} ${CLIBS} -fPIC -shared genCandidates.o itemsets_to_calc.o -o genCandidates.so

itemsets_to_calc.o: itemsets_to_calc.cpp itemsets_to_calc.h
	${CC} ${CFLAGS} -c itemsets_to_calc.cpp 

label.so: labelfunc.c
	${CC} ${CFLAGS} ${CLIBS} -shared -o label.so labelfunc.c 

invertedList.o: invertedList.cpp invertedList.h
	${CC} ${CFLAGS} -c invertedList.cpp 

generator:  crazyGenerate.c
	${CC} ${CFLAGS} ${CLIBS} crazyGenerate.c -o generator


main.o : main.c
	${CC} ${CFLAGS} -L../../../ -lds main.c -o main `pkg-config --cflags --libs sigc++-2.0`

clean:
	rm -f *.o reader.so merger.so adder.so genCandidates.so label.so main *.atc
