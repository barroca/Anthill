CC = g++ -g -Wall
CFLAGS = -Wno-deprecated -I${HOME}/toolkit/makoConnect/ -I${HOME}/sources/OSCVR/include/OSCVR -I${HOME}/sources/OSCVR/include/ -I./
CLIBS = -L${HOME}/toolkit/makoConnect/ -lMakoConnect -L${HOME}/sources/OSCVR/lib -L${HOME}/lib/ -L/usr/X11R6/lib -L${HOME}/sources/OSCVR/src -L${HOME}/lib/ -L/usr/X11R6/lib -L./ -loscvr_gl -lGLEW -lglut -lGL -lGLU -loscvr -lX11 -lXmu -lXi -lz -lpthread -ldl -lm -DDEBUG
CLIBSDIR =

all: submitLibrary retreiveLibrary 

submitLibrary: submitLibrary.cpp
	${CC} ${CFLAGS} ${CLIBS} submitLibrary.cpp -o submitLibrary

retreiveLibrary: retreiveLibrary.cpp
	${CC} ${CFLAGS} ${CLIBS} retreiveLibrary.cpp -o retreiveLibrary

clean:
	rm -f *.o submitLibrary retreiveLibrary
