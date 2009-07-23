#ifndef _CACHE_H_
#define _CACHE_H_

#include <pthread.h>
#include "constants.h"
#include "prod_cons.h"
#include "TaskIdList.h"
#include "DataSpaceId.h"
#include "Task.h"
#include "FilterDev.h"

#define KEY_DSI
#define VAL_VOID
#include "hash.h"
#undef VAL_VOID
#undef KEY_DSI

#define KEY_INT
#define VAL_INT
#include "hash.h"
#undef VAL_INT
#undef KEY_INT


typedef struct {
		int workId;
		int taskId;
		Task *task;
} FinishedTask;

//typedef int RecoverCallback_t( int taskId, int *deps, int depSize, char *metadata, int metaSize );

typedef struct {
	int IAmTaskCreator;
	int useTasks;
	int forwardTaskMsgs;
	HashDSIVoid *tasks;
	TaskIdList *terminatedTasks;
	HashIntInt *runningTasks;
	int currentWork;
	int currentTask;
	prod_cons_t *taskBuffer;
	pthread_t writeThreadDescriptor;
	char *finishedTasksDir;
	RecoverCallback_t * recoverCallback;
} Cache;


void initCache ();
void  destroyCache ();

void setCreator();
void cacheSetUseTasks();
int cacheGetUseTasks();
void cacheSetForwardTaskMsgs(int forward);
int cacheGetForwardTaskMsgs();

int cacheCreateTask(int taskId, int * deps, int depSize, char * metadata, int metaSize);
int cacheEndTask(int taskId);

int  cachePutData(char *key, void * data, int dataSz);
void *cacheGetData(int taskId, char *key, int *dataSz);
int  cacheRemoveData(char *key);

TaskIdList *getFinishedTasks();
int cacheRecoverTasks(TaskIdList *tasks);
int cacheGetRunningTasks( int * numTasks, int ** taskList );
int *cacheGetFinishedTasks(int *numTasks);

void cacheSetCurrentWork(int work);
int cacheGetCurrentWork();
int cacheSetCurrentTask(int task);
int cacheGetCurrentTask();

int  writeTaskState(FILE * outputFile, Cache * cache);
int  readTaskState(FILE * inputFile);
int *cacheGetTaskDeps(int taskId, int *depsSz);

int defaultRecoveryCallback(int newTaskId,int *newTaskDep,int depSize,char *metadata,int metaSize);
int cacheRegisterRecoverCallback( RecoverCallback_t * callback );


#ifdef _TEST_CACHE_
/// This function is for internel testing only
Cache *___getCache();
#endif


#endif
