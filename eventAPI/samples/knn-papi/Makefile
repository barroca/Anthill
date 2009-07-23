CC = g++ -g -Wall -fPIC
CFLAGS = -I../../ -I../../../ -I${PVM_ROOT}/include -I../../../FilterDev -I../../../AbstractDataTypes/Hash/ -I../../../AbstractDataTypes/AppParameters/ -I../../../ahBuffer/
CLIBS = -L../../ -lahevent -L../../../ -lds -lexpat -lpthread
CLIBSDIR =


all: reader.so base_gen classifier.so merger.so main.o papi_counters.o

reader.so: reader.cpp reader.h
	${CC} ${CFLAGS} -c reader.cpp `pkg-config --cflags --libs sigc++-2.0`
	${CC} ${CFLAGS} ${CLIBS} -fPIC -shared reader.o -o reader.so 

papi_counters.o: papi_counters.h papi_counters.c
	${CC} ${CFLAGS} -c -fPIC papi_counters.c 

classifier.so: classifier.cpp classifier.h structs.h  constants.h papi_counters.o
	${CC} ${CFLAGS} -c -fPIC classifier.cpp `pkg-config --cflags --libs sigc++-2.0`
	${CC} ${CFLAGS} ${CLIBS} -fPIC -shared papi_counters.c classifier.cpp /usr/local/lib64/libpapi.a -o classifier.so `pkg-config --cflags --libs sigc++-2.0`

merger.so: merger.cpp merger.h structs.h
	${CC} ${CFLAGS} -c merger.cpp `pkg-config --cflags --libs sigc++-2.0`
	${CC} ${CFLAGS} ${CLIBS} -fPIC -shared merger.o -o merger.so

base_gen: train_base_gen.cpp test_base_gen.cpp
	${CC} ${CFLAGS} train_base_gen.cpp -o train_gen
	${CC} ${CFLAGS} test_base_gen.cpp -o test_gen

main.o : main.c
	${CC} ${CFLAGS} -L../../../ -lds main.c -o main `pkg-config --cflags --libs sigc++-2.0`

clean:
	rm -f *.o test_gen train_gen reader.so classifier.so merger.so main *.atc
