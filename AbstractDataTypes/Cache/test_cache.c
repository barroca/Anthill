#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef DEBUG
#define DEBUG
#endif

#include <assert.h>

#define _TEST_CACHE_
#include "Cache.h"

#include "FilterData.h"
#include "Task.h"
#include "TaskIdList.h"


// Tests:

// Task creation:
//	current task:
//		with				(5), (9)
//		without (current task = -1) 	(1), (4)
//	test task recreation error checking 	(2)
//	dependencies and metedata:
//		with				(4), (9)
//		without (vector = NULL) 	(1)
//	dependences status:
//		non-existent (return error)	(6), (7)
//		created				(8)
//		running (create)		(5)
//		not all finished (create)	(5)
//		all finished (create and run)	(4), (9)
//
// End task:
//	task isn't current			(9)
//	task is current:
//		are other tasks running		(10)
//		aren't 				(3)
//	task dosn't exists			(12)
//	task is created				(13)
//	task already is finished		(11)
//
// Set current task:
//	inesistent task (error)			(14)
//	task still created (error)		(15)
//	normal operation			(16)
//	task terminated (error)			(17)
//
// Get data:
//	current task status:
//		dosen't exist (error)		(18)
//		running				(19)
//  referenced task status:
//		dosen't exist (error)		(20)
//		created (error)			(21)
//		running (error)			(22)
//		finished			(19)
//
// Remove data:
//	key exists				(23)
//	double removel				(24)
//	key doesn't exists			(25)


// This is only to this test compile, should never be used
FilterData *fd;


void initCacheStruct(int filterId, int instanceId);
void *writerThread(void *arg);

