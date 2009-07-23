#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "void.h"

// typedef int Work;


int main (int argc, char *argv[]) {
	char confFile[] = "./conf.xml";
	int num_prints = 2;
	int i;
	// Work *work = (Work *)calloc(1, sizeof(Work)); // Alocates and zeroes work
	
	printf("%s\n", argv[0]);
	
	printf("LD = %s\n", getenv("LD_LIBRARY_PATH"));

	Layout *systemLayout = initDs(confFile, argc, argv);

	for(i=1; i<num_prints; i++){
		appendWork(systemLayout, (void *)&i, sizeof(int));
	}
	
	finalizeDs(systemLayout);
	return 0;
}

