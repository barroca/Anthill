#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "void.h"

typedef int Work;


int main (int argc, char *argv[]) {
	char *confFile = "./conf.xml";
	Work *work = (Work*)malloc(sizeof(Work)); // Aloca e zera work
	*work=10; //numero de vezes que vai rodar la
	
	Layout *manager = initDs(confFile, argc, argv);
	appendWork(manager, (void *)work, sizeof(Work));
	finalizeDs(manager);
	return 0;
}

