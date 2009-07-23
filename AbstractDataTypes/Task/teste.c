#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Task.h"
#include "DataSpace.h"
#include "FilterData.h"



FILE *pFile;

// Coutinho: This is olny to this sample compile. Should never be used
FilterData *fd;


int main() {
	Task *task1 = createTask();
	Task *task2 = NULL;
	int i, j, ret, result = 0;
	int valSz1 = 20;
	int array1[100];
	int valSz = 100;

	srandom(107);

	TaskIdList *list1 = taskIdListCreate(10);
	TaskIdList *list2 = taskIdListCreate(10);
	
//////////////////////////////////////////////////////////

	char *val1 = malloc(valSz);
	char *val2 = malloc(valSz);
	char *val3 = malloc(valSz);
	char *val4 = NULL;
	char *val5 = NULL;
	char *val6 = NULL;
	FILE *pFile = NULL;

	for(i = 0; i < valSz; i++) {
		val1[i] = (char) (random() % 256);
	}
	putData(task1->dataSpace, "teste1", val1, valSz);

	for(i = 0; i < valSz; i++) {
		val2[i] = (char) (random() % 256);
	}
	putData(task1->dataSpace, "teste2", val2, valSz);

	for(i = 0; i < valSz; i++) {
		val3[i] = (char) (random() % 256);
	}
	putData(task1->dataSpace, "teste3", val3, valSz);

	val4 = getData(task1->dataSpace, "teste1", &valSz1);
	if(valSz1 != valSz) {
		printf("problema no valSz1");
	}
	if(memcmp(val1, val4, valSz)) {
		printf("val1 com problema\n");
	}

	val6 = getData(task1->dataSpace, "teste3", &valSz1);
	if(valSz1 != valSz) {
		printf("problema no valSz1");
	}
	if(memcmp(val3, val6, valSz)) {
		printf("val1 com problema\n");
	}
	
	val5 = getData(task1->dataSpace, "teste2", &valSz1);
	if(valSz1 != valSz) {
		printf("problema no valSz1");
	}
	if(memcmp(val2, val5, valSz)) {
		printf("val1 com problema\n");
	}


	
//////////////////////////////////////////////////////////


	
	char metadata[] = "metadata";

	setTaskEndedTasks(task1, 1);

	setTaskState(task1, 2);

	setTaskMetadata(task1, metadata, strlen(metadata)+1);


	
	for(i = 0; i < valSz1; i++) {
		array1[i] = (char) (random() % 256);
		if(array1[i] < 0){
			array1[i] = 10;
		}
		ret = taskIdListAdd(list1, array1[i]);
		
		if(ret != i) printf("O coutos senta\n");

		ret = taskIdListAdd(list2, array1[i]);
		
		if(ret != i) printf("O coutos senta\n");

	}

	
	setTaskMyDeps(task1, list1);

	setTaskDependsOnMe(task1, list2);
	
	pFile = fopen("saidatask.txt", "w");
	
	writeTask(pFile, task1);
	
	fclose(pFile);

	pFile = fopen("saidatask.txt", "r");

	task2 = (Task *) readTask(pFile);

	fclose(pFile);

////////////////////////////////////////////////////////////////////////////////////

	val4 = getData(task1->dataSpace, "teste1", &valSz);
	val5 = getData(task2->dataSpace, "teste1", &valSz1);
	if(valSz1 != valSz) {
		printf("problema no valSz1");
	}
	if(memcmp(val4, val5, valSz)) {
		printf("val1 com problema\n");
	}

	val4 = getData(task1->dataSpace, "teste2", &valSz);
	val5 = getData(task2->dataSpace, "teste2", &valSz1);
	if(valSz1 != valSz) {
		printf("problema no valSz1");
	}
	if(memcmp(val4, val5, valSz)) {
		printf("val1 com problema\n");
	}

	val4 = getData(task1->dataSpace, "teste3", &valSz);
	val5 = getData(task2->dataSpace, "teste3", &valSz1);
	if(valSz1 != valSz) {
		printf("problema no valSz1");
	}
	if(memcmp(val4, val5, valSz)) {
		printf("val1 com problema\n");
	}

	
////////////////////////////////////////////////////////////////////////////////////

	
	
	result = compareTasks(task1, task2);

	if(result) printf("O coutos senta.. oouuuu\n");

	if (!result) printf("task1 is equal to task2\n");



	// Test serialization of tasks with children
	task1 = createTask();

	int metaSize = random() % 10000;
	char *metaData = malloc(metaSize);
	for (i=0; i<metaSize; i++) {
		metaData[i] = random();
	}
	setTaskMetadata(task1, metaData, metaSize);

	int depSize = random() % 10000;
	TaskIdList *deps = taskIdListCreate(depSize);
	for (i=0; i<depSize; i++) {
		taskIdListAdd(deps, random());
	}
	setTaskMyDeps(task1, deps);

	// create some children tasks
	for (i=0; i<200; i++) {
		Task *child = createTask();
	
		metaSize = random() % 10000;
		metaData = malloc(metaSize);
		for (j=0; j<metaSize; j++) {
			metaData[j] = random();
		}
		setTaskMetadata(child, metaData, metaSize);
	
		depSize = random() % 10000;
		deps = taskIdListCreate(depSize);
		for (j=0; j<depSize; j++) {
			taskIdListAdd(deps, random());
		}
		setTaskMyDeps(child, deps);		
		
		taskAddChild(task1, random(), child);
	}

	pFile = fopen("/tmp/saidatask.txt", "w");	
	writeTask(pFile, task1);
	fclose(pFile);

	pFile = fopen("/tmp/saidatask.txt", "r");
	task2 = (Task *) readTask(pFile);
	fclose(pFile);

	result = compareTasks(task1, task2);
	assert(0 == result);

	
	return 0;
}
