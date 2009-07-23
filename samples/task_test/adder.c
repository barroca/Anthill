#include <stdio.h>
#include <stdlib.h>
#include "adder.h"

InputPortHandler localCounts;
OutputPortHandler globalCounts;

int initFilter(void *work, int worksize) {
	printf("Inicializing filter B\n");

	localCounts = dsGetInputPortByName("localCounts");
	globalCounts = dsGetOutputPortByName("globalCounts");
	return 0;
}

int processFilter(void *work, int worksize) {	
	int msgSz = sizeof(int);
	int msg = -1;
	printf("Processing filter B\n");
		
	while (dsReadBuffer(localCounts, &msg, msgSz) != EOW) {
		int valSz = -1;		
		int currentTask = dsGetCurrentTask();
		char sumId[] = "sum";
		char qtdId[] = "quantity";
		int *qtd = NULL;
		printf("current task: %d, mesg: %d\n", currentTask, msg);
		
		int *sum = (int *)dsGetData(currentTask, sumId, &valSz);
		if (sum == NULL) {
			sum = malloc(sizeof(int));
			*sum = msg;
			dsPutData(sumId, sum, sizeof(int));
			
			qtd = malloc(sizeof(int));
			*qtd = 1;
			dsPutData(qtdId, qtd, sizeof(int));
		} else {
			*sum += msg;			
			dsPutData(sumId, sum, sizeof(int));
			
			qtd = (int *)dsGetData(currentTask, qtdId, &valSz);
			(*qtd)++;
			dsPutData(qtdId, qtd, sizeof(int));
		}
		
		if ((*qtd) == dsGetNumReaders(globalCounts)) {
			printf("task: %d, counting complete. sum: %d, qtd: %d\n", currentTask, *sum, *qtd);
			dsWriteBuffer(globalCounts, qtd, sizeof(int));
		}
		
		printf("current task: %d, sum %d, qtd: %d\n", currentTask, *sum, *qtd);
		// sum and qtd are copies of the data space content
		free(sum);
		free(qtd);
		
	}
	return 0;
}

int finalizeFilter() {
	printf("Stopping filter B\n");
	return 0;
}

