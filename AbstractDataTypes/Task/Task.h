#ifndef TASK_H
#define TASK_H

#include <pthread.h>
#include "TaskIdList.h"
//#include "constants.h"
#include "DataSpace.h"
#include "cser.h"


#define KEY_INT
#define VAL_VOID
#include "hash.h"
#undef VAL_VOID
#undef KEY_INT


typedef enum {created, running, finished} TaskState_t;

typedef struct _task_ {
	TaskIdList *myDeps; ///< Task that this task has data dependencies
	TaskIdList *dependsOnMe; ///< Tasks that have data dependencies on this task
	HashIntVoid *children; ///< Tasks that where created by this task. The key is a integer (not a DSI) because a task cannot have children in other works
	int id;					///< The task id
	struct _task_ *mother; ///< The task that created this task
	int endedTasks;
	char *metadata;
	int metaSize;
	TaskState_t state;
	DataSpace *dataSpace;
}Task;

// Threads task struct field access:
// 				Checkpointed Task:			Checkpointed Task Child:
// State:		finished					any (writing coud be delayed)
// Thread:		Worker:		Writer:			Worker		Writer
// mydeps:		read		read			read		read
// dependsOnMe:	sync write	sync read		sync write	sync read
// children:	write1		read			write		-
// id			-			read			write2		read
// mother		-			-				write2 		-
// endedTasks	-			read			write		-
// metadata		read		read			read		read
// metaSize		read		read			read		read
// state		read		read			write		-		
// DataSpace	read		read			write		-

// 1: Before task checkpoint
// 2: Before mother chechpoint
 

/////////////////// SETS ////////////////////////
Task *createTask();

void destroyTask(Task *task);

void setTaskEndedTasks(Task *task, int endedTasks);

void setTaskState(Task *task, TaskState_t state);

void setTaskMetadata(Task *task, const char *metadata, const int metaSize);

void setTaskMyDeps(Task *task, TaskIdList *list);

// MARRETA:::: Esse eh o unico set q nao permite vc dar
// free na estrutura passada.. isso pq o Hash nao implementa
// clone()
void setTaskDataSpace(Task *task, DataSpace *dataSpace);

void setTaskDependsOnMe(Task *task, TaskIdList *list);

void setTaskMother(Task *task, Task *mother);

void setTaskId(Task *task, int id);

void addTaskToDependsOnMe(Task *task, int taskId);

void taskAddChild(Task *mother, int childId, Task *child);

HashIntVoid *taskTakeChildren(Task *task);

void taskMove(Task *dest, Task *src);

/*----------------------------- GETS -------------------------------*/
int getTaskEndedTasks(Task *task);

int getTaskMetasize(Task *task);

TaskState_t getTaskState(Task *task);

char *getTaskMetadata(Task *task);

TaskIdList *getTaskMyDeps(Task *task);

TaskIdList *getTaskDependsOnMe(Task *task);

HashIntVoid *getTaskChildren(Task *task);

DataSpace *getTaskDataSpace(Task *task);

Task *getTaskMother(Task *task);

int getTaskId(Task *task);

////////////// Funcs to write the Task to File/////////////////

int writeTask(FILE *outputFile, void *task);
void *readTask(FILE *inputFile);

int writeChildTask(FILE *outputFile, void *task);
void *readChildTask(FILE *inputFile);

int compareTasks(Task *task1, Task *task2);

#endif