int main(int argc, char *argv[]) {
	int i, j, ret=0;
	srandom(107);
/*--------------------------- writer thread tests ---------------------------*/

	// init the caches struct generate some data and call the writer_thread 
	// function to see it serialize something
	/*initCacheStruct(4, 5);
	cacheSetCurrentWork(0);

	ret = cacheCreateTask(0, NULL, 0, NULL, 0);
	cacheEndTask(0);

	// put a flag telling the writerThread to exit
	prod_cons_t *terminatedTaskBuff = (___getCache())->taskBuffer;
	FinishedTask *bufPos = malloc(sizeof(FinishedTask));
	bufPos->workId = -1;
	bufPos->taskId = -1;
	bufPos->task = NULL;
	put(terminatedTaskBuff, bufPos);	
	

	writerThread(NULL);*/

	
/*---------------------------- task creator tests ---------------------------*/
	// init the cache
	initCache(0, 0);
	setCreator();
	cacheSetCurrentWork(0);
	
	// As there isn't any task running, the current task should be -1
	int currentTask = cacheGetCurrentTask();
	assert(currentTask == -1);	
	
	
	
	// 1) Creates a task and verify if everything is rigth
	ret = cacheCreateTask(0, NULL, 0, NULL, 0);
	assert(ret==0);

	HashDSIVoid *taskHash = (___getCache())->tasks;
	DataSpaceId dsi = {0, 0};
	PosHandlerDSIVoid taskPos = hashDSIVoidGet(taskHash, dsi);
	Task *createdTask = (Task *)posGetValue(taskPos);

	TaskIdList *taskDeps = getTaskMyDeps(createdTask);
	assert(0 == taskIdListGetSize(taskDeps));

	assert(NULL == getTaskDependsOnMe(createdTask));
	assert(0 == getTaskMetasize(createdTask));
	assert(NULL == getTaskMetadata(createdTask));
	assert(running == getTaskState(createdTask));
	
	assert(0 == getTaskEndedTasks(createdTask));

	// get the current task, shoud be the created task
	currentTask = cacheGetCurrentTask();
	assert(currentTask == 0);	
		
	
	
	// 2) Create the same task twice, the cache can't let this happen
	ret = cacheCreateTask(0, NULL, 0, NULL, 0);
	assert(ret==E_TASK_EXISTS);

	// put some data to be used in test 18 and 19
	int labelSz = random()%10000;
	char *label = malloc(sizeof(char)*(labelSz+1));
	for (i=0; i<labelSz; i++) {
		label[i] = random();
	}
	label[labelSz] = '\0';
	int datasize = random()%10000;
	char *data = malloc(sizeof(char)*datasize);
	for (i=0; i<datasize; i++) {
		data[i] = random();
	}
	ret = cachePutData(label, data, datasize);
	assert(ret == 0);

	
	
	// 3) end task 0
	cacheEndTask(0);

	// get the current task, shoud be -1
	currentTask = cacheGetCurrentTask();
	assert(currentTask == -1);	



	// 18) get data form task 0, should return error because current task == -1
	int dataSz = -1;
	void *taskData = cacheGetData(0, label, &dataSz);
	assert(taskData == NULL);
	assert(dataSz < 0);

	

	// 4) now we will create a task with dependencies and metadata
	int metasize = random()%10000;
	char *metadata = malloc(sizeof(char)*metasize);
	for (i=0; i<metasize; i++) {
		metadata[i] = random();
	}

	int depSize=1;
	int *deps = malloc(sizeof(int)*depSize);
	deps[0]=0;
	
	ret = cacheCreateTask(1, deps, depSize, metadata, metasize);
	assert(ret==0);

	dsi.work = 0;
	dsi.task = 1;
	taskPos = hashDSIVoidGet(taskHash, dsi);
	createdTask = (Task *)posGetValue(taskPos);

	taskDeps = getTaskMyDeps(createdTask);
	assert(1 == taskIdListGetSize(taskDeps));
	assert(0 == taskIdListGet(taskDeps, 0));
	
	assert(NULL == getTaskDependsOnMe(createdTask));
	assert(metasize == getTaskMetasize(createdTask));
	char *taskMetadata = getTaskMetadata(createdTask);
	assert(0 == memcmp(metadata, taskMetadata, metasize));

	// TODO: make this on a task with some finished dependencies and some not 	
	assert(1 == getTaskEndedTasks(createdTask));
	
	assert(running == getTaskState(createdTask));
	
	// task must be in the running tasks list
	HashIntInt *runningTasks = (___getCache())->runningTasks;
	PosHandlerIntInt posRunning = hashIntIntGet(runningTasks, 1);
	assert(posRunning != NULL);
	assert(1 == posGetValue(posRunning));
	
	// get the current task, shoud be the created task
	currentTask = cacheGetCurrentTask();
	assert(currentTask == 1);
		
	// the new task must be in the mother's dependsOnMe list
	dsi.work = 0;
	dsi.task = 0;
	taskPos = hashDSIVoidGet(taskHash, dsi);
	
	Task *motherTask = posGetValue(taskPos);
	TaskIdList *motherDependees = getTaskDependsOnMe(motherTask);
	assert(1 == taskIdListGetSize(motherDependees));
	assert(1 == taskIdListGet(motherDependees, 0));



	// 19) get data for task 0, now shoud work
	dataSz = -1;
	taskData = cacheGetData(0, label, &dataSz);
	assert(taskData != NULL);
	assert(dataSz == datasize);
	assert(0 == memcmp(taskData, data, dataSz));



	// 5) now we will create task 2 that depends on task 0 (finished) and task 1 (running)
	metasize = random()%10000;
	metadata = malloc(sizeof(char)*metasize);
	for (i=0; i<metasize; i++) {
		metadata[i] = random();
	}

	depSize=2;
	deps = malloc(sizeof(int)*depSize);
	// TODO test with deps in other orders
	deps[0]=1;
	deps[1]=0;
	
	int taskId = 2;
	ret = cacheCreateTask(taskId, deps, depSize, metadata, metasize);
	assert(ret==0);

	dsi.work = 0;
	dsi.task = taskId;
	taskPos = hashDSIVoidGet(taskHash, dsi);
	createdTask = (Task *)posGetValue(taskPos);

	taskDeps = getTaskMyDeps(createdTask);
	assert(depSize == taskIdListGetSize(taskDeps));
	for (i=0; i<depSize; i++) {
		assert(deps[i] == taskIdListGet(taskDeps, i));
	}
	
	assert(NULL == getTaskDependsOnMe(createdTask));
	assert(metasize == getTaskMetasize(createdTask));
	taskMetadata = getTaskMetadata(createdTask);
	assert(0 == memcmp(metadata, taskMetadata, metasize));

	// TODO: make this on a task with some finished dependencies and some not 	
	assert(1 == getTaskEndedTasks(createdTask));
	
	assert(created == getTaskState(createdTask));
	
	// task must be not in the running tasks list
	posRunning = hashIntIntGet(runningTasks, taskId);
	assert(posRunning == NULL);
	
	// get the current task, shoud be the previous current task
	currentTask = cacheGetCurrentTask();
	assert(currentTask == 1);
		
	// the new task must be in the mothers' dependsOnMe list
	for (i=0; i<depSize; i++) {
		dsi.work = 0;
		dsi.task = deps[i];
		taskPos = hashDSIVoidGet(taskHash, dsi);
	
		Task *motherTask = posGetValue(taskPos);
		TaskIdList *motherDependees = getTaskDependsOnMe(motherTask);
		if (deps[i] == 0) {
			assert(2 == taskIdListGetSize(motherDependees));
		} else {
			assert(1 == taskIdListGetSize(motherDependees));
		}
		
		int found=0;
		for (j=0; j<taskIdListGetSize(motherDependees); j++) {
			if (taskId == taskIdListGet(motherDependees, j)) {
				found=1;
			}
		}
		assert(found);
	}



	// 6) Create a task that depends on itself
	depSize=2;
	deps = malloc(sizeof(int)*depSize);
	deps[0]=3;
	deps[1]=0;
	
	taskId = 3;
	ret = cacheCreateTask(taskId, deps, depSize, metadata, metasize);
	assert(ret==-1);

	dsi.work = 0;
	dsi.task = taskId;
	taskPos = hashDSIVoidGet(taskHash, dsi);
	assert(taskPos == NULL);

	// get the current task, shoud be the previous current task
	currentTask = cacheGetCurrentTask();
	assert(currentTask == 1);
		
		

	// 7) Create a task that has inexistent dependencies 
	depSize=2;
	deps = malloc(sizeof(int)*depSize);
	// TODO test with deps in other orders
	deps[0]=5;
	deps[1]=0;
	
	taskId = 4;
	ret = cacheCreateTask(taskId, deps, depSize, metadata, metasize);
	assert(ret==-1);

	dsi.work = 0;
	dsi.task = taskId;
	taskPos = hashDSIVoidGet(taskHash, dsi);
	assert(taskPos == NULL);

	// get the current task, shoud be the previous current task
	currentTask = cacheGetCurrentTask();
	assert(currentTask == 1);



	// 8) create a task which all dependencies are in the "created" state
	metasize = random()%10000;
	metadata = malloc(sizeof(char)*metasize);
	for (i=0; i<metasize; i++) {
		metadata[i] = random();
	}

	depSize=1;
	deps = malloc(sizeof(int)*depSize);
	deps[0]=2;
	
	taskId = 6;
	ret = cacheCreateTask(taskId, deps, depSize, metadata, metasize);
	assert(ret==0);

	dsi.work = 0;
	dsi.task = taskId;
	taskPos = hashDSIVoidGet(taskHash, dsi);
	createdTask = (Task *)posGetValue(taskPos);

	taskDeps = getTaskMyDeps(createdTask);
	assert(depSize == taskIdListGetSize(taskDeps));
	for (i=0; i<depSize; i++) {
		assert(deps[i] == taskIdListGet(taskDeps, i));
	}
	
	assert(NULL == getTaskDependsOnMe(createdTask));
	assert(metasize == getTaskMetasize(createdTask));
	taskMetadata = getTaskMetadata(createdTask);
	assert(0 == memcmp(metadata, taskMetadata, metasize));

	// TODO: make this on a task with some finished dependencies and some not 	
	assert(0 == getTaskEndedTasks(createdTask));
	
	assert(created == getTaskState(createdTask));
	
	// task must be not in the running tasks list
	posRunning = hashIntIntGet(runningTasks, taskId);
	assert(posRunning == NULL);
	
	// get the current task, shoud be the previous current task
	currentTask = cacheGetCurrentTask();
	assert(currentTask == 1);
		
	// the new task must be in the mothers' dependsOnMe list
	for (i=0; i<depSize; i++) {
		dsi.work = 0;
		dsi.task = deps[i];
		taskPos = hashDSIVoidGet(taskHash, dsi);
	
		Task *motherTask = posGetValue(taskPos);
		TaskIdList *motherDependees = getTaskDependsOnMe(motherTask);
		assert(1 == taskIdListGetSize(motherDependees));
		
		int found=0;
		for (j=0; j<taskIdListGetSize(motherDependees); j++) {
			if (taskId == taskIdListGet(motherDependees, j)) {
				found=1;
			}
		}
		assert(found);
	}



	// 9) create task 7 and end it
	metasize = random()%10000;
	metadata = malloc(sizeof(char)*metasize);
	for (i=0; i<metasize; i++) {
		metadata[i] = random();
	}

	depSize=1;
	deps = malloc(sizeof(int)*depSize);
	deps[0]=0;

	taskId=7;	
	ret = cacheCreateTask(taskId, deps, depSize, metadata, metasize);
	assert(ret==0);

	dsi.work = 0;
	dsi.task = taskId;
	taskPos = hashDSIVoidGet(taskHash, dsi);
	createdTask = (Task *)posGetValue(taskPos);

	taskDeps = getTaskMyDeps(createdTask);
	assert(1 == taskIdListGetSize(taskDeps));
	assert(0 == taskIdListGet(taskDeps, 0));
	
	assert(NULL == getTaskDependsOnMe(createdTask));
	assert(metasize == getTaskMetasize(createdTask));
	taskMetadata = getTaskMetadata(createdTask);
	assert(0 == memcmp(metadata, taskMetadata, metasize));

	// TODO: make this on a task with some finished dependencies and some not 	
	assert(1 == getTaskEndedTasks(createdTask));
	
	assert(running == getTaskState(createdTask));
	
	// task must be in the running tasks list
	runningTasks = (___getCache())->runningTasks;
	posRunning = hashIntIntGet(runningTasks, taskId);
	assert(posRunning != NULL);
	assert(taskId == posGetValue(posRunning));
	
	// get the current task, shoud be task 1
	currentTask = cacheGetCurrentTask();
	assert(currentTask == 1);
		
	// the new task must be in the mother's dependsOnMe list
	for (i=0; i<depSize; i++) {
		dsi.work = 0;
		dsi.task = deps[i];
		taskPos = hashDSIVoidGet(taskHash, dsi);
	
		Task *motherTask = posGetValue(taskPos);
		TaskIdList *motherDependees = getTaskDependsOnMe(motherTask);
		assert(3 == taskIdListGetSize(motherDependees));
		
		int found=0;
		for (j=0; j<taskIdListGetSize(motherDependees); j++) {
			if (taskId == taskIdListGet(motherDependees, j)) {
				found=1;
			}
		}
		assert(found);
	}
	
	
	// end task 7	
	ret = cacheEndTask(taskId);
	assert(0 == ret);
	
	// get the current task. As task 7 isn't the current task the current task don't will be changed
	currentTask = cacheGetCurrentTask();
	assert(currentTask == 1);
	
	// task must not be in the running tasks list
	runningTasks = (___getCache())->runningTasks;
	posRunning = hashIntIntGet(runningTasks, taskId);
	assert(posRunning == NULL);	
	
	// task must be in the finished tasks list	
	TaskIdList *finishedTasks = (___getCache())->terminatedTasks;
	int found=0;
	for (i=0; i<taskIdListGetSize(finishedTasks); i++) {
		if (taskIdListGet(finishedTasks, i) == taskId) {
			found=1;
		}
	}	
	assert(1 == found);
		
	// task state must be finished	
	dsi.work = 0;
	dsi.task = taskId;
	taskPos = hashDSIVoidGet(taskHash, dsi);
	Task *finishedTask = (Task *)posGetValue(taskPos);
	assert(finished == getTaskState(finishedTask));
	


	// 10) end task 1
	taskId = 1;
	ret = cacheEndTask(taskId);
	assert(0 == ret);
	
	// get the current task. As task 1 isn't the current task the current task don't will be changed
	currentTask = cacheGetCurrentTask();
	printf("Ending current task (1): new current task: %d\n", currentTask);
	assert(currentTask == 2);
	assert(currentTask != -1);
	
	// task must not be in the running tasks list
	runningTasks = (___getCache())->runningTasks;
	posRunning = hashIntIntGet(runningTasks, taskId);
	assert(posRunning == NULL);	
	
	// task must be in the finished tasks list	
	finishedTasks = (___getCache())->terminatedTasks;
	found=0;
	for (i=0; i<taskIdListGetSize(finishedTasks); i++) {
		if (taskIdListGet(finishedTasks, i) == taskId) {
			found=1;
		}
	}	
	assert(1 == found);
		
	// task state must be finished	
	dsi.work = 0;
	dsi.task = taskId;
	taskPos = hashDSIVoidGet(taskHash, dsi);
	finishedTask = (Task *)posGetValue(taskPos);
	assert(finished == getTaskState(finishedTask));
	
	
	// ended tasks in task 2 muste be 2
	taskId = 2;
	dsi.work = 0;
	dsi.task = taskId;
	taskPos = hashDSIVoidGet(taskHash, dsi);
	createdTask = (Task *)posGetValue(taskPos);
	
	assert(2 == getTaskEndedTasks(createdTask));	
	assert(running == getTaskState(createdTask));

	// task must be in running tasks
	posRunning = hashIntIntGet(runningTasks, taskId);
	assert(posRunning != NULL);
	assert(taskId == posGetValue(posRunning));



	// 11) try re-ending task 0
	taskId = 0;
	ret = cacheEndTask(taskId);
	assert(E_TASK_NOT_RUNNING == ret);
	
	// get the current task. As task 1 isn't the current task the current task don't will be changed
	currentTask = cacheGetCurrentTask();
	assert(currentTask == 2);
	
	// task must not be in the running tasks list
	runningTasks = (___getCache())->runningTasks;
	posRunning = hashIntIntGet(runningTasks, taskId);
	assert(posRunning == NULL);	
	
	// task must be in the finished tasks list	
	finishedTasks = (___getCache())->terminatedTasks;
	found=0;
	for (i=0; i<taskIdListGetSize(finishedTasks); i++) {
		if (taskIdListGet(finishedTasks, i) == taskId) {
			found=1;
		}
	}	
	assert(1 == found);
		
	// task state must be finished	
	dsi.work = 0;
	dsi.task = taskId;
	taskPos = hashDSIVoidGet(taskHash, dsi);
	finishedTask = (Task *)posGetValue(taskPos);
	assert(finished == getTaskState(finishedTask));



	// 12) do endTask() on a inexistent task
	taskId = 4;
	ret = cacheEndTask(taskId);
	assert(E_NO_SUCH_TASK == ret);
	
	// get the current task. As task 1 isn't the current task the current task don't will be changed
	currentTask = cacheGetCurrentTask();
	assert(currentTask == 2);
	
	// task must not be in the running tasks list
	runningTasks = (___getCache())->runningTasks;
	posRunning = hashIntIntGet(runningTasks, taskId);
	assert(posRunning == NULL);	
	
	// task must not be in the finished tasks list	
	finishedTasks = (___getCache())->terminatedTasks;
	found=0;
	for (i=0; i<taskIdListGetSize(finishedTasks); i++) {
		if (taskIdListGet(finishedTasks, i) == taskId) {
			found=1;
		}
	}	
	assert(0 == found);
		
	// task state must not exist	
	dsi.work = 0;
	dsi.task = taskId;
	taskPos = hashDSIVoidGet(taskHash, dsi);
	assert(taskPos == NULL);



	// 13) try end a task that still is in the created state
	taskId = 6;
	ret = cacheEndTask(taskId);
	assert(E_TASK_NOT_RUNNING == ret);
	
	// get the current task. As task 1 isn't the current task the current task don't will be changed
	currentTask = cacheGetCurrentTask();
	assert(currentTask == 2);
	
	// task must not be in the running tasks list
	runningTasks = (___getCache())->runningTasks;
	posRunning = hashIntIntGet(runningTasks, taskId);
	assert(posRunning == NULL);	
	
	// task must be not in the finished tasks list	
	finishedTasks = (___getCache())->terminatedTasks;
	found=0;
	for (i=0; i<taskIdListGetSize(finishedTasks); i++) {
		if (taskIdListGet(finishedTasks, i) == taskId) {
			found=1;
		}
	}	
	assert(0 == found);
		
	// task state must be created	
	dsi.work = 0;
	dsi.task = taskId;
	taskPos = hashDSIVoidGet(taskHash, dsi);
	finishedTask = (Task *)posGetValue(taskPos);
	assert(created == getTaskState(finishedTask));
	

	
	// 14) setCurrentTask(): Inexistent task (caugth an error!)
	ret = cacheSetCurrentTask(4);
	assert(ret == E_NO_SUCH_TASK);

	currentTask = cacheGetCurrentTask();
	assert(currentTask == 2);



	// 15) setCurrentTask(): 
	ret = cacheSetCurrentTask(6);
	assert(ret == E_TASK_NOT_RUNNING);

	currentTask = cacheGetCurrentTask();
	assert(currentTask == 2);



	// 16) create task 8 and change to it
	metasize = random()%10000;
	metadata = malloc(sizeof(char)*metasize);
	for (i=0; i<metasize; i++) {
		metadata[i] = random();
	}

	depSize=1;
	deps = malloc(sizeof(int)*depSize);
	deps[0]=0;

	taskId=8;	
	ret = cacheCreateTask(taskId, deps, depSize, metadata, metasize);
	assert(ret==0);

	dsi.work = 0;
	dsi.task = taskId;
	taskPos = hashDSIVoidGet(taskHash, dsi);
	createdTask = (Task *)posGetValue(taskPos);

	taskDeps = getTaskMyDeps(createdTask);
	assert(1 == taskIdListGetSize(taskDeps));
	assert(0 == taskIdListGet(taskDeps, 0));
	
	assert(NULL == getTaskDependsOnMe(createdTask));
	assert(metasize == getTaskMetasize(createdTask));
	taskMetadata = getTaskMetadata(createdTask);
	assert(0 == memcmp(metadata, taskMetadata, metasize));

	// TODO: make this on a task with some finished dependencies and some not 	
	assert(1 == getTaskEndedTasks(createdTask));
	
	assert(running == getTaskState(createdTask));
	
	// task must be in the running tasks list
	runningTasks = (___getCache())->runningTasks;
	posRunning = hashIntIntGet(runningTasks, taskId);
	assert(posRunning != NULL);
	assert(taskId == posGetValue(posRunning));
	
	// get the current task, shoud be task 1
	currentTask = cacheGetCurrentTask();
	assert(currentTask == 2);
		
	// the new task must be in the mother's dependsOnMe list
	for (i=0; i<depSize; i++) {
		dsi.work = 0;
		dsi.task = deps[i];
		taskPos = hashDSIVoidGet(taskHash, dsi);
	
		Task *motherTask = posGetValue(taskPos);
		TaskIdList *motherDependees = getTaskDependsOnMe(motherTask);
		assert(4 == taskIdListGetSize(motherDependees));
		
		int found=0;
		for (j=0; j<taskIdListGetSize(motherDependees); j++) {
			if (taskId == taskIdListGet(motherDependees, j)) {
				found=1;
			}
		}
		assert(found);
	}
	
	// change to it
	ret = cacheSetCurrentTask(taskId);	
	assert(ret == 0);

	// get the current task, must be task 8
	currentTask = cacheGetCurrentTask();
	assert(currentTask == taskId);

		

	// 17) task already finished
	ret = cacheSetCurrentTask(0);
	assert(ret == E_TASK_NOT_RUNNING);

	currentTask = cacheGetCurrentTask();
	assert(currentTask == 8);



	// 20) get data from inexistent task
	dataSz = -1;
	taskData = cacheGetData(4, label, &dataSz);
	assert(taskData == NULL);
	assert(dataSz < 0);



	// 21) get data from a task still in the created state
	dsi.work = 0;
	dsi.task = 6;
	taskPos = hashDSIVoidGet(taskHash, dsi);
	createdTask = (Task *)posGetValue(taskPos);
	
	assert(created == getTaskState(createdTask));
		
	dataSz = -1;
	taskData = cacheGetData(6, label, &dataSz);
	assert(taskData == NULL);
	assert(dataSz < 0);



	// 22) get data from a task still in the running state
	taskId = 8;
	
	dsi.work = 0;
	dsi.task = taskId;
	taskPos = hashDSIVoidGet(taskHash, dsi);
	createdTask = (Task *)posGetValue(taskPos);
	
	assert(running == getTaskState(createdTask));
		
	dataSz = -1;
	taskData = cacheGetData(taskId, label, &dataSz);
	assert(taskData == NULL);
	assert(dataSz < 0);

	

	// 23) create some data to to test data removal
	for (i=0; i<10; i++) {
		// put some data to be used in test 18 and 19
		labelSz = random()%10000;
		label = malloc(sizeof(char)*(labelSz+1));
		for (j=0; j<labelSz; j++) {
			label[j] = random();
		}
		label[labelSz] = '\0';
		datasize = random()%10000;
		data = malloc(sizeof(char)*datasize);
		for (j=0; j<datasize; j++) {
			data[j] = random();
		}
		ret = cachePutData(label, data, datasize);
		assert(ret == 0);
	}

	ret = cacheRemoveData(label);
	assert(ret == 0);

	taskId = cacheGetCurrentTask();
	dsi.work = 0;
	dsi.task = taskId;
	taskPos = hashDSIVoidGet(taskHash, dsi);
	createdTask = (Task *)posGetValue(taskPos);	
	DataSpace *ds = getTaskDataSpace(createdTask);
	HashStrVoid *hs = ds->dataHash;

	PosHandlerStrVoid possv = hashStrVoidGet(hs, label);
	assert(NULL == possv);



	// 24) test removal of non-existent values
	ret = cacheRemoveData(label);
	assert(ret < 0);
	assert(hs->chaves == 9);



	// 25) test removal of non-existent values
	label[0]++;
	ret = cacheRemoveData(label);
	assert(ret < 0);
	assert(hs->chaves == 9);



	// test getFinishedTasks()
	ret = cacheEndTask(2);
	assert(ret == 0);
	ret = cacheEndTask(6);
	assert(ret == 0);
	ret = cacheEndTask(8);
	assert(ret == 0);
	
	printf("enter some data to unblock the main thread:");
	int foo = getc(stdin);
	
	finishedTasks = getFinishedTasks();
	assert(0 < taskIdListGetSize(finishedTasks));
	printf("Finished tasks: ");
	for (i=0; i<taskIdListGetSize(finishedTasks); i++) {
		printf("%d ", taskIdListGet(finishedTasks, i));
	}
	printf("\n");


	destroyCache();
	
	return 0;	
}



