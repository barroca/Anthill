#ifndef _STACK_H
#define _STACK_H

#include <dbLkList.h>

/** function pointers */
typedef struct Stack *StackP;
typedef void (StDestroyFunction)(StackP st, void *object);

/** this is the stack structure, its just a double linked list
 * we insert tail and remove tail.
 */
typedef struct _Stack_ {
	DbLkList *elements;
	StDestroyFunction *destroyFunction;
} Stack;
//typedef struct Stack *StackP;
//typedef void (StDestroyFunction)(StackP st, void *object);


typedef enum { ST_DM_KEEP=0, ST_DM_FREE } StDestroyMode;


/** creates an empty stack 
 * return the stack created
 */
Stack *stCreate();
/** destroys the stack, mode defines if objects will be destroyed too 
 * @param stackAddress the address of the stack, which will be freed and NULLed 
 * @param mode how we destroy the stack
 */
void stDestroy(Stack **stackAddress, StDestroyMode mode);

/** inserts an element to the stack */
int stPush(Stack *st, void *object);
/** fill object with the top of the stack, which is removed */
int stPop(Stack *st, void **objectAddress);
/** get stack top, but does not remove it */
int stGetTop(Stack *st, void **objectAddress);
/** returns the number of elements in the stack */
int stNumElements(Stack *st);


/** register functions */
int stRegisterDestroyFunction(Stack *st, StDestroyFunction *destroyFunction);

#endif

