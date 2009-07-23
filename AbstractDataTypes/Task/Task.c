#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pvm3.h>
#include <assert.h>
#include "Task.h"
/*#include "Messages.h"
#include "Ports.h"
#include "FilterData.h"*/

pthread_mutex_t dependsOnMeMutex = PTHREAD_MUTEX_INITIALIZER;


/////////////////// SETS ////////////////////////
Task *createTask(){
	Task *task = (Task *) malloc(sizeof(Task));
	if(task == NULL){
		printf("createTask: Could not alocate memory\n");
		exit(1);
	}
	task->dataSpace = createDataSpace();
	task->dependsOnMe = NULL;
	task->state = created;
	task->mother = NULL;
	task->endedTasks = 0;
	task->metaSize = 0;
	task->metadata = NULL;

	task->children = hashIntVoidCreate(0);
	setIntVoidSerializer(task->children, &writeChildTask, &readChildTask);
	
	return task;
}

void destroyTask(Task *task){
	if(task == NULL){
		printf("destroyTask: We cant destroy a null Task\n");
		exit(1);
	}

	taskIdListDestroy(task->myDeps);
	taskIdListDestroy(task->dependsOnMe);
	if (task->children != NULL) {
		hashIntVoidDestroy(task->children);
	}

	if(task->metadata != NULL){
		free(task->metadata);		
	}
	free(task);
}

void setTaskEndedTasks(Task *task, int endedTasks){
	task->endedTasks = endedTasks;
}

void setTaskState(Task *task, TaskState_t state){
	task->state = state;
}

void setTaskMyDeps(Task *task, TaskIdList *myDeps){
	task->myDeps = taskIdListCopy(myDeps);
}

void setTaskMetadata(Task *task, const char *metadata, const int metaSize){
	task->metaSize = metaSize;
	
	if (metaSize == 0) {
		task->metadata = NULL;
	} else {
		task->metadata = malloc(sizeof(char) * metaSize);
		memcpy(task->metadata, metadata, metaSize);
	}
}

void setTaskDataSpace(Task *task, DataSpace *dataSpace){
	task->dataSpace = dataSpace;
}

void setTaskDependsOnMe(Task *task, TaskIdList *dependsOnMe){
	pthread_mutex_lock(&dependsOnMeMutex);	
	
	if (dependsOnMe == NULL) {
		task->dependsOnMe = NULL;
	} else {
		task->dependsOnMe = taskIdListCopy(dependsOnMe);
	}
	
	pthread_mutex_unlock(&dependsOnMeMutex);
}

void addTaskToDependsOnMe(Task *task, int taskId) {
	pthread_mutex_lock(&dependsOnMeMutex);	
	
	if (task->dependsOnMe == NULL) {
		task->dependsOnMe = taskIdListCreate(1);
	}
	taskIdListAdd(task->dependsOnMe, taskId);
	
	pthread_mutex_unlock(&dependsOnMeMutex);
}

void taskAddChild(Task *mother, int childId, Task *child) {
	child->mother = mother;
	PosHandlerIntVoid pos = hashIntVoidAdd(mother->children, childId);
	posSetValue(pos, child);
}

HashIntVoid *taskTakeChildren(Task *task) {
	HashIntVoid *ret = task->children;
	task->children = hashIntVoidCreate(0);

	return ret;
}

void setTaskMother(Task *task, Task *mother) {
	task->mother = mother;	
}

void setTaskId(Task *task, int id) {
	task->id = id;	
}

/// Move all data from task struct src to dest, destroy dest values and src. 
void taskMove(Task *dest, Task *src) {
	if (dest->myDeps != NULL) {
		taskIdListDestroy(dest->myDeps);
	}
	if (dest->dependsOnMe != NULL) {
		taskIdListDestroy(dest->dependsOnMe);	
	}
	if (dest->children != NULL) {
		hashIntVoidDestroy(dest->children);	
	} 
	if (dest->metadata != NULL) {
		free(dest->metadata);	
	}
	if (dest->dataSpace != NULL) {
		destroyDataSpace(dest->dataSpace);
	}
	
	dest->myDeps      = src->myDeps;
	dest->dependsOnMe = src->dependsOnMe; 
	dest->children    = src->children; 
	dest->id		  = src->id;
	dest->mother      = src->mother;
	dest->endedTasks  = src->endedTasks;
	dest->metadata    = src->metadata;
	dest->metaSize    = src->metaSize;
	dest->state       = src->state;
	dest->dataSpace   = src->dataSpace;
	
	free(src);
}


