#define _CACHE_C_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <pvm3.h>
#include <assert.h>
#include "Cache.h"
#include "constants.h"
#include "DataSpace.h"
#include "Task.h"
#include "prod_cons.h"

#define KEY_INT
#define VAL_VOID
#include "hash.h"
#undef VAL_VOID
#undef KEY_INT


#define TASK_FILENAME_SIZE 100
#define TASK_DIRNAME_SIZE ((1000+TASK_FILENAME_SIZE)*sizeof(char))


Cache *cache = NULL;


/// \todo move this to oher place. cser?
int createDir(char *dirName) {
	int nameSize = strlen(dirName);
	char *token  = dirName;
	char *aux    = malloc(nameSize+1); // in the worst case aux size will be equal nameSize, plus the '\0'
	aux[0] = '\0';	

	while (token < dirName+nameSize) {
		int tokenSize = strcspn(token, "/");
		if (tokenSize > 0) {
			strncat(aux, "/", 1);
			strncat(aux, token, tokenSize);
			
			int status = mkdir(aux, 0777);
			// if we get a error in directory creation and this error isn't directory 
			// already exists, exit
			if ((status==-1) && (errno!=EEXIST)) {
				char *msg = malloc(TASK_DIRNAME_SIZE+1); // +1 to the \0
				msg[TASK_DIRNAME_SIZE] = '\0';
				snprintf(msg, TASK_DIRNAME_SIZE, "Error creating tasks dir %s", aux);
				perror(msg);
				free(msg);
				
				return -1;
			}			
		}
		
		token += tokenSize+1;
	}
	free(aux);	

	return 0;	
}

void *writerThread(void *arg) {
	
	// creates the finished tasks dir
	int status = createDir(cache->finishedTasksDir);
	if (status == -1) exit(1);

	// get where we will put the instance temporary directory
	char *prefix = getenv("VOID_CHECKPOINT_DIR");
	if (prefix == NULL) prefix = strdup("/tmp");

	// Creates the instance temporary directory. Because we called createDir(cache->finishedTasksDir)
	// and the finished tasks dir contains <prefix>/void.<user id>/ , the call 
	// to mkdtemp() should be successful.
	char *tmpTasksDir = (char *)malloc((TASK_DIRNAME_SIZE+1)*sizeof(char)); // +1 to the \0
	tmpTasksDir[TASK_DIRNAME_SIZE] = '\0';
  	snprintf(tmpTasksDir, TASK_DIRNAME_SIZE, "%s/void.%d/tmpXXXXXX", prefix, geteuid());	
	char *success = mkdtemp(tmpTasksDir);
	if (success == NULL) {
		char *msg = malloc(TASK_DIRNAME_SIZE+1);
		msg[TASK_DIRNAME_SIZE] = '\0';
		snprintf(msg, TASK_DIRNAME_SIZE, "Error opening tasks dir %s", tmpTasksDir);
		perror(msg);
		free(msg);
		
		exit(1);
	}

	while (1) {
		// get task from the taskBuffer	
		FinishedTask *ft = (FinishedTask *)get(cache->taskBuffer);
		if (ft == NULL) {
  			fprintf(stderr, "WARNING: WritherThread recieved NULL pointer from taskBuffer!!!\n");
  			fprintf(stderr, "If you are inside GDB it's OK. Otherwise there is a BUG!!!\n");
  			continue;
  		}

		/// \todo make cache capable of saving/recovering multiple works

		// Verify exit		
		if (ft->taskId == -1) {
			printf("taskId == -1, exiting\n");
			free(ft);
			break;
		}
		
		// add task im terminatedTasks
		// cacheEndTask() already do that
		//taskIdListAdd(cache->terminatedTasks, ft->taskId);

		// creates the task file name, full temporary task path and full finished task path		
  		char taskFileName[TASK_FILENAME_SIZE+1]; // +1 to the \0
		taskFileName[TASK_FILENAME_SIZE] = '\0';
  		snprintf(taskFileName, TASK_FILENAME_SIZE, "/%d", ft->taskId);
  		
  		char *tmpTaskName = malloc(TASK_DIRNAME_SIZE+1); // +1 to the \0
		tmpTaskName[TASK_DIRNAME_SIZE] = '\0';
  		strncpy(tmpTaskName, tmpTasksDir, TASK_DIRNAME_SIZE);
  		strncat(tmpTaskName, taskFileName, TASK_FILENAME_SIZE - strlen(tmpTaskName));

  		char *finishedTaskName = malloc(TASK_DIRNAME_SIZE+1);
		finishedTaskName[TASK_DIRNAME_SIZE] = '\0';
  		strncpy(finishedTaskName, cache->finishedTasksDir, TASK_DIRNAME_SIZE);
  		strncat(finishedTaskName, taskFileName, TASK_FILENAME_SIZE - strlen(finishedTaskName));
  		
  		// opens task`s temporary file
  		FILE *serializeFile = fopen(tmpTaskName, "w");
  		if (serializeFile == NULL) {
  			char *msg = malloc(TASK_DIRNAME_SIZE+1);
			msg[TASK_DIRNAME_SIZE] = '\0';
  			snprintf(msg, TASK_DIRNAME_SIZE, "Error opening serialize file %s", tmpTaskName);
  			perror(msg);
  			free(msg);
  			
  			exit(1);
  		}
  		
  		// serialize task
  		// WARNING: concurrent task access
  		writeTask(serializeFile, ft->task);
  		
  		// close file
  		free(ft);
  		fclose(serializeFile);
  		
		// moves file to the directory of finished tasks
		rename(tmpTaskName, finishedTaskName);
		
		// free everything
		free(tmpTaskName);
		free(finishedTaskName);
	}
	
	status = rmdir(tmpTasksDir);
	if (status < 0) {
		perror("WARNING: Error removing temporary tasks dir");	
	}
	
	free(tmpTasksDir);
	pthread_exit(NULL);
	return NULL;	
}

