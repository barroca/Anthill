#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "Manager.h"

typedef int Work;


int main (int argc, char *argv[]) {

	char *confFile = "./conf.xml";
	Work work[1];

	if (argc > 1){
		work[0] = atoi(argv[1]);
		printf("Generating %d numbers, per arg\n", work[0]);
	}
	else {
		printf("Generating 10 numbers(no arg)\n");
		work[0] = 10;
	}
	

	Layout *layout = initDs(confFile, argc, argv);
	appendWork(layout, (void *)work, sizeof(Work));
	finalizeDs(layout);
	return 0;
}