/*----------------------------- GETS -------------------------------*/
int getTaskEndedTasks(Task *task){
	if(task == NULL){
		fprintf(stderr, "getTaskEndedTasks with NULL parameter\n");
		exit(1);
	}
	return task->endedTasks;
}

int getTaskMetasize(Task *task){
	if(task == NULL){
		fprintf(stderr, "getTaskMetasize with NULL parameter\n");
		exit(1);
	}
	return task->metaSize;
}

TaskState_t getTaskState(Task *task){
	if(task == NULL){
		fprintf(stderr, "getTaskState with NULL parameter\n");
		exit(1);
	}
	return task->state;
}

char *getTaskMetadata(Task *task){
#ifdef DEBUG
	if(task->metadata == NULL){
		fprintf(stderr, "warning - getTaskMetadata: task->metadata == NULL\n");
	}
#endif
	return task->metadata;
}

TaskIdList *getTaskMyDeps(Task *task){
#ifdef DEBUG
	if(task->myDeps == NULL){
		fprintf(stderr, "warning - getTaskMydeps task->myDeps == NULL\n");
	}
#endif
	return task->myDeps;
}

TaskIdList *getTaskDependsOnMe(Task *task){
#ifdef DEBUG
	if(task->dependsOnMe == NULL){
		fprintf(stderr, "warning - getTaskdepesOne task->dependsOnMe == NULL\n");
	}
#endif
	return task->dependsOnMe;
}

HashIntVoid *getTaskChildren(Task *task) {
	return task->children;	
}

DataSpace *getTaskDataSpace(Task *task){
	return task->dataSpace;
}

Task *getTaskMother(Task *task){
	return task->mother;	
}

int getTaskId(Task *task) {
	return task->id;	
}

////////////// Funcs to write the Task to File /////////////////

int writeTask(FILE *outputFile, void *task){

	int endedTasks, metaSize, taskState;
	Task *auxTask = (Task *)task;

	pthread_mutex_lock(&dependsOnMeMutex);	
	writeTaskIdList(outputFile, (void *)auxTask->dependsOnMe);
	pthread_mutex_unlock(&dependsOnMeMutex);
	writeTaskIdList(outputFile, auxTask->myDeps);

	HashIntVoid *children = auxTask->children;
	hashIntVoidSerialize(outputFile, children);
	
	int id = getTaskId(auxTask);
	WRITE_NUM(outputFile, "id", id);

	endedTasks = getTaskEndedTasks(auxTask);
	WRITE_NUM(outputFile, "endedTasks", endedTasks);

	metaSize = getTaskMetasize(auxTask);
	WRITE_NUM(outputFile, "metaSize", metaSize);

	WRITE_BYTES(outputFile, getTaskMetadata(auxTask), metaSize);

	taskState = getTaskState(auxTask);
	WRITE_NUM(outputFile, "taskState", taskState);

	writeDataSpace(outputFile, getTaskDataSpace(task));
	return 1;
}

void *readTask(FILE *inputFile){
	Task *task = createTask();
	TaskIdList *list = NULL;
	int endedTasks=-1, metaSize=-1, taskState=-1, id=-1;
	char *metadata = NULL;

	READ_BEGIN(inputFile);

	list = (TaskIdList *)readTaskIdList(inputFile);
	setTaskDependsOnMe(task, list);
	taskIdListDestroy(list);

	list = (TaskIdList *)readTaskIdList(inputFile);
	setTaskMyDeps(task, list);
	taskIdListDestroy(list);
	
	hashIntVoidDeserialize(inputFile, task->children);

	// make the children point to its mother
	HashIntVoidIterator *it = createHashIntVoidIterator(task->children, 0);
	PosHandlerIntVoid pos = hashIntVoidIteratorNext(it, task->children);
	while (pos != NULL) {
		Task *child = posGetValue(pos);
		child->mother = task;
		
		pos = hashIntVoidIteratorNext(it, task->children);
	}
	hashIntVoidIteratorDestroy(it, task->children);

	READ_NUM("id", id);
	setTaskId(task, id);

	READ_NUM("endedTasks", endedTasks);
	setTaskEndedTasks(task, endedTasks);

	READ_NUM("metaSize", metaSize);
	if (metaSize > 0) {
		metadata = malloc(metaSize);
	} else {
		metadata = NULL;	
	}
	READ_BYTES(inputFile, metadata, metaSize);
	setTaskMetadata(task, metadata, metaSize);
	if(metadata != NULL){
		free(metadata);
	}

	READ_NUM("taskState", taskState);
	setTaskState(task, taskState);
	
	DataSpace* dataSpace = createDataSpace();	
	readDataSpace(inputFile, dataSpace);

	setTaskDataSpace(task, dataSpace);
	
	READ_END
	return (void *)task;
}