// Coutinho: I separated the initialization of the cache and the creation of the
//	writer thread to make testing of the writer thread easier
void initCacheStruct(int filterId, int instanceId) {

	cache = (Cache *) malloc(sizeof(Cache));
	cache->IAmTaskCreator  = 0;
	cache->useTasks        = 0;
	cache->forwardTaskMsgs = 1; // by default we forward task messages, we only disable this if manager say that we could
	cache->tasks           = hashDSIVoidCreate(INITIAL_CAPACITY);
	cache->terminatedTasks = taskIdListCreate(ID_LIST_INIT_SIZE);
	cache->runningTasks    = hashIntIntCreate(1);
	cache->currentWork     = -1;
	cache->currentTask     = -1;
	cache->taskBuffer      = create_prod_cons(PROD_CONS_SIZE);
	cache->recoverCallback = &defaultRecoveryCallback;

	// get where we will put the checkpoints	
	/// \todo remove garbage files from previous executions
	// OBS: several executions, filters, instances can be using in the same filesystem
	char *prefix = getenv("VOID_CHECKPOINT_DIR");
	if (prefix == NULL) prefix = strdup("/tmp");

	// User id. We use the efective user id to a setuid filter don't make a
	// common user's directory owned by root.
	int uid = geteuid();
	
	// Execution. As we assume that the manager dosn't dies, it's tid is
	// constant in the whole execution.	
	int parentId = pvm_parent();	

	cache->finishedTasksDir = malloc(TASK_DIRNAME_SIZE+1);
	cache->finishedTasksDir[TASK_DIRNAME_SIZE] = '\0';
	snprintf(cache->finishedTasksDir, TASK_DIRNAME_SIZE, "%s/void.%d/t%x/%d/%d", prefix, uid, parentId, filterId, instanceId);
}


