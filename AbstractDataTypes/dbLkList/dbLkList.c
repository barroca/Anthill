#include "dbLkList.h"

/** static functions, used only in this file */

/** Returns the node which holds the object passed as argument.
 * @param lsit the list being searched
 * @param object we are looking for the node holding this
 * @return the node holding the object, NULL otherwise
 */
static DbLkListNode *gotoNodeWith(DbLkList*list, void *object){
	DbLkListNode *node = list->head.next;
	int i;

	for (i=0;i<list->numElements;i++){
		if (node->nodeContent.content == object){
			return node;	
		}
		node= node->next;
	}

	return NULL;

}


/** Returns a pointer to the node at the given position 
 * @param list the list
 * @param position the position we are looking for
 * @return the node at the given position, NULL otherwise
 */
static DbLkListNode *gotoNodeAt(DbLkList*list, int position){
	if ( (position < 0) || (position >= list->numElements) ){
		//invalid position
		return NULL;
	}

	int middle = list->numElements / 2;
	if (position <= middle){
		DbLkListNode *node = list->head.next;
		int i;
		//get the right spot to insert
		for (i=0;i<position;i++){
			node = node->next;
		}
		return node;
	}
	else {
		DbLkListNode *node = list->tail.previous;
		int i;
		for (i=(list->numElements-1);i>position;i--){
			node = node->previous;
		}
		return node;
	}
}

/**
 * Inserts an object after the given node, used internally
 * @param node we insert the object in a new node after this
 * @param object the object being inserted
 * @return 1 on success, -1 otherwise
 */
static int insertAfter(DbLkList*list, DbLkListNode *node, void *object){
	//now we have the right node
	DbLkListNode *aux = node->next, *newNode;

	//pointer adjust
	node->next = (DbLkListNode*)malloc(sizeof(DbLkListNode));
	newNode = node->next;
	newNode->previous = node;
	newNode->next = aux;
	aux->previous = newNode;
	
	//place the object
	newNode->nodeContent.content = object;

	//finally...
	list->numElements++;

	return 1;
}

// end static functions ----------------------------------

/** constructor and destructor */

DbLkList* listCreate(){
	DbLkList*list = (DbLkList*)malloc(sizeof(DbLkList));
	list->numElements = 0;
	list->head.previous = NULL;
	list->head.next = &list->tail;
	list->tail.previous = &list->head;
	list->tail.next = NULL;

	list->deleteFunction = NULL;
	list->printFunction = NULL;
	list->visitFunction = NULL;
	
	list->lastVisited = NULL;
	
	return list;
}

void listDestroy(DbLkList**listPointerAddress, DbLkDestroyMode mode){
	DbLkList*list = listPointerAddress[0];	
	while (list->numElements != 0){
		void *object;

		listRemoveObjectAt(list, 0, &object);	
		if (mode == DBLIST_DM_FREE){
			if (list->deleteFunction != NULL){
				list->deleteFunction(list, object);
			}
			else {
				if (object != NULL){
					free(object);
				}
			}
		}
	}

	free(list);
	listPointerAddress[0] = NULL;
}


/** iteration functions */

int listPrepare(DbLkList *list, DbLkTransMode from){
	if (list->numElements <= 0){
		return -1;
	}

	list->lastVisited = (from == DBLIST_TM_FROM_HEAD)? &list->head : &list->tail;
	return 1;
}

int listGetNext(DbLkList *list, void **object){
	list->lastVisited = ((list->lastVisited == NULL) ||  (list->lastVisited->next == &list->tail) ) ? 
			NULL : list->lastVisited->next;
	if (list->lastVisited == NULL){
		return -1;
	}
	object[0] = list->lastVisited->nodeContent.content;
	
	return 1;
}

int listGetPrevious(DbLkList *list, void **object){
	list->lastVisited = ((list->lastVisited == NULL) || (list->lastVisited->previous == &list->head)) ? 
			NULL : list->lastVisited->previous;
	if (list->lastVisited == NULL){
		return -1;
	}

	object[0] = list->lastVisited->nodeContent.content;
	
	return 1;

}


/** 
 * Insertion functions
 */


int listInsertHead(DbLkList*list, void *object){	
	DbLkListNode *node = &list->head;
	return insertAfter(list, node, object);
}
int listInsertTail(DbLkList*list, void *object){
	DbLkListNode *node = &list->tail;
	return insertAfter(list, node->previous, object);
}

