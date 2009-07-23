#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "void.h"

typedef int Work;


int main (int argc, char *argv[]) {

	char *confFile = "./conf.xml";
	Work work[1];
	work[0] = 10;

	Layout *layout = initDs(confFile, argc, argv);
	appendWork(layout, (void *)work, sizeof(Work));
	work[0] = 50;
	appendWork(layout, (void *)work, sizeof(Work));
	work[0] = 25;
	appendWork(layout, (void *)work, sizeof(Work));
	finalizeDs(layout);
	return 0;
}