void initCache(int filterId, int instanceId) {
	initCacheStruct(filterId, instanceId);

#ifdef VOID_FT			
	// create the writer thread
	pthread_create(&(cache->writeThreadDescriptor), NULL, &writerThread, NULL);
#endif
}

void destroyCache(){

#ifdef VOID_FT	
	// insert a task with taskId == -1 to break the loop of the writer thread
	FinishedTask *bufPos = malloc(sizeof(FinishedTask));
	bufPos->workId = -1;
	bufPos->taskId = -1;
	bufPos->task = NULL;
	put(cache->taskBuffer, bufPos);	
	
	// wait writer thread finish
	pthread_join(cache->writeThreadDescriptor, NULL);
#endif
	
	hashDSIVoidDestroy(cache->tasks);
	taskIdListDestroy(cache->terminatedTasks);
	hashIntIntDestroy(cache->runningTasks);
	destroy_prod_cons(cache->taskBuffer);
	free(cache->finishedTasksDir);
	free(cache);
}

int cachePutData(char *key, void * data, int dataSz) {
	PosHandlerDSIVoid pos = NULL;
	DataSpace *dataSpace = NULL;
	Task *task = NULL;
	DataSpaceId *dataSpaceId = malloc(sizeof(DataSpaceId));

	if(cache->tasks == NULL) {
		cache->tasks = hashDSIVoidCreate(INITIAL_CAPACITY);
	}
	
	dataSpaceId->work = cache->currentWork;
	dataSpaceId->task = cache->currentTask;
	pos = hashDSIVoidGet(cache->tasks, *dataSpaceId);
	if (pos == NULL) {
		return E_NO_SUCH_TASK;
	}
	free(dataSpaceId);

	task = posGetValue(pos);
	if(task == NULL) {
		task = createTask();
		posSetValue(pos, task);
	}

	dataSpace = getTaskDataSpace(task);
	if(dataSpace == NULL) {
		dataSpace = createDataSpace();
	}

	return putData(dataSpace, key, data, dataSz);
}

void *cacheGetData(int taskId, char *key, int *dataSz) {
	PosHandlerDSIVoid pos = NULL;
	DataSpace *dataSpace = NULL;
	DataSpaceId *dataSpaceId = malloc(sizeof(DataSpaceId));
	Task *task = NULL;
	void *val = NULL;

	*dataSz = -1;	
	// Coutinho: on recovey could be no running tasks and
	// the aplication could need read data from finished tasks
	//if (cacheGetCurrentTask() < 0) {
	//	return NULL;
	//}
	
	dataSpaceId->work = cache->currentWork;
	dataSpaceId->task = taskId;
	pos = hashDSIVoidGet(cache->tasks, *dataSpaceId);	
	if(pos == NULL) {
		printf("cacheGetData Error: work=%d and taskId=%d dont have saved data\n", cache->currentWork, taskId);
		return NULL;
	}
	free(dataSpaceId);

	task = posGetValue(pos);
	if (taskId != cache->currentTask) {
		// To get data of other tasks, they must be finished
		if (finished != getTaskState(task)) {
			return NULL;	
		}
	}
	
	dataSpace = getTaskDataSpace(task);

	if(dataSpace == NULL) {
		printf("cacheGetData Error: data space contains no data\n");
		return NULL;
	}

	val = getData(dataSpace, key, dataSz);	
	return val;
}

int cacheRemoveData(char *key) {
	PosHandlerDSIVoid pos = NULL;
	DataSpace *dataSpace = NULL;
	DataSpaceId *dataSpaceId = malloc(sizeof(DataSpaceId));
	Task *task = NULL;
	
	dataSpaceId->work = cache->currentWork;
	dataSpaceId->task = cache->currentTask;
	pos = hashDSIVoidGet(cache->tasks, *dataSpaceId);
	
	if(pos == NULL) {
		printf("cacheGetData Error: work=%d and taskId=%d dont have saved data\n", cache->currentWork, cache->currentTask);
		return -1;
	}
	free(dataSpaceId);

	task = posGetValue(pos);
	dataSpace = getTaskDataSpace(task);
	if(dataSpace == NULL) {
		printf("cacheGetData Error: data space contains no data\n");
		return -1;
	}

	return removeData (dataSpace, key);
}