int listInsertAt(DbLkList*list, int position, void *object){
	if ((position < 0) || (position > list->numElements)){
		//invalid position
		return -1;
	}
	else if (position == 0){
		return listInsertHead(list, object);
	}
	else if (position == list->numElements){
		return listInsertTail(list, object);
	}
	else {
		DbLkListNode *node = gotoNodeAt(list, position);
		if (node == NULL){
			return -1;
		}
		node = node->previous;
		return insertAfter(list, node, object);
	}
}

/** Retrieving functions */

int listGetObjectAt(DbLkList*list, int position, void **objectAddress){
	DbLkListNode *node = gotoNodeAt(list, position);
	if (node == NULL){
		return -1;
	}
	//we got the node, return the values
	(*objectAddress) = node->nodeContent.content;
	return 1;
}


/** Remove functions */

/** Removes a node from the list, does not check if its valid or not, so careful here
 * @param list the list
 * @param node the node being removed
 */
static void removeNode(DbLkList*list, DbLkListNode *node){
	node->next->previous = node->previous;
	node->previous->next = node->next;
	free(node);
	list->numElements--;
}

int listRemoveObjectAt(DbLkList*list, int position, void **objectAddress){
	DbLkListNode *node = gotoNodeAt(list, position);
	if (node == NULL){
		return -1;
	}

	if (objectAddress != NULL){
		objectAddress[0] = node->nodeContent.content;
	}
	removeNode(list, node);	
	return 1;
}

int listRemoveObject(DbLkList*list, void *object){
	DbLkListNode *node = gotoNodeWith(list, object);
	if (node == NULL){
		return -1;
	}
	removeNode(list, node);
	return 1;
}

int listRemoveHead(DbLkList *list, void **objectAddress){
	if (list->numElements == 0){
		return -1;
	}
	objectAddress[0] = list->head.next->nodeContent.content;
	removeNode(list, list->head.next);
	return 1;
}

int listRemoveTail(DbLkList *list, void **objectAddress){
	if (list->numElements == 0){
		return -1;
	}
	objectAddress[0] = list->tail.previous->nodeContent.content;
	removeNode(list, list->tail.previous);
	return 1;
}

int listGetTail(DbLkList *list, void **objectAddress){
	if (list->numElements == 0){
		return -1;
	}
	objectAddress[0] = list->tail.previous->nodeContent.content;
	return 1;
}

int listGetHead(DbLkList *list, void **objectAddress){
	if (list->numElements == 0){
		return -1;
	}
	objectAddress[0] = list->head.next->nodeContent.content;
	return 1;
}




/** Concatenate functions */

int listCat(DbLkList*dest, DbLkList**sourceAddress){
	DbLkList*source = sourceAddress[0];

	//if dest has 0 elements it works... if source has 0, the if catches it.
	if (source->numElements != 0){
		DbLkListNode *lastDest = dest->tail.previous;
		DbLkListNode *firstSource = source->head.next;
		DbLkListNode *lastSource = source->tail.previous;

		lastDest->next = firstSource;
		firstSource->previous = lastDest;
		lastSource->next = &dest->tail;
		dest->tail.previous = lastSource;

		dest->numElements += source->numElements;
	}
	free(source);
	sourceAddress[0] = NULL;

	return dest->numElements;
}


/** visit function */
void listVisit(DbLkList* list){
	if (list->visitFunction == NULL){
		//return if no registered visit function
		return;
	}
	int i;
	DbLkListNode *node = list->head.next;
	for (i=0;i<list->numElements;i++){
		list->visitFunction(list, node->nodeContent.content);
		node = node->next;
	}

}

/** Print functions */


void listPrint(DbLkList*list){
	if (list->printFunction == NULL){
		//we cant print if we dont have a print function	
		return;
	}
	int i;
	DbLkListNode *node = list->head.next;
	for (i=0;i<list->numElements;i++){
		list->printFunction(list, node->nodeContent.content);
		node = node->next;
	}
}

/** Register functions */

void listRegisterPrintFunction(DbLkList*list, ListPrintFunction *printFunction){
	list->printFunction = printFunction;
}

void listRegisterDeleteFunction(DbLkList*list, ListDeleteFunction *deleteFunction){
	list->deleteFunction = deleteFunction;
}

void listRegisterVisitFunction(DbLkList *list, ListVisitFunction *visitFunction){
	list->visitFunction = visitFunction;
}

