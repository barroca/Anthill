CC = g++ -g -Wall -fPIC
CFLAGS = -I../../ -I../../../ -I${PVM_ROOT}/include -I../../../FilterDev -I../../../AbstractDataTypes/Hash/ -I../../../AbstractDataTypes/AppParameters/ -I../../../ahBuffer/
CLIBS = -L../../ -lahevent -L../../../ -lds -lexpat -lpthread
CLIBSDIR =


all: reader.so base_gen classifier.so merger.so main.o

reader.so: reader.cpp reader.h
	${CC} ${CFLAGS} -c reader.cpp 
	${CC} ${CFLAGS} ${CLIBS} -fPIC -shared reader.o -o reader.so 

classifier.so: classifier.cpp classifier.h structs.h  constants.h
	${CC} ${CFLAGS} -c -fPIC classifier.cpp 
	${CC} ${CFLAGS} ${CLIBS} -fPIC -shared classifier.cpp -o classifier.so `pkg-config --cflags --libs sigc++-2.0`

merger.so: merger.cpp merger.h structs.h
	${CC} ${CFLAGS} -c merger.cpp 
	${CC} ${CFLAGS} ${CLIBS} -fPIC -shared merger.o -o merger.so

base_gen: train_base_gen.cpp test_base_gen.cpp
	${CC} ${CFLAGS} train_base_gen.cpp -o train_gen
	${CC} ${CFLAGS} test_base_gen.cpp -o test_gen

main.o : main.c
	${CC} ${CFLAGS} -L../../../ -lds main.c -o main

clean:
	rm -f *.o test_gen train_gen reader.so classifier.so merger.so main *.atc