int *cacheGetTaskDeps(int taskId, int *depsSz) {
	DataSpaceId *dataSpaceId = malloc(sizeof(DataSpaceId));

	*depsSz = -1;	
	/// \todo Define access verifications for this function
	if (cacheGetCurrentTask() < 0) {
		return NULL;
	}
	
	dataSpaceId->work = cache->currentWork;
	dataSpaceId->task = taskId;
	PosHandlerDSIVoid pos = hashDSIVoidGet(cache->tasks, *dataSpaceId);	
	if(pos == NULL) {
		printf("cacheGetData Error: work=%d and taskId=%d dont have saved data\n", cache->currentWork, taskId);
		return NULL;
	}
	free(dataSpaceId);

	Task *task = posGetValue(pos);
	TaskIdList *deps = getTaskMyDeps(task);
	if (deps == NULL) {
		*depsSz = 0;
		return NULL;
	}
	
	int listSz = taskIdListGetSize(deps);
	*depsSz = listSz;
	if (listSz == 0) return NULL;
	
	int *list = malloc(sizeof(int)*listSz);
	int i=0;
	for (i=0; i<listSz; i++)
		list[i] = taskIdListGet(deps, i);
	
	
	return list;
}


TaskIdList *getFinishedTasks() {
	TaskIdList *terminatedTasks = taskIdListCreate(0);
	DIR *finishedTasksDirDD = opendir(cache->finishedTasksDir);

	if (finishedTasksDirDD != NULL) {
		struct dirent *finishedTasksDirEntry = readdir(finishedTasksDirDD);	
		while (finishedTasksDirEntry != NULL) {
			char point[] = ".";
			char pointPoint[] = "..";
			
			char *fileName = strdup(finishedTasksDirEntry->d_name);	
			if ((strcmp(fileName, point) != 0) && (strcmp(fileName, pointPoint) != 0)) {
				int taskId = atoi(fileName);
				taskIdListAdd(terminatedTasks, taskId);	
				printf("Read task: %s\n", fileName);
			}
				
			free(fileName);	
			finishedTasksDirEntry = readdir(finishedTasksDirDD);	
		}
		
		closedir(finishedTasksDirDD);
	}
	
	return terminatedTasks;
}

