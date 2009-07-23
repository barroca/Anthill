#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TaskIdList.h"

#define DEBUG
#include <assert.h>

#define KEY_INT
#define VAL_INT
#include "hash.h"
#undef VAL_INT
#undef KEY_INT


/* Compare 2 task ids (integers)*/
int compareTaskId(const void *a, const void *b)
{
	if (( *(int *)a) < ( *(int *)b)) return -1;
	if (( *(int *)a) > ( *(int *)b)) return  1;
	return 0;
}


int main() {
	TaskIdList *list1 = taskIdListCreate(10);
	TaskIdList *list2;

	int i,j, ret;
	FILE *pFile;

	int valSz1 = 20;
	int val1[100];
	
	for(i = 0; i < valSz1; i++) {
		val1[i] = (char) (random() % 256);
		if(val1[i] < 0){
			val1[i] = 10;
		}
		ret = taskIdListAdd(list1, val1[i]);
		
		if(ret != i) printf("O coutos senta\n");
	}

	pFile = fopen("saidataskData.txt", "w");
	

	writeTaskIdList(pFile, (void *)list1);

	fclose(pFile);

	pFile = fopen("saidataskData.txt", "r");

		list2 = (TaskIdList *) readTaskIdList(pFile);

	for(i = 0; i < valSz1; i++){
		ret = taskIdListGet(list1, i);
		if(val1[i] != ret)printf("erro lendo taskIdList");
	}

	fclose(pFile);



	// Test taskIdListIntersection()
	list1 = taskIdListCreate(0);
	list2 = taskIdListCreate(0);
	TaskIdList *intersection = taskIdListCreate(0);

	taskIdListAdd(list1, 0);
	taskIdListAdd(list2, 0);
	taskIdListAdd(intersection, 0);

	taskIdListAdd(list1, 1);
	
	taskIdListAdd(list2, 2);
	
	TaskIdList *list3 = taskIdListIntersection(list1, list2);
	assert(0 ==	taskIdListCompare(intersection, list3));
	
	srandom(107);
	for (i=0; i<1000; i++) {
		list1 = taskIdListCreate(0);
		list2 = taskIdListCreate(0);
		TaskIdList *intersection = taskIdListCreate(0);
		HashIntInt *hash = hashIntIntCreate(10000);
	
		int list1Sz = random()%10000;
		for (j=0; j<list1Sz; j++) {
			int id = random()%10000;

			PosHandlerIntInt pos = hashIntIntAdd(hash, id);
			if (posGetValue(pos) == -1) {
				posSetValue(pos, id);
				taskIdListAdd(list1, id);
			}
		}
		//assert(list1->size == list1Sz);
		assert(list1->size == hash->chaves);
		
		HashIntInt *hash2 = hashIntIntCreate(10000);	
		int list2Sz = random()%10000;
		for (j=0; j<list2Sz; j++) {
			int id = random()%10000;
			
			PosHandlerIntInt pos = hashIntIntAdd(hash2, id);
			if (posGetValue(pos) == -1) {
				posSetValue(pos, id);
				taskIdListAdd(list2, id);
				
				pos = hashIntIntGet(hash, id);
				if (pos != NULL) {
					taskIdListAdd(intersection, id);
				}
			}
		}
		assert(list2->size == hash2->chaves);
		hashIntIntDestroy(hash2);
		
		//	Para fazer intersecao, we must ordenara as listas de tarefas recebidas e utilizar? a fun??o meet() do CrazyMiner/ID3.
		qsort(list1->vetor, list1->size, sizeof(int), compareTaskId);
		qsort(list2->vetor, list2->size, sizeof(int), compareTaskId);
		qsort(intersection->vetor, intersection->size, sizeof(int), &compareTaskId);
		
		list3 = taskIdListIntersection(list1, list2);
		assert(0 ==	taskIdListCompare(intersection, list3));
		
		taskIdListDestroy(list1);
		taskIdListDestroy(list2);
		taskIdListDestroy(list3);
		taskIdListDestroy(intersection);
		hashIntIntDestroy(hash);
	}

	return 0;
}
