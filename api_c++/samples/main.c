#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "void.h"

typedef int Work;


int main (int argc, char *argv[]) {
	char confFile[] = "./conf.xml";
	Work *work = (Work *)calloc(1, sizeof(Work)); // Alocates and zeroes work
	*work = 100;

	printf("%s\n", argv[0]);
	
	Layout *systemLayout = initDs(confFile, argc, argv);
	appendWork(systemLayout, (void *)&work, sizeof(Work));
	
	finalizeDs(systemLayout);
	return 0;
}