int cacheRecoverTasks(TaskIdList *tasks) {
	int taskListLen = taskIdListGetSize(tasks);
	int i=0;
	HashIntVoid *tasksToBeCreated = hashIntVoidCreate(10);

	/// \todo make cache capable of saving/recovering multiple works	

	// Recover tasks in ascendig order
	taskIdListSortAscendig(tasks);

	for (i=0; i<taskListLen; i++) {
		int taskId = taskIdListGet(tasks, i);

		// creates the task file name, and full finished task path		
		char taskFileName[TASK_FILENAME_SIZE+1];
		taskFileName[TASK_FILENAME_SIZE] = '\0';
		snprintf(taskFileName, TASK_FILENAME_SIZE, "/%d", taskId);
		
		char *finishedTaskName = malloc(TASK_DIRNAME_SIZE+1); // +1 to the \0
		finishedTaskName[TASK_DIRNAME_SIZE] = '\0';
		strncpy(finishedTaskName, cache->finishedTasksDir, TASK_DIRNAME_SIZE);
		strncat(finishedTaskName, taskFileName, TASK_FILENAME_SIZE - strlen(finishedTaskName));
		
		// opens task`s checkpoint file
		FILE *serializeFile = fopen(finishedTaskName, "r");
		if (serializeFile == NULL) {
			char *msg = malloc(TASK_DIRNAME_SIZE+1);
			msg[TASK_DIRNAME_SIZE] = '\0';
			snprintf(msg, TASK_DIRNAME_SIZE, "Error opening serialize file %s", finishedTaskName);
			perror(msg);
			free(msg);
			
			return E_COULD_NOT_RECOVER_TASK;
		}
	  	Task *recoveredTask = readTask(serializeFile);
	  	fclose(serializeFile);
  	  	free(finishedTaskName);
  	  	assert(recoveredTask->id == taskId);
#ifdef DEBUG
		printf("Recovered task %d\n", recoveredTask->id);
#endif

		// Se tarefa esta no hash de tarefas a serem criadas, tira ela de la
		PosHandlerIntVoid pos = hashIntVoidGet(tasksToBeCreated, taskId);
		if (pos != NULL) {
			Task *motherReference = posGetValue(pos);
			
			// Mother points to motherReference, so we make it
			// points to recovered task. taskAddChild() make
			// recoveredTask points to mother too.
			taskAddChild(motherReference->mother, recoveredTask->id, recoveredTask);
			
			destroyTask(motherReference);
			hashIntVoidRemove(tasksToBeCreated, taskId);
		}

		// switch the task state to finished
		setTaskState(recoveredTask, finished);

  	  	// Put the recovered task in the tasks hash
		DataSpaceId recoveredTaskId;
		recoveredTaskId.work = cache->currentWork;
		recoveredTaskId.task = taskId;
		PosHandlerDSIVoid recoveredTaskPos = hashDSIVoidAdd(cache->tasks, recoveredTaskId);
		posSetValue(recoveredTaskPos, recoveredTask);

		// insert task in terminatedTasks
		taskIdListAdd(cache->terminatedTasks, taskId);
	


		/// \todo test:
		/// mother inserted, then child
		/// child insered then mother

		/// \todo verify if mother points to the real child

		
		// Add children in the "to be created" hash
		HashIntVoid *children = taskTakeChildren(recoveredTask);

		// p/ cada tarefa filha: 
		// we use a destructor iterator, because we will discard this hash anyway
		HashIntVoidIterator *it = createHashIntVoidIterator(children, 1);  
		for (pos = hashIntVoidIteratorNext(it, children); pos != NULL; pos = hashIntVoidIteratorNext(it, children)) {
			Task *recoveredChild = posGetValue(pos);
			int recoveredChildId = posGetKey(pos);
			assert(recoveredChildId == recoveredChild->id);

			DataSpaceId childId;
			childId.work = recoveredTaskId.work;
			childId.task = posGetKey(pos);
			PosHandlerDSIVoid realChildPos = hashDSIVoidGet(cache->tasks, childId);
			
			// if child already exist
			if (realChildPos != NULL) {
				// Substitute the recovered child by it
				Task *realChild = posGetValue(realChildPos);			
				taskAddChild(recoveredTask, recoveredChildId, realChild);

				destroyTask(recoveredChild);
			} else {
				// put it in the "to be created" hash
				PosHandlerIntVoid childPos = hashIntVoidAdd(tasksToBeCreated, recoveredChildId);
				posSetValue(childPos, recoveredChild);
			}
		}
		hashIntVoidIteratorDestroy(it, children);
		hashIntVoidDestroy(children);
			
	}

	// Put the ids of the tasks to be reexecuted and sort them in ascending order
	PosHandlerIntVoid pos = NULL;
	HashIntVoidIterator *it = createHashIntVoidIterator(tasksToBeCreated, 0);
	TaskIdList *toCreateList = taskIdListCreate(0);
	for (pos = hashIntVoidIteratorNext(it, tasksToBeCreated); pos!= NULL; pos = hashIntVoidIteratorNext(it, tasksToBeCreated)) {
		Task *toBeCreated = posGetValue(pos); 
		taskIdListAdd(toCreateList, toBeCreated->id);
	}
	hashIntVoidIteratorDestroy(it, tasksToBeCreated);
	taskIdListSortAscendig(toCreateList);
	
	// 	P/ cada tarefa que ficou no hassh de tarefas a serem criadas, 
	// instancias chamam cacheCreateTask()
	int listSize = taskIdListGetSize(toCreateList);
	for (i=0; i<listSize; i++) {
		int toCreateId = taskIdListGet(toCreateList, i);
		pos = hashIntVoidGet(tasksToBeCreated, toCreateId);
		Task *toBeCreated = posGetValue(pos); 
		int newTaskId    = posGetKey(pos);
		char *metadata   = getTaskMetadata(toBeCreated); // returns toBeCreated->metadata
		int metasize     = getTaskMetasize(toBeCreated);
		
		int *depList     = NULL;
		TaskIdList *deps = getTaskMyDeps(toBeCreated);
		int depSize = taskIdListGetSize(deps);
		if (depSize > 0) {
			depList = malloc(sizeof(int)*depSize);			
		}
		int j; 
		for (j=0; j<depSize; j++) {
			depList[j] = taskIdListGet(deps, j);
		}

		// change context to the mother task
		Task *mother = toBeCreated->mother;
		cache->currentTask = mother->id;
	
		// If the task arrived here is because it's mother referenced it and 
		// it isn't finished or could not be recovered
		// cacheCreateTask() makes a internal copy of depList and metadata
		//
		// ABORTED ! Now using the more elegant callback solution !
		// 
/*		int ret = cacheCreateTask(newTaskId, depList, depSize, metadata, metasize);
		if (ret != 0) {
			fprintf(stderr, "Faltal error restarting task %d: %d\n", newTaskId, ret);
			// remove corrupt data
			char *cmd = malloc(TASK_DIRNAME_SIZE+1); // +1 to the \0
			cmd[TASK_DIRNAME_SIZE] = '\0';
			snprintf(cmd, TASK_DIRNAME_SIZE, "rm -rf %s \n", cache->finishedTasksDir);
			system(cmd);
			
			// abort recovering and restart everything
			abort();
		} 
*/
		
		// Calling user defined callback:
		if ( cache->recoverCallback == NULL ) {
			fprintf( stderr, "Cache.c: Fatal: Undefined user recover callback function.\n" );
		} else {
			// If callback calls createTask() it will make mother point to the createdTask
			(*(cache->recoverCallback))(newTaskId, depList, depSize, metadata, metasize);
		}
		
		if (depList != NULL) {
			free(depList);
		}
		destroyTask(toBeCreated);
	}
	taskIdListDestroy(toCreateList);
	hashIntVoidDestroy(tasksToBeCreated);
	
	// change context to any running task
	HashIntIntIterator *hit = createHashIntIntIterator(cache->runningTasks, 0);
	PosHandlerIntInt posi = hashIntIntIteratorNext(hit, cache->runningTasks);
	if (posi != NULL) {
		cache->currentTask = posGetKey(posi);
	} else {
		cache->currentTask = -1;	
	}
	hashIntIntIteratorDestroy(hit, cache->runningTasks);
		
	return 0;
}

