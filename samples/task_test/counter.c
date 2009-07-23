#include <stdio.h>
#include "counter.h"

OutputPortHandler localCounts;
InputPortHandler globalCounts;

int initFilter(void *work, int worksize) {
	printf("Inicializing filter A\n");
	
	localCounts = dsGetOutputPortByName("localCounts");
	globalCounts = dsGetInputPortByName("globalCounts");
	return 0;
}

int processFilter(void *work, int worksize) {	
	printf("Processing filter A\n");
	int j,currentTask = -1;
	char *metadata = NULL;
	int *deps = NULL;
	int nextTask=0;

	// currentTask starts with -1 (if we aren't recovering from a fault)
	currentTask = dsGetCurrentTask();
	if (currentTask < 0) {
		dsCreateTask(0, deps, 0, metadata, 0);
		dsWriteBuffer(localCounts, &nextTask, sizeof(int));
		dsReadBuffer(globalCounts, &j, sizeof(int));
	}
	
	while (currentTask<20) {
		currentTask = dsGetCurrentTask();

			nextTask = currentTask+1;			
			dsCreateTask(nextTask, deps, 0, metadata, 0);
			// tarefa nextTask n?o depende da tarefa currentTask
			int status = dsSetCurrentTask(nextTask);
			if (status != 0) {
				printf("Error setting current task to %d\n", nextTask);
			}
			dsWriteBuffer(localCounts, &nextTask, sizeof(int));
		
			
			dsReadBuffer(globalCounts, &j, sizeof(int));
			// The current task must not change
			assert(nextTask == dsGetCurrentTask());
			printf("Task:%d, Count:%d\n", currentTask, j);		
		
		dsEndTask(currentTask);	
		currentTask=nextTask;
	}	
	dsEndTask(currentTask);
	
	return 0;
}

int finalizeFilter() {
	printf("stoping filter A\n");
	return 0;
}

