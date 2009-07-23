
all: invertedList.o main invertedListTest

main: invertedList.o main.cpp
	g++ -g invertedList.o main.cpp -o main

invertedList.o: invertedList.h invertedList.cpp
	g++ -g -c invertedList.cpp

invertedListTest: invertedList.o invertedListTest.cpp
	g++ -g invertedList.o invertedListTest.cpp -o invertedListTest

clean:
	rm -rf invertedList.o main invertedListTest