void setCreator() {
	cache->IAmTaskCreator = 1;
}

int cacheSetCurrentTask(int task) {
	DataSpaceId taskId;
	taskId.work = cache->currentWork;
	taskId.task = task;
	
	PosHandlerDSIVoid taskPos = hashDSIVoidGet(cache->tasks, taskId);	
	if (taskPos != NULL) {
		Task *t = (Task *)posGetValue(taskPos);
		TaskState_t tState = getTaskState(t);
		if (tState == running) {
			cache->currentTask = task;
			return 0;
		} else {
			return E_TASK_NOT_RUNNING;
		}
	}
	return E_NO_SUCH_TASK;
}

int cacheGetCurrentTask() {
	return cache->currentTask;
}

void cacheSetCurrentWork(int work) {
	cache->currentWork = work;		
}

int cacheGetCurrentWork() {
	return cache->currentWork;
}

int cacheRunTask(int taskId) {

	// change the task state to running
	DataSpaceId taskDSI;
	taskDSI.work = cache->currentWork;
	taskDSI.task = taskId;
	PosHandlerDSIVoid taskPos = hashDSIVoidGet(cache->tasks, taskDSI);
	Task *task = (Task *)posGetValue(taskPos);
	assert(task != NULL);
	if (task == NULL) return -1;
	setTaskState(task, running);

	// Poe tarefa na lista de tarefas sendo executadas
	PosHandlerIntInt posi = hashIntIntAdd(cache->runningTasks, taskId);
	posSetValue(posi, taskId);

	// if there isn't any task running, the created task becomes the currentTask
	if (cache->currentTask <0){
		cache->currentTask = taskId;
	}		
		
	return 0;	
}

