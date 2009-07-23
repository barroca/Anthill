#include <stdio.h>
#include <stdlib.h>
#include "TaskIdList.h"
#include "constants.h"


#ifdef TASK_ID_LIST_SEND
#define TO_NETWORK
#endif
#include "cser.h"


#ifndef TASK_ID_LIST_SEND

/**
 * \file vetor_doc.c Vetor de documentos com realoca??o exponencial. Sempre que 
 * n?o houver espa?o para um novo documento, o vetor ? realocado para ter o 
 * dobro do tamanho. Assim n?o corre o risco de copiar o vetor todo para outro
 * lugar a cada inser??o. Se o vetor crescer de 2^0 at? 2^n, ent?o ter?o sido 
 * feitas no m?ximo: uma copia de 1 posi??o, uma de 2, uma de 4, uma de 8,..., 
 * uma de 2^n-1 o que equivale a uma c?pia de (2^n)-1 posi??es (somat?rio de
 * 2^0 at? 2^n-1 = (2^n)-1 ). Sendo assim em m?dia cada posi??o do vetor ? 
 * copiada apenas 1 vez, ou seja a complexidade de inser??o ? O(1). 
 *
 * O desperd?cio de mem?ria dessa abordagem pode ser estimado da seguinte forma:
 * no pior caso, o vetor utiliza o dobro do espa?o necess?rio e no melhor n?o 
 * desperdi?a nada. Ent?o o desperd?cio na m?dia ? de 50%.
 */

//-----------------------------------------------------------------------------
/// Retorna a capacidade m?nima (de acordo com a pol?tica de 
/// realoca??o exponencial) necess?ria para armazenar um vetor de tamanho size.
//-----------------------------------------------------------------------------
int taskIdListGetMinCpacity(int size){
	int cap;

	cap=1;
	for (cap=1; cap<size; cap=cap<<1);
	return cap;
}

//-----------------------------------------------------------------------------
/// Cria um vetor de documentos.
//-----------------------------------------------------------------------------
TaskIdList *taskIdListCreate(int cap_inicial) {
	TaskIdList *vet = malloc(sizeof(TaskIdList));

	vet->size     = 0;
	vet->capacity = taskIdListGetMinCpacity(cap_inicial);
	vet->vetor    = malloc(sizeof(int)*vet->capacity);

	return vet;
}

//-----------------------------------------------------------------------------
/** Adiciona um documento ao vetor. Caso o vetor n?o tenha a capacidade
 * necess?ria, sua capacidade ser? dobrada (de acordo com a pol?tica de 
 * realoca??o exponencial).
 * 	\param vet Vetor onde o documento ser? acrescentado.
 * 	\param doc Documento a ser acrescentado.
 * 	\param freq Freq??ncia do documento.
 * 	\return Posi??o do documento no vetor.
 */ 
//-----------------------------------------------------------------------------
int taskIdListAdd(TaskIdList *vet, int doc) {
	int ret = 0;
	
	// Se vai estourar capacidade, aumenta vetor (aumento exponencial)
	if (vet->size+1 > vet->capacity) {
		vet->capacity = vet->capacity << 1;
		vet->vetor = realloc(vet->vetor, sizeof(int)*vet->capacity);
	}

	vet->vetor[vet->size]  = doc;

	ret = vet->size;
	vet->size++;

	return ret;
}

//-----------------------------------------------------------------------------
/// Retorna o tamanho de um vetor de documentos.
//-----------------------------------------------------------------------------
int taskIdListGetSize(TaskIdList *vet) {
	if (vet == NULL) {
		return 0;
	}
	return vet->size;
}

//-----------------------------------------------------------------------------
/// Retorna um ponteiro para a posi??o pos do vetor de documentos vet.
//-----------------------------------------------------------------------------
int taskIdListGet(TaskIdList *vet, int pos) {
  return ((vet->vetor[pos]) );
}

//-----------------------------------------------------------------------------
/// Retorna um ponteiro para a ?ltima posi??o do vetor de documentos vet.
//-----------------------------------------------------------------------------
int taskIdListGetLast(TaskIdList *vet) {
	if (vet->size > 0) {
	  	return ((vet->vetor[vet->size-1]) );
	} else {
		return 0;
	}
}

int *taskIdListToArray(TaskIdList *list, int *listSize) {
	int size = list->size;
	int *ret = NULL;
	
	if (size > 0) {
		ret = malloc(sizeof(int)*size);	
		memcpy(ret, list->vetor, sizeof(int)*size);
	}
	
	*listSize = size;
	return ret;
}

//void taskIdListSort(TaskIdList * vet, int (*compFunc)(const void *, const void *)) {
//	qsort(vet->vetor, vet->size, sizeof(int), compFunc);
//}


