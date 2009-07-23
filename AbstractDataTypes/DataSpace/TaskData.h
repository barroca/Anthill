#ifndef _TASKDATA_H_
#define _TASKDATA_H_

#include <stdio.h>

typedef struct _TaskData_ {
	int valSz;
	void *val;
} TaskData;

TaskData *  createTaskData ();
void  destroyTaskData (TaskData * taskData);
void *readTaskData (FILE *inputFile);
int  writeTaskData (FILE *outputFile, void *taskData);
int getTaskDataSz (TaskData *taskData);
void putTaskDataSz (TaskData *taskData, int valSz);
void *getTaskDataVal (TaskData *taskData);
void putTaskDataVal (TaskData *taskData, void *val, int valSz);

#endif