int cacheCreateTask(int taskId, int *deps, int depSize, char *metadata, int metaSize) {
	int i=0;
	
	/*------------------- test if the task can be created -------------------*/

	// Verify if all dependences are finished
	// Non task creator filters: depSize==0
	for (i=0; i<depSize; i++) {
			DataSpaceId dependenceId;
			dependenceId.work = cache->currentWork;
			dependenceId.task = deps[i];
			PosHandlerDSIVoid dependencePos = hashDSIVoidGet(cache->tasks, dependenceId);
			
			if (dependencePos == NULL) {
				// new task depends on a inexistent task, return error
				return -1;
			}	
			/*if (dependenceTask->state != finished) {
				// new task depends on a non finished task, return error
				return -1;
			}*/
	}
	
	// get the new task position and test if is something in it
	DataSpaceId newTaskId;
	newTaskId.work = cache->currentWork;
	newTaskId.task = taskId;
	PosHandlerDSIVoid newPos = hashDSIVoidAdd(cache->tasks, newTaskId);
	if (posGetValue(newPos) != NULL) return E_TASK_EXISTS;
	
	/*--------------------------- task creation -----------------------------*/
	
	// Create the task object
	Task *newTask = createTask();
	setTaskMetadata(newTask, metadata, metaSize);
	setTaskId(newTask, taskId);

	// Create task dependence list
	TaskIdList *depList = taskIdListCreate(depSize);
	for (i=0; i<depSize; i++) {
		taskIdListAdd(depList, deps[i]);
	}
	setTaskMyDeps(newTask, depList);
	taskIdListDestroy(depList);
	
	// add the new task in the hash position
	posSetValue(newPos, newTask);
	
	DataSpaceId motherId;
	motherId.work = cache->currentWork;
	motherId.task = cache->currentTask;
	PosHandlerDSIVoid motherPos = hashDSIVoidGet(cache->tasks, motherId);
	
	if (motherPos != NULL) {
		// if this task has a mother add it to this mother
		Task *motherTask = (Task *) posGetValue(motherPos);
		taskAddChild(motherTask, taskId, newTask);
	}

	// Add task in the dependences` dependsOnMe lists and count the finished dependences
	int finishedDependences = 0;
	for (i=0; i<depSize; i++) {
			DataSpaceId dependenceId;
			dependenceId.work = cache->currentWork;
			dependenceId.task = deps[i];
			PosHandlerDSIVoid dependencePos = hashDSIVoidGet(cache->tasks, dependenceId);
			Task *dependenceTask = (Task *) posGetValue(dependencePos);
			
			// WARNING: Writing on a termiated task object
			addTaskToDependsOnMe(dependenceTask, taskId);
			
			if (dependenceTask->state == finished) {
				finishedDependences++;
			}
	}
	setTaskEndedTasks(newTask, finishedDependences);

	// If all dependencies are satisfied, run the new task
	if (finishedDependences >= depSize) {
		assert(finishedDependences == depSize);
		cacheRunTask(taskId);
	}
		
	return 0;
}

