#define _TASKDATA_C_

#include <stdlib.h>
#include "TaskData.h"
#include "cser.h"

TaskData *createTaskData () {
	TaskData *taskData = (TaskData *) calloc(1, sizeof(TaskData));

	taskData->valSz = 0;
	taskData->val = NULL;

	return taskData;
}

void destroyTaskData(TaskData *taskData) {
	if(taskData->valSz > 0) {
		free(taskData->val);
	}

	free(taskData);
}

void *readTaskData(FILE *inputFile) {
	int valSz;
	TaskData *taskData = (TaskData *) malloc(sizeof(TaskData));

	READ_BEGIN(inputFile);
	
	// Read data size from disk
	READ_NUM("valSz", valSz);
	taskData->valSz = valSz;

	if (valSz > 0) {
		taskData->val = calloc(taskData->valSz, sizeof(void));
		READ_BYTES(inputFile, taskData->val, taskData->valSz);
	} else {
		taskData->val = NULL;
	}

	READ_END
	
	return (void *) taskData;
}

int writeTaskData(FILE *outputFile, void *taskData) {
	TaskData *pTaskData = (TaskData *) taskData;

	// Write data size to disk
	WRITE_NUM(outputFile, "valSz", pTaskData->valSz);

	if (pTaskData->valSz > 0) {
		// Write data value to disk
		WRITE_BYTES(outputFile, pTaskData->val, pTaskData->valSz);
	}

	return 1;
}

int getTaskDataSz(TaskData *taskData) {
	return taskData->valSz;
}

void putTaskDataSz(TaskData *taskData, int valSz) {
	taskData->valSz = valSz;
}

void *getTaskDataVal(TaskData *taskData) {
	return taskData->val;
}

void putTaskDataVal(TaskData *taskData, void *val, int valSz) {
	taskData->val = malloc(valSz);
	
	memcpy(taskData->val, val, valSz);
}