//-----------------------------------------------------------------------------
/// Compares two TaskIdList. Returns -1, 0 or +1 if the list "list1" is found,
/// respectivelly, to be less than, to match, or be greater than "list2". 
/// Comparison semantics is the same of strcmp().
//-----------------------------------------------------------------------------
int taskIdListCompare(TaskIdList *list1, TaskIdList *list2) {
	int i=0;
	
	if (list1->size < list2->size) {
		for (i=0; i<list1->size; i++) {
			if (list1->vetor[i] > list2->vetor[i]) {
				return 1;
			} else if (list1->vetor[i] < list2->vetor[i]) {
				return -1;
			}
		}
		// as list2 has elements that list1 hasn't, list 2 is bigger
		return -1;
	} else if (list1->size > list2->size) {
		for (i=0; i<list2->size; i++) {
			if (list1->vetor[i] > list2->vetor[i]) {
				return 1;
			} else if (list1->vetor[i] < list2->vetor[i]) {
				return -1;
			}
		}
		// as list1 has elements that list2 hasn't, list 1 is bigger
		return 1;
	} else { // same size
		for (i=0; i<list1->size; i++) {
			if (list1->vetor[i] > list2->vetor[i]) {
				return 1;
			} else if (list1->vetor[i] < list2->vetor[i]) {
				return -1;
			}
		}
		return 0;
	}
}


//-----------------------------------------------------------------------------
/// Destroy a taskId list
//-----------------------------------------------------------------------------
void taskIdListDestroy(TaskIdList *vet) {
	if (vet != NULL) {
		if (vet->vetor != NULL) {
			free(vet->vetor);
		}
		free(vet);
	}
}


TaskIdList *taskIdListCopy(TaskIdList* list){
	if (list == NULL) {
		return NULL;
	}
	
	int i, value, size = taskIdListGetSize(list);
	TaskIdList *auxList = taskIdListCreate(size);
	
	for(i = 0; i < size; i++){
		value = taskIdListGet(list, i);
		taskIdListAdd(auxList, value);
	}
	return auxList;
}


/// This function assume that 
TaskIdList *taskIdListIntersection(TaskIdList *a, TaskIdList *b) {
	int i,j, resultSize=0;

	if(a->size < b->size) {
		resultSize = a->size;
	} else {
		resultSize = b->size;
	}
	TaskIdList *result = (TaskIdList *)taskIdListCreate(resultSize);
	
	for(i=0,j=0; i < a->size && j < b->size;) {
		if(a->vetor[i] > b->vetor[j]) {
			j++;
		} else if(a->vetor[i]==b->vetor[j]) {
			taskIdListAdd(result, a->vetor[i]);
			j++;
			i++;
		} else {
			i++;
		}
	}
	
	// Reduce the capacity of the list only to the needed
	if(result->size > 0) {
		result->vetor=(int*)realloc(result->vetor,sizeof(int)*result->size);
		result->capacity = result->size;
	} else {
		free(result->vetor);
		result->vetor = NULL;
		result->capacity = 0;
	}

	return result;
}

/// Comparison function for sorting task ids in ascendig order
static int compareTaskIds(const void *a, const void *b) {
	int* i=(int*)a;
	int* j=(int*)b;

	if (*i > *j) return(1);
	if (*i < *j) return(-1);
	return 0;
}

/// Sort the list ids in ascending order
void taskIdListSortAscendig(TaskIdList *list) {
	if (list->size > 1) {
		qsort(list->vetor, list->size, sizeof(int), &compareTaskIds);
	}
}


#endif


////////////// Funcs to write the TaskIdList to File/////////////////
int WRITE_TIDL(FILE *outputFile, void *list){
	int i, taskId, listSize = taskIdListGetSize((TaskIdList *)list);

	WRITE_NUM(outputFile, "listSize", listSize);
	
	for(i = 0; i < listSize; i++){
		taskId = taskIdListGet((TaskIdList *)list, i);
		
		WRITE_NUM(outputFile, "taskId", taskId);
	}
	
	return 1;
}

void *READ_TIDL(FILE *inputFile){
	int i, taskId, listSize = -1;
	TaskIdList *list = NULL;

	READ_BEGIN(inputFile);
		
	// Read data size from disk
	READ_NUM("listSize", listSize);
	if(listSize == -1)fprintf(stderr,"Error: Reading task Id List Size\n");

	list = taskIdListCreate(listSize); 
		
	for(i = 0; i< listSize; i++){
		taskId = -2;
		READ_NUM("taskId", taskId);
		if(taskId == -1){
			fprintf(stderr,"Error: Reading task Id List\n");
			return NULL;
		}
		taskIdListAdd(list, taskId);


	}
	READ_END
	return ((void *) list);
}