int cacheEndTask(int taskId) {
	DataSpaceId taskDSI;
	taskDSI.work = cache->currentWork;
	taskDSI.task = taskId;
	PosHandlerDSIVoid taskPos = hashDSIVoidGet(cache->tasks, taskDSI);
	
	if (taskPos == NULL) {
		return E_NO_SUCH_TASK;
	}
	
	Task *task = (Task *)posGetValue(taskPos);
	if (running != getTaskState(task)) {
		return E_TASK_NOT_RUNNING;
	}

	// switch the task state to finished
	setTaskState(task, finished);
	
	// remove task from running tasks
	hashIntIntRemove(cache->runningTasks, taskId);
	
	// insert task in terminatedTasks
	taskIdListAdd(cache->terminatedTasks, taskId);
	
	
	// if the current task is terminated, get another current task
	if (taskId == cache->currentTask) {
		HashIntIntIterator *hit = createHashIntIntIterator(cache->runningTasks, 0);
		PosHandlerIntInt posi = hashIntIntIteratorNext(hit, cache->runningTasks);
		if (posi != NULL) {
			cache->currentTask = posGetKey(posi);
		} else {
			cache->currentTask = -1;	
		}
		hashIntIntIteratorDestroy(hit, cache->runningTasks);
	}
	

	// incrementa endedTasks das tarefas em dependsOnMe
	int i=0;	
	int dependentTasks = taskIdListGetSize(task->dependsOnMe);
	for (i=0; i<dependentTasks; i++) {
		DataSpaceId dependentId;
		dependentId.work = cache->currentWork;
		dependentId.task = taskIdListGet(task->dependsOnMe, i);
		PosHandlerDSIVoid dependentPos = hashDSIVoidGet(cache->tasks, dependentId);
		Task *dependentTask = (Task *) posGetValue(dependentPos);
		
		dependentTask->endedTasks++;
		// caso task->endedTasks == task->depSize, dispare tarefa
		if (dependentTask->endedTasks == taskIdListGetSize(dependentTask->myDeps)) {
			cacheRunTask(dependentId.task);
		}
	}

	
#ifdef VOID_FT
	// insert task in the taskBuffer
	FinishedTask *bufPos = malloc(sizeof(FinishedTask));
	bufPos->workId = cache->currentWork;
	bufPos->taskId = taskId;
	bufPos->task = posGetValue(taskPos);
	put(cache->taskBuffer, (void *)bufPos);
#endif
		
	return 0;
}

int cacheGetRunningTasks( int * numTasks, int ** taskList ){
	*numTasks = hashGetChaves(cache->runningTasks);
	*taskList = malloc( sizeof(int) * (*numTasks));
	HashIntIntIterator *itii = createHashIntIntIterator(cache->runningTasks, 0);
	PosHandlerIntInt poshdr = NULL;
	int counter = 0;

	while( (( poshdr = hashIntIntIteratorNext( itii, cache->runningTasks) ) != NULL ) && counter < ( *numTasks )) {
		assert(poshdr != NULL); 
		(*taskList)[counter++] = posGetKey( poshdr );
	}
	hashIntIntIteratorDestroy( itii, cache->runningTasks );
	
	assert(counter == *numTasks);
	return 0;
}

int defaultRecoveryCallback(int newTaskId,int *newTaskDep,int depSize,char *metadata,int metaSize){
	dsCreateTask(newTaskId, newTaskDep, depSize, metadata, metaSize);
	return 0;
}

int cacheRegisterRecoverCallback( RecoverCallback_t * callback ) {
	cache->recoverCallback = callback;
	return(0);
}

void cacheSetUseTasks() {
	cache->useTasks = 1;
}

int cacheGetUseTasks() {
	return cache->useTasks;
}

void cacheSetForwardTaskMsgs(int forward) {
	cache->forwardTaskMsgs = forward;
}

int cacheGetForwardTaskMsgs() {
	return cache->forwardTaskMsgs;
}

int *cacheGetFinishedTasks(int *numTasks) {
	return taskIdListToArray(cache->terminatedTasks, numTasks);
}

/// This function is for internel testing only
Cache *___getCache() {
	return cache;
}

