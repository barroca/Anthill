#ifndef DEPLIST_H
#define DEPLIST_H

#include <stdio.h>

/**
 * \file TaskIdList.h Definition of data structures of a task id list.
 */

#ifdef TASK_ID_LIST_SEND
#define WRITE_TIDL(outputFile, list) packTaskIdList(list)
#define READ_TIDL(inputFile) unpackTaskIdList()
#else
#define WRITE_TIDL(outputFile, list) writeTaskIdList(outputFile, list)
#define READ_TIDL(inputFile) readTaskIdList(inputFile)
#endif

/// Task Id List data structure
typedef struct {
	int size; ///< List size
	int capacity; ///< List capacity
	int *vetor; ///< List positions
} TaskIdList;

//int taskIdListGetMinCpacity(int size);
TaskIdList *taskIdListCreate(int cap_inicial);
int taskIdListAdd(TaskIdList *vet, int doc);
int taskIdListGetSize(TaskIdList *vet);
int taskIdListGet(TaskIdList *vet, int pos);
int taskIdListGetLast(TaskIdList *vet);
int *taskIdListToArray(TaskIdList *list, int *listSize);
//void taskIdListSort(TaskIdList * vet, int (*compFunc)(const void *, const void *));
int taskIdListCompare(TaskIdList *list1, TaskIdList *list2);
void taskIdListDestroy(TaskIdList *vet);
TaskIdList *taskIdListIntersection(TaskIdList *a, TaskIdList *b);
TaskIdList *taskIdListCopy(TaskIdList* list);
void taskIdListSortAscendig(TaskIdList *list);

////////////// Funcs to write the TaskIdList to File/////////////////

int writeTaskIdList(FILE *outputFile, void *list);
void *readTaskIdList(FILE *inputFile);

int packTaskIdList(void *list);
void *unpackTaskIdList();

#endif
