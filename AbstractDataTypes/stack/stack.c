#include <stdlib.h>
#include <stdio.h>
#include "stack.h"

Stack *stCreate(){
	Stack *st = (Stack*)malloc(sizeof(Stack));

	st->elements = listCreate();
	st->destroyFunction = NULL;
	return st;
}

void stDestroy(Stack **stackAddress, StDestroyMode mode){
	Stack *st = stackAddress[0];
	if (mode == ST_DM_KEEP){
		listDestroy(&st->elements, DBLIST_DM_KEEP);
	}
	else {
		void *object;
		listPrepare(st->elements, DBLIST_TM_FROM_HEAD);
		while (listGetNext(st->elements, &object) != -1){
			if (st->destroyFunction == NULL){
				free(object);
			}
			else {
				st->destroyFunction((StackP)st, object);
			}
		}
	}
	free(st);
	stackAddress[0] = NULL;
}

int stPush(Stack *st, void *object){
	return listInsertTail(st->elements, object);
}

int stPop(Stack *st, void **objectAddress){
	return listRemoveTail(st->elements, objectAddress);
}

int stGetTop(Stack *st, void **objectAddress){
	return listGetTail(st->elements, objectAddress);
}

int stNumElements(Stack *st){
	return st->elements->numElements;
}

int stRegisterDestroyFunction(Stack *st, StDestroyFunction *destroyFunction){
	st->destroyFunction = destroyFunction;
	return 1;
}

