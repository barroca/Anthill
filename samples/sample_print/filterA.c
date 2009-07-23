#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filterA.h"

OutputPortHandler output;
int *numToPrint;


int initFilter(void *work, int size) {
	numToPrint = (int *) work;

	printf("%d\n", *numToPrint);

	printf("Inicializing filter A\n");

	output = dsGetOutputPortByName("output");

	return 0;
}

int processFilter(void *work, int size) {	
	printf("Processing filter A\n");
	int *deps = NULL;
	int depSize = 0;
	char *metadata = NULL;
	int metaSize = 0;
	int taskId = 12;
//	dsCreateTask(taskId, deps, depSize, metadata, metaSize);
	char *numToPrintS = (char *) malloc(sizeof(char) * 104857600);
	dsWriteBuffer(output, (void *)numToPrintS, 104857600);

//	fprintf(stderr, "Sending %d to be printed\n", *numToPrint);
	free(numToPrintS);
	return 0;
}

int finalizeFilter() {
	printf("Stoping filter A\n");
	return 0;
}

