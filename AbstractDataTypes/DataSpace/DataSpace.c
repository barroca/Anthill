#define _DATASPACE_C_

#include <stdlib.h>
#include <string.h>
#include "DataSpace.h"
#include "TaskData.h"
#include "../constants.h"

DataSpace *createDataSpace() {
	DataSpace *dataSpace = (DataSpace *) malloc(sizeof(DataSpace));
	dataSpace->dataHash = hashStrVoidCreate(INITIAL_CAPACITY);

	setStrVoidSerializer(dataSpace->dataHash, &writeTaskData, &readTaskData);
		
	return dataSpace;
}

void destroyDataSpace(DataSpace * space) {
	hashStrVoidDestroy(space->dataHash);
}

void *getData(DataSpace *space, char *key, int *valSz) {
	void *ret = NULL;
	PosHandlerStrVoid pos = hashStrVoidGet(space->dataHash, key);
	if(pos == NULL){
		return NULL;
	}

	TaskData *taskData = posGetValue(pos);
	*valSz = taskData->valSz;
	if (taskData->valSz > 0) {
		ret = malloc(taskData->valSz);
	}
	memcpy(ret, taskData->val, taskData->valSz);

	return ret;
}

int putData(DataSpace * space, char * key, void * val, int valSz) {
	PosHandlerStrVoid pos = NULL;
	TaskData *taskData = (TaskData *) malloc(sizeof(TaskData));
	
	if(space->dataHash == NULL){
		space->dataHash = hashStrVoidCreate(INITIAL_CAPACITY);
	}
	
	// Get the old val for posterior destruction
	pos = hashStrVoidAdd(space->dataHash, key);
	TaskData *oldTaskData = posGetValue(pos);
	
	taskData->valSz = valSz;
	if (valSz > 0) {
		taskData->val = (char *) malloc(sizeof(char)*valSz);
		memcpy(taskData->val, val, valSz);
	} else {
		taskData->val = NULL;		
	}
	posSetValue(pos, taskData);
	
	// destroy oldTaskData here, because it can be val 
	if (oldTaskData != NULL) {
		// Destroy the old task data, we will overwrite this
		destroyTaskData(oldTaskData);
	}	
	return 0;
}

int removeData(DataSpace *space, char *key) {	
	if(space->dataHash == NULL){
		printf("Error: key = %s dont have saved data\n", key);
		return -1;
	}

	PosHandlerStrVoid pos = hashStrVoidGet(space->dataHash, key);
	if(pos == NULL){
		return -1;
	}

	TaskData *taskData = posGetValue(pos);
	if (taskData != NULL) {
		destroyTaskData(taskData);
	}
	
	// get the handler to intern Hash where key==dataId
	return hashStrVoidRemove(space->dataHash, key);
}

int readDataSpace(FILE *inputFile, DataSpace *space) {
	hashStrVoidDeserialize(inputFile, space->dataHash);
	
	return 1;
}

int  writeDataSpace (FILE *outputSpace, DataSpace *space) {
	hashStrVoidSerialize(outputSpace, space->dataHash);
	
	return 1;
}