/// writeChildTask() writes only the necessary to 
int writeChildTask(FILE *outputFile, void *task){

	int metaSize = 0;
	Task *auxTask = (Task *)task;

	pthread_mutex_lock(&dependsOnMeMutex);	
	writeTaskIdList(outputFile, (void *)auxTask->dependsOnMe);
	pthread_mutex_unlock(&dependsOnMeMutex);
	writeTaskIdList(outputFile, auxTask->myDeps);

	int id = getTaskId(auxTask);
	WRITE_NUM(outputFile, "id", id);

	metaSize = getTaskMetasize(auxTask);
	WRITE_NUM(outputFile, "metaSize", metaSize);

	WRITE_BYTES(outputFile, getTaskMetadata(auxTask), metaSize);

	return 1;
}

void *readChildTask(FILE *inputFile){
	Task *task = createTask();
	TaskIdList *list = NULL;
	int metaSize = 0;
	char *metadata = NULL;
	int id = -1;

	READ_BEGIN(inputFile);

	list = (TaskIdList *)readTaskIdList(inputFile);
	setTaskDependsOnMe(task, list);
	taskIdListDestroy(list);

	list = (TaskIdList *)readTaskIdList(inputFile);
	setTaskMyDeps(task, list);
	taskIdListDestroy(list);

	READ_NUM("id", id);
	setTaskId(task, id);
	assert(id != -1);

	READ_NUM("metaSize", metaSize);
	if (metaSize > 0) {
		metadata = malloc(metaSize);
	} else {
		metadata = NULL;	
	}
	READ_BYTES(inputFile, metadata, metaSize);
	setTaskMetadata(task, metadata, metaSize);
	if(metadata != NULL){
		free(metadata);
	}
		
	READ_END
	return (void *)task;
}




/// if ret == 0 task1 is equal to task2 else no
int compareTasks(Task *task1, Task *task2){
	int ret = 0;

	ret = taskIdListCompare(task1->myDeps, task2->myDeps);
	if (ret != 0) return 1;

	
	if (task1->dependsOnMe == NULL) {
		if (task2->dependsOnMe != NULL) {
			if (0 != taskIdListGetSize(task2->dependsOnMe)) {
				return -1;
			}
		}
	} else {
		if (task2->dependsOnMe == NULL) {
			if (0 != taskIdListGetSize(task1->dependsOnMe)) {
				return 1;
			}
		} else {
			ret = taskIdListCompare(task1->dependsOnMe, task2->dependsOnMe);
			if (ret != 0) return ret;
		}
	}

	if (task1->id != task2->id) {
		return 1;
	}

	if(task1->endedTasks != task2->endedTasks || task1->metaSize != task2->metaSize || task1->state != task2->state){
		return 1;
	}

	if(memcmp(task1->metadata, task2->metadata, task1->metaSize)) return 1;


	// Vereify if task2->children has 
	HashIntVoidIterator *it1 = createHashIntVoidIterator(task1->children, 0);
	PosHandlerIntVoid pos1 = hashIntVoidIteratorNext(it1, task1->children);
	while (pos1!=NULL) {
		Task *child1 = posGetValue(pos1);
		PosHandlerIntVoid pos2 = hashIntVoidGet(task2->children, posGetKey(pos1));
		Task *child2 = posGetValue(pos2);
		
		ret = compareTasks(child1, child2);
		if (ret != 0) return ret;
		
		pos1 = hashIntVoidIteratorNext(it1, task1->children);
	}
	
	HashIntVoidIterator *it2 = createHashIntVoidIterator(task2->children, 0);
	PosHandlerIntVoid pos2 = hashIntVoidIteratorNext(it2, task2->children);
	while (pos2!=NULL) {
		Task *child2 = posGetValue(pos2);
		pos1 = hashIntVoidGet(task1->children, posGetKey(pos2));
		Task *child1 = posGetValue(pos1);
		
		ret = compareTasks(child1, child2);
		if (ret != 0) return ret;
		
		pos2 = hashIntVoidIteratorNext(it2, task2->children);
	}
	return 0;
}
