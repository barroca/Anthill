#ifndef _LIST_H
#define _LIST_H

/** Implements a double linked list. I changed the name from list to DbLkList because of some naming conflicts with 
  * C++ apps, which defines the List name.
  */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

/** 
  * this types indicates if we shall free contents
  */
typedef enum { DBLIST_DM_KEEP=0, DBLIST_DM_FREE } DbLkDestroyMode;

/**
  * the way we shall transverse the list 
  */
typedef enum { DBLIST_TM_FROM_HEAD, DBLIST_TM_FROM_TAIL } DbLkTransMode;

typedef struct _DbLkList *ListP;

// function prototypes
typedef void (ListVisitFunction)(ListP, void *object);
typedef void (ListPrintFunction)(ListP, void *object);
typedef void (ListDeleteFunction)(ListP, void *object);

///each node in the list contains this struct
typedef struct _DbLkListNodeContent {
	void *content; ///the content
} DbLkListNodeContent;

///a node in the list, has the content above and pointers to next and previous
typedef struct _DbLkListNode {
	struct _DbLkListNode *previous, *next; ///pointers to the previous(ie before me) and next(after me) nodes
	DbLkListNodeContent nodeContent; ///the contents of this node
} DbLkListNode;

///the list structure, holds pointer to tail and head, the number of elements, and callback functions
typedef struct _DbLkList {
	int numElements; //< number of elements in the list
	DbLkListNode head, tail; //< pointers to the head and tail of the list, both are not true nodes

	//internal variables
	DbLkListNode *lastVisited; 	//< last visited node(for iteration)
	
	//registered callback functions
	ListPrintFunction *printFunction; //< this will be called while printing the list
	ListDeleteFunction *deleteFunction; //< this will be called when deleting the list
	ListVisitFunction *visitFunction; //< this will get called by the listVisit function
} DbLkList;

/* -------------------functions -------------------------------------------*/
/**
All functios follow the same return patter: NULL or -1 if error found, or the object.
Exception are for boolean returns, which 0 means false and 1 means true
*/

/** Initializes the empty list.
 @return the empty list, just created
 */
DbLkList *listCreate();

/** Destroys the list. If theres a registered free function, it will be called for each node
 * @param list the address of the list pointer being destroyed. List pointer will be NULL after this
 * @param mode should the content be destroyed? If so, user function will get called, otherwise, a simple free is called on content
 */
void listDestroy(DbLkList **list, DbLkDestroyMode mode);

/** prepares the list for itaration. Should be called before getNext or getPrevious
  * @param list the list
  * @param from tail or head?
  * @return -1 if list is empty, 1 otherwise
  */
int listPrepare(DbLkList *list, DbLkTransMode from);

/** get next node 
 * @return -1 when pointer is at list last valid element, 1 otherwise
 */
int listGetNext(DbLkList *list, void **object);

/** get previous node
  * @return -1 when pointer is at list first valid element, 1 otherwise
  */
int listGetPrevious(DbLkList *list, void **object);


/** Inserts an object in the list, as the new head node. 
 * @param list the list being inserted
 * @param object the object being inserted
 * @return -1 on error, 1 otherwise
 */
int listInsertHead(DbLkList *list, void *object);

/** Inserts an object at the end of the list, as the new tail. 
 * @param list the list being inserted
 * @param object the object being inserted
 * @return -1 on error, 1 otherwise
 */
int listInsertTail(DbLkList *list, void *object);

/** Inserts the object at the given position. If position does not exist and is less than 0, it will be a head insertion.
 * Else is a tail insertion.
 * @param list the list being inserted
 * @param position where the object will be inserted
 * @param object the object being inserted
 * @return -1 on error, 1 otherwise
*/
int listInsertAt(DbLkList *list, int position, void *object);

/** Retrieves the object at the given position, but does not remove it from list.
 * @param list the list being searched
 * @param position the position we are getting. 0 is the list head(first element).
 * @param objectPointerAddress the address of an object pointer, so we can return a pointer to the object
 * @return 1 if successful, -1 otherwise(ie, position does not exist)
*/
int listGetObjectAt(DbLkList *list, int position, void **objectPointerAddress);

/** Retrieves the object at the given position, removing it from list.
 * @param list the list being searched
 * @param position the position we are getting. 0 is the list head(first element).
 * @param objectPointerAddress the address of an object pointer, so we can return a pointer to the object
 * @return 1 if successful, -1 otherwise(ie, position does not exist)
*/
int listRemoveObjectAt(DbLkList *list, int position, void **objectPointerAddress);

/** Removes the given object from the list 
 * @param list the list
 * @param object the object being removed
 * @return 1, -1 if object cant be found, 
 */
int listRemoveObject(DbLkList *list, void *object);

/** Concatenates 2 list. The source list is freed, the destination will hold all elements from the other.
 * Source list is concatenated at the end of the destination
 * @param destination the destination list, which will hold all info from src
 * @param sourceAddress the address of the src list pointer, which will be NULL after this
 * @return the number of elements of the destination list after concatenation, -1 if an error occurs
 */
int listCat(DbLkList *destination, DbLkList **sourceAddress);

/** Retrieves the first element in the list, ie, the head. Does not remove from the list
 * @param list the list being searched
 * @param objectAddress the address of an object pointer, so we can return a pointer to the object
 * @return 1 if successful, -1 otherwise(empty list)
*/
int listGetHead(DbLkList *list, void **objectAddress);

/** Retrieves the first element in the list, ie, the head. Does not remove from the list
 * @param list the list being searched
 * @param objectAddress the address of an object pointer, so we can return a pointer to the object
 * @return 1 if successful, -1 otherwise(empty list)
*/
int listGetTail(DbLkList *list, void **objectAddress);

/** Removes the list head, returning it at the object pointer argument
 * @return 1 if successful, -1 otherwise(list empty)
*/
int listRemoveHead(DbLkList *list, void **object);

/** Removes the list tail, returning it at the object pointer argument
 * @return 1 if successful, -1 otherwise(list empty)
*/
int listRemoveTail(DbLkList *list, void **object);

/** Registers a function to be called by the print function, in each list node.
 * If this function is not registered, calls to listPrint will return immediately.
 * @param list the list
 * @param printFunction a function, of prototype ListPrintFunction
 */
void listRegisterPrintFunction(DbLkList *list, ListPrintFunction *printFunction);

/** Registers a function to be called by the visit function, in each list node.
 * If this function is not registered, calls to listVisit will return immediately.
 * @param list the list
 * @param visitFunction a function, of prototype ListVisitFunction
 */
void listRegisterPrintFunction(DbLkList *list, ListVisitFunction *visitFunction);

/** Prints the list, from head to tail.
 * @param list the lsit being printed
 */
void listPrint(DbLkList* list);

/** Visits the list, from head to tail, calling the registered function. If none, return.
 * @param list the list being printed
 */
void listVisit(DbLkList* list);

/** Registers a function to be called by the destroy function, in each list node.
 * @param list the list
 * @param deleteFunction a function, of prototype ListPrintFunction
 */
void listRegisterDeleteFunction(DbLkList *list, ListDeleteFunction *deleteFunction);


#ifdef __cplusplus
}
#endif

#endif


